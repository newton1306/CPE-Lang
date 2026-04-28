# 🔵 คู่มือสำหรับเพื่อนคนที่ 1 — Lexer Developer

## 📋 งานของคุณคือ: เขียน `src/lexer.c`

Lexer คือ **"ตัวตัดคำ"** — รับ source code เป็น string ยาวๆ แล้ว **ตัดออกเป็นคำๆ (Token)** เรียงต่อกันเป็น Linked List

ลองคิดง่ายๆ: ถ้า source code คือ `set x as int to 10` Lexer ก็ตัดเป็น:

```
"set" → "x" → "as" → "int" → "to" → "10"
  ↓       ↓      ↓      ↓       ↓      ↓
 SET   IDENT    AS   INT_TYPE   TO   INT_LIT
```

---

## 📖 ไฟล์ที่ต้องอ่าน (เรียงตามลำดับ)

### 1. `README.md` (ที่ root)
อ่านส่วน **"Syntax Guide"** เพื่อรู้ว่าภาษา CPE มี syntax อะไรบ้าง

### 2. `ARCHITECTURE.md` ส่วน "Phase 2: Lexer"
อ่านเพื่อเข้าใจว่า Lexer ทำงานอย่างไร มี diagram แสดงการเปลี่ยน source code → Token Linked List

### 3. `src/lexer.h` (สำคัญมาก!)
นี่คือ **"สัญญา"** ที่บอกว่าคุณต้อง implement อะไร:
- `TokenType` enum — ประเภทของ token ทั้งหมด (SET, AS, TO, SHOW, ...)
- `Token` struct — โครงสร้างของ token (type, value, line, next)
- `TokenList` struct — Linked List ที่เก็บ tokens (head, tail, count)
- Functions ที่ต้อง implement:
  - `token_list_create()` — สร้าง list ว่าง
  - `token_list_append()` — เพิ่ม token ต่อท้าย
  - `token_list_free()` — ทำลาย list คืน memory
  - `lexer_tokenize()` — ฟังก์ชันหลัก! แปลง source code → TokenList
  - `token_type_name()` — debug utility

### 4. `src/lexer.c` (ไฟล์ที่คุณต้องแก้)
เปิดไฟล์นี้ จะเห็น **TODO 1-5** พร้อมคำอธิบายละเอียด

---

## 🔀 วิธีเริ่มงาน (ทำตามขั้นตอน)

```bash
# 1. Clone repo (ถ้ายังไม่ได้ทำ)
git clone <url>
cd CPE-Lang

# 2. สร้าง branch ของตัวเอง
git checkout develop
git checkout -b feature/lexer

# 3. เปิด VS Code
code .

# 4. เปิดไฟล์ src/lexer.c → เขียนโค้ด
# 5. commit + push
git add .
git commit -m "feat(lexer): implement lexer_tokenize"
git push origin feature/lexer

# 6. เข้า GitHub → สร้าง Pull Request → base: develop
```

---

## 📁 ไฟล์ที่แก้ได้

| แก้ได้ ✅ | ห้ามแก้ ❌ |
|----------|----------|
| `src/lexer.c` | `src/lexer.h` |
| | `src/parser.*` |
| | `src/env.*` |
| | `src/eval.*` |
| | `src/main.c` |

---

## 🛠️ อธิบาย TODO ทีละตัว

### TODO 1: `token_list_create()` — สร้าง TokenList ว่าง

**ต้องทำอะไร:** malloc TokenList → ตั้ง head=NULL, tail=NULL, count=0 → return

```c
TokenList *token_list_create(void)
{
    TokenList *list = (TokenList *)malloc(sizeof(TokenList));
    if (!list) {
        fprintf(stderr, "[CPE] Error: memory allocation failed\n");
        exit(1);
    }
    list->head  = NULL;
    list->tail  = NULL;
    list->count = 0;
    return list;
}
```

---

### TODO 2: `token_list_append()` — เพิ่ม token ต่อท้าย

**ต้องทำอะไร:**
1. malloc Token ใหม่
2. copy value ด้วย `strdup()` (copy string ไปเก็บบน heap)
3. ถ้า list ว่าง → head = tail = token ใหม่
4. ถ้า list ไม่ว่าง → tail->next = token ใหม่ → tail = token ใหม่
5. count++

```c
void token_list_append(TokenList *list, TokenType type,
                       const char *value, int line)
{
    Token *tok = (Token *)malloc(sizeof(Token));
    tok->type  = type;
    tok->value = strdup(value);
    tok->line  = line;
    tok->next  = NULL;

    if (!list->head) {
        list->head = tok;
        list->tail = tok;
    } else {
        list->tail->next = tok;
        list->tail = tok;
    }
    list->count++;
}
```

---

### TODO 3: `token_list_free()` — ทำลาย TokenList

**ต้องทำอะไร:** ไล่ free ทุก token (free value → free node) → free ตัว list

```c
void token_list_free(TokenList *list)
{
    if (!list) return;
    Token *cur = list->head;
    while (cur) {
        Token *next = cur->next;
        free(cur->value);
        free(cur);
        cur = next;
    }
    free(list);
}
```

---

### TODO 4: `token_type_name()` — แปลง TokenType → string

**ต้องทำอะไร:** ใช้ switch-case แปลง enum เป็นชื่อ

```c
const char *token_type_name(TokenType type)
{
    switch (type) {
        case TOKEN_INT_LIT:      return "INT_LIT";
        case TOKEN_STRING_LIT:   return "STRING_LIT";
        case TOKEN_IDENT:        return "IDENT";
        case TOKEN_SET:          return "SET";
        case TOKEN_AS:           return "AS";
        case TOKEN_TO:           return "TO";
        case TOKEN_SHOW:         return "SHOW";
        case TOKEN_IF:           return "IF";
        case TOKEN_ELSE:         return "ELSE";
        case TOKEN_WHILE:        return "WHILE";
        case TOKEN_THEN:         return "THEN";
        case TOKEN_DO:           return "DO";
        case TOKEN_END:          return "END";
        case TOKEN_INT_TYPE:     return "INT_TYPE";
        case TOKEN_STRING_TYPE:  return "STRING_TYPE";
        case TOKEN_PLUS:         return "PLUS";
        case TOKEN_MINUS:        return "MINUS";
        case TOKEN_STAR:         return "STAR";
        case TOKEN_SLASH:        return "SLASH";
        case TOKEN_EQ:           return "EQ";
        case TOKEN_GT:           return "GT";
        case TOKEN_LT:           return "LT";
        case TOKEN_LPAREN:       return "LPAREN";
        case TOKEN_RPAREN:       return "RPAREN";
        case TOKEN_NEWLINE:      return "NEWLINE";
        case TOKEN_EOF:          return "EOF";
        default:                 return "UNKNOWN";
    }
}
```

---

### TODO 5: `lexer_tokenize()` — ฟังก์ชันหลัก!

**นี่คืองานหลักของคุณ** — ตัว Lexer ที่แท้จริง

**ต้องทำอะไร:**
1. สร้าง TokenList ว่าง
2. ใช้ pointer `pos` ชี้ไปที่ตัวอักษรปัจจุบันใน source code
3. วนลูป `while (*pos != '\0')` ดูทีละตัวอักษร:
   - **space, tab, `\r`:** ข้าม → `pos++`
   - **`\n` (newline):** สร้าง TOKEN_NEWLINE (แต่ข้ามถ้าตัวก่อนหน้าก็เป็น NEWLINE)
   - **`//` (comment):** ข้ามจนจบบรรทัด
   - **ตัวเลข (0-9):** ไล่เก็บจนหมดตัวเลข → สร้าง TOKEN_INT_LIT
   - **`"` (string):** ไล่เก็บจนเจอ `"` ปิด → สร้าง TOKEN_STRING_LIT
   - **ตัวอักษร (a-z, A-Z, _):** ไล่เก็บจนหมด → เช็คว่าเป็น keyword ไหม:
     - `set` → TOKEN_SET
     - `as` → TOKEN_AS
     - `to` → TOKEN_TO
     - `show` → TOKEN_SHOW
     - `if` → TOKEN_IF
     - `else` → TOKEN_ELSE
     - `while` → TOKEN_WHILE
     - `then` → TOKEN_THEN
     - `do` → TOKEN_DO
     - `end` → TOKEN_END
     - `int` → TOKEN_INT_TYPE
     - `string` → TOKEN_STRING_TYPE
     - **อื่นๆ** → TOKEN_IDENT (ชื่อตัวแปร)
   - **`=` ตามด้วย `=`:** สร้าง TOKEN_EQ (`==`)
   - **`+`, `-`, `*`, `/`, `>`, `<`, `(`, `)`:** สร้าง token ตามตัว
4. ปิดท้ายด้วย TOKEN_EOF
5. return TokenList

**ตัวอย่างบางส่วน** (ช่วยให้เห็นภาพ):

```c
TokenList *lexer_tokenize(const char *source)
{
    TokenList *list = token_list_create();
    const char *pos = source;
    int line = 1;

    while (*pos != '\0') {

        /* ข้าม space และ tab */
        if (*pos == ' ' || *pos == '\t' || *pos == '\r') {
            pos++;
            continue;
        }

        /* Newline */
        if (*pos == '\n') {
            /* สร้าง NEWLINE แค่ถ้า token ก่อนหน้าไม่ใช่ NEWLINE */
            if (list->count == 0 || list->tail->type != TOKEN_NEWLINE) {
                token_list_append(list, TOKEN_NEWLINE, "\\n", line);
            }
            line++;
            pos++;
            continue;
        }

        /* Comment: // ข้ามจนจบบรรทัด */
        if (*pos == '/' && *(pos + 1) == '/') {
            while (*pos && *pos != '\n') pos++;
            continue;  /* ไม่ต้อง pos++ เพราะ \n จะถูกจัดการในรอบถัดไป */
        }

        /* ตัวเลข */
        if (isdigit(*pos)) {
            const char *start = pos;
            while (isdigit(*pos)) pos++;
            /* copy ตัวเลขออกมา */
            int len = pos - start;
            char *num = (char *)malloc(len + 1);
            strncpy(num, start, len);
            num[len] = '\0';
            token_list_append(list, TOKEN_INT_LIT, num, line);
            free(num);
            continue;
        }

        /* TODO: ใส่ส่วนที่เหลือ — string, identifier/keyword, operators */
        /* ... */
    }

    token_list_append(list, TOKEN_EOF, "EOF", line);
    return list;
}
```

> 💡 **ดูโค้ดเต็มใน `ARCHITECTURE.md`** ส่วน Lexer เพื่อเข้าใจ flow ทั้งหมด

---

## ✅ เช็คว่าเสร็จหรือยัง

ลอง compile:
```bash
gcc -Wall -Wextra src/main.c src/lexer.c src/parser.c src/env.c src/eval.c -o build/cpelang.exe
```

ถ้า compile ผ่านไม่มี error → พร้อมส่ง PR! 🎉
