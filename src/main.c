/*
 * =============================================================================
 *  CPE Language v1.0 - Main Entry Point
 * =============================================================================
 *  ไฟล์นี้เป็น entry point ของโปรแกรม cpe.exe
 *
 *  Pipeline การทำงาน:
 *  1. อ่านไฟล์ .cpe เข้า memory
 *  2. Lexer: แปลง source code → Token Linked List
 *  3. Parser: แปลง Token List → AST (Tree)
 *  4. Evaluator: Traverse AST → ประมวลผลโปรแกรม
 *  5. Cleanup: คืน memory ทั้งหมด
 *
 *  การใช้งาน: cpe.exe <filename.cpe>
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "env.h"
#include "eval.h"

/* ---------------------------------------------------------------------------
 *  read_file: อ่านไฟล์ทั้งหมดเข้า memory เป็น string
 *
 *  วิธีการ:
 *  1. เปิดไฟล์แบบ binary mode (เพื่อไม่ให้ OS แก้ไข line endings)
 *  2. หาขนาดไฟล์ด้วย fseek + ftell
 *  3. จอง memory ตาม ขนาดไฟล์ + 1 (สำหรับ null terminator)
 *  4. อ่านทั้งไฟล์ด้วย fread
 *  5. ปิดไฟล์แล้วคืน string
 * ---------------------------------------------------------------------------*/
static char *read_file(const char *path)
{
    FILE *fp = fopen(path, "rb");
    if (!fp) {
        fprintf(stderr, "[CPE] Error: cannot open file '%s'\n", path);
        exit(1);
    }

    /* หาขนาดไฟล์ */
    fseek(fp, 0, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    /* จอง memory */
    char *buf = (char *)malloc(size + 1);
    if (!buf) {
        fprintf(stderr, "[CPE] Error: memory allocation failed\n");
        fclose(fp);
        exit(1);
    }

    /* อ่านไฟล์ทั้งหมด */
    size_t read_bytes = fread(buf, 1, size, fp);
    buf[read_bytes] = '\0';   /* null-terminate */

    fclose(fp);
    return buf;
}

/* ---------------------------------------------------------------------------
 *  banner: แสดงข้อความต้อนรับ
 * ---------------------------------------------------------------------------*/
static void print_banner(void)
{
    printf("=== CPE Language v1.0 ===\n");
}

/* ---------------------------------------------------------------------------
 *  main: จุดเริ่มต้นของโปรแกรม
 * ---------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    /* ตรวจสอบว่ามี argument (ชื่อไฟล์) หรือไม่ */
    if (argc < 2) {
        print_banner();
        fprintf(stderr, "Usage: cpe <filename.cpe>\n");
        return 1;
    }

    const char *filename = argv[1];

    /* ============================================
     *  Phase 1: อ่านไฟล์ Source Code
     * ============================================*/
    char *source = read_file(filename);

    /* ============================================
     *  Phase 2: Lexer - แปลง source → Token List
     *  ผลลัพธ์: Linked List ของ Tokens
     * ============================================*/
    TokenList *tokens = lexer_tokenize(source);

    /* ============================================
     *  Phase 3: Parser - แปลง Token List → AST
     *  ผลลัพธ์: AST (N-ary Tree) ที่มี root เป็น PROGRAM node
     * ============================================*/
    ASTNode *ast = parser_parse(tokens);

    /* ============================================
     *  Phase 4: Evaluator - Traverse AST แล้วประมวลผล
     *  ใช้ Global Environment (Hash Table) เก็บตัวแปร
     * ============================================*/
    Env *global_env = env_create(NULL);   /* scope ระดับบนสุด, parent = NULL */
    Value result = eval(ast, global_env);
    value_free(&result);

    /* ============================================
     *  Phase 5: Cleanup - คืน memory ทั้งหมด
     * ============================================*/
    env_destroy(global_env);   /* ทำลาย Hash Table */
    ast_node_free(ast);        /* ทำลาย AST (Tree)  */
    token_list_free(tokens);   /* ทำลาย Token List (Linked List) */
    free(source);              /* คืน memory ของ source code */

    return 0;
}
