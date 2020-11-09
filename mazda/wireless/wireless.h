//
// Created by silverchris on 2020-11-06.
//

#ifndef HEADUNIT_WIRELESS_H
#define HEADUNIT_WIRELESS_H
#include <string>
#include "../dbus/generated_cmu.h"
#include "hu.pb.h"


class BDSClient : public com::jci::bds_proxy, public DBus::ObjectProxy
{
public:
    BDSClient(DBus::Connection &connection, const char *path, const char *name): DBus::ObjectProxy(connection, path, name){}

    virtual void Signaltest_cb(const uint32_t& type) override {}
    virtual void SignalReady_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalNotReady_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalDeviceDeleteInProgress_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalDeviceDeleted_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalDeviceDeleteAllInProgress_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalDeviceDeleteAll_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalDeviceDisconnectedAll_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalDeviceDisconnectionProgress_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalDeviceInquiryResult_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalDeviceNameStarted_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalDeviceNameSet_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalDeviceNameGet_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalDeviceAccessibilityGet_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalDevicePair_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalDeviceServiceDiscovery_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalDeviceWrittenAll_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalDeviceAuthenticate_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalRemoteSupportedFeatures_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalDeviceGetConnectedDevices_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalDeviceLocalOobData_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalDeviceExtendedSearchResult_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalServiceEnableStatus_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalServiceDisableStatus_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalConnected_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data);
    virtual void SignalDisconnected_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalServiceFeatures_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalServiceConfiguration_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalAudioSinkAppChange_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalAudioSinkBatteryStatusChange_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalAudioSinkBiased_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalAudioSinkBrowseConnect_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalAudioSinkBrowseDisonnect_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalAudioSinkBrowseFolderChange_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalAudioSinkBrowseFolderContent_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalAudioSinkBrowseFolderContents_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalAudioSinkBrowseSearch_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalAudioSinkBrowseUidsChange_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalAudioSinkContentAddToNowPlaying_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalAudioSinkContentAttributes_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalAudioSinkContentPlay_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalAudioSinkMediaPosition_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalAudioSinkPanelOperation_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalAudioSinkPlaybackStatusChange_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalAudioSinkPlayerAddressedChange_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalAudioSinkPlayerBrowsedChange_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalAudioSinkPlayerItem_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalAudioSinkPlayerItems_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalAudioSinkPlayerItemsChange_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalAudioSinkPlayerNowContentChange_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalAudioSinkPlayerVolumeChange_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalAudioSinkStreamClosed_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalAudioSinkStreamConfigured_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalAudioSinkStreamLine_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalAudioSinkStreamMuted_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalAudioSinkStreamOpened_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalAudioSinkSupportedChanges_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalAudioSinkSupportedPlayerSetting_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalAudioSinkSystemStatusChange_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalAudioSinkTrackChange_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalAudioSinkTrackEnd_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalAudioSinkTrackMetaData_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalAudioSinkTrackStart_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalCallClientCallList_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalCallClientStatus_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalCallClientAudioConnect_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalCallClientAudioDisconnect_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalCallClientIndicator_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalCallClientOperator_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalCallClientSubscriber_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalCallClientMicVolumeChange_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalCallClientSpeakerVolumeChange_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalCallClientActivateVoiceDial_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalCallClientDeActivateVoiceDial_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalCallClientSiriSupportStatus_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalCallClientSiriEyesFreeMode_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalCallClientSiriDisableNoiseReduction_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalPhoneStatus_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalCallClientCmdError_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalCallClientScoCodecId_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalMessageClientConfigured_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalMessageClientFolderListing_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalMessageClientMessageDeleted_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalMessageClientMessageGotten_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalMessageClientMessageListing_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalMessageClientMessageNotification_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalMessageClientMessagePushed_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalMessageClientMessageSetStatus_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalNetworkClientConnected_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalNetworkClientDisconnected_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalPimClientImportStatus_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalFileServerInfoEvent_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalFileServerFileAcceptanceStatus_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalFileServerDataFileInProgressStatus_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalServiceLessAudioConnectStatus_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalServiceLessAudioDisconnectStatus_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalServiceLessAclConnectStatus_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalServiceLessAclDisconnectStatus_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalBTChipFailure_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}
    virtual void SignalDeviceAddressGet_cb(const uint32_t& type, const ::DBus::Struct< std::vector< uint8_t > >& data) override {}

};

class NMSClient : public com::jci::nms_proxy, public DBus::ObjectProxy {
public:
    NMSClient(DBus::Connection&connection, const char *path, const char *name): DBus::ObjectProxy(connection, path, name){}

    virtual void InterfaceArrive(const int32_t &ifcId) override {}
    virtual void InterfaceDeparture(const int32_t &ifcId) override {}
    virtual void InterfaceChanged(const int32_t &ifcId, const int32_t &changeMask) override {}
    virtual void InterfaceConnecting(const int32_t &ifcId) override {}
    virtual void InterfaceDisconnecting(const int32_t &ifcId) override {}
    virtual void InterfaceConnected(const int32_t &ifcId) override {}
    virtual void InterfaceDisconnected(const int32_t &ifcId, const int32_t &reason) override {}
    virtual void WifiScanResultsReady(const int32_t &ifcId) override {}
    virtual void ConnectResult(const int32_t &ifcId, const int32_t &reqId, const int32_t &result) override {}
    virtual void InterfaceWiFiModeChanged(const int32_t &ifcId, const int32_t &mode) override {}
    virtual void WiFiAPClientConnected(const int32_t &ifcId, const std::string &bssid) override {}
    virtual void WiFiAPClientDisconnected(const int32_t &ifcId, const std::string &bssid) override {}
    virtual void APOpenInfo(const int32_t &ifcId, const ::DBus::Struct <std::string, uint32_t, int32_t, int32_t> &config) override {}
    virtual void APWepInfo(const int32_t &ifcId, const ::DBus::Struct<::DBus::Struct < std::string, uint32_t, int32_t, int32_t>, ::DBus::Struct <std::string, std::string, std::string, std::string>>& config) override {}
    virtual void APWpaInfo(const int32_t &ifcId, const ::DBus::Struct<::DBus::Struct < std::string, uint32_t, int32_t, int32_t>, ::DBus::Struct <int32_t, std::string>>& config) override {}
    virtual void InterfaceMonitorData(const int32_t &ifcId, const ::DBus::Struct <uint64_t, uint64_t> &statistics) override {}
    virtual void WifiSignalStrength(const int32_t &ifcId, const uint32_t &strength) override {}
};

class BCAClient : public com::jci::bca_proxy, public DBus::ObjectProxy {
public:
    BCAClient(DBus::Connection &connection, const char *path, const char *name) : DBus::ObjectProxy(connection, path, name) {}
    virtual void DontShowOnConnectionSettingStatusResp(const uint32_t& btDeviceId, const bool& setting) override {}
    virtual void ConnectionStatusResp(const uint32_t& serviceId, const uint32_t& connStatus, const uint32_t& btDeviceId, const uint32_t& status, const ::DBus::Struct< std::vector< uint8_t > >& terminalPath);
    virtual void HftReadyStatus(const uint32_t& hftReady, const uint32_t& reasonCode, const uint32_t& appId) override {}
    virtual void ReadyStatus(const uint32_t& isReady, const uint32_t& reasonCode) override {}
    virtual void AddStatusResp(const uint32_t& addServiceId, const uint32_t& status) override {}
    virtual void DeviceUnbarringStatusResp(const uint32_t& btDeviceId, const uint32_t& status) override {}
    virtual void DeviceBluetoothSettingResp(const bool& btSetting) override {}
    virtual void AvailablePairedListResp(const uint32_t& totalPairedDevices, const ::DBus::Struct< std::vector< uint8_t > >& pairedDeviceList) override {}
    virtual void SupportedProfilesResp(const ::DBus::Struct< std::vector< uint8_t > >& supportedProfiles) override {}
    virtual void FirstHfpSupportedInfo(const bool& isHfpSupportedDevicePresent) override {}
    virtual void CarPlayConnectionStatus(const uint32_t& connStatus, const uint32_t& carPlayDeviceId) override {}
    virtual void AAutoEnableBt() override {}
    virtual void AAutoNoEntryAvailable(const uint32_t& deleteDeviceId) override {}
    virtual void ReqStartAndroidAutoPairing() override {}
    virtual void ReqStopAndroidAutoPairing() override {}
    virtual void ReqDeleteDevice(const uint32_t& deleteDeviceId) override {}
    virtual void AAPairingSeqResult(const bool& result) override {}
    virtual void AndroidAutoPairingTimeout() override {}
    virtual void RequestStartAutoDownload(const uint32_t& btDeviceId) override {}
    virtual void DisableBluetoothRsp(const uint32_t& activeCallStatus) override {}
    virtual void ConnectingCarPlayError() override {}
};

void sendMessage(int fd, google::protobuf::MessageLite &message, uint16_t type);
void handleWifiInfoRequest(int fd, uint8_t *buffer, uint16_t length);
void handleWifiSecurityRequest(int fd, uint8_t *buffer, uint16_t length);
int handleWifiInfoRequestResponse(int fd, uint8_t *buffer, uint16_t length);
std::string hostapd_config(std::string key);
void wireless_stop();
void wireless_thread();

#endif //HEADUNIT_WIRELESS_H
