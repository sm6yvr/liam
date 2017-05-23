# liam - DIY Robot lawn mover 

Liam is an arduino project for the DIY Robot Lawn Mover. 

This program will control your mower and relies on a two coil
configuration (0 and 1) with an optional (2). 
   
The mower is assumed to be rear wheel driven and have three 
boundary wire reciever coils in the following configuration
   
<div>
     wheel<br/>
    ------------------<br/>
   |               (0)|<br/>
   |(2)	              |  ----> Mowing direction<br/>
   |               (1)|</br>
    -------------------</br>
     wheel</br>
</div>

(c) Jonas Forssell & team
Free to use for all.

Configuration
------
Most of the default values for your mower can be set in the 
Definition.h file. Use this to configure your mower for best
performance.

	
Compile
------
The following libraries must be added to your arduino IDE to be able to compile the source
  
  * HMC5883L
  * I2Cdev
  * LiquidCrystal_I2C
  * MPU9150
  * RTClib
  * RTIMULib

  It's possible to download a zip bundle with all libraries you need from our facebook groups file area. 

Change log
------
5.1  - Removed OzOLED Support for Arduino101 Compatibility

Links
------
[Facebook group DIY Robot Lawn Mower](https://www.facebook.com/groups/319588508137220/)

[Build instructions Trello](https://trello.com/b/gYQjoWY5/liam)
