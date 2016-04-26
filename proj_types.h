#ifndef PROJ_TYPES_H
#define PROJ_TYPES_H

#include "Arduino.h"
#include <VarSpeedServo.h> 


typedef struct{
    uint16_t act_pulse, req_pulse;
    uint16_t speed;
     VarSpeedServo obj;
} servos_t;

const int NUM_AXIS = 4; 
typedef enum { 
  jb_triangle,
  jb_circle,
  jb_cross,
  jb_square,
  jb_L2,
  jb_R2,
  jb_L1,
  jb_R1,
  jb_select,
  jb_start,
  jb_analog1,
  jb_analog2
} joy_button_t;


typedef struct{
    int axis[NUM_AXIS];
    uint16_t buttons;
} joystick_t;


#endif // PROJ_TYPES_H
