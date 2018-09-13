#ifndef MZD_HUD_H
#define MZD_HUD_H

#include <stdint.h>
#include <string>

// struct HUDDisplayMsg {
//   uint32_t diricon; //Navigation icon
//   uint16_t distance;
//   uint8_t distanceUnit;
//   uint16_t speedLimit;
//   uint8_t speedLimitUnit;
//   uint8_t msgNumber; //Used to sync with HUD_Display_Msg2
// };
//
// struct HUD_Display_Msg2 {
//    std::string street;
//    uint8_t msgNumber; //Used to sync with HUDDisplayMsg
// };

void hud_start();
void hud_stop();
bool hud_installed();
void hud_send(uint32_t diricon, uint16_t distance, std::string street, u_int8_t msg);
#endif
