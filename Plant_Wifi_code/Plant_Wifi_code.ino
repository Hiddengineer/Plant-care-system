/*
 Plant Care Web Server with Auto/Manual Grow Light and Moisture Sensors
*/

#include <WiFi.h>
#include <TimeLib.h>

const char *ssid = "MySpectrumWiFi92-2G";
const char *password = "chillylion847";

const int waterPumpPin = 26;
const int growLightPin = 27;
const int moistureSensorPin1 = 32;
const int moistureSensorPin2 = 33;

bool growLightOn = false;
bool growLightAutoMode = true; // start with auto mode
int sunriseHour = 6;
int sunsetHour = 20;
int moistureThreshold = 500;

NetworkServer server(80);

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected. IP: " + WiFi.localIP().toString());

  pinMode(waterPumpPin, OUTPUT);
  pinMode(growLightPin, OUTPUT);
  pinMode(moistureSensorPin1, INPUT);
  pinMode(moistureSensorPin2, INPUT);

  server.begin();

  // Start with the light off
  digitalWrite(growLightPin, LOW);
}

void waterPlant() {
  Serial.println("Watering the plant!");
  digitalWrite(waterPumpPin, HIGH);
  delay(3000);
  digitalWrite(waterPumpPin, LOW);
}

void toggleGrowLight() {
  growLightOn = !growLightOn;
  digitalWrite(growLightPin, growLightOn ? HIGH : LOW);

  if (!growLightOn) {
    // If turned off manually, re-enable auto mode
    growLightAutoMode = true;
  } else {
    // If turned on manually, disable auto mode
    growLightAutoMode = false;
  }
}

void updateGrowLightBasedOnTime() {
  if (growLightAutoMode) {
    int currentHour = hour();
    if (currentHour >= sunriseHour && currentHour < sunsetHour) {
      digitalWrite(growLightPin, HIGH);
      growLightOn = true;
    } else {
      digitalWrite(growLightPin, LOW);
      growLightOn = false;
    }
  }
}

void handleUpdateTime(String line) {
  int sunriseIndex = line.indexOf("sunrise=");
  int sunsetIndex = line.indexOf("sunset=");
  if (sunriseIndex > 0 && sunsetIndex > 0) {
    int sunriseEnd = line.indexOf('&', sunriseIndex);
    int sunsetEnd = line.indexOf(' ', sunsetIndex);

    String sunriseStr = line.substring(sunriseIndex + 8, sunriseEnd);
    String sunsetStr = line.substring(sunsetIndex + 7, sunsetEnd);

    sunriseHour = sunriseStr.toInt();
    sunsetHour = sunsetStr.toInt();

    Serial.print("Updated sunrise to: ");
    Serial.println(sunriseHour);
    Serial.print("Updated sunset to: ");
    Serial.println(sunsetHour);
  }
}

String getMoistureStatus(int sensorPin) {
  int moisture = analogRead(sensorPin);
  if (moisture < moistureThreshold) {
    return "<span style='color:green'>Full</span>";
  } else {
    return "<span style='color:red'>Empty</span>";
  }
}

void loop() {
  NetworkClient client = server.accept();
  if (client) {
    Serial.println("New Client.");
    String currentLine = "";

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        if (c == '\n') {
          if (currentLine.length() == 0) {
            // Send webpage
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            client.println("<html><body>");
            client.println("<h2>Plant Care System</h2>");
            client.println("<a href=\"/Water\">Water the Plant</a><br>");
            client.println("<a href=\"/Light\">Toggle Grow Light</a><br>");
            client.println("<form action=\"/UpdateTime\" method=\"GET\">");
            client.println("Sunrise Hour: <input type=\"text\" name=\"sunrise\" value=\"" + String(sunriseHour) + "\"><br>");
            client.println("Sunset Hour: <input type=\"text\" name=\"sunset\" value=\"" + String(sunsetHour) + "\"><br>");
            client.println("<input type=\"submit\" value=\"Update Time\"></form>");

            // Moisture Sensor Status
            client.println("<p>Moisture Sensor 1: " + getMoistureStatus(moistureSensorPin1) + "</p>");
            client.println("<p>Moisture Sensor 2: " + getMoistureStatus(moistureSensorPin2) + "</p>");

            client.println("</body></html>");
            client.println();

            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }

        // Handle requests
        if (currentLine.endsWith("GET /Water")) {
          waterPlant();
        }

        if (currentLine.endsWith("GET /Light")) {
          toggleGrowLight();
        }

        if (currentLine.startsWith("GET /UpdateTime")) {
          handleUpdateTime(currentLine);
        }
      }
    }

    // Check sensors for auto-watering
    int moisture1 = analogRead(moistureSensorPin1);
    if (moisture1 < moistureThreshold) {
      Serial.println("Soil is dry! Auto-watering...");
      waterPlant();
    }

    updateGrowLightBasedOnTime();

    client.stop();
    Serial.println("Client Disconnected.");
  }
}
