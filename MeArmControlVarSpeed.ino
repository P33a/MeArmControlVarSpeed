#include <VarSpeedServo.h>
#include <EEPROM.h>
#include <util/atomic.h>
#include "channels.h"
#include "proj_types.h"

const int NUM_SERVOS = 4; 

servos_t servos[NUM_SERVOS];
joystick_t joystick;

//int angles[4];
//char joy_angle_channels[4] = {'z', 'x', 'y', 'w'};

channels_t serial_channels;

unsigned long interval = 20;             // Sensor update period (ms)
unsigned long previous_micros = 0;        // will store last time LED was updated

static uint32_t build_32bits(byte b3, byte b2, byte b1, byte b0)
{
  uint32_t v = b3;
  v = (v << 8) | b2; 
  v = (v << 8) | b1; 
  v = (v << 8) | b0; 
  return v;
}

void serial_write(uint8_t b)
{
  Serial.write(b); 
} 
 
void process_serial_packet(char channel, uint32_t value, channels_t& obj)
{
  byte i;

  if (channel == 'S') {
    i = (value >> 24) & 0xFF;
    if (i < NUM_SERVOS) {
      servos[i].req_pulse = value & 0x00FFFF;
      servos[i].speed = (value >> 16) & 0xFF;
      servos[i].obj.write(servos[i].req_pulse, servos[i].speed, false);
    }  
  } else if (channel == 'O')  {
    joystick.buttons = value;  
  } else if (channel == 'X')  {
    joystick.axis[0] = value;  
  } else if (channel == 'Y')  {
    joystick.axis[1] = value;  
  } else if (channel == 'Z')  {
    joystick.axis[2] = value;  
  } else if (channel == 'W')  {
    joystick.axis[3] = value;  
  } else if (channel == '.')  {
    
  }
  
  if (channel == 's')  {
    i = (value >> 24) & 0xFF;
    if (i < NUM_SERVOS) {
      servos[i].req_pulse = value & 0x00FFFF;
    }  
  } else if (channel == 'w')  {
    i = (value >> 24) & 0xFF;
    if (i < NUM_SERVOS) {
      servos[i].speed = value & 0x00FFFF;
    }  
  } else if (channel == 't')  {
    obj.send(channel, value + 1);
    Serial.println(value + 1);
  }
}
 
 
void setup() 
{ 
  byte i;
  
  // Faster ADC - http://forum.arduino.cc/index.php/topic,6549.0.html
  // set prescaler to 16
  // sbi(ADCSRA,ADPS2);
  // cbi(ADCSRA,ADPS1);
  // cbi(ADCSRA,ADPS0);
  ADCSRA = (ADCSRA | (1 << ADPS2)) & ~((1 << ADPS1) | (1 << ADPS0));
  
  // Init Serial
  Serial.begin(115200);   
  Serial.println("MeArmControl v1.1 Started."); 
  serial_channels.init(process_serial_packet, serial_write);

  // Servo Pins
  servos[0].obj.attach(9);  // attaches the servo on pin 9 to the servo object
  servos[1].obj.attach(10);
  servos[2].obj.attach(11);
  servos[3].obj.attach(12);

  for (i = 0; i < NUM_SERVOS; i++) {
    servos[i].obj.write(1500, 255, false); // set the intial position of the servo, as fast as possible, run in background
    servos[i].speed = 30;
  }
 
  Serial.println("Init completed."); 
  // Signal a reset for the PC
  serial_channels.send('r', 0);
} 

static inline int sign(int val)
{
  if (val < 0) return -1;
  if (val==0) return 0;
  return 1;
}

byte control_done;
unsigned long current_micros;

void loop() 
{ 
  byte i;
  byte serialByte;
  uint32_t elapsed;
  
  
  // Serial Port Events
  if (Serial.available() > 0) {
    serialByte = Serial.read();
    serial_channels.StateMachine(serialByte);
    //Serial.write(serialByte);
  }   
  
  // This flag signals the "quiet" period after the servo pulses have been generated
  // It is the time to read the sensors and calculate the new servo pulses
  if (VarSpeedServo::refresh_flag) {
    current_micros = micros();
    Serial.println((current_micros - previous_micros)); 

    previous_micros = current_micros;
    VarSpeedServo::refresh_flag = 0;
    control_done++;
    
    // send the estimated position
    for (i = 0; i < NUM_SERVOS; i++) {
      uint32_t v = i;
      servos[i].act_pulse = servos[i].obj.readMicroseconds();
      serial_channels.send('s', (v << 24) | servos[i].act_pulse); 
    }
    
  }

  // Other Loop
  //current_micros = micros();

  //if(current_micros - previous_micros > interval * 1000) {
    // save the last loop time
    //previous_micros = current_micros;
    
    //slack = interval * 1000 - (micros() - current_micros);
    //elapsed = micros() - current_micros;
    //Serial.print(elapsed);
    //Serial.println(joystick.buttons, BIN);
    //Serial.print(joystick.axis[0]);
  //}
} 





