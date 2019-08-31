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

// uncomment this line if using a Common Anode RGB LED (meaning the longest pin is positive, not negative)
// #define COMMON_ANODE

// these define the digital/PWM pins for the RGB LED
const int redPin = 6;
const int greenPin = 5;
const int bluePin = 3;

// where our little piezo speaker is
const int piezoPin = 8;

void setup() {
  Serial.begin(9600);

  // get our pins in order
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT); 
  pinMode(piezoPin, OUTPUT);

  if (!nrf24.init()) {
    Serial.println("init failed");
  }
  
  // Defaults after init are 2.4 GHz (channel 7), 2Mbps, 0dBm
  if (!nrf24.setChannel(7)) {
    Serial.println("setChannel failed");
  }
  
  if (!nrf24.setRF(RH_NRF24::DataRate2Mbps, RH_NRF24::TransmitPower0dBm)) {
    Serial.println("setRF failed");
  }

  randomSeed(analogRead(0)); // generate some noise as a random seed

  // generate a "unique"/random ID for this session
  my_id = String(random(1, 1000));
  Serial.print("My unique ID is: ");
  Serial.println(my_id);
}

void loop() {
  Serial.println("START OF NEW LOOP!");

  // code for sending a message
  if (SEND_MODE) {
    Serial.print("Sending a message: ");
    setColor(0, 255, 0); // green light!
    
    // uint8_t data[] = "Hello World!"; // original test message

    // gotta do a ton of type juggling to compose this message
    String data = "Hello World! I am #";
    data.concat(my_id);
    Serial.println(data);
    uint8_t raw_data[data.length() + 1];
    data.getBytes(raw_data, sizeof(raw_data));

    // warn if the message is too big for anything to even receive
    if (sizeof(raw_data) > RH_NRF24_MAX_MESSAGE_LEN) {
      Serial.println("thing being sent is too large!!!");
    }
    
    nrf24.send(raw_data, sizeof(raw_data)); // send the message!
    nrf24.waitPacketSent(); // blocks execution until transmitter is done transmitting
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
    setColor(80, 0, 80); // purple!
    if (nrf24.waitAvailableTimeout(seconds)) { 
      // if the above code block passed, then we have a message
      uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN]; // allocate the max length to start
      uint8_t len = sizeof(buf);
      if (nrf24.recv(buf, &len)) {
        Serial.print("!!!! Received a message: ");
        Serial.println((char*) buf);
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
    
    setColor(0, 0, 0);
  }

  Serial.println("END OF LOOP!");
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
