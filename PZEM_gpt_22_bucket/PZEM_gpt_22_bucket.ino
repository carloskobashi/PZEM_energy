#include "config.h"
#include "pzem.h"
#include "wifi_manager.h"
#include "influx_manager.h"
#include "system_monitor.h"
#include "analysis.h"

PZEM pzem;
PZEMData meter;
SystemMonitor systemMonitor;
SystemData systemData;

HardwareSerial RS485Serial(1);

uint32_t lastRead = 0;

//======================================================

void printMeter()
{
    Serial.println();
    Serial.println("============================================================");
    Serial.println("                 PZEM-6L24");
    Serial.println("============================================================");

    Serial.printf("Estado : %s\n", meter.online ? "ONLINE" : "OFFLINE");

    //--------------------------------------------------------
    Serial.println("\n------------------- VOLTAJES -------------------");

    Serial.printf("VA : %8.1f V\n", meter.voltageA);
    Serial.printf("VB : %8.1f V\n", meter.voltageB);
    Serial.printf("VC : %8.1f V\n", meter.voltageC);

    //--------------------------------------------------------
    Serial.println("\n------------------ CORRIENTES ------------------");

    Serial.printf("IA : %8.3f A\n", meter.currentA);
    Serial.printf("IB : %8.3f A\n", meter.currentB);
    Serial.printf("IC : %8.3f A\n", meter.currentC);

    //--------------------------------------------------------
    Serial.println("\n----------------- FRECUENCIAS ------------------");

    Serial.printf("FA : %8.2f Hz\n", meter.frequencyA);
    Serial.printf("FB : %8.2f Hz\n", meter.frequencyB);
    Serial.printf("FC : %8.2f Hz\n", meter.frequencyC);

    //--------------------------------------------------------
    Serial.println("\n-------------- ANGULO VOLTAJE ------------------");

    Serial.printf("VB : %8.2f °\n", meter.voltageAngleB);
    Serial.printf("VC : %8.2f °\n", meter.voltageAngleC);

    //--------------------------------------------------------
    Serial.println("\n------------- ANGULO CORRIENTE -----------------");

    Serial.printf("IA : %8.2f °\n", meter.currentAngleA);
    Serial.printf("IB : %8.2f °\n", meter.currentAngleB);
    Serial.printf("IC : %8.2f °\n", meter.currentAngleC);

    //--------------------------------------------------------
    Serial.println("\n------------- POTENCIA ACTIVA ------------------");

    Serial.printf("PA : %8.1f W\n", meter.activePowerA);
    Serial.printf("PB : %8.1f W\n", meter.activePowerB);
    Serial.printf("PC : %8.1f W\n", meter.activePowerC);
    Serial.printf("PT : %8.1f W\n", meter.activePowerTotal);

    //--------------------------------------------------------
    Serial.println("\n------------ POTENCIA REACTIVA -----------------");

    Serial.printf("QA : %8.1f var\n", meter.reactivePowerA);
    Serial.printf("QB : %8.1f var\n", meter.reactivePowerB);
    Serial.printf("QC : %8.1f var\n", meter.reactivePowerC);
    Serial.printf("QT : %8.1f var\n", meter.reactivePowerTotal);

    //--------------------------------------------------------
    Serial.println("\n------------ POTENCIA APARENTE -----------------");

    Serial.printf("SA : %8.1f VA\n", meter.apparentPowerA);
    Serial.printf("SB : %8.1f VA\n", meter.apparentPowerB);
    Serial.printf("SC : %8.1f VA\n", meter.apparentPowerC);
    Serial.printf("ST : %8.1f VA\n", meter.apparentPowerTotal);

    //--------------------------------------------------------
    Serial.println("\n------------ FACTOR POTENCIA -------------------");

    Serial.printf("PFA : %8.2f\n", meter.powerFactorA);
    Serial.printf("PFB : %8.2f\n", meter.powerFactorB);
    Serial.printf("PFC : %8.2f\n", meter.powerFactorC);
    Serial.printf("PFT : %8.2f\n", meter.powerFactorTotal);

    //--------------------------------------------------------
    Serial.println("\n------------- ENERGIA ACTIVA -------------------");

    Serial.printf("EA : %8.1f kWh\n", meter.activeEnergyA);
    Serial.printf("EB : %8.1f kWh\n", meter.activeEnergyB);
    Serial.printf("EC : %8.1f kWh\n", meter.activeEnergyC);
    Serial.printf("ET : %8.1f kWh\n", meter.activeEnergyTotal);

    //--------------------------------------------------------
    Serial.println("\n------------ ENERGIA REACTIVA ------------------");

    Serial.printf("QEA : %8.1f kVarh\n", meter.reactiveEnergyA);
    Serial.printf("QEB : %8.1f kVarh\n", meter.reactiveEnergyB);
    Serial.printf("QEC : %8.1f kVarh\n", meter.reactiveEnergyC);
    Serial.printf("QET : %8.1f kVarh\n", meter.reactiveEnergyTotal);

    //--------------------------------------------------------
    Serial.println("\n------------ ENERGIA APARENTE ------------------");

    Serial.printf("SEA : %8.1f kVAh\n", meter.apparentEnergyA);
    Serial.printf("SEB : %8.1f kVAh\n", meter.apparentEnergyB);
    Serial.printf("SEC : %8.1f kVAh\n", meter.apparentEnergyC);
    Serial.printf("SET : %8.1f kVAh\n", meter.apparentEnergyTotal);


    Serial.println();

    Serial.println("------------ ANALISIS ----------------");

    Serial.printf("Voltaje Promedio     : %.1f V\n", analysis.data.voltageAvg);
    Serial.printf("Corriente Promedio   : %.2f A\n", analysis.data.currentAvg);

    Serial.printf("Potencia Total       : %.1f W\n", analysis.data.totalActivePower);

    Serial.printf("FP Promedio          : %.3f\n", analysis.data.pfAvg);

    Serial.printf("Desbalance Voltaje   : %.2f %%\n", analysis.data.voltageUnbalance);
    Serial.printf("Desbalance Corriente : %.2f %%\n", analysis.data.currentUnbalance);

    Serial.printf("Carga Motor          : %.1f %%\n", analysis.data.loadPercent);

    Serial.printf("Arranques            : %lu\n", analysis.data.starts);

    Serial.printf("Horas Operacion      : %.2f h\n",
                analysis.data.runtimeSeconds / 3600.0f);

    Serial.printf("Health Index         : %.1f %%\n",
                analysis.data.health);

    Serial.println();

    Serial.printf("Running      : %s\n",
                analysis.data.running ? "SI" : "NO");

    Serial.printf("Overload     : %s\n",
                analysis.data.overload ? "SI" : "NO");

    Serial.printf("Underload    : %s\n",
                analysis.data.underload ? "SI" : "NO");

    Serial.printf("Low PF       : %s\n",
                analysis.data.lowPF ? "SI" : "NO");

    Serial.printf("Phase Loss   : %s\n",
                analysis.data.phaseLoss ? "SI" : "NO");

    Serial.printf("Over Voltage : %s\n",
                analysis.data.overVoltage ? "SI" : "NO");

    Serial.printf("Under Voltage: %s\n",
                analysis.data.underVoltage ? "SI" : "NO");

                Serial.println();



Serial.printf("Estado Motor : ");

Serial.println();

Serial.println("----------- DIAGNOSTICO -----------");

Serial.printf("Estado       : %s\n",
              analysis.data.statusText);

Serial.printf("Severidad    : %u\n",
              analysis.data.severity);

Serial.printf("Diagnostico  : %s\n",
              analysis.data.diagnosticText);

Serial.printf("Accion       : %s\n",
              analysis.data.recommendationText);


switch(analysis.data.motorState)
{
    case MOTOR_STOPPED:
        Serial.println("DETENIDO");
        break;

    case MOTOR_STARTING:
        Serial.println("ARRANCANDO");
        break;

    case MOTOR_IDLE:
        Serial.println("VACIO");
        break;

    case MOTOR_LOW_LOAD:
        Serial.println("CARGA BAJA");
        break;

    case MOTOR_NORMAL_LOAD:
        Serial.println("CARGA NORMAL");
        break;

    case MOTOR_HIGH_LOAD:
        Serial.println("CARGA ALTA");
        break;

    case MOTOR_OVERLOAD:
        Serial.println("SOBRECARGA");
        break;

    case MOTOR_FAULT:
        Serial.println("FALLA");
        break;
}

    Serial.printf("Ultimo Evento : %s\n", analysis.data.lastEvent);
    Serial.printf("Eventos Totales : %lu\n", analysis.data.totalEvents);

    Serial.println();

    Serial.println();
    Serial.println("------------ TENDENCIAS ------------");

    Serial.printf("I promedio 1 min : %.2f A\n", analysis.data.currentTrend1m);
    Serial.printf("P promedio 1 min : %.1f W\n", analysis.data.powerTrend1m);
    Serial.printf("FP promedio 1 min: %.3f\n", analysis.data.pfTrend1m);

    Serial.printf("Tendencia I      : %d\n", analysis.data.currentTrendState);
    Serial.printf("Tendencia P      : %d\n", analysis.data.powerTrendState);
    Serial.printf("Tendencia FP     : %d\n", analysis.data.pfTrendState);

    Serial.println("----------- RECORDS -----------");

    Serial.printf("Max Voltaje : %.1f V\n", analysis.data.maxVoltage);
    Serial.printf("Min Voltaje : %.1f V\n", analysis.data.minVoltage);

    Serial.printf("Max Corriente : %.2f A\n", analysis.data.maxCurrent);

    Serial.printf("Max Potencia : %.1f W\n", analysis.data.maxPower);

    Serial.printf("Max Pot. Aparente : %.1f VA\n",
              analysis.data.maxApparentPower);

    Serial.println();

    
    Serial.println("------- ESTADISTICAS -------");

    Serial.printf("Tiempo Detenido   : %.2f h\n",
        analysis.data.stoppedSeconds / 3600.0f);

    Serial.printf("Tiempo Vacio      : %.2f h\n",
        analysis.data.idleSeconds / 3600.0f);

    Serial.printf("Tiempo Carga Baja : %.2f h\n",
        analysis.data.lowLoadSeconds / 3600.0f);

    Serial.printf("Tiempo Normal     : %.2f h\n",
        analysis.data.normalLoadSeconds / 3600.0f);

    Serial.printf("Tiempo Carga Alta : %.2f h\n",
        analysis.data.highLoadSeconds / 3600.0f);

    Serial.printf("Tiempo Sobrecarga : %.2f h\n",
        analysis.data.overloadSeconds / 3600.0f);

    Serial.printf("Utilizacion       : %.1f %%\n",
        analysis.data.utilization);
        
    //--------------------------------------------------------
    Serial.println("\n-------------- COMUNICACION --------------------");

    Serial.printf("CRC Errors : %lu\n", meter.crcErrors);
    Serial.printf("Timeouts   : %lu\n", meter.timeoutErrors);
    Serial.printf("Frames     : %lu\n", meter.frameErrors);

    Serial.println("============================================================");
}

//======================================================

void setup()
{
    Serial.begin(115200);

    delay(1000);

    Serial.println();
    Serial.println("========================================");
    Serial.println("PZEM-6L24 Monitor");
    Serial.println("ESP32-S3 Waveshare");
    Serial.println("========================================");

    if(!pzem.begin(
        &RS485Serial,
        MODBUS_BAUD,
        RS485_RX_PIN,
        RS485_TX_PIN,
        RS485_EN_PIN))
    {
        Serial.println("Error iniciando RS485");

        while(true);
    }
    analysis.begin();

    wifiBegin();

    influxBegin();
    systemMonitor.begin();

    Serial.println("Sistema iniciado");
}

//======================================================

void loop()
{
    wifiLoop();

    if (millis() - lastRead >= READ_INTERVAL)
    {
        uint32_t t0 = millis();

        lastRead = millis();

        //----------------------------------------
        // Leer PZEM
        //----------------------------------------

        if (pzem.read(meter))
        {
            //----------------------------------------
            // Actualizar estado del ESP32
            //----------------------------------------
            analysis.update(meter);

            systemMonitor.update(systemData);

            systemMonitor.setLoopTime(millis() - t0);

            

            //----------------------------------------
            // Mostrar en Serial
            //----------------------------------------

            printMeter();

            //----------------------------------------
            // Enviar a Influx
            //----------------------------------------

            if (wifiConnected())
            {
                if (influxSend(meter, systemData))
                {
                    Serial.println("Influx OK");
                }
                else
                {
                    systemMonitor.addInfluxError();

                    Serial.println("Error Influx");
                }
            }
        }
        else
        {
            Serial.println("No hay respuesta del PZEM");
        }
    }
}