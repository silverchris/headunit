//
// Created by silverchris on 2020-11-06.
//

#include "wireless.h"

#include <dbus/dbus.h>
#include <dbus-c++/dbus.h>
#include <stdint.h>
#include <string>
#include <cstring>
#include <functional>
#include <condition_variable>
#include <algorithm>
#include <signal.h>
#include <thread>
#include <unistd.h>
#include <fcntl.h>


#include "hu.pb.h"

#define LOGTAG "mazda-bt"

#include "hu_uti.h"

#define SERVICE_BUS_ADDRESS "unix:path=/tmp/dbus_service_socket"


void sendMessage(int fd, google::protobuf::MessageLite &message, uint16_t type) {
    int byteSize = message.ByteSize();
    uint16_t sizeOut = htobe16(byteSize);
    uint16_t typeOut = htobe16(type);
    char *out = (char *) malloc(byteSize + 4);
    memcpy(out, &sizeOut, 2);
    memcpy(out + 2, &typeOut, 2);

    message.SerializeToArray(out + 4, byteSize);

    auto written = write(fd, out, byteSize + 4);
    if (written > -1) {
        logd("Bytes written: %u\n", written);
    } else {
        logd("Could not write data\n");
    }
    free(out);
}

void handleWifiInfoRequest(int fd, uint8_t *buffer, uint16_t length) {
    HU::WifiInfoRequest msg;
    msg.ParseFromArray(buffer, length);
    logd("WifiInfoRequest: %s\n", msg.DebugString().c_str());

    HU::WifiInfoResponse response;
    response.set_ip_address("192.168.53.1");
    response.set_port(5000);
    response.set_status(HU::WifiInfoResponse_Status_STATUS_SUCCESS);

    sendMessage(fd, response, 7);
}

void handleWifiSecurityRequest(int fd, uint8_t *buffer, uint16_t length) {
    HU::WifiSecurityReponse response;

    response.set_ssid("MazdaCMU");
    response.set_bssid("0c:d9:c1:8f:7f:bb");
    response.set_key("password");
    response.set_security_mode(HU::WifiSecurityReponse_SecurityMode_WPA2_PERSONAL);
    response.set_access_point_type(HU::WifiSecurityReponse_AccessPointType_DYNAMIC);

    sendMessage(fd, response, 3);
}

int handleWifiInfoRequestResponse(int fd, uint8_t *buffer, uint16_t length) {
    HU::WifiInfoResponse msg;
    msg.ParseFromArray(buffer, length);
    logd("WifiInfoResponse: %s\n", msg.DebugString().c_str());
    return msg.get_status();
}

void BDSClient::SignalConnected_cb(const uint32_t &type, const ::DBus::Struct <std::vector<uint8_t>> &data) {
    char mac[18];
    char pty[100];
    char buf[100];
    logd("Signal Connected:\n");
    logd("\tType: %u\n", type);
    std::copy(data._1.begin() + 14, data._1.begin() + 32, mac);
    mac[18] = '\0';
    logd("\tMAC: %s\n", mac);
    logd("\tService ID: %u\n", data._1[36]);
    if (data._1[36] == 15) {
        std::strncpy(pty, (char *) &data._1[48], 100);
        logd("\tPTY: %s\n", pty);
        int fd = open(pty, O_RDWR | O_NOCTTY | O_SYNC);
        HU::WifiInfoRequest request;
        request.set_ip_address("192.168.53.1");
        request.set_port(30515);

        sendMessage(fd, request, 1);

        logd("PTY opened\n");
        ssize_t len = 0;
        int loop = 1;
        while (loop) {
            ssize_t i = read(fd, buf, 4);
            len += i;
            if (len >= 4) {
                uint16_t size = be16toh(*(uint16_t *) buf);
                uint16_t type = be16toh(*(uint16_t * )(buf + 2));
                logd("Size: %u, MessageID: %u, left: %u\n", size, type);
                if (len >= size + 4) {
                    uint8_t *buffer = (uint8_t *) malloc(size);
                    i = 0;
                    while (i < size) {
                        i += read(fd, buffer, size);
                    }
                    switch (type) {
                        case 1:
                            handleWifiInfoRequest(fd, buffer, size);
                            break;
                        case 2:
                            handleWifiSecurityRequest(fd, buffer, size);
                            break;
                        case 7:
                            if(handleWifiInfoRequestResponse(fd, buffer, size) == 0){
                                loop = 0;
                            }
                            break;
                        default:
                            break;
                    }
                }
            }
        }
    }
}

void wireless_thread() {
    static BDSClient *bds_client = NULL;
    DBus::BusDispatcher dispatcher;
    DBus::default_dispatcher = &dispatcher;

    logd("DBus::Glib::BusDispatcher attached\n");

    try {
        DBus::Connection service_bus(SERVICE_BUS_ADDRESS, false);
        service_bus.register_bus();
        bds_client = new BDSClient(service_bus, "/com/jci/bds", "com.jci.bds");
        dispatcher.enter();
    }
    catch (DBus::Error &error) {
        loge("DBUS: Failed to connect to SERVICE bus %s: %s\n", error.name(), error.message());
    }
}
