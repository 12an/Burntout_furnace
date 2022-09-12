#include "comunicacion.h"
void sensor_() {
  /*!
    @brief    Arduino method for the main program loop
    @details  This is the main program for the Arduino IDE, it is an infinite loop and keeps on
              repeating
    @return   void
  */

  
  uint8_t faultCode          = MAX31855.fault();        // retrieve any error codes
  if (faultCode)                                        // Display error code if present
  {
    if (faultCode & B001) {

    }
    if (faultCode & B010) {

    }
    if (faultCode & B100) {

    }
  } else {
    // clang-format off
    int32_t probeTemperature   = MAX31855.readProbe()/2000;    // retrieve thermocouple probe temp

    // clang-format on
  }  // of if-then-else an error occurred
  delay(5000);
}  // of method loop()


