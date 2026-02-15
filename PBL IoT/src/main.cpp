#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <UniversalTelegramBot.h>
#include <max6675.h>
#include <LiquidCrystal_I2C.h>

// Online
const char *ssid = "TurnOnMedia-2.4G";
const char *password = "turnon2560";

#define BOTtoken "8443285127:AAFCr39YeJOhXfsmw98RDInTQkiyIYiiG-I"
#define CHAT_id "6235767776"

unsigned long lastMsgTime = 0;    
unsigned long msgInterval = 15000;

bool gasSystemActive = false;
int editMode = 0;

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

LiquidCrystal_I2C lcd(0x27, 16, 2);

int thermoSO = 19;
int thermoCS = 5;
int thermoSCK = 18;
MAX6675 thermocouple(thermoSCK, thermoCS, thermoSO);

#define LED_Onboard 2

const int btnBoot = 0;
const int btn1 =  13;
const int btn2 =  27;
const int btn3 =  14;
const int btn4 =  26;

int in_up = btn2;
int in_down = btn3;
int in_msg = btn4;
int in_start = btn1;

const int in_Gas = 34;

float TempMax = 200.0;
float GasMin = 600;
int Current_Gas = 0;
float Current_Temp = 0.00;

void setup() {
    Serial.begin(115200);

    lcd.init();
    lcd.backlight();

    delay(3000);

    WiFi.begin(ssid, password);
    client.setInsecure();

    pinMode(LED_Onboard, OUTPUT);
    pinMode(btn1, INPUT);
    pinMode(btn2, INPUT);
    pinMode(btn3, INPUT);
    pinMode(btn4, INPUT);

    Serial.print("Connecting to WIFI ");

    while (WiFi.status() != WL_CONNECTED)
    {   
        lcd.setCursor(0, 0);
        lcd.print("Wifi Connecting");
        digitalWrite(LED_Onboard, 1);
        Serial.print(" . ");
        delay(350);
        digitalWrite(LED_Onboard, 0);
        Serial.print(" . ");
        delay(350);
    }
    Serial.println("\nWIFI Connected!");
    
    if(bot.sendMessage(CHAT_id, "ESP-32 Conneted!", "")){
        Serial.println("Message Sent Success!");
        digitalWrite(LED_Onboard, 1);
        
    }else{
    
        Serial.println("Fail To Send Message");
    }

    lcd.clear();
    lcd.print("WiFi Connected!");
    digitalWrite(LED_Onboard, 0);
}

void inputSW(){
    if (digitalRead(btnBoot) == LOW) {
        editMode = !editMode;
    
        Serial.print("Mode Switched to: ");
        Serial.println(editMode == 0 ? "EDIT TEMP" : "EDIT GAS");
        
        delay(400);
    }

    if (digitalRead(in_up) == LOW) {
        if (editMode == 0) {
            TempMax += 10;
        } else {
            GasMin += 100;
        }
        delay(200);
    }
    
    if(digitalRead(in_down) == LOW){
        if (editMode == 0) {
            TempMax -= 10;
        } else {
            GasMin -= 100;
        }
        delay(200);
    }

    if(digitalRead(in_msg) == LOW){
        String statusMsg = "    Realtime-Status📊\n";
        statusMsg += "Max-Temp :" + String(TempMax) +"\n";
        statusMsg += "Min-Gas :" + String(GasMin) +"\n";
        statusMsg += "Current-Temp :" + String(Current_Temp)+"\n";
        statusMsg += "Current-Gas :" + String(Current_Gas)+"\n";
        bot.sendMessage(CHAT_id, statusMsg, "");
        delay(500);
    }

    if (digitalRead(in_start) == LOW) {
        if (!gasSystemActive) {
            gasSystemActive = true;
            digitalWrite(LED_Onboard, 1);
            bot.sendMessage(CHAT_id, "🛡️ Gas Sensor : Active!", "");
            lcd.setCursor(0, 1);
            lcd.print("GAS SYSTEM: ON  ");
            delay(1000);
        }
    }
}

void Value_Input(){
    Current_Gas = analogRead(in_Gas);
    Current_Temp = thermocouple.readCelsius();
}

void Serial_Data(){
    inputSW();
    Value_Input();
    Serial.println("SW 1 : " + String(digitalRead(btn1)));
    Serial.println("SW 2 : " + String(digitalRead(btn2)));
    Serial.println("SW 3 : " + String(digitalRead(btn3)));
    Serial.println("SW 4 : " + String(digitalRead(btn4)));
    Serial.println("Temp : " + String(Current_Temp));
    Serial.println("Gas : " + String(Current_Gas));
    Serial.println("Max-Temp : " + String(TempMax));
    Serial.println("Min-Gas : " + String(GasMin));
    delay(200);
}

void Sensor_Alert(){
    

    if(Current_Temp >= TempMax){
        if (millis() - lastMsgTime > msgInterval) {
            String WarningMsg = "    Temperature Limit Point 💥\n";
            WarningMsg += "Max-Temp :" + String(TempMax)+"\n";
            WarningMsg += "Min-Gas :" + String(GasMin)+"\n";
            WarningMsg += "Current-Temp :" + String(Current_Temp)+"\n";
            WarningMsg += "Current-Gas :" + String(Current_Gas)+"\n";
            WarningMsg + "อุณหภูมิสูงเกินไป\n";
            if(bot.sendMessage(CHAT_id, WarningMsg, "")){
                Serial.println("Alert Sent!");
                lastMsgTime = millis();
            }
        }
    }

    if(gasSystemActive == true){
        if(Current_Gas <= GasMin){
            if (millis() - lastMsgTime > msgInterval) {
                String GasWarningMsg = "    Gas Safe Point ✅\n";
                GasWarningMsg += "Max-Temp :" + String(TempMax)+"\n";
                GasWarningMsg += "Min-Gas :" + String(GasMin)+"\n";
                GasWarningMsg += "Current-Temp :" + String(Current_Temp)+"\n";
                GasWarningMsg += "Current-Gas :" + String(Current_Gas)+"\n";
                GasWarningMsg +"แก๊สในถังหมดแล้ว\n";
                if (bot.sendMessage(CHAT_id, GasWarningMsg, "")) {
                    Serial.println("Gas Alert Sent!");
                    lastMsgTime = millis();
                }
            }
        }
    }
}

void loop(){
    inputSW();
    Serial_Data();
    Value_Input();
    Sensor_Alert();
    lcd.setCursor(0, 0);
    lcd.print("Max-Temp:");
    lcd.print(TempMax);
    lcd.setCursor(0, 1);
    lcd.print("Min-Gas:");
    lcd.print(GasMin);

    


    delay(200);
}