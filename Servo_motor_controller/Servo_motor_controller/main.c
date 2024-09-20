/*
 * ServoMotorControll.c
 *
 * Created: 19/09/2024 01:47:45 م
 * Author : Meteor
 */ 

#define F_CPU 16000000UL

#include "../MCAL/DIO_interface.h"
#include "../HAL/KPD_interface.h"
#include "../HAL/LCD_interface.h"
#include "../MCAL/PWM1_interface.h"

#include <string.h>

#define MAX_ARRAY_SIZE		17
#define FrequencyHz			50

void voidDecoderCalc(u8 u8_input_string[], s8 *s8_output);

void voidCalcDuty(s8 *u8_input, f32 *s32_output);

int main(void)
{
	// Initialization
	PWM1_voidInitChannel1A();
	KPD_voidInit();
	LCD_voidInit();
	
	LCD_voidGoToSpecificPosition(1, 0);
	LCD_voidDisplayString((u8 *)"Angle Now:");
	// OCR1A PIN set output
	DIO_voidSetPinDirection(DIO_PORTD, DIO_PIN5, DIO_PIN_OUTPUT);
    s8 servoAngle = 0;
	f32 dutyOut = 0;
	u8 keyPressed = KPD_NOT_PRESSED;
	u8 input[MAX_ARRAY_SIZE];
	u8 index = 0;
    while (1) 
    {
		KPD_voidGetValue(&keyPressed);
		switch(keyPressed){
			case KPD_NOT_PRESSED:
				break;
			case '*':
				
				break;
			case 'c':
				LCD_voidClear();
				index = 0;
				memset(input, '\0', MAX_ARRAY_SIZE);
				LCD_voidGoToSpecificPosition(1, 0);
				LCD_voidDisplayString((u8 *)"Angle Now:");
				//servoAngle = 0;
				break;
			case '=':
				voidDecoderCalc(input, &servoAngle);
				voidCalcDuty(&servoAngle, &dutyOut);
				LCD_voidGoToSpecificPosition(1, 10);
				LCD_voidDisplayString((u8 *)"     ");//empty to clear the display
				LCD_voidGoToSpecificPosition(1, 10);
				LCD_voidDisplayNumber(servoAngle);
				break;
			default:
				if((index < MAX_ARRAY_SIZE)){
					// inside the 16 digit bound
					input[index] = keyPressed;
					input[index + 1] = '\0';
					index++;
					LCD_voidGoToSpecificPosition(0, 0);
					LCD_voidDisplayString(input);
			}
			else{
				// out of bounds
			}
			break;
		}
		PWM1_voidGeneratePWM_channel1A((u16)FrequencyHz, dutyOut);
    }
}

// linear interpolation formula
void voidCalcDuty(s8 *u8_input, f32 *s32_output){
	*s32_output = (5.00 + 0.02778 * ((*u8_input) + 90) );
	// rounding to the nearest 2
	*s32_output = (int)(*s32_output * 100 + 0.5) / 100.0;
};


void voidDecoderCalc(u8 u8_input_string[], s8 *s8_output) {
	u8 counter = 0;
	s8 num = 0;
	char current_operator = '+';

	// If the expression starts with a negative sign, set the initial operator to '-'
	if (u8_input_string[0] == '-') {
		current_operator = '-';
		counter++;
	}
	
	while (u8_input_string[counter] != '\0') {
		num = 0;

		// Convert consecutive digits into a number
		while (u8_input_string[counter] >= '0' && u8_input_string[counter] <= '9') {
			num = num * 10 + (u8_input_string[counter] - '0');
			counter++;
		}
		switch (current_operator) {
			case '-':
				num = -num;
				break;
			default:
				break;
		}
		counter++;
	}
	// check for the number
	if(num > 90){
		num = 90;
	}else if(num < -90){
		num = -90;
	}
	// Write the result to the output
	*s8_output = num;
}
