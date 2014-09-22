#ifndef MINISEL_LCD_h
#define MINISEL_LCD_h

/* This is LCD driver library for ARDO washing machine display.
 * LCD is controlled by MINISEL 45DX-100DX-85S controller board.
 * LCD board supports SPI interface for display. MISO pin is used
 * for reading button status.
 *
 * Original microcontroller was replaced by Atmega328 leaving all 
 * analog components. MINISEL schematic and description can be  
 * found in magazine : "ремонт и сервис журнал 2007 12"
 * or "remont i servis 2007 12"
 *
 * MINISEL_LCD.update() method must be called at regular intervals to generate 
 * correct AC signal sequence for LCD segments. All default SPI pins are for 
 * communication and controll
 *
 * Emaple 
 * MINISEL_LCD lcd;
 * void setup() {
 *	....
 *	TCCR2A = 0b00000000;	//Normal Timer2 mode.
 *	TCCR2B = 0b00000100;	//Prescale 4Mhz/64 or 16Mhz/64
 *	TIMSK2 = 0b00000001;	//Enable overflow interrupt
 *	...
 * } 
 * ISR(TIMER2_OVF_vect)
 * {
 *	lcd.update();
 * }
 *
 *
 * LCD panel features
 * - 5 wahing mode icons with frames.
 * - 3 multisegment character area in [18] configuration 
 * - 1 multisegment character area in [8:88] configuration
 */

/* LCD PBC has 6 HC595 chips.
 *  This is descriptin of each bit in 6 command bytes

command 0
7  10 minutes bottom l/r
6  hours bottom d
5  hours bottom l/r
4  segment power line 1
3  segment power line 2
2  segment common power line
1  180' icon
0  10 minutes + minutes bottom

command 1
7  drum icon
6  sun icon
5  hours u/l
4  hours r/d
3  10 minutes top 10 u/l
2  10 minutes top 10 r/d
1  minutes top u/l
0  minutes top r/d

command 2
7  drop hunderts top u/r
6  timer bottom
5  timer hours top l/r
4  timer hours top l/d
3  timer hours top u/r
2  prewash 1 icon
1  main wash 2 icon
0  rinse 3 icon

command 3
7  180' one and 10 top
6  180' 10 top l/r
5  180' 10 bottom u/l
4  180' 10 bottom r/d
3  timer and drum speed icon
2  drum 1000 digit and 100 bottom apaksa
1  drum 100 bottom l/r
0  drum 100 top l/d

command 4
7  led 7 and button 7 test power line
6  led 6 and button 6 test power line
5  led 5 and button 5 test power line
4  led 4 and button 4 test power line
3  led 3 and button 3 test power line
2  led 2 (not present) and button 2 test power line
1  led 1 and button 1 test power line
0  not used ?

command 5
7  not used ?
6  not used ? 
5  not used ?
4  led and display backlight common power line
3  not used ?
2  not used ?
1  not used ?
0  minutes bottom l/r

*/

// Macros for command syntesis

// This is placeholder for font generation if bit value must be ignored
#define MsBit_IGNORE 0xff
// Two sets of powered lcd cells are controlled independently to 
// produce readable fonts.
// This does not matter for pictures.
// Command number:
#define Msc0 (0<<5)
#define Msc1 (1<<5)
#define Msc2 (2<<5)
#define Msc3 (3<<5)
#define Msc4 (4<<5)
#define Msc5 (5<<5)
#define MsFONT (6<<5) // This bit mask tells that this ir not a command

// Bit numbrt
#define Msb0 (0<<2)
#define Msb1 (1<<2)
#define Msb2 (2<<2)
#define Msb3 (3<<2)
#define Msb4 (4<<2)
#define Msb5 (5<<2)
#define Msb6 (6<<2)
#define Msb7 (7<<2)

// Enabled by bit value 0 or 1:
#define Mso0 (0<<1)
#define Mso1 (1<<1)

// Enabled by segment power line 0 or 1:
#define Msp0 (0)
#define Msp1 (1)

// icons, 1 command for 1 bit. Size 1byte

#define Ms180_BOX	(Msc0|Msb1|Mso1|Msp0)
#define Ms180_ICON	(Msc0|Msb1|Mso1|Msp1)
#define MsDRUM_BOX	(Msc1|Msb7|Mso1|Msp0)
#define MsDRUM_ICON	(Msc1|Msb7|Mso1|Msp1)
#define MsSUN_BOX	(Msc1|Msb6|Mso1|Msp0)
#define MsSUN_ICON	(Msc1|Msb6|Mso1|Msp1)
#define MsPREWASH_BOX	(Msc2|Msb2|Mso1|Msp0)
#define MsPREWASH_ICON	(Msc2|Msb2|Mso1|Msp1)
#define MsWASH_BOX	(Msc2|Msb1|Mso1|Msp0)
#define MsWASH_ICON	(Msc2|Msb1|Mso1|Msp1)
#define MsRINSE_BOX	(Msc2|Msb0|Mso1|Msp0)
#define MsRINSE_ICON	(Msc2|Msb0|Mso1|Msp1)
#define MsTIMER_BOX	(Msc3|Msb3|Mso1|Msp0)
#define MsTIMER_ICON	(Msc3|Msb3|Mso1|Msp1)

// Led controls, 2 commads for 2 bits. Size 2bytes
#define MsBACKLIGHT	((Msc5|Msb4|Mso0|Msp0)<<8|(Msc5|Msb4|Mso0|Msp1))
#define MsLED_7		((Msc4|Msb7|Mso0|Msp0)<<8|(Msc4|Msb7|Mso0|Msp1))
#define MsLED_6		((Msc4|Msb6|Mso0|Msp0)<<8|(Msc4|Msb6|Mso0|Msp1))
#define MsLED_5		((Msc4|Msb5|Mso0|Msp0)<<8|(Msc4|Msb5|Mso0|Msp1))
#define MsLED_4		((Msc4|Msb4|Mso0|Msp0)<<8|(Msc4|Msb4|Mso0|Msp1))
#define MsLED_3		((Msc4|Msb3|Mso0|Msp0)<<8|(Msc4|Msb3|Mso0|Msp1))
#define MsLED_1		((Msc4|Msb1|Mso0|Msp0)<<8|(Msc4|Msb1|Mso0|Msp1))

// 8 segmen lcd fonts definition. Bit numbers according to standard 8 bit segment led panel layout 
// These are some typical characters
#define Ms0 0b00111111
#define Ms1 0b00000110
#define Ms2 0b01011011
#define Ms3 0b01001111
#define Ms4 0b01100110
#define Ms5 0b01101101
#define Ms6 0b01111101
#define Ms7 0b00000111
#define Ms8 0b01111111
#define Ms9 0b01101111
#define MsA 0b01110111
#define Msb 0b01111100
#define MsC 0b00111001
#define Msd 0b01011110
#define MsE 0b01111001
#define MsF 0b01110001
#define MsDOT 0b10000000 // Actualy there is no dot symbols. Dot segment displays "1" or ":" or nothing depending or character area.
#define MsCLEAR 0b00000000
// Add your own symbols here

// Display character enumeration. Yes, there are only six 7segment character areas.
#define MsTIMER		((MsFONT | 0)<<8) 
#define MsDRUM		((MsFONT | 1)<<8) 
#define Ms180		((MsFONT | 2)<<8) 
#define MsMINUTE	((MsFONT | 3)<<8) 
#define MsMINU10	((MsFONT | 4)<<8) 
#define MsHOUR		((MsFONT | 5)<<8) 

// Character font commands must be created by user.
// Example:
// MsHOUR|Ms0 - command for printing 0 in HOUR area
// MsHOUR|Ms1 - command for printing 1 in HOUT area
// MsHOUR|0b01010101 - comand for printing some junk in HOUT area

class MINISEL_LCD
{
	public:
	// For the sake of simplicity SPI pins are used.
	MINISEL_LCD(void);

	void on(uint16_t symbol); // Show toggable element or draw 7(+1) segment cheracter.
	void off(uint16_t symbol); // Hide toggable element
	void toggle(uint16_t symbol,bool on); // Called by on and off.
	void clear(void); // Hide all
	void all(void); // Light all possible elements (diagnostics)
	void update(void); // LCD content update. Should be called at regular intervals for image stability
	uint8_t updateCounter; // there are 4 different LCD output modes and 8 input tests for pins
	void keyCallback(void (*keyPress)(MINISEL_LCD *lcd,uint8_t buttons,uint8_t buttons_oldstate));
	void (*keyPressCallback)(MINISEL_LCD *lcd,uint8_t,uint8_t);
	private:

	// Image update latch. During image drawing wokging copy is not read to prevent corrupted picture.
	uint8_t MSLCDDataOutReady; 
	// Precomputed content of alternating image sequences.
	// There are two parts of image each in two polarities.
	uint8_t MSLCDDataACOut[4][6];
	// Working copy. 
	uint8_t MSLCDDataOut[2][6]; 
	uint8_t buttons; // Button bitmap
	uint8_t buttons_oldstate; // Previous button bitmap before change

};

#endif
