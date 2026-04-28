/*
 * =============================================================================
 *  CPE Language v1.0 - Lexer (Tokenizer)
 * =============================================================================
 *  ผู้รับผิดชอบ: [Lexer Developer]
 *  Branch: feature/lexer
 *
 *  หน้าที่:
 *  - รับ source code (string) แล้ว scan ทีละตัวอักษร
 *  - สร้าง Token Linked List (head → tail, O(1) append)
 *  - แยก keywords, identifiers, literals, operators
 *
 *  ดู lexer.h สำหรับ TokenType enum และ Token/TokenList structs
 *  ดู ARCHITECTURE.md สำหรับรายละเอียดการทำงาน
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

/* ===========================================================================
 *  TODO 1: token_list_create
 *  สร้าง TokenList ว่าง (malloc + ตั้งค่า head=NULL, tail=NULL, count=0)
 * ===========================================================================*/
TokenList *token_list_create(void)
{
    /* TODO: implement */
    return NULL;
}

/* ===========================================================================
 *  TODO 2: token_list_append
 *  เพิ่ม token ใหม่ต่อท้าย linked list
 *  - malloc Token ใหม่
 *  - copy value ด้วย strdup
 *  - ถ้า list ว่าง: head = tail = new token
 *  - ถ้าไม่ว่าง: tail->next = new token, tail = new token
 *  - count++
 * ===========================================================================*/
void token_list_append(TokenList *list, TokenType type,
                       const char *value, int line)
{
    /* TODO: implement */
    (void)list; (void)type; (void)value; (void)line;
}

/* ===========================================================================
 *  TODO 3: token_list_free
 *  ทำลาย TokenList คืน memory ทั้งหมด
 *  - ไล่ free ทุก token ใน linked list (free value + free node)
 *  - free ตัว TokenList
 * ===========================================================================*/
void token_list_free(TokenList *list)
{
    /* TODO: implement */
    (void)list;
}

/* ===========================================================================
 *  TODO 4: token_type_name
 *  แปลง TokenType enum เป็น string (สำหรับ debug)
 *  เช่น TOKEN_SET → "SET", TOKEN_IDENT → "IDENT"
 * ===========================================================================*/
const char *token_type_name(TokenType type)
{
    /* TODO: implement — ใช้ switch-case */
    (void)type;
    return "UNKNOWN";
}

/* ===========================================================================
 *  TODO 5: lexer_tokenize (ฟังก์ชันหลัก)
 *
 *  รับ source code แล้วคืน TokenList
 *
 *  ขั้นตอน:
 *  1. สร้าง TokenList ว่าง
 *  2. ไล่ scan ทีละตัวอักษร:
 *     - whitespace (space, tab, \r): ข้าม
 *     - newline (\n): สร้าง TOKEN_NEWLINE (แต่ข้ามถ้า token ก่อนหน้าเป็น NEWLINE)
 *     - comment (//): ข้ามจนจบบรรทัด
 *     - ตัวเลข (0-9): scan ทั้งหมด → TOKEN_INT_LIT
 *     - string ("..."): scan จนเจอ " ปิด → TOKEN_STRING_LIT
 *     - ตัวอักษร (a-z, A-Z, _): scan ทั้ง word → เช็ค keyword table:
 *       - "set" → TOKEN_SET
 *       - "as" → TOKEN_AS
 *       - "to" → TOKEN_TO
 *       - "show" → TOKEN_SHOW
 *       - "if" → TOKEN_IF
 *       - "else" → TOKEN_ELSE
 *       - "while" → TOKEN_WHILE
 *       - "then" → TOKEN_THEN
 *       - "do" → TOKEN_DO
 *       - "end" → TOKEN_END
 *       - "int" → TOKEN_INT_TYPE
 *       - "string" → TOKEN_STRING_TYPE
 *       - อื่นๆ → TOKEN_IDENT
 *     - operators: +, -, *, /, ==, >, <, (, )
 *  3. ปิดท้ายด้วย TOKEN_EOF
 *  4. คืน TokenList
 *
 *  Keyword Table:
 *  | Keyword  | TokenType         |
 *  |----------|-------------------|
 *  | set      | TOKEN_SET         |
 *  | as       | TOKEN_AS          |
 *  | to       | TOKEN_TO          |
 *  | show     | TOKEN_SHOW        |
 *  | if       | TOKEN_IF          |
 *  | else     | TOKEN_ELSE        |
 *  | while    | TOKEN_WHILE       |
 *  | then     | TOKEN_THEN        |
 *  | do       | TOKEN_DO          |
 *  | end      | TOKEN_END         |
 *  | int      | TOKEN_INT_TYPE    |
 *  | string   | TOKEN_STRING_TYPE |
 * ===========================================================================*/
TokenList *lexer_tokenize(const char *source)
{
    /* TODO: implement */
    (void)source;
    return token_list_create();
}
