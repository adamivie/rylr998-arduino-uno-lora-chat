/*
 * RYLR998 LoRa - Arduino Uno Sender/Receiver
 * Device Address: 2
 * 
 * Connections:
 * RYLR998 TX -> Arduino Pin 2 (RX via SoftwareSerial)
 * RYLR998 RX -> Arduino Pin 3 (TX via SoftwareSerial)
 * RYLR998 VCC -> 5V
 * RYLR998 GND -> GND
 */

#include <SoftwareSerial.h>

// SoftwareSerial for RYLR998 (RX, TX)
SoftwareSerial loraSerial(2, 3);

const int MY_ADDRESS = 2;        // This device's address
const int TARGET_ADDRESS = 1;    // Target device address
const int NETWORK_ID = 6;        // Network ID (must match on both devices)

String inputBuffer = "";
bool messageComplete = false;

void setup() {
  Serial.begin(9600);      // USB Serial for debugging
  
  // Try common baud rates for RYLR998
  Serial.println("=== RYLR998 Arduino Uno - Device 2 ===");
  Serial.println("Detecting LoRa module baud rate...");
  
  int baudRates[] = {115200, 57600, 9600, 19200, 38400};
  bool detected = false;
  
  for (int i = 0; i < 5; i++) {
    loraSerial.begin(baudRates[i]);
    delay(100);
    loraSerial.println("AT");
    delay(200);
    
    if (loraSerial.available()) {
      String response = loraSerial.readString();
      if (response.indexOf("OK") >= 0 || response.indexOf("+OK") >= 0) {
        Serial.print("Module detected at ");
        Serial.print(baudRates[i]);
        Serial.println(" baud");
        detected = true;
        break;
      }
    }
  }
  
  if (!detected) {
    Serial.println("Warning: Module not responding, using 115200 baud");
    loraSerial.begin(115200);
  }
  
  delay(500);
  Serial.println("Initializing LoRa module...");
  
  // Configure the module
  configureModule();
  
  Serial.println("\nReady to send/receive messages!");
  Serial.println("Type your message and press Enter to send.");
  Serial.println("----------------------------------------");
}

void loop() {
  // Check for incoming LoRa messages
  if (loraSerial.available()) {
    receiveLoRaMessage();
  }
  
  // Check for user input to send
  if (Serial.available()) {
    char inChar = (char)Serial.read();
    if (inChar == '\n') {
      if (inputBuffer.length() > 0) {
        sendMessage(inputBuffer);
        inputBuffer = "";
      }
    } else if (inChar != '\r') {
      inputBuffer += inChar;
    }
  }
}

void configureModule() {
  // Clear any pending data
  while (loraSerial.available()) {
    loraSerial.read();
  }
  
  // Test AT command
  loraSerial.println("AT");
  delay(200);
  Serial.print("AT Test: ");
  printLoRaResponse();
  
  // Reset to factory settings (optional, comment out if not needed)
  // loraSerial.println("AT+RESET");
  // delay(1000);
  // Serial.print("Reset: ");
  // printLoRaResponse();
  
  // Set address
  String cmd = "AT+ADDRESS=" + String(MY_ADDRESS);
  loraSerial.println(cmd);
  delay(300);
  Serial.print("Set address to ");
  Serial.print(MY_ADDRESS);
  Serial.print(": ");
  printLoRaResponse();
  
  // Set network ID
  cmd = "AT+NETWORKID=" + String(NETWORK_ID);
  loraSerial.println(cmd);
  delay(300);
  Serial.print("Set network ID to ");
  Serial.print(NETWORK_ID);
  Serial.print(": ");
  printLoRaResponse();
  
  // Set parameters to match both devices
  // Format: AT+PARAMETER=<SpreadingFactor>,<Bandwidth>,<CodingRate>,<Preamble>
  // Using: SF=9, BW=7(125kHz), CR=1(4/5), Preamble=12
  loraSerial.println("AT+PARAMETER=9,7,1,12");
  delay(300);
  Serial.print("Set parameters: ");
  printLoRaResponse();
  
  // Get module parameters
  loraSerial.println("AT+PARAMETER?");
  delay(300);
  Serial.print("Current parameters: ");
  printLoRaResponse();
  
  // Verify address
  loraSerial.println("AT+ADDRESS?");
  delay(300);
  Serial.print("Current address: ");
  printLoRaResponse();
}

void sendMessage(String message) {
  // Format: AT+SEND=<address>,<length>,<data>
  String cmd = "AT+SEND=" + String(TARGET_ADDRESS) + "," + String(message.length()) + "," + message;
  loraSerial.println(cmd);
  
  Serial.print("[SENT to Device ");
  Serial.print(TARGET_ADDRESS);
  Serial.print("] ");
  Serial.println(message);
  
  delay(100);
  printLoRaResponse();
}

void receiveLoRaMessage() {
  String response = loraSerial.readStringUntil('\n');
  response.trim();
  
  // Parse incoming message format: +RCV=<address>,<length>,<data>,<RSSI>,<SNR>
  if (response.startsWith("+RCV=")) {
    int firstComma = response.indexOf(',');
    int secondComma = response.indexOf(',', firstComma + 1);
    int thirdComma = response.indexOf(',', secondComma + 1);
    int fourthComma = response.indexOf(',', thirdComma + 1);
    
    if (firstComma > 0 && secondComma > 0 && thirdComma > 0) {
      String senderAddr = response.substring(5, firstComma);
      String msgLength = response.substring(firstComma + 1, secondComma);
      String message = response.substring(secondComma + 1, thirdComma);
      String rssi = (fourthComma > 0) ? response.substring(thirdComma + 1, fourthComma) : "";
      String snr = (fourthComma > 0) ? response.substring(fourthComma + 1) : "";
      
      Serial.print("[RECEIVED from Device ");
      Serial.print(senderAddr);
      Serial.print("] ");
      Serial.print(message);
      if (rssi.length() > 0) {
        Serial.print(" (RSSI: ");
        Serial.print(rssi);
        Serial.print(" dBm, SNR: ");
        Serial.print(snr);
        Serial.print(" dB)");
      }
      Serial.println();
    }
  } else if (response.length() > 0) {
    Serial.print("[LoRa] ");
    Serial.println(response);
  }
}

void printLoRaResponse() {
  delay(100);
  if (loraSerial.available()) {
    String response = "";
    while (loraSerial.available()) {
      char c = loraSerial.read();
      if (c != '\r' && c != '\n') {
        response += c;
      }
      delay(2);
    }
    if (response.length() > 0) {
      Serial.println(response);
    } else {
      Serial.println("(no response)");
    }
  } else {
    Serial.println("(no response)");
  }
}
