# ShiftX2
Firmware for ShiftX2 - CAN bus controlled sequential shift light and alarm display module

![alt text](images/shiftX_multi.png?raw=true)

![alt text](images/shiftX2_components.jpg?raw=true)


# More information
Visit http://https://wiki.autosportlabs.com/ShiftX2 for full usage and documentation

# Hardware setup 

## Pinout
```
Color	Assignment	Notes
Red	5-12v Power	Connect this to switched ignition power. 0.5A max consumption.
Black	CAN High	
Yellow	CAN Low	
White	Ground	
```

## Flashing new firmware
Flashing firmware is provided by a SWD interface. Pads for a SWD connection are provided on the bottom of the board with 0.1" spacing:
* 3.3v
* GND
* Reset
* SWCLK
* SWDIO

To program new firmware, solder a 0.1" header to these pads, and connect the corresponding signals to a compatible SWD programmer, such as the STLink V2. 

## CAN termination
CAN termination is enabled by default. To disable termination, cut the CAN Term jumper on the bottom of ShiftX2.

## CAN default address

### CAN base ID
Default base Address is 0xE3600 (931328); cut the ADR1 jumper to enable the alternate base address of 0xE3700 (931584)


## CAN baud rate
500K is enabled by default; cut the jumper ADR2 on the bottom of ShiftX2 to enable 1MB.

# CAN bus API

## Overview
The CAN bus API provides the configuration and control interface for ShiftX2.

Two styles of control are available:

Low level control of LEDs - the ability to discretely set LED color and flash behavior
High level control - configuring alert thresholds and linear graph up front, and then providing simple value updates


## Configuration / Runtime Options
### Announcement
Broadcast by the device upon power up

CAN ID: Base + 0

```
Offset	What	                  Value
====================================================================
0	Total LEDs	          Total number of LEDs on the device
1	Alert Indicators	  Number of logical alert indicators
2	Linear Bar Graph Length	  Number of LEDs in linear graph
3	Major Version	          Firmware major version number
4	Minor Version	          Firmware minor version number
5	Patch Version	          Firmware patch version number
```

### Statistics
Statistics information, broadcast periodically by device

CAN ID: Base + 2

```
Offset	What	                  Value
=====================================================================
0	Major Version	          Firmware major version number
1	Minor Version	          Firmware minor version number
2	Patch Version	          Firmware patch version number
```

### Set Configuration Parameters Group 1
Sets various configuration options.

CAN ID: Base + 3

```
Offset	What	                   Value
======================================================================
0	Brightness	           0 - 100; default = 0 (0=automatic brightness)
1	Automatic brightness
        scaling  (Optional)	   0-255; default=51
```

### Set Discrete LED
A low level function to directly set any LED on the device.

CAN ID: Base + 10

```
Offset  What                       Value
======================================================================
0	LED index	           0 -> # of LEDs on device
1	Number of LEDs to set	   0 -> # of LEDs on device (0 = set all remaining)
2	Red	                   0 - 255
3	Green	                   0 - 255
4	Blue	                   0 - 255
5	Flash	                   0-10Hz (0 = full on)
```

## Alert Indicators
Alert Indicators are typically single LEDs or a group of LEDs treated as one logical unit. This is defined by the hardware configuration of the device.

### Set Alert
Directly set an alert indicator

CAN ID: Base + 20

```
Offset  What                       Value
======================================================================
0	Alert ID	           0 -> # of Alert indicators
1	Red	                   0 - 255
2	Green	                   0 - 255
3	Blue	                   0 - 255
4	Flash	                   0-10Hz (0 = full on)
```

### Set Alert Threshold
Configures an alert threshold. Up to 5 thresholds can be configured per alert indicator.

Notes:

* If the current value is greater than the last threshold, then the last threshold is selected.
* The first threshold may have a threshold value >= 0; remaining thresholds must be > 0

CAN ID: Base + 21

```
Offset  What                       Value
======================================================================
0	Alert ID	           0 -> # of Alert indicators
1	Threshold ID               0 - 4
2	Threshold                  (low byte)	
3	Threshold                  (high byte)	
4	Red	                   0 - 255
5	Green	                   0 - 255
6	Blue	                   0 - 255
7	Flash Hz	           0 - 10 (0 = full on)
```

### Update Current Alert Value
Updates the current value for an alert indicator. The configured alert thresholds will be applied to the current value.

CAN ID: Base + 22

```
Offset  What                       Value
======================================================================
0	Alert ID	           0 -> # of Alert indicators
1	Value                      (low byte)	
2	Value                      (high byte)	
```

## Linear Graph
The linear graph mode provides visualizations for common scenarios:

Sequential RPM shift light where the progression is stepped
Linear bar graph to linearly indicate a sensor value
Center left/right graph to indicate +/- performance against a reference - such as visualizing current predictive time vs best time
Power up default configuration
Upon power up the linear graph is configured:

Rendering style: left->right
Linear style: stepped
Low Range: 0
High Range: N/A
Threshold :
Threshold value: 3000 / segment length: 3 / color RGB: (0, 255, 0) / flash: 0
Threshold value: 5000 / segment length: 5 / color RGB: (0, 255, 255) / flash: 0
Threshold value: 7000 / segment length: 7 / color RGB: (255, 0, 0) / flash: 5

### Configure Linear Graph
Configures the options for the linear graph portion of the device.

Rendering style:

If left->right, linear graph illuminates left to right
If centered, values below the mid-point in the range extend from the center to the left, and values above the mid-point extend from the center to the right.
If right->left, linear graph illuminates right to left
Linear style:

if smooth, graph length is updated smoothly by interpolating the current value in-between LEDs
If stepped, creates the visual effect of stepped progressions by setting the segment length of the threshold configuration. High Range of configuration is ignored.

CAN ID: Base + 40

```
Offset  What                       Value
======================================================================
0	Rendering Style	           0 = left->right, 1=center, 2=right->left
1	Linear Style	           0 = Smooth / interpolated, 1 = stepped
2	Low Range                  (low byte)	
3	Low Range                  (high byte)	
4	High Range                 (low byte)	(ignored if linear style = stepped)
5	High Range                 (high byte)	(ignored if linear style = stepped)
```

### Set Linear Graph Threshold
Configures a linear threshold. Up to 5 thresholds can be configured

Note:
* If the current value is greater than the last threshold, then the last threshold is selected.
* The first threshold may have a threshold value >= 0; remaining thresholds must be > 0

CAN ID: Base + 41

```
Offset  What                       Value
======================================================================
0	Threshold ID	           0 - 4
1	Segment Length	           0 -> # of LEDs on device (ignored if linear style is ‘smooth’)
2	Threshold                  (low byte)	
3	Threshold                  (high byte)	
4	Red	                   0 - 255
5	Green	                   0 - 255
6	Blue	                   0 - 255
7	Flash Hz	           0 - 10 (0 = full on)
```

### Update Current Linear Graph Value
Updates the current value for the linear graph

CAN ID: Base + 42

```
Offset  What                       Value
======================================================================
0	Value                      (low byte)	
1	Value                      (high byte)	
```

## Notifications
Notifications related to events broadcasted from ShiftX2

### Button State
Indicates a change in the button state

CAN ID: Base + 60

```
Offset  What                       Value
======================================================================
0	Button state	           1 = button is pressed; 0 = button
```
