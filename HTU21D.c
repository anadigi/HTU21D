#include <stdio.h>
#include <linux/i2c-dev.h>
#include <fcntl.h>
#include <stdlib.h>
#include <getopt.h>
#include <math.h>
#include "HTU21D.h"

static const char *short_options = "htdo:";
static const struct option long_options[] = {
	{"humidity", no_argument, NULL, 'h'},
	{"temperature", no_argument, NULL, 't'},
	{"dewpoint", no_argument, NULL, 'd'},
	{"output", required_argument, NULL, 'o'},
	{NULL, no_argument, NULL, 0}
};

typedef struct {
	int temperature;
	int humidity;
	int dewpoint;
	int output;
	char *path;
} option_struct;

int main(int argc, char **argv)
{
	int next_option;
	option_struct own_options;
	own_options.temperature = 0;
	own_options.humidity = 0;
	own_options.dewpoint = 0;
	own_options.output = 0;

	do {
		next_option = getopt_long(argc, argv, short_options, long_options, NULL);

		switch (next_option)
		{
			case 'h':
			own_options.humidity = 1;
			break;

			case 't':
			own_options.temperature = 1;
			break;

			case 'd':
                        own_options.dewpoint = 1;
                        break;

			case 'o':
			own_options.output = 1;
			own_options.path = optarg;
			break;

			case '?':
			printf("To be updated...\n");
			break;

			case -1:
			break;

			default:
			printf("To be updated...\n");
			break;
		}
	} while (next_option != -1);

	if(!own_options.output)
		printf("**** TEMPERATURE & HUMIDITY SENSOR TEST PROGRAM ****\n");

	int file;
	char *filename = "/dev/i2c-1";
	char buf[5];

	file = open(filename, O_RDWR);
	if(file<0)
	{
		perror("HTU21D:Fail opening i2c device file");
		close(file);
		exit(EXIT_FAILURE);
	}

	if((ioctl(file, I2C_SLAVE, I2C_ADDR)) < 0)
	{
		perror("HTU21D:Fail selecting i2c device");
		close(file);
                exit(EXIT_FAILURE);
	}

	if(own_options.temperature) {
		if(own_options.output) {
			WriteToFile(own_options.path, "temperature.txt", readTemperature(file));
		}
		else {
			printf("Temperature: %.2f C\n", readTemperature(file));
		}
	}

	if(own_options.humidity) {
		if(own_options.output) {
                        WriteToFile(own_options.path, "humidity.txt", readHumidity(file));
                }
                else {
                        printf("Relative Humidity: %.2f %%\n", readHumidity(file));
                }
	}

	if(own_options.dewpoint) {
		if(own_options.output) {
                        WriteToFile(own_options.path, "dewpoint.txt", readDewPoint(file));
                }
                else {
                        printf("Dew Point: %.2f C\n", readDewPoint(file));
                }
	}

	close(file);

	return(EXIT_SUCCESS);
}

double readTemperature(int file) {

	char buf[5];

	if(i2c_smbus_write_byte(file, TRIG_T_NHM) < 0)
        {
                perror("HUT21D:smbus write failed");
                close(file);
                exit(EXIT_FAILURE);
        }
        usleep(70000);
        if(read(file, buf,3) <0)
        {
                perror("HUT21D:reading results failed");
                close(file);
                exit(EXIT_FAILURE);
        }

        //printf("Buffer 0: %x\n", buf[0]);
        //printf("Buffer 1: %x\n", buf[1]);
        //printf("Buffer 2: %x\n", buf[2]);

        int luku = (buf[0] << 8) | (buf[1] & 0xFC);
        //printf("luku: %d\n", luku);
        double temp;
        temp = -46.85 + 175.72 * ((double)luku/65536);
	//printf("Temperature: %.1f C\n", temp);

	return(temp);
}


double readHumidity(int file) {

	char buf[5];

	if(i2c_smbus_write_byte(file, TRIG_H_NHM) < 0)
        {
                perror("HUT21D:smbus write failed");
                close(file);
                exit(EXIT_FAILURE);
        }
        usleep(70000);
        if(read(file, buf,3) <0)
        {
                perror("HUT21D:reading results failed");
                close(file);
                exit(EXIT_FAILURE);
        }
        //printf("Buffer 0: %x\n", buf[0]);
        //printf("Buffer 1: %x\n", buf[1]);
        //printf("Buffer 2: %x\n", buf[2]);

        double humi;
        int humid = (buf[0] << 8) | (buf[1] & 0xFC);
        humi = -6 + 125 * (double)humid/65536;
        //printf("Humidity: %.1f %%\n", humi);

	return humi;
}

double readDewPoint(int file) {

	double A = 8.1332;
	double B = 1762.39;
	double C = 235.66;
	double dewpoint;
	double temp = readTemperature(file);
	double humi = readHumidity(file);

	//CALCULATE PARTIAL PRESSURE
	double exponent = (A - (B / (temp + C)));
	double PPtamb = pow(10, exponent);
	dewpoint = - ((B / (log10(humi * (PPtamb / 100)) - A)) + C);

	//printf("DewPoint %f C\n", dewpoint);
	return dewpoint;
}
