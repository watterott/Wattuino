/*
 * RC-Car WebServer Example
 *
 * Creates a webserver to control 2 motors on M1 (right) and M2 (left)
 * In AP mode the webserver URL is http://192.168.1.1
 */

#include <WiFi101.h>
#include <WiFiMDNSResponder.h>

//#define DEBUG 1 // enable debug output

//#define M1_INVERT // invert direction of M1/right
//#define M2_INVERT // invert direction of M2/left

#define IS_ACCESS_POINT 1 // 0 or 1

#if IS_ACCESS_POINT == 1
  char ssid[] = "rc-car-ap";   // AP name
  char pass[] = "";             // no AP password (open AP)
  //char pass[] = "1234567890";   // AP password
#else
  char ssid[] = "your-wifi-ssid"; // your network SSID (name)
  char pass[] = "your-password";  // your network password
#endif

//#define MDNS_SUPPORT // uncomment for MDNS support

char mdnsName[] = "wattuino"; // MDNS name "wattuino.local"

//#define WAIT_FOR_SERIAL_CONNECTION

#define MAX_PACKET_LEN 1400
#define CT_NONE  0
#define CT_PLAIN 1
#define CT_HTML  2
#define CT_JSON  3
#define CT_JPEG  4

const uint8_t pinJ[8] = { RCJ1, RCJ1, RCJ2, RCJ3, RCJ4, RCJ5, RCJ6, RCJ7 };
const uint8_t pinMotor[4][2] = { { RCM1A, RCM1B }, { RCM2A, RCM2B }, { RCM3A, RCM3B }, { RCM4A, RCM4B } };

WiFiServer webserver(80); // webserver at port 80
char *httpRequestURL; // HTTP request from client
#ifdef MDNS_SUPPORT
  WiFiMDNSResponder mdnsResponder; // MDNS responder
#endif

/*
 * initialize motor port (1...4) and set speed to 0
 */
void initMotor(uint32_t motorNr)
{
  if((motorNr >= 1) && (motorNr <= 4))
  {
    motorNr--;
    pinMode(pinMotor[motorNr][0], OUTPUT);
    analogWrite(pinMotor[motorNr][0], 0);
    pinMode(pinMotor[motorNr][1], OUTPUT);
    analogWrite(pinMotor[motorNr][1], 0);
  }
}

/*
 * set motor speed
 * motorNr: 1..4
 * value:   -255...255
 * breaks:  motor break activated, when true
 */
void setMotorSpeed(int motorNr, int value, bool breaks = false)
{
  int in1, in2;

  if((motorNr >= 1) && (motorNr <= 4))
  {
    motorNr--;

    if(value >= 0)
    {
      if(value > 255)
        value = 255;
        
      if(breaks)
      {
        in1 = 255;
        in2 = 255 - value;
      }
      else
      {
        in1 = value;
        in2 = 0;
      }
    }
    else  // value is negative, reverse motor
    {
      value = -value;
      if(value > 255)
        value = 255;

      if(breaks)
      {
        in1 = 255 - value;
        in2 = 255;
      }
      else
      {
        in1 = 0;
        in2 = value;
      }
    }

    analogWrite(pinMotor[motorNr][0], in1);
    analogWrite(pinMotor[motorNr][1], in2);
  }
}

/*
 * start WiFi connection
 * ssid:     network SSID
 * password: network password
 * isAP:     create AP: true or false
 */
bool startWifi(char *ssid, char *password, bool isAP)
{
  int status;

  WiFi.hostname(mdnsName); //use MDNS name as host name (DHCP option 12)

  if(isAP) 
  {
    // create AP
    Serial.print("Create an open Access Point: ");
    Serial.println(ssid);
    if(strlen(password) >= 10)
    {
      //status = WiFi.beginAP(ssid, 0, password); // WEP (key index 0 needed and password must be exactly 10 or 26 characters)
      status = WiFi.beginAP(ssid, password); // WPA (FW >=19.5.0)
    }
    else
    {
      status = WiFi.beginAP(ssid);
    }
    if(status != WL_AP_LISTENING) 
    {
      Serial.println("Creating access point failed");
      return false; // return error
    }
  }
  else
  {
    // connect to WPA/WPA2 network
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, password);
    if(status != WL_CONNECTED)
    {
      Serial.println("Connecting failed");
      return false; // return error
    }
  }

  return true;
}

/*
 * show WiFI status
 */
void printWifiStatus() 
{
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}

/*
 * setup function, runs once when you press reset or power the board
 */
void setup() 
{
  // init pins
  initMotor(1);
  initMotor(2);
  initMotor(3);
  initMotor(4);
  pinMode(RCS1, OUTPUT);

  // init analog inputs
  analogReadResolution(10); // 10 bit (0...1023)
  analogReference(AR_DEFAULT); // internal reference

  // init serial port
  Serial.begin(9600);
  #ifdef WAIT_FOR_SERIAL_CONNECTION  
    while(!Serial)
    {
      // wait for serial port to connect, needed for native USB port only
    }
  #endif

  // wait for WiFi connection   
  while(!startWifi(ssid, pass, IS_ACCESS_POINT))
  {
    delay(5000); // wait 5 seconds and retry
  } 
  
  // start webserver
  webserver.begin();

  // start MDNS responder to listen to the configured name
  #ifdef MDNS_SUPPORT
    mdnsResponder.begin(mdnsName);
  #endif

  // you're connected now, so print out the status
  printWifiStatus();
}

/*
 * loop function, runs over and over again forever
 */
void loop() 
{
  static unsigned long lastHttpRequest = 0;

  // call MDNS responder every loop iteration to make sure it can detect and respond to requests
  #ifdef MDNS_SUPPORT
    mdnsResponder.poll();
  #endif

  // listen for incoming clients
  WiFiClient client = webserver.available();
  if(client) 
  {
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;    
    char httpRequest[1024];
    size_t httpRequestLen = 0;

    while(client.connected()) 
    {
      if(client.available()) 
      {
        char c = client.read();
        if(httpRequestLen < (sizeof(httpRequest)-1))
        {
          httpRequest[httpRequestLen++] = c;
          httpRequest[httpRequestLen] = '\0';
        }

        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if(c == '\n' && currentLineIsBlank) 
        {
          //Serial.println(httpRequest);

          if(httpSplitRequest(httpRequest, httpRequestLen))
          {
            lastHttpRequest = millis(); // save time of last request

            String reply = "";
            #ifdef DEBUG
              Serial.println(httpRequestURL);
            #endif

            if(!strcmp(httpRequestURL, "/index.html") || (strlen(httpRequestURL) < 2))
            {
              sendIndexHTML(client);
            }
            else if(httpGetResource(0) == "car")  // /car/motorL/motorR
            {
              int mL = atoi( httpGetResource(1).c_str() );
              int mR = atoi( httpGetResource(2).c_str() );
              #ifdef M1_INVERT
                setMotorSpeed(1, -mR, false);
              #else
                setMotorSpeed(1, mR, false);
              #endif
              #ifdef M2_INVERT
                setMotorSpeed(2, -mL, false);
              #else
                setMotorSpeed(2, mL, false);
              #endif

              // send reply
              int value = analogRead(PIN_A2);  // A2 is connected to the power supply (VIN)
              reply = "{ \"vbat\": ";
              reply += (value * 3.3 * (62 + 14) / (14 * 1024)); // implicit conversion to string
              reply += " }";
              #ifdef DEBUG
                Serial.println(reply);
              #endif
              httpSendHeader(client, CT_JSON);
              httpSendData(client, reply.c_str(), reply.length());
            }
            else if((toupper(httpGetResource(0)[0]) == 'J') && \
               (httpGetResource(0).length() == 2)) // I/O 1..7
            {
              char c1 = httpGetResource(0)[1];
              if((c1 >= '1') && (c1 <= '7'))
              {
                int portNr = c1 - '0';  

                if(httpGetResource(1) == "input")
                {
                  pinMode(pinJ[portNr], INPUT);
                }            
                else if(httpGetResource(1) == "output")
                {
                  pinMode(pinJ[portNr], INPUT); // set as input to reset pinmux https://github.com/arduino/ArduinoCore-samd/issues/205
                  pinMode(pinJ[portNr], OUTPUT);
                }
                else if(httpGetResource(1) == "0")
                {
                  digitalWrite(pinJ[portNr], LOW);
                }
                else if(httpGetResource(1) == "1")
                {
                  digitalWrite(pinJ[portNr], HIGH);
                }
                else if(httpGetResource(1) == "analogwrite")
                {
                  int value = atoi( httpGetResource(2).c_str());
                  analogWrite(pinJ[portNr], value);
                }
              }
            }
            else if((toupper(httpGetResource(0)[0]) == 'S') && \
                    (httpGetResource(0).length() == 2)) // switch output
            {
              if(httpGetResource(0)[1] == '1')
              {
                if(httpGetResource(1) == "0")
                {
                  pinMode(RCS1, OUTPUT);
                  digitalWrite(RCS1, LOW);
                }
                else if(httpGetResource(1) == "1")
                {
                  pinMode(RCS1, OUTPUT);
                  digitalWrite(RCS1, HIGH);
                }
              }
            }
            else if((toupper(httpGetResource(0)[0]) == 'M') && \
                    (httpGetResource(0).length() == 2)) // motor
            {
              bool breaks = false;
              char c1 = httpGetResource(0)[1];
              if(c1 >= '1' && c1 <= '4')
              {
                int motorNr = c1 - '0'; 
                int value = atoi( httpGetResource(1).c_str() );

                // extra parameter for the break
                if(toupper(httpGetResource(2)[0]) == 'B')
                  breaks = true;

                setMotorSpeed(motorNr, value, breaks);
                setMotorSpeed(motorNr, value, breaks);
              }
            }
            else
            {
              httpSend404(client); // send error 404
            }
          }
          break;  // exit while loop and close connection
        }

        if (c == '\n') 
        {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') 
        {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // close the connection
    client.stop();
  }

  // stop motors, if no data received for 500ms
  if((millis()-lastHttpRequest) > 500)
  {
    lastHttpRequest = millis(); // save time
    setMotorSpeed(1, 0, false);
    setMotorSpeed(2, 0, false);
  }
}

/*
 * split HTTP request string
 */
bool httpSplitRequest(char *httpRequest, size_t len)
{
  char *reqpart[2];

  if(len >= 3)
  {
    reqpart[0] = strtok(httpRequest, " \t\n");
    if(strncmp(reqpart[0], "GET\0", 4) == 0)
    {
      httpRequestURL = strtok(NULL, " \t"); // url
      //if(!strncmp(httpRequestURL, "/\0", 2)) // rename empty url to index.html
      //  httpRequestURL = "/index.html";
  
      reqpart[1] = strtok (NULL, " \t\n");
      if(!strncmp(reqpart[1], "HTTP/1.0", 8) || \
         !strncmp(reqpart[1], "HTTP/1.1", 8))
      {
        #ifdef DEBUG
          //Serial.println(reqpart[0]);
          //Serial.println(reqpart[1]);
          //Serial.println(httpRequestURL);
        #endif
        return true;      
      }
    }
  }

  // wrong request
  return false;
}

/*
 * get HTTP resource string
 */
String httpGetResource(int nr)
{
  String resource = "";
  char *pnt = httpRequestURL;

  if(*pnt == '/') // URL must start with '/'
  {
    while(*pnt)
    {
      if(*pnt++ == '/')
      {
        if(nr-- <= 0)
        {
          while(*pnt != '\0' && *pnt != '/')
          {
            resource += *pnt++;
          }
          break;
        }
      }
    }
  }

  return resource;
}

/*
 * send HTTP header
 */
void httpSendHeader(WiFiClient &client, int content_type)
{
  const char header_none_data[]  = "HTTP/1.0 200 OK\n\n";
  const char header_plain_data[] = "HTTP/1.0 200 OK\nContent-type: text/plain\n\n";
  const char header_html_data[]  = "HTTP/1.0 200 OK\nContent-type: text/html\n\n";
  const char header_json_data[]  = "HTTP/1.0 200 OK\nContent-type: application/json\n\n"; //Cache-Control: no-cache
  const char header_jpeg_data[]  = "HTTP/1.0 200 OK\nContent-type: image/jpeg\n\n";

  if(content_type == CT_JSON)
    client.write(header_json_data, sizeof(header_json_data)-1);
  else if(content_type == CT_PLAIN)
    client.write(header_plain_data, sizeof(header_plain_data)-1);
  else if(content_type == CT_HTML)
    client.write(header_html_data, sizeof(header_html_data)-1);
  else if(content_type == CT_JPEG)
    client.write(header_jpeg_data, sizeof(header_jpeg_data)-1);
  else
    client.write(header_none_data, sizeof(header_none_data)-1);
}

/*
 * send HTTP data
 */
void httpSendData(WiFiClient &client, const char *data, int len)
{
  while(len > 0)
  {
    int sendlen = len;
    if(len > MAX_PACKET_LEN)
      sendlen = MAX_PACKET_LEN;

    client.write(data, sendlen);
    data += sendlen;
    len -= sendlen;
  }
}

/*
 * send error 404 not found
 */
void httpSend404(WiFiClient &client)
{
  const char error_404_data[] = 
      "HTTP/1.0 404 Not Found\n"
      "Content-type: text/plain\n"
      "\n"
      "Error 404: Not Found\n";

  client.write(error_404_data, sizeof(error_404_data)-1);
}

/*
 * send file index.html
 */
void sendIndexHTML(WiFiClient &client)
{
#include "index_html.h"  
  httpSendHeader(client, CT_HTML);
  httpSendData(client, (const char *)index_html, sizeof(index_html));
}

