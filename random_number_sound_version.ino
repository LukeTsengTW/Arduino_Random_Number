/*

Arduino_Randomly_Drawing_Numbers(Sound_Version)

Author : LukeTseng ( 鼓山高中 _ 電腦社社長 )
Date : 2024 / 04 / 05 初製作

Version : 1.0.8 版本 2024/08/08

Update : 優化程式碼（修改資料型態使記憶體整體縮小）、CLEAR, MUTIPLE 腳位修改、將插入排序法修改為計數排序法、號碼區間修改為 1 ~ 35 號、改用外部隨機數函式庫 TrueRandom
 
*/

#include <TrueRandom.h> // 引入函式庫 : TrueRandom
#include "LiquidCrystal_I2C.h" // 引入函式庫 : LCD 模組

#define PIN 2 // 腳位 2 為單號抽選按鈕
#define MUTIPLE 3 // 腳位 3 為連續抽選按鈕 (照情況可連抽 5 ~ 6 號)
#define CLEAR 4 // 腳位 4 為清除畫面按鈕

#include <DFMiniMp3.h>  
#include <SoftwareSerial.h> //使用軟體Serial
SoftwareSerial mySerial(11, 10); // RX, TX

class Mp3Notify; // 宣告 notify class

typedef DFMiniMp3<SoftwareSerial, Mp3Notify> DfMp3;

DfMp3 dfmp3(mySerial); // 建立mp3物件

// 實作notification的類別，可以在不同的事件中，寫入想要進行的動作
// 若沒有特別要進行的處理，這裡不用修改
class Mp3Notify
{
public:
  static void PrintlnSourceAction(DfMp3_PlaySources source, const char* action)
  {
    if (source & DfMp3_PlaySources_Sd) 
    {
        Serial.print("SD Card, ");
    }
    if (source & DfMp3_PlaySources_Usb) 
    {
        Serial.print("USB Disk, ");
    }
    if (source & DfMp3_PlaySources_Flash) 
    {
        Serial.print("Flash, ");
    }
    Serial.println(action);
  }
  static void OnError([[maybe_unused]] DfMp3& mp3, uint16_t errorCode)
  {
    // 錯誤訊息
    Serial.println();
    Serial.print("Com Error ");
    Serial.println(errorCode);
  }
  static void OnPlayFinished([[maybe_unused]] DfMp3& mp3, [[maybe_unused]] DfMp3_PlaySources source, uint16_t track)
  {
    Serial.print("Play finished for #");
    Serial.println(track);  
  }
  static void OnPlaySourceOnline([[maybe_unused]] DfMp3& mp3, DfMp3_PlaySources source)
  {
    PrintlnSourceAction(source, "online");
  }
  static void OnPlaySourceInserted([[maybe_unused]] DfMp3& mp3, DfMp3_PlaySources source)
  {
    PrintlnSourceAction(source, "inserted");
  }
  static void OnPlaySourceRemoved([[maybe_unused]] DfMp3& mp3, DfMp3_PlaySources source)
  {
    PrintlnSourceAction(source, "removed");
  }
};

LiquidCrystal_I2C lcd(0x27,2,1,0,4,5,6,7,3,POSITIVE); // LCD 模組初始化

uint8_t x = 0; // 抽號結果 : 全域變數

uint8_t i = 0; // 多號抽選 : 全域變數 : 抽選多號時兩數字間間隔所需的儲存資料型態的變數

bool switch_once = true; // 按鈕切換時執行的動作 : 全域變數 : 用於迴圈當中只需執行一次的代碼

uint8_t generated_numbers[36]; // 已經由亂數生成的隨機號碼先儲存至陣列當中
uint8_t generated_count = 0; // 確認已經抽選了幾次的號碼, 如果 >= 35 則重置所有號碼

uint8_t len(uint8_t n){      // len(int n) 為計算數字長度的函數, 用於多號抽選時判斷號碼長度, 以來取數字間間隔
    uint8_t length = 0;  // 初始化局域變數 length = 0
    while(n != 0){  // 如果 n != 0 則執行迴圈
        n /= 10; // 由於整數型態宣告, 所以使用除法時不會有除法, 只會有整數
        length++; // length 遞增 1
    }
    return length;  // 回傳 length 表示數字長度
}

void startup_init(uint8_t x){   // startup_init(uint8_t x) 為避免重複性代碼之函數, 主要是用於各抽號後的 LCD 訊息
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("What you drew is");
  lcd.setCursor(0, 1);
  lcd.print(x);
}

void normal_init(){   // normal_init() 為避免重複性代碼之函數, 主要是用於各抽號的初始化 LCD 訊息
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("What you drew is");
}

void button_init(){ 
  switch_once = true;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Reset number.");
  delay(1500);
  generated_count = 0;
  i=0;
  normal_init();
  delay(300);
}

void setup() {
    Serial.begin(9600); //啟用監控視窗
    Serial.println("initializing...");

    dfmp3.begin();  //開始使用DFPlayer模組

    //重置DFPlayer模組，會聽到"波"一聲
    dfmp3.reset();

    //音量控制，0~30
    uint16_t volume = dfmp3.getVolume();
    Serial.print("volume ");
    Serial.println(volume);
    dfmp3.setVolume(20);

    //取得所有MP3檔的總數
    uint16_t count = dfmp3.getTotalTrackCount(DfMp3_PlaySource_Sd);
    Serial.print("files ");
    Serial.println(count);

    // 初始化LCD螢幕
    lcd.begin(16, 2); // 像素格 16 x 2
    lcd.setBacklight(255); // 將 LCD 背光調整到最高 255
    
    // 初始化輸入腳位, 使用 for 迴圈迭代, 遍歷方式初始化從 PIN 腳位到 MUTIPLE 腳位
    for (uint8_t pin = PIN; pin <= CLEAR; pin++) {
        pinMode(pin, INPUT);
        digitalWrite(pin, HIGH);
    }
    
    // LCD 初始化訊息 : normal_init() 函數呼叫
    normal_init();
}

void loop() {
  
  if (digitalRead(PIN) == LOW) { // 判斷如果讀取 PIN 腳位的電位是低電位的話則進行單號抽選
    switch_once = true;
    if (generated_count >= 35){ // 判斷如果 generated_count >= 35 則重置所有號碼 (放在裡面是因為在外面的話抽到第 35 次則會直接將第 35 次的號碼刷掉, 為了實用性, 再按一次按鈕才能將它刷掉)
      lcd.clear(); // 清空先前的 LCD 面板之訊息
      lcd.setCursor(0, 0); // 表示 LCD 所顯示之位置 (0, 0) 第一行
      lcd.print("Number is run ou"); // 在 LCD 上印出文字 "Number is run ou" ( 由於面板為 16 x 2 第一行只輸出這樣 )
      lcd.setCursor(0, 1); // 表示 LCD 所顯示之位置 (0, 1) 第二行
      lcd.print("t of, Reset num"); // 在 LCD 上印出文字 "t of, Reset num"
      generated_count = 0; // 初始化 generated_count 變數為 0
      for (uint8_t i; i <= 35;i++){ // 使用 for 迴圈迭代, 從 i = 0 遍歷到 i = 35
        generated_numbers[i] = 0; // 初始化陣列值 generated_numbers[i] (0 ~ 35)
      }
      delay(1000); // 延時 1 秒繼續
      normal_init();
    }
    do {
      x = TrueRandom.random(1, 36); // 進行隨機亂數抽選
    } while (check_object(x)); // 放入函數 check_object(x) 判斷該亂數是否出現在 generated_numbers[] 陣列當中, 有的話就重新抽直到不重複為止
    dfmp3.playMp3FolderTrack(x); 
    generated_numbers[generated_count] = x; // 將亂數產生的數字放入已生成的亂數陣列 generated_numbers[] 當中
    generated_count++; // generated_count ++, 遞增 1 表示又多一個不重複亂數

    countingSort(generated_numbers, generated_count - 1);

    startup_init(x); // 呼叫 startup_init(x) 函數, 進行訊息初始化
    delay(250); // 延時 0.3 秒後繼續
  }

  if (digitalRead(MUTIPLE) == LOW) { // 判斷如果讀取 MUTIPLE 腳位的電位是低電位的話則進行多號抽選
    if (switch_once == true){
      lcd.clear();
      generated_count = 0;
      i=0;
      switch_once = false;
    }
    do {
      x = TrueRandom.random(1, 36); // 進行隨機亂數抽選
    } while (check_object(x)); // 放入函數 check_object(x) 判斷該亂數是否出現在 generated_numbers[] 陣列當中, 有的話就重新抽直到不重複為止
    dfmp3.playMp3FolderTrack(x); 
    generated_numbers[generated_count] = x; // 將亂數產生的數字放入已生成的亂數陣列 generated_numbers[] 當中
    generated_count++; // generated_count ++, 遞增 1 表示又多一個不重複亂數

    countingSort(generated_numbers, generated_count - 1);

    lcd.setCursor(0, 0);
    lcd.print("What you drew is");
    lcd.setCursor(i, 1);
    if (len(x)==1){
      i=i+2;
    }
    else if (len(x)==2){
      i=i+3;
    }
    if (i > 18){
      button_init();
    }
    lcd.print(x);
    delay(450);
  }

  if (digitalRead(CLEAR) == LOW) {
    button_init();
  }
}

bool check_object(uint8_t num) {
  // 內部搜尋使用二分搜尋演算法 : 因為 Arduino 本身效能使用線性搜尋會當機
  uint8_t left = 0;
  uint8_t right = generated_count - 1;
  
  while (left <= right) {
    uint8_t mid = left + (right - left) / 2;
    
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

// 計數排序法
void countingSort(uint8_t arr[], uint8_t n) {
    uint8_t max_val = 35; // 資料的範圍為1到35
    uint8_t count[max_val + 1] = {0}; // 初始化計數陣列

    // 計算每個元素的出現次數
    for (uint8_t i = 0; i < n; ++i) {
        count[arr[i]]++;
    }

    // 根據計數陣列來排序
    uint8_t index = 0;
    for (uint8_t i = 1; i <= max_val; ++i) {
        while (count[i] > 0) {
            arr[index++] = i;
            count[i]--;
        }
    }
}
