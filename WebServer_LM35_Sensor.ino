// Program used to monitor the temperature with LM35 sensor and access the information through website //

#include <SPI.h>
#include <Ethernet.h>

const int led = 8;                        // led pin
int tempPin = A3;                         // lm35 pin
float reading = 0;                        // temperature reading variable
const float voltage_reference = 1.1;      // used to change the reference's voltage, could be changed depending of the board used

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};    // mac address off the board
IPAddress ip(10, 156, 10, 13);                        // ip address off the board
EthernetServer server(80);                            // server to external access

String led_operation;         // variable used to control the led on http access
int led_status = 0;           // used to control the status of led in the html page
char ch;                      // reads char by char to build led_operation variable

void setup() {
  Serial.begin(9600);
  analogReference(INTERNAL1V1);
  pinMode(tempPin, INPUT);
  pinMode(led, OUTPUT);
  Ethernet.begin(mac, ip);
  server.begin();
}

void loop() {
  reading = (voltage_reference * analogRead(tempPin) * 100.0) / 1024;
  Serial.print("Temperature: ");
  Serial.println((float)reading);
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html; charset=utf-8");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          // HTML Code //
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          client.println("<title>Temperature Sensor </title>");
          client.println("<div align=center>");
          client.println("<a href=https://www.climatempo.com.br target='_blank'/><img src=https://www.luisllamas.es/wp-content/uploads/2015/10/arduino-sensor-temperatura-interno.png></a><br />");
          client.println("<div>");
          client.println("<h3 style='display:inline;'>Status do LED: </h3>");
          client.print("<h3 style='display:inline; ");
          if (led_status == 1) {
            client.print("color:green;'> on");
          } else {
            client.print("color:red;'> off");
          }
          client.println("</h3>");
          client.println("</div>");
          client.println("<br/>");
          client.println("<form action='' method='post'>");
          client.println("<button name='operation' type='submit' value='on'>Turn on LED</button>");
          client.println("<button name='operation' type='submit' value='off'>Turn off LED</button>");
          client.println("</form>");
          client.println("<h2>Arduino Webserver using LM-35 temperature sensor</h2>");
          client.println("<tr><td><hr size=4 color=#0099FF> </td></tr>");
          client.print("<h2>Current Temperature: </h2>");
          client.print("<h1 style='color:blue;'>");
          client.println((float)reading);
          client.print(" &#8451;");
          client.println("</h1>");
          client.println("<br/>");
          client.println("</div>");
          client.println("</html>");
          while (client.available()) {
            ch = client.read();
            //Serial.print(ch);
            led_operation.concat(ch);
          }
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        }
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    if (led_operation == "operation=on") {
      digitalWrite(led, HIGH);
      led_status = 1;
    } else if (led_operation == "operation=off") {
      digitalWrite(led, LOW);
      led_status = 0;
    }
    led_operation = "";
  }
  delay(3000); // wait for 3 seconds before taking the reading again
}

