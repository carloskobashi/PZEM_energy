#include "analysis.h"
#include <math.h>
#include <string.h>
#include "motor_config.h"

Analysis analysis;

void Analysis::begin()
{
    lastMillis = millis();

    data.starts = 0;
    data.runtimeSeconds = 0;
    data.health = 100.0f;
    strcpy(data.lastEvent, "Sistema iniciado");
    data.totalEvents = 0;
    data.newEvent = true;
    data.maxVoltage = 0;
    data.minVoltage = 1000;
    data.maxCurrent = 0;
    data.maxPower = 0;
    data.maxApparentPower = 0;
    data.maxTemperature = 0;
    data.stoppedSeconds = 0;
    data.idleSeconds = 0;
    data.lowLoadSeconds = 0;
    data.normalLoadSeconds = 0;
    data.highLoadSeconds = 0;
    data.overloadSeconds = 0;
    data.utilization = 0;

}

static void setEvent(AnalysisData &d, const char *txt)
{
    strncpy(d.lastEvent, txt, sizeof(d.lastEvent) - 1);
    d.lastEvent[sizeof(d.lastEvent) - 1] = '\0';

    d.totalEvents++;

    d.newEvent = true;
}

void Analysis::update(const PZEMData &m)
{
    //==============================
    // PROMEDIOS
    //==============================

    data.voltageAvg =
        (m.voltageA +
         m.voltageB +
         m.voltageC) / 3.0f;

    data.currentAvg =
        (m.currentA +
         m.currentB +
         m.currentC) / 3.0f;

    data.pfAvg =
        (m.powerFactorA +
         m.powerFactorB +
         m.powerFactorC) / 3.0f;

    data.totalActivePower =
        fabs(m.activePowerA) +
        fabs(m.activePowerB) +
        fabs(m.activePowerC);

    data.totalReactivePower =
        fabs(m.reactivePowerA) +
        fabs(m.reactivePowerB) +
        fabs(m.reactivePowerC);

    data.totalApparentPower =
        m.apparentPowerA +
        m.apparentPowerB +
        m.apparentPowerC;

    data.powerAvg =
        data.totalActivePower / 3.0f;

    //==============================
    // DESBALANCE DE VOLTAJE
    //==============================

    float dv1 = fabs(m.voltageA - data.voltageAvg);
    float dv2 = fabs(m.voltageB - data.voltageAvg);
    float dv3 = fabs(m.voltageC - data.voltageAvg);

    float dv = dv1;

    if (dv2 > dv) dv = dv2;
    if (dv3 > dv) dv = dv3;

    data.voltageUnbalance =
        (dv / data.voltageAvg) * 100.0f;

    //==============================
    // DESBALANCE DE CORRIENTE
    //==============================

    if(data.currentAvg > 0.05f)
    {
        float di1 = fabs(m.currentA - data.currentAvg);
        float di2 = fabs(m.currentB - data.currentAvg);
        float di3 = fabs(m.currentC - data.currentAvg);

        float di = di1;

        if(di2 > di) di = di2;
        if(di3 > di) di = di3;

        data.currentUnbalance =
            (di / data.currentAvg) * 100.0f;
    }
    else
    {
        data.currentUnbalance = 0;
    }

        //=========================================
    // ESTADO DEL MOTOR
    //=========================================

    data.running = data.currentAvg > 0.50f;

    if(data.running && !lastRunning)
        data.starts++;

    lastRunning = data.running;

    unsigned long now = millis();

    if(data.running)
    {
        data.runtimeSeconds += (now - lastMillis) / 1000;
    }

    lastMillis = now;

    //=========================================
    // PORCENTAJE DE CARGA
    // Motor 40 HP
    //=========================================

    float ratedPower =
        MOTOR_HP * 746.0f / MOTOR_EFFICIENCY;

    float ratedPower =
        MOTOR_HP * 746.0f / MOTOR_EFF;

    if(ratedPower > 0)
        data.loadPercent =
            (data.totalApparentPower / ratedPower) * 100.0f;
    else
        data.loadPercent = 0;


        //----------------------------------
    // Estadísticas de tiempo
    //----------------------------------

    static unsigned long lastSecond = millis();

    if(millis() - lastSecond >= 1000)
    {
        lastSecond += 1000;

        switch(data.motorState)
        {
            case MOTOR_STOPPED:
                data.stoppedSeconds++;
                break;

            case MOTOR_IDLE:
                data.idleSeconds++;
                break;

            case MOTOR_LOW_LOAD:
                data.lowLoadSeconds++;
                break;

            case MOTOR_NORMAL_LOAD:
                data.normalLoadSeconds++;
                break;

            case MOTOR_HIGH_LOAD:
                data.highLoadSeconds++;
                break;

            case MOTOR_OVERLOAD:
                data.overloadSeconds++;
                break;
        }

        uint32_t total =
            data.stoppedSeconds +
            data.idleSeconds +
            data.lowLoadSeconds +
            data.normalLoadSeconds +
            data.highLoadSeconds +
            data.overloadSeconds;

        if(total > 0)
        {
            data.utilization =
                100.0f *
                (data.normalLoadSeconds +
                 data.highLoadSeconds +
                 data.overloadSeconds)
                / total;
        }
    }

    //=========================================
    // ALARMAS
    //=========================================

    data.overVoltage =
        (m.voltageA > ALARM_VOLTAGE_HIGH) ||
        (m.voltageB > ALARM_VOLTAGE_HIGH) ||
        (m.voltageC > ALARM_VOLTAGE_HIGH);

    data.underVoltage =
        (m.voltageA < ALARM_VOLTAGE_LOW) ||
        (m.voltageB < ALARM_VOLTAGE_LOW) ||
        (m.voltageC < ALARM_VOLTAGE_LOW);

    data.lowPF =
        (data.pfAvg < WARNING_PF);

    data.phaseLoss =
        (m.currentA < 0.20f) ||
        (m.currentB < 0.20f) ||
        (m.currentC < 0.20f);

    data.overload =
        data.loadPercent > ALARM_LOAD;

    data.underload =
        data.running &&
        data.loadPercent < 25.0f;

            //=========================================
    // HEALTH INDEX
    //=========================================

    data.health = 100.0f;

    data.health -= data.voltageUnbalance * 2.0f;
    data.health -= data.currentUnbalance * 1.5f;

    if(data.lowPF)
        data.health -= 10.0f;

    if(data.overVoltage)
        data.health -= 10.0f;

    if(data.underVoltage)
        data.health -= 10.0f;

    if(data.overload)
        data.health -= 20.0f;

    if(data.phaseLoss)
        data.health -= 40.0f;

    if(data.health < 0.0f)
        data.health = 0.0f;

    if(data.health > 100.0f)
        data.health = 100.0f;

    //=========================================
    // ESTADO DEL MOTOR
    //=========================================

    data.motorState = MOTOR_STOPPED;

    if(data.phaseLoss)
    {
        data.motorState = MOTOR_FAULT;
    }
    else if(!data.running)
    {
        data.motorState = MOTOR_STOPPED;
    }
    else if(data.runtimeSeconds < 5)
    {
        data.motorState = MOTOR_STARTING;
    }
    else if(data.loadPercent < 10)
    {
        data.motorState = MOTOR_IDLE;
    }
    else if(data.loadPercent < 40)
    {
        data.motorState = MOTOR_LOW_LOAD;
    }
    else if(data.loadPercent < 85)
    {
        data.motorState = MOTOR_NORMAL_LOAD;
    }
    else if(data.loadPercent < 100)
    {
        data.motorState = MOTOR_HIGH_LOAD;
    }
    else
    {
        data.motorState = MOTOR_OVERLOAD;
    }

static uint8_t lastState = 255;

if(lastState != data.motorState)
{
    lastState = data.motorState;

    switch(data.motorState)
    {
        case MOTOR_STOPPED:
            setEvent(data, "Motor detenido");
            break;

        case MOTOR_STARTING:
            setEvent(data, "Motor arrancando");
            break;

        case MOTOR_IDLE:
            setEvent(data, "Motor en vacio");
            break;

        case MOTOR_LOW_LOAD:
            setEvent(data, "Carga baja");
            break;

        case MOTOR_NORMAL_LOAD:
            setEvent(data, "Carga normal");
            break;

        case MOTOR_HIGH_LOAD:
            setEvent(data, "Carga alta");
            break;

        case MOTOR_OVERLOAD:
            setEvent(data, "Sobrecarga");
            break;

        case MOTOR_FAULT:
            setEvent(data, "Falla detectada");
            break;
    }
}

    //----------------------------------
    // MAXIMOS Y MINIMOS
    //----------------------------------

    if(data.voltageAvg > data.maxVoltage)
        data.maxVoltage = data.voltageAvg;

    if(data.voltageAvg < data.minVoltage)
        data.minVoltage = data.voltageAvg;

    if(data.currentAvg > data.maxCurrent)
        data.maxCurrent = data.currentAvg;

    if(data.totalActivePower > data.maxPower)
        data.maxPower = data.totalActivePower;

    if(data.totalApparentPower > data.maxApparentPower)
        data.maxApparentPower = data.totalApparentPower;
      
}