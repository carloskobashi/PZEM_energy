#ifndef CONFIG_H
#define CONFIG_H

//======================================================
// WIFI
//======================================================

#define WIFI_SSID      "RPK"
#define WIFI_PASSWORD  "r3pl4st1k4"

//======================================================
// INFLUXDB
//======================================================

#define INFLUXDB_URL      "http://192.168.1.186:8086"
#define INFLUXDB_TOKEN    "hSXax3KFfpAP3xhgEsCN9LVsXVsM1ygr2JhpmJBp_JWlKnWxr5TRJgCFsZ5T5d6jYmbxo5_y6-sX0umSmD87Yw=="
#define INFLUXDB_ORG      "306068f2593dbbee"
#define INFLUXDB_BUCKET   "centrifuga"

//======================================================
// MODBUS / RS485
//======================================================

#define MODBUS_BAUD       9600
#define MODBUS_ID         0x01

// Pines ESP32-S3-RS485-CAN Waveshare
#define RS485_TX_PIN      17
#define RS485_RX_PIN      18
#define RS485_EN_PIN      21

//======================================================
// TIEMPOS
//======================================================

#define READ_INTERVAL     1000      // ms entre lecturas
#define MODBUS_TIMEOUT    300       // ms timeout respuesta


//======================================================

#endif