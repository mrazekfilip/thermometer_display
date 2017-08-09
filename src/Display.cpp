#include<TFT_HX8357.h>
#include<RingBufCPP.h>
#include<Display.h>

#define BG_COLOR 0x3186

Display::Display() :display(),
                    temperature(0),
                    setTemperature(0),
                    status(STAT_SET_TEMP),
                    seconds(0),
                    updateTemp(true),
                    updateSetTemp(true),
                    updateStat(true),
                    updateTime(true),
                    updateGraph(true) {
    display.begin();
    display.setRotation(3);
    drawBackground();
    update();
}

void Display::drawBackground(){
    display.fillScreen(0x0000);
    display.fillRect(0, 0, 480, 105, BG_COLOR);
    display.fillRect(0, 105, 480, 3, 0x528A);
    drawThermometer(0, 0);
    drawTimer(375, 0);
    drawGraphAxes(10, 120);
}

void Display::drawThermometer(int x, int y){
    display.fillCircle(30 + x, 80 + y, 16, 0xFFFF);
    display.fillRoundRect(20 + x, 10 + y, 20, 80, 9, 0xFFFF);
    display.fillCircle(30 + x, 80 + y, 12, BG_COLOR);
    display.fillRoundRect(24 + x, 14 + y, 12, 60, 5, BG_COLOR); 
    for (int i = 0; i < 4; i++){
        display.fillRect(x + 43, y + 27 + i * 10, 5, 3, 0xFFFF);
    }    
}

void Display::drawTemperature(int x, int y){
    int color;
    float slider = temperature;
    if (slider > 100){
        slider = 100;
    }
    if (slider < 40){
        color = 0x4158;
    } else if (slider < 70){
        color = 0x243B;
    } else {
        color = 0x371C;
    }

    display.fillRect(26 + x, y + 20, 
                    8, 
                    -20 + 80 - (slider / 100) * 60, 
                    BG_COLOR);
    display.fillCircle(30 + x, 80 + y, 10, color);
    display.fillRect(26 + x, 
                    80 - (slider / 100) * 60 + y,
                    8, 
                    (slider / 100) * 60, 
                    color);
}

void Display::drawTemperatureText(int x, int y){
    display.fillRect(x, y, 140, 40, BG_COLOR);
    display.setCursor(x, y);
    display.setTextColor(0xFFFF);
    display.setTextSize(5);
    if (temperature < 100){
        display.print(floor(temperature), 0);
        display.setTextSize(2);
        display.print(".");
        display.print(static_cast<int>((temperature * 10)) % 10);
    } else {
        display.print(temperature, 0);
    }
    display.setTextSize(2);
    display.print(" C");
}

void Display::drawSetTemperatureText(int x, int y){
    display.fillRect(x, y, 140, 20, BG_COLOR);
    display.setCursor(x, y);
    display.setTextColor(0xFFFF);
    display.setTextSize(2);
    display.print("SET: ");
    if (setTemperature < 100){
        display.print(setTemperature, 1);
    } else {
        display.print(setTemperature, 0);
    }
    display.print(" C");
}

void Display::drawStatus(int x, int y){
    int color;
    switch(status){
        case STAT_SET_TEMP:
            color = 0xB596;
            break;
        case STAT_HEATING:
        case STAT_TOO_HOT:
            color = 0x3638;
            break;
        case STAT_READY:
            color = 0x937A;
            break;
        case STAT_COOKING:
            color = 0xED92;
            break;
        case STAT_DONE:
            color = 0xEC9C;
            break;
    }
    display.fillRect(x, y, 120, 30, color);
    display.setTextSize(2);
    display.setTextColor(0x0000);
    switch(status){
        case STAT_SET_TEMP:
            display.setCursor(x + 12, y + 8);
            display.print("SET TEMP");
            break;
        case STAT_TOO_HOT:
            display.setCursor(x + 17, y + 8);
            display.print("TOO HOT");
            break;
        case STAT_HEATING:
            display.setCursor(x + 17, y + 8);
            display.print("HEATING");
            break;
        case STAT_READY:
            display.setCursor(x + 30, y + 8);
            display.print("READY");
            break;
        case STAT_COOKING:
            display.setCursor(x + 17, y + 8);
            display.print("COOKING");
            break;
        case STAT_DONE:
            display.setCursor(x + 30, y + 8);
            display.print("DONE!");
            break;
    }
}

void Display::drawTimer(int x, int y){
    display.fillCircle(x + 50, y + 53, 38, 0xFFFF);
    display.fillCircle(x + 50, y + 53, 33, BG_COLOR);
    display.fillRect(x + 48, y + 20, 5, 10, 0xFFFF);
    display.fillTriangle(x, y, 
                        x + 48, y, 
                        x + 48, y + 53, 
                        BG_COLOR);
    display.fillCircle(x + 50, y + 53, 4, 0xFFFF);
    display.fillTriangle(x + 52, y + 51,
                        x + 48, y + 55,
                        x + 32, y + 31,
                        0xFFFF);
}

void Display::drawTimerText(int x, int y){
    display.fillRect(x, y, 150, 30, BG_COLOR);
    display.setCursor(x, y);
    display.setTextColor(0xFFFF);
    display.setTextSize(3);
    int displayHours = seconds / 3600;
    int displayMinutes = (seconds - displayHours * 3600) / 60;
    int displaySeconds = seconds - displayHours * 3600 - displayMinutes * 60;

    if (displayHours < 10){
        display.print("0");
        display.print(displayHours);
    } else {
        display.print(displayHours);
    }
    display.print(":");
    if (displayMinutes < 10){
        display.print("0");
        display.print(displayMinutes);
    } else{
        display.print(displayMinutes);
    }
    display.print(":");
    if (displaySeconds < 10){
        display.print("0");
        display.print(displaySeconds);
    } else{
        display.print(displaySeconds);
    }
}

void Display::drawGraphAxes(int x, int y){
    display.fillRect(x, y, 460, 190, 0x0000);
    display.drawFastHLine(x + 30, y + 170, 410, 0xFFFF);
    display.drawFastVLine(x + 30, y + 20, 150, 0xFFFF);
    display.setTextColor(0xFFFF);
    display.setTextSize(1);
    for (int i = 0; i < 15; i++){
        display.drawFastHLine(x + 25, y + 20 + i * 10, 5, 0xFFFF);
        display.setCursor(x, y + 17 + i * 10);
        display.print(150 - i * 10);
    }
    for (int i = 0; i < 7; i++){
        display.drawFastVLine(x + 80 + i * 60, y + 170, 5, 0xFFFF);
        display.setCursor(x + 80 + i * 60, y + 180);
        display.print(-360 +60 * i);
    }
}

//TODO: drawGraph
void Display::drawGraph(int x, int y){
    display.fillRect(x, y, 410, 149, 0x000);
    display.drawFastHLine(x, y + 149 - setTemperature, 410, 0xD33D);
}

void Display::setTemp(float temperature){
    if (this->temperature == temperature)
        return;
    this->temperature = temperature;
    updateTemp = true;
}

void Display::setSetTemp(float setTemperature){
    if (this->setTemperature == setTemperature)
        return;
    this->setTemperature = setTemperature;
    updateSetTemp = true;
}

void Display::setStatus(Status status){
    if (this->status == status)
        return;
    this->status = status;
    updateStat = true;
}

void Display::setTime(unsigned long seconds){
    if (this->seconds == seconds)
        return;
    this->seconds = seconds;
    updateTime = true;
}

void Display::update(){
    if (updateTemp){
        drawTemperature(0, 0);
        drawTemperatureText(60, 20);
        updateTemp = false;
    }
    if (updateSetTemp){
        drawSetTemperatureText(60, 70);
        updateSetTemp = false;
    }
    if (updateStat){
        drawStatus(240, 55);
        updateStat = false;
    }
    if (updateTime){
        drawTimerText(230, 25);
        updateTime = false;
    }
}


