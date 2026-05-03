/*
 * =============================================================================
 * CPE Language v1.0 - Evaluator (AST Executor)
 * =============================================================================
 * ผู้รับผิดชอบ: GEAR
 * Branch: feature/evaluator
 *
 * หน้าที่:
 * - "เดิน" (traverse) ต้นไม้ AST แบบ recursive
 * - ประมวลผลแต่ละ node ตามประเภท (switch-case)
 * - จัดการ scope (สร้าง/ทำลาย child environment)
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "env.h"
#include "eval.h"

/* ===========================================================================
 * eval_block
 * ประมวลผล block ของ statements พร้อมสร้างและทำลาย Child Environment
 * ===========================================================================*/
void eval_block(ASTNode *node, Env *env)
{
    /* 1. สร้าง child scope (parent = env ปัจจุบัน) */
    Env *child_env = env_create(env);
    
    /* 2. eval ทุก statement ใน block */
    for (int i = 0; i < node->child_count; i++) {
        Value result = eval(node->children[i], child_env);
        value_free(&result);
    }
    
    /* 3. ทำลาย child scope (ตัวแปรใน block หายไป) */
    env_destroy(child_env);
}

/* ===========================================================================
 * eval
 * ประมวลผล AST node หลักแล้วคืนค่า Value
 * ===========================================================================*/
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
        /* คืน copy เพื่อป้องกัน side effects (string ต้อง duplicate ให้ถูกต้องตาม value_string) */
        if (v->type == VAL_STRING) {
            return value_string(v->data.str_val);
        }
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
                if (!buf) {
                    fprintf(stderr, "[CPE] Error: memory allocation failed\n");
                    exit(1);
                }
                strcpy(buf, left.data.str_val);
                strcat(buf, right.data.str_val);
                result = value_string(buf);
                free(buf);
            } else {
                fprintf(stderr, "[CPE] Error: unsupported operand types for +\n");
                exit(1);
            }
        }
        else if (strcmp(node->op, "-") == 0) {
            if (left.type != VAL_INT || right.type != VAL_INT) {
                fprintf(stderr, "[CPE] Error: operator - requires int operands\n");
                exit(1);
            }
            result = value_int(left.data.int_val - right.data.int_val);
        }
        else if (strcmp(node->op, "*") == 0) {
            if (left.type != VAL_INT || right.type != VAL_INT) {
                fprintf(stderr, "[CPE] Error: operator * requires int operands\n");
                exit(1);
            }
            result = value_int(left.data.int_val * right.data.int_val);
        }
        else if (strcmp(node->op, "/") == 0) {
            if (left.type != VAL_INT || right.type != VAL_INT) {
                fprintf(stderr, "[CPE] Error: operator / requires int operands\n");
                exit(1);
            }
            if (right.data.int_val == 0) {
                fprintf(stderr, "[CPE] Error: division by zero\n");
                exit(1);
            }
            result = value_int(left.data.int_val / right.data.int_val);
        }
        else if (strcmp(node->op, ">") == 0) {
            if (left.type != VAL_INT || right.type != VAL_INT) {
                fprintf(stderr, "[CPE] Error: operator > requires int operands\n");
                exit(1);
            }
            result = value_int(left.data.int_val > right.data.int_val ? 1 : 0);
        }
        else if (strcmp(node->op, "<") == 0) {
            if (left.type != VAL_INT || right.type != VAL_INT) {
                fprintf(stderr, "[CPE] Error: operator < requires int operands\n");
                exit(1);
            }
            result = value_int(left.data.int_val < right.data.int_val ? 1 : 0);
        }
        else if (strcmp(node->op, "==") == 0) {
            if (left.type != VAL_INT || right.type != VAL_INT) {
                fprintf(stderr, "[CPE] Error: operator == requires int operands\n");
                exit(1);
            }
            result = value_int(left.data.int_val == right.data.int_val ? 1 : 0);
        } else {
            fprintf(stderr, "[CPE] Error: unsupported operator '%s'\n", node->op);
            exit(1);
        }

        value_free(&left);
        value_free(&right);
        return result;
    }

    /* ============ ประกาศตัวแปร ============ */
    
    case AST_VAR_DECL: {
        Value init_val = eval(node->children[0], env);
        if (strcmp(node->var_type, "int") == 0 && init_val.type != VAL_INT) {
            fprintf(stderr, "[CPE] Error: cannot initialize int variable '%s' with non-int value\n", node->name);
            exit(1);
        }
        if (strcmp(node->var_type, "string") == 0 && init_val.type != VAL_STRING) {
            fprintf(stderr, "[CPE] Error: cannot initialize string variable '%s' with non-string value\n", node->name);
            exit(1);
        }
        env_set(env, node->name, init_val);
        return value_int(0);
    }

    /* ============ กำหนดค่าใหม่ ============ */
    
    case AST_ASSIGN: {
        Value *existing = env_get(env, node->name);
        if (!existing) {
            fprintf(stderr, "[CPE] Error: undefined variable '%s'\n", node->name);
            exit(1);
        }
        Value new_val = eval(node->children[0], env);
        value_free(existing);    /* free ค่าเดิมป้องกัน memory leak */
        *existing = new_val;     /* Update in-place ผ่าน pointer (ป้องกัน shadowing ใน while) */
        return value_int(0);
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

    /* ============ โปรแกรมและ block หลัก ============ */
    
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