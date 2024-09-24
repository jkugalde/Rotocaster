
#define potpin A0
#define transpin 3

void setup(){

pinMode(potpin,INPUT);
pinMode(transpin,OUTPUT);

}

void loop(){

analogWrite(transpin,map(0,1023,0,255,analogRead(potpin)));

}
