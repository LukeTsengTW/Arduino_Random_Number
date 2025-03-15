/*

ESP32_Randomly_Drawing_Numbers

Author : LukeTseng
Date : 2024 / 03 / 12 初製作

Version : 1.1 版本 2025/03/14

Update : Fix some bugs, Creating a new APP for this machine, can control machine.
 
*/

#include "BluetoothSerial.h"
BluetoothSerial SerialBT;

#include "LiquidCrystal_I2C.h" // 引入函式庫 : LCD 模組

#include <math.h>
#include <vector>
#include <algorithm>

using namespace std;

#define MAX_RANGE 40 // for counting_sort

// the range of random
#define MIN_NUM 1
#define MAX_NUM 35

// pins
#define SINGLE 25
#define MUTIPLE 26
#define CLEAR 27

// initial LCD 1602 I2C
LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3,POSITIVE);

// the random of result
int x = 0;

int i = 0; // 連續抽選 : 全域變數 : 抽選多個號碼時兩數字間間隔所需的儲存資料型態的變數

bool switch_once = true; // 按鈕切換時執行的動作 : 全域變數 : 用於迴圈當中只需執行一次的代碼

bool is_single = false;

bool is_mutiple = false;

vector<int> generated_numbers; // 已經由亂數生成的隨機號碼先儲存至陣列當中
int generated_count = 0; // 確認已經抽選了幾次的號碼, 如果 >= 35 則重置所有號碼

int len(int n) {
    return (n == 0) ? 1 : (int)log10(abs(n)) + 1;
}

vector<int> parseElements(String elements_str) {
  vector<int> elements;
  int commaIndex;
  while ((commaIndex = elements_str.indexOf(',')) != -1) {
    String element_str = elements_str.substring(0, commaIndex);
    elements.push_back(element_str.toInt());
    elements_str = elements_str.substring(commaIndex + 1);
  }
  if (elements_str.length() > 0) {
    elements.push_back(elements_str.toInt());
  }
  return elements;
}

void addMultipleElements(vector<int> to_add) {
  size_t original_size = generated_numbers.size();
  for (int value : to_add) {
    generated_numbers.push_back(value);  // 將元素加入陣列
  }
  SerialBT.print("已新增元素: ");
  for (int value : to_add) {
    SerialBT.print(String(value) + " ");
  }
  SerialBT.println();
  SerialBT.print("當前陣列: ");
  for (int value : generated_numbers) {
    SerialBT.print(String(value) + " ");
  }
  SerialBT.println();
}

void setup() {

  Serial.begin(115200);
  SerialBT.begin("ESP32_BT(RM)");  // 設定藍牙設備名稱
  Serial.println("藍牙設備已啟動，等待連接..."); 

  // 初始化LCD螢幕
  lcd.begin(16, 2); // 像素格 16 x 2
  lcd.setBacklight(128); // 將 LCD 背光調整到最高 255
    
  // 初始化輸入腳位, 使用 for 迴圈迭代, 遍歷方式初始化從 PIN 腳位到 MUTIPLE 腳位
  for (int pin = SINGLE; pin <= CLEAR; pin++) {
    pinMode(pin, INPUT_PULLUP);
  }
    
  lcd.clear();
  lcd.print("What you drew is");
}

void loop() {

  if (SerialBT.available()) {
    String received = SerialBT.readStringUntil('\n');
    received.trim();
    if (received.startsWith("REMOVE:")) {
      String elements_str = received.substring(7);  // 提取元素部分
      vector<int> to_remove = parseElements(elements_str);
      addMultipleElements(to_remove);
    }
    else if (received == "Single") {
      is_single = true;
      SerialBT.println("單號抽選");
    }
    else if (received == "Mutiple") {
      is_mutiple = true;
      SerialBT.println("連續抽選");
    }
    else if (received == "Reset"){
      lcd.clear();
      lcd.print("What you drew is");
      i=0;
      switch_once = true;
      generated_numbers.empty();
    }
    else {
      SerialBT.println("無效指令");
    }
  }
  
  if (digitalRead(SINGLE) == LOW || is_single == true) { // 如果按下按鈕
    switch_once = true;
    is_single = false;
    if (generated_count >= MAX_NUM) {
      lcd.clear();
      lcd.print("Reset");
      delay(2000);
      lcd.clear();
      lcd.print("What you drew is");
      i=0;
      switch_once = true;
      generated_numbers.empty();
    }
    
    do {
      x = esp_random() % MAX_NUM + MIN_NUM;
    } while (check_object(x));

    generated_numbers.push_back(x);
    sort(generated_numbers.begin(), generated_numbers.end());
    generated_count = generated_numbers.size();

    lcd.clear();
    lcd.print("What you drew is");
    lcd.setCursor(0, 1);
    lcd.print(x);
    delay(300); // 延時 0.3 秒後繼續
  }

  if (digitalRead(MUTIPLE) == LOW || is_mutiple == true) {
    is_mutiple = false;
    if (switch_once){
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("What you drew is");
      i=0;
      switch_once = false;
    }

    if (generated_count >= MAX_NUM) {
      lcd.clear();
      lcd.print("Reset");
      delay(2000);
      lcd.clear();
      lcd.print("What you drew is");
      i=0;
      switch_once = true;
      generated_numbers.empty();
    }

    do {
      x = esp_random() % MAX_NUM + MIN_NUM;
    } while (check_object(x));

    generated_numbers.push_back(x);
    sort(generated_numbers.begin(), generated_numbers.end());
    generated_count = generated_numbers.size();

    lcd.setCursor(i, 1);

    int x_len = len(x);
    i += (x_len == 1) ? 2 : 3;

    if (i > 14){
      switch_once = true;
      delay(1000);
    }

    lcd.print(x);
    delay(300);
  }

  if (digitalRead(CLEAR) == LOW) {
    lcd.clear();
    lcd.print("What you drew is");
    i=0;
    switch_once = true;
    generated_numbers.empty();
  }

}

bool check_object(int num) {
    int left = 0;
    int right = generated_numbers.size() - 1;

    while (left <= right) {
        int mid = left + (right - left) / 2;
        if (generated_numbers[mid] == num) {
            return true;
        } else if (generated_numbers[mid] < num) {
            left = mid + 1;
        } else {
            right = mid - 1;
        }
    }
    return false;
}
