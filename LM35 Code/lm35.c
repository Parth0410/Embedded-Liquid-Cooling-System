#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include <stdio.h>

#include "LCD_16x2_header.h"

#define degree_sysmbol 0xdf

void LCD_Command (char cmd)							/* LCD command write function */
{
	LCD_Data_Port = cmd & 0xF0;							/* Write command data to LCD data port */
	LCD_Command_Port &= ~((1<<RS)|(1<<RW));			/* Make RS LOW (command reg.), RW LOW (Write) */
	LCD_Command_Port |= (1<<EN);					/* High to Low transition on EN (Enable) */
	_delay_ms(5);
	LCD_Command_Port &= ~(1<<EN);
	_delay_ms(150);
	LCD_Data_Port = cmd <<4;
	LCD_Command_Port |= (1<<EN);
        _delay_ms(5);
	LCD_Command_Port &= ~(1<<EN);
	_delay_ms(150);									/* Wait little bit */
}

void LCD_Char (char char_data)						/* LCD data write function */
{
	LCD_Data_Port = char_data & 0xF0;						/* Write data to LCD data port */
	LCD_Command_Port &= ~(1<<RW);					/* Make RW LOW (Write) */
	LCD_Command_Port |= (1<<EN)|(1<<RS);
	_delay_ms(5);                                                  /* Make RS HIGH (data reg.) and High to Low transition on EN (Enable) */
	LCD_Command_Port &= ~(1<<EN); 
        _delay_ms(150);
	LCD_Command_Port &= ~(1<<EN);
        LCD_Data_Port = char_data << 4;
        LCD_Command_Port |= (1<<EN);
        _delay_ms(5);
        LCD_Command_Port &= ~(1<<EN);
	_delay_ms(150);									/* Wait little bit */
}

void LCD_Init (void)								/* LCD Initialize function */
{
	LCD_Command_Dir = 0xFF;     // |= (1<<RS)|(1<<RW)|(1<<EN);		/* Make LCD command port direction as o/p */
	LCD_Data_Dir = 0xFF;							/* Make LCD data port direction as o/p */
	
	_delay_ms(150);	
	LCD_Command (0x0E);		                        /* LCD power up time to get things ready, it should always >15ms */
	LCD_Command (0x33);								/* Initialize 16X2 LCD in 4 bit mode */
	LCD_Command (0x32);								
	LCD_Command (0x28);								/* Auto Increment cursor */
	LCD_Command (0x01);					                       /* Clear LCD command */
	 LCD_Command (0x06); 								/**/
	 LCD_Command (0x80);								/**/
	  LCD_Command (0x0C);								/*Display ON Cursor OFF*/    
   
   }

void LCD_String (char *str)							/* Send string to LCD function */
{
	int i;
	for(i=0;str[i]!=0;i++)							/* Send each char of string till the NULL */
	{
		LCD_Char (str[i]);							/* Call LCD data write */
	}
}

void LCD_String_xy (char row, char pos, char *str)	/* Send string to LCD function */
{
	if (row == 1)
		LCD_Command((pos & 0x0F)|0x80);				/* Command of first row and required position<16 */
	else if (row == 2)
		LCD_Command((pos & 0x0F)|0xC0);				/* Command of Second row and required position<16 */
	LCD_String(str);								/* Call LCD string function */
}

void LCD_Clear()
{
	LCD_Command(0x01);								/* clear display */
}

void ADC_Init()
{										
	DEVICE_Command_Dir |= (1<<PC2);					 /* Make ADC port as Output for Peltier and FAN */
	ADCSRA = 0x87;								/* Enable ADC, with freq/128  */
	ADMUX = 0x40;								/* Vref: Avcc, ADC channel: 0 */
}

int ADC_Read(char channel)							
{
	ADMUX = 0x40 | (channel & 0x07);				/* set input channel to read */
	ADCSRA |= (1<<ADSC);							/* Start ADC conversion */
	while (!(ADCSRA & (1<<ADIF)));					/* Wait until end of conversion by polling ADC interrupt flag */
	ADCSRA |= (1<<ADIF);							/* Clear interrupt flag */
	_delay_ms(1);									/* Wait a little bit */
	return ADCW;									/* Return ADC word */
}


int main()
{
	char Water_Temp[10];
	float celsius;
	char device[10]= {'\0'};
	LCD_Init();                 /* initialize 16x2 LCD*/
	ADC_Init();                 /* initialize ADC*/
	int prev =0;
	while(1)
	{
		LCD_String_xy(1,0,"Water Temperature");
	   
		celsius = (ADC_Read(1)*4.88);
		celsius = (celsius/10.00);
		sprintf(Water_Temp,"%d%cC\t            ", (int)celsius, degree_sysmbol);               /* convert integer value to ASCII string */
		LCD_String_xy(2,0,Water_Temp);                /* send string data for printing */
		
	  
		 
	       if( celsius >= 25)
	       {
		  strcpy(device,"Device=ON ");
		  DEVICE_Command_PORT &= ~(1<<PC2);
		  //DEVICE_Command_PORT |= (1<<PC2);
		  LCD_String_xy(2,6,device); 
		  _delay_ms(1000);
		   prev = 1;
		  
	       }
	       else if(celsius <15)
	       {
		  DEVICE_Command_PORT |= (1<<PC2);
		  //DEVICE_Command_PORT &= ~(1<<PC2);
		 strcpy(device,"Device=OFF");
		  LCD_String_xy(2,6,device); 
		  _delay_ms(1000);
		  prev = 0;
		   
	       }
	       else if(prev == 1)
	       {
		   strcpy(device,"Device=ON ");
		  DEVICE_Command_PORT &= ~(1<<PC2);
		  //DEVICE_Command_PORT |= (1<<PC2);
		  LCD_String_xy(2,6,device); 
		  _delay_ms(1000);  
	       }
	      
	   
		memset(Water_Temp,0,10);
	}
	return 0;
}