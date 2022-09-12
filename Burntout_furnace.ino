#include <stdlib.h>
#include <Arduino_FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <timers.h>
#include <MAX31855.h>
#include <ArduPID.h>

//#include "comunicacion.h"

void PID(void * pvParameters);
void view_leds(void * pvParameters);
void USB(void * pvParameters);
void sensor(TimerHandle_t xTimer);
void temperatura_minutos(TimerHandle_t xTimer);
void call_for_time(const unsigned int* CONSTANT_TEMPETURE,
                   const unsigned int* CONSTANT_TIME,
                   const unsigned int* SLOPE_TIME,
                   const unsigned int* TOTAL_TIME,
                   unsigned int temp_,
                   unsigned int stage_);
void stage_led(unsigned char mesage_, unsigned char stage_, char* LED_ESTAGES_);
void led_grupos(const unsigned char state,const unsigned char _hasta, char *LED_ESTAGES_);
unsigned char mesagge_access(char mesage_save, bool edit);
unsigned int temperatura_access(unsigned int temperatura_, bool edit);
void serialsend_access(String mesage_send, uint8_t debuging);
unsigned int stage_access(unsigned int stage_, bool edit);

  //variables software
TimerHandle_t xTimer_temp_minutos, xTimer_sensor;
SemaphoreHandle_t xSemaphore_USBouput, xSemaphore_TEMP, xSemaphore_MESAGE, xSemaphore_STAGE;
MAX31855_Class MAX31855; ///< Create an instance of MAX31855
ArduPID myController;
double input;
double output;

 //    CONSTANTES
// direccion pin para resistencias
//const uint8_t ENABLE_RESISTENCIA = 5;
//VARIABLES
//variables glovales a las task
unsigned int minutos = 0;
unsigned int stage = 0; //variable para saber en que estado estamos
double temperatura_minutos_objetivo = 0;
bool standard_fast = true; // 1 if standard 0 fast mode
unsigned int temperatura = 0;
unsigned char mesage = 0;
const uint8_t  SPI_CHIP_SELECT  =      7; ///< Chip-Select PIN for SPI
//USB
bool debug = false;


void setup(){
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600); // opens serial port, sets data rate to 9600 bps
  Serial.setTimeout(100); 
  if ( xSemaphore_USBouput == NULL ){
    xSemaphore_USBouput = xSemaphoreCreateMutex();
  } 
  if ( xSemaphore_TEMP == NULL ){ 
    xSemaphore_TEMP = xSemaphoreCreateMutex();
  }
  if ( xSemaphore_MESAGE == NULL ){ 
    xSemaphore_MESAGE = xSemaphoreCreateMutex();
  }
  if ( xSemaphore_STAGE == NULL ){ 
    xSemaphore_STAGE = xSemaphoreCreateMutex();
  }
  if(((xSemaphore_MESAGE) != NULL) ||
    ((xSemaphore_TEMP) != NULL) ||
    ((xSemaphore_USBouput) != NULL) ||
    ((xSemaphore_STAGE) != NULL)){
    xSemaphoreGive(xSemaphore_MESAGE);
    xSemaphoreGive(xSemaphore_TEMP);
    xSemaphoreGive(xSemaphore_USBouput);
    xSemaphoreGive(xSemaphore_STAGE);
  }
  while (!MAX31855.begin(SPI_CHIP_SELECT, MISO, SCK)){ // Software SPI for MAX31855
    serialsend_access("Unable to start MAX31855. Waiting 3 seconds.", 2);
    delay(3000);
  } // of loop until device is located
  myController.begin(&input, &output, &temperatura_minutos_objetivo, 10, 1, 0.5);
  myController.setOutputLimits(0, 1);
  //myController.setBias(255.0 / 2.0);
  myController.setWindUpLimits(-10, 10); // Groth bounds for the integral term to prevent integral wind-up
  myController.start();
  /*xTaskCreate(PID
              ,"PID_"
              ,100
              ,NULL
              ,1
              ,NULL
              );
  xTaskCreate(view_leds
              ,"view_leds_"
              ,250
              ,NULL
              ,0
              ,NULL
              );*/
  /*xTaskCreate(USB
              ,"USB_"
              ,100
              ,NULL
              ,3
              ,NULL
              ); */

  //creando timer de periodo de un minuto para calcular la temperatura objetibo por minuto
  xTimer_temp_minutos = xTimerCreate(
                                    "minutos_clock",
                                    pdMS_TO_TICKS(6000),
                                    pdTRUE,
                                    NULL,
                                    temperatura_minutos
                                    );
  xTimer_sensor = xTimerCreate("Sensor_muestra",
                                pdMS_TO_TICKS(3500),
                                pdTRUE,
                                NULL,
                                sensor
                                );
  if( xTimer_temp_minutos == NULL ){
    serialsend_access("no se ha creado el timer, para la temperatura minutos",2);
    }
  else{
    /* Start the timer.*/
    if( xTimerStart( xTimer_temp_minutos, 0 ) != pdPASS ){
      serialsend_access("no se activo el timer",2);
      }
    }

  if( xTimer_sensor == NULL ){
    serialsend_access("no se ha creado el timer, para leer el sensor",2);
    }
  else{
    if( xTimerStart( xTimer_sensor, 0 ) != pdPASS ){
      serialsend_access("no se activo el timer", 2);
      }
    }
    Serial.print("bbb");
    
    Serial.print("aaaaa");
}


void loop() {}


void PID( void *pvParameters __attribute__((unused)) ){
   
  for(;;){
                    
  input = temperatura_access(0, 0) + 10.8;
  Serial.println("jgf");
  myController.compute();
  if(output>0.5){
    digitalWrite(5, HIGH);
  }else{
    digitalWrite(5, LOW);
  }

  }
}


void view_leds( void *pvParameters __attribute__((unused)) ){
  char LED_ESTAGES[] = {2, 3, 4, 5, 6, 9, 10};
  int led_total = 7;
  int mesage_copy = 0;
  for(int led_ = 0; led_<led_total; led_++){
    pinMode(LED_ESTAGES[led_], OUTPUT);
  }
  for(;;){
    vTaskDelay(10);
    if( xSemaphoreTake(xSemaphore_MESAGE, ( TickType_t ) 5 )  == pdTRUE){
      if(mesagge_access(0, false)==0){
        mesage_copy = 1;
      }else{
        mesage_copy = mesagge_access(0, false);
      }
      stage_led(mesage_copy, stage_access(0, false), &LED_ESTAGES[0]);
      xSemaphoreGive(xSemaphore_MESAGE);
    }
  }
}


void USB( void *pvParameters __attribute__((unused)) ){
  char first_byte;
  for(;;){
      if (Serial.available()){
        first_byte = Serial.read();
        if(first_byte==20){
          debug = true;
        }
      }
if(debug){
  digitalWrite(LED_BUILTIN, HIGH);
  //vTaskDelay(1000);
  //digitalWrite(LED_BUILTIN, LOW);
  //vTaskDelay(1000);
}
    }
  }



void sensor(TimerHandle_t xTimer __attribute__((unused)) ){
  char sensor_fail;
  for(;;){
    sensor_fail = MAX31855.fault();
    if(sensor_fail){
	    if(sensor_fail & 0b010){        
        serialsend_access("\nThermocouple is short-circuited to GND.", 2);
        mesagge_access(4, true);
	    }
	    if(sensor_fail & 0b100){
        serialsend_access("\nthermocouple is short-circuited to VCC.", 2);
        mesagge_access(3, true);
	    }
	    if(sensor_fail & 0b001){
	      serialsend_access("\nThermocouple is open (no connections).", 2);
        mesagge_access(2, true);
	    }
    }else{ 
     
	    temperatura_access((MAX31855.readProbe()/1000)/2, true);
      if(mesagge_access(0, false)==2 ||
         mesagge_access(0, false)==3 ||
         mesagge_access(0, false)==4){
        mesagge_access(0, true);
      }
    }
  }
}


void temperatura_minutos(TimerHandle_t xTimer __attribute__((unused)) ){
  minutos++;    
  unsigned int temp_ = temperatura_access(0, 0);
  unsigned int stage_ = stage_access(0, false);
  serialsend_access("\n\nStage: ",1);
  serialsend_access(String(stage_), 1);
  serialsend_access("\nTemp: ", 1);
  serialsend_access(String(temp_), 1);
  if (standard_fast){
     //Standart
    const unsigned int Total_time_ST[5] =         {0, 0, 20, 90, 130}; 
    const unsigned int SLOPE_TIME_ST[5] =         {0, 0, 0, 30, 0};
    const unsigned int CONSTANT_TIME_ST[5] =      {0, 20, 40, 25, 15};
    const unsigned int CONSTANT_TEMPETURE_ST[5] = {0, 50, 80, 50, 90};
    if (stage_<=5){
      serialsend_access("\nTemp aim ST: ", 1);
      serialsend_access(String(CONSTANT_TEMPETURE_ST[stage_]), 1);        
      call_for_time(&CONSTANT_TEMPETURE_ST[stage_], 
                    &CONSTANT_TIME_ST[stage_], 
                    &SLOPE_TIME_ST[stage_],
                    &Total_time_ST[stage_],
                    temp_,
                    stage_);
    }    
  }else{
     //Fast
    const unsigned int Total_time_FAST[4] =          {0, 20, 90, 45};      
    const unsigned int CONSTANT_TIME_FAST[4] =      {0, 85, 35, 25};
    const unsigned int SLOPE_TIME_FAST[4] =         {0, 30, 20, 45};
    const unsigned int CONSTANT_TEMPETURE_FAST[4] = {270, 780, 950, 758};
    if (stage_<=4){
      serialsend_access("\nTemp aim FT: ", 1);
      serialsend_access(String(CONSTANT_TEMPETURE_FAST[stage_]), 1);
      call_for_time(&CONSTANT_TEMPETURE_FAST[stage_],
                    &CONSTANT_TIME_FAST[stage_],
                    &SLOPE_TIME_FAST[stage_],
                    &Total_time_FAST[stage_],
                    temp_,
                    stage_);
    }
  }
  serialsend_access("\nTemp aim minut: ", 1);
  serialsend_access(String(temperatura_minutos_objetivo), 1);
  serialsend_access("\nMinuts: ", 1);
  serialsend_access(String(minutos), 1);
}


void call_for_time(const unsigned int* CONSTANT_TEMPETURE,
                   const unsigned int* CONSTANT_TIME, 
                   const unsigned int* SLOPE_TIME,
                   const unsigned int* TOTAL_TIME,
                   unsigned int temp_,
                   unsigned int stage_){
  if((((temp_<=(*(CONSTANT_TEMPETURE + 1) * 1.35)) && 
       (temp_>=(*(CONSTANT_TEMPETURE + 1) * 0.88)) && 
       stage_==0) || 
      ((temp_<=(*CONSTANT_TEMPETURE * 1.35)) &&
       (temp_>=(*CONSTANT_TEMPETURE * 0.88)) && 
       stage_>0)) &&
     minutos>=*TOTAL_TIME + *CONSTANT_TIME){            

    if(stage_==0){
      minutos = 0;
    }
    if(stage_==4){
    //hemos terminado
      if(mesagge_access(0, false)==0){
        mesagge_access(6, true);
      }      
      serialsend_access("\nBurnt Out Finished", 1);          
    }else{
      stage_access(stage_ + 1, true);
    }
  }
  else{
    if((minutos>(*TOTAL_TIME + *CONSTANT_TIME) && stage_!=0 && stage_!=4) || (minutos>40 && stage_==0)){
      //error tempratura no aumenta
      if(mesagge_access(0, false)==0){
        mesagge_access(5, true);
      }
      serialsend_access("\nFail resistor", 1);
    }else{
      if(((minutos>=*TOTAL_TIME - *SLOPE_TIME) ||
          stage_==0 ||
          (stage_>0 && *SLOPE_TIME==0))
         && minutos<*TOTAL_TIME){
        if(*SLOPE_TIME==0){
          if((temperatura_minutos_objetivo!=*(CONSTANT_TEMPETURE + 1)) ||
           (temperatura_minutos_objetivo!=*CONSTANT_TEMPETURE)){
            if(stage_==0){
              temperatura_minutos_objetivo = *(CONSTANT_TEMPETURE + 1);
            }else{
              temperatura_minutos_objetivo = *(CONSTANT_TEMPETURE);
            }
          }                     
        }else{       
          if (*CONSTANT_TEMPETURE>*(CONSTANT_TEMPETURE - 1)){
            temperatura_minutos_objetivo += ((double)(*CONSTANT_TEMPETURE - *(CONSTANT_TEMPETURE - 1))/((double)(*SLOPE_TIME)));
          }
          if (*CONSTANT_TEMPETURE<*(CONSTANT_TEMPETURE - 1)){
            temperatura_minutos_objetivo -= (((double)(*(CONSTANT_TEMPETURE - 1) - *CONSTANT_TEMPETURE))/((double)(*SLOPE_TIME)));
          }
        }
      }
    }
  }
}
/*
view leds indicate current stage or error codes which represent some fails
case 0: nothing, apagamos todos los leds
case 1: activate the leds wich represent the stage
case 2: thermocouple is open (no connections).
case 3: thermocouple is short-circuited to VCC.
case 4: thermocouple is short-circuited to GND.
case 5: Fail resistor
case 6: Burnt Out Finished

*/

void stage_led(unsigned char mesage_, unsigned int stage_, char* LED_ESTAGES_){
  
  switch(mesage_){
	  case 0:
		  led_grupos(0, 5, LED_ESTAGES_);
      break;
	  case 1:
		  digitalWrite(*(LED_ESTAGES_ + stage_), HIGH);
    	if(stage_>0){
      	digitalWrite(*(LED_ESTAGES_ + stage_ - 1), LOW);
    	}
      break;
	  case 2:
		  led_grupos(1, 5, LED_ESTAGES_);
    	delay(500);
    	led_grupos(0, 5, LED_ESTAGES_);
    	delay(500);
      break;
	  case 3:
		  led_grupos(1, 4, LED_ESTAGES_);
    	delay(500);
    	led_grupos(0, 4, LED_ESTAGES_);
    	delay(500);
      break;
	  case 4:
		  led_grupos(1, 3, LED_ESTAGES_);
    	delay(500);
    	led_grupos(0, 3, LED_ESTAGES_);
    	delay(500);
      break;
	  case 5:
		  led_grupos(1, 2, LED_ESTAGES_);
    	delay(500);
    	led_grupos(0, 2, LED_ESTAGES_);
    	delay(500);
      break;
	  case 6:
		  led_grupos(1, 1, LED_ESTAGES_);
    	delay(500);
    	led_grupos(0, 1, LED_ESTAGES_);
    	delay(500);
      break;
	  case 7:
		  led_grupos(1, 7, LED_ESTAGES_);
    	delay(500);
    	led_grupos(0, 7, LED_ESTAGES_);
    	delay(500);
      break;          
	}
}


void led_grupos(const unsigned char state, const unsigned char _hasta, char *LED_ESTAGES_){
  for (int led_ = 0; led_<_hasta; led_++){
    digitalWrite(*LED_ESTAGES_, state);
    LED_ESTAGES_++;
  }
}

unsigned char mesagge_access(char mesage_save, bool edit){
  if( xSemaphoreTake(xSemaphore_MESAGE, ( TickType_t ) 5 )  == pdTRUE){
    if(edit){         
      mesage = mesage_save;
    }else{
      xSemaphoreGive(xSemaphore_MESAGE);
      return mesage;          
    }  
  }
  xSemaphoreGive(xSemaphore_MESAGE);
  return 0;
}


void serialsend_access(String mesage_send, uint8_t debuging){
  if( xSemaphoreTake(xSemaphore_USBouput, ( TickType_t ) 0 )  == pdTRUE){
    if((debuging==2 && debug) || debuging==1){
      Serial.print(mesage_send);
    }
    xSemaphoreGive(xSemaphore_USBouput);
  }
}


unsigned int temperatura_access(unsigned int temperatura_, bool edit){ 
  if( xSemaphoreTake(xSemaphore_TEMP, ( TickType_t ) 0 )  == pdTRUE){ 
    if(edit){         
      temperatura = temperatura_;
    }else{
      xSemaphoreGive(xSemaphore_TEMP);
      return temperatura;          
    }  
  }
  xSemaphoreGive(xSemaphore_TEMP);
  return 0;  
}


unsigned int stage_access(unsigned int stage_, bool edit){ 
  if( xSemaphoreTake(xSemaphore_STAGE, ( TickType_t ) 0 )  == pdTRUE){ 
    if(edit){
      stage = stage_;
    }else{
      xSemaphoreGive(xSemaphore_STAGE);
      return stage;
    }
  }
  xSemaphoreGive(xSemaphore_STAGE);
  return 0;
}