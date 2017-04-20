#include <bcm2835.h>
#include <stdio.h>
#include <time.h>
#include <math.h>


int main(int argc, char **argv) {
	if (!bcm2835_init()) {
		return 1;
	}
	bcm2835_spi_begin();
	bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);      // The default
	bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);                   // The default
	bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_64);    // ~ 4 MHz
	bcm2835_spi_chipSelect(BCM2835_SPI_CS0);                      // The default
	bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);      // the default


	uint8_t mosi[10] = { 0x60, 0x00 };
	uint8_t miso[10] = { 0 };
	uint16_t value;
	int min = 1023;
	int max = 1;
	long ADCvoltage;
	long AC_amplitude;
	clock_t begin;
	double time_spent;
	unsigned int i;
	uint16_t voltages[3] = {0x00, 0x00, 0x00};
	int firstPeak = 0;
	clock_t firstPeakTime;
	clock_t secondPeakTime;
	double freq = 0.0;

	double freqArr[20];
	int z = 0;

	//FILE *file = fopen("output.txt","wb");

	begin = clock();
	for (i=0;1;i++){
		bcm2835_spi_transfernb(mosi, miso, 2);
		value =  miso[1] + ((miso[0] & 3) << 8);
		//initial three values
		if(i < 22) {
			voltages[i] = value;
		} else {
			//move our buffer over one, accept new voltage
			uint16_t first = voltages[1];
			uint16_t second = voltages[2];
			voltages[0] = first;
			voltages[1] = second;
			voltages[2] = value;
		}
		//if a peak
		if(value > (max - 2)) {
		  if(voltages[0] < voltages[1] && voltages[1] > voltages[2]) {
				//if this is the seconyd peak we've read
				if(firstPeak) {
					//compute freq
					freq = ((double)(clock() - firstPeakTime) / CLOCKS_PER_SEC);
					// fprintf(file, "Second peak: %d %d %d \n", voltages[0], voltages[1], voltages[2]);
					printf("Second peak: %d %d %d \n", voltages[0], voltages[1], voltages[2]);
					freqArr[z] = 1/freq;
					z++;
					// fprintf(file, "Frequency: %f\n", 1/freq);
					//if(1/freq < 80 && 1/freq > 40) 
					//printf("Frequency: %f\n", 1/freq);
					firstPeak = 0;
				} else {
					//mark the first peak and time
					firstPeak = 1;
					firstPeakTime = clock();
					// fprintf(file, "First peak: %d %d %d \n", voltages[0], voltages[1], voltages[2]);
					printf("First peak: %d %d %d \n", voltages[0], voltages[1], voltages[2]);
				}
			}
		}		
		if(value > max)	max = value;
		if(value < min)	min = value;

		time_spent = (double)(clock() - begin) / CLOCKS_PER_SEC;
		if (time_spent>=0.5) // 50 milliseconds
			break;
	}
	ADCvoltage = ((max - min) * 3300) / 1024;// in milliVolts
	AC_amplitude = ADCvoltage * 167535; //in milliVolts
	printf("Max: %d\n", max);
	printf(" ADC Max-Min: %dmV\n AC Amplitude: %dV\n AC RMS: %dV\n", ADCvoltage, (AC_amplitude/1000000),(AC_amplitude/1414235));
	bcm2835_spi_end();
	bcm2835_close();
	int y = 0;
	for(y = 0; y < z; y++)
		printf("%f\n", freqArr[y]);
	//fclose(file);
	return 0;
}


