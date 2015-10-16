#!/usr/bin/env python

#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <http://www.gnu.org/licenses/>.

# PiVFO Version 0.1
# A GUI for freq_pi
# Jenny List G7CKF
# http://www.languagespy.com

import time
import math
import subprocess

#import and rename the 'tkinter' module for < Python 3.3:
import Tkinter as tkinter

#Initial frequency, variable then holds VFO frequency
vfoFreq = 14000000
#VFO state
vfoState = False

#Max frequency 250MHz
maxFreq = 250000000
#Min frequency is 125kHz
minFreq = 125000

#Location of freq_pi 
freqgen = "./freq_pi"

def doQuitEvent(key):
    c.resetty() # set terminal settings
    c.endwin()  # end curses session
    subprocess.call([freqgen, "-q"]) #Turn off the clock
    raise SystemExit

#VFO on and off
def vfoOnFunc():
    global vfoState
    subprocess.call([freqgen, "-f", str(vfoFreq)])
    vfoState = True
    vfoOnButton['state'] = 'disabled'
    vfoOffButton['state'] = 'normal'

def vfoOffFunc():
    global vfoState
    subprocess.call([freqgen, "-q"])
    vfoState = False
    vfoOnButton['state'] = 'normal'
    vfoOffButton['state'] = 'disabled'

#Change frequency
def freqChange(deltaF):
    global vfoFreq
    global frequencyLabel
    global frequencyUnitLabel
    newFreq = vfoFreq + deltaF
    #max/min check
    if(newFreq >= minFreq and newFreq <= maxFreq):
        vfoFreq = newFreq
    #else:
        #Need to display some kind of message perhaps?
    #set the display
    frequencyLabel.configure(text=freqToDisplay(vfoFreq))
    frequencyUnitLabel.configure(text=freqDisplayUnit(vfoFreq))
    if(vfoState == True):
        subprocess.call([freqgen, "-f", str(vfoFreq)])

#Format a display frequency
def freqToDisplay(intFreq):
    freq = str(intFreq)
    #Split off Hz. Everything will have a Hz fraction
    Hz = freq[-3] +freq[-2] +freq[-1]
    #Split off kHz. All frequencies will be above 100kHz.
    kHz = freq[-6] + freq[-5] +freq[-4] + '.'
    #Split off the Mhz fraction
    if intFreq >= 1000000:
        MHz = str(int(intFreq/1000000)) + '.'
    else: #Catch frequencies below 1MHz
        MHz = '' #We'll display these as kHz
    returnFreq = MHz + kHz + Hz
    return returnFreq

#Pick a unit for frequency display
def freqDisplayUnit(intFreq):
    unit = 'MHz'
    if intFreq < 1000000:
        unit = 'kHz'
    return unit

#Dial function
dialAngle = 0
def dialMove(event):
    global canvasWidth
    global canvasHeight
    global dialAngle
    global dialFingerHole
    global dialCanvas
    global fingerX
    global fingerY
    xRelativeToCentre = int(event.x-(canvasWidth/2))
    yRelativeToCentre = int(event.y-(canvasHeight/2))
    if(xRelativeToCentre == 0): #Catch divide by zero
        xRelativeToCentre = 1
    theta = (math.atan2(xRelativeToCentre,yRelativeToCentre) + math.pi) #make it 0 rather than -pi at the top.
    if( theta>dialAngle):
	subtended = theta-dialAngle
        if(subtended > 1):
            deltaF = -24 #Catch 2pi to zero switchover with about the delta-f we want.
        else:
            deltaF = int(-1*subtended*(500/2*math.pi))
    else:
	subtended = dialAngle-theta
        if(subtended > 1):
            deltaF = 24
        else:
            deltaF = int(subtended*(500/2*math.pi))
    freqChange(deltaF)
    dialAngle = theta
    #Move the finger hole.
    newFingerX = int(canvasWidth/2 + ((canvasWidth/2)-20)*math.cos(theta-math.pi/2))
    newFingerY = int(canvasHeight/2 + ((canvasHeight/2)-20)*math.sin(theta-math.pi/2))
    dialCanvas.move(dialFingerHole, fingerX-newFingerX, newFingerY-fingerY)
    fingerX = newFingerX
    fingerY = newFingerY


#create a new window
window = tkinter.Tk()
#set the window title
window.title("PiVFO by G7CKF/Language Spy")
#set the window background colour
windowBackground = '#123123123'
window.configure(background=windowBackground)

#Display row group, for frequency controls and display
frequencyRowGroup = tkinter.LabelFrame(window, relief="flat", background=windowBackground)
frequencyRowGroup.pack(side="left")

#Display row group, for freq display
displayRowGroup = tkinter.LabelFrame(frequencyRowGroup, relief="flat", background=windowBackground)
displayRowGroup.pack()

#Group to hold VFO display and adjustment
displayGroup = tkinter.LabelFrame(displayRowGroup, background="#000000000", foreground="#000fff000", padx=5, pady=5)
displayGroup.pack(padx=2, pady=2, side="left")
#Frequency display
frequencyLabel = tkinter.Label(displayGroup, text=freqToDisplay(vfoFreq), background="#000000000", foreground="#000fff000", width="12", justify="right", font=("Helvetica", 24))
#Frequency unit
frequencyUnitLabel = tkinter.Label(displayGroup, text=freqDisplayUnit(vfoFreq), background="#000000", foreground="#000fff000", width="3", justify="left", font=("Helvetica", 10))
frequencyLabel.pack(side="left")
frequencyUnitLabel.pack(side="left")

#Frequency control row group, for up/down buttons and dial
frequencyControlRowGroup = tkinter.LabelFrame(frequencyRowGroup, relief="flat", background=windowBackground)
frequencyControlRowGroup.pack()

#Frequency dial
canvasWidth = 100
canvasHeight = 100
fingerX = 50
fingerY = 20
dialCanvas = tkinter.Canvas(frequencyControlRowGroup, height=canvasHeight, width=canvasWidth, relief="flat", background=windowBackground)
dial = dialCanvas.create_oval(10, 10, 90, 90, width=5, outline='#800800800', fill='#200200200')
dialFingerHole = dialCanvas.create_oval(40, 10, 60, 30, width=2, fill='#500500500')
dialCanvas.bind("<B1-Motion>", dialMove)#Bound to whole canvas

#Groups to hold up/down buttons
frequencyControlGroupLeft = tkinter.LabelFrame(frequencyControlRowGroup, padx=0, pady=0, relief="flat", background=windowBackground)
frequencyControlGroupRight = tkinter.LabelFrame(frequencyControlRowGroup, padx=0, pady=0, relief="flat", background=windowBackground)
#create button widgets to increment
downTenMHz = tkinter.Button(frequencyControlGroupLeft, text="-10MHz", command=lambda: freqChange(-10000000), padx=2, pady=3, font=("Helvetica", 10))
downOneMHz = tkinter.Button(frequencyControlGroupLeft, text="-1MHz", command=lambda: freqChange(-1000000), padx=2, pady=3, font=("Helvetica", 10))
downhundredKHz = tkinter.Button(frequencyControlGroupLeft, text="-100kHz", command=lambda: freqChange(-100000), padx=2, pady=3, font=("Helvetica", 10))
downTenKHz = tkinter.Button(frequencyControlGroupLeft, text="-10kHz", command=lambda: freqChange(-10000), padx=2, pady=3, font=("Helvetica", 10))
downTenMHz.pack(fill='x')
downOneMHz.pack(fill='x')
downhundredKHz.pack(fill='x')
downTenKHz.pack(fill='x')
#create button widgets to decrement
upTenKHz = tkinter.Button(frequencyControlGroupRight, text="+10kHz", command=lambda: freqChange(10000), padx=2, pady=3, font=("Helvetica", 10))
uphundredKHz = tkinter.Button(frequencyControlGroupRight, text="+100kHz", command=lambda: freqChange(100000), padx=2, pady=3, font=("Helvetica", 10))
upOneMHz = tkinter.Button(frequencyControlGroupRight, text="+1MHz", command=lambda: freqChange(1000000), padx=2, pady=3, font=("Helvetica", 10))
upTenMHz = tkinter.Button(frequencyControlGroupRight, text="+10MHz", command=lambda: freqChange(10000000), padx=2, pady=3, font=("Helvetica", 10))
upTenMHz.pack(fill='x')
upOneMHz.pack(fill='x')
uphundredKHz.pack(fill='x')
upTenKHz.pack(fill='x')

#Pack all the frequency control stuff
frequencyControlGroupLeft.pack(padx=0, pady=0, side="left")
dialCanvas.pack(side='left')
frequencyControlGroupRight.pack(padx=0, pady=0, side="left")

#Group to hold VFO power functions
powerGroup = tkinter.LabelFrame(window, relief="flat", background=windowBackground)
powerGroup.pack(padx=0, pady=0, side="top")
#create a button widget to call vfoOn
vfoOnButton = tkinter.Button(powerGroup, text="Start VFO", command=vfoOnFunc, font=("Helvetica", 10))
#create a button widget to call vfoOff
vfoOffButton = tkinter.Button(powerGroup, text="Stop VFO", command=vfoOffFunc, state='disabled', font=("Helvetica", 10))
#Pack powerGroup widgets
vfoOnButton.pack()
vfoOffButton.pack()

#create a label widget called 'lbl'
#lbl = tkinter.Label(window, text="Label")
#pack (add) the widgets into the window
#lbl.pack()

#draw the window, and start the 'application'
window.mainloop()
