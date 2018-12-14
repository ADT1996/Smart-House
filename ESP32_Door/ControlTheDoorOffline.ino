#include "Header/ControlTheDoorOffline.h"

char getKey() {
  char key = '\0';
  while (!key) { 				// Neu chua bam phim
    key = myKeypad.getKey();
  }
  return key;
}

void Task_Keypad(void* parameter) {
  Serial.println("@");
  while (true) {
    char key = myKeypad.getKey();

    switch (key) {
      case '\0':
        break;

      case '#': {
          Key_Buff[i] = '\0';
          int lenBuff = strlen(Key_Buff);
          int lenOur = strlen(ourCode);

          if (strcmp(Key_Buff, ourCode) != 0) {
            Door_Stt = 0;
          } else {
            Door_Stt = 1;
          }

          if (Door_Stt == 1 ) {
            count = 3;
            mocua();
            vTaskDelay(5000);
            if (!lock) dongcua();
            Door_Stt = 0;
          } else {
            count--;
            if (count == 0) {
              Serial.println("Count = 0");
            } else {
              Serial.println("");
              Serial.println("Sai " + count);
            }
          }

        }

      case '*': {
          clearLCD();
          Serial.println("Da xoa man hinh");
          memset( Key_Buff, '\0', i);
          i = 0;
          break;
        }

      default: {
          if (i != 32) {
            Key_Buff[i] = key;
            Serial.print(key);
            writeLCD(key);
            i++;
          }
          break;
        }
    }
    vTaskDelay(1000);
  }
}
