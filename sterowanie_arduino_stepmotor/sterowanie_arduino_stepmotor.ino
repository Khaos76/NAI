    /*
     * W oparciu o:
     * Adafruit Arduino - Lesson 16. Stepper
    */
    #include <I2C.h>
    #include <Stepper.h>

    #define    LIDARLite_ADDRESS   0x62          // Default I2C Address of LIDAR-Lite.
    #define    RegisterMeasure     0x00          // Register to write to initiate ranging.  
    #define    MeasureValue        0x04          // Value to initiate ranging.
    #define    RegisterHighLowB    0x8f          // Register to get both High and Low bytes in 1 call.
     
    int in1Pin = 12;
    int in2Pin = 11;
    int in3Pin = 10;
    int in4Pin = 9;
    char inbyte = 'n';
    int steps = 0;
     
    Stepper motor(512, in1Pin, in2Pin, in3Pin, in4Pin);  
     
    void setup()
    {
      pinMode(in1Pin, OUTPUT);
      pinMode(in2Pin, OUTPUT);
      pinMode(in3Pin, OUTPUT);
      pinMode(in4Pin, OUTPUT);
     
      
      Serial.begin(9600);
      motor.setSpeed(20);
      I2c.begin(); 
      delay(100); 
      I2c.timeOut(50); 
      
    }
     
    void loop()
    {
      if (Serial.available())
      {
        inbyte = Serial.read();
        Serial.println(inbyte);
        
        if (inbyte=='l'){steps=-5;}
        else if (inbyte=='r') {steps = +5;}
        else {steps = 0;}

        motor.step(steps);
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
    }
