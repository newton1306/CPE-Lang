# 📋 GitHub Issues — สร้างให้ทีมแต่ละคน

คัดลอก Issue เหล่านี้ไปสร้างบน GitHub (Settings → Issues → New Issue)
Lead ควรสร้าง 6 Issues ในวันแรกแล้ว assign ให้แต่ละคน

---

## Issue #1: [Core] Setup Repository & Integration

**Assignee:** Lead  
**Labels:** `core`, `setup`  
**Branch:** `main`, `develop`

### 📝 สิ่งที่ต้องทำ
- [ ] สร้าง GitHub Repository
- [ ] Push initial commit (โค้ดปัจจุบันทั้งหมด)
- [ ] สร้าง `develop` branch จาก `main`
- [ ] ตั้ง Branch Protection Rules (main + develop)
- [ ] สร้าง `.gitignore`
- [ ] Invite collaborators ทั้ง 5 คน
- [ ] ประกาศ freeze `.h` files (Interface Contract)
- [ ] สร้าง Issues #2-#6 แล้ว assign
- [ ] Review & merge ทุก PR ตามลำดับ
- [ ] Integration test: compile + run demo.cpe + demo2.cpe
- [ ] Release: merge develop → main, tag v1.0

### 🔗 Dependencies
ไม่มี — ทำก่อนทุกคน

---

## Issue #2: [Lexer] Implement lexer.c — Tokenizer

**Assignee:** Lexer Developer  
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
- [ ] `lexer_tokenize("set x as int to 10\nshow x\n")` ได้ tokens ที่ถูกต้อง
- [ ] `gcc -Wall -Wextra src/*.c -o build/cpelang.exe` ไม่มี warning
- [ ] `demo.cpe` + `demo2.cpe` รันผ่าน
- [ ] ส่ง PR → Lead review ผ่าน

### 🔗 Dependencies
❌ ไม่มี — ทำได้ทันที

---

## Issue #3: [Parser] Implement parser.c — AST Builder

**Assignee:** Parser Developer  
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
- [ ] Handle `TOKEN_NEWLINE` ข้ามระหว่าง statements

### ✅ Definition of Done
- [ ] Parse `demo.cpe` ได้ AST ที่ถูกต้อง
- [ ] `gcc -Wall -Wextra src/*.c -o build/cpelang.exe` ไม่มี warning
- [ ] `demo.cpe` + `demo2.cpe` รันผ่าน
- [ ] ส่ง PR → Lead review ผ่าน

### 🔗 Dependencies
✅ ต้องรอ `lexer.h` ถูก freeze (TokenType enum + Token/TokenList struct)

---

## Issue #4: [Environment] Implement env.c — Symbol Table

**Assignee:** Environment Developer  
**Labels:** `feature`, `environment`  
**Branch:** `feature/environment`

### 📝 สิ่งที่ต้องทำ
- [ ] Implement djb2 hash function — `hash = 5381; hash = hash * 33 + c; return hash % 64`
- [ ] Implement `env_create()` — สร้าง Env ใหม่ (64 buckets, parent pointer)
- [ ] Implement `env_destroy()` — ทำลาย Hash Table คืน memory (ไม่ทำลาย parent)
- [ ] Implement `env_set()` — Insert/update ตัวแปรใน scope ปัจจุบัน
- [ ] Implement `env_get()` — ค้นหาตัวแปรตาม scope chain (local → parent → ... → NULL)
- [ ] Implement `value_int()` — สร้าง Value ชนิด int
- [ ] Implement `value_string()` — สร้าง Value ชนิด string (strdup)
- [ ] Implement `value_free()` — คืน memory ของ string value
- [ ] Handle Separate Chaining — collision resolution ด้วย linked list ที่แต่ละ bucket

### ✅ Definition of Done
- [ ] `env_set` + `env_get` ทำงานถูกต้อง (insert, update, lookup)
- [ ] Scope chain: child env ค้นหาตัวแปรจาก parent ได้
- [ ] ไม่มี memory leak
- [ ] `gcc -Wall -Wextra src/*.c -o build/cpelang.exe` ไม่มี warning
- [ ] ส่ง PR → Lead review ผ่าน

### 🔗 Dependencies
❌ ไม่มี — ทำได้ทันที

---

## Issue #5: [Evaluator] Implement eval.c — AST Executor

**Assignee:** Evaluator Developer  
**Labels:** `feature`, `evaluator`  
**Branch:** `feature/evaluator`

### 📝 สิ่งที่ต้องทำ
- [ ] Implement `eval()` — main switch ตาม ASTNodeType
  - [ ] `AST_INT_LIT` → `atoi(node->value)` → `value_int()`
  - [ ] `AST_STRING_LIT` → `value_string(node->value)`
  - [ ] `AST_IDENT` → `env_get()` → return ค่า
  - [ ] `AST_BINARY_OP` → eval left + right → คำนวณ (+,-,*,/,>,<,==)
  - [ ] `AST_VAR_DECL` → eval init value → ตรวจ type → `env_set()`
  - [ ] `AST_ASSIGN` → `env_get()` → update in-place ผ่าน pointer
  - [ ] `AST_IF` → eval condition → truthy? then : else
  - [ ] `AST_WHILE` → loop eval condition → truthy? body : break
  - [ ] `AST_PRINT` → eval expr → `printf()`
  - [ ] `AST_PROGRAM` / `AST_BLOCK` → eval ทุก children ตามลำดับ
- [ ] Implement `eval_block()` — สร้าง child Env → eval → destroy child Env
- [ ] Handle string concatenation ด้วย `+`
- [ ] Handle type checking (int vs string)
- [ ] ⚠️ Assignment ใน child scope ต้อง update in-place (ไม่ใช้ env_set ใน child!)

### ✅ Definition of Done
- [ ] `demo.cpe` ได้ output ตรงตาม expected
- [ ] `demo2.cpe` (Collatz + Pyramid) ได้ output ถูกต้อง
- [ ] Edge case tests ทั้งหมดผ่าน
- [ ] ไม่มี memory leak
- [ ] `gcc -Wall -Wextra src/*.c -o build/cpelang.exe` ไม่มี warning
- [ ] ส่ง PR → Lead review ผ่าน

### 🔗 Dependencies
✅ ต้องรอ `parser.h` (ASTNode struct) + `env.h` (Value/Env struct) ถูก freeze

### ⚠️ จุดที่ต้องระวังมาก
```c
// ❌ ผิด: สร้าง entry ใหม่ใน child scope → shadow ตัวแปรจาก parent
env_set(child_env, "x", new_value);

// ✅ ถูก: ค้นหาแล้ว update in-place
Value *existing = env_get(env, "x");
*existing = new_value;
```

---

## Issue #6: [Docs/Testing] Documentation, Tests & CI/CD

**Assignee:** Docs/Testing Developer  
**Labels:** `docs`, `testing`, `CI`  
**Branch:** `feature/docs-testing`

### 📝 สิ่งที่ต้องทำ
- [ ] ตรวจสอบและอัปเดต `README.md` ให้ตรงกับโค้ด
- [ ] ตรวจสอบและอัปเดต `ARCHITECTURE.md` ให้ตรงกับ implementation
- [ ] ตรวจสอบ edge case test files ใน `examples/tests/`:
  - [ ] `edge_empty.cpe` — ไฟล์ว่าง
  - [ ] `edge_comments_only.cpe` — มีแค่ comment
  - [ ] `edge_nested_if.cpe` — if ซ้อน if
  - [ ] `edge_nested_while.cpe` — while ซ้อน while
  - [ ] `edge_long_string.cpe` — string ยาว
  - [ ] `edge_many_vars.cpe` — ตัวแปรจำนวนมาก
  - [ ] `edge_arithmetic.cpe` — การคำนวณกรณีพิเศษ
- [ ] เพิ่ม test cases ถ้าพบ edge case ใหม่
- [ ] ตรวจสอบ `.vscode/tasks.json` + `settings.json`
- [ ] ตรวจสอบ GitHub Actions CI/CD — `.github/workflows/build.yml`
- [ ] ตรวจสอบ `CONTRIBUTING.md`

### ✅ Definition of Done
- [ ] เอกสารทั้งหมดถูกต้องตรงกับโค้ด
- [ ] Edge case tests รันผ่านทั้งหมด
- [ ] CI/CD pipeline ทำงานได้
- [ ] ส่ง PR → Lead review ผ่าน

### 🔗 Dependencies
❌ ไม่มี — ทำได้ทันที
