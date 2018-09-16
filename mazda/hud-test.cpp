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
#include <iostream>


#include <dbus-c++/dbus.h>
#include <dbus-c++/glib-integration.h>
#include <sys/time.h>

#include "hud/hud.h"

#include <stdint.h>
#include <string>

static void hud_test_func(std::condition_variable& quitcv, std::mutex& quitmutex)
{
  printf("Connecting to DBUS\n");
  hud_start();
  printf("hud installed %d\n", hud_installed());

  uint32_t i = 1;
  uint8_t msg = 1;
  std::string test_string;
  while(true)
  {
    char buff[100];
    snprintf(buff, sizeof(buff), "DIRICON %u", i);
    test_string = buff;

    hud_send(i, 1000, test_string, msg);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    i++;
    if(msg <= 8){
      msg++;
    }
    else
    {
      msg = 1;
    }
    {
        std::unique_lock<std::mutex> lk(quitmutex);
        if (quitcv.wait_for(lk, std::chrono::milliseconds(1000)) == std::cv_status::no_timeout)
        {
            break;
        }
    }
  }

}

GMainContext* run_on_thread_main_context = nullptr;

int main (int argc, char *argv[])
{
  DBus::_init_threading();
  std::condition_variable quitcv;
  std::mutex quitmutex;
  std::thread hud_thread([&quitcv, &quitmutex](){ hud_test_func(quitcv, quitmutex); } );
  while(true){
    //Make a new one instead of using the default so we can clean it up each run
    run_on_thread_main_context = g_main_context_new();
    //Recreate this each time, it makes the error handling logic simpler
    DBus::Glib::BusDispatcher dispatcher;
    dispatcher.attach(run_on_thread_main_context);
    printf("DBus::Glib::BusDispatcher attached\n");

    DBus::default_dispatcher = &dispatcher;

    quitcv.notify_all();

   hud_thread.join();
   g_main_context_unref(run_on_thread_main_context);
   run_on_thread_main_context = nullptr;

  }
}
