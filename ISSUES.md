# 📋 GitHub Issues — สร้างให้ทีมแต่ละคน

คัดลอก Issue เหล่านี้ไปสร้างบน GitHub (Settings → Issues → New Issue)
Lead ควรสร้าง 6 Issues ในวันแรกแล้ว assign ให้แต่ละคน

---

## การมอบหมายงาน

| คนที่ | บทบาท | ไฟล์ที่รับผิดชอบ | Branch |
|--------|--------|-----------------|--------|
| 1 | **Lead** | `main.c` + ทุก `.h` + integration | `main`, `develop` |
| 2 | **Lexer Developer** | `lexer.c` | `feature/lexer` |
| 3 | **Parser Developer** | `parser.c` | `feature/parser` |
| 4 | **Environment Developer** | `env.c` | `feature/environment` |
| 5 | **Evaluator Developer** | `eval.c` | `feature/evaluator` |
| 6 | **Testing Developer** | `examples/tests/*.cpe` | `feature/testing` |

---

## Issue #1: [Core] Setup Repository & Integration

**Assignee:** Lead (คุณ)
**Labels:** `core`, `setup`
**Branch:** `main`, `develop`

### 📝 สิ่งที่ต้องทำ

- [ ] สร้าง GitHub Repository
- [ ] Push initial commit (โค้ดปัจจุบันทั้งหมด รวม skeleton files)
- [ ] สร้าง `develop` branch จาก `main`
- [ ] ตั้ง Branch Protection Rules (main + develop)
- [ ] Invite collaborators ทั้ง 5 คน
- [ ] สร้าง Issues #2-#6 แล้ว assign
- [ ] Review & merge ทุก PR ตามลำดับ
- [ ] Integration test: compile + run demo.cpe + demo2.cpe
- [ ] Release: merge develop → main, tag v1.0

### 🔗 Dependencies

ไม่มี — ทำก่อนทุกคน

---

## Issue #2: [Lexer] Implement lexer.c — Tokenizer

**Assignee:** เพื่อนคนที่ 1
**Labels:** `feature`, `lexer`
**Branch:** `feature/lexer`

### 📝 สิ่งที่ต้องทำ

- [ ] Implement `token_list_create()` — สร้าง TokenList ว่าง
- [ ] Implement `token_list_append()` — เพิ่ม token ต่อท้าย (O(1) ด้วย tail pointer)
- [ ] Implement `token_list_free()` — ทำลาย Linked List คืน memory
- [ ] Implement `token_type_name()` — debug utility
- [ ] Implement `lexer_tokenize()`:
  - [ ] Scan ตัวเลข → `TOKEN_INT_LIT`
  - [ ] Scan string literal `"..."` → `TOKEN_STRING_LIT`
  - [ ] Scan identifier/keyword → match กับ keyword table
  - [ ] Handle operators: `+`, `-`, `*`, `/`, `==`, `>`, `<`, `(`, `)`
  - [ ] Handle comments: `//` → ข้ามจนจบบรรทัด
  - [ ] Handle newlines: สร้าง `TOKEN_NEWLINE` แต่ข้ามซ้ำ
  - [ ] Handle whitespace: ข้าม space/tab
  - [ ] ปิดท้ายด้วย `TOKEN_EOF`

### ✅ Definition of Done

- [ ] Compile ผ่าน: `gcc -Wall -Wextra src/*.c -o build/cpelang.exe`
- [ ] ส่ง PR → Lead review ผ่าน

### 🔗 Dependencies

❌ ไม่มี — ทำได้ทันที

---

## Issue #3: [Parser] Implement parser.c — AST Builder

**Assignee:** เพื่อนคนที่ 2
**Labels:** `feature`, `parser`
**Branch:** `feature/parser`

### 📝 สิ่งที่ต้องทำ

- [ ] Implement `ast_node_create()` — สร้าง AST node ใหม่
- [ ] Implement `ast_node_add_child()` — เพิ่ม child เข้า parent
- [ ] Implement `ast_node_free()` — ทำลาย AST tree แบบ recursive
- [ ] Implement `parser_parse()` — entry point สร้าง PROGRAM node
- [ ] Parse statements:
  - [ ] `set x as int to <expr>` → `AST_VAR_DECL`
  - [ ] `set x to <expr>` → `AST_ASSIGN`
  - [ ] `show <expr>` → `AST_PRINT`
  - [ ] `if <expr> then ... else ... end` → `AST_IF`
  - [ ] `while <expr> do ... end` → `AST_WHILE`
- [ ] Parse expressions (Operator Precedence):
  - [ ] `parse_comparison()` — `>`, `<`, `==`
  - [ ] `parse_addition()` — `+`, `-`
  - [ ] `parse_multiply()` — `*`, `/`
  - [ ] `parse_primary()` — INT_LIT, STRING_LIT, IDENT, `(expr)`

### ✅ Definition of Done

- [ ] Compile ผ่าน: `gcc -Wall -Wextra src/*.c -o build/cpelang.exe`
- [ ] ส่ง PR → Lead review ผ่าน

### 🔗 Dependencies

✅ ต้องรอ `lexer.h` (TokenType enum) — **แต่ `.h` ถูก freeze แล้ว เริ่มทำได้เลย**

---

## Issue #4: [Environment] Implement env.c — Symbol Table

**Assignee:** เพื่อนคนที่ 3
**Labels:** `feature`, `environment`
**Branch:** `feature/environment`

### 📝 สิ่งที่ต้องทำ

- [ ] Implement djb2 hash function
- [ ] Implement `env_create()` — สร้าง Env ใหม่ (64 buckets, parent pointer)
- [ ] Implement `env_destroy()` — ทำลาย Hash Table คืน memory
- [ ] Implement `env_set()` — Insert/update ตัวแปรใน scope ปัจจุบัน
- [ ] Implement `env_get()` — ค้นหาตัวแปรตาม scope chain
- [ ] Implement `value_int()` — สร้าง Value ชนิด int
- [ ] Implement `value_string()` — สร้าง Value ชนิด string
- [ ] Implement `value_free()` — คืน memory ของ string value

### ✅ Definition of Done

- [ ] Compile ผ่าน: `gcc -Wall -Wextra src/*.c -o build/cpelang.exe`
- [ ] ส่ง PR → Lead review ผ่าน

### 🔗 Dependencies

❌ ไม่มี — ทำได้ทันที

---

## Issue #5: [Evaluator] Implement eval.c — AST Executor

**Assignee:** เพื่อนคนที่ 4
**Labels:** `feature`, `evaluator`
**Branch:** `feature/evaluator`

### 📝 สิ่งที่ต้องทำ

- [ ] Implement `eval()` — main switch ตาม ASTNodeType
  - [ ] `AST_INT_LIT` → `atoi(node->value)` → `value_int()`
  - [ ] `AST_STRING_LIT` → `value_string(node->value)`
  - [ ] `AST_IDENT` → `env_get()` → return ค่า
  - [ ] `AST_BINARY_OP` → eval left + right → คำนวณ
  - [ ] `AST_VAR_DECL` → eval init value → ตรวจ type → `env_set()`
  - [ ] `AST_ASSIGN` → `env_get()` → **update in-place** ผ่าน pointer
  - [ ] `AST_IF` → eval condition → truthy? then : else
  - [ ] `AST_WHILE` → loop eval condition
  - [ ] `AST_PRINT` → eval expr → `printf()`
  - [ ] `AST_PROGRAM` / `AST_BLOCK` → eval ทุก children
- [ ] Implement `eval_block()` — สร้าง child Env → eval → destroy
- [ ] Handle string concatenation ด้วย `+`

### ⚠️ จุดที่ต้องระวัง

```c
// ❌ ผิด: env_set ใน child scope → shadow ตัวแปร → while loop ไม่จบ
env_set(child_env, "x", new_value);

// ✅ ถูก: ค้นหาแล้ว update in-place
Value *existing = env_get(env, "x");
*existing = new_value;
```

### ✅ Definition of Done

- [ ] Compile ผ่าน: `gcc -Wall -Wextra src/*.c -o build/cpelang.exe`
- [ ] `demo.cpe` + `demo2.cpe` ได้ output ถูกต้อง
- [ ] ส่ง PR → Lead review ผ่าน

### 🔗 Dependencies

✅ ต้องรอ `parser.h` + `env.h` — **แต่ `.h` ถูก freeze แล้ว เริ่มทำได้เลย**

---

## Issue #6: [Testing] เขียน Test Cases

**Assignee:** เพื่อนคนที่ 5
**Labels:** `testing`
**Branch:** `feature/testing`

### 📝 สิ่งที่ต้องทำ

เขียนโค้ดภาษา CPE ในไฟล์ test ที่อยู่ใน `examples/tests/` โดยแต่ละไฟล์มี requirement ระบุไว้แล้วในไฟล์:

- [ ] `edge_empty.cpe` — ทดสอบไฟล์ว่าง (ไม่ crash, ไม่มี output)
- [ ] `edge_comments_only.cpe` — ทดสอบไฟล์ที่มีแค่ comment
- [ ] `edge_nested_if.cpe` — ทดสอบ if ซ้อน if (อย่างน้อย 3 ชั้น)
- [ ] `edge_nested_while.cpe` — ทดสอบ while ซ้อน while
- [ ] `edge_long_string.cpe` — ทดสอบ string concatenation หลายรอบ
- [ ] `edge_many_vars.cpe` — ทดสอบตัวแปร 20+ ตัว (hash collision)
- [ ] `edge_arithmetic.cpe` — ทดสอบการคำนวณกรณีพิเศษ (ศูนย์, ค่าลบ, วงเล็บ, precedence)

**กฎสำคัญ:**

- อ่าน requirement ที่เขียนไว้ใน comment ของแต่ละไฟล์
- ทุกไฟล์ต้องระบุ **Expected Output** ไว้ใน comment
- เขียนโค้ด CPE ตาม syntax ที่อยู่ใน `README.md`
- ทดสอบได้ด้วย: `./build/cpelang.exe examples/tests/<filename>.cpe`

**อ้างอิง Syntax:**

```
set x as int to 10          // ประกาศตัวแปร int
set s as string to "hello"  // ประกาศตัวแปร string
set x to x + 1              // กำหนดค่าใหม่
show x                       // แสดงผล
if x > 5 then ... end       // เงื่อนไข
while x < 10 do ... end     // วนรอบ
// comment                   // comment
```

### ✅ Definition of Done

- [ ] ทุกไฟล์ test มีโค้ด CPE ที่ถูกต้อง (ไม่ใช่แค่ comment)
- [ ] ทุกไฟล์ระบุ expected output ไว้ใน comment
- [ ] ทุกไฟล์รันผ่าน interpreter ไม่ crash
- [ ] ส่ง PR → Lead review ผ่าน

### 🔗 Dependencies

⚠️ ต้องรอให้โมดูลอื่นๆ merge เสร็จก่อนจึงจะ **รัน test จริง** ได้ — แต่**เขียน test ได้ทันที** (เพราะเป็นภาษา CPE ไม่ใช่ C)
