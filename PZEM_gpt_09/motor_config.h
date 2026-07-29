#ifndef MOTOR_CONFIG_H
#define MOTOR_CONFIG_H

//--------------------------------------------------
// DATOS DEL MOTOR
//--------------------------------------------------

#define MOTOR_NAME             "Motor Principal"

#define MOTOR_HP               40.0f

#define MOTOR_EFFICIENCY       0.92f

#define MOTOR_NOMINAL_PF       0.88f

#define MOTOR_SERVICE_FACTOR   1.15f

#define MOTOR_VOLTAGE          220.0f

#define MOTOR_FREQUENCY        60.0f

//--------------------------------------------------
// ALARMAS
//--------------------------------------------------

#define WARNING_PF             0.80f

#define WARNING_UNBALANCE      3.0f

#define WARNING_LOAD           85.0f

#define ALARM_LOAD             100.0f

#define ALARM_VOLTAGE_HIGH     255.0f

#define ALARM_VOLTAGE_LOW      210.0f

#endif