#define BLYNK_TEMPLATE_ID "TMPL3xXaLaFM_"
#define BLYNK_TEMPLATE_NAME "Smart Irrigation System with ESP32"
#define BLYNK_AUTH_TOKEN "Cj_lLQOFsZbh9hvNgbTzW3eBk4w58Dtq"


#define BLYNK_PRINT Serial
#include <WiFi.h>
#include <BlynkSimpleEsp32.h> 

#define SOIL_MOISTURE_PIN 36   // Analog pin for soil moisture sensor
#define THRESHOLD_MOISTURE 30 // Adjust this value based on your soil moisture sensor readings
#define FLOW_PIN 34
#define PUMP_PIN 14  // Digital pin for controlling the pump
#define PUMP_SWITCH V6  // Virtual pin for controlling the pump manually

char auth[] = BLYNK_AUTH_TOKEN;  // Replace with your Blynk auth token
char ssid[] = "VM2CD698";   // Replace with your WiFi credentials
char pass[] = "wngq8dwhBBms";

BlynkTimer timer;
double volume = 0;
bool isPumpOn = false;  // Variable to track pump status
void sendSensorData()
{
  double soilMoisture = analogRead(SOIL_MOISTURE_PIN);
  //soilMoisture = float(analogRead(SOIL_MOISTURE_PIN)/4096) * 3.3;
  Serial.print("Soil Moisture ");
  Serial.println(soilMoisture);
  // Map the analog sensor values to a percentage (0-100) 
  double soilMoisturePercentage = ((soilMoisture-2656) * 100)/(-906);
  checkWaterflow(); //calculate value of waterflow
  Serial.print("Soil Moisture Percentage ");
  Serial.println(soilMoisturePercentage);
  // Send soil moisture data to Blynk
  Blynk.virtualWrite(V0, soilMoisturePercentage);
  Blynk.virtualWrite(V5, volume);

  // Check if the pump should be on based on manual switch or soil moisture level
  if (isPumpOn || soilMoisturePercentage < THRESHOLD_MOISTURE)
  {
    // Turn on the pump
    digitalWrite(PUMP_PIN, LOW);
    // Check if the pump is turned on automatically (not manually)
    if (!isPumpOn) {
      // Send alert notification to Blynk app if the pump is turned on automatically
      Blynk.logEvent("moisture_alert","Soil moisture is below the threshold!");
      Serial.println("Soil moisture is below the threshold!");
    }
  }
  else
  {
    // Turn off the pump only if it was not turned on manually
    if (!isPumpOn) {
      digitalWrite(PUMP_PIN, HIGH);
    }
  }
}

void checkWaterflow(){
  //Serial.println("Checking the waterflow now");
  volume = count * 2.6; // calibration factor
  if (millis() - lastTime > 1000){ // Convert from volume to volume/second in mililitres / Resets count after 1 second has elapsed
    count = 0;
    lastTime = millis();
  }
  Serial.print(volume); //print calculated volume rate per second
  Serial.println(" mL/s");
}

BLYNK_WRITE(PUMP_SWITCH)
{
  isPumpOn = param.asInt();
  if (isPumpOn) {
    Serial.println("Pump manually turned ON");
  } else {
    Serial.println("Pump manually turned OFF");
  }
}

void Flow(){ // count pulses from hall effect sensor
  count++;
}

void setup()
{
  Serial.begin(9600);
  pinMode(PUMP_PIN, OUTPUT); // Set pump pin as an output
  pinMode(FLOW_PIN, INPUT_PULLUP); // flow meter pin set to input pullup
  pinMode(SOIL_MOISTURE_PIN, INPUT); // soil moisture sensor pin for data input
  attachInterrupt(digitalPinToInterrupt(FLOW_PIN), Flow, RISING); // set flowmeter pin as interrupt pin
  Blynk.begin(auth, ssid, pass);

  timer.setInterval(3000L, sendSensorData); // Set the interval for checking soil moisture (every 3 seconds)

  // Setup switch widget
  Blynk.virtualWrite(PUMP_SWITCH, isPumpOn);
  Blynk.syncVirtual(PUMP_SWITCH);
}

void loop()
{
  Blynk.run();
  timer.run();
}