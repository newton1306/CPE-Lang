/*
 * =============================================================================
 *  CPE Language v1.0 - Evaluator (AST Executor)
 * =============================================================================
 *  ผู้รับผิดชอบ: GEAR
 *  Branch: feature/evaluator
 *
 *  หน้าที่:
 *  - "เดิน" (traverse) ต้นไม้ AST แบบ recursive
 *  - ประมวลผลแต่ละ node ตามประเภท (switch-case)
 *  - จัดการ scope (สร้าง/ทำลาย child environment)
 *
 *  ดู eval.h สำหรับ API
 *  ดู parser.h สำหรับ ASTNodeType enum และ ASTNode struct  
 *  ดู env.h สำหรับ Value, Env structs และ API
 *  ดู ARCHITECTURE.md สำหรับ Evaluator Flow และ Scope Management
 *
 *  ⚠️ จุดสำคัญ:
 *  - Assignment ใน child scope ต้อง update in-place ผ่าน pointer (env_get)
 *  - ห้ามใช้ env_set ใน child scope สำหรับ assignment → จะ shadow ตัวแปร
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "env.h"
#include "eval.h"

/* ===========================================================================
 *  TODO 1: eval_block
 *  ประมวลผล block ของ statements (then...end, do...end)
 *
 *  ขั้นตอน:
 *  1. สร้าง child Env (parent = env ปัจจุบัน)
 *  2. วนลูป eval ทุก children ของ node ด้วย child Env
 *  3. ทำลาย child Env (ตัวแปรใน block หายไป)
 *
 *  ภาพ:
 *    eval_block เริ่ม → สร้าง child Env → eval ทุก statement → ทำลาย child Env
 * ===========================================================================*/
void eval_block(ASTNode *node, Env *env)
{
    /* TODO: implement */
    (void)node; (void)env;
}

/* ===========================================================================
 *  TODO 2: eval (ฟังก์ชันหลัก)
 *  ประมวลผล AST node แล้วคืน Value
 *
 *  ใช้ switch(node->type) จัดการแต่ละประเภท:
 *
 *  ─── AST_PROGRAM ───
 *  - eval ทุก children ตามลำดับ
 *  - คืน value ของ child ตัวสุดท้าย
 *
 *  ─── AST_INT_LIT ───
 *  - แปลง node->value เป็น int ด้วย atoi()
 *  - คืน value_int()
 *
 *  ─── AST_STRING_LIT ───
 *  - คืน value_string(node->value)
 *
 *  ─── AST_IDENT ───
 *  - ค้นหาใน env ด้วย env_get(env, node->name)
 *  - ถ้าไม่เจอ → error "undefined variable"
 *  - ถ้าเจอ → คืน copy ของ value (ระวัง string ต้อง strdup)
 *
 *  ─── AST_BINARY_OP ───
 *  - eval children[0] → left
 *  - eval children[1] → right
 *  - ดู node->op:
 *    - "+": int + int = int, string + string = concat
 *    - "-", "*", "/": int only
 *    - ">", "<", "==": เปรียบเทียบ → คืน 1 (true) หรือ 0 (false)
 *  - ⚠️ อย่าลืม value_free left/right หลังใช้เสร็จ (ถ้าเป็น string)
 *
 *  ─── AST_VAR_DECL ───
 *  - eval children[0] → initial value
 *  - ตรวจสอบ type: node->var_type == "int" → value ต้องเป็น VAL_INT
 *                   node->var_type == "string" → value ต้องเป็น VAL_STRING
 *  - env_set(env, node->name, value)
 *
 *  ─── AST_ASSIGN ───
 *  - ⚠️ สำคัญมาก!
 *  - ค้นหาตัวแปรด้วย env_get(env, node->name) → ได้ pointer
 *  - eval children[0] → new value
 *  - update ผ่าน pointer: *existing = new_value
 *  - ❌ ห้ามใช้ env_set → จะ shadow ตัวแปร → while loop ไม่จบ
 *
 *  ─── AST_IF ───
 *  - eval children[0] → condition
 *  - ถ้า truthy (int_val != 0) → eval_block(children[1])
 *  - ถ้า falsy + มี children[2] → eval_block(children[2])
 *
 *  ─── AST_WHILE ───
 *  - loop:
 *    1. eval children[0] → condition
 *    2. ถ้า truthy → eval_block(children[1]) → กลับไป 1
 *    3. ถ้า falsy → break
 *
 *  ─── AST_PRINT ───
 *  - eval children[0] → result
 *  - ถ้า VAL_INT → printf("%d\n", result.data.int_val)
 *  - ถ้า VAL_STRING → printf("%s\n", result.data.str_val)
 *  - value_free(&result)
 *
 *  ─── AST_BLOCK ───
 *  - eval ทุก children ใน env ปัจจุบัน (ไม่สร้าง scope ใหม่ ← eval_block ทำแล้ว)
 * ===========================================================================*/
Value eval(ASTNode *node, Env *env)
{
    /* TODO: implement */
    Value none;
    memset(&none, 0, sizeof(none));
    none.type = VAL_INT;
    none.data.int_val = 0;

    if (!node) return none;

    (void)env;
    return none;
}
