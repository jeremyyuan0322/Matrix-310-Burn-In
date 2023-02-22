#include "src/Artila-Matrix310.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
// #include <time.h>
#include <WiFi.h>
#include <Ethernet.h>


void wifiConnect() {
  const char *ssid = "Artila";
  const char *password = "CF25B34315";
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  //reconnect every 10sec
  unsigned long startTime = millis();
  while (WiFi.status() != WL_CONNECTED) {
    vTaskDelay(500 / portTICK_PERIOD_MS);
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
  byte mac[] = { 0x98, 0xf4, 0xab, 0x17, 0x24, 0xc5 };
  Ethernet.init(LAN_CS);
  // start the Ethernet connection:
  Serial.println("Initialize Ethernet with DHCP:");
  //Matrix310 tries connecting the internet with DHCP
  //reconnect every 10sec
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
    vTaskDelay(2000/ portTICK_PERIOD_MS);
    digitalWrite(LED_READY, LOW);  // turn the LED off by making the voltage LOW
    vTaskDelay(2000/ portTICK_PERIOD_MS);
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
  unsigned long printRandom = millis();
  while (1) {
    srand(time(NULL));
    vTaskDelay(1000/ portTICK_PERIOD_MS);
    int x = rand();
    if (millis() - printRandom >= 5000) {
      Serial.printf("x = %d\n", x);
      printRandom = millis();
    }
    //reboot every 5min(300sec)
    if (millis() - startTime > 300000) {
      Serial.println("reboot");
      ESP.restart();
    }
  }
}

void setup() {
  const char *version = "v0.0.0";
  Serial.begin(115200);
  Serial.printf("Version: %s\n", version);
  Serial.println("Burn in start!");

  xTaskCreate(taskLED, "LED blink task", 2048, NULL, 1, NULL);
  xTaskCreate(taskConnect, "Connection task", 2048, NULL, 1, NULL);
  xTaskCreate(taskCpuRun, "Cpu run", 2048, NULL, 1, NULL);
}

void loop() {
}
