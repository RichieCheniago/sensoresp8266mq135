// The Libraries
#include <Firebase_ESP_Client.h>
#include <addons/TokenHelper.h>
#include <addons/RTDBHelper.h>

// The SSID and Password of your WiFi network
#define WIFI_SSID "testerwifi"
#define WIFI_PASSWORD "HOOOYEAHHH"
#define DATABASE_URL "https://esp32-mq135-9bdd0-default-rtdb.firebaseio.com/"
#define API_KEY "AIzaSyCF95rr2yPwHchwJJqMm8-qmoC_Np6_jZo"
#define MQ135_PIN A0
#define READ_INTERVAL 10000 // Read sensor every 10 seconds

unsigned long previousMillis = 0;
bool signupOK = false;
FirebaseAuth auth;
FirebaseConfig config;
FirebaseJson json;
FirebaseData fbdo;

// Initialize WiFi
void initWiFi()
{
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(1000);
  }
  
  // Print that WiFi is connected
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.print("RRSI: ");
  Serial.println(WiFi.RSSI());
}

// Initialize Firebase
void initFirebase()
{
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  Firebase.begin(&config, &auth);
  if (Firebase.signUp(&config, &auth, "", ""))
  {
    Serial.println("Firebase sign up successful");
    signupOK = true;
  }
  else
  {
    Serial.printf(config.signer.signupError.message.c_str());
    Firebase.reconnectWiFi(true);
  }
}

void setup()
{
  Serial.begin(9600);
  Serial.println("Starting the system");
  initWiFi();
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  Firebase.begin(&config, &auth);
}

void loop()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= READ_INTERVAL)
  {
    previousMillis = currentMillis;

    // Read analog value from MQ135 sensor
    int sensorValue = analogRead(MQ135_PIN);

    // Convert analog value to ppm (customize this conversion based on your sensor's calibration)
    float ppm = map(sensorValue, 0, 1023, 0, 1000); // Example conversion

    // Send data to Firebase
    json.set("airQuality", ppm);

    if (Firebase.RTDB.setJSON(&fbdo, "/Bedroom 2", &json) ? "ok" : fbdo.errorReason().c_str())
    {
      Serial.println("Data sent to Firebase");
    }
    else
    {
      Serial.println(fbdo.errorReason());
    }

    if (Firebase.signUp(&config, &auth, "", ""))
    {
      signupOK = true;
    }
    else
    {
      Serial.println(config.signer.signupError.message.c_str());
      Firebase.reconnectWiFi(true);
    }
  }
  delay(1000); // Wait 1 second between readings
}