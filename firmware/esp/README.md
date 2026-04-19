# SnakeGame

This is a game designed to be played with a 8x8 dot matrix. A buzzer can also be added to play sound during game run.

It's built around the ESP32 platform and the controls are available on a webpage hosted on the chip.


## Dot Matrix module
I used the 1588AS module in the development of the game.

Below are the rules I figured out for how to run them directly (without IO expander module)

### Physical Pins
**Top pins**: C8, C7, R2, C1, R4, C6, C4, R1

**Bottom pins** (this part contains the label, i.e. 1588AS):  R5, R7, C2, C3, R8, C5, R6, R3

### How to activate
**Row**: set all pins to 1 then toggle row pins to 0 to turn on that row

**Col**: set all pins to 0 then toggle col pins to 1 to turn on that col
 


## Startup connection
When booted up for the first time, the esp32 will enter AP mode and broadcast the SSID, "SnakeGame". After connecting to this, the user will need to enter the SSID and password of the network to be connected to. The esp32 will then restart and attempt to connect to the provided network. If it can't connect, it will restart again in AP mode but if it connects, you can go on and start the game from the portal or also revisit "/captive" to change the network it should connect to.