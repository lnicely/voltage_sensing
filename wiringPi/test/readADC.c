#include <wiringPi.h>
#include <stdio.h>

const int dataIn = 10;

int main() {
	wiringPiSetup();

	pinMode(dataIn, INPUT);

	int value = analogRead(dataIn);
	printf("%d\n", value);
}
