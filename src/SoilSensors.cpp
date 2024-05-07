#include "SoilSensors.h"
#include <arduino.h>

SoilSensors::SoilSensors(int select_pin_0, int select_pin_1, int select_pin_2, int select_pin_3, int analog_pin):
  select_pin_0_{select_pin_0}, select_pin_1_{select_pin_1}, select_pin_2_{select_pin_2}, select_pin_3_{select_pin_3},
  analog_pin_{analog_pin}
{
}

/**
 * initializes hardware
 */
void SoilSensors::Init(){

  pinMode(select_pin_0_, OUTPUT);
  pinMode(select_pin_1_, OUTPUT);
  pinMode(select_pin_2_, OUTPUT);
  pinMode(select_pin_3_, OUTPUT);
  pinMode(analog_pin_, INPUT);
}

/**
 * Read value from one of the MUX channels and access one individual sensor
 *
 * @param selected_sensor MUX channel / selected sensor
 * @return int (0-1023) - analog sensor value
 */
int SoilSensors::GetMoisture(int selected_sensor){   //TODO clean this Mist?
  
  switch(selected_sensor){
    case 1:
      digitalWrite(select_pin_0_, 0);
      digitalWrite(select_pin_1_, 0);
      digitalWrite(select_pin_2_, 0);
      digitalWrite(select_pin_3_, 0);
      break;
    
    case 2:
      digitalWrite(select_pin_0_, 1);
      digitalWrite(select_pin_1_, 0);
      digitalWrite(select_pin_2_, 0);
      digitalWrite(select_pin_3_, 0);
      break;

    case 3:
      digitalWrite(select_pin_0_, 0);
      digitalWrite(select_pin_1_, 1);
      digitalWrite(select_pin_2_, 0);
      digitalWrite(select_pin_3_, 0);
      break;

    case 4:
      digitalWrite(select_pin_0_, 1);
      digitalWrite(select_pin_1_, 1);
      digitalWrite(select_pin_2_, 0);
      digitalWrite(select_pin_3_, 0);
      break;

    case 5:
      digitalWrite(select_pin_0_, 0);
      digitalWrite(select_pin_1_, 0);
      digitalWrite(select_pin_2_, 1);
      digitalWrite(select_pin_3_, 0);
      break;

    case 6:
      digitalWrite(select_pin_0_, 1);
      digitalWrite(select_pin_1_, 0);
      digitalWrite(select_pin_2_, 1);
      digitalWrite(select_pin_3_, 0);
      break;

    case 7:
      digitalWrite(select_pin_0_, 0);
      digitalWrite(select_pin_1_, 1);
      digitalWrite(select_pin_2_, 1);
      digitalWrite(select_pin_3_, 0);
      break;

    case 8:
      digitalWrite(select_pin_0_, 1);
      digitalWrite(select_pin_1_, 1);
      digitalWrite(select_pin_2_, 1);
      digitalWrite(select_pin_3_, 0);
      break;

    case 9:
      digitalWrite(select_pin_0_, 0);
      digitalWrite(select_pin_1_, 0);
      digitalWrite(select_pin_2_, 0);
      digitalWrite(select_pin_3_, 1);
      break;
    
    case 10:
      digitalWrite(select_pin_0_, 1);
      digitalWrite(select_pin_1_, 0);
      digitalWrite(select_pin_2_, 0);
      digitalWrite(select_pin_3_, 1);
      break;

    case 11:
      digitalWrite(select_pin_0_, 0);
      digitalWrite(select_pin_1_, 1);
      digitalWrite(select_pin_2_, 0);
      digitalWrite(select_pin_3_, 1);
      break;

    case 12:
      digitalWrite(select_pin_0_, 1);
      digitalWrite(select_pin_1_, 1);
      digitalWrite(select_pin_2_, 0);
      digitalWrite(select_pin_3_, 1);
      break;

    case 13:
      digitalWrite(select_pin_0_, 0);
      digitalWrite(select_pin_1_, 0);
      digitalWrite(select_pin_2_, 1);
      digitalWrite(select_pin_3_, 1);
      break;

    case 14:
      digitalWrite(select_pin_0_, 1);
      digitalWrite(select_pin_1_, 0);
      digitalWrite(select_pin_2_, 1);
      digitalWrite(select_pin_3_, 1);
      break;

    case 15:
      digitalWrite(select_pin_0_, 0);
      digitalWrite(select_pin_1_, 1);
      digitalWrite(select_pin_2_, 1);
      digitalWrite(select_pin_3_, 1);
      break;

    case 16:
      digitalWrite(select_pin_0_, 1);
      digitalWrite(select_pin_1_, 1);
      digitalWrite(select_pin_2_, 1);
      digitalWrite(select_pin_3_, 1);
      break;
  }

  return analogRead(analog_pin_);

}