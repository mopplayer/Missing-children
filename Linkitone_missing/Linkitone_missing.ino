#include <LGATT.h>
#include <LGATTClient.h>
#include <LGATTServer.h>
#include <LBattery.h>
#include <LGPS.h>
#include <LGPRS.h>
#include <LGPRSClient.h>
#include <LGPRSServer.h>

#include <LGPS.h>
#include <HttpClient.h>
#include <LTask.h>
#include <LWiFi.h>
#include <LWiFiClient.h>
#include <LDateTime.h>

#define WIFI_AP "your ap"
#define WIFI_PASSWORD "your password"
#define WIFI_AUTH LWIFI_WPA  // choose from LWIFI_OPEN, LWIFI_WPA, or LWIFI_WEP.
#define per 50
#define per1 3
#define DEVICEID "your device id"
#define DEVICEKEY "your device key"
#define SITE_URL "api.mediatek.com"

gpsSentenceInfoStruct gps_info;
char buff[256];
double latitude;
double longitude;

char buffer_latitude[8];
char buffer_longitude[8];
//LGPRSClient c;
LWiFiClient c;
unsigned int rtc;
unsigned int lrtc;
unsigned int rtc1;
unsigned int lrtc1;
char port[4]={0};
char connection_info[21]={0};
char ip[15]={0};             
int portnum;
int val = 0;
String tcpdata = String(DEVICEID) + "," + String(DEVICEKEY) + ",0";
String tcpcmd_led_on = "LED_CONTROL,1";
String tcpcmd_led_off = "LED_CONTROL,0";
String upload_led;

//LGPRSClient c2;
LWiFiClient c2;
HttpClient http(c2);


static LGATTUUID test_uuid("B4B4B4B4-B4B4-B4B4-B4B4-B4B4B4B4B4B5");

LGATTClient c3;


static unsigned char getComma(unsigned char num,const char *str)
{
  unsigned char i,j = 0;
  int len=strlen(str);
  for(i = 0;i < len;i ++)
  {
     if(str[i] == ',')
      j++;
     if(j == num)
      return i + 1; 
  }
  return 0; 
}

static double getDoubleNumber(const char *s)
{
  char buf[10];
  unsigned char i;
  double rev;
  
  i=getComma(1, s);
  i = i - 1;
  strncpy(buf, s, i);
  buf[i] = 0;
  rev=atof(buf);
  return rev; 
}

static double getIntNumber(const char *s)
{
  char buf[10];
  unsigned char i;
  double rev;
  
  i=getComma(1, s);
  i = i - 1;
  strncpy(buf, s, i);
  buf[i] = 0;
  rev=atoi(buf);
  return rev; 
}

void parseGPGGA(const char* GPGGAstr)
{
  /* Refer to http://www.gpsinformation.org/dale/nmea.htm#GGA
   * Sample data: $GPGGA,123519,4807.038,N,01131.000,E,1,08,0.9,545.4,M,46.9,M,,*47
   * Where:
   *  GGA          Global Positioning System Fix Data
   *  123519       Fix taken at 12:35:19 UTC
   *  4807.038,N   Latitude 48 deg 07.038' N
   *  01131.000,E  Longitude 11 deg 31.000' E
   *  1            Fix quality: 0 = invalid
   *                            1 = GPS fix (SPS)
   *                            2 = DGPS fix
   *                            3 = PPS fix
   *                            4 = Real Time Kinematic
   *                            5 = Float RTK
   *                            6 = estimated (dead reckoning) (2.3 feature)
   *                            7 = Manual input mode
   *                            8 = Simulation mode
   *  08           Number of satellites being tracked
   *  0.9          Horizontal dilution of position
   *  545.4,M      Altitude, Meters, above mean sea level
   *  46.9,M       Height of geoid (mean sea level) above WGS84
   *                   ellipsoid
   *  (empty field) time in seconds since last DGPS update
   *  (empty field) DGPS station ID number
   *  *47          the checksum data, always begins with *
   */

  int tmp, hour, minute, second, num ;
  if(GPGGAstr[0] == '$')
  {
    tmp = getComma(1, GPGGAstr);
    hour     = (GPGGAstr[tmp + 0] - '0') * 10 + (GPGGAstr[tmp + 1] - '0');
    minute   = (GPGGAstr[tmp + 2] - '0') * 10 + (GPGGAstr[tmp + 3] - '0');
    second    = (GPGGAstr[tmp + 4] - '0') * 10 + (GPGGAstr[tmp + 5] - '0');
    
    sprintf(buff, "UTC timer %2d-%2d-%2d", hour, minute, second);
    Serial.println(buff);
    
    tmp = getComma(2, GPGGAstr);
    latitude = getDoubleNumber(&GPGGAstr[tmp]);
    tmp = getComma(4, GPGGAstr);
    longitude = getDoubleNumber(&GPGGAstr[tmp]);
    sprintf(buff, "latitude = %10.4f, longitude = %10.4f", latitude, longitude);
    Serial.println(buff); 
    
    tmp = getComma(7, GPGGAstr);
    num = getIntNumber(&GPGGAstr[tmp]);    
    sprintf(buff, "satellites number = %d", num);
    Serial.println(buff); 
  }
  else
  {
    Serial.println("Not get data"); 
  }
}

void GPS_receive() {
  LGPS.getData(&gps_info);
  //Serial.println((char*)gps_info.GPGGA); 
  parseGPGGA((const char*)gps_info.GPGGA);
}


void setup() {
    Serial.begin(115200); 
    LTask.begin();
    LWiFi.begin();
    LGPS.powerOn();
    AP_connect();
    getconnectInfo();
    connectTCP();
    Serial.println("==============================");
    Serial.println("LGPS Power on, and waiting ..."); 
    Serial.println("[LGATTC ino]setup.");
    pinMode(13, OUTPUT);
    delay(5000);
}

void loop() 
{    
       // gatt client register
     if (c3.begin(test_uuid))
     {
           Serial.println("[LGATTC ino]begin() success to register.");
     }
     else
     {
           Serial.println("[LGATTC ino]begin() failed to register.");
           delay(0xffffffff);
    }
    // scan device
    Serial.println("[LGATTC ino]loop::start to scan.");
    int num = 0;
    int found=0;
    LGATTDeviceInfo info = {0};
    while (1)
    {
        num = c3.scan(6);
        Serial.printf("scan num [%d] a", num);
        Serial.println();
        if(num>0)
        {
          for (int i = 0; i < num; i++)
          {
            c3.getScanResult(i, info);
            Serial.printf("[LGATTC ino]dev address : [%x:%x:%x:%x:%x:%x] rssi [%d]", 
            info.bd_addr.addr[5], info.bd_addr.addr[4], info.bd_addr.addr[3], info.bd_addr.addr[2], info.bd_addr.addr[1], info.bd_addr.addr[0],
            info.rssi);
            Serial.println();

            // check if the current ble device is the one you prefered.
            if (info.bd_addr.addr[5]==0x88 
             && info.bd_addr.addr[4]==0x33
             && info.bd_addr.addr[3]==0x14
             && info.bd_addr.addr[2]==0xDC
             && info.bd_addr.addr[1]==0x77
             && info.bd_addr.addr[0]==0x2A)
            {
                found = 1;
                break;
            }          
          }         
         
          if(found)
             break;
        }
        else
        {
          break;
        }
    }
    
    if(found)
    {
        int bd_connected=0;
        if (!c3.connect(info.bd_addr)) // search all services till timeout or searching done.
        {
            Serial.println("[LGATTC ino]begin() failed to connect.");
        }
        else
        {
            Serial.println("[LGATTC ino]begin() success to connect.");
            bd_connected=1;
        }
        
        int32_t rssi = 0; 
        while (bd_connected)
        {                  
            if (c3.readRemoteRSSI(info.bd_addr, rssi))
            {
                 Serial.printf("[LGATTC ino] readRemoteRSSI address : [%x:%x:%x:%x:%x:%x] rssi [%d]", 
                 info.bd_addr.addr[5], info.bd_addr.addr[4], info.bd_addr.addr[3], info.bd_addr.addr[2], info.bd_addr.addr[1], info.bd_addr.addr[0],
                 rssi);
                 Serial.println();
                 delay(2000);
            }
            else
            {
                 Serial.println("[LGATTC ino] failed to get rssi");
                 c3.disconnect(info.bd_addr);
                 break;
            }

            // write characteristic
            LGATTUUID serviceUUID=0xDFB0;
            boolean isPrimary=false;
            LGATTAttributeValue attrValue;
            String szbuf=String(rssi);
            szbuf.concat(",");
            szbuf.concat(LBattery.level()); 
            szbuf.getBytes(attrValue.value,szbuf.length()+1);
            LGATTUUID writeUUID = 0xDFB1;
            attrValue.len = szbuf.length()+1;
            if (c3.writeCharacteristic(serviceUUID, isPrimary, writeUUID, attrValue)){
                Serial.println( szbuf);
            }        
        }
    }
    else
    {
        String tcpcmd="";
        while (c.available())
        {
           int v = c.read();
           if (v != -1)
           {
               Serial.print((char)v);
              tcpcmd += (char)v;
               if (tcpcmd.substring(40).equals(tcpcmd_led_on)){
                  digitalWrite(13, HIGH);
                  Serial.print("Switch LED ON ");
                   tcpcmd="";
               }else if(tcpcmd.substring(40).equals(tcpcmd_led_off)){  
                 digitalWrite(13, LOW);
                  Serial.print("Switch LED OFF");
                 tcpcmd="";
            }
           }
           }

          LDateTime.getRtc(&rtc);
           if ((rtc - lrtc) >= per) {
             heartBeat();
             lrtc = rtc;
            }
          //Check for report datapoint status interval
          LDateTime.getRtc(&rtc1);
          if ((rtc1 - lrtc1) >= per1) {
    
              uploadstatus();
             GPS_receive();
              uploadGPS();
              lrtc1 = rtc1;
          }

    }
    Serial.println("[LGATTC ino] ended the client!");
    c3.end();
    delay(500);  
    //delay(0xffffffff);
}

void AP_connect(){
  Serial.print("Connecting to AP...");
  while (0 == LWiFi.connect(WIFI_AP, LWiFiLoginInfo(WIFI_AUTH, WIFI_PASSWORD)))
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("Success!");
  
  Serial.print("Connecting site...");

  while (!c2.connect(SITE_URL, 80))
  {
    Serial.print(".");
    delay(500);
  }
  Serial.println("Success!");
  delay(100);
}


void getconnectInfo(){
  //calling RESTful API to get TCP socket connection
  c2.print("GET /mcs/v2/devices/");
  c2.print(DEVICEID);
  c2.println("/connections.csv HTTP/1.1");
  c2.print("Host: ");
  c2.println(SITE_URL);
  c2.print("deviceKey: ");
  c2.println(DEVICEKEY);
  c2.println("Connection: close");
  c2.println();
  
  delay(500);

  int errorcount = 0;
  Serial.print("waiting for HTTP response...");
  while (!c2.available())
  {
    Serial.print(".");
    errorcount += 1;
    delay(150);
  }
  Serial.println();
  int err = http.skipResponseHeaders();

  int bodyLen = http.contentLength();
  char c;
  int ipcount = 0;
  int count = 0;
  int separater = 0;
  while (c2)
  {
    int v = (int)c2.read();
    if (v != -1)
    {
      c = v;
      //Serial.print(c);
      connection_info[ipcount]=c;
      if(c==',')
      separater=ipcount;
      ipcount++;    
    }
    else
    {
      Serial.println("no more content, disconnect");
      c2.stop();

    }
    
  }

  //connection_info[ipcount]=NULL;
  int i;
  for(i=0;i<separater;i++)
  {  ip[i]=connection_info[i];
  }
  int j=0;
  separater++;
  
  for(i=separater;i<21 && j<5 && i < ipcount;i++)
  {  port[j]=connection_info[i];
     j++;
  }
  //port[j] = NULL;
    Serial.println("The TCP Socket connection instructions:");
  Serial.print("IP: ");
  Serial.println(ip);
  Serial.print("Port: ");
  Serial.println(port);
  portnum = atoi (port);
  Serial.println(portnum);
} //getconnectInfo

void connectTCP(){
  //establish TCP connection with TCP Server with designate IP and Port
  c.stop();
  Serial.print("Connecting to TCP...");
  while (0 == c.connect(ip, portnum))
  {
    Serial.println("Re-Connecting to TCP");    
    delay(1000);
  }
  c.println(tcpdata);
  c.println();
  Serial.println("Success!");
} //connectTCP


void heartBeat(){
  Serial.println("send TCP heartBeat");
  c.println(tcpdata);
  c.println();
    
} //heartBeat

void uploadstatus(){//calling RESTful API to upload datapoint to MCS to report LED status
  LWiFiClient c2;
  while (!c2.connect(SITE_URL, 80))
  {
    Serial.print(".");
    delay(500);
  }
  
  delay(100);
  
  if(digitalRead(13)==1)
    upload_led = "LED_DISPLAY,,1";
  else
    upload_led = "LED_DISPLAY,,0";
  
  int thislength = upload_led.length();
  HttpClient http(c2);
  c2.print("POST /mcs/v2/devices/");
  c2.print(DEVICEID);
  c2.println("/datapoints.csv HTTP/1.1");
  c2.print("Host: ");
  c2.println(SITE_URL);
  c2.print("deviceKey: ");
  c2.println(DEVICEKEY);
  c2.print("Content-Length: ");
  c2.println(thislength);
  c2.println("Content-Type: text/csv");
  c2.println("Connection: close");
  c2.println();
  c2.println(upload_led);
  delay(500);

  int errorcount = 0;
  while (!c2.available())
  {
    Serial.print(".");
    delay(100);
  }
  int err = http.skipResponseHeaders();
  int bodyLen = http.contentLength();
  
  while (c2)
  {
    int v = c2.read();
    if (v != -1)
    {
      Serial.print(char(v));
    }
    else
    {
      Serial.println("no more content, disconnect");
      c2.stop();
    }
    
  }
  Serial.println();
}

void uploadGPS(){
  LWiFiClient c2;
  while (!c2.connect(SITE_URL, 80))
  {
    Serial.print(".");
    delay(500);
  }
  
  delay(100);
  double lat;
  double lon;
  lat=int((latitude/100));
  lat=lat+((latitude/100)-lat)*100/60;
  lon=int((longitude/100));
  lon=lon+((longitude/100)-lon)*100/60;
    
  Serial.printf("latitude=%.4f\tlongitude=%.4f\n",lat,lon);
  if(lat>-90 && lat<=90 && lon>=0 && lon<360){
    sprintf(buffer_latitude, "%.4f", lat);
    sprintf(buffer_longitude, "%.4f", lon);
  }
  String upload_GPS = "GPS,,"+String(buffer_latitude)+","+String(buffer_longitude)+","+"0"+"\n"+"LATITUDE,,"+buffer_latitude+"\n"+"LONGITUDE,,"+buffer_longitude;//null altitude
  int GPS_length = upload_GPS.length();
  HttpClient http(c2);
  c2.print("POST /mcs/v2/devices/");
  c2.print(DEVICEID);
  c2.println("/datapoints.csv HTTP/1.1");
  c2.print("Host: ");
  c2.println(SITE_URL);
  c2.print("deviceKey: ");
  c2.println(DEVICEKEY);
  c2.print("Content-Length: ");
  c2.println(GPS_length);
  c2.println("Content-Type: text/csv");
  c2.println("Connection: close");
  c2.println();
  c2.println(upload_GPS);
  delay(500);

  int errorcount = 0;
  
  while (!c2.available())
  {
    Serial.print(".");
    delay(100);
  }
  int err = http.skipResponseHeaders();
  int bodyLen = http.contentLength();
  
  while (c2)
  {
    int v = c2.read();
    if (v != -1)
    {
      Serial.print(char(v));
    }
    else
    {
      Serial.println("no more content, disconnect");
      c2.stop();
    }
    
  }
  Serial.println();
}

