#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <IRsend.h>

static const uint8_t D0   = 16;
  static const uint8_t D1   = 5;
  static const uint8_t D2   = 4;
  static const uint8_t D3   = 0;
  static const uint8_t D4   = 2;
  static const uint8_t D5   = 14;
  static const uint8_t D6   = 12;
  static const uint8_t D7   = 13;
  static const uint8_t D8   = 15;
  static const uint8_t D9   = 3;
  static const uint8_t D10  = 1;

extern "C"
{
#include "user_interface.h"
#include "wpa2_enterprise.h"
}

char auth[] = ""; // Auth Token in the Blynk App.

static const char *ssid = ""; // SSID to connect to

static const char *username = ""; // Username for authentification

static const char *password = ""; // Password for authentication

WidgetLED led1(V1); // Virtual Led on blynk

const uint16_t kIrLed = D2; // ESP8266 GPIO to Send IR command.

const int ledRed = D7; // Fisical red led indicator

IRsend irsend(kIrLed); // Set the GPIO to be used to sending the message.

// Keep this flag not to re-sync on every reconnection
bool isFirstConnect = true;

uint16_t ligar[71] = {9104, 4428, 628, 1646, 624, 516, 626, 514, 630, 514, 630, 512, 630, 514, 628, 514, 628, 514, 628, 516, 628, 1640, 630, 1642, 626, 1642, 628, 1640, 628, 1640, 630, 1640, 628, 1642, 628, 516, 628, 516, 628, 516, 628, 514, 628, 518, 626, 514, 630, 1642, 628, 514, 628, 1644, 624, 1642, 628, 1642, 628, 1640, 628, 1640, 630, 1638, 630, 516, 628, 1644, 626, 40104, 9106, 2208, 630};    // NEC 807F02FD
uint16_t aumentar[71] = {9082, 4450, 608, 1660, 606, 536, 606, 536, 634, 510, 634, 510, 606, 536, 608, 536, 604, 538, 606, 558, 586, 1664, 604, 1662, 606, 1664, 606, 1662, 608, 1662, 606, 1662, 606, 1664, 608, 538, 606, 538, 606, 538, 606, 1662, 608, 1662, 606, 538, 606, 538, 606, 536, 604, 1666, 602, 1666, 608, 1662, 606, 538, 606, 556, 588, 1662, 608, 1724, 542, 1666, 608, 40142, 9082, 2230, 604}; // NEC 807F18E7
uint16_t diminuir[71] = {9104, 4426, 628, 1640, 630, 514, 628, 516, 628, 516, 628, 518, 624, 516, 628, 514, 628, 514, 628, 516, 626, 1640, 630, 1640, 630, 1640, 628, 1640, 628, 1640, 630, 1640, 628, 1640, 628, 518, 628, 514, 626, 516, 626, 514, 630, 1640, 628, 516, 628, 514, 630, 514, 628, 1640, 628, 1642, 626, 1642, 626, 1642, 626, 514, 628, 1642, 628, 1642, 628, 1640, 628, 40070, 9128, 2206, 630}; // NEC 807F08F7

uint16_t desligar[59] = {3110, 9758, 502, 1554, 544, 490, 544, 524, 510, 518, 510, 1560, 510, 516, 510, 518, 508, 520, 508, 1536, 544, 1544, 510, 516, 510, 526, 508, 518, 510, 492, 544, 516, 510, 492, 542, 500, 542, 516, 510, 518, 508, 518, 510, 524, 512, 1544, 510, 518, 510, 1528, 544, 492, 542, 520, 508, 494, 542, 1544, 510};    // LG2 88C0051
uint16_t ligar18[59] = {3176, 9730, 566, 1502, 570, 488, 538, 488, 538, 488, 540, 1492, 570, 496, 536, 474, 572, 464, 568, 474, 570, 492, 534, 474, 572, 486, 540, 488, 538, 494, 540, 1516, 538, 1526, 534, 490, 536, 474, 570, 1516, 536, 1526, 536, 490, 538, 1520, 506, 500, 570, 488, 536, 1524, 538, 498, 538, 1518, 536, 464, 570};   // LG2 880334A
uint16_t ligar19[59] = {3128, 9774, 500, 1554, 540, 522, 504, 522, 504, 524, 504, 1522, 538, 534, 502, 540, 502, 498, 538, 528, 512, 524, 504, 540, 502, 522, 504, 522, 506, 528, 506, 1552, 504, 1554, 506, 522, 504, 1540, 538, 524, 504, 510, 516, 512, 514, 1566, 504, 530, 504, 524, 504, 1548, 504, 524, 504, 1538, 522, 1558, 506};   // LG2 880344B
uint16_t ligar20[59] = {3156, 9728, 568, 1500, 570, 498, 504, 522, 504, 522, 504, 1520, 570, 500, 506, 500, 570, 464, 570, 472, 572, 490, 508, 500, 570, 486, 540, 490, 534, 500, 538, 1516, 536, 1524, 538, 490, 512, 1532, 570, 488, 536, 1518, 538, 490, 538, 1522, 506, 500, 568, 492, 536, 1524, 538, 1534, 536, 496, 540, 464, 572};   // LG2 880354C
uint16_t ligar21[59] = {3132, 9812, 486, 1558, 508, 542, 482, 544, 484, 544, 458, 1578, 510, 550, 484, 560, 482, 552, 486, 558, 458, 568, 484, 556, 486, 542, 486, 542, 484, 552, 484, 1572, 456, 1602, 484, 544, 486, 1568, 508, 1554, 534, 516, 486, 542, 482, 1580, 484, 518, 508, 542, 482, 1554, 482, 1588, 484, 552, 482, 1578, 510};  // LG2 880364D
uint16_t ligar22[59] = {3154, 9728, 568, 1504, 570, 490, 538, 490, 534, 488, 540, 1492, 570, 496, 510, 500, 570, 464, 570, 474, 570, 492, 508, 500, 570, 490, 538, 488, 538, 502, 506, 1544, 534, 1524, 538, 490, 512, 1534, 570, 1524, 538, 1524, 538, 492, 534, 1518, 536, 488, 540, 490, 538, 1490, 568, 1526, 538, 1528, 532, 492, 536}; // LG2 880374E
uint16_t ligar23[59] = {3176, 9730, 568, 1502, 572, 490, 536, 490, 538, 492, 536, 1490, 570, 496, 538, 474, 570, 466, 570, 472, 572, 492, 532, 474, 570, 488, 540, 488, 538, 502, 514, 1538, 536, 1524, 538, 1526, 536, 488, 538, 490, 536, 490, 538, 488, 538, 1518, 536, 464, 572, 490, 536, 1516, 538, 1524, 538, 1524, 540, 1520, 534};  // LG2 880384F
uint16_t ligar24[59] = {3128, 9738, 536, 1552, 546, 516, 506, 520, 506, 518, 508, 1522, 542, 526, 506, 504, 540, 494, 540, 504, 538, 520, 508, 502, 540, 522, 478, 546, 506, 528, 506, 1550, 506, 1556, 506, 1556, 506, 522, 506, 520, 506, 1546, 508, 522, 506, 1522, 540, 520, 506, 520, 508, 518, 506, 522, 506, 496, 514, 530, 512};     // LG2 8803940
uint16_t ligar25[59] = {3128, 9758, 514, 1554, 512, 546, 506, 522, 504, 522, 506, 1524, 540, 530, 506, 504, 538, 498, 538, 504, 512, 548, 504, 504, 540, 520, 506, 522, 504, 530, 506, 1550, 504, 1554, 508, 1554, 508, 520, 506, 1548, 506, 520, 506, 522, 504, 1524, 538, 522, 504, 522, 506, 522, 506, 520, 508, 496, 538, 1540, 538};    // LG2 8803A41
uint16_t ligar26[59] = {3124, 9770, 514, 1570, 512, 550, 478, 546, 506, 504, 512, 1582, 506, 522, 480, 522, 512, 548, 480, 522, 512, 556, 504, 520, 506, 528, 472, 552, 478, 546, 482, 1556, 538, 1550, 504, 1538, 538, 524, 478, 1574, 480, 1574, 480, 546, 506, 1532, 512, 546, 506, 496, 540, 520, 506, 494, 540, 1558, 478, 548, 482};   // LG2 8803B42
uint16_t ligar27[59] = {3174, 9744, 568, 1502, 570, 488, 538, 488, 538, 490, 536, 1536, 534, 466, 572, 464, 568, 474, 570, 490, 534, 476, 570, 488, 538, 492, 534, 500, 536, 490, 538, 1532, 538, 1516, 536, 1500, 570, 1524, 538, 490, 536, 490, 538, 490, 536, 1508, 570, 488, 540, 464, 570, 490, 536, 466, 570, 1526, 536, 1528, 532};   // LG2 8803C43
uint16_t ligar28[59] = {3130, 9768, 516, 1570, 542, 516, 510, 522, 506, 502, 540, 1554, 510, 518, 510, 492, 542, 520, 506, 494, 540, 528, 506, 522, 506, 518, 508, 520, 506, 520, 508, 1528, 544, 1544, 510, 1536, 544, 1552, 508, 516, 510, 1528, 544, 518, 508, 1536, 542, 518, 506, 528, 508, 516, 510, 1520, 542, 502, 540, 518, 510};   // LG2 8803D44
uint16_t ligar29[59] = {3158, 9774, 538, 1530, 538, 510, 514, 512, 542, 486, 514, 1558, 514, 522, 514, 522, 514, 528, 514, 514, 512, 530, 514, 514, 512, 514, 516, 518, 514, 512, 514, 1556, 516, 1538, 516, 1554, 512, 1548, 516, 1546, 516, 512, 514, 512, 516, 1546, 516, 510, 516, 512, 516, 518, 516, 1538, 516, 520, 514, 1548, 514};  // LG2 8803E45
uint16_t ligar30[59] = {3160, 9770, 534, 1516, 570, 490, 512, 492, 570, 490, 510, 1526, 570, 488, 540, 464, 568, 492, 510, 492, 542, 514, 538, 492, 508, 516, 512, 516, 510, 518, 510, 1534, 570, 1518, 536, 1534, 536, 1530, 506, 1554, 506, 1544, 538, 490, 536, 1508, 570, 492, 536, 464, 570, 490, 510, 1518, 570, 1492, 570, 472, 570}; // LG2 8803F46

uint16_t temperatura = 0;

SimpleTimer timer;
bool Connected2Blynk = false;

// This function will run every time Blynk connection is established
BLYNK_CONNECTED()
{
  if (isFirstConnect)
  {
    // Request Blynk server to re-send latest values for all pins
    Blynk.syncAll();
    isFirstConnect = false;
    Serial.println("BLYNK SINCRONIZADO: ");
    Serial.println(temperatura);
  }
}

BLYNK_WRITE(V0) // valor da temperatura vindo do servidor
{

  if (param.asInt())
  {
    Serial.println("get temperatura from server: ");
    Serial.print(param.asInt());
    temperatura = param.asInt();
  }
}

// Function to get value from virtual pin V2 (LIGA/DESLIGA)
BLYNK_WRITE(V2)
{
  // int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  if (param.asInt())
  {
    Serial.println("Enviando comando de LIGAR ");
    irsend.sendRaw(ligar23, 59, 38); // ligando no 23

    led1.on();
    digitalWrite(ledRed, HIGH);
    delay(40);
    digitalWrite(ledRed, LOW);
    led1.off();
    Blynk.virtualWrite(V0, 23);
  }
  else
  {
    Serial.println("Enviando comando de desligar");
    irsend.sendRaw(desligar, 59, 38); // desligando
    temperatura = 0;
    led1.on();
    digitalWrite(ledRed, HIGH);
    delay(40);
    digitalWrite(ledRed, LOW);
    led1.off();
    Blynk.virtualWrite(V0, 0);
  }
  Blynk.syncVirtual(V0); // sincronizando a temperatura
}

// Function to get value from virtual pin V3 (AUMENTAR)
BLYNK_WRITE(V3)
{

  if (param.asInt())
  {
    Serial.print("Comando de aumentar ---> ");
    Serial.print("temperatura ");
    Serial.println(temperatura);
    led1.on();
    digitalWrite(ledRed, HIGH);

    if (temperatura < 30 && temperatura != 0)
    { // se a temperatura for menor que 30 então aumenta
      switch (temperatura)
      {
      case 18:
        irsend.sendRaw(ligar19, 59, 38);
        delay(20);
        Blynk.virtualWrite(V0, 19);
        break;
      case 19:
        irsend.sendRaw(ligar20, 59, 38);
        delay(20);
        Blynk.virtualWrite(V0, 20);
        break;
      case 20:
        irsend.sendRaw(ligar21, 59, 38);
        delay(20);
        Blynk.virtualWrite(V0, 21);
        break;
      case 21:
        irsend.sendRaw(ligar22, 59, 38);
        delay(20);
        Blynk.virtualWrite(V0, 22);
        break;
      case 22:
        irsend.sendRaw(ligar23, 59, 38);
        delay(20);
        Blynk.virtualWrite(V0, 23);
        break;
      case 23:
        irsend.sendRaw(ligar24, 59, 38);
        delay(20);
        Blynk.virtualWrite(V0, 24);
        break;
      case 24:
        irsend.sendRaw(ligar25, 59, 38);
        delay(20);
        Blynk.virtualWrite(V0, 25);
        break;
      case 25:
        irsend.sendRaw(ligar26, 59, 38);
        delay(20);
        Blynk.virtualWrite(V0, 26);
        break;
      case 26:
        irsend.sendRaw(ligar27, 59, 38);
        delay(20);
        Blynk.virtualWrite(V0, 27);
        break;
      case 27:
        irsend.sendRaw(ligar28, 59, 38);
        delay(20);
        Blynk.virtualWrite(V0, 28);
        break;
      case 28:
        irsend.sendRaw(ligar29, 59, 38);
        delay(20);
        Blynk.virtualWrite(V0, 29);
        break;
      case 29:
        irsend.sendRaw(ligar30, 59, 38);
        delay(20);
        Blynk.virtualWrite(V0, 30);
        break;
      }
    }

    Blynk.syncVirtual(V0);
    digitalWrite(ledRed, LOW);
    led1.off();
  }
}

// Function to get value from virtual pin V4 (DIMINUIR)
BLYNK_WRITE(V4)
{

  if (param.asInt())
  {
    Serial.print("Comando de diminuir ---> ");
    Serial.print("temperatura: ");
    Serial.println(temperatura);
    led1.on();
    digitalWrite(ledRed, HIGH);

    if (temperatura > 17 && temperatura != 0)
    { // se a temperatura for menor que 30 então aumenta

      switch (temperatura)
      {

      case 19:
        irsend.sendRaw(ligar18, 59, 38);
        delay(20);
        Blynk.virtualWrite(V0, 18);
        break;
      case 20:
        irsend.sendRaw(ligar19, 59, 38);
        delay(20);
        Blynk.virtualWrite(V0, 19);
        break;
      case 21:
        irsend.sendRaw(ligar20, 59, 38);
        delay(20);
        Blynk.virtualWrite(V0, 20);
        break;
      case 22:
        irsend.sendRaw(ligar21, 59, 38);
        delay(20);
        Blynk.virtualWrite(V0, 21);
        break;
      case 23:
        irsend.sendRaw(ligar22, 59, 38);
        delay(20);
        Blynk.virtualWrite(V0, 22);
        break;
      case 24:
        irsend.sendRaw(ligar23, 59, 38);
        delay(20);
        Blynk.virtualWrite(V0, 23);
        break;
      case 25:
        irsend.sendRaw(ligar24, 59, 38);
        delay(20);
        Blynk.virtualWrite(V0, 24);
        break;
      case 26:
        irsend.sendRaw(ligar25, 59, 38);
        delay(20);
        Blynk.virtualWrite(V0, 25);
        break;
      case 27:
        irsend.sendRaw(ligar26, 59, 38);
        delay(20);
        Blynk.virtualWrite(V0, 26);
        break;
      case 28:
        irsend.sendRaw(ligar27, 59, 38);
        delay(20);
        Blynk.virtualWrite(V0, 27);
        break;
      case 29:
        irsend.sendRaw(ligar28, 59, 38);
        delay(20);
        Blynk.virtualWrite(V0, 28);
        break;
      case 30:
        irsend.sendRaw(ligar29, 59, 38);
        delay(20);
        Blynk.virtualWrite(V0, 29);
        break;
      }
    }

    Blynk.syncVirtual(V0);
    digitalWrite(ledRed, LOW);
    led1.off();
  }
}

// function to connect PEAP wifi network
void connectWifiAtech()
{
  if (WiFi.status() != WL_CONNECTED)
  {
    WiFi.disconnect(true);
    Serial.println();
    Serial.println("TENTANDO CONECTAR ");
    Serial.println();

    // Setting ESP into STATION mode only (no AP mode or dual mode)
    wifi_set_opmode(STATION_MODE);

    struct station_config wifi_config;

    memset(&wifi_config, 0, sizeof(wifi_config));
    strcpy((char *)wifi_config.ssid, ssid);

    wifi_station_set_config(&wifi_config);

    wifi_station_clear_cert_key();
    wifi_station_clear_enterprise_ca_cert();

    wifi_station_set_wpa2_enterprise_auth(1);
    wifi_station_set_enterprise_identity((uint8 *)username, strlen(username));
    wifi_station_set_enterprise_username((uint8 *)username, strlen(username));
    wifi_station_set_enterprise_password((uint8 *)password, strlen(password));

    wifi_station_connect();
    Serial.print("Wifi station connect status:");
    Serial.println(wifi_station_get_connect_status());

    // Wait for connection AND IP address from DHCP
    Serial.println();
    Serial.println("PEGANDO UM IP ");
    Serial.println();
    while (WiFi.waitForConnectResult() != WL_CONNECTED)
    {
      delay(2000);
      Serial.print(".");
    }
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    Blynk.config(auth); // in place of Blynk.begin(auth, ssid, pass);
    Serial.println("try conect to Blynk server");
    while (Blynk.connect() == false)
    {
      // Wait until connected
      Serial.print("|-");
    }
    Serial.println("Connected to Blynk server");
  }
  else
  {
    Serial.println("\nCheck Router ");
  }
}

void CheckConnection()
{
  Serial.println("check connection");
  Serial.print("blynk:");
  Connected2Blynk = Blynk.connected();
  Serial.println(Connected2Blynk);
  Serial.print("WIFI:");
  Serial.println(WiFi.status());

  delay(10);
  if (WiFi.status() != WL_CONNECTED || !Connected2Blynk)
  {

    Serial.println("Not connected to Blynk server");
    connectWifiAtech();
  }
  else
  {
    Serial.print("Still connected to Blynk server: ");
    Serial.println(WiFi.localIP());
  }
}

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  delay(10);
  pinMode(ledRed, OUTPUT);
  irsend.begin();
  // Blynk.begin(auth, ssid, pass); // WIFI NORMAL
  connectWifiAtech();                         // WIFI ATECH
  timer.setInterval(20000L, CheckConnection); // check if still connected every 20 seconds
}

void loop()
{
  if (WiFi.status() == WL_CONNECTED && Connected2Blynk)
  {
    Blynk.run(); // only process Blyk.run() function if we are connected to Blynk server
  }
  timer.run();
}
