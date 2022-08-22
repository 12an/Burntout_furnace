#include <Arduino_FreeRTOS.h>
#include <timers.h>
#include "Modulos/comunicacion.h"
#include "Modulos/datas_model.h"
#include "Modulos/view_model.h"


void PID( void *parametros);
void view_leds(void *parametros);
void USB(void *parametros);
void sensor(void *parametros);
void minutos_clock(void *parametros);
void temperatura_minutos(TimerHandle_t xTimer);


void setup() {
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
  Serial.setTimeout(500)  
  pinMode(enable_resistencia, OUTPUT);
  pinMode(stage_1, OUTPUT);
  pinMode(stage_2, OUTPUT);
  pinMode(stage_3, OUTPUT);
  pinMode(stage_4, OUTPUT);
  xTaskCreate(
              PID
              ,"PID_" // A name just for humans
              ,128      // This stack size can be checked and adjusted by reading the Stack Highwater
              ,NULL
              ,1        // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
              ,NULL
              );
  xTaskCreate(
              view_leds
              ,"view_leds_" // A name just for humans
              ,50      // This stack size can be checked and adjusted by reading the Stack Highwater
              ,NULL
              ,0        // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
              ,NULL
              );  
  xTaskCreate(
              USB
              ,"USB_" // A name just for humans
              ,128      // This stack size can be checked and adjusted by reading the Stack Highwater
              ,NULL
              ,3        // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
              ,NULL
              ); 
  xTaskCreate(
              sensor
              ,"sensor_" // A name just for humans
              ,128      // This stack size can be checked and adjusted by reading the Stack Highwater
              ,NULL
              ,1        // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
              ,NULL
              );
  xTimers = xTimerCreate(
              minutos_clock,
              pdMS_TO_TICKS(60000),
              pdTRUE,
              ( void * ) 0,
              temperatura_minutos
              );
                
             
}


void loop() {}

void PID( void *parametros){
  (void) *parametros;
  for(;;){

  }

}


void view_leds( void *parametros){
  (void) *parametros;
  for(;;){
    
  }

}


void USB( void *parametros){
  (void) *parametros;
  unsigned char byts_fromusb[6];
  for(;;){
    Serial.readBytesUntil(255, byts_fromusb, 6);
    if(byts_fromusb==-1){
      update_preset_values(&byts_fromusb[0]);
    }
  }
}


void sensor( void *parametros){
  (void) *parametros;
  for(;;){
    
  }

}


void temperatura_minutos(TimerHandle_t xTimer){


}