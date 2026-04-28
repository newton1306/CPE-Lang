# 🟣 คู่มือสำหรับเพื่อนคนที่ 2 — Parser Developer

## 📋 งานของคุณคือ: เขียน `src/parser.c`

Parser คือ **"ตัวสร้างต้นไม้"** — รับ Token List จาก Lexer แล้วสร้าง **Abstract Syntax Tree (AST)** ซึ่งเป็นต้นไม้ที่แสดงโครงสร้างของโปรแกรม

ลองคิดง่ายๆ: ถ้า token list คือ `SET, IDENT("x"), AS, INT_TYPE, TO, INT_LIT("2"), PLUS, INT_LIT("3")` Parser จะสร้างต้นไม้:

```
    VAR_DECL (name="x", type="int")
        └── BINARY_OP (op="+")
              ├── INT_LIT ("2")
              └── INT_LIT ("3")
```

---

## 📖 ไฟล์ที่ต้องอ่าน (เรียงตามลำดับ)

### 1. `ARCHITECTURE.md` ส่วน "Phase 3: Parser"
อ่าน Grammar ของภาษา CPE + ดู AST ตัวอย่าง — สำคัญมาก!

### 2. `src/parser.h` (สำคัญมาก!)
ดู `ASTNodeType` enum (AST_PROGRAM, AST_VAR_DECL, AST_IF, ...) และ `ASTNode` struct:
- `type` — ประเภทของ node
- `value` — ค่า literal (สำหรับ INT_LIT, STRING_LIT)
- `name` — ชื่อตัวแปร (สำหรับ VAR_DECL, ASSIGN, IDENT)
- `op` — ตัวดำเนินการ (สำหรับ BINARY_OP)
- `var_type` — ชนิดตัวแปร "int"/"string" (สำหรับ VAR_DECL)
- `children[128]` — child nodes
- `child_count` — จำนวน children

### 3. `src/lexer.h`
ดู `TokenType` enum เพื่อรู้ว่า token มีประเภทอะไรบ้าง (TOKEN_SET, TOKEN_IF, ...)

### 4. `src/parser.c` (ไฟล์ที่คุณต้องแก้)
เปิดไฟล์นี้ จะเห็น **TODO 1-11** พร้อม helper functions ที่เขียนไว้ให้แล้ว: `advance()`, `expect()`, `skip_newlines()`

---

## 🔀 วิธีเริ่มงาน

```bash
git checkout develop
git checkout -b feature/parser
code .
# เปิด src/parser.c → เขียนโค้ด
```

---

## 📁 ไฟล์ที่แก้ได้

| แก้ได้ ✅ | ห้ามแก้ ❌ |
|----------|----------|
| `src/parser.c` | `src/parser.h`, `src/lexer.*`, `src/env.*`, `src/eval.*`, `src/main.c` |

---

## 🛠️ อธิบาย TODO ทีละตัว

### TODO 1: `ast_node_create()` — สร้าง AST Node ใหม่

```c
ASTNode *ast_node_create(ASTNodeType type)
{
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    memset(node, 0, sizeof(ASTNode));
    node->type = type;
    return node;
}
```

### TODO 2: `ast_node_add_child()` — เพิ่ม child เข้า parent

```c
void ast_node_add_child(ASTNode *parent, ASTNode *child)
{
    if (parent->child_count < AST_MAX_CHILDREN) {
        parent->children[parent->child_count++] = child;
    }
}
```

### TODO 3: `ast_node_free()` — ทำลาย tree แบบ recursive

```c
void ast_node_free(ASTNode *node)
{
    if (!node) return;
    for (int i = 0; i < node->child_count; i++) {
        ast_node_free(node->children[i]);
    }
    free(node->value);
    free(node->name);
    free(node->op);
    free(node->var_type);
    free(node);
}
```

### TODO 5: `parse_primary()` — จัดการค่าพื้นฐาน

```c
static ASTNode *parse_expression(void);  /* forward declaration */

static ASTNode *parse_primary(void)
{
    if (current_token->type == TOKEN_INT_LIT) {
        ASTNode *node = ast_node_create(AST_INT_LIT);
        node->value = strdup(current_token->value);
        advance();
        return node;
    }
    if (current_token->type == TOKEN_STRING_LIT) {
        ASTNode *node = ast_node_create(AST_STRING_LIT);
        node->value = strdup(current_token->value);
        advance();
        return node;
    }
    if (current_token->type == TOKEN_IDENT) {
        ASTNode *node = ast_node_create(AST_IDENT);
        node->name = strdup(current_token->value);
        advance();
        return node;
    }
    if (current_token->type == TOKEN_LPAREN) {
        advance();
        ASTNode *expr = parse_expression();
        expect(TOKEN_RPAREN);
        return expr;
    }
    fprintf(stderr, "[CPE] Parse error line %d: unexpected '%s'\n",
            current_token->line, current_token->value);
    exit(1);
}
```

### TODO 6-9: Expression Parsing (Operator Precedence)

ลำดับ: comparison → addition → multiply → primary

```c
/* TODO 6: * และ / */
static ASTNode *parse_multiply(void)
{
    ASTNode *left = parse_primary();
    while (current_token->type == TOKEN_STAR || current_token->type == TOKEN_SLASH) {
        ASTNode *op_node = ast_node_create(AST_BINARY_OP);
        op_node->op = strdup(current_token->value);
        advance();
        ASTNode *right = parse_primary();
        ast_node_add_child(op_node, left);
        ast_node_add_child(op_node, right);
        left = op_node;
    }
    return left;
}

/* TODO 7: + และ - (เหมือน multiply แต่เรียก parse_multiply) */
/* TODO 8: >, <, == (เหมือน addition แต่เรียก parse_addition) */
/* TODO 9: parse_expression → เรียก parse_comparison */
```

> 💡 **Pattern เหมือนกันหมด!** แค่เปลี่ยน operator กับ function ที่เรียก

### TODO 10: `parse_statement()` — แจกจ่ายงาน

นี่คือ function ที่ดู token ปัจจุบัน แล้วเรียก function ที่ถูกต้อง:

```c
static ASTNode *parse_statement(void)
{
    if (current_token->type == TOKEN_SET) {
        /* ดู token ที่ 3 ว่าเป็น AS (declaration) หรือ TO (assignment) */
        /* ... */
    }
    if (current_token->type == TOKEN_SHOW) {
        advance();
        ASTNode *node = ast_node_create(AST_PRINT);
        ast_node_add_child(node, parse_expression());
        expect(TOKEN_NEWLINE);
        return node;
    }
    if (current_token->type == TOKEN_IF) {
        /* parse condition → then block → else block (ถ้ามี) → end */
        /* ... */
    }
    if (current_token->type == TOKEN_WHILE) {
        /* parse condition → do block → end */
        /* ... */
    }
    fprintf(stderr, "[CPE] Parse error line %d\n", current_token->line);
    exit(1);
}
```

### TODO 11: `parser_parse()` — entry point

```c
ASTNode *parser_parse(TokenList *tokens)
{
    current_token = tokens->head;
    ASTNode *program = ast_node_create(AST_PROGRAM);
    skip_newlines();
    while (current_token && current_token->type != TOKEN_EOF) {
        ast_node_add_child(program, parse_statement());
        skip_newlines();
    }
    return program;
}
```

---

## ✅ เช็คว่าเสร็จหรือยัง

```bash
gcc -Wall -Wextra src/main.c src/lexer.c src/parser.c src/env.c src/eval.c -o build/cpelang.exe
```

ถ้า compile ผ่าน → พร้อมส่ง PR! 🎉
