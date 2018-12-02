//Sample using LiquidCrystal library
#include <LiquidCrystal.h>
#include <IRremote.h>

int RECV_PIN = 2; 
IRrecv irrecv(RECV_PIN);
decode_results results;

IRsend irsend;

int mode = 2;

// select the pins used on the LCD panel
LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5
int key         = btnNONE;
int key_cont    = 0;

int code1 = 0;
int code2 = 0;

// read the buttons
int read_LCD_buttons()
{
 adc_key_in = analogRead(0);      // read the value from the sensor 
 // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
 // we add approx 50 to those values and check to see if we are close
 if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
 // For V1.1 us this threshold
 if (adc_key_in < 50)   return btnRIGHT;  
 if (adc_key_in < 250)  return btnUP; 
 if (adc_key_in < 450)  return btnDOWN; 
 if (adc_key_in < 650)  return btnLEFT; 
 if (adc_key_in < 850)  return btnSELECT;  

 // For V1.0 comment the other threshold and use the one below:
/*
 if (adc_key_in < 50)   return btnRIGHT;  
 if (adc_key_in < 195)  return btnUP; 
 if (adc_key_in < 380)  return btnDOWN; 
 if (adc_key_in < 555)  return btnLEFT; 
 if (adc_key_in < 790)  return btnSELECT;   
*/
 return btnNONE;  // when all others fail, return this...
}

void setup()
{
 lcd.begin(16, 2);              // start the library
 lcd.setCursor(0,0);
 irrecv.enableIRIn(); 
}

long nec_code(int i, int j)
{
  long a = ((long)i&0xFF)<<24 | (~(long)i&0xFF)<<16 | ((long)j&0xFF)<<8 | (~(long)j&0xFF);
  return a;
}

void loop()
{
  if (mode == 1) {
    long a = 0;
    for (int i=4; i<5; i++) {
      for (int j=0; j<=0xff; j++) {
        //a = ((i<<24)&0xff000000) | ((~i<<16)&0xff0000) | ((j<<8)&0xff00) | ((~j)&0xff);
        a = ((long)i&0xFF)<<24 | (~(long)i&0xFF)<<16 | ((long)j&0xFF)<<8 | (~(long)j&0xFF);
        //lcd.clear();
        lcd.setCursor(0,1);
        lcd.print(a, HEX);
        irsend.sendNEC(a, 32);
        delay(1000);
      }
    }
  } else if (mode == 2) {
     lcd_key = read_LCD_buttons();

     if (lcd_key != btnNONE && key == lcd_key) {
       key_cont++;
     } else {
       key_cont = 0;
     }
     
     // trigger
     if (((key != btnNONE) && key_cont > 3) && ((lcd_key != key) || (key_cont % 10 == 9))) {
       lcd.setCursor(1, 0);
       switch (key)
       {
         case btnRIGHT:
           code1++;
           break;
         case btnLEFT:
           code1--;
           break;
         case btnUP:
           code2++;
           break;
         case btnDOWN:
           code2--;
           break;
         case btnSELECT:
           irsend.sendNEC(nec_code(code1, code2), 32);
           break;
       }
     }
     lcd.setCursor(0, 1);
     lcd.print(nec_code(code1, code2), HEX);
     delay(10);
     key = lcd_key;
     
  } else {
     //lcd.setCursor(9,1);            // move cursor to second line "1" and 9 spaces over
     //lcd.print(millis()/1000);      // display seconds elapsed since power-up
      if (irrecv.decode(&results)) {
        if (results.value != 0xFFFFFFFF) {
          lcd.clear();
          lcd.setCursor(0,1);
          if (results.decode_type == NEC) {
            lcd.print("NEC: ");
          } else if (results.decode_type == SONY) {
            lcd.print("SNY: ");
          } else if (results.decode_type == RC5) {
            lcd.print("RC5: ");
          } else if (results.decode_type == RC6) {
            lcd.print("RC6: ");
          } else if (results.decode_type == UNKNOWN) {
            lcd.print("UND: ");
          }
          lcd.setCursor(5,1);
          lcd.print(results.value, HEX);
        }
        irrecv.resume(); 
      }
     
     lcd.setCursor(0,1);            // move to the begining of the second line
     lcd_key = read_LCD_buttons();  // read the buttons
    
     switch (lcd_key)               // depending on which button was pushed, we perform an action
     {
       case btnRIGHT:
         lcd.print("RIGHT ");
         break;
       case btnLEFT:
         lcd.print("LEFT   ");
         break;
       case btnUP:
         lcd.print("UP    ");
         break;
       case btnDOWN:
         lcd.print("DOWN  ");
         break;
       case btnSELECT:
         lcd.print("SELECT");
         break;
         case btnNONE:
         //lcd.print("NONE  ");
         break;
     }
  }
}
