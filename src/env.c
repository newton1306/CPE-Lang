/*
 * =============================================================================
 *  CPE Language v1.0 - Environment / Symbol Table
 * =============================================================================
 *  ผู้รับผิดชอบ: JEEFOON
 *  Branch: feature/environment
 *
 *  หน้าที่:
 *  - จัดการ Symbol Table ด้วย Hash Table (djb2 hash + Separate Chaining)
 *  - เก็บตัวแปร (ชื่อ → ค่า) พร้อม scope chain (parent pointer)
 *  - สร้าง/ทำลาย Value (int / string)
 *
 *  ดู env.h สำหรับ Value, EnvEntry, Env structs
 *  ดู ARCHITECTURE.md สำหรับรายละเอียด Hash Table และ Scope Chain
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "env.h"

static char *my_strdup(const char *s) {
    if (s == NULL) return NULL;
    size_t len = strlen(s) + 1;
    char *dup = (char *)malloc(len);
    if (dup != NULL) {
        strcpy(dup, s);
    }
    return dup;
}

/* ===========================================================================
 *  TODO 1: hash function (djb2)
 *
 *  อัลกอริทึม:
 *    hash = 5381
 *    for each character c in name:
 *        hash = hash * 33 + c
 *    return hash % ENV_TABLE_SIZE   (ENV_TABLE_SIZE = 64)
 *
 *  ตัวอย่าง: hash("x") = (5381 * 33 + 120) % 64 = ?
 * ===========================================================================*/
static unsigned int hash(const char *name)
{
    unsigned long hash_val = 5381;
    int c;
    while ((c = (unsigned char)*name++) != 0) {
        hash_val = ((hash_val << 5) + hash_val) + c; /* hash_val * 33 + c */
    }
    return (unsigned int)(hash_val % ENV_TABLE_SIZE);
}

/* ===========================================================================
 *  TODO 2: value_int
 *  สร้าง Value ชนิด int
 *  - ตั้ง type = VAL_INT
 *  - ตั้ง data.int_val = v
 * ===========================================================================*/
Value value_int(int v)
{
    /* TODO: implement */
    Value val;
    memset(&val, 0, sizeof(val));
    val.type = VAL_INT;
    val.data.int_val = v;
    return val;
}

/* ===========================================================================
 *  TODO 3: value_string
 *  สร้าง Value ชนิด string
 *  - ตั้ง type = VAL_STRING
 *  - ตั้ง data.str_val = strdup(s)  ← คัดลอก string ไปเก็บบน heap
 * ===========================================================================*/
Value value_string(const char *s)
{
    /* TODO: implement */
    Value val;
    memset(&val, 0, sizeof(val));
    val.type = VAL_STRING;
    val.data.str_val = my_strdup(s);
    return val;
}

/* ===========================================================================
 *  TODO 4: value_free
 *  คืน memory ของ Value
 *  - ถ้า type == VAL_STRING → free(data.str_val)
 *  - ถ้า type == VAL_INT → ไม่ต้องทำอะไร
 * ===========================================================================*/
void value_free(Value *val)
{
    if (val != NULL && val->type == VAL_STRING && val->data.str_val != NULL) {
        free(val->data.str_val);
        val->data.str_val = NULL;
    }
}

/* ===========================================================================
 *  TODO 5: env_create
 *  สร้าง Environment ใหม่
 *  - malloc Env struct
 *  - ตั้ง buckets ทั้ง 64 ช่องเป็น NULL
 *  - ตั้ง parent = parent parameter
 * ===========================================================================*/
Env *env_create(Env *parent)
{
    Env *env = (Env *)malloc(sizeof(Env));
    if (env == NULL) return NULL;
    
    for (int i = 0; i < ENV_TABLE_SIZE; i++) {
        env->buckets[i] = NULL;
    }

    env->parent = parent;    
    return env;
}

/* ===========================================================================
 *  TODO 6: env_destroy
 *  ทำลาย Environment คืน memory
 *  - ไล่ทุก bucket:
 *    - ไล่ทุก entry ใน chain:
 *      - free entry->name
 *      - value_free(&entry->value)
 *      - free entry
 *  - free ตัว env
 *  - ⚠️ ไม่ทำลาย parent!
 * ===========================================================================*/
void env_destroy(Env *env)
{
    if (env == NULL) return;
    
    for (int i = 0; i < ENV_TABLE_SIZE; i++) {
        EnvEntry *entry = env->buckets[i];
        while (entry != NULL) {
            EnvEntry *next = entry->next;
            
            if (entry->name != NULL) {
                free(entry->name);             
            }
            value_free(&entry->value);     
            free(entry);                  
            
            entry = next;
        }
    }
    free(env);
}

/* ===========================================================================
 *  TODO 7: env_set
 *  ตั้งค่าตัวแปรใน scope ปัจจุบัน
 *
 *  ขั้นตอน:
 *  1. คำนวณ hash index จากชื่อ
 *  2. ไล่ chain ที่ bucket นั้น:
 *     - ถ้าเจอ entry ที่ชื่อตรง → update value (free ค่าเก่า + ใส่ค่าใหม่)
 *  3. ถ้าไม่เจอ → สร้าง entry ใหม่ แทรกที่หัว chain
 *     - malloc EnvEntry
 *     - entry->name = strdup(name)
 *     - entry->value = value
 *     - entry->next = bucket head
 *     - bucket head = entry
 * ===========================================================================*/
void env_set(Env *env, const char *name, Value value)
{
    if (env == NULL || name == NULL) return;

    unsigned int index = hash(name);
    EnvEntry *entry = env->buckets[index];
    
    while (entry != NULL) {
        if (strcmp(entry->name, name) == 0) {
            value_free(&entry->value);  
            entry->value = value;      
            return;
        }
        entry = entry->next;
    }
    
    
    EnvEntry *new_entry = (EnvEntry *)malloc(sizeof(EnvEntry));
    if (new_entry == NULL) return; 
    
    new_entry->name = my_strdup(name); 
    new_entry->value = value;
    
   
    new_entry->next = env->buckets[index];
    env->buckets[index] = new_entry;
}

/* ===========================================================================
 *  TODO 8: env_get
 *  ค้นหาตัวแปร — ค้นตาม Scope Chain
 *
 *  ขั้นตอน:
 *  1. คำนวณ hash index จากชื่อ
 *  2. ไล่ chain ที่ bucket นั้น:
 *     - ถ้าเจอ → คืน &entry->value (pointer!)
 *  3. ถ้าไม่เจอ + มี parent → เรียก env_get(parent, name) ← recursive!
 *  4. ถ้าไม่เจอ + ไม่มี parent → คืน NULL (ตัวแปรไม่มี)
 *
 *  ⚠️ สำคัญ: คืน pointer (&entry->value) ไม่ใช่ copy!
 *  เพราะ Evaluator ต้องใช้ pointer นี้ในการ update in-place
 * ===========================================================================*/
Value *env_get(Env *env, const char *name)
{
   if (env == NULL || name == NULL) return NULL;

    unsigned int index = hash(name);
    EnvEntry *entry = env->buckets[index];

   
    while (entry != NULL) {
        if (strcmp(entry->name, name) == 0) {
            return &entry->value;   
        }
        entry = entry->next;
    }

    
    if (env->parent != NULL) {
        return env_get(env->parent, name);
    }

    return NULL;
}

