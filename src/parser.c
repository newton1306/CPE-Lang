/*
 * =============================================================================
 *  CPE Language v1.0 - Parser Implementation (Action/Conversational Syntax)
 * =============================================================================
 *  Parser ทำหน้าที่แปลง Token stream (Linked List) เป็น AST (Tree)
 *
 *  ใช้เทคนิค Recursive Descent Parsing:
 *  - แต่ละ production rule ของ grammar มีฟังก์ชันของตัวเอง
 *  - ฟังก์ชันเหล่านี้เรียกกันเอง (recursive) ตามโครงสร้าง grammar
 *
 *  Grammar ของ CPE v1.0 (Action/Conversational):
 *
 *  program    → statement*
 *  statement  → set_stmt | show_stmt | if_stmt | while_stmt
 *
 *  set_stmt   → "set" IDENT "as" ("int"|"string") "to" expr NEWLINE
 *             | "set" IDENT "to" expr NEWLINE
 *
 *  show_stmt  → "show" expr NEWLINE
 *
 *  if_stmt    → "if" expr "then" NEWLINE body ("else" NEWLINE body)? "end" NEWLINE
 *  while_stmt → "while" expr "do" NEWLINE body "end" NEWLINE
 *  body       → statement*
 *
 *  expr       → comparison
 *  comparison → addition ((">" | "<" | "==") addition)*
 *  addition   → multiply (("+" | "-") multiply)*
 *  multiply   → primary (("*" | "/") primary)*
 *  primary    → INT_LIT | STRING_LIT | IDENT | "(" expr ")"
 *
 *  หมายเหตุ:
 *  - ไม่มี semicolon → ใช้ NEWLINE เป็นตัวจบ statement
 *  - ไม่มี braces → ใช้ then/do เริ่มบล็อก และ end จบบล็อก
 *  - ไม่มี = → ใช้ keyword "to" กำหนดค่า
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

/* ---------------------------------------------------------------------------
 *  Parser State
 *  เก็บ pointer ไปยัง token ปัจจุบันใน linked list
 *  ทำงานเหมือน cursor ที่เลื่อนไปข้างหน้าทีละ token
 * ---------------------------------------------------------------------------*/
typedef struct {
    Token *current;    /* token ปัจจุบันที่กำลังดู */
} Parser;

/* ---------------------------------------------------------------------------
 *  AST Node Utilities
 * ---------------------------------------------------------------------------*/

/* สร้าง AST node ใหม่ พร้อมตั้งค่าเริ่มต้น */
ASTNode *ast_node_create(ASTNodeType type)
{
    ASTNode *node = (ASTNode *)calloc(1, sizeof(ASTNode));
    if (!node) {
        fprintf(stderr, "[Parser] Error: memory allocation failed\n");
        exit(1);
    }
    node->type        = type;
    node->child_count = 0;
    return node;
}

/*
 * เพิ่ม child เข้า parent node (N-ary Tree insertion)
 */
void ast_node_add_child(ASTNode *parent, ASTNode *child)
{
    if (parent->child_count >= AST_MAX_CHILDREN) {
        fprintf(stderr, "[Parser] Error: too many children in AST node\n");
        exit(1);
    }
    parent->children[parent->child_count++] = child;
}

/*
 * ทำลาย AST tree แบบ recursive (Post-order traversal)
 */
void ast_node_free(ASTNode *node)
{
    if (!node) return;

    for (int i = 0; i < node->child_count; i++) {
        ast_node_free(node->children[i]);
    }

    if (node->value)    free(node->value);
    if (node->name)     free(node->name);
    if (node->op)       free(node->op);
    if (node->var_type) free(node->var_type);

    free(node);
}

/* ---------------------------------------------------------------------------
 *  Parser Helpers
 * ---------------------------------------------------------------------------*/

/* ดู token ปัจจุบันโดยไม่เลื่อน cursor */
static Token *peek(Parser *p)
{
    return p->current;
}

/* เลื่อน cursor ไปยัง token ถัดไปใน linked list */
static Token *advance(Parser *p)
{
    Token *tok = p->current;
    if (p->current->type != TOKEN_EOF) {
        p->current = p->current->next;
    }
    return tok;
}

/* ตรวจสอบว่า token ปัจจุบันเป็นประเภทที่ต้องการหรือไม่ */
static int check(Parser *p, TokenType type)
{
    return peek(p)->type == type;
}

/*
 * คาดหวัง token ประเภทที่กำหนด ถ้าไม่ใช่ → แจ้ง error แล้วหยุด
 */
static Token *expect(Parser *p, TokenType type, const char *msg)
{
    if (!check(p, type)) {
        fprintf(stderr, "[Parser] Error at line %d: expected %s but got '%s'\n",
                peek(p)->line, msg, peek(p)->value);
        exit(1);
    }
    return advance(p);
}

/*
 * ข้าม NEWLINE tokens ทั้งหมดที่ติดกัน
 * เรียกก่อนอ่าน statement ใหม่ เพื่อข้ามบรรทัดว่าง
 */
static void skip_newlines(Parser *p)
{
    while (check(p, TOKEN_NEWLINE)) {
        advance(p);
    }
}

/*
 * คาดหวัง NEWLINE หรือ EOF (จบ statement)
 * เรียกหลังจบ statement ทุกตัว
 */
static void expect_newline_or_eof(Parser *p)
{
    if (!check(p, TOKEN_NEWLINE) && !check(p, TOKEN_EOF)) {
        fprintf(stderr, "[Parser] Error at line %d: expected newline but got '%s'\n",
                peek(p)->line, peek(p)->value);
        exit(1);
    }
    if (check(p, TOKEN_NEWLINE)) {
        advance(p);
    }
}

/* ---------------------------------------------------------------------------
 *  Forward Declarations
 * ---------------------------------------------------------------------------*/
static ASTNode *parse_statement(Parser *p);
static ASTNode *parse_expression(Parser *p);

/* ---------------------------------------------------------------------------
 *  Expression Parsing (แยกวิเคราะห์นิพจน์)
 *
 *  ใช้ Operator Precedence Parsing:
 *    1. primary     (ค่าคงที่, ตัวแปร, วงเล็บ) - สูงสุด
 *    2. multiply    (* /)
 *    3. addition    (+ -)
 *    4. comparison  (> < ==) - ต่ำสุด
 * ---------------------------------------------------------------------------*/

/* parse_primary: อ่านค่าพื้นฐาน (leaf node) */
static ASTNode *parse_primary(Parser *p)
{
    Token *tok = peek(p);

    /* ตัวเลข → leaf node AST_INT_LIT */
    if (tok->type == TOKEN_INT_LIT) {
        advance(p);
        ASTNode *node = ast_node_create(AST_INT_LIT);
        node->value = strdup(tok->value);
        return node;
    }

    /* สตริง → leaf node AST_STRING_LIT */
    if (tok->type == TOKEN_STRING_LIT) {
        advance(p);
        ASTNode *node = ast_node_create(AST_STRING_LIT);
        node->value = strdup(tok->value);
        return node;
    }

    /* ชื่อตัวแปร → leaf node AST_IDENT */
    if (tok->type == TOKEN_IDENT) {
        advance(p);
        ASTNode *node = ast_node_create(AST_IDENT);
        node->name = strdup(tok->value);
        return node;
    }

    /* วงเล็บ → parse expression ข้างใน */
    if (tok->type == TOKEN_LPAREN) {
        advance(p);
        ASTNode *expr = parse_expression(p);
        expect(p, TOKEN_RPAREN, "')'");
        return expr;
    }

    fprintf(stderr, "[Parser] Error at line %d: unexpected token '%s'\n",
            tok->line, tok->value);
    exit(1);
    return NULL;
}

/*
 * parse_multiply: จัดการ * และ /
 * สร้าง Binary Tree (left-associative)
 */
static ASTNode *parse_multiply(Parser *p)
{
    ASTNode *left = parse_primary(p);

    while (check(p, TOKEN_STAR) || check(p, TOKEN_SLASH)) {
        Token *op_tok = advance(p);
        ASTNode *right = parse_primary(p);

        ASTNode *bin = ast_node_create(AST_BINARY_OP);
        bin->op = strdup(op_tok->value);
        ast_node_add_child(bin, left);
        ast_node_add_child(bin, right);

        left = bin;
    }

    return left;
}

/* parse_addition: จัดการ + และ - */
static ASTNode *parse_addition(Parser *p)
{
    ASTNode *left = parse_multiply(p);

    while (check(p, TOKEN_PLUS) || check(p, TOKEN_MINUS)) {
        Token *op_tok = advance(p);
        ASTNode *right = parse_multiply(p);

        ASTNode *bin = ast_node_create(AST_BINARY_OP);
        bin->op = strdup(op_tok->value);
        ast_node_add_child(bin, left);
        ast_node_add_child(bin, right);

        left = bin;
    }

    return left;
}

/* parse_comparison: จัดการ >, <, == */
static ASTNode *parse_comparison(Parser *p)
{
    ASTNode *left = parse_addition(p);

    while (check(p, TOKEN_GT) || check(p, TOKEN_LT) || check(p, TOKEN_EQ)) {
        Token *op_tok = advance(p);
        ASTNode *right = parse_addition(p);

        ASTNode *bin = ast_node_create(AST_BINARY_OP);
        bin->op = strdup(op_tok->value);
        ast_node_add_child(bin, left);
        ast_node_add_child(bin, right);

        left = bin;
    }

    return left;
}

/* parse_expression: entry point สำหรับทุก expression */
static ASTNode *parse_expression(Parser *p)
{
    return parse_comparison(p);
}

/* ---------------------------------------------------------------------------
 *  Body Parsing (แยกวิเคราะห์เนื้อหาของ block)
 *
 *  body คือลำดับของ statements ที่จบด้วย "end" หรือ "else"
 *  สร้าง AST_BLOCK node ที่มี children เป็น statements (N-ary Tree)
 *
 *  ใช้แทน parse_block เดิมที่ใช้ { ... }
 *  ตอนนี้ใช้ then/do เริ่มบล็อก และ end/else จบบล็อก
 * ---------------------------------------------------------------------------*/
static ASTNode *parse_body(Parser *p)
{
    ASTNode *block = ast_node_create(AST_BLOCK);

    skip_newlines(p);

    /* วน loop อ่าน statements จนเจอ 'end' หรือ 'else' หรือ EOF */
    while (!check(p, TOKEN_END) && !check(p, TOKEN_ELSE)
           && !check(p, TOKEN_EOF)) {
        skip_newlines(p);
        /* ตรวจอีกรอบหลังข้าม newlines */
        if (check(p, TOKEN_END) || check(p, TOKEN_ELSE) || check(p, TOKEN_EOF)) {
            break;
        }
        ASTNode *stmt = parse_statement(p);
        if (stmt) {
            ast_node_add_child(block, stmt);
        }
        skip_newlines(p);
    }

    return block;
}

/* ---------------------------------------------------------------------------
 *  Statement Parsing (แยกวิเคราะห์คำสั่ง)
 *
 *  ดู token ปัจจุบันเพื่อตัดสินใจว่าเป็นคำสั่งประเภทไหน:
 *  - set → declaration หรือ assignment
 *  - show → print
 *  - if → conditional
 *  - while → loop
 * ---------------------------------------------------------------------------*/
static ASTNode *parse_statement(Parser *p)
{
    Token *tok = peek(p);

    /* -----------------------------------------------------------
     *  SET Statement (Declaration หรือ Assignment)
     *
     *  Syntax 1 (Declaration):
     *    set <name> as <type> to <expr>
     *    → ตรวจว่า token ที่ 3 เป็น "as" หรือ "to"
     *
     *  Syntax 2 (Assignment):
     *    set <name> to <expr>
     * -----------------------------------------------------------*/
    if (tok->type == TOKEN_SET) {
        advance(p);   /* ข้าม 'set' */

        Token *name_tok = expect(p, TOKEN_IDENT, "variable name");

        /* ตรวจสอบว่าเป็น declaration (as) หรือ assignment (to) */
        if (check(p, TOKEN_AS)) {
            /* --- Declaration: set x as int to 5 --- */
            advance(p);   /* ข้าม 'as' */

            /* อ่านชนิดตัวแปร: int หรือ string */
            Token *type_tok = peek(p);
            if (type_tok->type != TOKEN_INT_TYPE &&
                type_tok->type != TOKEN_STRING_TYPE) {
                fprintf(stderr,
                    "[Parser] Error at line %d: expected type (int/string) "
                    "but got '%s'\n", type_tok->line, type_tok->value);
                exit(1);
            }
            advance(p);   /* ข้าม type */

            expect(p, TOKEN_TO, "'to'");   /* คาดหวัง 'to' */

            ASTNode *init = parse_expression(p);
            expect_newline_or_eof(p);

            ASTNode *decl = ast_node_create(AST_VAR_DECL);
            decl->var_type = strdup(type_tok->value);
            decl->name     = strdup(name_tok->value);
            ast_node_add_child(decl, init);
            return decl;

        } else if (check(p, TOKEN_TO)) {
            /* --- Assignment: set x to expr --- */
            advance(p);   /* ข้าม 'to' */

            ASTNode *val = parse_expression(p);
            expect_newline_or_eof(p);

            ASTNode *assign = ast_node_create(AST_ASSIGN);
            assign->name = strdup(name_tok->value);
            ast_node_add_child(assign, val);
            return assign;

        } else {
            fprintf(stderr,
                "[Parser] Error at line %d: expected 'as' or 'to' after "
                "'set %s' but got '%s'\n",
                peek(p)->line, name_tok->value, peek(p)->value);
            exit(1);
        }
    }

    /* -----------------------------------------------------------
     *  SHOW Statement (Print)
     *  Syntax: show <expr>
     *
     *  AST Structure:
     *       [PRINT]
     *          |
     *        expr
     * -----------------------------------------------------------*/
    if (tok->type == TOKEN_SHOW) {
        advance(p);   /* ข้าม 'show' */

        ASTNode *expr = parse_expression(p);
        expect_newline_or_eof(p);

        ASTNode *print_node = ast_node_create(AST_PRINT);
        ast_node_add_child(print_node, expr);
        return print_node;
    }

    /* -----------------------------------------------------------
     *  IF Statement
     *  Syntax: if <expr> then
     *              ...
     *          else
     *              ...
     *          end
     *
     *  AST Structure:
     *       [IF]
     *      / | \
     *   cond then else  (else อาจไม่มี)
     * -----------------------------------------------------------*/
    if (tok->type == TOKEN_IF) {
        advance(p);   /* ข้าม 'if' */

        ASTNode *cond = parse_expression(p);
        expect(p, TOKEN_THEN, "'then'");
        expect_newline_or_eof(p);     /* newline หลัง then */

        ASTNode *then_block = parse_body(p);

        ASTNode *if_node = ast_node_create(AST_IF);
        ast_node_add_child(if_node, cond);
        ast_node_add_child(if_node, then_block);

        /* ตรวจสอบว่ามี else หรือไม่ */
        if (check(p, TOKEN_ELSE)) {
            advance(p);   /* ข้าม 'else' */
            skip_newlines(p);

            ASTNode *else_block = parse_body(p);
            ast_node_add_child(if_node, else_block);
        }

        expect(p, TOKEN_END, "'end'");
        expect_newline_or_eof(p);

        return if_node;
    }

    /* -----------------------------------------------------------
     *  WHILE Statement
     *  Syntax: while <expr> do
     *              ...
     *          end
     *
     *  AST Structure:
     *       [WHILE]
     *       /    \
     *    cond   body
     * -----------------------------------------------------------*/
    if (tok->type == TOKEN_WHILE) {
        advance(p);   /* ข้าม 'while' */

        ASTNode *cond = parse_expression(p);
        expect(p, TOKEN_DO, "'do'");
        expect_newline_or_eof(p);     /* newline หลัง do */

        ASTNode *body = parse_body(p);

        expect(p, TOKEN_END, "'end'");
        expect_newline_or_eof(p);

        ASTNode *while_node = ast_node_create(AST_WHILE);
        ast_node_add_child(while_node, cond);
        ast_node_add_child(while_node, body);
        return while_node;
    }

    /* ไม่ตรงกับ production rule ใดเลย → error */
    fprintf(stderr, "[Parser] Error at line %d: unexpected token '%s'\n",
            tok->line, tok->value);
    exit(1);
    return NULL;
}

/* ---------------------------------------------------------------------------
 *  parser_parse: ฟังก์ชันหลักของ Parser
 *
 *  - สร้าง PROGRAM node เป็น root ของ AST
 *  - วน loop เรียก parse_statement จนจบไฟล์ (TOKEN_EOF)
 *  - แต่ละ statement ถูกเพิ่มเป็น child ของ PROGRAM node
 *
 *  ผลลัพธ์: AST ที่มีโครงสร้าง:
 *       [PROGRAM]
 *      / | | | \
 *   stmt1 stmt2 stmt3 ... (N-ary Tree)
 * ---------------------------------------------------------------------------*/
ASTNode *parser_parse(TokenList *tokens)
{
    Parser parser;
    parser.current = tokens->head;

    ASTNode *program = ast_node_create(AST_PROGRAM);

    /* ข้าม newlines นำหน้า */
    skip_newlines(&parser);

    while (!check(&parser, TOKEN_EOF)) {
        ASTNode *stmt = parse_statement(&parser);
        if (stmt) {
            ast_node_add_child(program, stmt);
        }
        skip_newlines(&parser);
    }

    return program;
}
