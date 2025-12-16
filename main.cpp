/*
  RavKavProject - An ESP32-based RFID gate controller that interacts with MIFARE RFID cards.
  Created by Gal Argov Sofer, December 2025.
  Released under the MIT License.

  For use with WaveShare 3.5" TFT LCD and MFRC522 RFID reader.

  Before running the code, make sure to:
  1. Install the required libraries: MFRC522, WebServer, and WaveShareDemo.
  2. Update the WiFi credentials (SSID and PASSWORD) in the code.
  3. Update the RFID card UID in the code to match your card.
*/
#include <string>

#include <Arduino.h>
#include <SPI.h>
#include <WiFi.h>
#include <WebServer.h>
#include <MFRC522.h>

#include "WaveShareDemo.h"


#define RST_PIN         25          // Configurable, see typical pin layout above
#define SS_PIN          27          // Configurable, see typical pin layout above
#define LCD_CS          15          // Configurable, see typical pin layout above
#define LED_PIN         13          // On-board LED pin

#define BUDGET_BLOCK    14          // MIFARE Sector 3 Block 1 to store balance
#define ENTRY_COST      10          // Cost per entrance
#define FIRST_BALANCE   100         // Initial balance on the card


MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance
MFRC522::MIFARE_Key key;            // Create MIFARE key structure

WebServer server(80);               // Create a webserver object that listens for HTTP request on port 80

// WiFi network credentials - need to update manually
const char* ssid = "INPUT_YOUR_SSID_HERE";
const char* password = "INPUT_YOUR_PASSWORD_HERE";

// RFID card UID number - need to update manually
const String myUID = "INPUT_YOUR_CARD_UID_HERE";

int count = FIRST_BALANCE;

void handleRoot();
void readWriteCard();
void cardInteraction();
void cardInteractionOK();
void cardInteractionNoBalance();
void cardInteractionNOT();
void gateCloseLED();

void setup() {
	// Initialize serial communications with the PC
  Serial.begin(115200);
  
  // Internal LED setup - blink when gate close.
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Connect to WiFi network - please fill your wifi SSID and PASSWORD manually
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");

  // Initialize SPI bus
  while (!Serial);		    // Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
	SPI.begin();			      // Init SPI bus
  
  // Initialize MFRC522
	mfrc522.PCD_Init();		  // Init MFRC522
	delay(4);				        // Optional delay. Some board do need more time after init to be ready, see Readme
	mfrc522.PCD_DumpVersionToSerial();	// Show details of PCD - MFRC522 Card Reader details
	Serial.println(F("Scan PICC to see UID, SAK, type, and data blocks..."));

  // Initialize LCD
  Wvshr_Init();
  LCD_SCAN_DIR Lcd_ScanDir = SCAN_DIR_DFT;  
  LCD_Init(Lcd_ScanDir, 200);
  LCD_Clear(LCD_BACKGROUND);
  TP_Init();

  // Print initial screen
  GUI_DisString_EN(80, 80, "GAL KAV <3", &Font24, LCD_BACKGROUND, BLUE);
  GUI_DisString_EN(80, 120, "Balance:", &Font24, LCD_BACKGROUND, BLUE);
  GUI_DisNum(220, 120, count, &Font24, LCD_BACKGROUND, BLUE);
  SPI.endTransaction();

  // Initialize the MIFARE key structure
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }

  Serial.println("Setup completed.");
}

byte buffer[18] = {0};
byte len = 18;
MFRC522::StatusCode status;

void loop() {
  server.handleClient();
  if ( ! mfrc522.PICC_IsNewCardPresent())   return;
  if ( ! mfrc522.PICC_ReadCardSerial())     return; 
  // mfrc522.PICC_DumpToSerial(&(mfrc522.uid)); // Dump UID and SAK - good for testing
  cardInteraction();
}

// Transaction with card/tag (authentication, write, read and halt)
void readWriteCard() {
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, BUDGET_BLOCK, &key, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print("PCD_Authenticate() failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  
  if (count >= 0) {
    memcpy(buffer, &count, sizeof(count));
    status = mfrc522.MIFARE_Write(BUDGET_BLOCK, buffer, 16);
  }

  if (status != MFRC522::STATUS_OK) {
    Serial.print("MIFARE_Write() failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }
  
  status = mfrc522.MIFARE_Read(BUDGET_BLOCK, buffer, &len);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("MIFARE_Read() failed: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  memcpy(&count, buffer, sizeof(count));
  mfrc522.PICC_HaltA(); // Halt PICC
  mfrc522.PCD_StopCrypto1();  // Stop encryption on PCD

  // Print the budget block data in HEX and DEC formats
  Serial.print("Budget block data HEX: ");
  for (byte i = 0; i < 16; i++) {
    Serial.print(buffer[i],HEX);
    Serial.print(" ");
  }
  Serial.println();
  Serial.print("Budget block data DEC: ");
  for (byte i = 0; i < 16; i++) {
    Serial.print(buffer[i],DEC);
    Serial.print(" ");
  }
  Serial.println();
}

void cardInteraction() {
  String uid = "";
  Serial.print("UID: ");
  
  // Read the card UID and print it
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    uid += String(mfrc522.uid.uidByte[i], HEX);
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println();

  readWriteCard();

  // Update LCD display
  LCD_Clear(LCD_BACKGROUND);
  GUI_DisString_EN(80, 80, "GAL KAV:", &Font24, LCD_BACKGROUND, BLUE);
  if (uid == myUID && count > 0) {
    cardInteractionOK();            // Successful interaction
  } else if (uid == myUID && count <= 0) {
    cardInteractionNoBalance();     // Not enough balance
    gateCloseLED();                 // Close gate LED indication
  } else {
    cardInteractionNOT();           // Unknown UID
    gateCloseLED();                 // Close gate LED indication
  }
}

// Successful card interaction
void cardInteractionOK() {
  count = count - ENTRY_COST;
  Serial.print("Balance before deduct: ");
  Serial.println(count);  
  GUI_DisString_EN(80, 120, "Balance:", &Font24, LCD_BACKGROUND, BLUE);
  GUI_DisNum(220, 120, count, &Font24, LCD_BACKGROUND, BLUE);
  GUI_DisString_EN(80, 160, "Entrance OK :-)", &Font20, LCD_BACKGROUND, BLACK);
  // readWriteCard();
}

// Card interaction with no balance
void cardInteractionNoBalance() {
  GUI_DisString_EN(80, 120, "Balance < 0", &Font24, LCD_BACKGROUND, RED);
  GUI_DisString_EN(80, 160, "Entrance not OK :-(", &Font20, LCD_BACKGROUND, RED);
}

// Card interaction with unknown UID
void cardInteractionNOT() {
  GUI_DisString_EN(80, 120, "Unknown UID", &Font24, LCD_BACKGROUND, RED);
  GUI_DisString_EN(80, 160, "Entrance not OK :-(", &Font20, LCD_BACKGROUND, RED);
}

// Gate close LED indication
void gateCloseLED(){
  digitalWrite(LED_PIN, HIGH);
  delay(1000);
  digitalWrite(LED_PIN, LOW);
  delay(1000);
}

// Function to handle the root URL and show the current states
void handleRoot() {
  String buff = "";
  buff += String(count);
  String html = "<!DOCTYPE html><html><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  html += "<link rel=\"icon\" href=\"data:,\">";
  html += "<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;} </style></head>";
  html += "<body><h1>GAL KAV</h1>";
  html += "<p>Balance " + buff + "</p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}