#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <math.h>

const int led1 = 5;
const int led2 = 25;
const int led3 = 26;
const int touch_pin1 = 2;
const int touch_pin2 = 13;
const int touch_pin3 = 12;

int toi1_state = 0;
int toi2_state = 0;
int toi3_state = 0;
int touch1Val;
int touch2Val;
int touch3Val;
char str[50];

const int _size = 2*JSON_OBJECT_SIZE(4);
StaticJsonDocument<_size> JSONPost;

const char* ssid = "Mrw";
const char* password = "1q2w3e4r";
const char* url_post = "https://ecourse.cpe.ku.ac.th/exceed05/api/toilet";



void WiFi_Connect(){
  WiFi.disconnect();
  WiFi.begin(ssid,password);
  while(WiFi.status()!=WL_CONNECTED){
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to the WiFi network");
  Serial.print("IP Address : ");
  Serial.println(WiFi.localIP());
}


void _post(int room,int state){
  Serial.println(room);
  Serial.println(state);
  if(WiFi.status() == WL_CONNECTED){
    HTTPClient http;
    
    http.begin(url_post);
    http.addHeader("Content-Type", "application/json");

    JSONPost["room"] = room;
    JSONPost["state"] = state;

    serializeJson(JSONPost, str);
    int httpCode = http.POST(str);

    if(httpCode == HTTP_CODE_OK){
      String payload = http.getString();
      Serial.println(httpCode);
      Serial.println("POST result");
      Serial.println(payload);
    } else{
      Serial.println(httpCode);
      Serial.println("ERROR on HTTP Request");
    }
  } else{
    WiFi_Connect();
  }
  //delay(100);
  vTaskDelay(100 / portTICK_PERIOD_MS);
}




void Task1(void* param){
  while(1){
    touch1Val=touchRead(touch_pin1);
    if(toi1_state==0 && touch1Val <= 30){
      toi1_state=1;
      digitalWrite(led1,!LOW);
      _post(1,1);
      //vTaskDelay(100 / portTICK_PERIOD_MS);
    }else if(toi1_state==1 && touch1Val > 30){
      toi1_state=0;
      digitalWrite(led1, !HIGH);
      _post(1,0);
      //vTaskDelay(100 / portTICK_PERIOD_MS);
    } 
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}


void Task2(void* param){
  while(1){
    touch2Val=touchRead(touch_pin2);
    //Serial.print("touch 2 ");
    //Serial.println(touch2Val);
    if(toi2_state==0 && touch2Val <= 30){
      toi2_state=1;
      digitalWrite(led2,LOW);
      _post(2,1);
    }else if(toi2_state==1 && touch2Val > 30){
      toi2_state=0;
      digitalWrite(led2,HIGH);
      _post(2,0);
    } 
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}


void Task3(void* param){
  while(1){
    touch3Val=touchRead(touch_pin3);
    //Serial.print("touch 3 ");
    //Serial.println(touch3Val);
    if(toi3_state==0 && touch3Val <= 30){
      toi3_state=1;
      digitalWrite(led3,LOW);
      _post(3,1);
    }else if(toi3_state==1 && touch3Val > 30){
      toi3_state=0;
      digitalWrite(led3,HIGH);
      _post(3,0);
    } 
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}


void setup() {
  Serial.begin(115200); 
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(led3, OUTPUT);
  digitalWrite(led1 ,!HIGH);
  digitalWrite(led2 ,HIGH);
  digitalWrite(led3, HIGH);
  WiFi_Connect();
  xTaskCreatePinnedToCore(Task1, "Task1",32*1024, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(Task2, "Task2",32*1024, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(Task3, "Task3",32*1024, NULL, 1, NULL, 1);

  
}




void loop() {

}
