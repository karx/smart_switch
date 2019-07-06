#include <Arduino.h>
#include "painlessMesh.h"
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include "EEPROM.h"
#include <WiFiClient.h>
#include <PubSubClient.h>
#include<Wire.h>
#include "RTClib.h"
#include "esp_system.h"


RTC_DS1307 rtc;
char bafi[20];

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

BLEServer *pServer = NULL;
BLECharacteristic * pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
uint8_t txValue = 0;
uint32_t  ID;
bool klag = false;
bool mqttFlag = false;

void mqttCallback(char* topic, byte* payload, unsigned int length);


#define SERVICE_UUID         "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID_RX "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define CHARACTERISTIC_UUID_TX "e3327c31-3123-4a99-bf27-b900c24c4e68"


// #define   MESH_PREFIX     "whateverYouLike"
#define   MESH_PASSWORD   ""
#define   MESH_PORT       5555

#define   STATION_SSID     "ESP_1"
#define   STATION_PASSWORD "123456789q"

#define HOSTNAME "MQTT_Bridge"

const uint8_t HIDDEN = 1;
const uint8_t CHANNEL = 6;

IPAddress getlocalIP();

IPAddress myIP(0,0,0,0);
const char* mqtt_server ="api.akriya.co.in";
IPAddress mqttBroker(192, 168, 1, 1);

WiFiClient wifiClient;
PubSubClient mqttClient(mqtt_server, 1883, mqttCallback, wifiClient);

Scheduler userScheduler; // to control your personal task
painlessMesh  mesh;
int LED_BUILTI = 2;
String data = "";

#define EEPROM_SIZE 60
boolean mesh1 = false;
String BID = "";
int ledState;
uint32_t DeviceId;
String meshSsid="";
int address = 0; 
char MSSID[20];
String StatusBT= "";
bool flag = false;
bool ota = false;
char UserSSID[12];
char UserPWD[12];
String UserPWD1 = "";
String UserSSID1 = "";
bool flag1 = false;
String D1;
String msg;
int IN1 = 19;    // select the INPUT pin for the BOARD
int IN2 = 18;
int IN3 = 5;
int IN4 = 17; 
int IN5 = 16;
int IN6 = 4;
int IN7 = 0;
int IN8 = 2;

int OUT1 = 12;    // select the OUTPUT pin for the BOARD
int OUT2 = 27;
int OUT3 = 26;
int OUT4 = 25; 
int OUT5 = 33;
int OUT6 = 32;
int OUT7 = 13;
int OUT8 = 15;
boolean LED_state[8] = {0};
int reset_pin = 23;

void ledToggle(int o, int c){

  switch (o)
  {
   case 1:
   if (c == 1){
     LED_state[0] = 1;
    digitalWrite(OUT1, HIGH);
   }
   else{
     LED_state[0] = 0;
    digitalWrite(OUT1, LOW);
   }
    Serial.printf("Light toggled 1 and state = %i", c);
    Serial.println();
    break;
  case 2:
     if (c == 1){
             LED_state[1] = 1;

    digitalWrite(OUT2, HIGH);
   }
   else{
           LED_state[1] = 0;

    digitalWrite(OUT2, LOW);
   }
    Serial.printf("Light toggled 2 and state = %i", c);
    Serial.println();
    break;
  case 3:
     if (c == 1){
             LED_state[2] = 1;
    digitalWrite(OUT3, HIGH);
   }
   else{
           LED_state[2] = 0;
    digitalWrite(OUT3, LOW);
   }
    Serial.printf("Light toggled 3 and state = %i", c);
    Serial.println();
    break;
  case 4:
     if (c == 1){
             LED_state[3] = 1;
    digitalWrite(OUT4, HIGH);
   }
   else{     
      LED_state[3] = 0;
    digitalWrite(OUT4, LOW);
   }
    Serial.printf("Light toggled 4 and state = %i", c);
    Serial.println();
    break;
  case 5:
       if (c == 1){
               LED_state[4] = 1;
    digitalWrite(OUT5, HIGH);
   }
   else{
           LED_state[4] = 0;
    digitalWrite(OUT5, LOW);
   }
    Serial.printf("Light toggled 5 and state = %i", c);
    Serial.println();
    break;
  case 6:
     if (c == 1){
             LED_state[5] = 1;
    digitalWrite(OUT6, HIGH);
   }
   else{
           LED_state[5] = 0;
    digitalWrite(OUT6, LOW);
   }
    Serial.printf("Light toggled 6 and state = %i", c);
    Serial.println();
    break;
  case 7:
     if (c == 1){
             LED_state[6] = 1;
    digitalWrite(OUT7, HIGH);
   }
   else{
           LED_state[6] = 0;
    digitalWrite(OUT7, LOW);
   }
    Serial.printf("Light toggled 7 and state = %i", c);
    Serial.println();
    break;
  case 8:
     if (c == 1){
             LED_state[7] = 1;
    digitalWrite(OUT8, HIGH);
   }
   else{
           LED_state[7] = 0;
    digitalWrite(OUT8, LOW);
   }
    Serial.printf("Light toggled 8 and state = %i", c);
    Serial.println();
    break;
  
  default:
    break;
  }

        StatusBT = String(DeviceId) + "/" + (String)LED_state[0]+(String)LED_state[1]+(String)LED_state[2]+(String)LED_state[3]+(String)LED_state[4]+(String)LED_state[5]+(String)LED_state[6]+(String)LED_state[7];
        Serial.print("for BT = ");
        Serial.print(StatusBT);
        Serial.println();
        flag = true;

}



void checkbhai(uint32_t fro , String ms){

  int slashIndex1 = ms.indexOf('/');
        //  Search for the next comma just after the first
        int secondSlashIndex1 = ms.indexOf('/', slashIndex1 + 1);
        int thirdSlashIndex1 = ms.indexOf('/',secondSlashIndex1 + 1);
  
  if(ms.startsWith("S")){
      String BID2 = ms.substring(slashIndex1+1,secondSlashIndex1);
      if(BID2 == String (DeviceId)){

       StatusBT = ms.substring(secondSlashIndex1+1);
        Serial.print("for BT = ");
        Serial.print(StatusBT);
        Serial.println();
        flag = true;


      }
  }
  
  else if (ms.equals("status")){


      String idd = String(DeviceId);
    // if (msg.equals(idd)){



        String statusD = "S/" + String(fro) + "/" + idd + "/"+LED_state[0]+LED_state[1]+LED_state[2]+LED_state[3]+LED_state[4]+LED_state[5]+LED_state[6]+LED_state[7];
        uint32_t EID = fro;
        

      mesh.sendBroadcast(statusD);
      
    // }
}

    else if (ms.endsWith("1") || ms.endsWith("0")){
        

        String meshID1 = ms.substring(0,slashIndex1);
        Serial.print("MID = ");
        Serial.print(meshID1);
        Serial.println();

        String BID1 = "";
        BID1 = ms.substring(slashIndex1+1,secondSlashIndex1);
        Serial.print("BID1 = ");
        Serial.print(BID1);
        Serial.println();

        String ledState1 = ms.substring( thirdSlashIndex1 + 1 );
        int ledState2 = ledState1.toInt();
        Serial.print("*callback Led State = ");
        Serial.print(ledState2);
        Serial.println();

        String ledNum = ms.substring(secondSlashIndex1+1,thirdSlashIndex1); 
        int Lednum = ledNum.toInt();
        Serial.print("*callback Led Number = ");
        Serial.print(Lednum);
        Serial.println();




        if (String(DeviceId) == BID1){
          //Toggle Lights

        ledToggle(Lednum, ledState2);
       Serial.printf("Light toggled by %u and device ID = %s", fro, String(DeviceId));
        Serial.println();
        }
    }

}
uint32_t bswqweqwe(String payload,int len)
{
    uint32_t i=0;
    uint32_t result = 0;
    for(i = 0;i<len;i++)
    {
        result *= 10;
        result += (char)payload[i]-'0';
    }
    Serial.print("Result Returning = ");
    Serial.println(result);
    return result;
}



void checkRom(){
  Serial.println("\n Starting  EEPROM \n");
  if (!EEPROM.begin(EEPROM_SIZE)) {
    Serial.println("Failed to initialise EEPROM");
    Serial.println("Restarting...");
    delay(500);
    // ESP.restart();
  }
Serial.println("Reading EEPROM ssid");
Serial.print("SSID: ");
for (int i=0;i<20;i++){

  char a = EEPROM.readChar(i);
  if( isAscii(a) && a != 0){
  Serial.print(a);
  MSSID[i] = a;
  meshSsid.concat(a);
   mesh1 = true;

  }
}
Serial.print("WIFI SSID: ");
  int df =30;                   // User wifi ssid address 
  for (int p=0;p<11;p++){
  char a = EEPROM.readChar(df);
  if( isAscii(a) && a != 0){
  Serial.print(a);
  UserSSID[p] = a;
  UserSSID1.concat(a);
   mqttFlag = true;
   df++;
  }

}

Serial.print("PWD: ");
int bf = 41;                    // User wifi pwd address 
for (int l=0;l<11;l++){

  char a = EEPROM.readChar(bf);
  if( isAscii(a) && a != 0){
  Serial.print(a);
  UserPWD[l] = a;
  UserPWD1.concat(a);
  bf++;
  }

}


  char m = EEPROM.readChar(25);
  if(isAlphaNumeric(m) || m == 1){
    Serial.print("Already Have SSID ");
    klag = true;
  }
  else{ 
    Serial.print("Need SSID ");
    klag = false;
  }

  Serial.println("mesh id = ");
 Serial.println(meshSsid);


  // meshSsid.trim();
  // Serial.println(meshSsid.length());
  // Serial.print("SSID: ");
  // Serial.println(meshSsid); 

}

void sendMessage() ; // Prototype so PlatformIO doesn't complain

Task taskSendMessage( TASK_SECOND * 1 , TASK_FOREVER, &sendMessage );

void sendMessage() {
  String msg = "Hello from node ";
  msg += mesh.getNodeId();
  mesh.sendBroadcast( msg );
  taskSendMessage.setInterval( random( TASK_SECOND * 1, TASK_SECOND * 5 ));
}

// Needed for painless library
void receivedCallback( uint32_t from, String &msg ) {
  Serial.printf("startHere: Received from %u msg=%s\n", from, msg.c_str());
  checkbhai(from , msg);

}

void newConnectionCallback(uint32_t nodeId) {
    Serial.printf("--> startHere: New Connection, nodeId = %u\n", nodeId);
}

void changedConnectionCallback() {
    Serial.printf("Changed connections %s\n",mesh.subConnectionJson().c_str());
}

void nodeTimeAdjustedCallback(int32_t offset) {
    Serial.printf("Adjusted time %u. Offset = %d\n", mesh.getNodeTime(),offset);
}

void initMesh(){
            Serial.println("In mesh");

  // mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION );  // set before init() so that you can see startup messages

  mesh.init( meshSsid, MESH_PASSWORD,MESH_PORT ,WIFI_AP_STA ,  CHANNEL ,HIDDEN);

  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);

  DeviceId = mesh.getNodeId();
  Serial.print("DeviceID = ");
        Serial.print(DeviceId);
        Serial.println();
}

void initMesh1(){
  Serial.println("In mesh with MQTT");

  // mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ); // all types on
  mesh.setDebugMsgTypes( ERROR | STARTUP | CONNECTION );  // set before init() so that you can see startup messages

  mesh.init( meshSsid, MESH_PASSWORD,MESH_PORT ,WIFI_AP_STA ,  CHANNEL ,HIDDEN);

  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);
  mesh.onNodeTimeAdjusted(&nodeTimeAdjustedCallback);
  mesh.stationManual(UserSSID1, UserPWD1);
  mesh.setHostname(HOSTNAME);
  DeviceId = mesh.getNodeId();
  Serial.print("DeviceID = ");
        Serial.print(DeviceId);
        Serial.println();
}
// User stub
void lorry(String d){

        int slashIndex = d.indexOf('/');
        //  Search for the next comma just after the first
        int secondSlashIndex = d.indexOf('/', slashIndex + 1);
        int thirdSlashIndex = d.indexOf('/',secondSlashIndex + 1);

        String meshID = d.substring(0,slashIndex);
        // Serial.print("_");
        // Serial.print(meshID);
        // Serial.println();

        BID = "";
        BID = d.substring(slashIndex+1,secondSlashIndex);
        // Serial.print(". ");
        // Serial.print(BID);
        // Serial.println();

  if (d.endsWith("1") || d.endsWith("0")){                 // make conditions
        // ledState = 0;
        String ledState1 = d.substring( thirdSlashIndex + 1 );
        ledState = ledState1.toInt();
        Serial.print("Led State = ");
        Serial.print(ledState);
        Serial.println();
       String ledNum = d.substring(secondSlashIndex+1,thirdSlashIndex); 
        int Lednum = ledNum.toInt();
        Serial.print("Led Number = ");
        Serial.print(Lednum);
        Serial.println();

        if (BID == String(DeviceId)){
          //toggle light
        ledToggle(Lednum,ledState);
        Serial.printf("Light toggled by BLE and device ID = %s", String(DeviceId));
        Serial.println();
        }
        else {
          mesh.sendBroadcast(d);
        }

        String topology = mesh.subConnectionJson();
        Serial.print("Topology = ");
        Serial.print(topology);
        Serial.println();

      }
   else if ( d.endsWith("*") ){

              String p = "status";                // first check id for itself else sendSingle
              char as[13] ;
              BID.toCharArray(as, BID.length()+1);
              long  d1 = atol(as);
              Serial.print(d1);
               Serial.println(" ");

              if (BID == String(DeviceId)){


        StatusBT = String(DeviceId) + "/" + (String)LED_state[0]+(String)LED_state[1]+(String)LED_state[2]+(String)LED_state[3]+(String)LED_state[4]+(String)LED_state[5]+(String)LED_state[6]+(String)LED_state[7];
        Serial.print("for BT = ");
        Serial.print(StatusBT);
        Serial.println();
        flag = true;


              }
              else{
              uint32_t u = bswqweqwe(BID, BID.length());
              
              Serial.print(u);
              mesh.sendSingle( u , p);
              }


              
      }

    else if (d.startsWith("#")){
        mesh1 = true;
        if (d.length() < 9){
        meshSsid = d.substring(d.indexOf("#")+1,slashIndex);
        Serial.print("Mesh SSID = ");
        Serial.print(meshSsid);
        Serial.println();
        memset(MSSID, 0, sizeof MSSID);
        meshSsid.toCharArray(MSSID,meshSsid.length()+1);

        }
    // if (d.length() > 6){

    //       mqttFlag = true;
    // }
    //        flag1 = true;
    //        flag = true;
      // if (d.indexOf("/")!= NULL){

      if (d.length() > 9){
        String UserSSID1 = d.substring(slashIndex+1,secondSlashIndex);
        Serial.print("User Wifi SSID = ");
        Serial.println();
        memset(UserSSID, 0, sizeof UserSSID);
        UserSSID1.toCharArray(UserSSID,UserSSID1.length()+1);
        Serial.print(UserSSID);

        String UserPWD1 = d.substring(secondSlashIndex+1,thirdSlashIndex);
        Serial.print("User Wifi PWD = ");
        Serial.println();
        memset(UserPWD, 0, sizeof UserPWD);
        UserPWD1.toCharArray(UserPWD,UserPWD1.length()+1);
        Serial.print(UserPWD);
      }
         Serial.println("writing eeprom ssid:");


   for(int i=0;i<20;i++){

    EEPROM.writeChar(i,MSSID[i]);
    EEPROM.commit();
    delay(100);

  }
    int JN = 30;
     for(int j=0;j<11;j++){
    EEPROM.writeChar(JN,UserSSID[j]);
    EEPROM.commit();
    delay(100);
    JN++;

  }
      int gf=41;
     for(int k=0;k<11;k++){
    EEPROM.writeChar(gf,UserPWD[k]);
    EEPROM.commit();
    delay(100);
    gf++;
  }

  klag = true;
  EEPROM.writeChar(25,1);
  delay(100);
  EEPROM.commit();
  delay(500);
  ESP.restart();



  // EEPROM.writeChar(meshSsid.length() + 1, 1);
  // EEPROM.commit();
                     //Add termination null character for String Data
 }

 else if(d.equals("clear")){
    for (int i = 0 ; i < 29 ; i++) {
    EEPROM.writeChar(i, 0);
    EEPROM.commit();
    delay(200);


  }
      Serial.println("eeprom MESHID cleared");
    ESP.restart();
 }
   else if(d.equals("wifi/clear")){
    for (int i = 30 ; i < 52 ; i++) {
    EEPROM.writeChar(i, 0);
    EEPROM.commit();
    delay(200);

  }
    Serial.println("eeprom SSID & PWD cleared");
    ESP.restart();
 }
   else if(d.equals(D1+"/clear")){

    for (int i = 0 ; i < 52 ; i++) {
    EEPROM.writeChar(i, 0);
    EEPROM.commit();
    delay(200);

  }
    Serial.println("eeprom ALL cleared");
    ESP.restart();
 }

 else if (d.startsWith("$")){
    String deviceIDS = d.substring(slashIndex+1,secondSlashIndex);
    String timeStamp = d.substring(secondSlashIndex+1,thirdSlashIndex);
    String Swit = d.substring(thirdSlashIndex+1,d.indexOf("/", thirdSlashIndex + 1));
 }

}

void mqttCallback(char* topic, uint8_t* payload, unsigned int length) {
  char* cleanPayload = (char*)malloc(length+1);
  payload[length] = '\0';
  memcpy(cleanPayload, payload, length+1);
   msg = String(cleanPayload);
  free(cleanPayload);

Serial.print("From MQTT = ");
Serial.println(msg);
lorry(msg);


}
IPAddress getlocalIP() {
  return IPAddress(mesh.getStationIP());
}

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();
        data = "";
      if (rxValue.length() > 0) {
        Serial.println("*********");
        Serial.print("Received Value: ");
        for (int i = 0; i < rxValue.length(); i++){

          data.concat(rxValue[i]);

          Serial.print(rxValue[i]);
        }

        

        Serial.println();
        Serial.println("*********");
                Serial.print("data = ");
                Serial.print(data);
                Serial.println();
      
      
      lorry(data);

      }
    }
};




void initBle(){

  if (!klag)
  {
    String noBleName = "UR-" + D1;
    BLEDevice::init(noBleName.c_str());
  }
  else{
    String BleName = "HS-" + D1;
    BLEDevice::init(BleName.c_str());
  }
  
  

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pTxCharacteristic = pService->createCharacteristic(
										CHARACTERISTIC_UUID_TX,
										BLECharacteristic::PROPERTY_NOTIFY
									);
                      
  pTxCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic * pRxCharacteristic = pService->createCharacteristic(
											 CHARACTERISTIC_UUID_RX,
											BLECharacteristic::PROPERTY_WRITE
										);

  pRxCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");

}
void ButtonDebounce(void)
{
    static byte buttonState[8]     = {LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW};   
    static byte lastButtonState[8] = {LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW};   

    static long lastDebounceTime[8] = {0};  
    long debounceDelay = 50; 
  
    byte reading[8];
    
    reading[0] = digitalRead(IN1);
    reading[1] = digitalRead(IN2);
    reading[2] = digitalRead(IN3);
    reading[3] = digitalRead(IN4);
    reading[4] = digitalRead(IN5);
    reading[5] = digitalRead(IN6);
    reading[6] = digitalRead(IN7);
    reading[7] = digitalRead(IN8);
    
    for (volatile int i = 0; i < 8; i++) {
        if (reading[i] != lastButtonState[i]) {
            lastDebounceTime[i] = millis();
        }
      
        if ((millis() - lastDebounceTime[i]) > debounceDelay) {
            // whatever the reading is at, it's been there for longer
            // than the debounce delay, so take it as the actual current state:
        
            // if the button state has changed:
            if (reading[i] != buttonState[i]) {
                buttonState[i] = reading[i];
          
                // only toggle the LED if the new button state is HIGH
                if (buttonState[i] == HIGH) {
                    LED_state[i] = LOW;
                    
                }
                else{
                    LED_state[i] = HIGH;
                }
            }
        }
    } // end for() loop
    

    digitalWrite(OUT1, LED_state[0]);
    digitalWrite(OUT2, LED_state[1]);
    digitalWrite(OUT3, LED_state[2]);
    digitalWrite(OUT4, LED_state[3]);
    digitalWrite(OUT5, LED_state[4]);
    digitalWrite(OUT6, LED_state[5]);
    digitalWrite(OUT7, LED_state[6]);
    digitalWrite(OUT8, LED_state[7]);

    // Serial.printf("LED STATE 1 = %i, \n 2 = %i \n 3 = %i \n 4 = %i, \n 5 = %i \n 6 = %i \n 7 = %i, \n 8 = %i",LED_state[0],LED_state[1],LED_state[2],LED_state[3],LED_state[4],LED_state[5],LED_state[6],LED_state[7]);

    lastButtonState[0] = reading[0];
    lastButtonState[1] = reading[1];
    lastButtonState[2] = reading[2];
    lastButtonState[3] = reading[3];
    lastButtonState[4] = reading[4];
    lastButtonState[5] = reading[5];
    lastButtonState[6] = reading[6];
    lastButtonState[7] = reading[7];
}
uint32_t getMacAddress1(const uint8_t *hwaddr1) {
	// uint8_t baseMac[6];
	// // Get MAC address for WiFi station
	// esp_read_mac(*baseMac, ESP_MAC_WIFI_STA);
	// // char baseMacChr[18] = {0};
	// // sprintf(baseMacChr, "%02X:%02X:%02X:%02X:%02X:%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
	// return uint8_t(*baseMac);

    uint32_t value = 0;

    value |= hwaddr1[2] << 24; //Big endian (aka "network order"):
    value |= hwaddr1[3] << 16;
    value |= hwaddr1[4] << 8;
    value |= hwaddr1[5];
    // Serial.println(hwaddr1[5]);
    return value + 1;
}

String getMacAddress() {
	uint8_t baseMac[6];
	// Get MAC address for WiFi station
	esp_read_mac(baseMac, ESP_MAC_WIFI_STA);
  // Serial.println(baseMac[6]);
  uint8_t* fsas = (uint8_t*)baseMac;
  uint32_t value1 = getMacAddress1(fsas);
  Serial.println(value1);

	// char baseMacChr[18] = {0};
	// sprintf(baseMacChr, "%02X:%02X:%02X:%02X:%02X:%02X", baseMac[0], baseMac[1], baseMac[2], baseMac[3], baseMac[4], baseMac[5]);
	return String(value1);
}


void setup() {
  Serial.begin(115200);
    Wire.begin();
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  pinMode(IN1, INPUT);
  pinMode(IN2, INPUT);
  pinMode(IN3, INPUT);
  pinMode(IN4, INPUT);
  pinMode(IN5, INPUT);
  pinMode(IN6, INPUT);
  pinMode(IN7, INPUT);
  pinMode(IN8, INPUT);

  pinMode(OUT1, OUTPUT);
  pinMode(OUT2, OUTPUT);
  pinMode(OUT3, OUTPUT);
  pinMode(OUT4, OUTPUT);
  pinMode(OUT5, OUTPUT);
  pinMode(OUT6, OUTPUT);
  pinMode(OUT7, OUTPUT);
  pinMode(OUT8, OUTPUT);
  pinMode(reset_pin, INPUT);


     D1 = getMacAddress();



    
Serial.print("Device ID = ");
Serial.print(D1);
Serial.println(" ");

  Serial.println("Starting BLE work!");

// Create the BLE Device

// Serial.println("In mesh");
checkRom();
initBle();

    while(!mesh1){
    lorry(data);

    }

if(mqttFlag == false){
initMesh();
}
else if(mqttFlag == true){
  initMesh1();
}

// }
  
  pinMode (LED_BUILTI, OUTPUT);
  digitalWrite(LED_BUILTI,LOW);


}

void loop() {

  userScheduler.execute(); // it will run mesh scheduler as well
  mesh.update();
//  DateTime now = rtc.now();



    if (mqttFlag){

  mqttClient.loop();

  if(myIP != getlocalIP()){
    myIP = getlocalIP();
    Serial.println("My IP is " + myIP.toString());

    if (mqttClient.connect("homeSwitchClient")) {
      char DevID[29];
      String ads = "homeSwitch/ready/"+String(DeviceId);
      ads.toCharArray(DevID,ads.length()+1);
      mqttClient.publish( DevID,"Ready!");
      char DevID1[20];
       String ads1 = "HS/"+String(DeviceId) + "/all";
      ads1.toCharArray(DevID1,ads1.length()+1);

      mqttClient.subscribe(DevID1);
            Serial.println(DevID);
           Serial.println(DevID1);

      Serial.println("Ready");
    } 
  }
  // if ( (now.minute() % 2) == 0){

  // }
    }

 if (deviceConnected && flag) {

        // uint8_t gj= 9899213123123123123; //hasd(StatusBT, StatusBT.length())

      //  if(flag1 == true){
      //   char response[3] ;
      //   String res = "OK";
      //   res.toCharArray(response, res.length()+1);
      //   pTxCharacteristic->setValue(response);  
      //   pTxCharacteristic->setNotifyProperty(true);
      //   pTxCharacteristic->notify();
      //   Serial.println("from loop flag");
      //   flag1 = false;
      //   ESP.restart();
      //   }  // first check id for itself else sendSingle


      //   else
      //   {
        char toSend[20] ;
        StatusBT.toCharArray(toSend, StatusBT.length()+1);
        pTxCharacteristic->setValue(toSend);
        // pTxCharacteristic->setNotifyProperty(true);
        pTxCharacteristic->notify();
        Serial.println("from loop flag status");
        // }
        




		delay(10); // bluetooth stack will go into congestion, if too many packets are sent
    flag = false ; 
    
	}

  //  if (deviceConnected && mqttFlag && flag1) {
  //       char response[20] ;
  //       String res = "OK";
  //       StatusBT.toCharArray(response, res.length()+1);
  //       pTxCharacteristic->setValue(response);
  //       pTxCharacteristic->setNotifyProperty(true);
  //       pTxCharacteristic->notify();
  //       delay(10); 
  //       flag1  = false;
  //           mqttFlag = false ; 
  //           ESP.restart();

  //  }


    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
		// do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }

    ButtonDebounce();
    // if (digitalRead(reset_pin)==HIGH){
    // for (int i = 0 ; i < 52 ; i++) {
    // EEPROM.writeChar(i, 0);
    // EEPROM.commit();
    // delay(200);
    // }
    // Serial.print("EEPROM cleared manually");
    // ESP.restart();
    // }

 }
