/*
 * =============================================================================
 *  CPE Language v1.0 - Parser Header (Action/Conversational Syntax)
 * =============================================================================
 *  ไฟล์นี้กำหนดโครงสร้าง Abstract Syntax Tree (AST)
 *
 *  Data Structure ที่ใช้:
 *  - ASTNode: โครงสร้าง N-ary/Binary Tree
 *    แต่ละ node อาจมี child หลายตัว (เช่น block มีหลาย statements)
 *    หรือมี left/right child (เช่น binary expression)
 *
 *  Grammar ของ CPE v1.0 (Action/Conversational):
 *
 *  statement  → set_stmt | show_stmt | if_stmt | while_stmt
 *  set_stmt   → "set" IDENT "as" TYPE "to" expr NEWLINE   (declaration)
 *             | "set" IDENT "to" expr NEWLINE              (assignment)
 *  show_stmt  → "show" expr NEWLINE
 *  if_stmt    → "if" expr "then" NEWLINE body ("else" NEWLINE body)? "end"
 *  while_stmt → "while" expr "do" NEWLINE body "end"
 *  body       → statement*
 * =============================================================================
 */

#ifndef CPE_PARSER_H
#define CPE_PARSER_H

#include "lexer.h"

/* ---------------------------------------------------------------------------
 *  AST Node Types (ประเภทของ node ใน AST)
 * ---------------------------------------------------------------------------*/
typedef enum {
    /* --- โปรแกรมและบล็อก --- */
    AST_PROGRAM,         /* root node: มี statements หลายตัวเป็น children  */
    AST_BLOCK,           /* block of statements (then...end, do...end)     */

    /* --- ประกาศและกำหนดค่าตัวแปร --- */
    AST_VAR_DECL,        /* set x as int to 5                              */
    AST_ASSIGN,          /* set x to 10                                    */

    /* --- Expressions (นิพจน์) --- */
    AST_INT_LIT,         /* ค่าคงที่จำนวนเต็ม: 42                          */
    AST_STRING_LIT,      /* ค่าคงที่สตริง: "hello"                         */
    AST_IDENT,           /* ชื่อตัวแปร: x                                  */
    AST_BINARY_OP,       /* นิพจน์ทวิภาค: x + 5, a > b                    */

    /* --- Control Flow --- */
    AST_IF,              /* if...then...else...end                         */
    AST_WHILE,           /* while...do...end                               */

    /* --- I/O --- */
    AST_PRINT,           /* show expr                                      */
} ASTNodeType;

/* ---------------------------------------------------------------------------
 *  ASTNode Structure (โครงสร้าง Node ของ AST)
 *
 *  ออกแบบเป็น N-ary Tree ด้วย array ของ children:
 *    - children[]  : array ของ pointer ไปยัง child nodes
 *    - child_count : จำนวน children ที่มีอยู่
 *
 *  สำหรับ Binary Operations:
 *    - children[0] = left operand
 *    - children[1] = right operand
 *    - op          = ตัวดำเนินการ
 *
 *  สำหรับ If-Else:
 *    - children[0] = condition
 *    - children[1] = then-block
 *    - children[2] = else-block (ถ้ามี)
 *
 *  สำหรับ While:
 *    - children[0] = condition
 *    - children[1] = body block
 *
 *  สำหรับ Variable Declaration (set x as int to 5):
 *    - name        = ชื่อตัวแปร
 *    - var_type    = "int" หรือ "string"
 *    - children[0] = initial value expression
 *
 *  สำหรับ Assignment (set x to 10):
 *    - name        = ชื่อตัวแปร
 *    - children[0] = value expression
 *
 *  สำหรับ Print (show expr):
 *    - children[0] = expression ที่จะ print
 * ---------------------------------------------------------------------------*/
#define AST_MAX_CHILDREN 128   /* จำนวน children สูงสุดต่อ node */

typedef struct ASTNode {
    ASTNodeType      type;
    char            *value;       /* ค่า literal (สำหรับ INT_LIT, STRING_LIT) */
    char            *name;        /* ชื่อตัวแปร (สำหรับ VAR_DECL, ASSIGN, IDENT) */
    char            *op;          /* ตัวดำเนินการ (สำหรับ BINARY_OP) */
    char            *var_type;    /* ชนิดตัวแปร "int"/"string" (สำหรับ VAR_DECL) */

    struct ASTNode  *children[AST_MAX_CHILDREN];  /* N-ary tree children array */
    int              child_count;                  /* จำนวน children ปัจจุบัน  */
} ASTNode;

/* ---------------------------------------------------------------------------
 *  Public API
 * ---------------------------------------------------------------------------*/

/* สร้าง AST node ใหม่ */
ASTNode *ast_node_create(ASTNodeType type);

/* เพิ่ม child เข้าไปใน parent node */
void ast_node_add_child(ASTNode *parent, ASTNode *child);

/* ทำลาย AST tree ทั้งหมดแบบ recursive */
void ast_node_free(ASTNode *node);

/* ฟังก์ชันหลัก: รับ TokenList แล้วคืน AST root node */
ASTNode *parser_parse(TokenList *tokens);

#endif /* CPE_PARSER_H */
