# Asip #
ASIP (Arduino Service Interface Protocol) is a protocol to control an Arduino board from a computer. The protocol is intended to be a more extensible replacement for Firmata.

### What is this repository for? ###

*This repository includes the code for the core Asip functionalities: setting pin modes, reading and writing digital and analog pins.
*Version: 1.2.0


### Quick installation instructions ###

* Open the Arduino IDE (please use version 1.6.8 or above).
* Select Sketch -> Include Library -> Manage Libraries...
* Search for and install asip
* Click on File -> Examples -> asip -> AsipIO
* Connect a board, select the appropriate board and port from the Tools menu and upload the AsipIO sketch.
* You are now ready to go. You can either send messages directly through the serial port (remember to set the baud rate to 57600) or you can download a client for a programming language such as Java, Python and Racket. 
* The examples folder contains sketches showing how to use the sensors and actuators supported by ASIP.
* The documents folder contains details of protocol for the supported services 


### Additional help ###

* Don't be afraid of contacting us. We'll try to reply as soon as possible. Feel free to open issues.
* Do you want to contribute? Please get in touch :-)! We need help for a number of things, starting from documentation to the development of other clients, additional services, examples, etc.