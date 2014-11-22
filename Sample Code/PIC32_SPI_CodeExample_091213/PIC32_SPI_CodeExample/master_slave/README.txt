	Readme File for Code Example:
--------------------------------------------------

PIC32_SPI\master_slave

--------------------------------------------------
Code Example Description:
--------------------------------------------------
This test uses both SPI channels.
The master channel (SPI1) sends data to a slave device (SPI2).
The slave device relays data back to the master.
This way we can verify that the connection to the slave is ok.
Hardware connections have to be made:
	- SCK1 <-> SCK2
	- SDO1 <-> SDI2
	- SDI1 <-> SDO2
	- SS1  <-> SS2 (needed only if we use the framed mode)

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
            	PIC32MX220F032D 
		PIC32MX250F128D 
		

Complier:	MPLAB C32 v1 or higher
		MPLAB XC32 

IDE:		MPLAB IDE v8 or higher
            	MPLAB X IDE

--------------------------------------------------
Further Documentation:
--------------------------------------------------
For more information on this topic see Family Reference Manual (FRM)

FRM PDF can be found on Microchip website by searching for: 
"PIC32 Family Reference Manual, Sect. 23 Serial Peripheral Interface.pdf"