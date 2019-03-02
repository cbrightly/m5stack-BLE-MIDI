// M5STACK(ESP32)向け BLE MIDI
// Programed by Kazuyuki Eguchi 2018/04/24
// Initially posted to:
//  https://qiita.com/KazuyukiEguchi/items/e166ede5c97438b90187

#include <M5Stack.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

#define MIDI_SERVICE_UUID        "03b80e5a-ede8-4b33-a751-6ce34ec4c700"
#define MIDI_CHARACTERISTIC_UUID "7772e5db-3868-4112-a1a9-f2669d106bf3"
#define DEVIVE_NAME "M5"

BLEServer *pServer;
BLEAdvertising *pAdvertising;
BLECharacteristic *pCharacteristic;
int pos = 0;
char midi[5];

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      M5.Lcd.fillScreen(BLACK);
      M5.Lcd.setTextSize(1);
      M5.Lcd.setCursor(10,0);
      M5.Lcd.printf("BLE MIDI Connected.");
    };

    void onDisconnect(BLEServer* pServer) {
      M5.Lcd.fillScreen(BLACK);
      M5.Lcd.setTextSize(1);
      M5.Lcd.setCursor(10,0);
      M5.Lcd.printf("BLE MIDI Disconnect.");
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();
      pos = 0;

      if (rxValue.length() > 0) {
        for (int i = 0; i < rxValue.length(); i++)
        {
          Serial.printf("%02x",rxValue[i]);
          midi[pos] = rxValue[i];
          pos++;
          if(pos == 5)
          {
            M5.Lcd.fillScreen(BLACK);
            M5.Lcd.setTextSize(3);
            M5.Lcd.setCursor(10,0);
            M5.Lcd.printf("%02x %02x %02x",midi[2],midi[3],midi[4]);
            pos = 0;
          }          
        }
        Serial.println();
      }
    }
};

void setup() {
  M5.begin();
  Serial.begin(115200);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextColor(WHITE);
  M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(10,0);
  M5.Lcd.printf("BLE MIDI Disconnect.");

  BLEDevice::init(DEVIVE_NAME);
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService *pService = pServer->createService(BLEUUID(MIDI_SERVICE_UUID));
  pCharacteristic = pService->createCharacteristic(
    BLEUUID(MIDI_CHARACTERISTIC_UUID),
    BLECharacteristic::PROPERTY_READ   |
    BLECharacteristic::PROPERTY_WRITE  |
    BLECharacteristic::PROPERTY_NOTIFY |
    BLECharacteristic::PROPERTY_WRITE_NR
  );
  pCharacteristic->setCallbacks(new MyCallbacks());
  pService->start();

  BLEAdvertisementData oAdvertisementData = BLEAdvertisementData();
  oAdvertisementData.setFlags(0x04);
  oAdvertisementData.setCompleteServices(BLEUUID(MIDI_SERVICE_UUID));
  oAdvertisementData.setName(DEVIVE_NAME);
  pAdvertising = pServer->getAdvertising();
  pAdvertising->setAdvertisementData(oAdvertisementData);
  pAdvertising->start();
}

void loop() {
  delay(10);
}


