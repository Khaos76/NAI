#include <I2C.h>
#define    LIDARLite_ADDRESS   0x62          // Default I2C Address of LIDAR-Lite.
#define    RegisterMeasure     0x00          // Register to write to initiate ranging.
#define    MeasureValue        0x04          // Value to initiate ranging.
#define    RegisterHighLowB    0x8f          // Register to get both High and Low bytes in 1 call.


void setup(){
  Serial.begin(9600);      
  I2c.begin(); 
  delay(100); 
  I2c.timeOut(50); 
}

void loop(){
  // Write 0x04 to register 0x00
  uint8_t nackack = 100;    
  while (nackack != 0){ 
    nackack = I2c.write(LIDARLite_ADDRESS,RegisterMeasure, MeasureValue);
    delay(1); 
  }

  byte distanceArray[2]; 
  
  
  nackack = 100;  
  while (nackack != 0){
    nackack = I2c.read(LIDARLite_ADDRESS,RegisterHighLowB, 2, distanceArray);
    delay(1); 
  int distance = (distanceArray[0] << 8) + distanceArray[1];
  Serial.println(distance);
  }
}
