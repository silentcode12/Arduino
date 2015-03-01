//Adapted from SunFounder project LCD1602

#include <LiquidCrystal.h>

char array1[]="Why hello there...";  //the string to print on the LCD
char array2[]="Love you XOXOXOXOXOXOX";  //the string to print on the LCD
              
int tim = 500;  //the value of delay time
// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(4, 6, 10, 11, 12, 13);

byte heart[8] = {
  B00000,
  B01010,
  B10101,
  B10001,
  B01010,
  B00100,
  B00000,
};

void setup()
{
  lcd.begin(16, 2);  // set up the LCD's number of columns and rows: 
  
  lcd.createChar(0, heart);
}

void loop() 
{  
    lcd.clear();    
    lcd.setCursor(16,0);  // set the cursor to column 15, line 0
    for (int pos = 0; pos < 19; pos++)
    {          
      lcd.print(array1[pos]);  // Print a message to the LCD.
       delay(tim);  //wait for 250 microseconds
       lcd.scrollDisplayLeft();  //Scrolls the contents of the display one space to the left.
    }
    
    lcd.clear();
    lcd.setCursor(16,1);  // set the cursor to column 15, line 0
    for (int pos = 0; pos < 16; pos++)
    {          
      lcd.print(array2[pos]);  // Print a message to the LCD.
       delay(tim);  //wait for 250 microseconds
       lcd.scrollDisplayLeft();  //Scrolls the contents of the display one space to the left.
    }      
    
    delay(2000);
    lcd.clear();
    for(int row = 0; row < 2; row++)
    for(int col = 0; col < 16; col++)
    {      
      lcd.setCursor(col, row);
      lcd.write((uint8_t)0);
    }
    
    delay(2000);
}
