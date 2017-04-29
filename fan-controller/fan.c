/*
*  Fan PWM controller
*
*  Uses temperature sensor: /sys/class/thermal/thermal_zone1/temp
*
*  compile: gcc fan.c -lwiringPi -o fan
*  run: sudo ./fan
*/

#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

FILE *fp;

#define BUFFER_SIZE 6

char buffer[BUFFER_SIZE];

int pwm_clock_frequency = 4420;

int cpu_temperature = 0;
int speed = 0;
int current_speed = 0;

void set_fan_speed(int duty){
	// speed is percentage between 0 and 100


	if (duty >= 100){
		speed = pwm_clock_frequency;
	}
	else if (duty <= 0){
		speed = 0;
	}
	else{
		speed = (pwm_clock_frequency / 100 * duty);
	}
	if (current_speed == 0 && duty != 0){ // give a short pulse at 100% duty cycle to get the fan spinning
		asus_pwm_start(3,0,pwm_clock_frequency,pwm_clock_frequency);
		usleep(100000); // sleep for 100ms
	}
	asus_pwm_start(3,0,pwm_clock_frequency,speed);
	current_speed = duty;
}

int main (void)
{
	wiringPiSetupGpio();
	pinMode (239, PWM_OUTPUT) ;

	while(1)
	{
		cpu_temperature = 0;

		fp = popen("cat /sys/class/thermal/thermal_zone1/temp", "r");
		if (fp != NULL)
		{
		    while (fgets(buffer, BUFFER_SIZE, fp) != NULL)
		        //printf("%s", buffer);
		    pclose(fp);

		    cpu_temperature = atoi(buffer);
		}

		//printf("temperature: %d\n", cpu_temperature); // display the cpu temperature

		if (cpu_temperature < 40000){
			set_fan_speed(0);
		}
		else if (cpu_temperature >= 40000 && cpu_temperature < 45000){
			set_fan_speed(30);
		}
		else if (cpu_temperature >= 45000 && cpu_temperature < 50000){
			set_fan_speed(50);
		}
		else if (cpu_temperature >= 50000 && cpu_temperature < 55000){
			set_fan_speed(70);
		}
		else if (cpu_temperature >= 55000 && cpu_temperature < 60000){
			set_fan_speed(90);
		}
		else if (cpu_temperature >= 60000){
			set_fan_speed(100);
		}
		else{
			set_fan_speed(100);
		}

		sleep(5);

	}

}
