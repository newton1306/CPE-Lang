# 📖 Walkthrough — คู่มือการทำงานทีม CPE Language

โฟลเดอร์นี้เป็นคู่มือสำหรับทุกคนในทีม อ่านตามลำดับนี้:

---

## 📚 สำหรับทุกคน — อ่านก่อนเริ่มงาน

| ลำดับ | ไฟล์ | เนื้อหา |
|-------|------|---------|
| 1 | `README.md` (ที่ root) | ภาพรวมภาษา CPE + Syntax Guide |
| 2 | `ARCHITECTURE.md` (ที่ root) | สถาปัตยกรรมทั้งหมด — Lexer, Parser, Env, Eval |
| 3 | `CONTRIBUTING.md` (ที่ root) | กฎ Branch, Commit, PR, สิทธิ์แก้ไขไฟล์ |

---

## 👤 สำหรับแต่ละคน — อ่านไฟล์ของตัวเอง

| ไฟล์ในโฟลเดอร์นี้ | สำหรับใคร | งานอะไร |
|-------------------|-----------|---------|
| [1_SHOGUN_LEXER.md](./1_SHOGUN_LEXER.md) | SHOGUN | เขียน `src/lexer.c` |
| [2_IKKIW_PARSER.md](./2_IKKIW_PARSER.md) | IKKIW | เขียน `src/parser.c` |
| [3_JEEFOON_ENVIRONMENT.md](./3_JEEFOON_ENVIRONMENT.md) | JEEFOON | เขียน `src/env.c` |
| [4_GEAR_EVALUATOR.md](./4_GEAR_EVALUATOR.md) | GEAR | เขียน `src/eval.c` |
| [5_NORTH_TESTING.md](./5_NORTH_TESTING.md) | NORTH | เขียน `examples/tests/*.cpe` |

---

## 🔄 ขั้นตอนการทำงาน (ทุกคนทำเหมือนกัน)

### ขั้นตอนที่ 1: Clone โปรเจกต์

เปิด Terminal (Command Prompt / PowerShell / Git Bash) แล้วพิมพ์:

```bash
git clone <url ของ repo>
```

> 💡 **url ของ repo** คือลิงก์ที่ Lead ส่งให้ เช่น `https://github.com/username/CPE-Lang.git`
> วิธีหา: เข้า GitHub repo → คลิกปุ่มสีเขียว **"Code"** → copy URL

เสร็จแล้วเข้าไปในโฟลเดอร์:

```bash
cd CPE-Lang
```

---

### ขั้นตอนที่ 2: สร้าง Branch ของตัวเอง

```bash
git checkout develop
git checkout -b feature/<ชื่อ branch ของตัวเอง>
```

แต่ละคนใช้ชื่อ branch ต่างกัน:

| คุณเป็น | ชื่อ branch |
|---------|------------|
| Lexer Developer | `feature/lexer` |
| Parser Developer | `feature/parser` |
| Environment Developer | `feature/environment` |
| Evaluator Developer | `feature/evaluator` |
| Testing Developer | `feature/testing` |

**ตัวอย่าง** (ถ้าคุณเป็น Lexer Developer):

```bash
git checkout develop
git checkout -b feature/lexer
```

> 💡 **`git checkout -b`** แปลว่า "สร้าง branch ใหม่แล้วย้ายเข้าไป" — เหมือนเปิดห้องทำงานส่วนตัว ที่ทำอะไรก็ได้ โดยไม่กระทบคนอื่น

---

### ขั้นตอนที่ 3: เขียนโค้ด

เปิดไฟล์ที่ assign ให้ (ใช้ VS Code หรือ editor อะไรก็ได้):

```bash
code .
```

เปิดไฟล์ที่ต้องแก้ → อ่าน comment/TODO ในไฟล์ → เขียนโค้ด

---

### ขั้นตอนที่ 4: บันทึกงาน (Commit)

เมื่อเขียนโค้ดเสร็จ (หรือเสร็จบางส่วน) ให้บันทึกลง git:

```bash
git add .
git commit -m "feat(lexer): implement lexer_tokenize function"
```

> 💡 **Commit message** ต้องเป็นรูปแบบนี้:
>
> ```
> <type>(<module>): <อธิบายว่าทำอะไร>
> ```
>
> ตัวอย่าง:
>
> - `feat(lexer): implement keyword table matching`
> - `feat(parser): add recursive descent for if-else`
> - `feat(env): implement hash table with djb2`
> - `fix(eval): fix scope chain lookup`
> - `test(tests): add nested while loop test case`

---

### ขั้นตอนที่ 5: Push ขึ้น GitHub

```bash
git push origin feature/<ชื่อ branch ของตัวเอง>
```

**ตัวอย่าง** (ถ้าคุณเป็น Lexer Developer):

```bash
git push origin feature/lexer
```

ถ้าเป็นครั้งแรกอาจต้องตั้ง upstream:

```bash
git push -u origin feature/lexer
```

---

### ขั้นตอนที่ 6: สร้าง Pull Request (PR)

1. เปิด GitHub repo ในเบราว์เซอร์
2. จะเห็นแถบสีเหลืองเขียนว่า **"feature/lexer had recent pushes"** → คลิก **"Compare & pull request"**
3. ตรวจสอบว่า:
   - **base:** `develop` (ไม่ใช่ main!)
   - **compare:** `feature/lexer` (branch ของคุณ)
4. กรอกข้อมูลตาม template ที่ขึ้นมา
5. คลิก **"Create pull request"**
6. **รอ Lead review** → ถ้ามีอะไรต้องแก้ Lead จะ comment บอก → แก้แล้ว push ใหม่

---

### ⚠️ กฎสำคัญที่ทุกคนต้องจำ

| กฎ | รายละเอียด |
|----|-----------|
| ❌ ห้ามแก้ไฟล์ `.h` | Header files ถูก freeze แล้ว ถ้าอยากเปลี่ยน → แจ้ง Lead |
| ❌ ห้ามแก้ไฟล์คนอื่น | แก้ได้เฉพาะไฟล์ที่ assign ให้เท่านั้น |
| ❌ ห้าม push ตรงไป `main` หรือ `develop` | ต้อง push ไป branch ตัวเอง → สร้าง PR → Lead review |
| ✅ Commit message ตามรูปแบบ | `feat(module): description` |
| ✅ ทดสอบก่อน push | compile ด้วย `gcc src/*.c -o build/cpelang.exe` ให้ผ่านก่อน |

---

## 📁 โครงสร้างโปรเจกต์ (เพื่อให้เข้าใจภาพรวม)

```
CPE-Lang/
├── src/                     ← ซอร์สโค้ดของ interpreter
│   ├── main.c               ← Lead ดูแล (พร้อมแล้ว)
│   ├── lexer.h              ← Interface — ห้ามแก้!
│   ├── lexer.c              ← 🟡 เพื่อนคนที่ 1 เขียน
│   ├── parser.h             ← Interface — ห้ามแก้!
│   ├── parser.c             ← 🟡 เพื่อนคนที่ 2 เขียน
│   ├── env.h                ← Interface — ห้ามแก้!
│   ├── env.c                ← 🟡 เพื่อนคนที่ 3 เขียน
│   ├── eval.h               ← Interface — ห้ามแก้!
│   └── eval.c               ← 🟡 เพื่อนคนที่ 4 เขียน
├── examples/
│   ├── demo.cpe             ← โปรแกรมตัวอย่าง (อ่านเป็นตัวอย่าง)
│   ├── demo2.cpe            ← โปรแกรมตัวอย่างที่ซับซ้อน
│   └── tests/               ← 🟡 เพื่อนคนที่ 5 เขียน
│       ├── edge_empty.cpe
│       ├── edge_comments_only.cpe
│       ├── edge_nested_if.cpe
│       ├── edge_nested_while.cpe
│       ├── edge_long_string.cpe
│       ├── edge_many_vars.cpe
│       └── edge_arithmetic.cpe
├── walkthrough/             ← 📖 คู่มือ (โฟลเดอร์นี้)
├── ARCHITECTURE.md          ← สถาปัตยกรรมเชิงลึก
├── CONTRIBUTING.md          ← กฎการทำงาน
├── ISSUES.md                ← Issue สำเร็จรูปสำหรับ Lead
└── README.md                ← คู่มือผู้ใช้
```
