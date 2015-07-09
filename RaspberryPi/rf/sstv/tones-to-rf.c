/*

tones-to-rf.c

Purpose:
Proof of concept code for Raspberry Pi SSTV transmission
Takes a CSV file called test.csv containing tone(Hz),duration(mSec) pairs,
adds tone to an RF carrier generated on GPCLK0 for duration.

Jenny List, http://www.languagespy.com
Derived from freq_pi Copyright (c) Jan Panteltje 2013-always
 email: panteltje@yahoo.com
This code contains parts of code from Pifm.c 

Config: find the BCM2708_PERI_BASE definitions, and uncomment the one for your Raspberry Pi version
As supplied, this file has the Raspberry Pi 2 version uncommented.

You may wish to adjust the loop fudge factor to suit your SSTV timing when calculating duration,
 my figure came from trial and error. This is only a proof of concept, not polished and lovely code.

To compile:
gcc -Wall -O4 -o tones-to-rf tones-to-rf.c -std=gnu99 -lm

Start GPL license:
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <math.h>
#include <fcntl.h>
#include <assert.h>
#include <malloc.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <getopt.h>
#include <stdint.h>
#include <time.h>
#include <getopt.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>


#define PAGE_SIZE (4*1024)
#define BLOCK_SIZE (4*1024)


//Uncomment the relevant base address definition for your platform
//Base address definition for Pi 1
//#define BCM2708_PERI_BASE        0x20000000 
//Base address definition for Pi 2
#define BCM2708_PERI_BASE        0x3F000000


int  mem_fd;
char *gpio_mem, *gpio_map;
char *spi0_mem, *spi0_map;


// I/O access
volatile unsigned *gpio;
volatile unsigned *allof7e;

// GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x) or SET_GPIO_ALT(x,y)
#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpio+((g)/10)) |=  (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))

#define GPIO_SET *(gpio+7)  // sets   bits which are 1 ignores bits which are 0
#define GPIO_CLR *(gpio+10) // clears bits which are 1 ignores bits which are 0
#define GPIO_GET *(gpio+13)  // sets   bits which are 1 ignores bits which are 0

#define ACCESS(base) *(volatile int*)((int)allof7e+base-0x7e000000)
#define SETBIT(base, bit) ACCESS(base) |= 1<<bit
#define CLRBIT(base, bit) ACCESS(base) &= ~(1<<bit)

#define CM_GP0CTL (0x7e101070)
#define GPFSEL0 (0x7E200000)
#define CM_GP0DIV (0x7e101074)
#define CLKBASE (0x7E101000)
#define DMABASE (0x7E007000)
#define PWMBASE  (0x7e20C000) /* PWM controller */


struct GPCTL
{
char SRC         : 4;
char ENAB        : 1;
char KILL        : 1;
char             : 1;
char BUSY        : 1;
char FLIP        : 1;
char MASH        : 2;
unsigned int     : 13;
char PASSWD      : 8;
};



void getRealMemPage(void** vAddr, void** pAddr)
{
void* a = valloc(4096);
    
((int*)a)[0] = 1;  // use page to force allocation.
    
mlock(a, 4096);  // lock into ram.
    
*vAddr = a;  // yay - we know the virtual address
    
unsigned long long frameinfo;
    
int fp = open("/proc/self/pagemap", 'r');
lseek(fp, ((int)a)/4096*8, SEEK_SET);
read(fp, &frameinfo, sizeof(frameinfo));
    
*pAddr = (void*)((int)(frameinfo*4096));
}


void freeRealMemPage(void* vAddr)
{
munlock(vAddr, 4096);  // unlock ram.
    
free(vAddr);
}




uint32_t calculate_divider(uint32_t frequency)
{
	uint32_t ua;
	/* set frequency */
	//struct GPCTL setupword = {6 /* clock source */, 1 /* enable */,  0 /* not kill */, 0 , 0, 1 /* 1 stage MASH (equivalent to no MASH */, 0x5a /* password */ };
	//ACCESS(CM_GP0CTL) = *((int*)&setupword);
	//ACCESS(CM_GP0DIV) = (0x5a << 24) + 0x4d72 + m

	uint16_t divi; // integer part divider [23:12]        12 bits wide, max 4095
	uint16_t divf; // fractional part divider [11:0]    12 bits wide, max 4095

	/*
	clock sources are 650 MHz, 400 MHz, and 200 MHz
	So the lowest frequency we can make is 200,000,000 / 4095.4095 = 48,835.165323516 Hz 
	But I get 61,043 Hz
	4095.4095 * 61043 = 249,996,082.108499999 Hz....
	But for MASH 1,
	 MASH           min DIVI min output freq       average output freq             max output freq
	 0 (int divide) 1        source / ( DIVI )     source / ( DIVI )               source / ( DIVI )
	* 1              2        source / ( DIVI )     source / ( DIVI + DIVF / 1024 ) source / ( DIVI + 1 )
	 2              3        source / ( DIVI - 1 ) source / ( DIVI + DIVF / 1024 ) source / ( DIVI + 2 )
	 3              5        source / ( DIVI - 3 ) source / ( DIVI + DIVF / 1024 ) source / ( DIVI + 4 )

	200,000,000 / (4095 = 48840.048840048
	200,000,000 / (4095 + (4095/1024) ) = 48792.400011912
	So 
	61043  * 4099
	61043 * (4095 + .3999023437) = 249995496.238766479 Hz, looks liike we have a 250 MHz clock.
	Lowest frequency then is 61,043 Hz,
	highest frequency then is 250,000,000 / 1 = 250,000,000 Hz
	*/

	//#define PLL0_FREQUENCY 250000000.0
	#define PLL0_FREQUENCY 500000000.0

	/* calculate divider */
	double da; 

	da = PLL0_FREQUENCY / (double) frequency;

	divi = (int) da;
	divf = 4096.0 *(da - (double)divi);

	//fprintf(stderr, "frequency=%d da=%f divi=%d divf=%d\n", frequency, da, divi, divf);

	if( (divi > 4095.0) || (divi < 1.0) )
	    {
	    fprintf(stderr, "freq_pi: requested frequency out of range, aborting.\n");

	    exit(1);
	    }


	if(divf > 4095.0)
	    {
	    fprintf(stderr, "freq_pi: requested frequency out of range, aborting.\n");

	    exit(1);
	    }

	ua = (0x5a << 24) + (divi << 12) + divf;

	return ua;
}
/* end of calculate_divider */

void start_rf_output(int source)
{

printf("Starting RF\n");

	/* open /dev/mem */
	if ((mem_fd = open("/dev/mem", O_RDWR|O_SYNC) ) < 0)
	    {
	    printf("can't open /dev/mem \n");
	    exit (-1);
	    }
	    
	allof7e = (unsigned *)mmap( NULL, 0x01000000,  /*len */ PROT_READ|PROT_WRITE, MAP_SHARED, mem_fd, BCM2708_PERI_BASE   /* base */ );

	if ((int)allof7e==-1) exit(-1);

	SETBIT(GPFSEL0 , 14);
	CLRBIT(GPFSEL0 , 13);
	CLRBIT(GPFSEL0 , 12);

	/*
	Clock source
	0 = GND
	1 = oscillator
	2 = testdebug0
	3 = testdebug1
	4 = PLLA per
	5 = PLLC per
	6 = PLLD per
	7 = HDMI auxiliary
	8-15 = GND
	*/
	 
	struct GPCTL setupword = {source/*SRC*/, 1, 0, 0, 0, 1,0x5a};

	ACCESS(CM_GP0CTL) = *((int*)&setupword);
}

void stop_rf_output(int source)
{
printf("Stopping RF\n");

	/*
	Clock source
	0 = GND
	1 = oscillator
	2 = testdebug0
	3 = testdebug1
	4 = PLLA per
	5 = PLLC per
	6 = PLLD per
	7 = HDMI auxiliary
	8-15 = GND
	*/
	 
	struct GPCTL setupword = {source/*SRC*/, 0, 0, 0, 0, 0,0x5a};

	ACCESS(CM_GP0CTL) = *((int*)&setupword);
}

void modulate(int m)
{
ACCESS(CM_GP0DIV) = (0x5a << 24) + 0x4d72 + m;
}


struct CB
{
volatile unsigned int TI;
volatile unsigned int SOURCE_AD;
volatile unsigned int DEST_AD;
volatile unsigned int TXFR_LEN;
volatile unsigned int STRIDE;
volatile unsigned int NEXTCONBK;
volatile unsigned int RES1;
volatile unsigned int RES2;
};


struct DMAregs
{
volatile unsigned int CS;
volatile unsigned int CONBLK_AD;
volatile unsigned int TI;
volatile unsigned int SOURCE_AD;
volatile unsigned int DEST_AD;
volatile unsigned int TXFR_LEN;
volatile unsigned int STRIDE;
volatile unsigned int NEXTCONBK;
volatile unsigned int DEBUG;
};


struct PageInfo
{
void* p;  // physical address
void* v;   // virtual address
};


struct PageInfo constPage;   
struct PageInfo instrPage;
struct PageInfo instrs[1024];




void print_usage()
{
fprintf(stderr,\
"\ntones-to-rf \n\
Usage:\nfreq_pic -f frequency [-h]\n\
-f int        frequency to ouput on GPIO_4 pin 7.\n\
-h            help (this help).\n\
\n");
fprintf(stderr,\
"Example for 1 MHz:\n\
freq_pi -f 100000000\n\
\n"\
);

} /* end function print_usage */




int main(int argc, char **argv)
{
int a;
uint32_t frequency = 0; // -Wall


/* defaults */



/* end defaults */



/* proces any command line arguments */
while(1)
    {
    a = getopt(argc, argv, "f:h");
    if(a == -1) break;

    switch(a)
        {
        case 'f':    // frequency
            a = atoi(optarg);


            frequency = a;
            break;
        case 'h': // help
            print_usage();
            exit(1);
            break;
            break;
        case -1:
            break;
        case '?':
            if (isprint(optopt) )
                 {
                 fprintf(stderr, "send_iq: unknown option `-%c'.\n", optopt);
                 }
            else
                {
                fprintf(stderr, "send_iq: unknown option character `\\x%x'.\n", optopt);
                }
            print_usage();

            exit(1);
            break;            
        default:
            print_usage();

            exit(1);
            break;
        }/* end switch a */
    }/* end while getopt() */

int clock_source;
/*
Clock source
0 = GND
1 = oscillator
2 = testdebug0
3 = testdebug1
4 = PLLA per
5 = PLLC per
6 = PLLD per
7 = HDMI auxiliary
8-15 = GND
*/

/* init hardware */

clock_source = 6; /* this GPIO_4 pin 7 allows only the 200 MHz clock????? as source, the other clock GPIO lines are not on a pin in revision 2 board, so we have to work with 200 MHz, and that seems to be 250 MHz */

	start_rf_output(clock_source);

	FILE *file = fopen("./test.csv", "r");
	if ( file )
	{

		//Borrowing from example at http://stackoverflow.com/a/13195361 CSV extraction
		char line[512];
		int tone;
		int duration;
		while (fgets(line, 512, file))
		{
		
		    char tmp[sizeof line]; /* this will save a malloc()/free() pair */
		    char *tok, *fence, *pstr;
		    char ch, *cp1=line, *cp2=tmp;
		
		    while (0 != (ch = *cp1++))
		        *cp2++ = (ch == ',') ? 0 : ch;
		
		    fence = cp2; /* remember end of string */
		    *fence = 0;  /* and terminate final string */
		    tok = tmp;   /* point to first token */
		
		    tone =strtol(tok, &pstr, 10);
		    if (tok < fence) tok += strlen(tok) + 1;
		
		    //Make duration an int in microseconds
		    //Subtracting an ugly fudge factor to account for the loop time taking about 90usec on a Raspberry Pi 2
		    //Your fudge factor may vary with trial and error.
		    duration =(int)(strtof(tok,NULL)*1000)-90; 
		
		    //Make the RF tone
		    ACCESS(CM_GP0DIV) = calculate_divider(frequency+tone);
		    //Time interval for tone
		    usleep(duration);
		}


	}
	else /* fopen() returned NULL */
	{
		printf("error opening csv file");
	}


	stop_rf_output(clock_source);


//fprintf(stderr, "WAS here\n");

exit(0);
} /* end function  main */

