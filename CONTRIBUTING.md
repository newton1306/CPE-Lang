# 🤝 Contributing Guide — CPE Language

คู่มือสำหรับสมาชิกทีม 6 คนในการทำงานร่วมกันผ่าน GitHub

---

## 📋 สิ่งที่ต้องทำก่อนเริ่ม

1. **Clone repo:**
   ```bash
   git clone <repo-url>
   cd CPE-Lang
   ```

2. **สร้าง branch ของตัวเอง:**
   ```bash
   git checkout develop
   git checkout -b feature/<module-name>
   ```

3. **ตรวจสอบว่า compile ได้:**
   ```bash
   gcc src/main.c src/lexer.c src/parser.c src/env.c src/eval.c -o build/cpelang.exe
   ```

---

## 🔀 Branch Strategy

```
main (protected — release only)
 └── develop (protected — integration branch)
      ├── feature/lexer        ← Lexer Developer
      ├── feature/parser       ← Parser Developer
      ├── feature/environment  ← Environment Developer
      ├── feature/evaluator    ← Evaluator Developer
      └── feature/docs-testing ← Docs & Testing Developer
```

**กฎ:**
- ❌ ห้าม push ตรงไปที่ `main` หรือ `develop`
- ✅ ทำงานใน feature branch → ส่ง PR → Lead review → merge เข้า `develop`
- ✅ `main` จะ merge จาก `develop` เมื่อ integration test ผ่านเท่านั้น

---

## 📝 Commit Message Format

```
<type>(<module>): <description>
```

| Type | ใช้เมื่อ |
|------|---------|
| `feat` | เพิ่มฟีเจอร์ใหม่ |
| `fix` | แก้ bug |
| `docs` | แก้เอกสาร |
| `test` | เพิ่ม/แก้ test |
| `refactor` | ปรับโครงสร้างโค้ด (ไม่เปลี่ยน behavior) |
| `style` | จัดรูปแบบโค้ด (ไม่เปลี่ยน logic) |

**ตัวอย่าง:**
```
feat(lexer): implement keyword table for action syntax
fix(env): fix memory leak in separate chaining cleanup
docs(readme): add compilation instructions for Windows
test(examples): add nested while loop edge case
```

---

## 📁 สิทธิ์การแก้ไขไฟล์

| สมาชิก | แก้ไขได้ | ห้ามแตะ |
|---------|----------|---------|
| Lead (Core) | `main.c`, ทุก `.h` | — |
| Lexer Dev | `lexer.c` | `parser.*`, `env.*`, `eval.*`, `main.c` |
| Parser Dev | `parser.c` | `lexer.*`, `env.*`, `eval.*`, `main.c` |
| Environment Dev | `env.c` | `lexer.*`, `parser.*`, `eval.*`, `main.c` |
| Evaluator Dev | `eval.c` | `lexer.*`, `parser.*`, `env.*`, `main.c` |
| Docs/Test Dev | `.md`, `examples/`, `.vscode/`, CI | `src/*.c`, `src/*.h` |

> ⚠️ **ห้ามแก้ไข `.h` files!** Header files ถูก freeze โดย Lead เป็น Interface Contract ถ้าต้องการเปลี่ยน → แจ้ง Lead ก่อนเท่านั้น

---

## 🔄 Pull Request Workflow

### 1. Sync กับ develop ล่าสุด
```bash
git checkout develop
git pull origin develop
git checkout feature/<your-branch>
git merge develop
```

### 2. ทดสอบก่อน push
```bash
# Compile
gcc -Wall -Wextra src/main.c src/lexer.c src/parser.c src/env.c src/eval.c -o build/cpelang.exe

# Test
./build/cpelang.exe examples/demo.cpe
./build/cpelang.exe examples/demo2.cpe
```

### 3. Push และสร้าง PR
```bash
git push origin feature/<your-branch>
```
จากนั้นเข้า GitHub → Create Pull Request → เลือก base: `develop`

### 4. กรอก PR Template
- อธิบายสิ่งที่ทำ
- ลิสต์ไฟล์ที่แก้
- ✅ Checklist ครบทุกข้อ
- แนบ output ผลทดสอบ

### 5. รอ Lead Review
Lead จะตรวจสอบ:
- File scope ถูกต้องไหม
- API ตรงกับ `.h` ไหม
- Compile + test ผ่านไหม
- Memory management ถูกต้องไหม

---

## 🧪 การทดสอบ

### ไฟล์ test ที่มี:
```
examples/
├── demo.cpe              ← ฟีเจอร์หลักทั้งหมด
├── demo2.cpe             ← Collatz + ASCII Pyramid
└── tests/
    ├── edge_empty.cpe         ← ไฟล์ว่าง
    ├── edge_comments_only.cpe ← มีแค่ comment
    ├── edge_nested_if.cpe     ← if ซ้อน if
    ├── edge_nested_while.cpe  ← while ซ้อน while
    ├── edge_long_string.cpe   ← string ยาวๆ
    ├── edge_many_vars.cpe     ← ตัวแปรจำนวนมาก
    └── edge_arithmetic.cpe    ← การคำนวณกรณีพิเศษ
```

### รัน test ทั้งหมด:
```bash
# Windows PowerShell
Get-ChildItem examples/tests/*.cpe | ForEach-Object { 
    Write-Host "--- Testing: $($_.Name) ---"
    ./build/cpelang.exe $_.FullName 
}

# Bash/Linux
for f in examples/tests/*.cpe; do echo "--- $f ---"; ./build/cpelang "$f"; done
```

---

## ❓ มีปัญหา?

1. **Compile error:** ตรวจสอบว่าแก้เฉพาะไฟล์ที่อนุญาต
2. **Merge conflict:** `git merge develop` แล้วแก้ conflict → commit → push
3. **API ไม่ตรง:** ตรวจสอบ `.h` files → ถ้าต้องเปลี่ยน แจ้ง Lead
4. **ไม่แน่ใจ:** ถามใน GitHub Issue/Discussion ก่อนทำ
