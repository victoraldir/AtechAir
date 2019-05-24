#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WebServer.h>
#include <IRsend.h>
#include <EEPROM.h>
extern "C"
{
#include "user_interface.h"
#include "wpa2_enterprise.h"
}


//===============================================================
//  NODEMCU PINS TABLE
//===============================================================
/*
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
  */
  



// Absolute min and max eeprom addresses. Actual values are hardware-dependent.
// These values can be changed e.g. to protect eeprom cells outside this range.
const int EEPROM_MIN_ADDR = 0;
const int EEPROM_MAX_ADDR = 511;



char auth[] = ""; // Auth Token in the Blynk App.

static const char *ssid = ""; // SSID to connect to

static const char *username = ""; // Username for authentification

static const char *password = ""; // Password for authentication

#define LED_BUILTIN 2 // led built in of lolin v3

WidgetLED led1(V1); // Virtual Led on blynk

const uint16_t kIrLed = D2; // ESP8266 GPIO to Send IR command.

IRsend irsend(kIrLed); // Set the GPIO to be used to sending the message.

// Keep this flag not to re-sync on every reconnection
bool isFirstConnect = true;
bool modoAP = false;
//SSID and Password for ESP8266 AP mode
const char *ssidAp = "ATECH-AIR-";

ESP8266WebServer server(80); //Server on port 80

String nomeRede = "";
String usuarioRede = "";
String senhaRede = "";

// página inicial de configuração
const char MAIN_page[] PROGMEM = R"=====( 
<!DOCTYPE html>
<html>
  <head>
      <title>Configurando seu equipamento</title>
      <meta name="viewport" content="width=device-width, user-scalable=no" />
      <meta charset="ISO-8859-1">
  </head>
  <body>
      <h2>ATECH AIR 1.2.1<h2>
      <h3> Configuration Wifi Mode</h3>

      <form action="/action_page">
          Nome da rede:<br />
          <input type="text" name="nomeRede" value="" placeholder="SSID">
            <br/>
            <br/>
          Usuario da rede:<br/>
          <input type="text" name="usuarioRede" value="" placeholder="USERNAME">
            <br/>
            <br/>
          Senha da rede:<br/>
          <input type="text" name="senhaRede" value="" placeholder="PASSWORD">
            <br/>
            <br/>
          <input type="submit" value="Gravar configuração">                  
      </form>             
  </body>
</html>
)=====";

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
uint16_t timerId;

SimpleTimer timer;
bool Connected2Blynk = false;

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0; // will store last time LED was updated

// constants won't change:
const long interval = 1000; // interval at which to blink (milliseconds)

// Variables will change:
int ledState = LOW; // ledState used to set the LED


int tentativasConexao = 0;

//===============================================================
// BLYNK OPERATIONS
//===============================================================

// This function will run every time Blynk connection is established
BLYNK_CONNECTED()
{
  if (isFirstConnect)
  {
    // Request Blynk server to re-send latest values for all pins
    Blynk.syncAll();
    isFirstConnect = false;
    Serial.println("BLYNK SINCRONIZADO: ");
  }
}

BLYNK_WRITE(V0) // valor da temperatura vindo do servidor
{

  if (param.asInt())
  {
    temperatura = param.asInt();
  }
}

// Function to get value from virtual pin V2 (LIGA/DESLIGA)
BLYNK_WRITE(V2)
{
  // int pinValue = param.asInt(); // assigning incoming value from pin V1 to a variable
  if (param.asInt())
  {
    Serial.println("LIGAR");
    irsend.sendRaw(ligar23, 59, 38); // ligando no 23

    led1.on();
    digitalWrite(LED_BUILTIN, LOW); // Turn the LED on
    delay(40);
    digitalWrite(LED_BUILTIN, HIGH); // Turn the LED off
    led1.off();
    Blynk.virtualWrite(V0, 23);
  }
  else
  {
    Serial.println("DESLIGAR");
    irsend.sendRaw(desligar, 59, 38); // desligando
    temperatura = 0;
    led1.on();
    digitalWrite(LED_BUILTIN, LOW); // Turn the LED on
    delay(40);
    digitalWrite(LED_BUILTIN, HIGH); // Turn the LED off
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
    digitalWrite(LED_BUILTIN, LOW); // Turn the LED on

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
    digitalWrite(LED_BUILTIN, HIGH); // Turn the LED off
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
    digitalWrite(LED_BUILTIN, LOW); // Turn the LED on

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
    digitalWrite(LED_BUILTIN, HIGH); // Turn the LED off
    led1.off();
  }
}

//===============================================================
// This routine is executed when you open its IP in browser
//===============================================================
void handleRoot()
{
  String homePage = MAIN_page;             //Read HTML contents
  server.send(200, "text/html", homePage); //Send web page
}

void handleForm()
{

  String data = server.arg("nomeRede");
  String result = "";
  String recivedData;

  String s = "<a href='/'> Go Back </a>";
  server.send(200, "text/html", s); //Send web page

  ESP.reset(); // GUARDAR NO EPRO E RESETAR O BICHO
}

// ==============================================================
// EEPROM MEMORY OPERATIONS
// ==============================================================
void writeEEPROM(char add, String data)
{
  int _size = data.length();
  int i;
  for (i = 0; i < _size; i++)
  {
    EEPROM.write(add + i, data[i]);
  }
  EEPROM.write(add + _size, '\0'); //Add termination null character for String Data
  EEPROM.commit();
}

String read_EEPROM(char add)
{
  int i;
  char data[100]; //Max 100 Bytes
  int len = 0;
  unsigned char k;
  k = EEPROM.read(add);
  if (k == 255)
  {
    return String("vazio");
  }
  while (k != '\0' && len < 500) //Read until null character
  {
    k = EEPROM.read(add + len);
    data[len] = k;
    len++;
  }
  data[len] = '\0';
  return String(data);
}

// ==============================================================
// WIFI OPERATIONS
// ==============================================================


void ConnectWifi()
{

  if (WiFi.status() != WL_CONNECTED)
  {
    
    Serial.println();
    Serial.println("TENTANDO CONECTAR NO WIFI: ");

    connectWifiWpa();

    int count = 0;
    while (WiFi.status() != WL_CONNECTED)// aguardando o wifi conectar 
    {
      count++;
      Serial.print(count);
      Serial.print(". ");
      delay(1000);
      if (count == 30)
      { 
        tentativasConexao ++;
        Serial.println();
        Serial.println("ERRO AO CONECTAR AO WIFI ");
        Serial.print("Tentativa: ");
        Serial.print(tentativasConexao);
        if(tentativasConexao == 3) {
          Serial.println();
          Serial.println("Tentou conectar 3 vezes, entrar em modoAP e ficar por 180 segundos");
        }
        return;
      }
    }
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    tentativasConexao = 0;
    Serial.println();
    Serial.print("DISPOSITIVO CONECTADO AO WIFI, IP: ");
    Serial.println(WiFi.localIP());
    conectarBlynk();
  } else
  {
    Serial.println();
    Serial.println("PROBLEMAS PARA CONECTAR NO WIFI");
  }
}

void connectWifiComun(){
 WiFi.begin(ssid, password);
}

void connectWifiWpa(){
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
}

void conectarBlynk() {
    Blynk.config(auth); // in place of Blynk.begin(auth, ssid, pass);
    Serial.println();
    Serial.println("TENTASNDO CONECTAR AO BLYNK ");
    while (Blynk.connect() == false)
    {
      // Wait until connected
      Serial.print(".");
      delay(1000);
    }
    Serial.println("DISPOSITIVO CONECTADO AO BLYNK");
    digitalWrite(LED_BUILTIN, HIGH); // desligando o led
    isFirstConnect = false;
}

void initApMode()
{
  Serial.println("");
  Serial.println("tENTANDO ENTRAR EM MODO AP");
  Serial.println("");

  WiFi.mode(WIFI_AP);              //Only Access point
  WiFi.softAP(ssidAp+WiFi.macAddress()); //Start HOTspot removing password will disable security

  IPAddress myIP = WiFi.softAPIP(); //Get IP address
  Serial.print("HotSpt IP:");
  Serial.println(myIP);

  server.on("/", handleRoot);            //Which routine to handle at root location
  server.on("/action_page", handleForm); //form action is handled here
  server.begin();                        //Start server
  Serial.println("MODO AP INCIADO COM SUCESSO ");
}

void CheckConnection()
{
  Serial.println();
  Serial.println(" ---------------------- check connection  ------------------------");
  Serial.println();
  Connected2Blynk = Blynk.connected();
  
  Serial.print("STATUS BLYNK:");
  Serial.println(Connected2Blynk);
  Serial.println("--------");
  Serial.print("STATUS WIFI:");
  Serial.println(WiFi.status());

  if (WiFi.status() != WL_CONNECTED || !Connected2Blynk)
  {
    Serial.println();
    Serial.println("NOT CONNECTED TO  Blynk or WIFI");
    ConnectWifi();
  }
  else
  {
    Serial.println();
    Serial.print("STILL CONECTED, IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();
  }
}



void IsWifiConfigInMemory()
{ // tamanho da palavra no eeprom é de 50 bytes

  Serial.println();
  Serial.println("---------------------------------------------------------------");
  Serial.println("IsWifiConfigInMemory: VERIFICANDO SE EXISTE UMA CONFIGURAÇÃO DE WIFI NA MEMÓRIA EEPROM ");
  Serial.println();
  Serial.print(" -> Lendo nomeRede do wifi no EEPROM: ");
  Serial.println(read_EEPROM(0));
  Serial.print(" -> Lendo usuarioRede do wifi no EEPROM: ");
  Serial.println(read_EEPROM(50));
  Serial.print(" -> Lendo senhaRede do wifi no EEPROM: ");
  Serial.println(read_EEPROM(100));
  Serial.println("----------------------------------------");
  Serial.println();

  if (read_EEPROM(0) != "vazio" && read_EEPROM(50) != "vazio" && read_EEPROM(100) != "vazio")
  { // se existir algo nos 3 endereços eeprom então entra em station mode
    modoAP = false;
  }
  else
  {
    modoAP = true;
  }
}




void setup()
{
  // put your setup code here, to run once:
  Serial.begin(9600);
  EEPROM.begin(512);
  irsend.begin();
  pinMode(LED_BUILTIN, OUTPUT);
  delay(400);

  IsWifiConfigInMemory(); // verifica se já tem rede wifi gravada na placa
  //modoAP = false // *****************APAGAR

  if (modoAP == true)
  {
    Serial.println("ENTRANDO EM MODO AP");
    initApMode();
  }
  else
  {
    Serial.println("ENTRANDO EM MODO STATION");
    Serial.println("iniciando o timer para conectar no wifi em 20 segundos");
    timerId = timer.setInterval(20000L, CheckConnection); // check if still connected every 20 seconds
  }
}

void piscarLedModoAp()
{
  // check to see if it's time to blink the LED; that is, if the difference
  // between the current time and last time you blinked the LED is bigger than
  // the interval at which you want to blink the LED.
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval)
  {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW)
    {
      ledState = HIGH;
    }
    else
    {
      ledState = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(LED_BUILTIN, ledState);
  }
}

void loop()
{
  timer.run();
  if (modoAP == false) // MODO STATION
  {                
    if (WiFi.status() == WL_CONNECTED && Connected2Blynk)
    {
      Blynk.run(); // only process Blyk.run() function if we are connected to Blynk server
    }
  }
  else // MODO AP
  {
    server.handleClient(); // escuta os clientes
    piscarLedModoAp();
  }
}
