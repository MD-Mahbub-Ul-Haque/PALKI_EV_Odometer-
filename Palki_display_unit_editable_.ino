//Define colours for tft
#define RED2RED 0
#define GREEN2GREEN 1
#define BLUE2BLUE 2
#define BLUE2RED 3
#define GREEN2RED 4
#define RED2GREEN 5
#define TFT_GREY 0x2104
#define TFT_GREY 0x5AEB
#define LTBLUE    0xB6DF
#define LTTEAL    0xBF5F
#define LTGREEN         0xBFF7
#define LTCYAN    0xC7FF
#define LTRED           0xFD34
#define LTMAGENTA       0xFD5F
#define LTYELLOW        0xFFF8
#define LTORANGE        0xFE73
#define LTPINK          0xFDDF
#define LTPURPLE  0xCCFF
#define LTGREY          0xE71C
#define BLUE            0x001F
#define TEAL    0x0438
#define GREEN           0x07E0
#define CYAN          0x07FF
#define RED           0xF800
#define MAGENTA       0xF81F
#define YELLOW        0xFFE0
#define ORANGE        0xFD20
#define PINK          0xF81F
#define PURPLE    0x801F
#define GREY        0xC618
#define WHITE         0xFFFF
#define BLACK         0x0000
#define DKBLUE        0x000D
#define DKTEAL    0x020C
#define DKGREEN       0x03E0
#define DKCYAN        0x03EF
#define DKRED         0x6000
#define DKMAGENTA       0x8008
#define DKYELLOW        0x8400
#define DKORANGE        0x8200
#define DKPINK          0x9009
#define DKPURPLE      0x4010
#define DKGREY        0x4A49


// Include lib
#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>
#include <JPEGDecoder.h>// for the logo detector
#define minimum(a,b)     (((a) < (b)) ? (a) : (b))

// include .h file
#include "palki.h" //Palki motors logo
#include "alert.h" // Out of range alert icon

//sd card
/*
  SPI Pins of ESP32 SD card as follows:
 SS    = 5;
 MOSI  = 13;
 MISO  = 12;
 SCK   = 14;
*/
//#include <SD.h>
//
//File myFile;
//const int SD_CS = 5;


// inter eprom

#include <EEPROM.h>

//other define 
#define right_PIN 21
#define left_PIN 22
#define deeper 25
double odo =0 ;
TFT_eSPI tft = TFT_eSPI(); // Invoke custom library with default width and height

// Other veriables 
boolean graph_2 = true;


uint32_t runTime = -99999;// time for next update

int reading = 0; // Value to be displayed
int bar_val = 0;
int d = 0; // Variable used for the sinewave test waveform
bool range_error = 0;
int8_t ramp = 1;

double distance = 0;
double k=0;
unsigned long previousMillis = 0;
const int interval = 1000;
String distanceString;

// firebase setup

#if defined(ESP32)
#include <WiFi.h>
#include <FirebaseESP32.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#endif

#include "time.h"

/* 1. Define the WiFi credentials */
#define WIFI_SSID "Palki2"
#define WIFI_PASSWORD "palki1100"

/* 2. Define the API Key */
#define API_KEY "fgyoNEcMk1nRVI4CZJ9Y78qXBgtDkSFVlxccnubX"

/* 3. Define the RTDB URL */
#define DATABASE_URL "https://palki-genesis-default-rtdb.firebaseio.com/"

// Define Firebase Data object
FirebaseData fbdo;



// variabls for firebase

int base_Distence;
int base_charge; 
int base_speed;



void setup() {
  
  tft.begin();
  Serial.begin(115200);
  tft.setRotation(2);  // portrait
  tft.fillScreen(0x0000);
  int x = (tft.width()  - 300) / 2 - 1; //x cordinate for image
  int y = (tft.height() - 300) / 2 - 1; //y cordinate for image

  drawArrayJpeg(palki, sizeof(palki), x, y); // Draw Palki logo jpeg image stored in memory at x,y
  delay(500); // Staying the logo for 1 sec
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  pinMode(right_PIN, INPUT_PULLUP);
  pinMode(left_PIN, INPUT_PULLUP);
  pinMode(deeper, INPUT_PULLUP);

//  //Sd setup
//  while (!Serial);
//  
//  // initialize HSPI
//  SPI.begin(14, 12, 13, SD_CS);
//
//  Serial.print("Initializing SD card...");
//
//  if (!SD.begin(SD_CS, SPI, 4000000)) {
//    Serial.println("initialization failed!");
//    return;
//  }
//  Serial.println("initialization done.");
//  WriteFile("/test.txt", "palki motors limited");
//  ReadFile("/test.txt");


//database setup
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
  }
  
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(DATABASE_URL, API_KEY);
  Firebase.setDoubleDigits(5);

  configTime(0, 0, "pool.ntp.org");



  EEPROM.begin(512); // initialize EEPROM with size of 512 bytes
}

void loop() {

    if (millis() - runTime >= 0L) { // Execute every TBD ms
    runTime = millis();
    int volt = analogRead(34);// value form Analogue 34
   // Serial.println(volt);
    bar_val = (map(volt, 0, 1023, 0, 100))/4; // Ready the value for charging bar
    DrawBarChartV(tft, 370,  270, 50, 180, 0, 100 , 5, bar_val , 4 , 0, MAGENTA, DKBLUE, CYAN, WHITE, BLACK, "Battery", graph_2); //xpos,ypos, width,height,L_range,H_range,step,reading, font, decimial 

    base_charge = bar_val; 
    
    int rightState = digitalRead(right_PIN);
    int leftState = digitalRead(left_PIN);
    int deeperState = digitalRead(deeper);

  //print out the right state
  //Serial.println(rightState);


    // Set the the position, gap between meters, and inner radius of the meters
    int xpos = 0, ypos = 5, gap = 4, radius = 52;

    // Draw the meter
    xpos = 50, ypos = 75, gap = 25, radius = 120;
  
    // read the input on analog pin GIOP36:
    //int speedValue = analogRead(13);
    //Serial.println(speedValue);
    int speedValue = masureSpeed();
    Serial.println(speedValue);
    base_speed = speedValue; 

    // Rescale to potentiometer's voltage (from 0V to 3.3V):
    float voltage = floatMap(speedValue, 0, 45, 0, 45);

    reading +=(ramp);
    if (reading>98) ramp = -1;
    if (reading<0) ramp = 1;
    // Comment out above meters, then uncomment the next line to show large meter
    ringMeter(voltage,0,100, xpos,ypos,radius," km/h",GREEN2RED); // Draw analogue meter
    if (reading<0) delay(1000);

    

    //for idicator 
    if (rightState == HIGH)
      rightIdicator();
    else if (leftState == HIGH)
      leftIdicator();
    else if (leftState == HIGH && rightState == HIGH)
      emergencyIdicator();
    else {
      noIdicator();
      }


      unsigned long currentMilis = millis();
      tripMeter(speedValue, currentMilis);

      

      
      //odoMeter(speedValue, currentMilis);
      if (deeperState == HIGH){
        deeper_on();
      }
      else{
        clear_deeper();
        }
  }

  
  if (Firebase.ready())
  {
    time_t now;
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
      Serial.println("Failed to obtain time");
      return;
    }
    now = time(nullptr);
    char timestamp[20];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", &timeinfo);
    
    Firebase.setString(fbdo, "/test/timestamp", timestamp);
    Firebase.setInt(fbdo, "/test/Charge", base_charge);
    Firebase.setInt(fbdo, "/test/Speed", base_speed);
    Firebase.setInt(fbdo, "/test/Distence", base_Distence);
    delay(200);

    Serial.printf("Get int Charge--  %s\n", Firebase.getInt(fbdo, "/Charge/base_charge") ? String(fbdo.to<int>()).c_str() : fbdo.errorReason().c_str());
    base_charge = fbdo.to<int>();
    Serial.printf("Get int Speed--  %s\n", Firebase.getInt(fbdo, "/Speed/base_speed") ? String(fbdo.to<int>()).c_str() : fbdo.errorReason().c_str());
    base_speed = fbdo.to<int>();
    Serial.printf("Get int Distence--  %s\n", Firebase.getInt(fbdo, "/Distence/base_Distence") ? String(fbdo.to<int>()).c_str() : fbdo.errorReason().c_str());
    base_Distence = fbdo.to<int>();

//    Serial.println();
//    Serial.print("a:");
    
  }

}

// SD card read & write

//
//void WriteFile(const char * path, const char * message){
//  // open the file. note that only one file can be open at a time,
//  // so you have to close this one before opening another.
//  myFile = SD.open(path, FILE_WRITE);
//  // if the file opened okay, write to it:
//  if (myFile) {
//    Serial.printf("Writing to %s ", path);
//    myFile.println(message);
//    myFile.close(); // close the file:
//    Serial.println("completed.");
//  } 
//  // if the file didn't open, print an error:
//  else {
//    Serial.println("error opening file ");
//    Serial.println(path);
//  }
//}
//String ReadFile(const char * path){
//  String content;
//  // open the file for reading:
//  myFile = SD.open(path);
//  if (myFile) {
//     Serial.printf("Reading file from %s\n", path);
//     // read from the file until there's nothing else in it:
//    while (myFile.available()) {
//      content += (char)myFile.read();
//    }
//    myFile.close(); // close the file:
//  } 
//  else {
//    // if the file didn't open, print an error:
//    Serial.println("error opening test.txt");
//  }
//  return content;
//}
//



int masureSpeed(){
  int pulseDuration = pulseIn(13, HIGH); // measure the pulse duration on digital input 2
  int waveLength = 2 * pulseDuration;   // calculate the wave length
  Serial.println(waveLength);           // print the wave length to the serial monitor
  int speed = map(waveLength, 0, 17000, 0, 36);
  Serial.print(speed);  
  Serial.println("km/h"); 

  return speed;
  
  }

void odoMeter(int s,unsigned long currentMillis ){


  int address = 0xFF;
  double readValue = EEPROM.read(address);
  Serial.println(readValue); // print value to serial monitor
  
  
  // write to EEPROM
  
  double value = readValue + distance;
  EEPROM.write(address, value);
  EEPROM.commit(); // save changes to EEPROM
  //odo
//
    tft.setTextColor(CYAN);
    tft.setTextSize(2);
    tft.setCursor(100 , 12);
    tft.fillRect(100, 12, tft.width(), tft.fontHeight() + 2, TFT_BLACK);
//    
//    Serial.print("prining: ");
//    Serial.println(readodo);
    tft.print("Odo: ");
    tft.print(value);
    tft.println(" Km");
    delay(500);

  }

 void tripMeter(int s,unsigned long currentMillis ){


  int address = 0xFF;
  int readValue = EEPROM.read(address);
  
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    int speed = s; 
    distance += (double)speed / 3600;
    k += (double)speed / 3600;
    distanceString = String(distance);

    if (k >= 1){
    readValue = readValue +1;
    double value = readValue;
    EEPROM.write(address, value);
    EEPROM.commit(); // save changes to EEPROM
    k=0;
    }

   
  }

    tft.setTextColor(YELLOW);
    tft.setTextSize(4);
    tft.setCursor(100 , 10);
    //tft.fillRect(100, 50, tft.width(), tft.fontHeight() + 2, TFT_BLACK);
    tft.print("CITY BOY LFP" );

    
  
    tft.setTextColor(CYAN);
    tft.setTextSize(2);
    tft.setCursor(100 , 50);
    tft.fillRect(100, 50, tft.width(), tft.fontHeight() + 2, TFT_BLACK);
    tft.print("Signle Trip: " );
    tft.print(distance);
    tft.println(" km" );

      //odo
    int PrintValue = EEPROM.read(address);
    tft.setTextColor(CYAN);
    tft.setTextSize(2);
    tft.setCursor(80 , 300);
    tft.fillRect(80, 300, tft.width(), tft.fontHeight() + 2, TFT_BLACK);
    tft.print("Total Distance: ");
    tft.print(PrintValue);
    tft.println(" Km");
    base_Distence = PrintValue; 
  }

void deeper_on(){
  int sun_x = 35;
  int sun_y = 75;
  int sun_radius = 10;

  // Clear the previous frame
  //tft.fillCircle(sun_x, sun_y, sun_radius+10, BLACK);
  
  // Draw the sun
  tft.fillCircle(sun_x, sun_y, sun_radius, YELLOW);
  
  // Draw the sun's rays
  for (int i = 0; i < 360; i += 30) {
    float angle = i * 3.14159 / 180.0;
    int x1 = sun_x + sun_radius * cos(angle);
    int y1 = sun_y + sun_radius * sin(angle);
    int x2 = sun_x + (sun_radius+20) * cos(angle);
    int y2 = sun_y + (sun_radius+20) * sin(angle);
    tft.drawLine(x1, y1, x2, y2, YELLOW);
  }

}

void clear_deeper(){
  int sun_x = 35;
  int sun_y = 75;
  int sun_radius = 10;

  // Clear the previous frame
  tft.fillCircle(sun_x, sun_y, sun_radius+10, BLACK);
}
void rightIdicator(){
  
//    x0：x coordinate of the apex
//
//    y0：y coordinate of the apex
//
//    x1：x coordinate of the left bottom point
//
//    y1：y coordinate of the left bottom point
//
//    x2：x coordinate of the right bottom point
//
//    y2：y coordinate of the right bottom point
//
//    color：color of the triangle

    
    // For right tringle 
    tft.drawTriangle(480, 18, 400, 2, 400, 38, CYAN); // outer line 
    tft.fillTriangle(478, 18, 402, 3, 402, 37, YELLOW);// filler
  }

  void leftIdicator(){
     
    // For left tringle 
    tft.drawTriangle(2, 18, 82, 2, 82, 38, CYAN); // outer line 
    tft.fillTriangle(4, 18, 80, 3, 80, 37, YELLOW);// filler

  }
  void emergencyIdicator(){
    // For right tringle 
    tft.drawTriangle(480, 18, 400, 2, 400, 38, CYAN); // outer line 
    tft.fillTriangle(478, 18, 402, 3, 402, 37, YELLOW);// filler
    // For left tringle 
    tft.drawTriangle(2, 18, 82, 2, 82, 38, CYAN); // outer line 
    tft.fillTriangle(4, 18, 80, 3, 80, 37, YELLOW);// filler
  }
   
  void noIdicator(){
    // For right tringle 
    tft.drawTriangle(480, 18, 400, 2, 400, 38, CYAN); // outer line 
    tft.fillTriangle(478, 18, 402, 3, 402, 37, BLACK);// filler
    // For left tringle 
    tft.drawTriangle(2, 18, 82, 2, 82, 38, CYAN); // outer line 
    tft.fillTriangle(4, 18, 80, 3, 80, 37, BLACK);// filler
  }
//method for mapping 
float floatMap(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


// #########################################################################
//  Draw the meter on the screen, returns x coord of righthand side
// #########################################################################
int ringMeter(int value, int vmin, int vmax, int x, int y, int r, const char *units, byte scheme)
{
  // Minimum value of r is about 52 before value text intrudes on ring
  // drawing the text first is an option
  
  x += r; y += r;   // Calculate coords of centre of ring

  int w = r / 3;    // Width of outer ring is 1/4 of radius
  
  int angle = 150;  // Half the sweep angle of meter (300 degrees)

  int v = map(value, vmin, vmax, -angle, angle); // Map the value to an angle v

  byte seg = 3; // Segments are 3 degrees wide = 100 segments for 300 degrees
  byte inc = 6; // Draw segments every 3 degrees, increase to 6 for segmented ring

  // Variable to save "value" text colour from scheme and set default
  int colour = TFT_BLUE;
 
  // Draw colour blocks every inc degrees
  for (int i = -angle+inc/2; i < angle-inc/2; i += inc) {
    // Calculate pair of coordinates for segment start
    float sx = cos((i - 90) * 0.0174532925);
    float sy = sin((i - 90) * 0.0174532925);
    uint16_t x0 = sx * (r - w) + x;
    uint16_t y0 = sy * (r - w) + y;
    uint16_t x1 = sx * r + x;
    uint16_t y1 = sy * r + y;

    // Calculate pair of coordinates for segment end
    float sx2 = cos((i + seg - 90) * 0.0174532925);
    float sy2 = sin((i + seg - 90) * 0.0174532925);
    int x2 = sx2 * (r - w) + x;
    int y2 = sy2 * (r - w) + y;
    int x3 = sx2 * r + x;
    int y3 = sy2 * r + y;

    if (i < v) { // Fill in coloured segments with 2 triangles
      switch (scheme) {
        case 0: colour = TFT_RED; break; // Fixed colour
        case 1: colour = TFT_GREEN; break; // Fixed colour
        case 2: colour = TFT_BLUE; break; // Fixed colour
        case 3: colour = rainbow(map(i, -angle, angle, 0, 127)); break; // Full spectrum blue to red
        case 4: colour = rainbow(map(i, -angle, angle, 70, 127)); break; // Green to red (high temperature etc)
        case 5: colour = rainbow(map(i, -angle, angle, 127, 63)); break; // Red to green (low battery etc)
        default: colour = TFT_BLUE; break; // Fixed colour
      }
      tft.fillTriangle(x0, y0, x1, y1, x2, y2, colour);
      tft.fillTriangle(x1, y1, x2, y2, x3, y3, colour);
      //text_colour = colour; // Save the last colour drawn
    }
    else // Fill in blank segments
    {
      tft.fillTriangle(x0, y0, x1, y1, x2, y2, TFT_GREY);
      tft.fillTriangle(x1, y1, x2, y2, x3, y3, TFT_GREY);
    }
  }
  // Convert value to a string
  char buf[10];
  byte len = 3; if (value > 999) len = 5;
  dtostrf(value, len, 0, buf);
  buf[len] = ' '; buf[len+1] = 0; // Add blanking space and terminator, helps to centre text too!
  // Set the text colour to default
  tft.setTextSize(1);

  if (value<vmin || value>vmax) {
    drawAlert(x,y+90,50,1);
  }
  else {
    drawAlert(x,y+90,50,0);
  }

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  // Uncomment next line to set the text colour to the last segment value!
  tft.setTextColor(colour, TFT_BLACK);
  tft.setTextDatum(MC_DATUM);
  // Print value, if the meter is large then use big font 8, othewise use 4
  if (r > 84) {
    tft.setTextPadding(25*3); // Allow for 3 digits each 55 pixels wide
    tft.drawString(buf, x, y, 6); // Value in middle
  }
  else {
    tft.setTextPadding(3 * 5); // Allow for 3 digits each 14 pixels wide
    tft.drawString(buf, x, y, 4); // Value in middle
  }
  tft.setTextSize(1);
  tft.setTextPadding(0);
  // Print units, if the meter is large then use big font 4, othewise use 2
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  if (r > 84) tft.drawString(units, x, y + 60, 4); // Units display
  else tft.drawString(units, x, y + 15, 2); // Units display

  // Calculate and return right hand side x coordinate
  return x + r;
}

void drawAlert(int x, int y , int side, bool draw)
{
  if (draw && !range_error) {
    drawIcon(alert, x - alertWidth/2, y - alertHeight/2, alertWidth, alertHeight);
    range_error = 1;
  }
  else if (!draw) {
    tft.fillRect(x - alertWidth/2, y - alertHeight/2, alertWidth, alertHeight, TFT_BLACK);
    range_error = 0;
  }
}

// #########################################################################
// Return a 16 bit rainbow colour
// #########################################################################
unsigned int rainbow(byte value)
{
  // Value is expected to be in range 0-127
  // The value is converted to a spectrum colour from 0 = blue through to 127 = red

  byte red = 0; // Red is the top 5 bits of a 16 bit colour value
  byte green = 0;// Green is the middle 6 bits
  byte blue = 0; // Blue is the bottom 5 bits

  byte quadrant = value / 32;

  if (quadrant == 0) {
    blue = 31;
    green = 2 * (value % 32);
    red = 0;
  }
  if (quadrant == 1) {
    blue = 31 - (value % 32);
    green = 63;
    red = 0;
  }
  if (quadrant == 2) {
    blue = 0;
    green = 63;
    red = value % 32;
  }
  if (quadrant == 3) {
    blue = 0;
    green = 63 - 2 * (value % 32);
    red = 31;
  }
  return (red << 11) + (green << 5) + blue;
}

// #########################################################################
// Return a value in range -1 to +1 for a given phase angle in degrees
// #########################################################################
float sineWave(int phase) {
  return sin(phase * 0.0174532925);
}

//====================================================================================
// This is the function to draw the icon stored as an array in program memory (FLASH)
//====================================================================================

// To speed up rendering we use a 64 pixel buffer
#define BUFF_SIZE 64

// Draw array "icon" of defined width and height at coordinate x,y
// Maximum icon size is 255x255 pixels to avoid integer overflow

void drawIcon(const unsigned short* icon, int16_t x, int16_t y, int8_t width, int8_t height) {

  uint16_t  pix_buffer[BUFF_SIZE];   // Pixel buffer (16 bits per pixel)

  tft.startWrite();

  // Set up a window the right size to stream pixels into
  tft.setAddrWindow(x, y, width, height);

  // Work out the number whole buffers to send
  uint16_t nb = ((uint16_t)height * width) / BUFF_SIZE;

  // Fill and send "nb" buffers to TFT
  for (int i = 0; i < nb; i++) {
    for (int j = 0; j < BUFF_SIZE; j++) {
      pix_buffer[j] = pgm_read_word(&icon[i * BUFF_SIZE + j]);
    }
    tft.pushColors(pix_buffer, BUFF_SIZE);
  }

  // Work out number of pixels not yet sent
  uint16_t np = ((uint16_t)height * width) % BUFF_SIZE;

  // Send any partial buffer left over
  if (np) {
    for (int i = 0; i < np; i++) pix_buffer[i] = pgm_read_word(&icon[nb * BUFF_SIZE + i]);
    tft.pushColors(pix_buffer, np);
  }

  tft.endWrite();
}

void DrawBarChartV(TFT_eSPI & d, double x , double y , double w, double h , double loval , double hival , double inc , double curval ,  int dig , int dec, unsigned int barcolor, unsigned int voidcolor, unsigned int bordercolor, unsigned int textcolor, unsigned int backcolor, String label, boolean & redraw)
{

  double stepval, range;
  double my, level;
  double i, data;
  // draw the border, scale, and label once
  // avoid doing this on every update to minimize flicker
  if (redraw == true) {
    redraw = false;

    d.drawRect(x - 1, y - h - 1, w + 2, h + 2, bordercolor);
    d.setTextColor(textcolor, backcolor);
    d.setTextSize(2);
    d.setCursor(x , y + 10);
    d.print(label);
    d.println(" %");
    // step val basically scales the hival and low val to the height
    // deducting a small value to eliminate round off errors
    // this val may need to be adjusted
    stepval = ( inc) * (double (h) / (double (hival - loval))) - .001;
    for (i = 0; i <= h; i += stepval) {
      my =  y - h + i;
      d.drawFastHLine(x + w + 1, my,  5, textcolor);
      // draw lables
      d.setTextSize(1);
      d.setTextColor(textcolor, backcolor);
      d.setCursor(x + w + 12, my - 3 );
      data = hival - ( i * (inc / stepval));
      d.println(Format(data, dig, dec));
      
    }
  }
  // compute level of bar graph that is scaled to the  height and the hi and low vals
  // this is needed to accompdate for +/- range
  level = (h * (((curval - loval) / (hival - loval))));
  // draw the bar graph
  // write a upper and lower bar to minimize flicker cause by blanking out bar and redraw on update
  d.fillRect(x, y - h, w, h - level,  voidcolor);
  if(curval<=25){
  d.fillRect(x, y - level, w,  level, RED);
  }
  else if(curval>25 && curval <= 50 ){
  d.fillRect(x, y - level, w,  level, ORANGE);
  }
  else if(curval>50 && curval <=75 ){
  d.fillRect(x, y - level, w,  level, YELLOW);
  }
  else{
  d.fillRect(x, y - level, w,  level, GREEN);
  }
  // write the current value
  d.setTextColor(textcolor, backcolor);
  d.setTextSize(2);
  d.setCursor(x , y - h - 23);
  d.print(Format(curval, dig, dec));
  d.println(" %");

}


String Format(double val, int dec, int dig ) {
  int addpad = 0;
  char sbuf[20];
  String condata = (dtostrf(val, dec, dig, sbuf));


  int slen = condata.length();
  for ( addpad = 1; addpad <= dec + dig - slen; addpad++) {
    condata = " " + condata;
  }
  return (condata);

}


//####################################################################################################
// Draw a JPEG on the TFT pulled from a program memory array
//####################################################################################################
void drawArrayJpeg(const uint8_t arrayname[], uint32_t array_size, int xpos, int ypos) {

  int x = xpos;
  int y = ypos;

  JpegDec.decodeArray(arrayname, array_size);
  
  
  
  renderJPEG(x, y);
  
  Serial.println("#########################");
}

//####################################################################################################
// Draw a JPEG on the TFT, images will be cropped on the right/bottom sides if they do not fit
//####################################################################################################
// This function assumes xpos,ypos is a valid screen coordinate. For convenience images that do not
// fit totally on the screen are cropped to the nearest MCU size and may leave right/bottom borders.
void renderJPEG(int xpos, int ypos) {

  // retrieve infomration about the image
  uint16_t *pImg;
  uint16_t mcu_w = JpegDec.MCUWidth;
  uint16_t mcu_h = JpegDec.MCUHeight;
  uint32_t max_x = JpegDec.width;
  uint32_t max_y = JpegDec.height;

  // Jpeg images are draw as a set of image block (tiles) called Minimum Coding Units (MCUs)
  // Typically these MCUs are 16x16 pixel blocks
  // Determine the width and height of the right and bottom edge image blocks
  uint32_t min_w = minimum(mcu_w, max_x % mcu_w);
  uint32_t min_h = minimum(mcu_h, max_y % mcu_h);

  // save the current image block size
  uint32_t win_w = mcu_w;
  uint32_t win_h = mcu_h;

  // record the current time so we can measure how long it takes to draw an image
  uint32_t drawTime = millis();

  // save the coordinate of the right and bottom edges to assist image cropping
  // to the screen size
  max_x += xpos;
  max_y += ypos;

  // read each MCU block until there are no more
  while (JpegDec.read()) {
    
    // save a pointer to the image block
    pImg = JpegDec.pImage ;

    // calculate where the image block should be drawn on the screen
    int mcu_x = JpegDec.MCUx * mcu_w + xpos;  // Calculate coordinates of top left corner of current MCU
    int mcu_y = JpegDec.MCUy * mcu_h + ypos;

    // check if the image block size needs to be changed for the right edge
    if (mcu_x + mcu_w <= max_x) win_w = mcu_w;
    else win_w = min_w;

    // check if the image block size needs to be changed for the bottom edge
    if (mcu_y + mcu_h <= max_y) win_h = mcu_h;
    else win_h = min_h;

    // copy pixels into a contiguous block
    if (win_w != mcu_w)
    {
      uint16_t *cImg;
      int p = 0;
      cImg = pImg + win_w;
      for (int h = 1; h < win_h; h++)
      {
        p += mcu_w;
        for (int w = 0; w < win_w; w++)
        {
          *cImg = *(pImg + w + p);
          cImg++;
        }
      }
    }

    // calculate how many pixels must be drawn
    uint32_t mcu_pixels = win_w * win_h;

    tft.startWrite();

    // draw image MCU block only if it will fit on the screen
    if (( mcu_x + win_w ) <= tft.width() && ( mcu_y + win_h ) <= tft.height())
    {

      // Now set a MCU bounding window on the TFT to push pixels into (x, y, x + width - 1, y + height - 1)
      tft.setAddrWindow(mcu_x, mcu_y, win_w, win_h);

      // Write all MCU pixels to the TFT window
      while (mcu_pixels--) {
        // Push each pixel to the TFT MCU area
        tft.pushColor(*pImg++);
      }

    }
    else if ( (mcu_y + win_h) >= tft.height()) JpegDec.abort(); // Image has run off bottom of screen so abort decoding

    tft.endWrite();
  }

  // calculate how long it took to draw the image
  drawTime = millis() - drawTime;

  // print the results to the serial port
  Serial.print(F(  "Total render time was    : ")); Serial.print(drawTime); Serial.println(F(" ms"));
  Serial.println(F(""));
}


//####################################################################################################
// Show the execution time (optional)
//####################################################################################################
// WARNING: for UNO/AVR legacy reasons printing text to the screen with the Mega might not work for
// sketch sizes greater than ~70KBytes because 16 bit address pointers are used in some libraries.

// The Due will work fine with the HX8357_Due library.

void showTime(uint32_t msTime) {
  //tft.setCursor(0, 0);
  //tft.setTextFont(1);
  //tft.setTextSize(2);
  //tft.setTextColor(TFT_WHITE, TFT_BLACK);
  //tft.print(F(" JPEG drawn in "));
  //tft.print(msTime);
  //tft.println(F(" ms "));
  Serial.print(F(" JPEG drawn in "));
  Serial.print(msTime);
  Serial.println(F(" ms "));
}
