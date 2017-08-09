#pragma once

#include <TFT_HX8357.h>
#include <RingBufCPP.h>

enum Status{
    STAT_SET_TEMP,
    STAT_DONE,
    STAT_HEATING,
    STAT_TOO_HOT,
    STAT_READY,
    STAT_COOKING
};

struct GraphPoint{
    unsigned long time;
    float temp;
};

class Display{
private:
    TFT_HX8357 display;
    float temperature;
    float setTemperature;
    Status status;
    unsigned long seconds;
    RingBufCPP<GraphPoint, 410> tempHist;
    bool updateTemp;
    bool updateSetTemp;
    bool updateStat;
    bool updateTime;
    bool updateGraph;

    void drawBackground();

    void drawThermometer(int x, int y);
    void drawTemperature(int x, int y);
    void drawTemperatureText(int x, int y);
    void drawSetTemperatureText(int x, int y);
    
    void drawStatus(int x, int y);

    void drawTimer(int x, int y);
    void drawTimerText(int x, int y);

    void drawGraphAxes(int x, int y);
    void drawGraph(int x, int y);

public:
    Display();
    void setTemp(float temperature);
    void setSetTemp(float setTemperature);
    void setStatus(Status status);
    void setTime(unsigned long seconds);

    void update();
};