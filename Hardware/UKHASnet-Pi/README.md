# A HopeRF RFM69 board for the Raspberry Pi
This is a board to allow the [HopeRF RFM69](http://www.hoperf.com/rf_transceiver/modules/RFM69HW.html) RF modules to be interfaced to a Raspberry Pi. It can be used for experimenting with [UKHASnet](https://ukhas.net/), or just as an experimenting board in its own right for the RFM69 series.

###Files:
- RPi-UKHASnet-node.brd, an EAGLE CAD board file.
- RPi-UKHASnet-node.sch, an EAGLE CAD schematic file.
- UKHASnet-node-gerbers.zip, Gerber files for manufacturing generated through EAGLE CAD using Dirt Cheap Dirty Boards' CAM file.
- UKHASnet-Pi-board.png, a generated image of the PCB.
- UKHASnet-Pi-schematic.png, the converter circuit diagram.
- LICENCE, a copy of the Solderpad Hardware Licence v0.51

###Description:
This is a simple breakout board, very little rocket science. It features a 3.3v regulator on the underside because the module is capable of taking more current than the Pi's regulator has to spare.

###Licence and boilerplate

This project is licenced under the Solderpad Hardware licence.

Copyright 2016 Jenny List
Copyright and related rights are licensed under the Solderpad Hardware License, Version 0.51 (the “License”); you may not use these files except in compliance with the License. 
You may obtain a copy of the License at [http://solderpad.org/licenses/SHL-0.51](http://solderpad.org/licenses/SHL-0.51). Unless required by applicable law or agreed to in writing, software, hardware and materials distributed under this License is distributed on an “AS IS” BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
