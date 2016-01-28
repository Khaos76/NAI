    /*
     * W oparciu o:
     * Adafruit Arduino - Lesson 16. Stepper
    */
     
    #include <Stepper.h>
     
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
    }
     
    void loop()
    {
      if (Serial.available())
      {
        inbyte = Serial.read();
        Serial.println(inbyte);
        
        if (inbyte=='l'){steps=-5;
        Serial.print("kupa");}
        else if (inbyte=='r') {steps = +5;}
        else {steps = 0;}

        motor.step(steps);
      }
    }
