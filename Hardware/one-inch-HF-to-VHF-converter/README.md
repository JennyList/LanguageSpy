# One inch HF to VHF converter
This is an entry for the [2015 Hackaday.io One Square Inch competition](https://hackaday.io/project/7813-the-square-inch-project), an HF receive converter for VHF receivers like the RTL SDR or similar. You can find its project page [here](https://hackaday.io/project/8486-hf-receive-converter-for-rtl-sdrs-and-similar).

###Files:
- hf-upconverter.brd, an EAGLE CAD board file.
- hf-upconverter.sch, an EAGLE CAD schematic file.
- one-inch-HF-receive-converter-gerbers-v1.2.zip, Gerber files for manufacturing generated through EAGLE CAD using OSH Park's CAM file.
- upconverter PCB.png, a generated image of the PCB.
- upconverter schematic.png, the converter circuit diagram.
- hf-upconverter-bom.csv, the bill of materials for the project.
- LICENCE, a copy of the Solderpad Hardware Licence v0.51

###Instructions:
It's a small board, so the instructions are shre and sweet.
Solder all the chip components first, then the mixer IC, then the transformer, the electrolytic, the oscillator, and finally the connectors. You'll get lots of flux all over everything, but a quick brush with a bit of solvent cleaner should deal with that.

###Licence and boilerplate

This project is licenced under the Solderpad Hardware licence.

Copyright 2015 Jenny List
Copyright and related rights are licensed under the Solderpad Hardware License, Version 0.51 (the “License”); you may not use these files except in compliance with the License. 
You may obtain a copy of the License at [http://solderpad.org/licenses/SHL-0.51](http://solderpad.org/licenses/SHL-0.51). Unless required by applicable law or agreed to in writing, software, hardware and materials distributed under this License is distributed on an “AS IS” BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.
