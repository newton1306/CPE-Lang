# 🟠 คู่มือสำหรับเพื่อนคนที่ 4 — Evaluator Developer

## 📋 งานของคุณคือ: เขียน `src/eval.c`

> ⚠️ **งานนี้ยากที่สุด** เพราะต้องใช้ทุกโมดูล! ถ้าไม่เข้าใจตรงไหน อ่าน ARCHITECTURE.md ทั้งหมดก่อน

Evaluator คือ **"ตัวประมวลผล"** — รับ AST (ต้นไม้) จาก Parser แล้ว **เดินผ่านทุก node** ตามลำดับ → ประมวลผลจริง (คำนวณ, เก็บตัวแปร, แสดงผล)

ลองคิดง่ายๆ: เหมือนคนอ่านโปรแกรมทีละบรรทัดแล้วทำตาม

---

## 📖 ไฟล์ที่ต้องอ่าน (ต้องอ่านทั้ง 3!)

### 1. `src/eval.h`
ดู API: `eval()` กับ `eval_block()`

### 2. `src/parser.h` (สำคัญมาก!)
ดู `ASTNodeType` enum + `ASTNode` struct — ต้องรู้ว่า node แต่ละประเภทมี field อะไร:
- `AST_INT_LIT` → ใช้ `node->value` (เช่น "42")
- `AST_STRING_LIT` → ใช้ `node->value` (เช่น "hello")
- `AST_IDENT` → ใช้ `node->name` (เช่น "x")
- `AST_BINARY_OP` → ใช้ `node->op` (เช่น "+") + `children[0]`=ซ้าย, `children[1]`=ขวา
- `AST_VAR_DECL` → ใช้ `node->name`, `node->var_type`, `children[0]`=initial value
- `AST_ASSIGN` → ใช้ `node->name`, `children[0]`=new value
- `AST_IF` → `children[0]`=condition, `children[1]`=then, `children[2]`=else(ถ้ามี)
- `AST_WHILE` → `children[0]`=condition, `children[1]`=body

### 3. `src/env.h`
ดู `Value`, `Env` struct + API:
- `env_create(parent)` — สร้าง scope ใหม่
- `env_destroy(env)` — ทำลาย scope
- `env_set(env, name, value)` — ตั้งค่าตัวแปร
- `env_get(env, name)` — ค้นหาตัวแปร → **คืน pointer!**
- `value_int(v)` — สร้างค่า int
- `value_string(s)` — สร้างค่า string
- `value_free(val)` — คืน memory

### 4. `src/eval.c` (ไฟล์ที่คุณต้องแก้)
เปิดไฟล์นี้ จะเห็น **TODO 1-2** พร้อมคำอธิบายทุก node type

### 5. `ARCHITECTURE.md` — ทั้งหมด
อ่านทั้งหมดเพื่อเข้าใจ flow ตั้งแต่ Lexer → Parser → Environment → Evaluator

---

## 🔀 วิธีเริ่มงาน

```bash
git checkout develop
git checkout -b feature/evaluator
code .
# เปิด src/eval.c → เขียนโค้ด
```

---

## 📁 ไฟล์ที่แก้ได้

| แก้ได้ ✅ | ห้ามแก้ ❌ |
|----------|----------|
| `src/eval.c` | ทุกไฟล์อื่น |

---

## 🛠️ อธิบาย TODO

### TODO 1: `eval_block()` — ประมวลผล block (สร้าง scope ใหม่)

```c
void eval_block(ASTNode *node, Env *env)
{
    /* สร้าง child scope (parent = env ปัจจุบัน) */
    Env *child_env = env_create(env);
    
    /* eval ทุก statement ใน block */
    for (int i = 0; i < node->child_count; i++) {
        Value result = eval(node->children[i], child_env);
        value_free(&result);
    }
    
    /* ทำลาย child scope (ตัวแปรใน block หายไป) */
    env_destroy(child_env);
}
```

---

### TODO 2: `eval()` — ฟังก์ชันหลัก (ใหญ่มาก!)

```c
Value eval(ASTNode *node, Env *env)
{
    Value none = value_int(0);
    if (!node) return none;

    switch (node->type) {

    /* ============ ค่าคงที่ ============ */
    
    case AST_INT_LIT:
        return value_int(atoi(node->value));

    case AST_STRING_LIT:
        return value_string(node->value);

    /* ============ ชื่อตัวแปร ============ */
    
    case AST_IDENT: {
        Value *v = env_get(env, node->name);
        if (!v) {
            fprintf(stderr, "[CPE] Error: undefined variable '%s'\n", node->name);
            exit(1);
        }
        /* คืน copy (ไม่ใช่ pointer) — ระวัง string ต้อง strdup */
        if (v->type == VAL_STRING)
            return value_string(v->data.str_val);
        return *v;
    }

    /* ============ การคำนวณ ============ */
    
    case AST_BINARY_OP: {
        Value left  = eval(node->children[0], env);
        Value right = eval(node->children[1], env);
        Value result = none;

        if (strcmp(node->op, "+") == 0) {
            if (left.type == VAL_INT && right.type == VAL_INT) {
                result = value_int(left.data.int_val + right.data.int_val);
            } else if (left.type == VAL_STRING && right.type == VAL_STRING) {
                /* String concatenation */
                int len = strlen(left.data.str_val) + strlen(right.data.str_val) + 1;
                char *buf = (char *)malloc(len);
                strcpy(buf, left.data.str_val);
                strcat(buf, right.data.str_val);
                result = value_string(buf);
                free(buf);
            }
        }
        else if (strcmp(node->op, "-") == 0)
            result = value_int(left.data.int_val - right.data.int_val);
        else if (strcmp(node->op, "*") == 0)
            result = value_int(left.data.int_val * right.data.int_val);
        else if (strcmp(node->op, "/") == 0)
            result = value_int(left.data.int_val / right.data.int_val);
        else if (strcmp(node->op, ">") == 0)
            result = value_int(left.data.int_val > right.data.int_val ? 1 : 0);
        else if (strcmp(node->op, "<") == 0)
            result = value_int(left.data.int_val < right.data.int_val ? 1 : 0);
        else if (strcmp(node->op, "==") == 0)
            result = value_int(left.data.int_val == right.data.int_val ? 1 : 0);

        value_free(&left);
        value_free(&right);
        return result;
    }

    /* ============ ประกาศตัวแปร ============ */
    
    case AST_VAR_DECL: {
        Value init_val = eval(node->children[0], env);
        /* ตรวจสอบ type (ถ้าต้องการ) */
        env_set(env, node->name, init_val);
        return value_int(0);
    }

    /* ============ กำหนดค่าใหม่ (⚠️ สำคัญมาก!) ============ */
    
    case AST_ASSIGN: {
        Value *existing = env_get(env, node->name);
        if (!existing) {
            fprintf(stderr, "[CPE] Error: undefined variable '%s'\n", node->name);
            exit(1);
        }
        Value new_val = eval(node->children[0], env);
        value_free(existing);    /* free ค่าเก่า */
        *existing = new_val;     /* ✅ update in-place ผ่าน pointer */
        return value_int(0);
        
        /* ❌ ห้ามเขียนแบบนี้:
         * env_set(env, node->name, new_val);
         * เพราะจะสร้าง entry ใหม่ใน child scope → shadow ตัวแปร
         * → ทำให้ while loop ไม่จบ! */
    }

    /* ============ เงื่อนไข ============ */
    
    case AST_IF: {
        Value cond = eval(node->children[0], env);
        if (cond.data.int_val != 0) {
            /* true → ทำ then block */
            eval_block(node->children[1], env);
        } else if (node->child_count > 2) {
            /* false + มี else → ทำ else block */
            eval_block(node->children[2], env);
        }
        value_free(&cond);
        return value_int(0);
    }

    /* ============ วนรอบ ============ */
    
    case AST_WHILE: {
        while (1) {
            Value cond = eval(node->children[0], env);
            if (cond.data.int_val == 0) {
                value_free(&cond);
                break;  /* false → หยุด loop */
            }
            value_free(&cond);
            eval_block(node->children[1], env);
        }
        return value_int(0);
    }

    /* ============ แสดงผล ============ */
    
    case AST_PRINT: {
        Value result = eval(node->children[0], env);
        if (result.type == VAL_INT)
            printf("%d\n", result.data.int_val);
        else if (result.type == VAL_STRING)
            printf("%s\n", result.data.str_val);
        value_free(&result);
        return value_int(0);
    }

    /* ============ โปรแกรมและ block ============ */
    
    case AST_PROGRAM:
    case AST_BLOCK: {
        Value last = none;
        for (int i = 0; i < node->child_count; i++) {
            value_free(&last);
            last = eval(node->children[i], env);
        }
        return last;
    }

    default:
        return none;
    }
}
```

---

## ⚠️ จุดอันตราย — Assignment ใน while loop

**ปัญหา:** ถ้าใช้ `env_set()` แทน pointer update ใน `AST_ASSIGN` → ตัวแปรจะถูก shadow ใน child scope → while loop ไม่มีทางจบ!

```c
/* ❌ ผิด — ห้ามทำแบบนี้! */
case AST_ASSIGN: {
    Value new_val = eval(node->children[0], env);
    env_set(env, node->name, new_val);  /* สร้าง entry ใหม่ใน child scope! */
    return value_int(0);
}

/* ✅ ถูก — ต้องทำแบบนี้! */
case AST_ASSIGN: {
    Value *existing = env_get(env, node->name);  /* ค้นหาตาม scope chain */
    Value new_val = eval(node->children[0], env);
    value_free(existing);
    *existing = new_val;  /* update ตรงจุดที่เก็บจริง */
    return value_int(0);
}
```

---

## ✅ เช็คว่าเสร็จหรือยัง

```bash
gcc -Wall -Wextra src/main.c src/lexer.c src/parser.c src/env.c src/eval.c -o build/cpelang.exe
./build/cpelang.exe examples/demo.cpe
./build/cpelang.exe examples/demo2.cpe
```

ถ้า output ตรงกับที่คาดหวัง (ดูใน demo.cpe) → พร้อมส่ง PR! 🎉
