//Mux control pins
int s0 = 8;
int s1 = 9;
int s2 = 10;
int s3 = 11;

int status[10]; //array to store Piezo inputs

//Mux in “SIG” pin
int SIG_pin = 2;

int readMux(int channel) {
    int controlPin[] = {s0,s1,s2,s3};
    int muxChannel[16][4] = {
        {0,0,0,0}, //channel 0
        {1,0,0,0}, //channel 1
        {0,1,0,0}, //channel 2
        {1,1,0,0}, //channel 3
        {0,0,1,0}, //channel 4
        {1,0,1,0}, //channel 5
        {0,1,1,0}, //channel 6
        {1,1,1,0}, //channel 7
        {0,0,0,1}, //channel 8
        {1,0,0,1}, //channel 9
        {0,1,0,1}, //channel 10
        {1,1,0,1}, //channel 11
        {0,0,1,1}, //channel 12
        {1,0,1,1}, //channel 13
        {0,1,1,1}, //channel 14
        {1,1,1,1} //channel 15
    };
    //loop through the 4 sig 
    for (int i = 0; i < 4; i++) {
        digitalWrite(controlPin[i], muxChannel[channel][i]);
    }
    //read the value at the SIG pin 
    int val = analogRead(SIG_pin);
    //return the value 
    return val;
}

void setup() {
    pinMode(s0, OUTPUT);
    pinMode(s1, OUTPUT);
    pinMode(s2, OUTPUT);
    pinMode(s3, OUTPUT);

    digitalWrite(s0, LOW);
    digitalWrite(s1, LOW);
    digitalWrite(s2, LOW);
    digitalWrite(s3, LOW);

    Serial.begin(9600);
}

void loop() {


    bool printing = false;//these print statements are useful for debugging, turn them off when sending serialized information to Max
    
    //Loop through and read all 16 values
    if(printing)
      Serial.println("0\t1\t2\t3\t4\t5\t6\t7\t8\t9");
    for(int i = 0; i < 10; i++){
      int reading = readMux(i);
      if(printing)
        Serial.print(reading);
      
      if (status[i] == 0 && reading > 100){ //100 is my threshold 'on value', set this anywhere from 0-1024
        status[i] = 1;
      } else if (status[i] == 1){ //status[i] = 1 or 2 is to prevent multiple on signals from the same input
        status[i] = 2;
      } else if (status[i] == 2 && reading < 5){//5 is my threshold 'off value'
        status[i] = 0;
      }
      if(printing){
        Serial.print(status[i]);
        Serial.print("\t");
      }
    }
    if(printing)
      Serial.println("");

    int printnum = 0;//printnum is the number sent to max
    for(int i = 0; i < 10; i++){
      if(status[i] == 0 || status[i] == 2){
        printnum = printnum << 1;//'write a 0' and shift left
      } else {
        printnum = printnum | 1;//'write a 1' and shift left
        printnum = printnum << 1;
      }
    }
    printnum = printnum >> 1;
    //Numbers larger than 128 represent the status of the left hand
    //Numbers smaller than 128 represent the status of the right hand
    Serial.write((printnum & 31) | 128);
    delay(5);
    Serial.write(printnum >> 5);
    delay(5);
