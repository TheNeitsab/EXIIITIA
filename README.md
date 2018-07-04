# **EXIIITIA - EXIII's HACKberry by EXTIA**
EXIIITIA is a project implemented by EXTIA in order to provide our version of EXIII's HACKberry artificial arm as part of ANA's project which is to conceive and optimize an open-source robotic hand for Ana, a 13-year-old girl born with left arm agenesis.
## HARDWARE
Here is a list of all the Hardware used for the arm (see DOCUMENTS/EXIIITIA_SCHEMATIC.png to get a better overview and see how everything is connected) :
- [1 Arduino Micro](https://store.arduino.cc/usa/arduino-micro)
- [1 Adafruit GEMMA V2](https://www.adafruit.com/product/1222)
- [1 GEEETECH Voice recognition module](http://www.geeetech.com/wiki/index.php/Arduino_Voice_Recognition_Module)
- [1 DC/DC Converter](https://www.amazon.fr/SODIAL-Abaisseur-Regulateur-Tension-Ajustable/dp/B00JGFEGG4/ref=pd_sim_107_1?_encoding=UTF8&pd_rd_i=B00JGFEGG4&pd_rd_r=2630e3d8-7f66-11e8-b707-a7ec52204a2a&pd_rd_w=iQ7HK&pd_rd_wg=8EBfT&pf_rd_i=desktop-dp-sims&pf_rd_m=A1X6FK5RDHNB96&pf_rd_p=8946694021807602816&pf_rd_r=9M8TWWDEC4EY7C10XZ0G&pf_rd_s=desktop-dp-sims&pf_rd_t=40701&psc=1&refRID=9M8TWWDEC4EY7C10XZ0G)
- [1 RGB LED](https://www.adafruit.com/product/159)
- [4 Adafruit NeoPixels](https://www.adafruit.com/product/1612)
- [1 Force Sensing Resistor](https://www.adafruit.com/product/166)
- [2 little servos](http://www.electronicoscaldas.com/datasheet/MG90S_Tower-Pro.pdf) & [1 big one](http://www.electronicoscaldas.com/datasheet/MG996R_Tower-Pro.pdf)
- 3 Pushbuttons
## CODE
### `Charge_Indicator`
Contains the Arduino code `Charge_Indicator.ino` uploaded in the GEMMA located in the arm handling the battery level indicator operation.
### `Exiiitia`
Contains the Arduino code `Exiiitia.ino` which is the original file imported in the arm, with a global C architecture, it is build around the original file provided by HACKberry and sligtly modified to fit our needs with some added specs such as vocal recognition module functions or inactivity check for servos.
> Please note that even though this file is functional, it is quite old and might be a little bit tiring and hard to apprehend since everything is stored in this one and only file. See `My_Exiiitia` files for clearer options.
### `LIB`
Contains the library files that are used in following My_Exiitia Arduino codes.
#### ExiiitiaLibV3 :
This library is the one mananging the hand behavior for `My_Exiiitia_CPP.ino`.
#### ExtiaCounter :
This library is an homemade counters library using Arduino timer 1 since we've been facing some issues with interferences because of the use of LEDs, servos, softwareSerial etc...
### `My_Exiiitia`
Contains a personal version of the software used for the arm's behavior, this Arduino code `My_Exiiitia.ino` is an extension of `Exiitia.ino` using homemade libraries managing counters using Arduino timers.
### `My_Exiiitia_CPP`
Contains the Arduino code `My_Exiiitia_CPP.ino` which is basically also an homemade one with a C++ architecture.
## DOCUMENTS
### `EXIIITIA_SCHEMATIC.png`
This is the schematic with all the hardware that is contained into the prototype that we made.
### `Guide_GEMMA_EN.pdf` & `Guide_GEMMA_FR.pdf`
These documents are "how to" manuals for GEMMA's use respectively in English and French.
### `Guide_Module_GEEETECH_EN.pdf` & `Guide_Module_GEEETECH_FR.pdf`
This is a "how to" manual to configure and use the GEEETECH vocal recognition module that we implemented in our prototype.

## CONTACT
**Arnaud LAURENT**
*Responsable de mécénat de compétences*
alaurent@extia.fr