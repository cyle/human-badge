#include <SPI.h>
#include <RH_NRF24.h>
#include <Adafruit_GFX.h> // Core display graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789

// some colors to use for the screen are here:
// https://github.com/adafruit/Adafruit-ST7735-Library/blob/master/Adafruit_ST77xx.h#L77-L86

// Singleton instance of the radio driver
RH_NRF24 nrf24;

// do you want this to be sending a pulse on every loop?
#define SEND_MODE true

// do you want this to be trying to receive pulses on every loop?
#define RECEIVE_MODE true

// the maximum receive wait timeout, in milliseconds, note the .0 to make it a float
#define MAX_RECEIVE_WAIT_TIME 15000.0

// this'll hold this instance's random identifier, generated at power on
String my_id;

// these define the digital/PWM pins for the RGB LED
const int redPin = 6;
const int greenPin = 5;
const int bluePin = 3;

// where our little piezo speaker is
const int piezoPin = 9;

// where our 10kohm potentiometer is, expects to be on 5V
const int potPin = A5;

// this pin reads the state of the switch
const int switchPin = 4;

/**
 * Some notes about SPI since this has two SPI devices...
 * pin 8 on nano goes to CE on radio
 * pin 10 on nano goes to CSN on radio
 * no IRQ pin is needed for radio, leave it off, nevermind this
 * SPI MOSI pin on nano is 11, shared by radio + display
 * SPI MISO pin on nano is 12, shared by radio + display
 * SPI clock (SCK) pin on nano is 13, shared by radio + display
 * SPI Slave Select for radio is pin 10; radio has other pins to consider as well
 * SPI Slave Select for display is pin A0 (digital pin 14); display has other pins to consider as well
 */
#define TFT_CS 14
#define TFT_RST 15
#define TFT_DC 16

// comment this next line out if you don't actually have a display connected
#define HAS_DISPLAY

#ifdef HAS_DISPLAY
// create our display singleton
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
#endif

/**
 * It's the setup function, where we set things up on power on.
 */
void setup() {
  Serial.begin(9600);

  // get our LED RGB pins in order
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT); 

  // get our piezo speaker pin in order
  pinMode(piezoPin, OUTPUT);

  // get our switch pin in order
  pinMode(switchPin, INPUT);

  #ifdef HAS_DISPLAY
  // initialize the ST7789 display, 240x240
  tft.init(240, 240);
  drawText("INIT\nTIME\nLOL\nHELLO", ST77XX_WHITE);
  #endif

  // initialize the radio
  if (!nrf24.init()) {
    Serial.println("radio init failed");
  }
  
  // Defaults after init are 2.4 GHz (channel 1), 2Mbps, 0dBm
  if (!nrf24.setChannel(76)) {
    Serial.println("radio setChannel failed");
  }
  
  if (!nrf24.setRF(RH_NRF24::DataRate250kbps, RH_NRF24::TransmitPower0dBm)) {
    Serial.println("radio setRF failed");
  }

  // generate some noise as a random seed
  randomSeed(analogRead(0));

  // generate a "unique"/random ID for this session
  my_id = String(random(1, 1000));
  Serial.print("My unique ID is: ");
  Serial.println(my_id);
}

/**
 * It's the main loop, where we do things over and over again while the badge is on.
 */
void loop() {
  Serial.println("START OF NEW LOOP!");

  int switchValue = digitalRead(switchPin);
  Serial.print("switch state: ");
  Serial.println(switchValue);

  // check the state of the switch
  if (switchValue == 1) {
    terminalOperations();
  } else {
    sendReceiveOperations();
  }

  Serial.println("END OF LOOP!");
}

/**
 * The "terminal" operations when in this mode.
 * Expects you to have a serial connection to the device so you can interact.
 */
void terminalOperations() {
  Serial.println("we are in TERMINAL MODE!!!!");
  drawText(" xxx \nTERMINAL\nMODE\nACTIVE\n xxx ", ST77XX_BLUE);
  setColor(0, 0, 255); // blue light!
  delay(1000);
}

/**
 * Normal send/receive of pulses with other devices.
 * Just needs power running to it in this mode.
 */
void sendReceiveOperations() {
  // code for sending a message
  if (SEND_MODE) {
    drawText("Sending\npulse...", ST77XX_GREEN);
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
  } else {
    Serial.println("Not in send mode, so not sending.");
  }

  // read the value of the potentiometer, which will control the receive timeout
  int potValue = analogRead(potPin);
  Serial.print("got knob value: ");
  Serial.println(potValue); // if a 10kohm pot is given 5v, this should be between 0 and 1024

  // code for receiving a message, only use if RECEIVE_MODE is true,
  // and if the knob has been turned up enough to be receiving things
  if (RECEIVE_MODE && potValue > 100) {
    // this waits X milliseconds or until a message is received, whatever is first
    // using nrf24.available() waits indefinitely for a message
    long milliseconds = round((potValue / 1024.0) * MAX_RECEIVE_WAIT_TIME); // note the .0s to convert to floats
    // long milliseconds = random(5000, 15000);
    drawText("Listening for pulses...", ST77XX_MAGENTA);
    Serial.print("Milliseconds to wait for the next message: ");
    Serial.println(milliseconds);
    setColor(80, 0, 80); // purple!
    if (nrf24.waitAvailableTimeout(milliseconds)) { 
      // if the above code block passed, then we have a message
      uint8_t buf[RH_NRF24_MAX_MESSAGE_LEN]; // allocate the max length to start
      uint8_t len = sizeof(buf);
      if (nrf24.recv(buf, &len)) {
        drawText("Got a \nmessage!", ST77XX_RED);
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
  } else {
    Serial.println("Not in receive mode, so not receiving.");
  }
}

/**
 * Helper function to set the color of the RGB LED.
 */
void setColor(int red, int green, int blue) {
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);  
}

/**
 * Helper function to draw some text on the display.
 */
void drawText(char *text, uint16_t color) {
  #ifdef HAS_DISPLAY
  tft.fillScreen(ST77XX_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(color);
  tft.setTextSize(4);
  tft.setTextWrap(true);
  tft.print(text);
  #endif
}
