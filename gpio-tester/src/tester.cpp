#include <Arduino.h>
#include <WiFi.h>

const char *ssid = "*";
const char *password = "*";

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

const int pins_num = 9;
const int pins[pins_num] = { 33, 25, 26, 27, 14, 12, 13, 16, 4 };
int pin_modes[pins_num] = {};
int output_states[pins_num] = {};

//#define LED_TEST
#include <FastLED.h>
#ifdef LED_TEST
CRGB leds[90];
#endif

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);
  for (int i = 0; i < pins_num; i++)
    pinMode(pins[i], INPUT);

#ifdef LED_TEST
  FastLED.addLeds<WS2812B, 25, GRB>(leds, 90);
  FastLED.addLeds<WS2812B, 26, GRB>(leds, 90);
  FastLED.addLeds<WS2812B, 27, GRB>(leds, 90);
  FastLED.addLeds<WS2812B, 33, GRB>(leds, 73);
#endif

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  // Print local IP address and start web server
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (!client)
    return;

  currentTime = millis();
  previousTime = currentTime;
  Serial.println("New Client.");          // print a message out in the serial port
  String currentLine = "";                // make a String to hold incoming data from the client
  while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
    currentTime = millis();
    if (client.available()) {             // if there's bytes to read from the client,
      char c = client.read();             // read a byte, then
      Serial.write(c);                    // print it out the serial monitor
      header += c;
      if (c == '\n') {                    // if the byte is a newline character
        // if the current line is blank, you got two newline characters in a row.
        // that's the end of the client HTTP request, so send a response:
        if (currentLine.length() == 0) {
          // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
          // and a content-type so the client knows what's coming, then a blank line:
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/html");
          client.println("Connection: close");
          client.println();

          // turns the GPIOs on and off
          if (header.startsWith("GET /set/") && header[10] == '/' && header[12] == ' ') {
            int i = header.substring(9, 10).toInt();
            int s = header.substring(11, 12).toInt();
            if (i >= 0 && i < pins_num && s >= 0 && s <= 3) {
#ifdef LED_TEST
              if (i == 0) {
                if (s == 0)
                  leds[0] = CRGB::Black;
		else if (s == 1)
                  leds[0] = CRGB::White;
                else if (s == 2)
                  leds[10] = CRGB::Black;
                else if (s == 3)
                  leds[10] = CRGB::Yellow;
                FastLED.show();
                Serial.println("LEDs updated");
              } else
#endif
              if (s <= 1) {
                pin_modes[i] = s;
                pinMode(pins[i], s ? OUTPUT : INPUT);
		if (s)
                  digitalWrite(pins[i], output_states[i]);
		else
                  digitalWrite(pins[i], 0);
                Serial.println(String("GPIO ") + pins[i] + " mode " + (s ? "OUTPUT" : "INPUT"));
              } else {
                output_states[i] = (s == 3);
                if (pin_modes[i]) {
                  digitalWrite(pins[i], output_states[i]);
                  Serial.println(String("GPIO ") + pins[i] + " set " + (output_states[i] ? "HIGH" : "LOW"));
                } else
                  Serial.println(String("GPIO ") + pins[i] + " saved as " + (output_states[i] ? "HIGH" : "LOW"));
              }
            } else
              Serial.println("Bad 'set' request");
          } else if (!header.startsWith("GET / ")) {
            Serial.println("Bad request");
          }

          // Display the HTML web page
          client.println("<!DOCTYPE html><html>");
          client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
          client.println("<link rel=\"icon\" href=\"data:,\">");
          // CSS to style the on/off buttons
          // Feel free to change the background-color and font-size attributes to fit your preferences
          client.println("<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}");
          client.println(".button { background-color: #4CAF50; border: none; color: white; padding: 16px 40px;");
          client.println("text-decoration: none; font-size: 30px; margin: 2px; cursor: pointer;}");
          client.println(".red {background-color: #ff5555;");
          client.println(".blue {background-color: #5555ff;");
          client.println(".grey {background-color: #555555;}</style></head>");

          // Web Page Heading
          client.println("<body><h1>ESP32 Web Server</h1>");

          for (int i = 0; i < pins_num; i++) {
            // Display current state, and ON/OFF buttons for GPIO 26
            client.println(String("<p>GPIO ") + pins[i] + " - Mode " + (pin_modes[i] ? "OUTPUT" : "INPUT") + " State " + output_states[i] + "</p><p>");
            if (pin_modes[i])
              client.println(String("<a href=\"/set/") + i + "/0\"><button class=\"button blue\">INPUT</button></a>");
            else
              client.println(String("<a href=\"/set/") + i + "/1\"><button class=\"button red\">OUTPUT</button></a>");
            if (output_states[i])
              client.println(String("<a href=\"/set/") + i + "/2\"><button class=\"button grey\">OFF</button></a>");
            else
              client.println(String("<a href=\"/set/") + i + "/3\"><button class=\"button\">ON</button></a>");
          }

          client.println("</p></body></html>");

          // The HTTP response ends with another blank line
          client.println();
          // Break out of the while loop
          break;
        } else { // if you got a newline, then clear currentLine
          currentLine = "";
        }
      } else if (c != '\r') {  // if you got anything else but a carriage return character,
        currentLine += c;      // add it to the end of the currentLine
      }
    }
  }
  // Clear the header variable
  header = "";
  // Close the connection
  client.stop();
  Serial.println("Client disconnected.");
  Serial.println("");
}
