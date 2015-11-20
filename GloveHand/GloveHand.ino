// GLOVE HAND
#include <Servo.h>

Servo servo;

const int flexPort = A0;
const int flexPort2 = A1;
const int servoPort = 4;

int flexValue;
int flexValue2;
const int arrayLength = 20;
unsigned int flexArray[arrayLength];
unsigned int flexArray2[arrayLength];
int angle;
int fval;
int fval2;

unsigned int n = 0;

void setup() {
	servo.attach(servoPort);
	Serial.begin(9600);
}

void loop() {
	flexValue = analogRead(flexPort);
	flexValue2 = analogRead(flexPort2);
	
	flexArray[n % arrayLength] = flexValue;
	flexArray2[n % arrayLength] = flexValue2;
	Serial.print("flexArray  ");
	for(int i = 0; i < arrayLength; i++) {
		Serial.print(flexArray[i]);
		Serial.print(" ");
	}
	Serial.println();
	Serial.print("flexArray2  ");
	for(int i = 0; i < arrayLength; i++) {
		Serial.print(flexArray2[i]);
		Serial.print(" ");
	}
	Serial.println();


	unsigned int avg = 0;
	unsigned int avg2 = 0;
	for(int i = 0; i < arrayLength; i++) {
		avg += flexArray[i];
		avg2 += flexArray2[i];
	}
	avg = avg / arrayLength;
	avg2 = avg2 / arrayLength;
	Serial.print("avg   ");
	Serial.println(avg);
	Serial.print("avg2  ");
	Serial.println(avg2);

	fval = map(avg, 750, 800, 90, 0);
	fval2 = map(avg2, 750, 800, 90, 0);
	fval = constrain(fval, 0, 90);
	fval2 = constrain	(fval2, 0, 90);
	
	Serial.print("fval  ");
	Serial.println(fval);
	Serial.print("fval2 ");
	Serial.println(fval2);
	
	angle = fval + fval2;
	servo.write(angle);

	Serial.println();
	delay(10);
	if(n < 1000) n++;
	else n = 0;
}
