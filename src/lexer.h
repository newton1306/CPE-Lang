/*
 * =============================================================================
 *  CPE Language v1.0 - Lexer Header (Action/Conversational Syntax)
 * =============================================================================
 *  ไฟล์นี้กำหนดโครงสร้างข้อมูลสำหรับ Lexer (ตัววิเคราะห์คำ)
 *
 *  Data Structure ที่ใช้:
 *  - Token: โครงสร้างข้อมูลพื้นฐานที่เก็บข้อมูลของแต่ละ token
 *  - TokenList (Linked List): ใช้เก็บลำดับของ tokens ทั้งหมด
 *    โดยแต่ละ node มี pointer ชี้ไปยัง node ถัดไป (singly linked list)
 * =============================================================================
 */

#ifndef CPE_LEXER_H
#define CPE_LEXER_H

/* ---------------------------------------------------------------------------
 *  Token Types (ประเภทของ Token)
 *  แต่ละ token ที่ lexer สร้างขึ้นจะถูกกำหนดประเภทตาม enum นี้
 * ---------------------------------------------------------------------------*/
typedef enum {
    /* --- Literals (ค่าคงที่) --- */
    TOKEN_INT_LIT,       /* ตัวเลขจำนวนเต็ม เช่น 42, 100           */
    TOKEN_STRING_LIT,    /* สตริง เช่น "hello world"                */

    /* --- Identifier --- */
    TOKEN_IDENT,         /* ชื่อตัวแปร เช่น x, count, name          */

    /* --- Keywords (คำสงวน - Action/Conversational style) --- */
    TOKEN_SET,           /* คำสงวน 'set'   → ประกาศ/กำหนดค่าตัวแปร  */
    TOKEN_AS,            /* คำสงวน 'as'    → ระบุชนิดตัวแปร          */
    TOKEN_TO,            /* คำสงวน 'to'    → กำหนดค่า                */
    TOKEN_SHOW,          /* คำสงวน 'show'  → แสดงผล                  */
    TOKEN_IF,            /* คำสงวน 'if'    → เงื่อนไข                */
    TOKEN_ELSE,          /* คำสงวน 'else'  → ทางเลือก                */
    TOKEN_WHILE,         /* คำสงวน 'while' → วนรอบ                   */
    TOKEN_THEN,          /* คำสงวน 'then'  → เริ่มบล็อก if            */
    TOKEN_DO,            /* คำสงวน 'do'    → เริ่มบล็อก while         */
    TOKEN_END,           /* คำสงวน 'end'   → จบบล็อก                 */
    TOKEN_INT_TYPE,      /* คำสงวน 'int'   → ชนิดจำนวนเต็ม           */
    TOKEN_STRING_TYPE,   /* คำสงวน 'string'→ ชนิดสตริง               */

    /* --- Operators (ตัวดำเนินการ) --- */
    TOKEN_PLUS,          /* +                                       */
    TOKEN_MINUS,         /* -                                       */
    TOKEN_STAR,          /* *                                       */
    TOKEN_SLASH,         /* /                                       */
    TOKEN_EQ,            /* ==                                      */
    TOKEN_GT,            /* >                                       */
    TOKEN_LT,            /* <                                       */

    /* --- Delimiters (ตัวคั่น) --- */
    TOKEN_LPAREN,        /* (                                       */
    TOKEN_RPAREN,        /* )                                       */
    TOKEN_NEWLINE,       /* ขึ้นบรรทัดใหม่ (ใช้แทน ; เป็นตัวจบ statement) */

    /* --- Special --- */
    TOKEN_EOF            /* จุดสิ้นสุดของไฟล์                        */
} TokenType;

/* ---------------------------------------------------------------------------
 *  Token Structure (โครงสร้าง Token)
 *  แต่ละ token เก็บ:
 *    - type  : ประเภทของ token
 *    - value : ค่าข้อความดิบของ token (เช่น "42", "hello", "x")
 *    - line  : หมายเลขบรรทัดที่พบ token นี้ (สำหรับแจ้ง error)
 *    - next  : pointer ไปยัง token ถัดไปใน linked list
 * ---------------------------------------------------------------------------*/
typedef struct Token {
    TokenType    type;
    char        *value;    /* dynamically allocated string */
    int          line;     /* source line number           */
    struct Token *next;    /* → next node in linked list   */
} Token;

/* ---------------------------------------------------------------------------
 *  TokenList Structure (โครงสร้าง Linked List ของ Tokens)
 *  ทำหน้าที่เป็น Queue: เพิ่ม token ที่ท้าย (tail) อ่าน token จากหัว (head)
 *    - head  : pointer ไปยัง token แรก
 *    - tail  : pointer ไปยัง token สุดท้าย (สำหรับ O(1) append)
 *    - count : จำนวน token ทั้งหมด
 * ---------------------------------------------------------------------------*/
typedef struct {
    Token *head;
    Token *tail;
    int    count;
} TokenList;

/* ---------------------------------------------------------------------------
 *  Public API
 * ---------------------------------------------------------------------------*/

/* สร้าง TokenList ว่าง */
TokenList *token_list_create(void);

/* เพิ่ม token ใหม่ต่อท้าย linked list (O(1) ด้วย tail pointer) */
void token_list_append(TokenList *list, TokenType type,
                       const char *value, int line);

/* ทำลาย TokenList พร้อมคืน memory ทั้งหมด */
void token_list_free(TokenList *list);

/* ฟังก์ชันหลัก: รับ source code แล้วคืน TokenList */
TokenList *lexer_tokenize(const char *source);

/* แปลง TokenType เป็นชื่อ string (สำหรับ debug) */
const char *token_type_name(TokenType type);

#endif /* CPE_LEXER_H */
