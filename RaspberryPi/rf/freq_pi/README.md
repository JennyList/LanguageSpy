# freq__pi signal generator for Raspberry Pi 2.
This directory contains a copy of Jan Panteltje's freq_pi signal generator modified for use with the Raspberry Pi 2. The licence is GPL, as per the original.

###Function:###
programmable frequency generator on GPIO_4 pin 7


###To compile:###
gcc -Wall -O4 -o freq_pi freq_pi.c -std=gnu99 -lm


###To install:###
cp freq_pi /usr/local/bin/

###To run:###
freq_pi

