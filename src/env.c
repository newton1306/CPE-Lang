/*
 * =============================================================================
 *  CPE Language v1.0 - Environment / Symbol Table Implementation
 * =============================================================================
 *  Implementation ของ Hash Table ด้วย Separate Chaining
 *
 *  Hash Table ทำงานดังนี้:
 *  1. รับ key (ชื่อตัวแปร) มาผ่าน hash function → ได้ index ของ bucket
 *  2. ที่ bucket นั้นเป็น linked list → ค้นหา/เพิ่ม entry ใน list
 *
 *  Scope Chaining:
 *  - แต่ละ Env มี parent pointer ชี้ไปยัง scope ระดับบน
 *  - เมื่อค้นหาตัวแปร ถ้าไม่เจอใน scope ปัจจุบัน → ไล่ขึ้น parent
 *  - ทำให้ตัวแปรใน scope นอกสามารถเข้าถึงได้จาก scope ใน
 *
 *  ตัวอย่าง Scope Chain:
 *    Global Env (x=10)
 *        ↑ parent
 *    If-Block Env (y=20)
 *        ↑ parent
 *    While-Block Env (z=30)
 *
 *  เมื่อ While-Block ค้นหา x → ไม่เจอในตัวเอง → ไปหาที่ If-Block
 *  → ไม่เจอ → ไปหาที่ Global → เจอ x=10
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "env.h"

/* ---------------------------------------------------------------------------
 *  Hash Function: djb2 by Dan Bernstein
 *  เป็น hash function ที่ง่ายแต่มีประสิทธิภาพดีสำหรับ string
 *
 *  Algorithm:
 *  1. เริ่มจาก hash = 5381
 *  2. สำหรับแต่ละตัวอักษร: hash = hash * 33 + c
 *  3. return hash mod ขนาดตาราง
 *
 *  ใช้ bit shift (hash << 5) + hash แทน hash * 33 (เร็วกว่า)
 * ---------------------------------------------------------------------------*/
static unsigned int hash_function(const char *key)
{
    unsigned long hash = 5381;
    int c;
    while ((c = (unsigned char)*key++) != 0) {
        hash = ((hash << 5) + hash) + c;   /* hash * 33 + c */
    }
    return (unsigned int)(hash % ENV_TABLE_SIZE);
}

/* ---------------------------------------------------------------------------
 *  value_int: สร้าง Value ชนิด int
 * ---------------------------------------------------------------------------*/
Value value_int(int v)
{
    Value val;
    val.type         = VAL_INT;
    val.data.int_val = v;
    return val;
}

/* ---------------------------------------------------------------------------
 *  value_string: สร้าง Value ชนิด string
 *  คัดลอก string ไปเก็บบน heap (ไม่ได้ share pointer)
 * ---------------------------------------------------------------------------*/
Value value_string(const char *s)
{
    Value val;
    val.type         = VAL_STRING;
    val.data.str_val = strdup(s);
    return val;
}

/* ---------------------------------------------------------------------------
 *  value_free: คืน memory ของ Value
 *  - int ไม่ต้อง free (เก็บอยู่ใน struct โดยตรง)
 *  - string ต้อง free เพราะ strdup allocate memory บน heap
 * ---------------------------------------------------------------------------*/
void value_free(Value *val)
{
    if (val->type == VAL_STRING && val->data.str_val) {
        free(val->data.str_val);
        val->data.str_val = NULL;
    }
}

/* ---------------------------------------------------------------------------
 *  env_create: สร้าง Environment ใหม่
 *
 *  - จอง memory สำหรับ Env struct
 *  - ตั้งค่า buckets ทั้งหมดเป็น NULL (hash table ว่าง)
 *  - ตั้ง parent pointer ไปยัง scope ระดับบน (NULL ถ้าเป็น global)
 *
 *  Time Complexity: O(ENV_TABLE_SIZE) สำหรับ initialize buckets
 * ---------------------------------------------------------------------------*/
Env *env_create(Env *parent)
{
    Env *env = (Env *)malloc(sizeof(Env));
    if (!env) {
        fprintf(stderr, "[Env] Error: memory allocation failed\n");
        exit(1);
    }

    /* Initialize ทุก bucket เป็น NULL (hash table ว่าง) */
    for (int i = 0; i < ENV_TABLE_SIZE; i++) {
        env->buckets[i] = NULL;
    }

    env->parent = parent;   /* เชื่อม scope chain */
    return env;
}

/* ---------------------------------------------------------------------------
 *  env_destroy: ทำลาย Environment
 *
 *  Algorithm:
 *  1. วนทุก bucket
 *  2. ในแต่ละ bucket วน linked list ของ entries
 *  3. free ชื่อตัวแปร (name) และค่า (value) ของแต่ละ entry
 *  4. free ตัว entry node
 *  5. free ตัว Env struct
 *
 *  หมายเหตุ: ไม่ทำลาย parent (เพราะ parent อาจยังถูกใช้อยู่)
 * ---------------------------------------------------------------------------*/
void env_destroy(Env *env)
{
    if (!env) return;

    for (int i = 0; i < ENV_TABLE_SIZE; i++) {
        EnvEntry *entry = env->buckets[i];
        while (entry != NULL) {
            EnvEntry *next = entry->next;  /* เก็บ pointer ก่อน free */
            free(entry->name);             /* คืน memory ชื่อตัวแปร  */
            value_free(&entry->value);     /* คืน memory ค่า (ถ้า string) */
            free(entry);                   /* คืน memory ตัว entry   */
            entry = next;
        }
    }

    free(env);
}

/* ---------------------------------------------------------------------------
 *  env_set: ตั้งค่าตัวแปรใน scope ปัจจุบัน
 *
 *  Algorithm (Hash Table Insert/Update with Separate Chaining):
 *  1. Hash ชื่อตัวแปร → ได้ bucket index
 *  2. ค้นหาใน linked list ของ bucket:
 *     a. ถ้าเจอ entry ที่ชื่อตรง → update ค่า (free ค่าเก่าก่อน)
 *     b. ถ้าไม่เจอ → สร้าง entry ใหม่ เพิ่มที่หัว linked list
 *
 *  การเพิ่ม entry ใหม่ที่หัว list (prepend):
 *     new_entry->next = buckets[index]
 *     buckets[index] = new_entry
 *  ทำได้ O(1) โดยไม่ต้อง traverse หาท้าย list
 *
 *  Time Complexity:
 *  - Best case: O(1) ถ้า bucket ไม่มี collision
 *  - Worst case: O(n) ถ้าทุก entry hash ไปที่ bucket เดียวกัน
 *  - Average case: O(1+α) โดย α = load factor
 * ---------------------------------------------------------------------------*/
void env_set(Env *env, const char *name, Value value)
{
    unsigned int index = hash_function(name);

    /* ค้นหาใน chain ว่ามี entry นี้อยู่แล้วหรือไม่ */
    EnvEntry *entry = env->buckets[index];
    while (entry != NULL) {
        if (strcmp(entry->name, name) == 0) {
            /* พบ entry ที่มีชื่อเดียวกัน → update ค่า */
            value_free(&entry->value);   /* free ค่าเก่า          */
            entry->value = value;        /* ใส่ค่าใหม่            */
            return;
        }
        entry = entry->next;   /* ไป entry ถัดไปใน chain */
    }

    /* ไม่เจอ → สร้าง entry ใหม่ เพิ่มที่หัว chain (prepend) */
    EnvEntry *new_entry = (EnvEntry *)malloc(sizeof(EnvEntry));
    if (!new_entry) {
        fprintf(stderr, "[Env] Error: memory allocation failed\n");
        exit(1);
    }
    new_entry->name  = strdup(name);        /* คัดลอกชื่อ         */
    new_entry->value = value;               /* เก็บค่า            */
    new_entry->next  = env->buckets[index]; /* ชี้ไปยังหัวเดิม    */
    env->buckets[index] = new_entry;        /* entry ใหม่เป็นหัว  */
}

/* ---------------------------------------------------------------------------
 *  env_get: ค้นหาตัวแปร
 *
 *  Algorithm (Scope Chain Lookup):
 *  1. Hash ชื่อตัวแปร → ได้ bucket index
 *  2. ค้นหาใน linked list ของ bucket ใน scope ปัจจุบัน
 *  3. ถ้าเจอ → return pointer ไปยังค่า
 *  4. ถ้าไม่เจอ → ไปค้นหาใน parent scope (recursive upward)
 *  5. ถ้าถึง global scope (parent = NULL) แล้วยังไม่เจอ → return NULL
 *
 *  ลำดับการค้นหา: Local → Enclosing → ... → Global
 *
 *  Time Complexity: O(d × (1+α))
 *  โดย d = ความลึกของ scope chain, α = load factor ของ hash table
 * ---------------------------------------------------------------------------*/
Value *env_get(Env *env, const char *name)
{
    Env *current = env;

    /* วน loop ไล่ขึ้น scope chain */
    while (current != NULL) {
        unsigned int index = hash_function(name);
        EnvEntry *entry = current->buckets[index];

        /* ค้นหาใน linked list ของ bucket */
        while (entry != NULL) {
            if (strcmp(entry->name, name) == 0) {
                return &entry->value;   /* เจอแล้ว! */
            }
            entry = entry->next;
        }

        current = current->parent;   /* ไม่เจอ → ไล่ขึ้น parent scope */
    }

    return NULL;   /* ค้นจนจบ scope chain แล้ว ไม่เจอ */
}
