# Arduino_Randomly_Drawing_Numbers

I mainly use it to solve problems in my school life. For example, when a teacher needs to draw lots, they use a lottery box. Some students would secretly remove their number tags. To prevent this issue and ensure fair outcomes, I leveraged my programming skills and Arduino technology to address the problem.

# Version（版本歷史）

1.0.0 - No. 1 machine is born.
(初號機誕生!)

1.0.1 - Added continuous drawing mode
(新增連續抽選模式 -> 添加一顆按鈕)

1.0.2 - Added a reset mode to refresh drawing records
(新增重置模式 -> 添加一顆按鈕，且用於刷新抽號紀錄)

1.0.3 - Change the drawing result to non-repetitive drawing results
(將抽號結果更改成不重複抽號)

1.0.4 - Fix the situation where Arduino crashes halfway through use.
(修復使用到一半導致Arduino當機的情形)

1.0.5 - Use ChatGPT to generate more random random seeds and increase randomness.
(用 ChatGPT 生成一個更隨機的隨機種子，增加隨機性)

1.0.6 - Fixed a bug. Pressing the continuous drawing mode first and then the single number mode. Pressing the continuous drawing mode again will cause an error to be displayed.
(修復 bug, 先按連續按鈕再按單號, 再按一次連續會導致顯示錯誤)

1.0.6.5 (sound_only) - Added MP3 module, and updated the program code so that the corresponding number can be loud out when drawing numbers.
(新增MP3模組, 更新程式碼, 抽獎時可以大聲說出對應的數字)

1.0.7 - Replace the full-range drawing with a four-stage drawing. The first four drawing results range as follows: 1`~`10, 10`~`20, 20`~`30, 30`~`38. The fifth time it returns to the full-range drawing (1`~`38)
(將全範圍抽號更換成四段式抽號，前四次抽號結果的範圍如下：1`~`10、10`~`20、20`~`30、30`~`38，第五次時則還原成全範圍抽號（1`~`38）)

# Code Reference（程式碼參考）

Author Jason Chu, his website : https://blog.jmaker.com.tw/arduino-dfplayer-mp3/
(我使用此作者的Mp3模組程式碼)

# Librarys （函式庫）

New-LiquidCrystal : https://github.com/fmalpartida/New-LiquidCrystal/tree/master

DFPlayer Mini Mp3 by Makuna : https://www.arduino.cc/reference/en/libraries/dfplayer-mini-mp3-by-makuna/

# Material (reference) （素材（參考用））

- Arduino Uno x 1
- Button x 3 (For Mode Change x 2, For Clear Record x 1) （給模式切換的 x 2，給清除打卡紀錄的 x 1）
- Arduino 1602 LCD Displayer x 1（1602 LCD 顯示屏幕 x 1）
- BreadBoard x 1
- DFPlayer Mini Mp3 x 1（Mp3 模組 -> sound_version_only）
- Speaker x 1（喇叭 -> sound_version_only）

# Comments Language & Introduction （註解語言）

Comments Language : Traditional Chinese

註解語言：繁體中文

This program is mainly used to solve problems encountered in school life. For instance, when a teacher conducts a draw to ask students questions, he requires a lottery box. However, some students may surreptitiously remove their lottery sticks, leading to an unfair situation.

So, in order to solve such problems, I use Arduino and write programs to address them, which can be considered a practical daily application.

In fact, this set of devices is widely accepted and actively used by many teachers in the classroom. As for the code, you are free to modify and use it as you wish.

本程式主要是用於解決學校生活上的問題，例如像是老師在抽籤問同學問題時，需要一個籤筒，而有些同學會偷偷把自己的籤拿走，這樣就造成了一種不公平。

所以為了解決這樣的問題，我透過 Arduino 與寫程式來解決這樣的問題，算是一種生活上的應用。

實際上這組裝置在班級上也被許多老師廣泛接受並且積極使用，有關於程式碼的部分，你可以隨意修改跟使用。

made by LukeTseng ( 鼓山高中 _ 電腦社社長 ).
