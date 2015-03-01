//Each character for the 8x8 display is 8 bytes in size.  
//The Arduino long data type is four bytes, so two longs can represent a single character.
//This table is a 1 bit per pixel lookup for standard ASCII characters (plus some "special" additions)..
const unsigned long fontTable[] =
{
    //my own creations
    0xbdff7e18, 0x815a24ff, //alien
    0x7e4a3c82, 0x6681bde7, //bulldog
    0xffff6600, 0x183c3c7e, //heart
  
    //taken from http://www.coranac.com/tonc/text/text.htm. A great write up!
    0x00000000, 0x00000000, 0x18181818, 0x00180018, 0x00003636, 0x00000000, 0x367F3636, 0x0036367F, 
    0x3C067C18, 0x00183E60, 0x1B356600, 0x0033566C, 0x6E16361C, 0x00DE733B, 0x000C1818, 0x00000000, 
    0x0C0C1830, 0x0030180C, 0x3030180C, 0x000C1830, 0xFF3C6600, 0x0000663C, 0x7E181800, 0x00001818, 
    0x00000000, 0x0C181800, 0x7E000000, 0x00000000, 0x00000000, 0x00181800, 0x183060C0, 0x0003060C, 
    0x7E76663C, 0x003C666E, 0x181E1C18, 0x00181818, 0x3060663C, 0x007E0C18, 0x3860663C, 0x003C6660, 
    0x33363C38, 0x0030307F, 0x603E067E, 0x003C6660, 0x3E060C38, 0x003C6666, 0x3060607E, 0x00181818,
    0x3C66663C, 0x003C6666, 0x7C66663C, 0x001C3060, 0x00181800, 0x00181800, 0x00181800, 0x0C181800, 
    0x06186000, 0x00006018, 0x007E0000, 0x0000007E, 0x60180600, 0x00000618, 0x3060663C, 0x00180018, 

    0x5A5A663C, 0x003C067A, 0x7E66663C, 0x00666666, 0x3E66663E, 0x003E6666, 0x06060C78, 0x00780C06, 
    0x6666361E, 0x001E3666, 0x1E06067E, 0x007E0606, 0x1E06067E, 0x00060606, 0x7606663C, 0x007C6666, 
    0x7E666666, 0x00666666, 0x1818183C, 0x003C1818, 0x60606060, 0x003C6660, 0x0F1B3363, 0x0063331B, 
    0x06060606, 0x007E0606, 0x6B7F7763, 0x00636363, 0x7B6F6763, 0x00636373, 0x6666663C, 0x003C6666, 
    0x3E66663E, 0x00060606, 0x3333331E, 0x007E3B33, 0x3E66663E, 0x00666636, 0x3C0E663C, 0x003C6670, 
    0x1818187E, 0x00181818, 0x66666666, 0x003C6666, 0x66666666, 0x00183C3C, 0x6B636363, 0x0063777F, 
    0x183C66C3, 0x00C3663C, 0x183C66C3, 0x00181818, 0x0C18307F, 0x007F0306, 0x0C0C0C3C, 0x003C0C0C, 
    0x180C0603, 0x00C06030, 0x3030303C, 0x003C3030, 0x00663C18, 0x00000000, 0x00000000, 0x003F0000, 

    0x00301818, 0x00000000, 0x603C0000, 0x007C667C, 0x663E0606, 0x003E6666, 0x063C0000, 0x003C0606, 
    0x667C6060, 0x007C6666, 0x663C0000, 0x003C067E, 0x0C3E0C38, 0x000C0C0C, 0x667C0000, 0x3C607C66, 
    0x663E0606, 0x00666666, 0x18180018, 0x00301818, 0x30300030, 0x1E303030, 0x36660606, 0x0066361E,
    0x18181818, 0x00301818, 0x7F370000, 0x0063636B, 0x663E0000, 0x00666666, 0x663C0000, 0x003C6666, 
    0x663E0000, 0x06063E66, 0x667C0000, 0x60607C66, 0x663E0000, 0x00060606, 0x063C0000, 0x003E603C, 
    0x0C3E0C0C, 0x00380C0C, 0x66660000, 0x007C6666, 0x66660000, 0x00183C66, 0x63630000, 0x00367F6B, 
    0x36630000, 0x0063361C, 0x66660000, 0x0C183C66, 0x307E0000, 0x007E0C18, 0x0C181830, 0x00301818, 
    0x18181818, 0x00181818, 0x3018180C, 0x000C1818, 0x003B6E00, 0x00000000, 0x00000000, 0x00000000, 
};

//Circuit design and code adapted from the SunFounder super kit project 12 Dot matrix Display
const int latchPin = 8; //Pin connected to ST_CP of 74HC595
const int clockPin = 12; //Pin connected to SH_CP of 74HC595 
const int dataPin = 11; //Pin connected to DS of 74HC595 

void setup()
{
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
}

//Writes out eight bytes of data to fill the 8x8 LED display
void DisplayCharacter(unsigned long rows1_4, unsigned long rows5_8)
{
  //Start the row select at binary 10000000.  The will be shifted to the right as each row is displayed.
  int rowSelect = 0x80;
  
  for (int fourRowSelect = 0; fourRowSelect < 2; fourRowSelect++)
  {
    //Process first four rows and then the second for rows.
    unsigned long fourRows = fourRowSelect == 0 ? ~rows1_4 : ~rows5_8;  //For common anode negate the bits so that the characters are not negative.
    
    //Cycle through the four bytes of data
    for (int rowIndex = 0; rowIndex < 4; rowIndex++)
    {
        digitalWrite(latchPin, LOW); //Set low while writing out data.
        
        unsigned char row = fourRows >> ((rowIndex) * 8);  //shift out can only write a single byte.  Extract each successive byte with each pass.

        shiftOut(dataPin, clockPin, LSBFIRST, row);        
        shiftOut(dataPin, clockPin, LSBFIRST, rowSelect);    
        
        rowSelect = rowSelect >> 1;  //Select the next row
       
        digitalWrite(latchPin, HIGH); //Set high to latch the data to the output       
        
        //for fun play with the delay to see each row get written.
        //delay(250);
    }
  }
}

unsigned long lastMillis = 0;
int fontCharacterIndex = 0;

//After the duration has been exceeded the fontCharacterIndex is incremented to the next character.
void ProgressToNextCharacterAfterDuration(int duration)
{
    unsigned long currentMillis = millis();
    
    if (currentMillis - lastMillis > duration)
    {
      lastMillis = currentMillis;
      fontCharacterIndex += 2;
      
      if (fontCharacterIndex >= 198)
      {
        fontCharacterIndex = 0;
      }
    }
}

void loop()
{
    DisplayCharacter(fontTable[fontCharacterIndex], fontTable[fontCharacterIndex+1]);
    
    ProgressToNextCharacterAfterDuration(500);
}
