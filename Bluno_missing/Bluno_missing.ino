/*
 * TimeRTC.pde
 * Example code illustrating Time library with Real Time Clock.
 * This example is identical to the example provided with the Time Library,
 * only the #include statement has been changed to include the DS3232RTC library.
 */
#include <DS3232RTC.h>    //http://github.com/JChristensen/DS3232RTC
#include <Time.h>         //http://www.arduino.cc/playground/Code/Time  
#include <Wire.h>         //http://arduino.cc/en/Reference/Wire (included with Arduino IDE)
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH1106.h>
// If using software SPI (the default case):
#define OLED_MOSI   11
#define OLED_CLK   13
#define OLED_DC    12
#define OLED_CS    10
#define OLED_RESET -1
Adafruit_SH1106 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);
#include <DHT.h>
#define DHTPIN 5
#define DHTTYPE DHT11 
DHT dht(DHTPIN, DHTTYPE); 
float h,t;     
/* Uncomment this block to use hardware SPI
#define OLED_DC     6
#define OLED_CS     7
#define OLED_RESET  8
Adafruit_SSD1306 display(OLED_DC, OLED_RESET, OLED_CS);
*/

static const unsigned char PROGMEM mop_bmp[] =
{
0x3F,0xF8,0xC1,0x1F,0xF8,0x3F,0x3F,0xFC,0xF1,0x7F,0xF8,0x7F,0x3F,0xFC,0xF9,0xFF,
0xF8,0xFF,0x3F,0xFC,0xF9,0xF0,0x79,0xF8,0x7F,0xFC,0x3D,0xE0,0x79,0xF0,0x7F,0xFE,
0x3D,0xE0,0x7B,0xF0,0x7F,0xFE,0x3D,0xC0,0x7B,0xF8,0xFF,0xEE,0x3F,0xC0,0xFB,0xFF,
0xFF,0xEF,0x3F,0xC0,0xFB,0x7F,0xFF,0xEF,0x3D,0xC0,0xFB,0x3F,0xEF,0xEF,0x3D,0xE0,
0x7B,0x00,0xEF,0xE7,0x7D,0xE0,0x79,0x00,0xEF,0xE7,0xF9,0xF0,0x79,0x00,0xEF,0xE7,
0xF9,0xFF,0x78,0x00,0xCF,0xE3,0xF1,0x7F,0x78,0x00,0xCF,0xE3,0xC1,0x1F,0x78,0x00,
};

static const unsigned char PROGMEM NTU_bmp[] =
{
0x00,0x00,0x00,0x80,0x0F,0x00,0x00,0x00,0x1C,0x00,0x00,0xE0,0x00,0x00,0x1F,0x00,
0x00,0x00,0x34,0x00,0x00,0xF0,0x18,0x00,0x7C,0x00,0x00,0x00,0x38,0x00,0x00,0xF0,
0x3F,0x00,0x7F,0x00,0x00,0x30,0x38,0x00,0x00,0xF0,0x3F,0x80,0x3F,0x00,0x00,0x30,
0x18,0x00,0x00,0xFF,0x1F,0xC0,0x6F,0x01,0x00,0x70,0x18,0x00,0x00,0xFF,0x03,0xC0,
0x67,0x03,0x00,0x63,0x1C,0x00,0x00,0xFF,0x01,0xC2,0x64,0x03,0x00,0xE3,0x0C,0x00,
0x00,0xF0,0x03,0x9E,0xF7,0x01,0x00,0xEF,0x0E,0x00,0x00,0xF8,0xFB,0xBD,0xFF,0x21,
0x00,0xEF,0x06,0x00,0x00,0xF8,0xFF,0x3F,0xFF,0x60,0x00,0xEF,0x07,0x00,0x00,0xF8,
0xFF,0x03,0x7E,0x60,0x03,0xC7,0x03,0x00,0x00,0xFC,0x87,0x03,0x7E,0xE0,0x07,0x8E,
0xFB,0x00,0x00,0xFC,0xE7,0x01,0x7C,0x70,0x00,0xCE,0xFF,0x00,0x00,0xDE,0xFB,0x00,
0x2E,0xFC,0x00,0xFE,0xF0,0x00,0x80,0x8F,0x1F,0x16,0x77,0xFE,0x03,0x2F,0x38,0x00,
0xC0,0xF7,0x03,0x1E,0xFB,0x38,0x0F,0xF7,0x00,0x00,0xE0,0xF1,0x03,0x3C,0xFA,0x38,
0x9C,0xFF,0x00,0x00,0x70,0xE0,0x01,0x3C,0x3C,0x18,0x98,0xCF,0x01,0x00,0x30,0xF0,
0x01,0x3C,0xEC,0x03,0x00,0x80,0x03,0x00,0x18,0xF0,0x01,0x38,0xF0,0x03,0x00,0xC0,
0x03,0x00,0x00,0xE0,0x01,0x30,0x78,0x06,0x00,0xE0,0x0B,0x00,0x00,0xF0,0x03,0x00,
0x1C,0x06,0x00,0x60,0x0B,0x00,0x00,0xF0,0x07,0x00,0x1C,0x06,0x00,0x80,0x13,0x00,
0x00,0xE8,0x07,0x00,0x08,0x06,0x00,0xA0,0x01,0x00,0x00,0xE0,0x07,0x00,0x00,0x03,
0x00,0xA0,0x01,0x00,0x00,0xE0,0x3B,0x00,0x00,0x03,0x00,0xE0,0x01,0x00,0x00,0xE0,
0x1F,0x30,0x80,0x01,0x00,0xC0,0x01,0x00,0x00,0x80,0x1F,0x60,0xC0,0x01,0x00,0xC0,
0x00,0x00,0x00,0x00,0x03,0xC0,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x80,
0x7F,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1E,0x00,0x00,0x00,0x00,0x00,
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};
  
void setup(void)
{
    Serial.begin(115200);
    pinMode(3, OUTPUT); 
    setSyncProvider(RTC.get);   // the function to get the time from the RTC
    /*if(timeStatus() != timeSet) 
        Serial.println("Unable to sync with the RTC");
    else
       Serial.println("RTC has set the system time"); */

    display.begin(SH1106_SWITCHCAPVCC);
    display.clearDisplay();
  //  Serial.print("+");
  //  Serial.print("+");
  //  Serial.print("+");
    delay(1000);
}

void loop(void)
{
   //Serial.println("AT+RSSI=?");
   if(Serial.available())
   {
      double dd=0;
      String cmd;
      char buf[20];
      int set_dd;
      String ddd="";
      String bat="";
 
      
      Serial.readBytes(buf,20);
        cmd=String(buf);
      //Serial.println(cmd);
      ddd=cmd.substring(0,cmd.indexOf(","));
      bat=cmd.substring(cmd.indexOf(",")+1);
      double power= (abs(ddd.toInt())-70)/(10*2.029);
      //Serial.print("D=");
      dd=pow(10,power);
      
      display.clearDisplay();
      display.drawXBitmap(0, 0,  mop_bmp, 48, 16, 1);
      display.drawXBitmap(50, 0,  NTU_bmp, 80, 33, 1);

      digitalClockDisplay();
      // display.println();
      //display.print(cmd);
      digitalDist(dd);
      set_dd=map(analogRead(0),0,530,0,29);

      digitalSetDist(set_dd);
      digitalBAT(bat.toInt());
      display.display();

      if(dd>set_dd)
          tone(3,880,5000);
      else if(dd>5*set_dd)
          tone(3,880);
      else 
          noTone(3);
   }
   else
   {
      display.clearDisplay();
      display.drawXBitmap(0, 0,  mop_bmp, 48, 16, 1);
      display.drawXBitmap(50, 0,  NTU_bmp, 80, 33, 1);
      // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
      h = dht.readHumidity();
      // Read temperature as Celsius (the default)
      t = dht.readTemperature();
      digitalClockDisplay();
      display.display();
   }
   delay(1000);
}

void digitalDHT(float h,float t)
{
    display.print("C=");
    display.print(t);
    display.print(" H=");
    display.print(h);  
}

void digitalClockDisplay(void)
{
    // digital clock display of the time
    /*Serial.print(hour());
    printDigits(minute());
    printDigits(second());
    Serial.print(' ');
    Serial.print(day());
    Serial.print(' ');
    Serial.print(month());
    Serial.print(' ');
    Serial.print(year()); 
    Serial.println(); */
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0,40);
    display.println();
    display.print(hour());
    display.print(":");
    display.print(minute());
    display.print(":");
    display.print(second());
    display.print(" ");
    display.print(year());
    display.print("/");
    display.print(month());
    display.print("/");
    display.print(day());
}

void digitalDist(double d)
{
    display.println(); 
    display.print("D=");
    display.print(d,4);
}

void digitalSetDist(int d)
{

    display.print(" S=");
    display.print(d);
}

void digitalBAT(int b)
{
    display.print(" B=");
    display.print(b);
}
