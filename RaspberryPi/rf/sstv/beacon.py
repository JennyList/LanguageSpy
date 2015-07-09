#!/usr/bin/env python

"""
This file is a modified version of the beacon.py from http://hsbp.org/rpi-sstv.
Their repo: https://github.com/hsbp/rpi-sstv
I am much indebted to them for their work.

What it does: Takes a jpeg image called test.jpg, passes it through PySSTV, returns tone,duration pairs.

Modifications to original:

Removed callsign insertion code
Changed tuple output to CSV output to stdout
Removed GPIO code

Usage: Pipe output to a file

python beacon.py > test.csv

"""

from PIL import Image, ImageFont, ImageDraw
from pysstv.color import MartinM2
from subprocess import check_output
from cStringIO import StringIO
import RPi.GPIO as GPIO
import struct, sys

TX_PIN = 18

def main():
    img = Image.open('./test.jpg')
    img = img.resize((MartinM2.WIDTH, MartinM2.HEIGHT))
    font = ImageFont.load_default()
    sstv = MartinM2(img, 44100, 16)
    sstv.vox_enabled = True
    for freq, msec in sstv.gen_freq_bits():
        print freq,',',msec

if __name__ == '__main__':
    main()
