#include<string.h>
#include <String.h>
#include <Stepper.h>
#include <Keypad.h>
#include <WiFi.h>
#include <HTTPClient.h>
#define LED_BUILTIN 2

//const char* ssid = "ADT";
//const char* password = "Tuyen@ADT1996";
const char* ssid = "BoSu";
const char* password = "DSNW25121996";
String pass;
int httpCode;

int IN1 = 13;
int IN2 = 12;
int IN3 = 33;
int IN4 = 32;
int Steps = 0;
boolean Direction = true;
unsigned long last_time;
unsigned long currentMillis ;
int steps_left = 1150;
long time1;

int currentPosition = 0;
const byte n_rows = 4;
const byte n_cols = 4;
char keys[n_rows][n_cols] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

const char* ourCode;
byte rowPins[n_cols] = {14, 27, 26, 25};
byte colPins[n_rows] = {5, 18, 19, 23};
Keypad myKeypad = Keypad( makeKeymap(keys), rowPins, colPins, n_rows, n_cols);

TaskHandle_t Task1, Task2;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(LED_BUILTIN, OUTPUT);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    vTaskDelay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP Address");
  Serial.println(WiFi.localIP());

  xTaskCreatePinnedToCore(
    Task_Controlbyweb, /* Task function. */
    "Task_Controlbyweb", /* String with name of task. */
    10000, /* Stack size in words. */
    NULL, /* Parameter passed as input of the task */
    2, /* Priority of the task. */
    &Task1, /* Task handle. */
    1); /* Core. */

  xTaskCreatePinnedToCore(
    Task_Keypad, /* Task function. */
    "Task_Keypad", /* String with name of task. */
    10000, /* Stack size in words. */
    NULL, /* Parameter passed as input of the task */
    1, /* Priority of the task. */
    &Task2, /* Task handle. */
    0); /* Core. */
}

int count = 3;
bool door = false;
bool lock = false;

void mocua() {
  if (!door) {
    digitalWrite(LED_BUILTIN, HIGH);
    motor();
    door = !door;
    Direction = !Direction;
    steps_left = 1150;
  }
}

void dongcua() {
  if (door) {
    digitalWrite(LED_BUILTIN, LOW);
    motor();
    door = !door;
    Direction = !Direction;
    steps_left = 1150;
  }
}
//====================================================================================================

void motor() {
  while (steps_left > 0) {
    currentMillis = micros();
    if (currentMillis - last_time >= 1000) {
      stepper(1);
      time1 = time1 + micros() - last_time;
      last_time = micros();
      steps_left--;
    }
  }
}

//=============================================
//motor
void stepper(int xw) {
  for (int x = 0; x < xw; x++) {
    switch (Steps) {
      case 0:
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
        break;
      case 1:
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, HIGH);
        break;
      case 2:
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
        break;
      case 3:
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
        digitalWrite(IN3, HIGH);
        digitalWrite(IN4, LOW);
        break;
      case 4:
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, HIGH);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
        break;
      case 5:
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, HIGH);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
        break;
      case 6:
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
        break;
      case 7:
        digitalWrite(IN1, HIGH);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, HIGH);
        break;
      default:
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
        break;
    }
    SetDirection();
  }
}
void SetDirection() {
  if (Direction == 1)
    Steps++;
  if (Direction == 0)
    Steps--;
  if (Steps > 7)
    Steps = 0;
  if (Steps < 0)
    Steps = 7;
}


void loop() {
  // put your main code here, to run repeatedly:

}

void Task_Controlbyweb(void * parameter)
{
  for (;;) {
    if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status

      HTTPClient http;

      http.begin("http://newws.somee.com/CuaRaVao/isMoCua"); //Specify the URL
      httpCode = http.GET();                                        //Make the request

      if (httpCode == 200) { //Check for the returning code

        String res = http.getString();
        Serial.println(res);
        if(res == "true")        
      }
      else {
        Serial.println("Error on HTTP request");
      }
      http.end(); //Free the resources
    }
  }
}

void Task_Keypad(void * parameter)
{
  for (;;) {
    char key, Key_Buff[100];
    int i = 0, Door_Stt = 0;
    if ((WiFi.status() == WL_CONNECTED)) { //Check the current connection status

      HTTPClient http;

      http.begin("http://newws.somee.com/CuaRaVao/MatKhauCua"); //Specify the URL
      httpCode = http.GET();                                        //Make the request

      if (httpCode == 200) //Check for the returning code

        pass = http.getString();

      else {
        Serial.println("Error on HTTP request");
      }
      http.end(); //Free the resources
    }
    Serial.println(httpCode);
    Serial.println(pass);

    char pass_array[pass.length() + 1];
    pass.toCharArray(pass_array, pass.length() + 1);
    ourCode = pass_array;
    Serial.println(ourCode);


    while (true) {
      key = myKeypad.getKey();

      if (int(key) != 0) {
        Key_Buff[i] = key;
        Serial.print(key);
        i++;
      }
      switch (key) {
        case '#':
          {
            if (Key_Buff[3] != NULL)
            {
              //Door_Stt = strcmp(Key_Buff,ourCode);
              Door_Stt = 1;
              int lenBuff = strlen(Key_Buff);
              int lenOur = strlen(ourCode) + 1;
              if (lenBuff != lenOur) {
                Door_Stt = 0;
              } else
              {
                for (int j = 0; j < 4; j++) {
                  if (Key_Buff[j] != ourCode[j])
                    Door_Stt = 0 ;
                }
              }
            }
          }
          //memset( Key_Buff, '\0', i);
          //i = 0;
          //break;
          if (Door_Stt == 1 )
          {
            count = 3;
            mocua();
            vTaskDelay(5000);
            if (!lock) dongcua();
            Door_Stt = 0;
          }

          else
          {
            count--;
            if (count == 0)
            {
              Serial.println("Count = 0");
            }
            else
            {
              Serial.println("");
              Serial.println("Sai " + count);
            }
          }



        case '*': {
            Serial.println("Da xoa man hinh");
          }
          memset( Key_Buff, '\0', i);
          i = 0;
          break;
      }
    }
    vTaskDelay(1000);

  }
}