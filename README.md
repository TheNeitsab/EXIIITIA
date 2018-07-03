# **EXIIITIA - EXIII's HACKberry by EXTIA**
EXIIITIA is a project implemented by EXTIA in order to provide our version of EXIII's HACKberry artificial arm as part of ANA's project which is to conceive and optimize an open-source robotic hand for Ana, a 13-year-old girl born with left arm agenesis.
## CODE
### `Charge_Indicator`
Contains the Arduino code `Charge_Indicator.ino` uploaded in the GEMMA located in the arm handling the battery level indicator operation.
### `Exiiitia`
Contains the Arduino code `Exiiitia.ino` which is the original file imported in the arm, with a global C architecture, it is build around the original file provided by HACKberry and sligtly modified to fit our needs with some added specs such as vocal recognition module functions or inactivity check for servos.
> Please note that even though this file is functional, it is quite old and might be a little bit tiring and hard to apprehend since everything is sored in this one and only file. See `My_Exiiitia` files for clearer options.
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
### `Guide_Module_GEEETECH_FR.pdf`
This is a "how to" manual to use the GEEETECH vocal recognition module that we implemented in our prototype.
> It is only available in French for the moment, English one is coming soon ! For the moment, you can enjoy the one already available on [GEEETECH's website](http://www.geeetech.com/wiki/images/6/69/Voice_Recognize_manual.pdf).

## CONTACT
**Arnaud LAURENT**
*Responsable de mécénat de compétences*
alaurent@extia.fr