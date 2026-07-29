#ifndef ANALYSIS_H
#define ANALYSIS_H

#include "pzem.h"

enum
{
    MOTOR_STOPPED = 0,
    MOTOR_STARTING,
    MOTOR_IDLE,
    MOTOR_LOW_LOAD,
    MOTOR_NORMAL_LOAD,
    MOTOR_HIGH_LOAD,
    MOTOR_OVERLOAD,
    MOTOR_FAULT
};

struct AnalysisData
{
    // Promedios
    float voltageAvg;
    float currentAvg;
    float powerAvg;
    float pfAvg;

    // Totales
    float totalActivePower;
    float totalReactivePower;
    float totalApparentPower;

    // Balance
    float voltageUnbalance;
    float currentUnbalance;

    // Motor
    float loadPercent;

    // Estado
    bool running;
    bool overload;
    bool underload;
    bool phaseLoss;
    bool lowPF;
    bool overVoltage;
    bool underVoltage;

    // Estadísticas
    uint32_t starts;
    uint32_t runtimeSeconds;

    // Salud
    float health;
    uint8_t motorState;
    char lastEvent[40];
    uint32_t totalEvents;

    bool newEvent;

    float maxVoltage;
    float minVoltage;
    float maxCurrent;
    float maxPower;
    float maxApparentPower;
    float maxTemperature;

        // Estadísticas de operación

    uint32_t stoppedSeconds;
    uint32_t idleSeconds;
    uint32_t lowLoadSeconds;
    uint32_t normalLoadSeconds;
    uint32_t highLoadSeconds;
    uint32_t overloadSeconds;

    float utilization;
};

class Analysis
{
public:

    void begin();

    void update(const PZEMData &m);

    AnalysisData data;

private:

    bool lastRunning=false;

    unsigned long lastMillis=0;
};

extern Analysis analysis;

#endif