#include <minisel_lcd.h>

MINISEL_LCD lcd;
void setup() {
	cli();
	// Set CPU prescaler to 4Mhz
	CLKPR = 0b10000000;
	CLKPR = 0b00000010;
  	//Set Timer2 for LCD update because lower priority Timer0 is used by millisleep.
	TCCR2A = 0b00000000;	//Normal Timer2 mode.
	TCCR2B = 0b00000100;	//Prescale 16Mhz/64
	TIMSK2 = 0b00000001;	//Enable overflow interrupt
	sei();
	lcd.keyCallback(&keyHandler);
	lcd.on(MsBACKLIGHT);
}
ISR(TIMER2_OVF_vect)
{
	lcd.update();
}


uint16_t icon_show[] = {
MsTIMER_ICON,	
MsPREWASH_ICON,
MsWASH_ICON,
MsRINSE_ICON,
MsDRUM_ICON,
MsSUN_ICON,
Ms180_ICON,
MsTIMER_BOX,
MsPREWASH_BOX,
MsWASH_BOX,
MsRINSE_BOX,
MsDRUM_BOX,
MsSUN_BOX,
Ms180_BOX
};

uint16_t small_numbers[]={
	MsTIMER,
	MsDRUM,
	Ms180
};
uint8_t numbers_19[]={
Ms0,
Ms1,
Ms2,
Ms3,
Ms4,
Ms5,
Ms6,
Ms7,
Ms8,
Ms9,
MsDOT|Ms0,
MsDOT|Ms1,
MsDOT|Ms2,
MsDOT|Ms3,
MsDOT|Ms4,
MsDOT|Ms5,
MsDOT|Ms6,
MsDOT|Ms7,
MsDOT|Ms8,
MsDOT|Ms9
};

uint16_t numbers_F[] = {
Ms0,
Ms1,
Ms2,
Ms3,
Ms4,
Ms5,
Ms6,
Ms7,
Ms8,
Ms9,
MsA,
Msb,
MsC,
Msd,
MsE,
MsF
};
uint16_t clock[8][2] = {
	{MsMINU10|0b00000001,MsMINUTE|0b00000001},
	{MsMINU10|0b00000000,MsMINUTE|0b00000011},
	{MsMINU10|0b00000000,MsMINUTE|0b00000110},
	{MsMINU10|0b00000000,MsMINUTE|0b00001100},
	{MsMINU10|0b00001000,MsMINUTE|0b00001000},
	{MsMINU10|0b00011000,MsMINUTE|0b00000000},
	{MsMINU10|0b00110000,MsMINUTE|0b00000000},
	{MsMINU10|0b00100001,MsMINUTE|0b00000000}
};
uint16_t led[]={
	MsBit_IGNORE,
	MsLED_1,
	MsBit_IGNORE,
	MsLED_3,
	MsLED_4,
	MsLED_5,
	MsLED_6,
	MsLED_7
};

void keyHandler(MINISEL_LCD *lcd,uint8_t buttons,uint8_t buttons_oldstate) {
	uint8_t i;
	lcd->on(MsMINU10|numbers_F[buttons>>4]);	
	lcd->on(MsMINUTE|numbers_F[buttons&0xf]);	
	for(i=0;i<8;i++) {
		if((buttons>>i)&1) {
			lcd->on(led[i]);
		} else {
			lcd->off(led[i]);
		}
	}
}

void loop() {
	int i,j;
	// Show icons
	for(j=1;j<6;j++) {
		for(i=0;i<sizeof(icon_show)/sizeof(icon_show[0]);i++) {
			lcd.toggle(icon_show[i],j&1);
			delay(10);
		}
	}
	// Count simple numbers
	for(j=0;j<sizeof(small_numbers)/sizeof(small_numbers[0]);j++) {
		for(i=0;i<sizeof(numbers_19)/sizeof(numbers_19[0]);i++) {
			lcd.on(small_numbers[j]|numbers_19[i]);
			delay(10);
		}
	}
	// Hide icons
	for(i=0;i<sizeof(icon_show)/sizeof(icon_show[0]);i++) {
		lcd.off(icon_show[i]);
		delay(10);
	}	
	// Count down small numbers and clear
	for(j=0;j<sizeof(small_numbers)/sizeof(small_numbers[0]);j++) {
		for(i=sizeof(numbers_19)/sizeof(numbers_19[0])-1;i>=0;i--) {
			lcd.on(small_numbers[j]|numbers_19[i]);
			delay(10);
		}
		lcd.on(small_numbers[j]|MsCLEAR);
		delay(10);
	}
	// Count simple numbers
	for(i=0;i<0x1000;i+=0x111) {
		lcd.on(MsHOUR|numbers_F[(i&0xF00)>>8]);
		lcd.on(MsMINU10|numbers_F[(i&0xF0)>>4]);
		lcd.on(MsMINUTE|numbers_F[(i&0xF)]);
		delay(30);
	}
	// Custom font
	for(i=0;i<50;i++) {
		if(i&0x8){
			lcd.on(MsHOUR|MsDOT);
		} else {
			lcd.on(MsHOUR|MsCLEAR);
		}	
		lcd.on(clock[i&0x7][0]);
		lcd.on(clock[i&0x7][1]);
		delay(10);
	}
	lcd.on(MsHOUR|MsCLEAR);
	lcd.on(MsMINU10|MsCLEAR);
	lcd.on(MsMINUTE|MsCLEAR);
	// Night rider
	for(i=0;i<50;i++) {
		for(j=0;j<8;j++) {
			if((0x380>>(abs(i%14-7)+j))&1){
				lcd.on(led[j]);
			} else {
				lcd.off(led[j]);
			}
		}
		delay(20);
	}
	for(j=0;j<8;j++) {
		lcd.off(led[j]);
	}
}
