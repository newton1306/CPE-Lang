# 🟤 คู่มือสำหรับ NORTH — Testing Developer

## 📋 งานของคุณคือ: เขียน test cases ใน `examples/tests/*.cpe`

งานนี้ **ไม่ต้องเขียน C** — คุณเขียน **ภาษา CPE** (ภาษาที่ทีมเรากำลังสร้าง interpreter ให้)

คุณต้องเขียนโปรแกรมภาษา CPE เพื่อ **ทดสอบว่า interpreter ทำงานถูกต้อง** ในกรณีต่างๆ

---

## 📖 ไฟล์ที่ต้องอ่าน

### 1. `README.md` ส่วน "Syntax Guide" (สำคัญมาก!)
อ่านให้รู้ว่าภาษา CPE เขียนยังไง:

```
set x as int to 10          ← ประกาศตัวแปร int
set name as string to "hi"  ← ประกาศตัวแปร string
set x to x + 1              ← กำหนดค่าใหม่
show x                       ← แสดงผล
if x > 5 then ... end       ← เงื่อนไข
if x > 5 then ... else ... end  ← เงื่อนไข + else
while x < 10 do ... end     ← วนรอบ
// comment                   ← comment (ถูกข้าม)
```

### 2. `examples/demo.cpe`
ดูโปรแกรมตัวอย่างจริง — มีทุกฟีเจอร์ (ตัวแปร, คำนวณ, if-else, while, string)

### 3. `examples/demo2.cpe`
ดูโปรแกรมที่ซับซ้อนขึ้น — nested loops, string building, Collatz conjecture

### 4. `examples/tests/` (ทุกไฟล์!)
เปิดทุกไฟล์ → อ่าน **"Requirement"** ที่เขียนไว้ใน comment → เขียนโค้ดตาม spec

---

## 🔀 วิธีเริ่มงาน

```bash
git checkout develop
git checkout -b feature/testing
code .
# เปิดไฟล์ใน examples/tests/ → เขียนโค้ด
```

---

## 📁 ไฟล์ที่แก้ได้

| แก้ได้ ✅ | ห้ามแก้ ❌ |
|----------|----------|
| `examples/tests/edge_empty.cpe` | ทุกไฟล์ใน `src/` |
| `examples/tests/edge_comments_only.cpe` | |
| `examples/tests/edge_nested_if.cpe` | |
| `examples/tests/edge_nested_while.cpe` | |
| `examples/tests/edge_long_string.cpe` | |
| `examples/tests/edge_many_vars.cpe` | |
| `examples/tests/edge_arithmetic.cpe` | |

---

## 🛠️ วิธีเขียน test case

### กฎสำคัญ:
1. อ่าน **Requirement** ใน comment ของแต่ละไฟล์
2. เขียน **โค้ดภาษา CPE** ที่ทดสอบตาม requirement
3. ระบุ **Expected Output** ไว้ใน comment (บอกว่าผลลัพธ์ที่ถูกต้องคืออะไร)
4. ลบ comment ที่เขียนว่า "TODO" ออก

---

## 📝 ตัวอย่างการเขียน test case

### ตัวอย่าง 1: `edge_empty.cpe`

**Requirement:** ทดสอบไฟล์ว่าง → ไม่ crash, ไม่มี output

**วิธีเขียน:** ไฟล์นี้ควร **ว่างเปล่า** หรือมีแค่ comment:

```
// Test: Empty file
// Expected Output: (ไม่มี output)
```

---

### ตัวอย่าง 2: `edge_comments_only.cpe`

**Requirement:** ไฟล์มีแค่ comment → ไม่ crash

```
// =============================================
// Test: Comments Only
// Expected Output: (ไม่มี output)
// =============================================

// บรรทัดนี้เป็น comment
// บรรทัดนี้ก็เป็น comment

// ไม่มี statement จริงสักบรรทัด
// interpreter ควรจบโปรแกรมได้ปกติ
```

---

### ตัวอย่าง 3: `edge_nested_if.cpe`

**Requirement:** if ซ้อน if อย่างน้อย 3 ชั้น + if-else ซ้อน

```
// =============================================
// Test: Nested If Statements
// Expected Output:
//   outer: true
//   inner: true
//   deepest: true
//   else branch reached
// =============================================

set a as int to 10
set b as int to 20
set c as int to 30

// if ซ้อน 3 ชั้น
if a < b then
    show "outer: true"
    if b < c then
        show "inner: true"
        if c > 25 then
            show "deepest: true"
        end
    end
end

// if-else ซ้อนใน else
if a > 100 then
    show "should not print"
else
    if b == 20 then
        show "else branch reached"
    end
end
```

---

### ตัวอย่าง 4: `edge_nested_while.cpe`

**Requirement:** while ซ้อน while

```
// =============================================
// Test: Nested While Loops
// Expected Output:
//   1
//   2
//   3
//   2
//   4
//   6
//   3
//   6
//   9
//   done
// =============================================

set i as int to 1

while i < 4 do
    set j as int to 1
    while j < 4 do
        set result as int to i * j
        show result
        set j to j + 1
    end
    set i to i + 1
end

show "done"
```

---

### ตัวอย่าง 5: `edge_long_string.cpe`

**Requirement:** string concatenation หลายรอบ

```
// =============================================
// Test: Long String Concatenation
// Expected Output:
//   abcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabcabc
//   length test passed
// =============================================

set s as string to ""
set count as int to 0

while count < 20 do
    set s to s + "abc"
    set count to count + 1
end

show s
show "length test passed"
```

---

### ตัวอย่าง 6: `edge_many_vars.cpe`

**Requirement:** ประกาศ 20+ ตัวแปร → คำนวณผลรวม

```
// =============================================
// Test: Many Variables (Hash Table Stress)
// Expected Output:
//   55
//   155
//   210
// =============================================

set a as int to 1
set b as int to 2
set c as int to 3
set d as int to 4
set e as int to 5
set f as int to 6
set g as int to 7
set h as int to 8
set i as int to 9
set j as int to 10
set k as int to 11
set l as int to 12
set m as int to 13
set n as int to 14
set o as int to 15
set p as int to 16
set q as int to 17
set r as int to 18
set s as int to 19
set t as int to 20

set total as int to a + b + c + d + e + f + g + h + i + j
set total2 as int to k + l + m + n + o + p + q + r + s + t

show total
show total2

set grand_total as int to total + total2
show grand_total
```

---

### ตัวอย่าง 7: `edge_arithmetic.cpe`

**Requirement:** การคำนวณกรณีพิเศษ

```
// =============================================
// Test: Arithmetic Edge Cases
// Expected Output:
//   0
//   0
//   -5
//   100
//   1
//   complex passed
// =============================================

set zero as int to 0
show zero

set result as int to 42 * 0
show result

set neg as int to 0 - 5
show neg

set paren as int to (2 + 3) * (4 + 16)
show paren

set prec as int to 2 + 3 * 4 - 13
show prec

show "complex passed"
```

---

## ⚠️ หมายเหตุสำคัญ

- คุณ **เขียน test ได้ทันที** (เป็นภาษา CPE ไม่ใช่ C)
- แต่จะ **รัน test จริงได้** ก็ต่อเมื่อ **เพื่อนคนอื่น implement โมดูล C เสร็จแล้ว** (หลัง merge ทุก PR)
- ถ้าอยากรู้ว่า syntax ถูกไหม → ดู `examples/demo.cpe` เป็นตัวอย่าง

---

## ✅ เช็คว่าเสร็จหรือยัง

- [ ] ทุกไฟล์มีโค้ด CPE จริง (ไม่ใช่แค่ comment)
- [ ] ทุกไฟล์ระบุ Expected Output ไว้ใน comment  
- [ ] ไม่ได้แก้ไฟล์อื่นนอกจากที่อนุญาต
- [ ] Commit message: `test(tests): add nested if edge case test`
- [ ] สร้าง PR → base: develop

พร้อมส่ง PR! 🎉

---

## 🤖 ทางเลือกสุดท้าย — ใช้ AI ช่วย

ถ้าทำเองไม่ไหวจริงๆ ให้ copy prompt ด้านล่างนี้ไปวางใน ChatGPT, Claude, Gemini หรือ AI ตัวไหนก็ได้:

```
ฉันกำลังเขียน test cases สำหรับภาษาโปรแกรมมิ่งที่ชื่อ CPE Language
ภาษานี้มี syntax ดังนี้:

- ประกาศตัวแปร: set x as int to 10
- ประกาศ string: set name as string to "hello"
- กำหนดค่าใหม่: set x to x + 1
- แสดงผล: show x
- เงื่อนไข: if x > 5 then ... end
- เงื่อนไข+else: if x > 5 then ... else ... end
- วนรอบ: while x < 10 do ... end
- comment: // comment
- operators: +, -, *, /, >, <, ==, (, )

ตัวอย่างโปรแกรมจริง:

[วางโค้ด examples/demo.cpe ทั้งไฟล์ที่นี่]

ฉันต้องเขียน test cases 7 ไฟล์ แต่ละไฟล์มี requirement ระบุไว้ใน comment:

=== ไฟล์ 1: edge_empty.cpe ===
[วางเนื้อหา examples/tests/edge_empty.cpe ที่นี่]

=== ไฟล์ 2: edge_comments_only.cpe ===
[วางเนื้อหา examples/tests/edge_comments_only.cpe ที่นี่]

=== ไฟล์ 3: edge_nested_if.cpe ===
[วางเนื้อหา examples/tests/edge_nested_if.cpe ที่นี่]

=== ไฟล์ 4: edge_nested_while.cpe ===
[วางเนื้อหา examples/tests/edge_nested_while.cpe ที่นี่]

=== ไฟล์ 5: edge_long_string.cpe ===
[วางเนื้อหา examples/tests/edge_long_string.cpe ที่นี่]

=== ไฟล์ 6: edge_many_vars.cpe ===
[วางเนื้อหา examples/tests/edge_many_vars.cpe ที่นี่]

=== ไฟล์ 7: edge_arithmetic.cpe ===
[วางเนื้อหา examples/tests/edge_arithmetic.cpe ที่นี่]

ช่วยเขียนโค้ดภาษา CPE สำหรับ test case ทั้ง 7 ไฟล์
ตาม requirement ที่ระบุไว้ในแต่ละไฟล์

กฎ:
1. ทุกไฟล์ต้องมี Expected Output ระบุไว้ใน comment
2. ใช้ syntax ภาษา CPE เท่านั้น (ไม่ใช่ C หรือ Python)
3. ดูตัวอย่าง demo.cpe เป็นแนวทาง
4. คืนโค้ดแยกทีละไฟล์ พร้อมบอกว่าแต่ละไฟล์ Expected Output คืออะไร
```
