/*
 * =============================================================================
 *  CPE Language v1.0 - Environment / Symbol Table Header
 * =============================================================================
 *  ไฟล์นี้กำหนดโครงสร้าง Symbol Table สำหรับเก็บตัวแปร
 *
 *  Data Structure ที่ใช้:
 *  - Hash Table ด้วย Separate Chaining (Linked List ที่แต่ละ bucket)
 *    → แต่ละ bucket เก็บ linked list ของ entries ที่ hash ตรงกัน
 *    → ใช้ hash function แบบ djb2
 *
 *  การจัดการ Scope:
 *  - ใช้โครงสร้าง Env (Environment) ที่มี parent pointer
 *  - เมื่อเข้า block ใหม่ → สร้าง Env ใหม่ที่ชี้ parent ไปยัง Env เดิม
 *  - เมื่อค้นหาตัวแปร → ค้นหาจาก Env ปัจจุบัน ถ้าไม่เจอก็ไล่ขึ้นไป parent
 *  - เมื่อออกจาก block → ทำลาย Env ปัจจุบัน กลับไป parent
 * =============================================================================
 */

#ifndef CPE_ENV_H
#define CPE_ENV_H

/* ---------------------------------------------------------------------------
 *  Value Types (ชนิดของค่าที่เก็บใน symbol table)
 * ---------------------------------------------------------------------------*/
typedef enum {
    VAL_INT,       /* จำนวนเต็ม */
    VAL_STRING     /* สตริง     */
} ValueType;

/* ---------------------------------------------------------------------------
 *  Value Structure (ค่าที่เก็บ)
 *  ใช้ union เพื่อประหยัด memory - เก็บได้ทีละ 1 ชนิด
 * ---------------------------------------------------------------------------*/
typedef struct {
    ValueType type;
    union {
        int   int_val;      /* ค่า int */
        char *str_val;      /* ค่า string (dynamically allocated) */
    } data;
} Value;

/* ---------------------------------------------------------------------------
 *  EnvEntry: entry ใน hash table (node ของ linked list ในแต่ละ bucket)
 *  - name  : ชื่อตัวแปร (key)
 *  - value : ค่าของตัวแปร
 *  - next  : pointer ไปยัง entry ถัดไปใน chain
 * ---------------------------------------------------------------------------*/
typedef struct EnvEntry {
    char            *name;    /* key: ชื่อตัวแปร          */
    Value            value;   /* value: ค่าของตัวแปร      */
    struct EnvEntry *next;    /* → next in chain (collision) */
} EnvEntry;

/* ---------------------------------------------------------------------------
 *  Env Structure (Environment / Symbol Table)
 *  - buckets[]  : array ของ pointer ไปยังหัว linked list ของแต่ละ bucket
 *  - size       : จำนวน buckets (ขนาดของ hash table)
 *  - parent     : pointer ไปยัง scope ระดับบน (สำหรับ scope chaining)
 * ---------------------------------------------------------------------------*/
#define ENV_TABLE_SIZE 64    /* จำนวน buckets ของ hash table */

typedef struct Env {
    EnvEntry   *buckets[ENV_TABLE_SIZE];  /* hash table buckets  */
    struct Env *parent;                    /* parent scope        */
} Env;

/* ---------------------------------------------------------------------------
 *  Public API
 * ---------------------------------------------------------------------------*/

/* สร้าง Environment ใหม่ (scope ใหม่) โดยมี parent เป็น scope ระดับบน */
Env *env_create(Env *parent);

/* ทำลาย Environment พร้อมคืน memory (ไม่ทำลาย parent) */
void env_destroy(Env *env);

/* ตั้งค่าตัวแปรใน scope ปัจจุบัน (insert หรือ update) */
void env_set(Env *env, const char *name, Value value);

/* ค้นหาตัวแปร: ค้นจาก scope ปัจจุบัน ถ้าไม่เจอไล่ขึ้น parent */
Value *env_get(Env *env, const char *name);

/* สร้าง Value ชนิด int */
Value value_int(int v);

/* สร้าง Value ชนิด string (คัดลอก string ไปเก็บบน heap) */
Value value_string(const char *s);

/* คืน memory ของ Value (เฉพาะ string) */
void value_free(Value *val);

#endif /* CPE_ENV_H */
