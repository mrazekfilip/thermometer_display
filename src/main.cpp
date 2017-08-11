#include "Arduino.h"
#include <SPI.h>
#include <ClickEncoder.h>
#include <TimerOne.h>
#include <PID_v1.h>
#include "Adafruit_MAX31855.h"


#include "Display.h"


enum EncMode{
    ENC_NONE,
    ENC_TEMP,
    ENC_HOUR,
    ENC_MIN,
    ENC_SEC
};

ClickEncoder encoder(2, 3, 4);
void timerIsr() {
    encoder.service();
}



void setup(){
    Serial.begin(9600);

    double temperature;
    double setTemperature;
    Status status;
    unsigned long timer;
    bool cooking = false;
    unsigned long startCook = 0;
    ClickEncoder::Button button;
    EncMode encMode = ENC_NONE;
    unsigned long tempTimer;

    setTemperature = 0; 
    timer = 0;
    status = STAT_SET_TEMP;
    unsigned long seconds = -1;

    double heatPower = 0;
    double P = 0, I = 0, D = 0;
    Display display;

    encoder.setAccelerationEnabled(true);
    encoder.setDoubleClickEnabled(true);
    Timer1.initialize(1000);
    Timer1.attachInterrupt(timerIsr);

    Adafruit_MAX31855 thermometer(6, 7, 5);
    delay(500);
    tempTimer = millis();

    PID pid(&temperature, 
            &heatPower, 
            &setTemperature, 
            P, I, D, DIRECT);
    pid.SetMode(AUTOMATIC);
    pid.SetOutputLimits(0, 1);
    
    while (true){
        //set PID
        if (Serial.available() > 0){
            char key = Serial.read();
            switch(key){
                case 'q':
                    P += 0.1;
                    break;
                case 'a':
                    P -= +0.1;
                    break;
                case 'w':
                    I += 0.1;
                    break;
                case 's':
                    I -= 0.1;
                    break;
                case 'e':
                    D += 0.1;
                    break;
                case 'd':
                    D -= 0.1;
                    break;
            }
        }
        pid.SetTunings(P, I, D);

        //read status
        ClickEncoder::Button buttonStatus = encoder.getButton();
        if (buttonStatus != ClickEncoder::Open){
            button = buttonStatus;
        }

        //start cooking
        if (encMode == ENC_NONE && status == STAT_READY
                && button == ClickEncoder::Clicked
                && buttonStatus == ClickEncoder::Clicked){
            cooking = true;
            startCook = millis() / 1000;
            Serial.println(startCook);
            seconds = timer;
        }

        if (encMode == ENC_NONE && status == STAT_DONE
                && button == ClickEncoder::Clicked
                && buttonStatus == ClickEncoder::Clicked) {
            cooking = false;
            status = STAT_SET_TEMP;
            seconds = -1;
        }

        //set what
        if (encMode == ENC_NONE 
                && button == ClickEncoder::Released){
            encMode = ENC_TEMP;
        } else if (encMode == ENC_TEMP
                && button == ClickEncoder::Clicked
                && buttonStatus == ClickEncoder::Clicked) {
            encMode = ENC_HOUR;
        } else if (encMode == ENC_HOUR
                && button == ClickEncoder::Clicked
                && buttonStatus == ClickEncoder::Clicked) {
            encMode = ENC_MIN;
        } else if (encMode == ENC_MIN
                && button == ClickEncoder::Clicked
                && buttonStatus == ClickEncoder::Clicked) {
            encMode = ENC_SEC;
        } else if (encMode == ENC_SEC
                && button == ClickEncoder::Clicked
                && buttonStatus == ClickEncoder::Clicked) {
            encMode = ENC_NONE;
        }

        //set values
        switch(encMode){
            case ENC_NONE: 
                break;
            case ENC_TEMP:
                setTemperature += encoder.getValue() / 10.;
                break;
            case ENC_HOUR:
                timer += encoder.getValue() * 3600;
                break;
            case ENC_MIN:
                timer += encoder.getValue() * 60;
                break;
            case ENC_SEC:
                timer += encoder.getValue();
                break;
        }
        if (timer >= 360000){
            timer = 0;
        }

        //setTemp
        display.setSetTemp(setTemperature);

        //time
        if (seconds == 0){
            timer = 0;
        }
        else if (seconds > 0 && cooking){
            seconds = timer + startCook - millis() / 1000;
            display.setTime(seconds);
        } else {
            display.setTime(timer);
        }

        //temp
        if (millis() - tempTimer > 1000){
            tempTimer = millis();
            temperature = thermometer.readCelsius();
            pid.Compute();

            Serial.print(temperature);
            Serial.print(" ");
            Serial.print(setTemperature);
            Serial.print(" ");
            Serial.print(heatPower, 5);
            Serial.print(" ");
            Serial.print(P);
            Serial.print(" ");
            Serial.print(I);
            Serial.print(" ");
            Serial.print(D);
            Serial.print('\n');
        }
        display.setTemp(temperature);

        //status
        if (setTemperature > 0 && temperature - 1 < setTemperature){
            status = STAT_HEATING;
        }
        if (temperature - 1 < setTemperature &&
            temperature + 1 > setTemperature){
            status = STAT_READY;
        }
        if (temperature - 1 > setTemperature && !cooking){
            status = STAT_TOO_HOT;
        }
        if (cooking){
            status = STAT_COOKING;
        }
        if (cooking && seconds == 0){
            status = STAT_DONE;
        }
        display.setStatus(status);

        //update
        display.update();
    }
}

void loop(){
}