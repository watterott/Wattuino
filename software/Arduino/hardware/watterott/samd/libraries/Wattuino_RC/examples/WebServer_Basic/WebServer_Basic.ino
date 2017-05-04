/*
 * Basic WebServer Example
 *
 * Creates a webserver to control the I/Os.
 * In AP mode the webserver URL is http://192.168.1.1
 */

#include <WiFi101.h>
#include <WiFiMDNSResponder.h>

#define IS_ACCESS_POINT 1 // 0 or 1

#if IS_ACCESS_POINT == 1
  char ssid[] = "basic-ap";     // AP name
  char pass[] = "";             // no AP password (open AP)
  //char pass[] = "1234567890";   // AP password
#else
  char ssid[] = "your-wifi-ssid"; // your network SSID (name)
  char pass[] = "your-password";  // your network password
#endif

//#define MDNS_SUPPORT // uncomment for MDNS support

char mdnsName[] = "wattuino"; // MDNS name "wattuino.local"

//#define WAIT_FOR_SERIAL_CONNECTION // wait for USB serial connection

#define MAX_PACKET_LEN 1400 // max ethernet frame payload
#define CT_NONE  0
#define CT_PLAIN 1
#define CT_HTML  2

const uint8_t pinJ[8] = { RCJ1, RCJ1, RCJ2, RCJ3, RCJ4, RCJ5, RCJ6, RCJ7 };
const uint8_t pinMotor[4][2] = { { RCM1A, RCM1B }, { RCM2A, RCM2B }, { RCM3A, RCM3B }, { RCM4A, RCM4B } };

WiFiServer webserver(80); // webserver at port 80
char *httpRequestURL; // HTTP request from client
#ifdef MDNS_SUPPORT
  WiFiMDNSResponder mdnsResponder; // MDNS responder
#endif

const char index_html_text[] = 
    "<!DOCTYPE html>"
    "<html>"
    "<head>"
    "<meta charset='utf-8'>"
    "<title>Basic I/O</title>"
    "<style>"
      "a:link, a:visited {"
      "padding:3px 3px; overflow:hidden; width:80%; background:#7b3; color:#fff;"
      "-webkit-border-radius:5px; -moz-border-radius:5px; border-radius:5px; border:solid 1px #351;" 
      "-webkit-box-shadow:inset 0 1px 0 rgba(255, 255, 255, 0.4), 0 1px 1px rgba(0, 0, 0, 0.2);"
      "-moz-box-shadow:inset 0 1px 0 rgba(255, 255, 255, 0.4), 0 1px 1px rgba(0, 0, 0, 0.2);"
      "box-shadow:inset 0 1px 0 rgba(255, 255, 255, 0.4), 0 1px 1px rgba(0, 0, 0, 0.2);"
      "font-size:14px; font-weight:bold; text-align:center; text-decoration:none; display:inline-block;"
      "}"
      "table{"
      "font-family:Arial, sans-serif;"
      "font-size:16px;"
      "text-align:center;"
      "border-collapse:collapse;"
      "border-color:#26b;"
      "}"
      "th{"
      "font-weight:bold;"
      "padding:3px 3px;"
      "border-style:solid;"
      "border-width:2px;"
      "border-color:#26b;"
      "border-right:solid 2px #7ad;"
      "color:#fff;"
      "background-color:#26b;"
      "}"
      "th:last-child { border-right:solid 2px #26b; }"
      "td{"
      "width:8%;"
      "padding:3px 3px;"
      "border-style:solid;"
      "border-width:1px;"
      "border-color:#26b;"
      "color:#000;"
      "background-color:#fff;"
      "}"  
    "</style>"
    "</head>"
    "<body>"
    "<h3>I/O Ports:</h3>"
    "<table>"
      "<tr>"
        "<th>I/O Port</th>"
        "<th>Direction</th>"
        "<th colspan='2'>DigitalWrite</th>"
        "<th colspan='4'>Analog Output</th>"
        "<th>Direction</th>"
        "<th colspan='2'>Request</th>"
      "</tr>"
      "<tr>"
        "<td>J1</td>"
        "<td><a href='/J1/output'>Output</a></td>"
        "<td><a href='/J1/0'>0</a></td>"
        "<td><a href='/J1/1'>1</a></td>"
        "<td></td>"
        "<td></td>"
        "<td></td>"
        "<td></td>"
        "<td><a href='/J1/input'>Input</a></td>"
        "<td><a href='/J1/read'>DigitalRead</a></td>"
        "<td><a href='/J1/analogread'>AnalogRead</a></td>"
      "</tr>"
      "<tr>"
        "<td>J2</td>"
        "<td><a href='/J2/output'>Output</a></td>"
        "<td><a href='/J2/0'>0</a></td>"
        "<td><a href='/J2/1'>1</a></td>"
        "<td></td>"
        "<td></td>"
        "<td></td>"
        "<td></td>"
        "<td><a href='/J2/input'>Input</a></td>"
        "<td><a href='/J2/read'>DigitalRead</a></td>"
        "<td><a href='/J2/analogread'>AnalogRead</a></td>"
      "</tr>"
      "<tr>"
        "<td>J3</td>"
        "<td><a href='/J3/output'>Output</a></td>"
        "<td><a href='/J3/0'>0</a></td>"
        "<td><a href='/J3/1'>1</a></td>"
        "<td><a href='/J3/analogwrite/25'>PWM 10%</a></td>"
        "<td><a href='/J3/analogwrite/64'>PWM 25%</a></td>"
        "<td><a href='/J3/analogwrite/128'>PWM 50%</a></td>"
        "<td><a href='/J3/analogwrite/192'>PWM 75%</a></td>"
        "<td><a href='/J3/input'>Input</a></td>"
        "<td><a href='/J3/read'>DigitalRead</a></td>"
        "<td><a href='/J3/analogread'>AnalogRead</a></td>"
      "</tr>"
      "<tr>"
        "<td>J4</td>"
        "<td><a href='/J4/output'>Output</a></td>"
        "<td><a href='/J4/0'>0</a></td>"
        "<td><a href='/J4/1'>1</a></td>"
        "<td><a href='/J4/analogwrite/25'>PWM 10%</a></td>"
        "<td><a href='/J4/analogwrite/64'>PWM 25%</a></td>"
        "<td><a href='/J4/analogwrite/128'>PWM 50%</a></td>"
        "<td><a href='/J4/analogwrite/192'>PWM 75%</a></td>"
        "<td><a href='/J4/input'>Input</a></td>"
        "<td><a href='/J4/read'>DigitalRead</a></td>"
        "<td><a href='/J4/analogread'>AnalogRead</a></td>"
      "</tr>"
      "<tr>"
        "<td>J5</td>"
        "<td><a href='/J5/output'>Output</a></td>"
        "<td><a href='/J5/0'>0</a></td>"
        "<td><a href='/J5/1'>1</a></td>"
        "<td></td>"
        "<td></td>"
        "<td></td>"
        "<td></td>"
        "<td><a href='/J5/input'>Input</a></td>"
        "<td><a href='/J5/read'>DigitalRead</a></td>"
        "<td><a href='/J5/analogread'>AnalogRead</a></td>"
      "</tr>"
      "<tr>"
        "<td>J6</td>"
        "<td><a href='/J6/output'>Output</a></td>"
        "<td><a href='/J6/0'>0</a></td>"
        "<td><a href='/J6/1'>1</a></td>"
        "<td></td>"
        "<td></td>"
        "<td></td>"
        "<td></td>"
        "<td><a href='/J6/input'>Input</a></td>"
        "<td><a href='/J6/read'>DigitalRead</a></td>"
        "<td><a href='/J6/analogread'>AnalogRead</a></td>"
      "</tr>"
      "<tr>"
        "<td>J7</td>"
        "<td><a href='/J7/output'>Output</a></td>"
        "<td><a href='/J7/0'>0</a></td>"
        "<td><a href='/J7/1'>1</a></td>"
        "<td></td>"
        "<td></td>"
        "<td></td>"
        "<td></td>"
        "<td><a href='/J7/input'>Input</a></td>"
        "<td><a href='/J7/read'>DigitalRead</a></td>"
        "<td><a href='/J7/analogread'>AnalogRead</a></td>"
      "</tr>"
    "</table>"
    "<h3>Switch Output:</h3>"
    "<table>"
      "<tr>"
        "<th>Output</th>"
        "<th colspan='2'>State</th>"
      "</tr>"
      "<tr>"
        "<td>S1</td>"
        "<td><a href='/S1/1'>On</a></td>"
        "<td><a href='/S1/0'>Off</a></td>"
      "</tr>"
    "</table>"
    "<h3>Motor Driver:</h3>"
    "<table>"
      "<tr>"
        "<th>Motor</th>"
        "<th colspan='10'>Speed</th>"
      "</tr>" 
      "<tr>"
        "<td>M1</td>"
        "<td><a href='/M1/-255'>-100%</a></td>"
        "<td><a href='/M1/-192'>-75%</a></td>"
        "<td><a href='/M1/-128'>-50%</a></td>"
        "<td><a href='/M1/-64'>-25%</a></td>"
        "<td><a href='/M1/0'>Off</a></td>"
        "<td><a href='/M1/0/b'>Break</a></td>"
        "<td><a href='/M1/64'>+25%</a></td>"
        "<td><a href='/M1/128'>+50%</a></td>"
        "<td><a href='/M1/192'>+75%</a></td>"
        "<td><a href='/M1/255'>+100%</a></td>"
      "</tr>"
      "<tr>"
        "<td>M2</td>"
        "<td><a href='/M2/-255'>-100%</a></td>"
        "<td><a href='/M2/-192'>-75%</a></td>"
        "<td><a href='/M2/-128'>-50%</a></td>"
        "<td><a href='/M2/-64'>-25%</a></td>"
        "<td><a href='/M2/0'>Off</a></td>"
        "<td><a href='/M2/0/b'>Break</a></td>"
        "<td><a href='/M2/64'>+25%</a></td>"
        "<td><a href='/M2/128'>+50%</a></td>"
        "<td><a href='/M2/192'>+75%</a></td>"
        "<td><a href='/M2/255'>+100%</a></td>"
      "</tr>"
      "<tr>"
        "<td>M3</td>"
        "<td><a href='/M3/-255'>-100%</a></td>"
        "<td><a href='/M3/-192'>-75%</a></td>"
        "<td><a href='/M3/-128'>-50%</a></td>"
        "<td><a href='/M3/-64'>-25%</a></td>"
        "<td><a href='/M3/0'>Off</a></td>"
        "<td><a href='/M3/0/b'>Break</a></td>"
        "<td><a href='/M3/64'>+25%</a></td>"
        "<td><a href='/M3/128'>+50%</a></td>"
        "<td><a href='/M3/192'>+75%</a></td>"
        "<td><a href='/M3/255'>+100%</a></td>"
      "</tr>"
      "<tr>"
        "<td>M4</td>"
        "<td><a href='/M4/-255'>-100%</a></td>"
        "<td><a href='/M4/-192'>-75%</a></td>"
        "<td><a href='/M4/-128'>-50%</a></td>"
        "<td><a href='/M4/-64'>-25%</a></td>"
        "<td><a href='/M4/0'>Off</a></td>"
        "<td><a href='/M4/0/b'>Break</a></td>"
        "<td><a href='/M4/64'>+25%</a></td>"
        "<td><a href='/M4/128'>+50%</a></td>"
        "<td><a href='/M4/192'>+75%</a></td>"
        "<td><a href='/M4/255'>+100%</a></td>"
      "</tr>" 
    "</table>"
    "</body>"
    "</html>";

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
  // call MDNS responder every loop iteration to make sure it can detect and respond to requests
  #ifdef MDNS_SUPPORT
    mdnsResponder.poll();
  #endif

  // listen for incoming clients
  WiFiClient client = webserver.available();
  if(client) 
  {
    // a http request ends with a blank line
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
            String reply = "";
            //Serial.println(httpRequestURL);

            if((toupper(httpGetResource(0)[0]) == 'J') && \
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
                else if(httpGetResource(1) == "read")
                {
                  reply = digitalRead(pinJ[portNr]);
                  reply += " (J";
                  reply += portNr;
                  reply += " )";
                }
                else if(httpGetResource(1) == "analogread")
                {
                  int value = analogRead(pinJ[portNr]); 
                  reply = value; // implicit conversion to string
                  reply += " = ";
                  if(pinJ[portNr] == PIN_A2) // A2 is connected to the power supply (VIN)
                  {
                    reply += value * 3.3 * (62 + 14) / (14 * 1024);
                    reply += " Volt power supply";
                  }
                  else
                  {
                    reply += value * 3.3/1024;
                    reply += " Volt";
                  }
                  reply += " (J";
                  reply += portNr;
                  reply += " )";
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
              }
            }
            else if(httpGetResource(0) != "")
            {
              reply = "Unkown Command: " + httpGetResource(0);
            }
  
            // send answer to the client (browser)
            httpSendHeader(client, CT_HTML);
            if(reply != "") // no explicit input request
            {
              String str = "<h2>Read Value: " + reply + "</h2><hr>";
              httpSendData(client, str.c_str(), str.length());
            }

            // show default home page    
            httpSendData(client, index_html_text, sizeof(index_html_text)-1);
          }       
          break; // exit while loop and close connection
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
        //Serial.println(reqpart[0]);
        //Serial.println(reqpart[1]);
        //Serial.println(httpRequestURL);
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

  if(content_type == CT_PLAIN)
    client.write(header_plain_data, sizeof(header_plain_data)-1);
  else if(content_type == CT_HTML)
    client.write(header_html_data, sizeof(header_html_data)-1);
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
