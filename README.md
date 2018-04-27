# Vacation Down

這是一個在假期結束前兩天和結束後三天嘗試寫的markdown parser和HTML renderer，故稱之Vacation Down，只有做我想用的功能照著CommonMark的spec硬幹，保證有bug

---

## Features

* 分辨blockquote, list, ATX heading, fenced code block, thematic break paragraph
* 以基本UTF8編碼輸出轉換過的HTML，不會轉換HTML特殊字元
* inline element還沒寫
```
hello
```
1. 不同形式的列表
   而且處理了縮進，這行只是要測試用
2. 可以編出這份README
