#include "Modulos/datas_model.h"
//variables software
TimerHandle_t xTimers;

// direccion pin para resistencias
#define enable_resistencia 5
// direccion led stages
#define stage_1 1
#define stage_2 2
#define stage_3 3
#define stage_4 4
// direccion sensor temperatura SPI
#define SPI_sensor 1
//variables glovales a las task
int minutos = 0;
int temperatura = 0;
//tiempo y siclo de cada stage (minutos, grados celcius)
// Standart
float slope_time_st[4] = {0, 30, 140, 0};
float contstant_time_st[4] = {180, 180, 300, 10};
float contstant_tempeture_stages_st[4] = {150, 260, 950, 900};
// Fast
float contstant_time_Fast[3] = {85, 35, 25};
float slope_time_Fast[3] = {0, 30, 20};
float contstant_tempeture_stages_Fast[3] = {270, 780, 950};


//funcion a llamar, tamaño index, bytes a leer (se suman 3 byts por posicion)
//funcion 1 = slope_time_st
//funcion 2 = contstant_time_st
//funcion 3 = contstant_tempeture_stages_st
//funcion 4 = contstant_time_Fast
//funcion 5 = contstant_tempeture_stages_Fast
//funcion 6 = contstant_tempeture_stages_Fast
void update_preset_values(unsigned char *value){
  unsigned char value_to_archive = 0;
  int bytes_reads = 0;
  for(int index=0; index<=*(value++); index++){
    for(unsigned char *reading = value + (2 + index * 3); bytes_reads<=3; reading++){
      bytes_reads++;
      value_to_archive += *reading;
    }
    if (*value==1){
      slope_time_st[index] = (int)value_to_archive;
    }
    if (*value==2){
      contstant_time_st[index] = (int)value_to_archive;
    }
    if (*value==3){
      contstant_tempeture_stages_st[index] = (int)value_to_archive;
    }
    if (*value==4){
      contstant_time_Fast[index] = (int)value_to_archive;
    }
    if (*value==5){
      slope_time_Fast[index] = (int)value_to_archive;
    }
    if (*value==6){
      contstant_tempeture_stages_Fast[index] = (int)value_to_archive;
    }    
  }  
}


