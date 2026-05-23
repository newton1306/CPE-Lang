/*
 * =============================================================================
 *  CPE Language v1.0 - Evaluator Implementation
 * =============================================================================
 *  Evaluator ทำหน้าที่ traverse AST แบบ recursive แล้วประมวลผลแต่ละ node
 *
 *  การทำงาน:
 *  - ใช้ switch-case ตาม node type
 *  - แต่ละ case เรียก eval() กับ children ก่อน แล้วประมวลผล
 *  - ค่า (Value) ถูกส่งกลับทาง return value
 *  - ใช้ Environment (Hash Table) เก็บค่าตัวแปร
 *
 *  Control Flow:
 *  - if: evaluate เงื่อนไข → ถ้า true ให้ execute then-block
 *  - while: evaluate เงื่อนไข → ถ้า true ให้ execute body แล้วกลับมาเช็คใหม่
 *
 *  Scope Management:
 *  - block ใหม่ → สร้าง Env ใหม่ (child scope)
 *  - ออกจาก block → ทำลาย Env (กลับไป parent scope)
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "eval.h"

/* ---------------------------------------------------------------------------
 *  Helper: ตรวจสอบว่า Value เป็น "truthy" หรือไม่
 *  - int: 0 = false, อื่นๆ = true
 *  - string: empty string = false, อื่นๆ = true
 * ---------------------------------------------------------------------------*/
static int is_truthy(Value val)
{
    if (val.type == VAL_INT) {
        return val.data.int_val != 0;
    }
    if (val.type == VAL_STRING) {
        return val.data.str_val != NULL && strlen(val.data.str_val) > 0;
    }
    return 0;
}

/* ---------------------------------------------------------------------------
 *  eval: ฟังก์ชันหลักของ Evaluator
 *
 *  รับ: ASTNode และ Environment ปัจจุบัน
 *  คืน: Value ที่เป็นผลลัพธ์ของการประมวลผล node นั้น
 *
 *  ใช้ Pattern Matching ด้วย switch-case ตาม node type:
 *  - Leaf nodes (INT_LIT, STRING_LIT, IDENT) → คืนค่าโดยตรง
 *  - Internal nodes (BINARY_OP, etc.) → evaluate children ก่อน แล้วคำนวณ
 *  - Statement nodes (VAR_DECL, ASSIGN, etc.) → ดำเนินการแล้วคืนค่า dummy
 * ---------------------------------------------------------------------------*/
Value eval(ASTNode *node, Env *env)
{
    if (!node) {
        return value_int(0);
    }

    switch (node->type) {

    /* -----------------------------------------------------------
     *  Integer Literal (Leaf Node)
     *  แปลง string value เป็น int แล้วคืน
     * -----------------------------------------------------------*/
    case AST_INT_LIT:
        return value_int(atoi(node->value));

    /* -----------------------------------------------------------
     *  String Literal (Leaf Node)
     *  คัดลอก string value แล้วคืน
     * -----------------------------------------------------------*/
    case AST_STRING_LIT:
        return value_string(node->value);

    /* -----------------------------------------------------------
     *  Identifier (Leaf Node)
     *  ค้นหาค่าตัวแปรจาก Environment (Hash Table Lookup)
     *  ถ้าไม่เจอ → error: undefined variable
     * -----------------------------------------------------------*/
    case AST_IDENT: {
        Value *found = env_get(env, node->name);
        if (!found) {
            fprintf(stderr, "[Runtime] Error: undefined variable '%s'\n",
                    node->name);
            exit(1);
        }
        /* คืนสำเนาของค่า (ไม่ได้ share pointer) */
        if (found->type == VAL_STRING) {
            return value_string(found->data.str_val);
        }
        return *found;
    }

    /* -----------------------------------------------------------
     *  Binary Operation (Internal Node ของ Binary Tree)
     *
     *  1. Evaluate children[0] (left operand)
     *  2. Evaluate children[1] (right operand)
     *  3. ดำเนินการตาม operator
     *
     *  รองรับ:
     *  - int op int → คำนวณทางคณิตศาสตร์/เปรียบเทียบ
     *  - string + string → ต่อสตริง (concatenation)
     * -----------------------------------------------------------*/
    case AST_BINARY_OP: {
        Value left  = eval(node->children[0], env);   /* evaluate ซ้าย */
        Value right = eval(node->children[1], env);   /* evaluate ขวา  */
        char *op = node->op;

        /* --- int op int --- */
        if (left.type == VAL_INT && right.type == VAL_INT) {
            int l = left.data.int_val;
            int r = right.data.int_val;
            int result = 0;

            if (strcmp(op, "+") == 0)       result = l + r;
            else if (strcmp(op, "-") == 0)  result = l - r;
            else if (strcmp(op, "*") == 0)  result = l * r;
            else if (strcmp(op, "/") == 0) {
                if (r == 0) {
                    fprintf(stderr, "[Runtime] Error: division by zero\n");
                    exit(1);
                }
                result = l / r;
            }
            else if (strcmp(op, ">") == 0)  result = (l > r)  ? 1 : 0;
            else if (strcmp(op, "<") == 0)  result = (l < r)  ? 1 : 0;
            else if (strcmp(op, "==") == 0) result = (l == r) ? 1 : 0;
            else {
                fprintf(stderr, "[Runtime] Error: unknown operator '%s'\n", op);
                exit(1);
            }

            return value_int(result);
        }

        /* --- string + string → concatenation --- */
        if (left.type == VAL_STRING && right.type == VAL_STRING
            && strcmp(op, "+") == 0) {
            /* คำนวณขนาดที่ต้องการ แล้วจองเพียงครั้งเดียว */
            size_t len = strlen(left.data.str_val) + strlen(right.data.str_val) + 1;
            char *buf = (char *)malloc(len);
            if (!buf) {
                fprintf(stderr, "[Runtime] Error: memory allocation failed\n");
                exit(1);
            }
            strcpy(buf, left.data.str_val);
            strcat(buf, right.data.str_val);

            Value result = value_string(buf);
            free(buf);

            /* Free temporary values */
            value_free(&left);
            value_free(&right);

            return result;
        }

        /* --- string == string → comparison --- */
        if (left.type == VAL_STRING && right.type == VAL_STRING
            && strcmp(op, "==") == 0) {
            int eq = (strcmp(left.data.str_val, right.data.str_val) == 0) ? 1 : 0;
            value_free(&left);
            value_free(&right);
            return value_int(eq);
        }

        fprintf(stderr, "[Runtime] Error: invalid operation '%s' on types %d and %d\n",
                op, left.type, right.type);
        exit(1);
    }

    /* -----------------------------------------------------------
     *  Variable Declaration
     *
     *  1. Evaluate children[0] (initial value expression)
     *  2. ตรวจสอบว่าชนิดตรงกัน
     *  3. เพิ่มตัวแปรเข้า Environment (Hash Table Insert)
     * -----------------------------------------------------------*/
    case AST_VAR_DECL: {
        Value init_val = eval(node->children[0], env);

        /* ตรวจสอบว่าชนิดตรงกับที่ประกาศ */
        if (strcmp(node->var_type, "int") == 0 && init_val.type != VAL_INT) {
            fprintf(stderr, "[Runtime] Error: type mismatch for variable '%s' "
                    "(expected int)\n", node->name);
            exit(1);
        }
        if (strcmp(node->var_type, "string") == 0 && init_val.type != VAL_STRING) {
            fprintf(stderr, "[Runtime] Error: type mismatch for variable '%s' "
                    "(expected string)\n", node->name);
            exit(1);
        }

        env_set(env, node->name, init_val);   /* Insert เข้า hash table */
        return value_int(0);   /* statement ไม่คืนค่าที่มีความหมาย */
    }

    /* -----------------------------------------------------------
     *  Assignment
     *
     *  1. ตรวจสอบว่าตัวแปรมีอยู่จริง (Hash Table Lookup)
     *  2. Evaluate children[0] (new value expression)
     *  3. อัปเดตค่าใน Environment (Hash Table Update)
     * -----------------------------------------------------------*/
    case AST_ASSIGN: {
        /*
         * ค้นหาตัวแปรผ่าน scope chain (env_get ค้นหาจาก local → parent → global)
         * env_get คืน pointer ตรง ๆ ไปยัง Value ที่เก็บอยู่ใน hash table
         * ทำให้เราสามารถ update ค่า "in-place" ได้โดยไม่ต้องสร้าง entry ใหม่
         *
         * สำคัญ: ถ้าใช้ env_set(env, ...) แทน จะสร้าง entry ใหม่ใน child scope
         * ทำให้ตัวแปรจาก scope นอกไม่ถูก update →  while loop วนไม่จบ (bug!)
         */
        Value *existing = env_get(env, node->name);
        if (!existing) {
            fprintf(stderr, "[Runtime] Error: undefined variable '%s'\n",
                    node->name);
            exit(1);
        }

        Value new_val = eval(node->children[0], env);

        /* ตรวจสอบ type ให้ตรงกับที่ประกาศ */
        if (existing->type != new_val.type) {
            fprintf(stderr, "[Runtime] Error: type mismatch in assignment to '%s'\n",
                    node->name);
            value_free(&new_val);
            exit(1);
        }

        /* Update ค่า in-place ผ่าน pointer ที่ได้จาก env_get
         * → update ที่ scope ที่ตัวแปรถูกประกาศ ไม่ใช่ scope ปัจจุบัน */
        value_free(existing);    /* free ค่าเก่า */
        *existing = new_val;     /* เขียนค่าใหม่ทับ in-place */
        return value_int(0);
    }

    /* -----------------------------------------------------------
     *  If-Else Statement
     *
     *  1. Evaluate children[0] (condition)
     *  2. ถ้า truthy → execute children[1] (then-block)
     *  3. ถ้า falsy และมี children[2] → execute children[2] (else-block)
     *
     *  แต่ละ block สร้าง scope ใหม่ (child Env)
     * -----------------------------------------------------------*/
    case AST_IF: {
        Value cond = eval(node->children[0], env);   /* evaluate เงื่อนไข */

        if (is_truthy(cond)) {
            eval_block(node->children[1], env);      /* execute then-block */
        } else if (node->child_count > 2) {
            eval_block(node->children[2], env);      /* execute else-block */
        }

        value_free(&cond);
        return value_int(0);
    }

    /* -----------------------------------------------------------
     *  While Loop
     *
     *  1. Evaluate children[0] (condition)
     *  2. ถ้า truthy → execute children[1] (body) แล้วกลับไปข้อ 1
     *  3. ถ้า falsy → ออกจาก loop
     *
     *  แต่ละรอบสร้าง scope ใหม่สำหรับ body
     * -----------------------------------------------------------*/
    case AST_WHILE: {
        while (1) {
            Value cond = eval(node->children[0], env);
            int go = is_truthy(cond);
            value_free(&cond);

            if (!go) break;   /* เงื่อนไขเป็น false → ออกจาก loop */

            eval_block(node->children[1], env);   /* execute body */
        }
        return value_int(0);
    }

    /* -----------------------------------------------------------
     *  Print Statement
     *
     *  1. Evaluate children[0] (expression)
     *  2. แสดงผลตามชนิดของค่า
     *  3. Free temporary value
     * -----------------------------------------------------------*/
    case AST_PRINT: {
        Value val = eval(node->children[0], env);

        if (val.type == VAL_INT) {
            printf("%d\n", val.data.int_val);
        } else if (val.type == VAL_STRING) {
            printf("%s\n", val.data.str_val);
        }

        value_free(&val);
        return value_int(0);
    }

    /* -----------------------------------------------------------
     *  Program / Block (N-ary Tree Nodes)
     *
     *  Traverse children ทุกตัวแล้ว evaluate ทีละตัว
     *  ใช้สำหรับ top-level program (ไม่สร้าง scope ใหม่)
     * -----------------------------------------------------------*/
    case AST_PROGRAM:
    case AST_BLOCK:
        for (int i = 0; i < node->child_count; i++) {
            Value v = eval(node->children[i], env);
            value_free(&v);
        }
        return value_int(0);

    default:
        fprintf(stderr, "[Runtime] Error: unknown AST node type %d\n",
                node->type);
        exit(1);
    }

    return value_int(0);
}

/* ---------------------------------------------------------------------------
 *  eval_block: ประมวลผล block พร้อมสร้าง scope ใหม่
 *
 *  1. สร้าง child Env (scope ใหม่) ที่ parent เป็น env ปัจจุบัน
 *  2. Evaluate ทุก statement ใน block ด้วย child Env
 *  3. ทำลาย child Env (กลับไป parent scope)
 *
 *  ทำให้ตัวแปรที่ประกาศใน block ถูกทำลายเมื่อออกจาก block
 *  แต่ตัวแปรจาก scope ภายนอกยังเข้าถึงได้ผ่าน parent pointer
 * ---------------------------------------------------------------------------*/
void eval_block(ASTNode *node, Env *env)
{
    /* สร้าง scope ใหม่: child env ชี้ parent ไปยัง env ปัจจุบัน */
    Env *child_env = env_create(env);

    /* Evaluate ทุก statement ใน block */
    for (int i = 0; i < node->child_count; i++) {
        Value v = eval(node->children[i], child_env);
        value_free(&v);
    }

    /*
     * ก่อนทำลาย child scope ต้อง "propagate" ค่าที่เปลี่ยนแปลง
     * กลับไป parent scope
     * (ตัวแปรที่ assign ใน block ควรมีผลที่ scope นอก ถ้าประกาศที่นอก)
     *
     * วิธีที่ใช้ใน v1.0: env_set ค้นหาตัวแปรใน scope ปัจจุบันก่อน
     * ถ้าไม่เจอจะสร้างใหม่ใน scope ปัจจุบัน
     *
     * สำหรับ v1.0 เราใช้วิธีง่ายๆ:
     * - Assignment จะทำใน scope ที่ถูกต้อง (ค้นหาด้วย env_get ก่อน)
     * - Variable ที่ประกาศใน block จะหายไปเมื่อออกจาก block
     */

    env_destroy(child_env);   /* ทำลาย scope ใน */
}
