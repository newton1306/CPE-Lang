# 🚀 CPE Language v1.0

**CPE** เป็นภาษาโปรแกรมที่ออกแบบด้วย syntax แบบ **"พูดคุย" (Conversational)** ที่อ่านเข้าใจง่ายเหมือนภาษาอังกฤษ สร้างทั้งหมดจากศูนย์ด้วยภาษา C

```
set name as string to "CPE Language"
set count as int to 0

while count < 5 do
    show count
    set count to count + 1
end

show name
```

---

## 📦 สิ่งที่ต้องมี

- **GCC** (MinGW-w64 สำหรับ Windows) — [ดาวน์โหลดที่นี่](https://www.msys2.org/)

ตรวจสอบว่าติดตั้งแล้ว:
```
gcc --version
```

---

## ⚡ เริ่มต้นใช้งาน

### 1. Compile Interpreter (ทำครั้งเดียว)

```bash
cd C:\Users\nsand\Documents\self_product\CPE-Lang
mkdir build
gcc src\main.c src\lexer.c src\parser.c src\env.c src\eval.c -o build\cpelang.exe
```

### 2. รันโปรแกรม .cpe

```bash
.\build\cpelang.exe examples\demo.cpe
```

> 💡 **เมื่อแก้ไขไฟล์ .cpe ไม่ต้อง compile ใหม่** — แค่รันคำสั่งด้านบนซ้ำได้เลย
> ต้อง compile ใหม่เฉพาะเมื่อแก้ไฟล์ `.c` ในโฟลเดอร์ `src/` เท่านั้น

---

## 💻 ใช้งานใน VS Code

1. **ติดตั้ง** extension **"Code Runner"** จาก Marketplace
2. **Build:** กด `Ctrl+Shift+B` → เลือก "Build CPE Interpreter"
3. **Run:** เปิดไฟล์ `.cpe` → กด `Ctrl+Alt+N` หรือกดปุ่ม ▶

---

## 📖 Syntax Guide

### ตัวแปร

```
// ประกาศตัวแปรใหม่
set x as int to 42
set name as string to "Hello"

// กำหนดค่าใหม่
set x to x + 1
set name to "World"
```

### การคำนวณ

```
set a as int to (2 + 3) * 4     // 20
set b as int to 100 / 5 - 10    // 10
```

| ลำดับ | Operators | ความหมาย |
|-------|-----------|----------|
| สูงสุด | `( )` | วงเล็บ |
| ↓ | `*` `/` | คูณ หาร |
| ↓ | `+` `-` | บวก ลบ |
| ต่ำสุด | `>` `<` `==` | เปรียบเทียบ |

### ต่อสตริง

```
set full as string to "Hello" + " " + "World"
```

### แสดงผล

```
show 42
show "Hello"
show x + y
```

### เงื่อนไข (If-Else)

```
if x > 10 then
    show "big"
else
    show "small"
end
```

### วนรอบ (While)

```
set i as int to 0
while i < 5 do
    show i
    set i to i + 1
end
```

### Comment

```
// ข้อความนี้จะถูกข้ามโดย interpreter
set x as int to 10  // comment ท้ายบรรทัดก็ได้
```

---

## 🗂️ Cheat Sheet

| ต้องการทำอะไร | เขียนยังไง |
|--------------|-----------|
| ประกาศ int | `set x as int to 10` |
| ประกาศ string | `set s as string to "hello"` |
| กำหนดค่าใหม่ | `set x to x + 1` |
| แสดงผล | `show x` |
| if | `if x > 5 then ... end` |
| if-else | `if x > 5 then ... else ... end` |
| while | `while x < 10 do ... end` |
| comment | `// text` |

---

## ⚠️ กฎสำคัญ

- **แต่ละ statement ต้องอยู่คนละบรรทัด** (ใช้ขึ้นบรรทัดใหม่แทน `;`)
- ต้อง **ประกาศตัวแปร** ก่อนใช้งาน (`set x as int to ...`)
- block ต้องปิดด้วย `end` เสมอ

---

## 📁 โครงสร้างโปรเจกต์

```
CPE-Lang/
├── src/                 ← source code ของ interpreter (ภาษา C)
│   ├── main.c           ← จุดเริ่มต้น — อ่านไฟล์ .cpe แล้วรัน pipeline
│   ├── lexer.h / .c     ← แปลง source code → Token List
│   ├── parser.h / .c    ← แปลง Token List → AST (Tree)
│   ├── env.h / .c       ← Symbol Table (Hash Table) เก็บตัวแปร
│   └── eval.h / .c      ← เดิน AST แล้วประมวลผล
├── build/
│   └── cpelang.exe      ← ไฟล์ interpreter ที่ compile แล้ว
├── examples/
│   └── demo.cpe         ← โปรแกรมตัวอย่าง
├── .vscode/             ← config สำหรับ VS Code
├── ARCHITECTURE.md      ← รายละเอียดโครงสร้างการทำงานเชิงลึก
└── README.md            ← ไฟล์นี้
```

> 📘 อ่านรายละเอียดเชิงลึกเกี่ยวกับโครงสร้างการทำงานภายในได้ที่ [ARCHITECTURE.md](ARCHITECTURE.md)

---

## 📜 License

This project is open source and available under the MIT License.
