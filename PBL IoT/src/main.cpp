#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include <UniversalTelegramBot.h>
#include <max6675.h>

// Online
const char *ssid = "Atsc-2.4G";
const char *password = "1245@atsc";

#define BOTtoken "8443285127:AAFCr39YeJOhXfsmw98RDInTQkiyIYiiG-I"
#define CHAT_id "6235767776"

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

int thermoSO = 19;
int thermoCS = 5;
int thermoSCK = 18;
MAX6675 thermocouple(thermoSCK, thermoCS, thermoSO);



#define LED_Onboard 2

const int btn1 =  13;
const int btn2 =  27;
const int btn3 =  14;
const int btn4 =  26;

int in_up = btn2;
int in_down = btn3;
int in_msg = btn4;
int in_start = btn1;

const int Gas = 34;

int Max_temp = 400;

void setup() {
    Serial.begin(115200);

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


    digitalWrite(LED_Onboard, 0);
}
void inputSW(){
    if(digitalRead(in_up) == LOW){
        Max_temp += 10;
        Serial.println("Max-Temp : " + String(Max_temp));
        delay(500);
    }

    if(digitalRead(in_down) == LOW){
        Max_temp -= 10;
        Serial.println("Max-Temp : " + String(Max_temp));
        delay(500);
    }
    if(digitalRead(in_msg) == LOW){
        String statusMsg = "📊 Realtime-Status\n";
        statusMsg += "Max-Temp :" + String(Max_temp);
        bot.sendMessage(CHAT_id, statusMsg, "");
        delay(500);
    }
}

void loop(){
    inputSW();
}