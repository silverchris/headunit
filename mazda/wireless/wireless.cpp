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
#include <fstream>

#include "hu.pb.h"

#define LOGTAG "mazda-bt"

#include "hu_uti.h"

#define HMI_BUS_ADDRESS "unix:path=/tmp/dbus_hmi_socket"
#define SERVICE_BUS_ADDRESS "unix:path=/tmp/dbus_service_socket"

std::string IP_ADDRESS;
std::string MAC_ADDRESS;


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
    response.set_ip_address(IP_ADDRESS.c_str());
    response.set_port(30515);
    response.set_status(HU::WifiInfoResponse_Status_STATUS_SUCCESS);

    sendMessage(fd, response, 7);
}

void handleWifiSecurityRequest(int fd, uint8_t *buffer, uint16_t length) {
    HU::WifiSecurityReponse response;

    response.set_ssid(hostapd_config("ssid").c_str());
    response.set_bssid(MAC_ADDRESS.c_str());
    response.set_key(hostapd_config("wpa_passphrase").c_str());
    response.set_security_mode(HU::WifiSecurityReponse_SecurityMode_WPA2_PERSONAL);
    response.set_access_point_type(HU::WifiSecurityReponse_AccessPointType_DYNAMIC);

    sendMessage(fd, response, 3);
}

int handleWifiInfoRequestResponse(int fd, uint8_t *buffer, uint16_t length) {
    HU::WifiInfoResponse msg;
    msg.ParseFromArray(buffer, length);
    logd("WifiInfoResponse: %s\n", msg.DebugString().c_str());
    return msg.status();
}

//uint32 15
//uint32 3
//uint32 1
//uint32 100
//struct {
//    array of bytes [
//    2f 64 65 76 2f 70 74 73 2f 30 00 00 00 00 00 00 00 00 00 00 00
//    ]


void handle_connect(char *pty){
    char buf[100];
    logd("\tPTY: %s\n", pty);
    int fd = open(pty, O_RDWR | O_NOCTTY | O_SYNC);
    HU::WifiInfoRequest request;
    request.set_ip_address(IP_ADDRESS.c_str());
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
                        if (handleWifiInfoRequestResponse(fd, buffer, size) == 0) {
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

void BCAClient::ConnectionStatusResp(
        const uint32_t &serviceId,
        const uint32_t &connStatus,
        const uint32_t &btDeviceId,
        const uint32_t &status,
        const ::DBus::Struct <std::vector<uint8_t>> &terminalPath) {
    if (serviceId == 15 && connStatus == 3) {
        char *pty = (char *) malloc(terminalPath._1.size());
        std::copy(terminalPath._1.begin(), terminalPath._1.end(), pty);
        logd("\tPTY: %s\n", pty);
        handle_connect(pty);
        free(pty);
    }
}

void BDSClient::SignalConnected_cb(const uint32_t &type, const ::DBus::Struct <std::vector<uint8_t>> &data) {
    char mac[18];
    char pty[100];
    logd("Signal Connected:\n");
    logd("\tType: %u\n", type);
    std::copy(data._1.begin() + 14, data._1.begin() + 32, mac);
    mac[18] = '\0';
    logd("\tMAC: %s\n", mac);
    logd("\tService ID: %u\n", data._1[36]);
    if (data._1[36] == 15) {
        std::strncpy(pty, (char *) &data._1[48], 100);
        logd("\tPTY: %s\n", pty);
        handle_connect(pty);
    }
}

std::string hostapd_config(std::string key) {
    std::ifstream hostapd_file;
    hostapd_file.open("/tmp/current-session-hostapd.conf");

    if (hostapd_file) {
        std::string line;
        size_t pos;
        while (hostapd_file.good()) {
            getline(hostapd_file, line); // get line from file
            if (line[0] != '#') {
                pos = line.find(key); // search
                if (pos != std::string::npos) // string::npos is returned if string is not found
                {
                    int equalPosition = line.find("=");
                    std::string value = line.substr(equalPosition + 1).c_str();
                    return value.c_str();
                }
            }
        }
        return "";
    }
    else {
        return "";
    }
};

DBus::BusDispatcher wireless_dispatcher;

void wireless_stop(){
    wireless_dispatcher.leave();
}


void wireless_thread() {
    static BDSClient *bds_client = NULL;
    static NMSClient *nms_client = NULL;
    static BCAClient *bca_client = NULL;
    DBus::default_dispatcher = &wireless_dispatcher;

    logd("DBus::Glib::BusDispatcher attached\n");

    try {
        DBus::Connection hmi_bus(HMI_BUS_ADDRESS, false);
        hmi_bus.register_bus();
        DBus::Connection service_bus(SERVICE_BUS_ADDRESS, false);
        service_bus.register_bus();
        bds_client = new BDSClient(service_bus, "/com/jci/bds", "com.jci.bds");
        nms_client = new NMSClient(service_bus, "/com/jci/nms", "com.jci.nms");
        bca_client = new BCAClient(hmi_bus, "/com/jci/bca", "com.jci.bca");


        ::DBus::Struct <std::vector<int32_t>> interface_list;
        int32_t rvalue;
        nms_client->GetInterfaceList(rvalue, interface_list);
        ::DBus::Struct <int32_t, int32_t, std::string, std::string, std::string, std::string, std::string, std::string, std::string, std::string, int32_t, int32_t, int32_t, int32_t> interface_params;
        for (auto const &interface: interface_list._1) {
            nms_client->GetInterfaceParams(interface, rvalue, interface_params);
            logd("Interface: %s, MAC: %s, IP: %s\n", interface_params._4.c_str(), interface_params._6.c_str(),
                 interface_params._5.c_str());
            if (interface_params._4.compare("wlan0") == 0) {
                IP_ADDRESS.assign(interface_params._5.c_str());
                MAC_ADDRESS.assign(interface_params._6.c_str());
            }
        }

        bca_client->StartAdd(15);


        wireless_dispatcher.enter();
        logd("Exiting");
    }
    catch (DBus::Error &error) {
        loge("DBUS: Failed to connect to SERVICE bus %s: %s\n", error.name(), error.message());
    }
}
