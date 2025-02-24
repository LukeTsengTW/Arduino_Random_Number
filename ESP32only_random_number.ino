/*

ESP32_Randomly_Drawing_Numbers

Author : LukeTseng
Date : 2024 / 03 / 12 初製作

Version : 1.0 版本 2025/02/24
 
*/

#include <WiFi.h>
#include <math.h>
#include "LiquidCrystal_I2C.h" // 引入函式庫 : LCD 模組

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

int generated_numbers[MAX_NUM+1]; // 已經由亂數生成的隨機號碼先儲存至陣列當中
int generated_count = 0; // 確認已經抽選了幾次的號碼, 如果 >= 35 則重置所有號碼

int len(int n) {
    return (n == 0) ? 1 : (int)log10(abs(n)) + 1;
}

void setup() {
  WiFi.begin("your_SSID", "your_PASSWORD");

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
  
  if (digitalRead(SINGLE) == LOW) { // 如果按下按鈕
    switch_once = true;
    if (generated_count >= MAX_NUM) {
      lcd.clear();
      lcd.print("Reset");
      delay(2000);
      ESP.restart();
    }
    
    do {
      x = esp_random() % MAX_NUM + MIN_NUM;
    } while (check_object(x));

    generated_numbers[generated_count++] = x;

    counting_sort(generated_numbers, generated_count);

    lcd.clear();
    lcd.print("What you drew is");
    lcd.setCursor(0, 1);
    lcd.print(x);
    delay(300); // 延時 0.3 秒後繼續
  }

  if (digitalRead(MUTIPLE) == LOW) {
    if (switch_once){
      lcd.clear();
      generated_count = 0;
      i=0;
      switch_once = false;
    }

    do {
      x = esp_random() % MAX_NUM + MIN_NUM;
    } while (check_object(x));

    generated_numbers[generated_count++] = x;

    counting_sort(generated_numbers, generated_count);

    lcd.setCursor(0, 0);
    lcd.print("What you drew is");
    lcd.setCursor(i, 1);

    int x_len = len(x);
    i += (x_len == 1) ? 2 : 3;

    if (i > 16){
      switch_once = true;
      lcd.clear();
      lcd.print("Reset");
      delay(1000);
      generated_count = 0;
      i=0;
      lcd.clear();
      lcd.print("What you drew is");
    }

    lcd.print(x);
    delay(300);
  }

  if (digitalRead(CLEAR) == LOW) {
    ESP.restart();
  }

}

bool check_object(int num) {
  int left = 0;
  int right = generated_count - 1;
  
  while (left <= right) {
    int mid = left + (right - left) / 2;
    
    if (generated_numbers[mid] == num) {
      return true;
    }
    else if (generated_numbers[mid] < num) {
      left = mid + 1;
    }
    else {
      right = mid - 1;
    }
  }
  
  return false;
}

void counting_sort(int arr[], int n) {
  // 靜態分配計數陣列，假設範圍為 0 到 MAX_RANGE-1
  int count[MAX_RANGE] = {0};
  int output[n]; // 靜態分配輸出陣列

  // 檢查輸入是否超出範圍
  for (int i = 0; i < n; i++) {
    if (arr[i] < 0 || arr[i] >= MAX_RANGE) {
      Serial.println("Error: Value out of range!");
      return;
    }
  }

  // 統計每個元素的出現次數
  for (int i = 0; i < n; i++) {
    count[arr[i]]++;
  }

  // 累加計數陣列
  for (int i = 1; i < MAX_RANGE; i++) {
    count[i] += count[i - 1];
  }

  // 構建排序後的陣列
  for (int i = n - 1; i >= 0; i--) {
    output[count[arr[i]] - 1] = arr[i];
    count[arr[i]]--;
  }

  // 將結果複製回原陣列
  for (int i = 0; i < n; i++) {
    arr[i] = output[i];
  }
}