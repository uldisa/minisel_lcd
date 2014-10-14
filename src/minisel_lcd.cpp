#include "Arduino.h"
#include "minisel_lcd.h"
#include <SPI.h>

/* 7 segment character area mapping to command bits for 6 enumerater character*/
uint8_t MINISEL_FONT_SEGMENT_MAP[][8] =
{
	{ // TIMER 
	Msc2|Msb6|Mso1|Msp0,
	Msc2|Msb4|Mso1|Msp1,
	Msc2|Msb4|Mso1|Msp0,
	Msc2|Msb5|Mso1|Msp0,
	Msc2|Msb6|Mso1|Msp1,
	Msc2|Msb5|Mso1|Msp1,
	Msc2|Msb3|Mso1|Msp1,
	Msc2|Msb3|Mso1|Msp0
	},
	{ // DRUM 
	Msc3|Msb2|Mso1|Msp0,
	Msc3|Msb0|Mso1|Msp1,
	Msc3|Msb0|Mso1|Msp0,
	Msc3|Msb1|Mso1|Msp0,
	Msc3|Msb2|Mso1|Msp1,
	Msc3|Msb1|Mso1|Msp1,
	Msc2|Msb7|Mso1|Msp1,
	Msc2|Msb7|Mso1|Msp0
	},
	{ // 180
	Msc3|Msb7|Mso1|Msp0,
	Msc3|Msb5|Mso1|Msp1,
	Msc3|Msb6|Mso1|Msp0,
	Msc3|Msb5|Mso1|Msp0,
	Msc3|Msb4|Mso1|Msp0,
	Msc3|Msb4|Mso1|Msp1,
	Msc3|Msb6|Mso1|Msp1,
	Msc3|Msb7|Mso1|Msp1
	},
	{ // Minutes
	MsBit_IGNORE,
	Msc1|Msb0|Mso1|Msp1,
	Msc1|Msb1|Mso1|Msp1,
	Msc5|Msb0|Mso1|Msp1,
	Msc0|Msb0|Mso1|Msp0,
	Msc5|Msb0|Mso1|Msp0,
	Msc1|Msb0|Mso1|Msp0,
	Msc1|Msb1|Mso1|Msp0
	},
	{ // Minutes 10
	MsBit_IGNORE,
	Msc1|Msb2|Mso1|Msp1,
	Msc1|Msb3|Mso1|Msp1,
	Msc0|Msb7|Mso1|Msp1,
	Msc0|Msb0|Mso1|Msp1,
	Msc0|Msb7|Mso1|Msp0,
	Msc1|Msb2|Mso1|Msp0,
	Msc1|Msb3|Mso1|Msp0
	},
	{ // Hours
	Msc0|Msb6|Mso1|Msp0,
	Msc1|Msb4|Mso1|Msp1,
	Msc1|Msb5|Mso1|Msp1,
	Msc0|Msb5|Mso1|Msp1,
	Msc0|Msb6|Mso1|Msp1,
	Msc0|Msb5|Mso1|Msp0,
	Msc1|Msb4|Mso1|Msp0,
	Msc1|Msb5|Mso1|Msp0
	}
};
MINISEL_LCD::MINISEL_LCD(void) {	
	updateCounter=0;
	MSLCDDataOutReady=false; 
	clear();
	SPI.begin();
}
void MINISEL_LCD::on(uint16_t symbol){
	toggle(symbol,true);
}
void MINISEL_LCD::off(uint16_t symbol){
	toggle(symbol,false);
}
void MINISEL_LCD::toggle(uint16_t symbol,bool on){
	uint8_t i; //counter
	uint8_t bit; //Current bit
	uint8_t count; //count of bits
	uint8_t bits; //Bitmap
	uint8_t command; //current command
	uint8_t *translation; //Bitmap command set;
	bits=0b10000000;
	count=1;
	translation=(uint8_t *)&symbol; // One byte command
	if(symbol&0xff00) {
		bits=0b11000000;
		count=2;
		translation=(uint8_t *)&symbol; // Two commands
		if(((symbol>>8)&0xe0)==MsFONT){
			bits=symbol&0xff;
			count=8;
			translation=MINISEL_FONT_SEGMENT_MAP[(symbol>>8)&0x17];
		}	
	}

	MSLCDDataOutReady=false;
	for(i=0;i<count;i++){
		// MSB firts
		command=translation[i];
		if(command==MsBit_IGNORE){
			continue;
		}
		bit=((bits<<i)&0x80);
		if(((command&0x02 && on) || (!(command&0x02) && !on)) && bit ){
			bitSet(MSLCDDataOut[command&0x01][(command&0xe0)>>5],(command&0x1c)>>2);
		} else {
			bitClear(MSLCDDataOut[command&0x01][(command&0xe0)>>5],(command&0x1c)>>2);
		}
	}
	MSLCDDataOutReady=true;
}
void MINISEL_LCD::clear(void){

	MSLCDDataOutReady=false;
	MSLCDDataOut[0][0]=MSLCDDataOut[1][0]=0x00;
	MSLCDDataOut[0][1]=MSLCDDataOut[1][1]=0x00;
	MSLCDDataOut[0][2]=MSLCDDataOut[1][2]=0x00;
	MSLCDDataOut[0][3]=MSLCDDataOut[1][3]=0x00;
	MSLCDDataOut[0][4]=MSLCDDataOut[1][4]=0xff;
	MSLCDDataOut[0][5]=MSLCDDataOut[1][5]=0x10;
	MSLCDDataOut[0][0]=(MSLCDDataOut[0][0]&0b11100011) | 0b00010000;
	MSLCDDataOut[1][0]=(MSLCDDataOut[1][0]&0b11100011) | 0b00001000;
	MSLCDDataOut[0][0]=(MSLCDDataOut[0][0]&0b11100011) | 0b00010100;
	MSLCDDataOut[1][0]=(MSLCDDataOut[1][0]&0b11100011) | 0b00001100;
	MSLCDDataOutReady=true;

}
/*void MINISEL_LCD::all(void){
	MSLCDDataOutReady=false;
	MSLCDDataOut[0][0]=MSLCDDataOut[1][0]=0xff;
	MSLCDDataOut[0][1]=MSLCDDataOut[1][1]=0xff;
	MSLCDDataOut[0][2]=MSLCDDataOut[1][2]=0xff;
	MSLCDDataOut[0][3]=MSLCDDataOut[1][3]=0xff;
	MSLCDDataOut[0][4]=MSLCDDataOut[1][4]=0x00;
	MSLCDDataOut[0][5]=MSLCDDataOut[1][5]=0x0f;
	MSLCDDataOut[0][0]=(MSLCDDataOut[0][0]&0b11100011) | 0b00010000;
	MSLCDDataOut[1][0]=(MSLCDDataOut[1][0]&0b11100011) | 0b00001000;
	MSLCDDataOutReady=true;
}*/
void MINISEL_LCD::update(void) {
	uint8_t i;
	digitalWrite(SS,LOW);
	SPI.transfer(0x00);
	SPI.transfer(0x00);
	SPI.transfer(0x00);
	SPI.transfer(0x00);
	SPI.transfer(0x01<<(updateCounter&0x7));
	SPI.transfer(0x10);
	digitalWrite(SS,HIGH);
	digitalWrite(SS,LOW);
	for(i=0;i<6;i++) {
		SPI.transfer(MSLCDDataACOut[updateCounter&0x3][i]);
	}
	// Use MISO pin for reading pushbottons. Values are not read by SPI because
	// LCS panel output is not controled by clock but by SS (latch_pin).
	if(digitalRead(MISO)) {
		buttons|=0x01<<(updateCounter&0x07);
	} else {
		buttons&=~(0x01<<(updateCounter&0x07));
	}
	digitalWrite(SS,HIGH);
	updateCounter++;
	if(!(updateCounter&0x3) && MSLCDDataOutReady==true) {
		MSLCDDataOutReady=false; // Copy only if changes are available
		// Assume, update is triggered at regular intervals
		// Now its time to update data

		for(i=0;i<6;i++) {
			MSLCDDataACOut[0][i]=MSLCDDataOut[0][i];
			MSLCDDataACOut[1][i]=MSLCDDataOut[1][i];
			MSLCDDataACOut[2][i]=~MSLCDDataOut[0][i];
			MSLCDDataACOut[3][i]=~MSLCDDataOut[1][i];
			}	
		// Put back LED power pin values
		MSLCDDataACOut[2][4]=MSLCDDataACOut[0][4];
		MSLCDDataACOut[3][4]=MSLCDDataACOut[1][4];
		MSLCDDataACOut[2][5]=(MSLCDDataACOut[0][5]&0b00010000) | (MSLCDDataACOut[2][5]&0b11101111);
		MSLCDDataACOut[3][5]=(MSLCDDataACOut[1][5]&0b00010000) | (MSLCDDataACOut[3][5]&0b11101111);

		MSLCDDataACOut[0][0]=(MSLCDDataACOut[0][0]&0b11100011) | 0b00010000;
		MSLCDDataACOut[1][0]=(MSLCDDataACOut[1][0]&0b11100011) | 0b00001000;
		MSLCDDataACOut[2][0]=(MSLCDDataACOut[2][0]&0b11100011) | 0b00001100;
		MSLCDDataACOut[3][0]=(MSLCDDataACOut[3][0]&0b11100011) | 0b00010100;
	}
	if(buttons!=buttons_oldstate && keyPressCallback) {
		(*keyPressCallback)(this,buttons,buttons_oldstate);
		buttons_oldstate=buttons;
	}
}	
void MINISEL_LCD::keyCallback(void (*keyPress)(MINISEL_LCD *lcd,uint8_t buttons,uint8_t buttons_oldstate)) {
	keyPressCallback=keyPress;
}
