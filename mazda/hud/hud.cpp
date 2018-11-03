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

  NaviData *navi_data = NULL;

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
  navi_data = new NaviData();
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
      //printf("DBUS: GetHUDIsInstalled failed %s: %s\n", error.name(), error.message());
      return(false);
  }
}

uint8_t turns[][3] = {
  {0,0,0}, //TURN_UNKNOWN
  {NaviTurns::FLAG_LEFT,NaviTurns::FLAG_RIGHT,NaviTurns::FLAG}, //TURN_DEPART
  {NaviTurns::STRAIGHT,NaviTurns::STRAIGHT,NaviTurns::STRAIGHT}, //TURN_NAME_CHANGE
  {NaviTurns::SLIGHT_LEFT,NaviTurns::SLIGHT_RIGHT,0}, //TURN_SLIGHT_TURN
  {NaviTurns::LEFT,NaviTurns::RIGHT,0}, //TURN_TURN
  {NaviTurns::SHARP_LEFT,NaviTurns::SHARP_RIGHT,0}, //TURN_SHARP_TURN
  {NaviTurns::U_TURN_LEFT, NaviTurns::U_TURN_RIGHT,0}, //TURN_U_TURN
  {NaviTurns::LEFT,NaviTurns::RIGHT,0}, //TURN_ON_RAMP
  {NaviTurns::LEFT,NaviTurns::RIGHT,0}, //TURN_OFF_RAMP
  {NaviTurns::FORK_LEFT, NaviTurns::FORK_RIGHT, 0}, //TURN_FORK
  {NaviTurns::MERGE_LEFT, NaviTurns::MERGE_RIGHT, 0}, //TURN_MERGE
  {0,0,0},  //TURN_ROUNDABOUT_ENTER
  {0,0,0}, // TURN_ROUNDABOUT_EXIT
  {0,0,0}, //TURN_ROUNDABOUT_ENTER_AND_EXIT (Will have to handle seperatly)
  {NaviTurns::STRAIGHT,NaviTurns::STRAIGHT,NaviTurns::STRAIGHT}, //TURN_STRAIGHT
  {0,0,0}, //unused?
  {0,0,0}, //TURN_FERRY_BOAT
  {0,0,0}, //TURN_FERRY_TRAIN
  {0,0,0}, //unused??
  {NaviTurns::DESTINATION_LEFT, NaviTurns::DESTINATION_RIGHT, NaviTurns::DESTINATION} //TURN_DESTINATION
};

uint8_t roundabout(int32_t degrees){
  uint8_t nearest = (((degrees + 15) / 30)*30);
  uint8_t offset = 37; //+49 for Left hand drive?
  return(nearest+offset);
}

int32_t round_to_nearest(int32_t number, int32_t nearest){
  return (((number + (nearest/2)) / nearest)*nearest);
}

void hud_update(){
  if (hud_client == NULL) {
    //Don't bother with the HUD if we aren't connected via dbus
    return;
  }

  uint32_t diricon;
  if(navi_data->turn_event == 13){
        diricon = roundabout(navi_data->turn_angle);
  }
  else{
    int32_t turn_side = navi_data->turn_side-1; //Google starts at 1 for some reason...
    diricon = turns[navi_data->turn_event][turn_side];
  }

  uint8_t distance_unit;
  if(navi_data->distance > 3000){
    distance_unit = 3;
    navi_data->distance = navi_data->distance/100;
  }
  else{
    distance_unit = 1;
    navi_data->distance = (((navi_data->distance + 5) / 10)*10);
    navi_data->distance = navi_data->distance*10;
  }

  ::DBus::Struct< uint32_t, uint16_t, uint8_t, uint16_t, uint8_t, uint8_t > hudDisplayMsg;
  hudDisplayMsg._1 = diricon;
  hudDisplayMsg._2 = navi_data->distance;
  hudDisplayMsg._3 = distance_unit; //distance unit 1 = meter?
  hudDisplayMsg._4 = 0; //Speed limit (Not Used)
  hudDisplayMsg._5 = 0; //Speed limit units (Not used)
  hudDisplayMsg._6 = navi_data->previous_msg+1;

  ::DBus::Struct< std::string, uint8_t > guidancePointData;
  guidancePointData._1 = navi_data->event_name;
  guidancePointData._2 = navi_data->previous_msg+1;

  navi_data->previous_msg = navi_data->previous_msg+1;
  if(navi_data->previous_msg == 8){
    navi_data->previous_msg = 0;
  }

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
