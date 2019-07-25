
#ifndef LCD_16x2_header					/* Define library H file if not defined */
#define LCD_16x2_header


#define LCD_Data_Dir DDRD				/* Define LCD data port direction */
#define LCD_Command_Dir DDRB			/* Define LCD command port direction register */
#define LCD_Data_Port PORTD				/* Define LCD data port */
#define LCD_Command_Port PORTB			/* Define LCD command port */
#define DEVICE_Command_Dir DDRC                                              /*Direction for the Peltier & FAN*/
#define DEVICE_Command_PORT PORTC		/*PORT for the Peltier & FAN*/
#define EN PB0					/* Define Enable signal pin */
#define RW PB3	       /*NO NEED, GROUNDED IN THE CIRCUIT*/		/* Define Read/Write signal pin */
#define RS PB1							/* Define Register Select (data reg./command reg.) signal pin */

void LCD_Command (char);				/* LCD command write function */
void LCD_Char (char);					/* LCD data write function */
void LCD_Init (void);					/* LCD Initialize function */
void LCD_String (char*);				/* Send string to LCD function */
void LCD_String_xy (char,char,char*);	/* Send row, position and string to LCD function */
void LCD_Clear();						/* Clear LCD*/

#endif