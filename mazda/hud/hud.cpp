#include "hud.h"

#include <dbus/dbus.h>
#include <dbus-c++/dbus.h>
#include <stdint.h>
#include <string>

#include "../dbus/generated_cmu.h"

// #define LOGTAG "mazda-hud"

// #include "hu_uti.h"

#define SERVICE_BUS_ADDRESS "unix:path=/tmp/dbus_service_socket"
#define HMI_BUS_ADDRESS "unix:path=/tmp/dbus_hmi_socket"



class HUDSettingsClient : public com::jci::navi2IHU::HUDSettings_proxy,
                     public DBus::ObjectProxy
{
public:
    HUDSettingsClient(DBus::Connection &connection, const char *path, const char *name)
        : DBus::ObjectProxy(connection, path, name)
    {
    }

    virtual void HUDInstalledChanged(const bool& hUDInstalled) override {}
    virtual void SetHUDSettingFailed(const int32_t& hUDSettingType, const int32_t& err) override {}
    virtual void HUDControlAllowed(const bool& bAllowed) override {}
    virtual void HUDSettingChanged(const int32_t& hUDSettingType, const int32_t& value) override {}
};

class NaviClient : public com::jci::vbs::navi_proxy,
                     public DBus::ObjectProxy
{
public:
    NaviClient(DBus::Connection &connection, const char *path, const char *name)
        : DBus::ObjectProxy(connection, path, name)
    {
    }

    virtual void FuelTypeResp(const uint8_t& fuelType) override {}
    virtual void HUDResp(const uint8_t& hudStatus) override {}
    virtual void TSRResp(const uint8_t& tsrStatus) override {}
    virtual void GccConfigMgmtResp(const ::DBus::Struct< std::vector< uint8_t > >& vin_Character) override {}
    virtual void TSRFeatureMode(const uint8_t& tsrMode) override {}
};

  class TMCClient : public com::jci::vbs::navi::tmc_proxy,
                       public DBus::ObjectProxy
  {
  public:
      TMCClient(DBus::Connection &connection, const char *path, const char *name)
          : DBus::ObjectProxy(connection, path, name)
      {
      }

      virtual void ServiceListResponse(const ::DBus::Struct< uint8_t, std::vector< uint8_t >, std::vector< uint8_t >, std::vector< uint8_t >, std::vector< uint8_t >, std::vector< uint8_t > >& providerList) override {}
      virtual void ResponseToTMCSelection(const uint8_t& rdstmcOperation, const uint8_t& tmcSearchMode, const uint8_t& countryCode, const uint8_t& locationTableNumber, const uint8_t& serviceIdentifier, const uint8_t& quality, const uint8_t& receptionStatus) override {}
};

  static HUDSettingsClient *hud_client = NULL;
  static NaviClient *vbsnavi_client = NULL;
  static TMCClient *tmc_client = NULL;

void hud_start()
{
  if (hud_client != NULL)
    return;

  try
  {
    DBus::Connection service_bus(SERVICE_BUS_ADDRESS, false);
    service_bus.register_bus();
    DBus::Connection hmiBus(HMI_BUS_ADDRESS, false);
    hmiBus.register_bus();
    hud_client = new HUDSettingsClient(hmiBus, "/com/jci/navi2IHU", "com.jci.navi2IHU");
    vbsnavi_client = new NaviClient(service_bus, "/com/jci/vbs/navi", "com.jci.vbs.navi");
    tmc_client = new TMCClient(service_bus, "/com/jci/vbs/navi", "com.jci.vbs.navi");
  }
  catch(DBus::Error& error)
  {
    printf("DBUS: Failed to connect to SERVICE bus %s: %s\n", error.name(), error.message());
    hud_stop();
    return;
  }
  printf("HUD dbus connections established\n");
  return;
}

void hud_stop()
{
  delete hud_client;
  hud_client = nullptr;

  delete vbsnavi_client;
  vbsnavi_client = nullptr;

  delete tmc_client;
  tmc_client = nullptr;
}

bool hud_installed()
{
  if (hud_client == NULL)
      return(false);

  try
  {
      return(hud_client->GetHUDIsInstalled());
  }
  catch(DBus::Error& error)
  {
      printf("DBUS: GetHUDIsInstalled failed %s: %s\n", error.name(), error.message());
      return(false);
  }
}

void hud_send(uint32_t diricon, uint16_t distance, std::string street, u_int8_t msg){


  ::DBus::Struct< uint32_t, uint16_t, uint8_t, uint16_t, uint8_t, uint8_t > hudDisplayMsg;
  hudDisplayMsg._1 = diricon;
  hudDisplayMsg._2 = distance;
  hudDisplayMsg._3 = 1; //distance unit 1 = meter?
  hudDisplayMsg._4 = 0;
  hudDisplayMsg._5 = 0;
  hudDisplayMsg._6 = msg;

  ::DBus::Struct< std::string, uint8_t > guidancePointData;
  guidancePointData._1 = street;
  guidancePointData._2 = msg;

  if (hud_client == NULL)
    return;
  try
  {
    vbsnavi_client->SetHUDDisplayMsgReq(hudDisplayMsg);
    tmc_client->SetHUD_Display_Msg2(guidancePointData);
  }
  catch(DBus::Error& error)
  {
      printf("DBUS: hud_send failed %s: %s\n", error.name(), error.message());
      return;
  }
  return;
}
