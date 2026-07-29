#include "config.h"
#include "pzem.h"
#include "wifi_manager.h"
#include "influx_manager.h"
#include "system_monitor.h"

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