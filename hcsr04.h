#define PULSE_US	11

void init_ulsosensor(void);		//	Initializes input capture and trigger pin used by ultrasonic sensor
uint16_t getDist(void);				//	Returns measured distance in Centimeters

