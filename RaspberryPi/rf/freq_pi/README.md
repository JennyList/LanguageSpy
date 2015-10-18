# freq_pi signal generator and PiVFO GUI
This directory contains two pieces of software:
* A copy of Jan Panteltje's freq_pi signal generator modified to detect which Raspberry Pi model it is running on.
* PiVFO, a Python GUI for freq_pi that provides a simple variable frequency oscillator.

##Quick install for both pieces of software:
At a command line in your home directory on a stock Raspbian install:
```
mkdir freq_pi
cd freq_pi
curl -o freq_pi.c https://raw.githubusercontent.com/JennyList/LanguageSpy/master/RaspberryPi/rf/freq_pi/freq_pi.c
curl -o PiVFO.py https://raw.githubusercontent.com/JennyList/LanguageSpy/master/RaspberryPi/rf/freq_pi/PiVFO.py
gcc -Wall -O4 -o freq_pi freq_pi.c -std=gnu99 -lm
```

##freq_pi

###Function:###
freq_pi is a programmable frequency generator that uses the Raspberry Pi GPCLK0 line, on pin 7 of the GPIO port.
This version 0.72 now does not need any configuration to run on either BCM2385 or BCM2386 Pi boards, so will run on the Pi or the Pi 2.

##To compile freq_pi:###
gcc -Wall -O4 -o freq_pi freq_pi.c -std=gnu99 -lm

###To run freq_pi:###
sudo freq_pi -f (frequency in Hz)

Example for 14MHz: sudo freq_pi -f 14000000

To turn freq_pi off: sudo freq_pi -q

sudo is required to access the Pi hardware directly.

##PiVFO

###Function:###
PiVFO is a simple GUI for freq_pi that simulates a simple variable frequency oscillator.

###To configure PiVFO:###
If the freq_pi executable is in the same directory as PiVFO.py then no configuration should be necessary.
If it is in a different directory then find the line that defines the variable called freqgen and update it to the path where freq_pi can be found.

###To run PiVFO###

sudo python ./PiVFO.py

sudo is required to access the Pi hardware directly.

Click the "Start VFO" button to start the VFO, select your frequency with the buttons and dial.
