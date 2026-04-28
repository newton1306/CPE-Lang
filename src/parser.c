/*
 * =============================================================================
 *  CPE Language v1.0 - Parser (AST Builder)
 * =============================================================================
 *  ผู้รับผิดชอบ: IKKIW
 *  Branch: feature/parser
 *
 *  หน้าที่:
 *  - รับ Token Linked List จาก Lexer
 *  - สร้าง Abstract Syntax Tree (AST) ด้วย Recursive Descent Parsing
 *  - จัดการ operator precedence (comparison → addition → multiply → primary)
 *
 *  ดู parser.h สำหรับ ASTNodeType enum และ ASTNode struct
 *  ดู lexer.h สำหรับ TokenType ที่ต้องจับคู่
 *  ดู ARCHITECTURE.md สำหรับ Grammar และ AST ตัวอย่าง
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"

/* ---------------------------------------------------------------------------
 *  Parser State
 *  เก็บ pointer ไปยัง token ปัจจุบันที่กำลังอ่าน
 * ---------------------------------------------------------------------------*/
static Token *current_token = NULL;

/* ---------------------------------------------------------------------------
 *  Helper: advance — เลื่อน current_token ไปยัง token ถัดไป
 * ---------------------------------------------------------------------------*/
static void advance(void)
{
    if (current_token && current_token->next)
        current_token = current_token->next;
}

/* ---------------------------------------------------------------------------
 *  Helper: expect — ตรวจสอบว่า token ปัจจุบันเป็นประเภทที่คาดหวัง
 *  ถ้าไม่ตรง → แจ้ง error แล้วหยุดโปรแกรม
 * ---------------------------------------------------------------------------*/
static void expect(TokenType type)
{
    if (!current_token || current_token->type != type) {
        fprintf(stderr, "[CPE] Parse error at line %d: expected token type %d, got %d\n",
                current_token ? current_token->line : 0, type,
                current_token ? current_token->type : -1);
        exit(1);
    }
    advance();
}

/* ---------------------------------------------------------------------------
 *  Helper: skip_newlines — ข้าม TOKEN_NEWLINE ที่ติดกัน
 * ---------------------------------------------------------------------------*/
static void skip_newlines(void)
{
    while (current_token && current_token->type == TOKEN_NEWLINE)
        advance();
}

/* ===========================================================================
 *  TODO 1: ast_node_create
 *  สร้าง ASTNode ใหม่
 *  - malloc + memset ทั้ง struct เป็น 0
 *  - ตั้ง type ตามที่ส่งมา
 * ===========================================================================*/
ASTNode *ast_node_create(ASTNodeType type)
{
    /* TODO: implement */
    (void)type;
    return NULL;
}

/* ===========================================================================
 *  TODO 2: ast_node_add_child
 *  เพิ่ม child เข้าไปใน parent->children[]
 *  - ตรวจสอบ child_count < AST_MAX_CHILDREN
 *  - parent->children[parent->child_count++] = child
 * ===========================================================================*/
void ast_node_add_child(ASTNode *parent, ASTNode *child)
{
    /* TODO: implement */
    (void)parent; (void)child;
}

/* ===========================================================================
 *  TODO 3: ast_node_free
 *  ทำลาย AST tree ทั้งหมดแบบ recursive
 *  - free children ก่อน (recursive)
 *  - free value, name, op, var_type (ถ้ามี)
 *  - free ตัว node
 * ===========================================================================*/
void ast_node_free(ASTNode *node)
{
    /* TODO: implement */
    (void)node;
}

/* ===========================================================================
 *  TODO 4: Forward declarations สำหรับ Recursive Descent
 *
 *  ประกาศฟังก์ชัน parse ล่วงหน้า เพราะแต่ละฟังก์ชันเรียกกันเป็นวงกลม:
 *
 *  static ASTNode *parse_statement(void);
 *  static ASTNode *parse_expression(void);   → เรียก parse_comparison
 *  static ASTNode *parse_comparison(void);   → เรียก parse_addition
 *  static ASTNode *parse_addition(void);     → เรียก parse_multiply
 *  static ASTNode *parse_multiply(void);     → เรียก parse_primary
 *  static ASTNode *parse_primary(void);      → INT_LIT, STRING_LIT, IDENT, (expr)
 * ===========================================================================*/

/* TODO: ประกาศ forward declarations ที่นี่ */

/* ===========================================================================
 *  TODO 5: parse_primary
 *  จัดการ token ที่เป็น "ค่า" พื้นฐาน:
 *  - TOKEN_INT_LIT    → AST_INT_LIT   (เก็บ value)
 *  - TOKEN_STRING_LIT → AST_STRING_LIT (เก็บ value)
 *  - TOKEN_IDENT      → AST_IDENT     (เก็บ name)
 *  - TOKEN_LPAREN     → parse_expression() → expect TOKEN_RPAREN
 * ===========================================================================*/

/* TODO: implement parse_primary */

/* ===========================================================================
 *  TODO 6: parse_multiply
 *  จัดการ * และ /
 *  - เรียก parse_primary() ก่อน
 *  - ถ้าเจอ * หรือ / → สร้าง AST_BINARY_OP → เรียก parse_primary() อีกครั้ง
 *  - ทำวนจน operators หมด
 * ===========================================================================*/

/* TODO: implement parse_multiply */

/* ===========================================================================
 *  TODO 7: parse_addition
 *  จัดการ + และ -
 *  - เรียก parse_multiply() ก่อน
 *  - ถ้าเจอ + หรือ - → สร้าง AST_BINARY_OP
 * ===========================================================================*/

/* TODO: implement parse_addition */

/* ===========================================================================
 *  TODO 8: parse_comparison
 *  จัดการ >, <, ==
 *  - เรียก parse_addition() ก่อน
 *  - ถ้าเจอ > หรือ < หรือ == → สร้าง AST_BINARY_OP
 * ===========================================================================*/

/* TODO: implement parse_comparison */

/* ===========================================================================
 *  TODO 9: parse_expression
 *  Entry point สำหรับ expression → เรียก parse_comparison()
 * ===========================================================================*/

/* TODO: implement parse_expression */

/* ===========================================================================
 *  TODO 10: parse_statement
 *  จัดการแต่ละ statement:
 *
 *  TOKEN_SET → ดู token ถัดถัดไป:
 *    - ถ้าเจอ TOKEN_AS → VAR_DECL: set <name> as <type> to <expr>
 *      - children[0] = initial value expression
 *    - ถ้าไม่ → ASSIGN: set <name> to <expr>
 *      - children[0] = new value expression
 *
 *  TOKEN_SHOW → PRINT:
 *    - children[0] = expression ที่จะ print
 *
 *  TOKEN_IF → IF:
 *    - children[0] = condition expression
 *    - children[1] = then-block (AST_BLOCK)
 *    - children[2] = else-block (AST_BLOCK, ถ้ามี)
 *    - parse จนเจอ TOKEN_END
 *
 *  TOKEN_WHILE → WHILE:
 *    - children[0] = condition expression
 *    - children[1] = body block (AST_BLOCK)
 *    - parse จนเจอ TOKEN_END
 * ===========================================================================*/

/* TODO: implement parse_statement */

/* ===========================================================================
 *  TODO 11: parser_parse (ฟังก์ชันหลัก)
 *
 *  1. ตั้ง current_token = tokens->head
 *  2. สร้าง PROGRAM node (root ของ AST)
 *  3. วนลูป skip_newlines → parse_statement → add_child
 *     จนกว่า current_token->type == TOKEN_EOF
 *  4. คืน PROGRAM node
 * ===========================================================================*/
ASTNode *parser_parse(TokenList *tokens)
{
    /* TODO: implement */
    (void)tokens;
    return ast_node_create(AST_PROGRAM);
}
