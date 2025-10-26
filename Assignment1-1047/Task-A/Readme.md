Task # 01

Wokwi Link:
https://wokwi.com/projects/445770326495778817


Loom Vedio Link:
https://www.loom.com/share/4f91f37e2e174540972ca70a2289b9a5


Pin Map:
* Pin 16 is a LED pin used in blinking and ON/OFF modes_LED-A
* Pin 17 is a LED pin used in blinking and ON/OFF modes_LED-B
* Pin 18 is a PWM LED used for smooth brightness fading_LED-FADE
* Pin 33 is a button to change modes (interrupt-based, active LOW)_BUTTON_MODE
* Pin 27 is a button to reset system to Mode 0 (interrupt-based, active LOW)_BUTTON_RESET
* Pin 21  is a I²C SDA pin that is Data line for OLED display
* Pin 22 is a	I²C SCL	Clock used for OLED display
* 3.3V & GND are Power & Ground respectivly where power supply for OLED and other components

Summary:
Two buttons control multiple LED modes displayed on an OLED screen. 
The Mode button (GPIO 33) cycles through four modes: 
* Mode 0: Keeps both LEDs off 
* Mode 1: Makes them blink alternately 
* Mode 2: Turns both LEDs on
* Mode 3: Smoothly fades the third LED using PWM. 
The Reset button (GPIO 27) returns the system to Mode 0. 
The OLED display (SDA = 21, SCL = 22) shows the current mode or system status messages for clear visual feedback.