#include "Arduino.h"

#include "Display.h"


void setup(){
    Serial.begin(9600);
    Display display;
    float temperature;
    float setTemperature;
    Status status;
    unsigned long timer;
    bool cooking;
    unsigned long secondCounter = millis();

    delay(3000);
    setTemperature = 52.7; 
    timer = 150;
    status = STAT_SET_TEMP;


    while (true){
        for (int i = 0; i < 1300; i++){
            //setTemp
            display.setSetTemp(setTemperature);

            //time
            unsigned long seconds;
            if (seconds != 0){
                seconds = timer - millis() / 1000;
            }
            display.setTime(seconds);

            //temp
            temperature = i / 10.;
            display.setTemp(temperature);

            //status
            if (setTemperature > 0 && temperature - 1 < setTemperature){
                status = STAT_HEATING;
            }
            if (cooking){
                status = STAT_COOKING;
            }
            if (temperature - 1 < setTemperature &&
                    temperature + 1 > setTemperature){
                status = STAT_READY;
            }
            if (temperature - 1 > setTemperature && !cooking){
                status = STAT_TOO_HOT;
            }
            if (cooking && seconds == 0){
                status = STAT_DONE;
            }
            display.setStatus(status);

            //update
            display.update();

            delay(100);
        }
    }
}

void loop(){
}