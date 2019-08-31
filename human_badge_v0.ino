#include <SPI.h>
#include <RH_NRF24.h>

// Singleton instance of the radio driver
RH_NRF24 nrf24;

// do you want this to be sending a pulse on every loop?
#define SEND_MODE true

// do you want this to be trying to receive pulses on every loop?
#define RECEIVE_MODE true

// this'll hold this instance's random identifier, generated at power on
String my_id;

// these are for when you're flashing individual LEDs, NOT the RGB one
#define RED_LED_PIN 7
#define GREEN_LED_PIN 6

// uncomment this line if using a Common Anode RGB LED (meaning the longest pin is positive, not negative)
// #define COMMON_ANODE

// these define the digital/PWM pins for the RGB LED
int redPin = 6;
int greenPin = 5;
int bluePin = 3;

const int piezoPin = 8;

void setup() {
  Serial.begin(9600);
  //pinMode(LED_BUILTIN, OUTPUT);
  //pinMode(RED_LED_PIN, OUTPUT);
  //pinMode(GREEN_LED_PIN, OUTPUT);

  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT); 

  pinMode(piezoPin, OUTPUT);

  if (!nrf24.init()) {
    Serial.println("init failed");
  }
  
  // Defaults after init are 2.402 GHz (channel 2), 2Mbps, 0dBm
  if (!nrf24.setChannel(7)) {
    Serial.println("setChannel failed");
  }
  
  if (!nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm)) {
    Serial.println("setRF failed");
  }

  randomSeed(analogRead(0)); // some noise
  my_id = String(random(1, 1000));
}

void loop() {
  Serial.println("START OF NEW LOOP!");
  Serial.print("My unique ID is: ");
  Serial.println(my_id);
  
  // flash the LED at the start of the loop
  // digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
  // delay(500);
  // digitalWrite(LED_BUILTIN, LOW); // turn the LED off by making the voltage LOW
  // delay(500);

  // code for sending a message
  if (SEND_MODE) {
    Serial.print("Sending a message: ");
    setColor(0, 255, 0); // green!
    // uint8_t data[] = "Hello World! I am #";
    String data = "Hello World! I am #";
    data.concat(my_id);
    Serial.println(data);
    uint8_t raw_data[data.length() + 1];
    data.getBytes(raw_data, sizeof(raw_data));
    if (sizeof(raw_data) > RH_NRF24_MAX_MESSAGE_LEN) {
      Serial.println("thing being sent is too large!!!");
    }
    
    nrf24.send(raw_data, sizeof(raw_data)); // send the message!
    nrf24.waitPacketSent(); // blocks execution until transmitter is done transmitting
    // flashLedPin(GREEN_LED_PIN);
    Serial.println("Message sent!");
    tone(piezoPin, 880, 200);
    delay(400);
    setColor(0, 0, 0);
  }
  
  // code for receiving a message
  if (RECEIVE_MODE) {
    // this waits X milliseconds or until a message is received, whatever is first
    // using nrf24.available() waits indefinitely for a message
    long seconds = random(5000, 15000);
    Serial.print("Seconds to wait for the next message: ");
    Serial.println(seconds);
    // digitalWrite(LED_BUILTIN, HIGH);
    setColor(80, 0, 80); // purple!
    if (nrf24.waitAvailableTimeout(seconds)) { 
      // if the above code block passed, then we have a message
      uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN]; // allocate the max length to start
      uint8_t len = sizeof(buf);
      if (nrf24.recv(buf, &len)) {
        Serial.print("!!!! Received a message: ");
        Serial.println((char*) buf);
        // flashLedPin(RED_LED_PIN);
        setColor(255, 0, 0); // red!
        tone(piezoPin, 1047, 200);
        delay(200);
        tone(piezoPin, 1319, 200);
        delay(200);
        tone(piezoPin, 1568, 200);
        delay(500);
      } else {
        Serial.println("receive failed for some reason");
      }
    } else {
      // this happens on the timeout of not getting a new message
      Serial.println("No message, is another radio running nearby?");
    }
    // digitalWrite(LED_BUILTIN, LOW);
    setColor(0, 0, 0);
  }

  Serial.println("END OF LOOP!");
}

void flashLedPin(int pin) {
  digitalWrite(pin, HIGH);
  delay(400);
  digitalWrite(pin, LOW);
}

void setColor(int red, int green, int blue) {
  #ifdef COMMON_ANODE
    red = 255 - red;
    green = 255 - green;
    blue = 255 - blue;
  #endif
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);  
}
