

//funcion a llamar, tamaño index, bytes a leer (se suman 3 byts por posicion)
//funcion 1 = slope_time_st
//funcion 2 = contstant_time_st
//funcion 3 = contstant_tempeture_stages_st
//funcion 4 = contstant_time_Fast
//funcion 5 = contstant_tempeture_stages_Fast
//funcion 6 = contstant_tempeture_stages_Fast
#ifndef DATAS_MODEL_
  #define DATAS_MODEL_
  void update_preset_values(unsigned char *value);
  #ifndef RTOS_VARIABLES
    #define RTOS_VARIABLES
    #include <Arduino_FreeRTOS.h>
    #include <semphr.h>
    #include <timers.h>
  #endif

  //variables software
  TimerHandle_t xTimers;
  SemaphoreHandle_t xSemaphore_USB, xSemaphore_TEMP, xSemaphore_MESAGE;
  //    CONSTANTES
  // direccion pin para resistencias
  #define ENABLE_RESISTENCIA 5
  // direccion led stages
  const int LED_ESTAGES[] = {0,1,2,3,4};
  // direccion sensor temperatura SPI
  #define SPI 1
  //VARIABLES
  //variables glovales a las task
  int minutos = 0;
  int stage = 0; //variable para saber en que estado estamos
  int temperatura_minutos_objetivo = 0;
  int standard_fast = 1; // 1 if standard 0 fast mode
  int temperatura = 0;
  int mesage = 0;
  //tiempo y siclo de cada stage (minutos, grados celcius)
  // Standart
  float SLOPE_TIME_ST[5] = {0, 0, 30, 140, 0};
  float CONSTANT_TIME_ST[5] = {0, 180, 360, 660, 670};
  float CONSTANT_TEMPETURE_ST[5] = {0, 150, 260, 950, 900};
  // Fast
  float CONSTANT_TIME_FAST[4] = {0, 85, 35, 25};
  float SLOPE_TIME_FAST[3] = {0, 30, 20};
  float CONSTANT_TEMPETURE_FAST[3] = {270, 780, 950};
  
  //work kind
#endif