/* frequency: the time between periods
 * keep an array of the last three values read
 * if we see the patter low, high, low then we are at the peak
 * the current time from the past peak is the frequency
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>

#include <time.h>

#include <wiringPi.h>
#include <wiringPiSPI.h>

#define	TRUE	(1==1)
#define	FALSE	(!TRUE)
#define CHAN_CONFIG_SINGLE 8
#define CHAN_CONFIG_DIFF 0

static int myFd ;

int seen_value = 0;
int min = 100000;
int max = 0;

char *usage = "Usage: mcp3008 all|analogChannel[1-8] [-l] [-ce1] [-d]";
// -l = load SPI driver,default: do not load
// -ce1= spi analogChannel 1, default:0
// -d = differential analogChannel input, default: single ended
void loadSpiDriver()
{
	if (system("gpio load spi") == -1)
	{
		fprintf (stderr, "Can't load the SPI driver: %s\n", strerror (errno)) ;
		exit (EXIT_FAILURE) ;
	}
}

void spiSetup (int spiChannel)
{
	if ((myFd = wiringPiSPISetup (spiChannel, 1000000)) < 0)
	{
		fprintf (stderr, "Can't open the SPI bus: %s\n", strerror (errno)) ;
		exit (EXIT_FAILURE) ;
	}
}

int myAnalogRead(int spiChannel,int channelConfig,int analogChannel)
{
	if(analogChannel<0 || analogChannel>7)
		return -1;
	unsigned char buffer[3] = {1}; // start bit

	//change this (shift bits according to mcp3002)
	//original buffer[1] = (channelConfig+analogChannel) << 4;
	buffer[1] = (channelConfig+analogChannel) << 4;
	wiringPiSPIDataRW(spiChannel, buffer, 3);
	//original return ( (buffer[1] & 3 ) << 8 ) + buffer[2]; // get last 10 bits
	printf("Buffer: %d %d %d \n", buffer[0], buffer[1], buffer[2]);
	return ( (buffer[0] & 3 ) << 8) + buffer[1]; // get last 10 bits
}

int main (int argc, char *argv [])
{
	int loadSpi=FALSE;
	int analogChannel=0;
	int spiChannel=0;
	int channelConfig=CHAN_CONFIG_SINGLE;
	if (argc < 2)
	{
		fprintf (stderr, "%s\n", usage) ;
		return 1 ;
	}
	if((strcasecmp (argv [1], "all") == 0) )
		argv[1] = "0";
	if ( (sscanf (argv[1], "%i", &analogChannel)!=1) || analogChannel < 0 || analogChannel > 8 )
	{
		printf ("%s\n",usage) ;
		return 1 ;
	}
	int i;
	for(i=2; i<argc; i++)
	{
		if (strcasecmp (argv [i], "-l") == 0 || strcasecmp (argv [i], "-load") == 0)
			loadSpi=TRUE;
		else if (strcasecmp (argv [i], "-ce1") == 0)
			spiChannel=1;
		else if (strcasecmp (argv [i], "-d") == 0 || strcasecmp (argv [i], "-diff") == 0)
			channelConfig=CHAN_CONFIG_DIFF;
	}
	
	if(loadSpi==TRUE)
		loadSpiDriver();

	wiringPiSetup () ;

	spiSetup(spiChannel);
	
	if(analogChannel>0)
	{
		clock_t start, end;
		start = time(NULL);
		double elapsed;
		int terminate = 1;
		while(terminate) {
			int ret = myAnalogRead(spiChannel, channelConfig, analogChannel-1);
			printf("%d\n", ret);
			
			if(ret > max) max = ret;
			if(ret < min) min = ret;

			printf("%d & %d\n", ret, seen_value);

			if(ret == seen_value && ret != 0) {
				printf("Ret is %d and seen is %d\n", ret, seen_value);
				printf(" Max: %d \n Min: %d\n Difference: %d\n", max, min, max - min);
				exit(1); 	
			}

			if(seen_value == 0) seen_value = ret;

			end = time(NULL);

			elapsed = difftime(end, start);

			if(elapsed >= 1.0) terminate = 0;
			
		}

		printf(" Max: %d \n Min: %d\n Difference: %d\n", max, min, max - min);

	}

	close (myFd) ;
	return 0;
}
