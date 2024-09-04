//---------------------------------------- Include Library
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>    
#include <HTTPClient.h>
#include <UrlEncode.h>
#include <TinyGPS++.h>
//----------------------------------------

//---------------------------------------- Configure GPS MODULE
#define GPS_BAUDRATE 9600  // The default baudrate of NEO-6M is 9600
#define RXD2 16
#define TXD2 17
HardwareSerial neogps(1);
TinyGPSPlus gps;  // the TinyGPS++ object
float latitude, longitude;
String location;

//---------------------------------------- Configure Pulse Sensor
#define PulseSensor_PIN 36 
#define LED_PIN         23 
#define Button_PIN      32

//---------------------------------------- Configure WIFI
const char* ssid = "SSID WiFi";
const char* password = "Password WiFi";

// +international_country_code + phone number
// Portugal +351, example: +351912345678
String phoneNumber = "Phone Number";
String apiKey = "API Key";

//---------------------------------------- Configure OLED screen size in pixels
#define SCREEN_WIDTH 128 //--> OLED display width, in pixels
#define SCREEN_HEIGHT 64 //--> OLED display height, in pixels
//----------------------------------------

//----------------------------------------Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
//----------------------------------------

unsigned long previousMillisGetHB = 0; //--> will store the last time Millis (to get Heartbeat) was updated.
unsigned long previousMillisResultHB = 0; //--> will store the last time Millis (to get BPM) was updated.

const long intervalGetHB = 20; //--> Interval for reading heart rate (Heartbeat) = 20ms.
const long intervalResultHB = 1000; //--> The reading interval for the result of the Heart Rate calculation.

int timer_Get_BPM = 0;

int PulseSensorSignal; //--> Variable to accommodate the signal value from the sensor.
int UpperThreshold = 520; //--> Determine which Signal to "count as a beat", and which to ignore.
int LowerThreshold = 500; 

int cntHB = 0; //--> Variable for counting the number of heartbeats.
boolean ThresholdStat = true; //--> Variable for triggers in calculating heartbeats.
int BPMval = 0; //--> Variable to hold the result of heartbeats calculation.

int x=0; //--> Variable axis x graph values to display on OLED
int y=0; //--> Variable axis y graph values to display on OLED
int lastx=0; //--> The graph's last x axis variable value to display on the OLED
int lasty=0; //--> The graph's last y axis variable value to display on the OLED

// Boolean variables to start and stop getting BPM values.
bool get_BPM = false;

//----------------------------------------'Heart_Icon', 16x16px
// I drew this heart icon at : http://dotmatrixtool.com/
const unsigned char Heart_Icon [] PROGMEM = {
  0x00, 0x00, 0x18, 0x30, 0x3c, 0x78, 0x7e, 0xfc, 0xff, 0xfe, 0xff, 0xfe, 0xee, 0xee, 0xd5, 0x56, 
  0x7b, 0xbc, 0x3f, 0xf8, 0x1f, 0xf0, 0x0f, 0xe0, 0x07, 0xc0, 0x03, 0x80, 0x01, 0x00, 0x00, 0x00
};
//----------------------------------------

//__void SendMessage()
void sendMessage(String message){

  // Data to send with HTTP POST
  String url = "https://api.callmebot.com/whatsapp.php?phone=" + phoneNumber + "&apikey=" + apiKey + "&text=" + urlEncode(message);    
  HTTPClient http;
  http.begin(url);

  // Specify content-type header
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  
  // Send HTTP POST request
  int httpResponseCode = http.POST(url);
  if (httpResponseCode == 200){
    Serial.print("Message sent successfully");
  }
  else{
    Serial.println("Error sending the message");
    Serial.print("HTTP response code: ");
    Serial.println(httpResponseCode);
  }

  // Free resources
  http.end();
}

//__void GetHeartRate()
void GetHeartRate() {
  //----------------------------------------Process of reading heart beat.
  unsigned long currentMillisGetHB = millis();

  if (currentMillisGetHB - previousMillisGetHB >= intervalGetHB) {
    previousMillisGetHB = currentMillisGetHB;

    PulseSensorSignal = analogRead(PulseSensor_PIN); //--> Read the PulseSensor's value. Assign this value to the "Signal" variable.

    if (PulseSensorSignal > UpperThreshold && ThresholdStat == true) {
      if (get_BPM == true) cntHB++;
      ThresholdStat = false;
      digitalWrite(LED_PIN,HIGH);
    }

    if (PulseSensorSignal < LowerThreshold) {
      ThresholdStat = true;
      digitalWrite(LED_PIN,LOW);
    }
    
    DrawGraph(); //--> Calling the DrawGraph() subroutine.
  }
  //----------------------------------------

  //----------------------------------------The process for getting the BPM value.
  unsigned long currentMillisResultHB = millis();

  if (currentMillisResultHB - previousMillisResultHB >= intervalResultHB) {
    previousMillisResultHB = currentMillisResultHB;

    if (get_BPM == true) {
      timer_Get_BPM++;
      // "timer_Get_BPM > 10" means taking the number of heartbeats for 10 seconds.
      if (timer_Get_BPM > 10) {
        timer_Get_BPM = 1;

        BPMval = cntHB * 6; //--> The taken heart rate is for 10 seconds. So to get the BPM value, the total heart rate in 10 seconds x 6.
        Serial.print("BPM : ");
        Serial.println(BPMval);
        
        display.fillRect(20, 48, 108, 18, BLACK);
        
        display.drawBitmap(0, 47, Heart_Icon, 16, 16, WHITE); //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)   
        display.drawLine(0, 43, 127, 43, WHITE); //--> drawLine(x1, y1, x2, y2, color)
        display.drawLine(69, 43, 69, 64, WHITE); //--> drawLine(x1, y1, x2, y2, color)

        display.setTextSize(1.4);
        display.setTextColor(WHITE);
        display.setCursor(16, 50); //--> (x position, y position)
        display.print(":");
        display.print(BPMval);
        display.setCursor(45, 50); //--> (x position, y position)
        display.print("BPM");
        display.setCursor(77, 50); //--> (x position, y position)
        display.print("LOC: ");
        display.setCursor(102, 50); //--> (x position, y position)
        display.print(location);
        display.display(); 
        cntHB = 0;
        notification(); //---> Send Notification when BPM < 60 or > 100
      }
    }
  }
  //----------------------------------------
}
//________________________________________________________________________________

//__DrawGraph()
// Subroutines for drawing or displaying heart rate graphic signals.
void DrawGraph() {
  //----------------------------------------Condition to reset the graphic display if it fills the width of the OLED screen.
  if (x > 127) {
    display.fillRect(0, 0, 128, 42, BLACK);
    x = 0;
    lastx = 0;
  }
  //----------------------------------------

  //----------------------------------------Process signal data to be displayed on OLED in graphic form.
  int ySignal = PulseSensorSignal; 
  
  if (ySignal > 850) ySignal = 850;
  if (ySignal < 350) ySignal = 350;

  int ySignalMap = map(ySignal, 350, 850, 0, 40); //--> The y-axis used on OLEDs is from 0 to 40.
  
  y = 40 - ySignalMap;
  //----------------------------------------

  //----------------------------------------Displays the heart rate graph.
  display.writeLine(lastx,lasty,x,y,WHITE);
  display.display(); 
  //----------------------------------------
  
  lastx = x;
  lasty = y;
  
  x++;
}
//________________________________________________________________________________

//__VOID SETUP()
void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200); //--> Set's up Serial Communication at certain speed.
  neogps.begin(GPS_BAUDRATE, SERIAL_8N1, RXD2, TXD2); //Begin serial communication Neo6mGPS
  Serial.println();
  delay(2000);

  analogReadResolution(10);

  pinMode(LED_PIN,OUTPUT); 
  pinMode(Button_PIN, INPUT_PULLUP);

  //---------------------------------------- SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally.
  // Address 0x3C for 128x32 and Address 0x3D for 128x64.
  // But on my 128x64 module the 0x3D address doesn't work. What works is the 0x3C address.
  // So please try which address works on your module.
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); //--> Don't proceed, loop forever
  }
  //----------------------------------------

  //---------------------------------------- WIFI
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  display.clearDisplay(); 
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.setCursor(37, 0); 
  display.print("ESP32"); 
  display.setCursor(13, 20); 
  display.print("HEARTBEAT"); 
  display.setCursor(7, 40); 
  display.print("MONITORING"); 
  display.display();
  delay(2000);

  //----------------------------------------
  display.clearDisplay(); 
  display.drawLine(0, 43, 127, 43, WHITE); //--> drawLine(x1, y1, x2, y2, color)
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10, 48); //--> (x position, y position)
  display.print("HeartBeat");
  display.display(); 
}

void GPS(){
  if (neogps.available() > 0){
    if (gps.encode(neogps.read())){
      if (gps.location.isValid()){
        location = "DET";
        latitude = gps.location.lat();
        longitude = gps.location.lng();
        Serial.print(F("- latitude: "));
        Serial.println(latitude);

        Serial.print(F("- longitude: "));
        Serial.println(longitude);

        Serial.print(F("- altitude: "));
        if (gps.altitude.isValid())
          Serial.println(gps.altitude.meters());
        else
          Serial.println(F("INVALID"));
      } else {
        location = "INV";
        Serial.println(F("- location: INVALID"));
      }
    }
  }
  if (millis() > 5000 && gps.charsProcessed() < 10)
    Serial.println(F("No GPS data received: check wiring"));
}

void notification(){
  if (BPMval < 60){
    String message = "LAPORAN DETAK JANTUNG\n";
    message += "===============================\n";
    message += "BPM: " + String(BPMval) + "\n";
    message += "Status: Detak Jantung Rendah\n";
    message += "Lokasi: ";
    message += "https://maps.google.com/maps?&z=15&mrt=yp&t=k&q=" + String(latitude, 6) + "+" +  String(longitude, 6);
    sendMessage(message);
    Serial.print(message);
  } if (BPMval > 100){
    String message = "LAPORAN DETAK JANTUNG\n";
    message += "===============================\n";
    message += "BPM: " + String(BPMval) + "\n";
    message += "Status: Detak Jantung Tinggi\n";
    message += "Lokasi: ";
    message += "https://maps.google.com/maps?&z=15&mrt=yp&t=k&q=" + String(latitude, 6) + "+" + String(longitude, 6);
    sendMessage(message);
    Serial.print(message);
  }
}
//________________________________________________________________________________

//__VOID LOOP()
void loop() {
  // put your main code here, to run repeatedly:
  if (Button_PIN == LOW) {
    delay(100);

    cntHB = 0;
    BPMval = 0;
    x = 0;
    y = 0;
    lastx = 0;
    lasty = 0;

    get_BPM = !get_BPM;

    if (get_BPM == true) {
      display.clearDisplay(); 
      display.setTextColor(WHITE);
      display.setTextSize(1);
      display.setCursor(14, 0); 
      display.print("Start Getting BPM"); 

      display.setTextSize(3);

      for (byte i = 3; i > 0; i--) {
        display.setTextColor(WHITE);
        display.setCursor(50, 20); 
        display.print(i); 
        display.display();
        delay(1000);
        display.setTextColor(BLACK);
        display.setCursor(50, 20); 
        display.print(i); 
        display.display();
      }

      delay(500);

      //----------------------------------------Displays BPM value reading information
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      
      display.setCursor(0, 12); //--> (x position, y position)
      display.print("     Please wait");
      
      display.setCursor(0, 22); //--> (x position, y position)
      display.print("     10  seconds");
      
      display.setCursor(0, 32); //--> (x position, y position)
      display.print("       to get");
      
      display.setCursor(0, 42); //--> (x position, y position)
      display.print("    the BPM value");
      
      display.display(); 
      delay(3000);
      //----------------------------------------

      //----------------------------------------Displays the initial display of BPM value
      display.clearDisplay(); //--> for Clearing the display

      display.drawBitmap(0, 47, Heart_Icon, 16, 16, WHITE); //--> display.drawBitmap(x position, y position, bitmap data, bitmap width, bitmap height, color)   
      display.drawLine(0, 43, 127, 43, WHITE); //--> drawLine(x1, y1, x2, y2, color)
      display.drawLine(69, 43, 69, 64, WHITE); //--> drawLine(x1, y1, x2, y2, color)

      display.setTextSize(1.4);
      display.setTextColor(WHITE);
      display.setCursor(16, 50); //--> (x position, y position)
      display.print(":");
      display.print("0");
      display.setCursor(45, 50); //--> (x position, y position)
      display.print("BPM");
      display.setCursor(77, 50); //--> (x position, y position)
      display.print("LOC: ");
      display.setCursor(102, 50); //--> (x position, y position)
      display.print(location);
      display.display(); 
      //----------------------------------------
    }
    else {
      display.clearDisplay();
      display.setTextColor(WHITE);
      display.setTextSize(2);
      display.setCursor(42, 25); 
      display.print("STOP"); 
      display.display();
      delay(2000);
      display.clearDisplay(); 
      display.drawLine(0, 43, 127, 43, WHITE); //--> drawLine(x1, y1, x2, y2, color)
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(10, 48); //--> (x position, y position)
      display.print("HeartBeat");
      display.display(); 
    }
  }
  GPS(); //---> Calling GPS Coordinate.
  GetHeartRate(); //--> Calling the GetHeartRate() subroutine.
}