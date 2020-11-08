#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <dbus/dbus.h>
#include <poll.h>
#include <inttypes.h>
#include <cmath>
#include <functional>
#include <condition_variable>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <thread>

#include <dbus-c++/dbus.h>
#include <dbus-c++/glib-integration.h>
#include <sys/time.h>
#include <sys/stat.h>

#include "hu_uti.h"
#include "hu_aap.h"

#include "gps/mzd_gps.h"
#include "hud/hud.h"
#include "wireless/wireless.h"

#include "audio.h"
#include "main.h"
#include "command_server.h"
#include "callbacks.h"
#include "glib_utils.h"
#include "config.h"

#define HMI_BUS_ADDRESS "unix:path=/tmp/dbus_hmi_socket"
#define SERVICE_BUS_ADDRESS "unix:path=/tmp/dbus_service_socket"
// Check the content folder. sd_nav still exists without the card installed
#define SD_CARD_PATH "/tmp/mnt/sd_nav/content"

__asm__(".symver realpath1,realpath1@GLIBC_2.11.1");

gst_app_t gst_app;
IHUAnyThreadInterface* g_hu = nullptr;

static void nightmode_thread_func(std::condition_variable &quitcv, std::mutex &quitmutex) {
    char gpio_value[3];
    int fd = open("/sys/class/gpio/CAN_Day_Mode/value", O_RDONLY);
    if (-1 == fd) {
        loge("Failed to open CAN_Day_Mode gpio value for reading");
    } else {
        //Offset so the GPS and NM thread are not perfectly in sync testing each second
        std::this_thread::sleep_for(std::chrono::milliseconds(500));

        while (true) {
            if (-1 == read(fd, gpio_value, 3)) {
                loge("Failed to read CAN_Day_Mode gpio value");
            }
            int nightmodenow = !atoi(gpio_value);

            if(nightmodenow){
                logd("It's night now");
            }
            else{
                logd("It's day now");
            }

            // We send nightmode status periodically, otherwise Google Maps
            // doesn't switch to nightmode if it's started late. Even if the
            // other AA UI is already in nightmode.
            g_hu->hu_queue_command([nightmodenow](IHUConnectionThreadInterface &s) {
                HU::SensorEvent sensorEvent;
                sensorEvent.add_night_mode()->set_is_night(nightmodenow);

                s.hu_aap_enc_send_message(0, AA_CH_SEN, HU_SENSOR_CHANNEL_MESSAGE::SensorEvent, sensorEvent);
            });

            {
                std::unique_lock <std::mutex> lk(quitmutex);
                if (quitcv.wait_for(lk, std::chrono::milliseconds(1000)) == std::cv_status::no_timeout) {
                    close(fd);
                    break;
                }
            }
        }
    }
}

static void gps_thread_func(std::condition_variable& quitcv, std::mutex& quitmutex)
{
    GPSData data, newData;
    uint64_t oldTs = 0;
    int debugLogCount = 0;
    mzd_gps2_start();

    //Not sure if this is actually required but the built-in Nav code on CMU does it
    mzd_gps2_set_enabled(true);

    config::readConfig();
    while (true)
    {
        if (config::carGPS && mzd_gps2_get(newData) && !data.IsSame(newData))
        {
            data = newData;
            timeval tv;
            gettimeofday(&tv, nullptr);
            uint64_t timestamp = tv.tv_sec * 1000000 + tv.tv_usec;
            if (debugLogCount < 50) //only print the first 50 to avoid spamming the log and breaking the opera text box
            {
                logd("GPS data: %d %d %f %f %d %f %f %f %f   \n",data.positionAccuracy, data.uTCtime, data.latitude, data.longitude, data.altitude, data.heading, data.velocity, data.horizontalAccuracy, data.verticalAccuracy);
                logd("Delta %f\n", (timestamp - oldTs)/1000000.0);
                debugLogCount++;
            }
            oldTs = timestamp;

            g_hu->hu_queue_command([data, timestamp](IHUConnectionThreadInterface& s)
            {
                HU::SensorEvent sensorEvent;
                HU::SensorEvent::LocationData* location = sensorEvent.add_location_data();
                //AA uses uS and the gps data just has seconds, just use the current time to get more precision so AA can
                //interpolate better
                location->set_timestamp(timestamp);
                location->set_latitude(static_cast<int32_t>(data.latitude * 1E7));
                location->set_longitude(static_cast<int32_t>(data.longitude * 1E7));

                // If the sd card exists then reverse heading. This should only be used on installs that have the
                // reversed heading issue.
                double newHeading = data.heading;

                if (config::reverseGPS)
                {
                    const char* sdCardFolder;
                    sdCardFolder = SD_CARD_PATH;
                    struct stat sb;

                    if (stat(sdCardFolder, &sb) == 0 && S_ISDIR(sb.st_mode))
                    {
                        newHeading = data.heading + 180;
                        if (newHeading >= 360)
                        {
                            newHeading = newHeading - 360;
                        }
                    }
                }

                location->set_bearing(static_cast<int32_t>(newHeading * 1E6));
                //assuming these are the same units as the Android Location API (the rest are)
                double velocityMetersPerSecond = data.velocity * 0.277778; //convert km/h to m/s
                location->set_speed(static_cast<int32_t>(velocityMetersPerSecond * 1E3));

                location->set_altitude(static_cast<int32_t>(data.altitude * 1E2));
                location->set_accuracy(static_cast<int32_t>(data.horizontalAccuracy * 1E3));

                s.hu_aap_enc_send_message(0, AA_CH_SEN, HU_SENSOR_CHANNEL_MESSAGE::SensorEvent, sensorEvent);
            });
        }

        {
            std::unique_lock<std::mutex> lk(quitmutex);
            //The timestamps on the GPS events are in seconds, but based on logging the data actually changes faster with the same timestamp
            if (quitcv.wait_for(lk, std::chrono::milliseconds(250)) == std::cv_status::no_timeout)
            {
                break;
            }
        }
    }

    mzd_gps2_set_enabled(false);

    mzd_gps2_stop();
}

class BLMSystemClient : public com::jci::blmsystem::Interface_proxy, public DBus::ObjectProxy {
public:
    BLMSystemClient(DBus::Connection &connection, const char *path, const char *name) : DBus::ObjectProxy(connection, path, name) {}

    virtual void NotifyStateTransition(const uint32_t &current_state, const uint32_t &target_state) override {}

    virtual void NotifyShutdown(const uint32_t &generic_reboot_flags) override {}

    virtual void NotifyAccChange(const int32_t &acc_state, const uint32_t &system_state) override {}

    virtual void NotifySystemStateChange(const uint32_t &old_state, const uint32_t &current_state) {
        if(current_state >= 4){
            logd("Got Shutdown Signal\n");
            g_main_loop_quit(gst_app.loop);
        }
    }
};

int main (int argc, char *argv[])
{
    //Force line-only buffering so we can see the output during hangs
    setvbuf(stdout, NULL, _IOLBF, 0);
    setvbuf(stderr, NULL, _IOLBF, 0);

    GOOGLE_PROTOBUF_VERIFY_VERSION;

    hu_log_library_versions();
    hu_install_crash_handler();

    DBus::_init_threading();

    gst_init(&argc, &argv);

    try
    {
        MazdaCommandServerCallbacks commandCallbacks;
        CommandServer commandServer(commandCallbacks);
        printf("headunit version: %s \n", commandCallbacks.GetVersion().c_str());
        if (!commandServer.Start())
        {
            loge("Command server failed to start");
            return 1;
        }

        if (argc >= 2 && strcmp(argv[1], "test") == 0)
        {
            //test mode from the installer, if we got here it's ok
            printf("###TESTMODE_OK###\n");
            return 0;
        }

        config::readConfig();
        printf("Looping\n");
        while (true)
        {
            //Make a new one instead of using the default so we can clean it up each run
            run_on_thread_main_context = g_main_context_new();
            //Recreate this each time, it makes the error handling logic simpler
            DBus::Glib::BusDispatcher dispatcher;
            dispatcher.attach(run_on_thread_main_context);
            printf("DBus::Glib::BusDispatcher attached\n");

            DBus::default_dispatcher = &dispatcher;

            printf("Making debug connections\n");
            DBus::Connection hmiBus(HMI_BUS_ADDRESS, false);
            hmiBus.register_bus();

            DBus::Connection serviceBus(SERVICE_BUS_ADDRESS, false);
            serviceBus.register_bus();

            std::thread wireless_handle(wireless_thread);
            static BLMSystemClient *blmsystem_client = new BLMSystemClient(serviceBus, "/com/jci/blm/system", "com.jci.blmsystem.Interface");

            hud_start();

            MazdaEventCallbacks callbacks(serviceBus, hmiBus);
            HUServer headunit(callbacks);
            g_hu = &headunit.GetAnyThreadInterface();
            commandCallbacks.eventCallbacks = &callbacks;

            //Wait forever for a connection
            int ret = headunit.hu_aap_start(config::transport_type, config::phoneIpAddress, true);
            if (ret < 0) {
                loge("Something bad happened");
                continue;
            }

            gst_app.loop = g_main_loop_new(run_on_thread_main_context, FALSE);
            callbacks.connected = true;

            std::condition_variable quitcv;
            std::mutex quitmutex;
            std::mutex hudmutex;

            std::thread nm_thread([&quitcv, &quitmutex](){ nightmode_thread_func(quitcv, quitmutex); } );
            std::thread gp_thread([&quitcv, &quitmutex](){ gps_thread_func(quitcv, quitmutex); } );
            std::thread hud_thread([&quitcv, &quitmutex, &hudmutex](){ hud_thread_func(quitcv, quitmutex, hudmutex); } );

            /* Start gstreamer pipeline and main loop */

            printf("Starting Android Auto...\n");

            g_main_loop_run (gst_app.loop);

            commandCallbacks.eventCallbacks = nullptr;

            callbacks.connected = false;
            callbacks.videoFocus = false;
            callbacks.audioFocus = AudioManagerClient::FocusType::NONE;
            callbacks.inCall = false;

            printf("quitting...\n");
            //wake up night mode  and gps polling threads
            quitcv.notify_all();


            printf("waiting for nm_thread\n");
            nm_thread.join();

            printf("waiting for gps_thread\n");
            gp_thread.join();

            printf("waiting for hud_thread\n");
            hud_thread.join();

            printf("shutting down\n");

            g_main_loop_unref(gst_app.loop);
            gst_app.loop = nullptr;

            /* Stop AA processing */
            ret = headunit.hu_aap_shutdown();
            if (ret < 0) {
                printf("hu_aap_shutdown() ret: %d\n", ret);
                return ret;
            }

            g_main_context_unref(run_on_thread_main_context);
            run_on_thread_main_context = nullptr;
            g_hu = nullptr;
            DBus::default_dispatcher = nullptr;
        }
    }
    catch(DBus::Error& error)
    {
        loge("DBUS Error: %s: %s", error.name(), error.message());
        return 1;
    }

    return 0;
}
