/* Condensed version of the Dophin O Matic using a HCMODU 95
 * containing a TM1638 IC which combines the LED, LED 8 digit 7 seg display
 * and 8 push buttons all accessed via 3 pins :-)
 * The only issue is the LEDs and buttons go from left to right
 * So LED 1 and switch 1 are the reverse of the V1 arrangement
 * By using some borrowed assembler, the input value is flipped
 * from LSB to MSB format
 */



// 1306 Display connects SDA Analog 4, SCL - Analog 5
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

/* TM1638 Library */
#include "HCTM1638.h"

/* Set the digital pins used to interface to the module */
#define STB 10 // Strobe pin
#define CLK 9 // Clock pin
#define DIO 8 // Data pin
byte  buttonVal;
byte oldVal;
byte fishWanted;
/* Create an instance of the library */
HCTM1638 HCTM1638(STB, DIO, CLK);


void showCurrentFish(byte byteVal)
{
    Serial.println(F("Bit Status:"));
    for(byte i = 0; i < 8; i++)
    {
        Serial.print(F("  Bit-"));
        Serial.print(i);
        Serial.print(F(": "));
        if((byteVal >> i) & 1)
            Serial.println(F("HIGH"));
        else
            Serial.println(F("LOW"));       
    }
    Serial.println();
}

void showLCD(int value)
{
  display.setTextSize(2);
  display.clearDisplay();     
  display.setCursor(0,0); 
  display.print (F("I wish for"));
  display.setCursor(0,18); 
  display.setTextSize(3);
  display.print(value);
  display.setTextSize(2);
  display.setCursor(0,44);
  display.print (F("fish"));
  display.display(); 
}




void showLeds(byte value)
{
  Serial.print("LED val is ");
  Serial.println(value);
  for(byte i = 0;i<8;i ++)
  {
    if ((value>> i) & 1)
    {
      HCTM1638.SetLED(8 - i, true);
    }
    else
    {
      HCTM1638.SetLED(8 - i, false);
    }
  }   
}
 


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Init screen

    display.clearDisplay();     
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.setCursor(0,0); 
    display.print(F("DolphinOMatic V2.0!"));
    display.display();
    
  HCTM1638.Brightness(0x07);
  byte index;
  for (index = 0; index < 44; index++)
  {
    HCTM1638.Clear();    
    HCTM1638.print7Seg("Dolphin o matic Ver 2.0 Joe Deller", index);
    delay(130); 
  }
  
  HCTM1638.print7Seg("FISH", 8);
  showLCD(0);
  delay (1500);

}

void showDisplay(byte value)
{
  //HCTM1638.Clear();
  char sval [3];
  sprintf(sval, "%03d", value);
  
  HCTM1638.print7Seg("FISH", 8);
  HCTM1638.print7Seg(sval, 3); 
}

void loop() {
  // put your main code here, to run repeatedly:
 // On the module buttons are labeled S1-8 from left to right
 // So values are LSB FIRST !  Use the bitswap to sort this out
 buttonVal = HCTM1638.ReadButtons();
 if (buttonVal != 0)
 {
  buttonVal = bitswap(buttonVal);
  // XOR as we are using push buttons, if bit was set last time turn it off and vice versa
  buttonVal = (buttonVal ^ oldVal); 
  showCurrentFish(buttonVal);  
  showLeds(buttonVal);
  showDisplay (buttonVal);
  showLCD(buttonVal);
  oldVal = buttonVal;
  delay(100);
 }
}

unsigned char bitswap (unsigned char x)
{
  // http://forum.arduino.cc/index.php?topic=42559.0
  // One reg's bits get left shifted into carry
  // the next gets the carry bit right shifted into it
  // repeat until done
 byte result;

   asm("mov __tmp_reg__, %[in] \n\t"
     "lsl __tmp_reg__  \n\t"   /* shift out high bit to carry */
     "ror %[out] \n\t"  /* rotate carry __tmp_reg__to low bit (eventually) */
     "lsl __tmp_reg__  \n\t"   /* 2 */
     "ror %[out] \n\t"
     "lsl __tmp_reg__  \n\t"   /* 3 */
     "ror %[out] \n\t"
     "lsl __tmp_reg__  \n\t"   /* 4 */
     "ror %[out] \n\t"
     
     "lsl __tmp_reg__  \n\t"   /* 5 */
     "ror %[out] \n\t"
     "lsl __tmp_reg__  \n\t"   /* 6 */
     "ror %[out] \n\t"
     "lsl __tmp_reg__  \n\t"   /* 7 */
     "ror %[out] \n\t"
     "lsl __tmp_reg__  \n\t"   /* 8 */
     "ror %[out] \n\t"
     : [out] "=r" (result) : [in] "r" (x));
     return(result);
}
