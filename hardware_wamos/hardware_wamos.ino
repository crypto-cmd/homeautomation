

#include <SoftwareSerial.h>
#include <ArduinoJson.h>

#include <NewPing.h>

#include <math.h>

//**********ENTER IP ADDRESS OF SERVER******************//

#define HOST_IP "172.16.193.254" // REPLACE with IP address of your server
#define HOST_PORT "9000"         // REPLACE with your server port
#define route "api/update"       // leave unchanged
#define idNumber "620164974"     // REPLACE with your ID number

// WIFI CREDENTIALS
#define SSID "MonaConnect" // REPLACE with your WiFi SSID
#define password ""      // REPLACE with your WiFi password

#define stay 100

//**********PIN DEFINITIONS******************//

#define espRX 10
#define espTX 11
#define espTimeout_ms 300

// HC‐SR04 sensor pins (using pins 12 and 13 since 10 & 11 go to the ESP)
#define TRIGGER_PIN 12   // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN 13      // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 500 // Maximum distance we want to ping for in inches

/* Declare your functions below */
void espSend(char command[]);
void espUpdate(char mssg[]);
void espInit();
double getWaterHeight(double radar);
double getRadar();
double getWaterReserve(double height);

SoftwareSerial esp(espRX, espTX);
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

//**********TANK DIMENSIONS & SENSOR HEIGHT**********//
#define MAX_WATER_HEIGHT 77.763 // Maximum water level/height from the base of the tank
#define MAX_CAPACITY 1000       // Maximum capacity of the tank in US Gallons
#define SENSOR_HEIGHT 94.5      // Ultrasonic sensor height from the base of the tank
#define TANK_DIAMETER 61.5      // Diameter of the tank
#define TANK_RADIUS TANK_DIAMETER / 2

void setup()
{

  Serial.begin(115200);
  // Configure GPIO pins here

  espInit();
}

void loop()
{

  // send updates with schema ‘{"id": "student_id", "type": "ultrasonic", "radar": 0, "waterheight": 0, "reserve": 0, "percentage": 0}’
  // Continuously measure the distance between the sensor and the water in the tank. This must be considered as the ‘radar’ measurement.

  // send updates with schema ‘{"id": "student_id", "type": "ultrasonic", "radar": 0, "waterheight": 0, "reserve": 0, "percentage": 0}’
  double radar = sonar.ping_in();
  double WaterHeight = getWaterHeight(radar);

  double reserve = getWaterReserve(WaterHeight);        // Calculate water reserve
  double percentage = (WaterHeight / MAX_WATER_HEIGHT) * 100; // Calculate percentage of water in the tank

  StaticJsonDocument<290> doc;
  char mssg[290] = {0};

  Serial.println(radar);

  doc["id"] = "620164974";
  doc["type"] = "ultrasonic";
  doc["radar"] = radar;
  doc["waterheight"] = WaterHeight;
  doc["reserve"] = reserve;
  doc["percentage"] = percentage;

  serializeJson(doc, mssg);

  // snprintf(mssg,sizeof(mssg),"{\"id\": \"%s\", \"type\": \"ultrasonic\", \"radar\": 0, \"waterheight\": %.2f, \"reserve\": %.2f, \"percentage\": %.2f}",idNumber,WaterHeight,reserve,percentage);

  // Serial.println(mssg);
  espUpdate(mssg);

  delay(1000);
}

void espSend(char command[])
{
  esp.print(command); // send the read character to the esp
  while (esp.available())
  {
    Serial.println(esp.readString());
  }
}

void espUpdate(char mssg[])
{
  char espCommandString[50] = {0};
  char post[290] = {0};

  snprintf(espCommandString, sizeof(espCommandString), "AT+CIPSTART=\"TCP\",\"%s\",%s\r\n", HOST_IP, HOST_PORT);
  espSend(espCommandString); // starts the connection to the server
  delay(stay);

  // GET REQUEST
  // snprintf(post,sizeof(post),"GET /%s HTTP/1.1\r\nHost: %s\r\nContent-Type: application/x-www-form-urlencoded\r\nContent-Length: %d\r\n\r\n%s\r\n\r\n",route,HOST_IP,strlen(mssg),mssg);

  // POST REQUEST
  snprintf(post, sizeof(post), "POST /%s HTTP/1.1\r\nHost: %s\r\nContent-Type: application/json\r\nContent-Length: %d\r\n\r\n%s\r\n\r\n", route, HOST_IP, strlen(mssg), mssg);

  snprintf(espCommandString, sizeof(espCommandString), "AT+CIPSEND=%d\r\n", strlen(post));
  espSend(espCommandString); // sends post length
  delay(stay);
  Serial.println(post);
  espSend(post); // sends POST request with the parameters
  delay(stay);
  espSend("AT+CIPCLOSE\r\n"); // closes server connection
}

void espInit()
{
  char connection[100] = {0};
  esp.begin(115200);
  Serial.println("Initiallizing");
  esp.println("AT");
  delay(1000);
  esp.println("AT+CWMODE=1");
  delay(1000);
  while (esp.available())
  {
    Serial.println(esp.readString());
  }

  snprintf(connection, sizeof(connection), "AT+CWJAP=\"%s\",\"%s\"\r\n", SSID, password);
  esp.print(connection);

  delay(3000); // gives ESP some time to get IP

  if (esp.available())
  {
    Serial.print(esp.readString());
  }

  Serial.println("\nFinish Initializing");
}

//***** Design and implement all util functions below ******
double getWaterHeight(double radar)
{
  // Calculate water height using the radar measurement
  double val = radar + 16.737;
  return SENSOR_HEIGHT - val;
}

double getWaterReserve(double height)
{
  // Calculate water reserve using the radar measurement
  // Convert height to gallons using the volume formula for a cylinder
  double volume = M_PI * TANK_RADIUS * TANK_RADIUS * height / 231.0; // 231 cubic inches in a gallon

  return volume;
}
