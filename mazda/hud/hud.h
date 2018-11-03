#ifndef MZD_HUD_H
#define MZD_HUD_H

#include <stdint.h>
#include <string>

struct NaviData {
  std::string event_name;
  int32_t turn_side;
  int32_t turn_event;
  int32_t turn_number;
  int32_t turn_angle;
  int32_t distance;
  int32_t time_until;
  uint8_t previous_msg;
};

enum NaviTurns: uint32_t {
  STRAIGHT = 1,
  LEFT = 2,
  RIGHT = 3,
  SLIGHT_LEFT = 4,
  SLIGHT_RIGHT = 5,
  DESTINATION  = 8,
  DESTINATION_LEFT = 33,
  DESTINATION_RIGHT = 34,
  SHARP_LEFT = 11,
  SHARP_RIGHT = 9,
  U_TURN_LEFT = 13,
  U_TURN_RIGHT = 10,
  FLAG = 12,
  FLAG_LEFT = 35,
  FLAG_RIGHT = 36,
  FORK_LEFT = 15,
  FORK_RIGHT = 14,
  MERGE_LEFT = 16,
  MERGE_RIGHT = 17
};

void hud_start();
void hud_stop();
bool hud_installed();
void hud_update();
#endif
