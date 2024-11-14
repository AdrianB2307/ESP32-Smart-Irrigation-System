#define LED_BUILTIN 2 //don't use pins 6-11
#define ANA_PIN     36 //soil moisture sensor pin
#define FLOW_PIN    34 //flowmeter pin
#define RELAY_PIN   14 //relay pin

// Flow meter variables
volatile int count;
float volume;
unsigned long lastTime;
// Soil moisture sensor variables
float analogInput; //sensor
double moistureValue;
double potentialDifference;
// Relay & Pump variables
unsigned long relayStartTime;
unsigned long pumpOnTime;

// the setup function runs once when you press reset or power the board
void setup() 
{
  //pinMode(LED_BUILTIN, OUTPUT);
  pinMode(ANA_PIN, INPUT); // soil moisture sensor pin for data input
  pinMode(RELAY_PIN, OUTPUT); // relay pin for data output to the relay
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(FLOW_PIN, INPUT_PULLUP); // flow meter pin set to input pullup

  // Set flow meter pin to interrupt pin
  attachInterrupt(digitalPinToInterrupt(FLOW_PIN), Flow, RISING); // set flowmeter pin as interrupt pin
  //attatchInterrupt(FLOW_PIN, Flow, RISING);

  // Flow meter
  count = 0;
  volume = 0;
  lastTime = 0;
  // Soil moisture sensor
  analogInput = 0.0f;
  moistureValue = 0;
  potentialDifference = -0.73;
  //Pump & Relay
  relayStartTime = 0;
  pumpOnTime = 0;
  Serial.begin(9600);
}

// the loop function runs over and over again forever
void loop() 
{
  digitalWrite(RELAY_PIN, HIGH);
  int command = 0;
  while(true){
    if(Serial.available() > 0){
      command = Serial.parseInt();

      if(command == 1){
        while(true){
          checkSoilMoisture();

          if(moistureValue <= 60){
            digitalWrite(LED_BUILTIN, HIGH);
            switchOnRelay();
          }
          else{
            digitalWrite(LED_BUILTIN, LOW);
            switchOffRelay();
          }

          checkWaterflow();
          delay(1000);

          if (Serial.available() > 0){
            command = Serial.parseInt();

            if(command == 2){
              break;
            }
          }
        }
      }
    }
  }
}

void checkSoilMoisture(){
  analogInput = float(analogRead(ANA_PIN)/4096.0) * 3.3; //convert 12 bit resolution digital value to Voltage
  moistureValue = ((analogInput-2.14) * 100)/(potentialDifference); // Linear interpolation calibration
  Serial.println(moistureValue);
}

void getVolume(){ // Get total volume of water flowed through sensor
  double calibrationFactor = 2.6; // independant variable
  volume = count * calibrationFactor; // volume calculation
  Serial.print(volume);
  Serial.print(" mL");
}

void checkWaterflow(){
  //Serial.println("Checking the waterflow now");
  volume = count * 2.6; // calibration factor
  if (millis() - lastTime > 1000){ // Convert from volume to volume/second in mililitres / Resets count after 1 second has elapsed
    count = 0;
    lastTime = millis();
  }
  Serial.print(volume);
  Serial.println(" mL/s");
}

void Flow(){ // count pulses from hall effect sensor
  count++;
}

// Function to switch on the relay and start the pump
void switchOnRelay() { 
  // Check if the relay is already on
  if (digitalRead(RELAY_PIN) == HIGH) {
    // Switch on the relay
    delay(5000); //5 Second delay before pump starts
    digitalWrite(RELAY_PIN, LOW);
    // Store the current time
    relayStartTime = millis();
  }
}

// Function to switch off the relay and stop the pump
void switchOffRelay() {
  // Check if the relay is already off
  if (digitalRead(RELAY_PIN) == LOW) {
    // Switch off the relay
    digitalWrite(RELAY_PIN, HIGH);
    // Calculate the pump on time
    pumpOnTime += millis() - relayStartTime;
    Serial.println(pumpOnTime);
  }
}

