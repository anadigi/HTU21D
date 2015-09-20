#ifndef _HTU21D_H
#define _HTU21D_H

#define I2C_ADDR 0x40
#define TRIG_T_HM 0xE3 //Trigeer Temperature measurement, hold master.
#define TRIG_H_HM 0xE5 //Trigger Humidity measurement, hold master.
#define TRIG_T_NHM 0xF3 //Trigger Temperature measurement, no hold master.
#define TRIG_H_NHM 0xF5 //Trigger Humidity measurement, no hold master.
#define W_USR_REG 0xE6 //Write user register.
#define R_USR_REG 0xE7 //Read user register.
#define SOFT_RST 0xFE //Perform soft reset.

double readHumidity(int file);
double readTemperature(int file);
double readDewPoint(int file);

#endif
