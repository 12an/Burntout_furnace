#include "datas_model.h"


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
      SLOPE_TIME_ST[index] = (int)value_to_archive;
    }
    if (*value==2){
      CONSTANT_TIME_ST[index] = (int)value_to_archive;
    }
    if (*value==3){
      CONSTANT_TEMPETURE_ST[index] = (int)value_to_archive;
    }
    if (*value==4){
      CONSTANT_TIME_FAST[index] = (int)value_to_archive;
    }
    if (*value==5){
      SLOPE_TIME_FAST[index] = (int)value_to_archive;
    }
    if (*value==6){
      CONSTANT_TEMPETURE_FAST[index] = (int)value_to_archive;
    }    
  }  
}


