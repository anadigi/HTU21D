#include <stdio.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	printf("**** TEMPERATURE & HUMIDITY SENSOR TEST PROGRAM ****\n");

	int file;
	char *filename = "/dev/i2c-1";
	int I2C_ADDR = 0x40;
	unsigned char TRIG_T_HM = 0xE3; //Trigeer Temperature measurement, hold master.
	unsigned char TRIG_H_HM = 0xE5; //Trigger Humidity measurement, hold master.
	unsigned char TRIG_T_NHM = 0xF3; //Trigger Temperature measurement, no hold master.
	unsigned char TRIG_H_NHM = 0xF5; //Trigger Humidity measurement, no hold master.
	unsigned char W_USR_REG = 0xE6; //Write user register.
	unsigned char R_USR_REG = 0xE7; //Read user register.
	unsigned char SOFT_RST = 0xFE; //Perform soft reset.
	char buf[5];

	file = open(filename, O_RDWR);
	if(file<0)
	{
		perror("HTU21D:Fail opening i2c device file");
		exit(EXIT_FAILURE);
	}

	if((ioctl(file, I2C_SLAVE, I2C_ADDR)) < 0)
	{
		perror("HTU21D:Fail selecting i2c device");
	}

	if(i2c_smbus_write_byte(file, TRIG_T_NHM) < 0)
	{
		perror("HUT21D:smbus write failed");
	}
	usleep(70000);
	if(read(file, buf,3) <0)
	{
		perror("HUT21D:reading results failed");
	}

	//printf("Buffer 0: %x\n", buf[0]);
	//printf("Buffer 1: %x\n", buf[1]);
	//printf("Buffer 2: %x\n", buf[2]);

	int luku = (buf[0] << 8) | (buf[1] & 0xFC);
	//printf("luku: %d\n", luku);
	float temp;
	temp = -46.85 + 175.72 * ((float)luku/65536);
	printf("Temperature: %.1f C\n", temp);

	if(i2c_smbus_write_byte(file, TRIG_H_NHM) < 0)
        {
                perror("HUT21D:smbus write failed");
        }
	usleep(70000);
        if(read(file, buf,3) <0)
	{
                perror("HUT21D:reading results failed");
        }
        //printf("Buffer 0: %x\n", buf[0]);
        //printf("Buffer 1: %x\n", buf[1]);
        //printf("Buffer 2: %x\n", buf[2]);

	float humi;
	int humid = (buf[0] << 8) | (buf[1] & 0xFC);
	humi = -6 + 125 * (float)humid/65536;
	printf("Humidity: %.1f %%\n", humi);

	close(file);
	return(EXIT_SUCCESS);
}
