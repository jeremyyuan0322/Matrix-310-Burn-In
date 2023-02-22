#include "src/Artila-Matrix310.h"
#include <Arduino.h>
#include <time.h>
#include <WiFi.h>
#include <Ethernet.h>
const char *ssid = "Artila";
const char *password = "CF25B34315";
byte mac[] = { 0x98, 0xf4, 0xab, 0x17, 0x24, 0xc5 };
void wifiConnect() {
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);

  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED) {
    vTaskDelay(500);
    Serial.print(".");
    if (WiFi.status() == WL_CONNECTED) {
      break;
    }
    if (millis() - startTime > 10000) {
      break;
    }
  }
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi connect fail");
  }
}
void ethConnect() {
  
  Ethernet.init(LAN_CS);
  // start the Ethernet connection:
  Serial.println("Initialize Ethernet with DHCP:");
  //Matrix310 tries connecting the internet with DHCP
  unsigned long connectTimeout = 10000;
  if (Ethernet.begin(mac, connectTimeout) == 0) {
    //Fail to use DHCP
    Serial.println("Failed to configure Ethernet using DHCP");
    // Check for Ethernet hardware present
    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    }
    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    } else {
      Serial.println("Ethernet connect fail");
    }
  } else {  //Matrix310 already connect to the internet
    Serial.print("  DHCP assigned IP ");
    Serial.println(Ethernet.localIP());
  }
}
void taskLED(void *pvParam) {
  pinMode(LED_READY, OUTPUT);  // pin 27
  while (1) {
    digitalWrite(LED_READY, HIGH);  // turn the LED on (HIGH is the voltage level)
    vTaskDelay(2000);
    digitalWrite(LED_READY, LOW);  // turn the LED off by making the voltage LOW
    vTaskDelay(2000);
  }
}
void taskConnect(void *pvParam) {
  while (1) {
    wifiConnect();
    ethConnect();
  }
}
void taskCpuRun(void *pvParam) {
  unsigned long startTime = millis();
  while (1) {
    // srand(time(NULL));
    // int x = rand();
    // Serial.printf("x = %d\n", x);
    if (millis() - startTime > 300000) {
      Serial.println("reboot");
      ESP.restart();
    }
  }
}

void setup() {
  
  Serial.begin(115200);
  Serial.println(xTaskCreate(taskLED, "LED blink task", 1000, NULL, 1, NULL));
  
  xTaskCreate(taskConnect, "Connection task", 1000, NULL, 1, NULL);
  xTaskCreate(taskCpuRun, "Cpu run", 1000, NULL, 1, NULL);
}

void loop() {
}
