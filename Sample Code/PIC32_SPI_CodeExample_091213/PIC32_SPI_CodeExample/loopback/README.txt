	Readme File for Code Example:
--------------------------------------------------

PIC32_SPI\loopback

--------------------------------------------------
Code Example Description:
--------------------------------------------------
This test assumes that the SPI SDO output is connected to the SDI input. If data is transferred successfully, the first LED on the Explorer-16 is turned on.

--------------------------------------------------
Suggested Development Resources:
--------------------------------------------------
Explorer-16 demo board with 

Processors:	PIC32MX###F512L
		       360
		       460
		       795
		PIC32MX430F064L 
		PIC32MX450F256L 
		PIC32MX470F512L
            	PIC32MX220F032D (LEDs must be soldered to the PIM)
		PIC32MX250F128D (LEDs must be soldered to the PIM)
		

Complier:	MPLAB C32 v1 or higher
		MPLAB XC32 

IDE:		MPLAB IDE v8 or higher
            	MPLAB X IDE

--------------------------------------------------
Notes:
--------------------------------------------------
On PIC32MX 2xx PIMS, connect pin RA1(SDI1) to pin RA2(SDO1). On PIC32MX 430 PIM, connect RF2(SDI1) to RD4(SDO1). 
All other devices connect pin RF7(SDI1) to pin RF8(SDO1).

For PIC32MX 220/250 PIMS, Explorer-16 LEDs are unconnected and must be soldered to the test points on top of the PIM as stated below.

--------------------------------------------------
Soldering Instructions for PIC32MX 220/250 PIMS:
--------------------------------------------------
The test points (TP) are located on top of the PIM and must be connected as follows to the Explorer-16 for proper LED functionality.

TP19 to D3  (RA0 to LED0)

