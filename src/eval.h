/*
 * =============================================================================
 *  CPE Language v1.0 - Evaluator Header
 * =============================================================================
 *  ตัว Evaluator ทำหน้าที่ "เดิน" (traverse) ต้นไม้ AST
 *  แล้วประมวลผลแต่ละ node ตามประเภทของมัน
 *
 *  ใช้ Post-order Traversal:
 *  1. ประมวลผล children ก่อน
 *  2. แล้วจึงประมวลผล node ปัจจุบัน
 *
 *  ตัวอย่าง: สำหรับ 2 + 3 × 4
 *       [+]
 *      /   \
 *    [2]   [×]
 *         /   \
 *       [3]   [4]
 *
 *  ลำดับการ evaluate: 2, 3, 4, (3×4=12), (2+12=14)
 * =============================================================================
 */

#ifndef CPE_EVAL_H
#define CPE_EVAL_H

#include "parser.h"
#include "env.h"

/* ประมวลผล AST node แล้วคืนค่าผลลัพธ์ */
Value eval(ASTNode *node, Env *env);

/* ประมวลผล block ของ statements (สร้าง scope ใหม่) */
void eval_block(ASTNode *node, Env *env);

#endif /* CPE_EVAL_H */
