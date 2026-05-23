/*
 * =============================================================================
 *  CPE Language v1.0 - Lexer Implementation (Action/Conversational Syntax)
 * =============================================================================
 *  ตัว Lexer (Scanner) ทำหน้าที่อ่าน source code ทีละตัวอักษร
 *  แล้วแปลงเป็น Token ต่อเนื่องเก็บไว้ใน Linked List (TokenList)
 *
 *  ความแตกต่างจาก C-style Lexer:
 *  - ไม่มี semicolon (;) → ใช้ newline (\n) เป็นตัวจบ statement แทน
 *  - ไม่มี braces ({}) → ใช้ then/do/end keywords แทน
 *  - ไม่มี = (assign) → ใช้ keyword "to" แทน
 *  - Keywords ใหม่: set, as, to, show, then, do, end
 *
 *  Data Structure: Singly Linked List ที่มี tail pointer
 *  → append เป็น O(1) เพราะไม่ต้อง traverse หา node สุดท้าย
 * =============================================================================
 */

#include "lexer.h"
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---------------------------------------------------------------------------
 *  Helper: สร้าง Token node ใหม่
 *  - จอง memory สำหรับ Token struct
 *  - คัดลอกค่า value ลง heap (strdup)
 *  - ตั้ง next เป็น NULL (เพราะยังไม่ได้ต่อเข้า list)
 * ---------------------------------------------------------------------------*/
static Token *token_create(TokenType type, const char *value, int line) {
  Token *tok = (Token *)malloc(sizeof(Token));
  if (!tok) {
    fprintf(stderr, "[Lexer] Error: memory allocation failed\n");
    exit(1);
  }
  tok->type = type;
  tok->value = strdup(value); /* คัดลอก string ไปเก็บบน heap */
  tok->line = line;
  tok->next = NULL; /* ยังไม่ได้เชื่อมต่อ linked list */
  return tok;
}

/* ---------------------------------------------------------------------------
 *  token_list_create: สร้าง TokenList ว่าง
 *  - head = tail = NULL (ไม่มี node ใดๆ)
 *  - count = 0
 * ---------------------------------------------------------------------------*/
TokenList *token_list_create(void) {
  TokenList *list = (TokenList *)malloc(sizeof(TokenList));
  if (!list) {
    fprintf(stderr, "[Lexer] Error: memory allocation failed\n");
    exit(1);
  }
  list->head = NULL;
  list->tail = NULL;
  list->count = 0;
  return list;
}

/* ---------------------------------------------------------------------------
 *  token_list_append: เพิ่ม token ต่อท้าย linked list
 *
 *  Algorithm (Linked List Append with Tail Pointer):
 *  1. สร้าง Token node ใหม่
 *  2. ถ้า list ว่าง → head = tail = node ใหม่
 *  3. ถ้า list ไม่ว่าง → tail->next = node ใหม่, tail = node ใหม่
 *  4. เพิ่ม count
 *
 *  Time Complexity: O(1) เพราะมี tail pointer
 * ---------------------------------------------------------------------------*/
void token_list_append(TokenList *list, TokenType type, const char *value,
                       int line) {
  Token *tok = token_create(type, value, line);

  if (list->tail == NULL) {
    /* กรณี list ว่าง: node ใหม่เป็นทั้ง head และ tail */
    list->head = tok;
    list->tail = tok;
  } else {
    /* กรณี list มี node อยู่แล้ว: ต่อท้าย tail */
    list->tail->next = tok; /* เชื่อม node เดิมไปยัง node ใหม่ */
    list->tail = tok;       /* อัปเดต tail เป็น node ใหม่      */
  }
  list->count++;
}

/* ---------------------------------------------------------------------------
 *  token_list_free: ทำลาย TokenList พร้อมคืน memory ทั้งหมด
 *
 *  Algorithm (Linked List Traversal & Free):
 *  1. เริ่มจาก head
 *  2. เก็บ pointer ไปยัง node ถัดไปก่อน
 *  3. free ค่า value ของ node ปัจจุบัน
 *  4. free ตัว node ปัจจุบัน
 *  5. ย้ายไปยัง node ถัดไป วนจนครบ
 * ---------------------------------------------------------------------------*/
void token_list_free(TokenList *list) {
  if (!list)
    return;

  Token *cur = list->head;
  while (cur != NULL) {
    Token *next = cur->next; /* เก็บ pointer ก่อน free */
    free(cur->value);        /* คืน memory ของ string  */
    free(cur);               /* คืน memory ของ node    */
    cur = next;              /* ไปยัง node ถัดไป       */
  }
  free(list); /* คืน memory ของ list struct */
}

/* ---------------------------------------------------------------------------
 *  Keyword Table: ตารางคำสงวนของภาษา CPE (Action/Conversational)
 *  ใช้ linear search เทียบกับ identifier ที่อ่านได้
 * ---------------------------------------------------------------------------*/
typedef struct {
  const char *keyword;
  TokenType type;
} KeywordEntry;

static const KeywordEntry keywords[] = {
    {"set", TOKEN_SET},      {"as", TOKEN_AS},
    {"to", TOKEN_TO},        {"show", TOKEN_SHOW},
    {"if", TOKEN_IF},        {"else", TOKEN_ELSE},
    {"while", TOKEN_WHILE},  {"then", TOKEN_THEN},
    {"do", TOKEN_DO},        {"end", TOKEN_END},
    {"int", TOKEN_INT_TYPE}, {"string", TOKEN_STRING_TYPE},
    {NULL, TOKEN_EOF} /* sentinel (ตัวบอกจบ) */
};

/* ค้นหาว่า word เป็น keyword หรือไม่ คืน TokenType ที่ตรง หรือ TOKEN_IDENT */
static TokenType lookup_keyword(const char *word) {
  for (int i = 0; keywords[i].keyword != NULL; i++) {
    if (strcmp(word, keywords[i].keyword) == 0) {
      return keywords[i].type;
    }
  }
  return TOKEN_IDENT; /* ไม่ใช่ keyword → เป็น identifier */
}

/* ---------------------------------------------------------------------------
 *  lexer_tokenize: ฟังก์ชันหลักของ Lexer
 *
 *  รับ: source code เป็น string
 *  คืน: TokenList (linked list ของ tokens)
 *
 *  ทำงานโดยวน loop อ่านทีละตัวอักษร:
 *  - ข้ามช่องว่าง/tab (แต่ไม่ข้าม newline!)
 *  - ข้าม comment (// จนจบบรรทัด)
 *  - newline (\n) ถูกเปลี่ยนเป็น TOKEN_NEWLINE (ใช้แทน ;)
 *  - จำแนกประเภท token ตามตัวอักษรแรกที่พบ
 * ---------------------------------------------------------------------------*/
TokenList *lexer_tokenize(const char *source) {
  TokenList *list = token_list_create();
  int pos = 0;  /* ตำแหน่งปัจจุบันใน source string */
  int line = 1; /* หมายเลขบรรทัดปัจจุบัน          */
  int len = (int)strlen(source);

  while (pos < len) {
    char c = source[pos];

    /* --- ข้ามช่องว่างและ tab (แต่ไม่ข้าม newline) --- */
    if (c == ' ' || c == '\t' || c == '\r') {
      pos++;
      continue;
    }

    /* --- Newline → TOKEN_NEWLINE (ตัวจบ statement) ---
     * ไม่เพิ่ม NEWLINE ซ้ำถ้า token ก่อนหน้าเป็น NEWLINE อยู่แล้ว
     * เพื่อให้ parser ไม่ต้องจัดการกับบรรทัดว่าง */
    if (c == '\n') {
      /* เพิ่ม NEWLINE เฉพาะเมื่อ token ก่อนหน้าไม่ใช่ NEWLINE */
      if (list->tail == NULL || list->tail->type != TOKEN_NEWLINE) {
        token_list_append(list, TOKEN_NEWLINE, "\\n", line);
      }
      line++;
      pos++;
      continue;
    }

    /* --- ข้าม comment: // จนจบบรรทัด --- */
    if (c == '/' && pos + 1 < len && source[pos + 1] == '/') {
      pos += 2;
      while (pos < len && source[pos] != '\n') {
        pos++;
      }
      continue; /* ไม่ต้อง pos++ เพราะ \n จะถูกจัดการรอบถัดไป */
    }

    /* -----------------------------------------------------------
     *  ตัวเลข (Integer Literal)
     *  อ่านตัวเลขติดต่อกันทั้งหมดเก็บในบัฟเฟอร์
     * -----------------------------------------------------------*/
    if (isdigit(c)) {
      char buf[64];
      int bi = 0;
      while (pos < len && isdigit(source[pos]) && bi < 63) {
        buf[bi++] = source[pos++];
      }
      buf[bi] = '\0';
      token_list_append(list, TOKEN_INT_LIT, buf, line);
      continue;
    }

    /* -----------------------------------------------------------
     *  ตัวอักษร (Identifier หรือ Keyword)
     *  อ่านตัวอักษร/ตัวเลข/_ ติดต่อกัน
     *  แล้วเทียบกับตาราง keyword
     * -----------------------------------------------------------*/
    if (isalpha(c) || c == '_') {
      char buf[256];
      int bi = 0;
      while (pos < len && (isalnum(source[pos]) || source[pos] == '_') &&
             bi < 255) {
        buf[bi++] = source[pos++];
      }
      buf[bi] = '\0';

      TokenType kw = lookup_keyword(buf);
      token_list_append(list, kw, buf, line);
      continue;
    }

    /* -----------------------------------------------------------
     *  String Literal: "..."
     *  อ่านทุกตัวอักษรระหว่างเครื่องหมาย "
     *  รองรับ escape sequence พื้นฐาน: \n, \t, \\, \"
     * -----------------------------------------------------------*/
    if (c == '"') {
      pos++; /* ข้าม " เปิด */
      char buf[1024];
      int bi = 0;
      while (pos < len && source[pos] != '"' && bi < 1023) {
        if (source[pos] == '\\' && pos + 1 < len) {
          /* Escape sequences */
          pos++;
          switch (source[pos]) {
          case 'n':
            buf[bi++] = '\n';
            break;
          case 't':
            buf[bi++] = '\t';
            break;
          case '\\':
            buf[bi++] = '\\';
            break;
          case '"':
            buf[bi++] = '"';
            break;
          default:
            buf[bi++] = source[pos];
            break;
          }
          pos++;
        } else {
          buf[bi++] = source[pos++];
        }
      }
      if (pos < len && source[pos] == '"') {
        pos++; /* ข้าม " ปิด */
      } else {
        fprintf(stderr, "[Lexer] Error: unterminated string at line %d\n",
                line);
        exit(1);
      }
      buf[bi] = '\0';
      token_list_append(list, TOKEN_STRING_LIT, buf, line);
      continue;
    }

    /* -----------------------------------------------------------
     *  Operators & Delimiters (ตัวดำเนินการและตัวคั่น)
     *  หมายเหตุ: ไม่มี = (assign), ;, {, } ในภาษา CPE แล้ว
     *  มีแค่ == (เปรียบเทียบ) เท่านั้น
     * -----------------------------------------------------------*/
    switch (c) {
    case '+':
      token_list_append(list, TOKEN_PLUS, "+", line);
      pos++;
      break;
    case '-':
      token_list_append(list, TOKEN_MINUS, "-", line);
      pos++;
      break;
    case '*':
      token_list_append(list, TOKEN_STAR, "*", line);
      pos++;
      break;
    case '/':
      token_list_append(list, TOKEN_SLASH, "/", line);
      pos++;
      break;
    case '(':
      token_list_append(list, TOKEN_LPAREN, "(", line);
      pos++;
      break;
    case ')':
      token_list_append(list, TOKEN_RPAREN, ")", line);
      pos++;
      break;
    case '>':
      token_list_append(list, TOKEN_GT, ">", line);
      pos++;
      break;
    case '<':
      token_list_append(list, TOKEN_LT, "<", line);
      pos++;
      break;
    case '=':
      /* ตรวจสอบว่าเป็น == (เท่ากับ) — ไม่มี = เดี่ยวในภาษา CPE */
      if (pos + 1 < len && source[pos + 1] == '=') {
        token_list_append(list, TOKEN_EQ, "==", line);
        pos += 2;
      } else {
        fprintf(stderr,
                "[Lexer] Error: unexpected '=' at line %d "
                "(use 'set x to ...' for assignment)\n",
                line);
        exit(1);
      }
      break;

    default:
      fprintf(stderr, "[Lexer] Error: unexpected character '%c' at line %d\n",
              c, line);
      exit(1);
    }
  }

  /* เพิ่ม EOF token ที่ท้ายสุด เป็นสัญญาณบอก parser ว่าจบแล้ว */
  token_list_append(list, TOKEN_EOF, "EOF", line);
  return list;
}

/* ---------------------------------------------------------------------------
 *  token_type_name: แปลง TokenType เป็นชื่อ string (สำหรับ debug)
 * ---------------------------------------------------------------------------*/
const char *token_type_name(TokenType type) {
  switch (type) {
  case TOKEN_INT_LIT:
    return "INT_LIT";
  case TOKEN_STRING_LIT:
    return "STRING_LIT";
  case TOKEN_IDENT:
    return "IDENT";
  case TOKEN_SET:
    return "KW_SET";
  case TOKEN_AS:
    return "KW_AS";
  case TOKEN_TO:
    return "KW_TO";
  case TOKEN_SHOW:
    return "KW_SHOW";
  case TOKEN_IF:
    return "KW_IF";
  case TOKEN_ELSE:
    return "KW_ELSE";
  case TOKEN_WHILE:
    return "KW_WHILE";
  case TOKEN_THEN:
    return "KW_THEN";
  case TOKEN_DO:
    return "KW_DO";
  case TOKEN_END:
    return "KW_END";
  case TOKEN_INT_TYPE:
    return "KW_INT";
  case TOKEN_STRING_TYPE:
    return "KW_STRING";
  case TOKEN_PLUS:
    return "PLUS";
  case TOKEN_MINUS:
    return "MINUS";
  case TOKEN_STAR:
    return "STAR";
  case TOKEN_SLASH:
    return "SLASH";
  case TOKEN_EQ:
    return "EQ";
  case TOKEN_GT:
    return "GT";
  case TOKEN_LT:
    return "LT";
  case TOKEN_LPAREN:
    return "LPAREN";
  case TOKEN_RPAREN:
    return "RPAREN";
  case TOKEN_NEWLINE:
    return "NEWLINE";
  case TOKEN_EOF:
    return "EOF";
  default:
    return "UNKNOWN";
  }
}
