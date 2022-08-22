void stage_1_led(int message, int stage){
  if (message==1){
    digitalWrite(stage, HIGH);
  }
  if (message==0){
    digitalWrite(stage, LOW);
  }
  if (message==2){
    if (digitalRead(stage)){
      digitalWrite(stage, LOW);
      }
    else{
      digitalWrite(stage, HIGH);
      }
    }
}