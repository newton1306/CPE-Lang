# 🟢 คู่มือสำหรับเพื่อนคนที่ 3 — Environment Developer

## 📋 งานของคุณคือ: เขียน `src/env.c`

Environment คือ **"ตัวเก็บตัวแปร"** — ใช้ **Hash Table** เก็บคู่ (ชื่อ → ค่า) เช่น `x → 10`, `name → "hello"`

ลองคิดง่ายๆ เหมือน Dictionary/Map:
```
ชื่อตัวแปร: "x"     → ค่า: 10
ชื่อตัวแปร: "name"  → ค่า: "hello"
ชื่อตัวแปร: "count" → ค่า: 5
```

---

## 📖 ไฟล์ที่ต้องอ่าน

### 1. `ARCHITECTURE.md` ส่วน "Phase 4: Environment"
ดู diagram ของ Hash Table + Scope Chain + djb2 hash function

### 2. `src/env.h` (สำคัญมาก!)
ดู struct ทั้งหมด:
- `Value` — ค่าที่เก็บ (int หรือ string, ใช้ union)
- `EnvEntry` — entry ใน hash table (name, value, next pointer)
- `Env` — ตัว hash table (buckets[64], parent pointer)

### 3. `src/env.c` (ไฟล์ที่คุณต้องแก้)
เปิดไฟล์นี้ จะเห็น **TODO 1-8** — `value_int` กับ `value_string` เขียนไว้เป็นตัวอย่างแล้ว!

---

## 🔀 วิธีเริ่มงาน

```bash
git checkout develop
git checkout -b feature/environment
code .
# เปิด src/env.c → เขียนโค้ด
```

---

## 📁 ไฟล์ที่แก้ได้

| แก้ได้ ✅ | ห้ามแก้ ❌ |
|----------|----------|
| `src/env.c` | `src/env.h`, `src/lexer.*`, `src/parser.*`, `src/eval.*`, `src/main.c` |

---

## 🛠️ อธิบาย TODO ทีละตัว

### TODO 1: `hash()` — djb2 hash function

**ใช้ทำอะไร:** แปลงชื่อตัวแปร (string) → ตัวเลข 0-63 เพื่อเลือก bucket

```c
static unsigned int hash(const char *name)
{
    unsigned int h = 5381;
    while (*name) {
        h = h * 33 + (unsigned char)(*name);
        name++;
    }
    return h % ENV_TABLE_SIZE;  /* ENV_TABLE_SIZE = 64 */
}
```

> 💡 **ทำไม 5381 กับ 33?** — เป็นค่าที่ Daniel J. Bernstein ค้นพบว่ากระจาย hash ได้ดี ไม่ต้องสนใจว่ามาจากไหน แค่ใช้ตามนี้

---

### TODO 2-3: `value_int()` + `value_string()` — ✅ เขียนไว้แล้ว!

เปิดไฟล์จะเห็นว่าทั้งสองตัวนี้ **เขียนเป็นตัวอย่างไว้แล้ว** ใช้เป็นแนวทาง

---

### TODO 4: `value_free()` — คืน memory ของ Value

```c
void value_free(Value *val)
{
    if (val && val->type == VAL_STRING) {
        free(val->data.str_val);
        val->data.str_val = NULL;
    }
}
```

---

### TODO 5: `env_create()` — สร้าง Environment ใหม่

```c
Env *env_create(Env *parent)
{
    Env *env = (Env *)malloc(sizeof(Env));
    for (int i = 0; i < ENV_TABLE_SIZE; i++) {
        env->buckets[i] = NULL;
    }
    env->parent = parent;
    return env;
}
```

---

### TODO 6: `env_destroy()` — ทำลาย Environment

**ระวัง:** ต้อง free ทุก entry ในทุก bucket + free ตัว env — แต่ **ไม่ทำลาย parent!**

```c
void env_destroy(Env *env)
{
    if (!env) return;
    for (int i = 0; i < ENV_TABLE_SIZE; i++) {
        EnvEntry *entry = env->buckets[i];
        while (entry) {
            EnvEntry *next = entry->next;
            free(entry->name);
            value_free(&entry->value);
            free(entry);
            entry = next;
        }
    }
    free(env);
}
```

---

### TODO 7: `env_set()` — ตั้งค่าตัวแปร

**ขั้นตอน:**
1. คำนวณ hash(name) → ได้ index ของ bucket
2. ไล่ดู chain ที่ bucket นั้น ถ้าเจอชื่อตรง → update ค่า
3. ถ้าไม่เจอ → สร้าง entry ใหม่ แทรกที่หัว chain

```c
void env_set(Env *env, const char *name, Value value)
{
    unsigned int idx = hash(name);
    
    /* ค้นหาว่ามีอยู่แล้วไหม */
    EnvEntry *entry = env->buckets[idx];
    while (entry) {
        if (strcmp(entry->name, name) == 0) {
            /* เจอ! → update ค่า */
            value_free(&entry->value);  /* free ค่าเก่า */
            entry->value = value;        /* ใส่ค่าใหม่ */
            return;
        }
        entry = entry->next;
    }
    
    /* ไม่เจอ → สร้าง entry ใหม่ แทรกที่หัว chain */
    EnvEntry *new_entry = (EnvEntry *)malloc(sizeof(EnvEntry));
    new_entry->name  = strdup(name);
    new_entry->value = value;
    new_entry->next  = env->buckets[idx];  /* ชี้ไปหัวเก่า */
    env->buckets[idx] = new_entry;          /* หัวใหม่ = entry ใหม่ */
}
```

---

### TODO 8: `env_get()` — ค้นหาตัวแปร (Scope Chain!)

**สำคัญ:** ถ้าหาไม่เจอใน scope ปัจจุบัน → ไล่ขึ้นไป parent!

```c
Value *env_get(Env *env, const char *name)
{
    if (!env) return NULL;
    
    unsigned int idx = hash(name);
    EnvEntry *entry = env->buckets[idx];
    
    while (entry) {
        if (strcmp(entry->name, name) == 0) {
            return &entry->value;  /* ⚠️ คืน pointer ไม่ใช่ copy! */
        }
        entry = entry->next;
    }
    
    /* ไม่เจอ → ค้นจาก parent ต่อ (recursive) */
    return env_get(env->parent, name);
}
```

> ⚠️ **สำคัญมาก:** `env_get` คืน **pointer** (`&entry->value`) ไม่ใช่ copy — เพราะ Evaluator ต้องใช้ pointer นี้ในการ update ค่าตัวแปร in-place

---

## ✅ เช็คว่าเสร็จหรือยัง

```bash
gcc -Wall -Wextra src/main.c src/lexer.c src/parser.c src/env.c src/eval.c -o build/cpelang.exe
```

ถ้า compile ผ่าน → พร้อมส่ง PR! 🎉
