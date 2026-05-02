/*
 * =============================================================================
 * CPE Language v1.0 - Parser (AST Builder)
 * =============================================================================
 * ผู้รับผิดชอบ: IKKIW
 * Branch: feature/parser
 *
 * หน้าที่:
 * - รับ Token Linked List จาก Lexer
 * - สร้าง Abstract Syntax Tree (AST) ด้วย Recursive Descent Parsing
 * - จัดการ operator precedence (comparison → addition → multiply → primary)
 * =============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"

/* ---------------------------------------------------------------------------
 * Parser State
 * ---------------------------------------------------------------------------*/
static Token *current_token = NULL;

/* ---------------------------------------------------------------------------
 * Helper Functions
 * ---------------------------------------------------------------------------*/
static void advance(void)
{
    if (current_token && current_token->next)
        current_token = current_token->next;
}

static void expect(TokenType type)
{
    if (!current_token || current_token->type != type) {
        fprintf(stderr, "[CPE] Parse error at line %d: expected token type %d, got %d\n",
                current_token ? current_token->line : 0, type,
                current_token ? current_token->type : TOKEN_EOF);
        exit(1);
    }
    advance();
}

static void skip_newlines(void)
{
    while (current_token && current_token->type == TOKEN_NEWLINE)
        advance();
}

/* ===========================================================================
 * TODO 1: ast_node_create
 * ===========================================================================*/
ASTNode *ast_node_create(ASTNodeType type)
{
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    if (!node) {
        fprintf(stderr, "[CPE] Memory allocation failed!\n");
        exit(1);
    }
    memset(node, 0, sizeof(ASTNode));
    node->type = type;
    return node;
}

/* ===========================================================================
 * TODO 2: ast_node_add_child
 * ===========================================================================*/
void ast_node_add_child(ASTNode *parent, ASTNode *child)
{
    if (!parent || !child) return;
    if (parent->child_count < AST_MAX_CHILDREN) {
        parent->children[parent->child_count++] = child;
    }
}

/* ===========================================================================
 * TODO 3: ast_node_free
 * ===========================================================================*/
void ast_node_free(ASTNode *node)
{
    if (!node) return;
    for (int i = 0; i < node->child_count; i++) {
        ast_node_free(node->children[i]);
    }
    if (node->value) free(node->value);
    if (node->name) free(node->name);
    if (node->op) free(node->op);
    if (node->var_type) free(node->var_type);
    free(node);
}

/* ===========================================================================
 * TODO 4: Forward declarations
 * ===========================================================================*/
static ASTNode *parse_statement(void);
static ASTNode *parse_expression(void);
static ASTNode *parse_comparison(void);
static ASTNode *parse_addition(void);
static ASTNode *parse_multiply(void);
static ASTNode *parse_primary(void);

/* ===========================================================================
 * TODO 5: parse_primary
 * ===========================================================================*/
static ASTNode *parse_primary(void)
{
    if (!current_token) return NULL;

    if (current_token->type == TOKEN_INT_LIT) {
        ASTNode *node = ast_node_create(AST_INT_LIT);
        if (current_token->value) node->value = strdup(current_token->value);
        advance();
        return node;
    }
    if (current_token->type == TOKEN_STRING_LIT) {
        ASTNode *node = ast_node_create(AST_STRING_LIT);
        if (current_token->value) node->value = strdup(current_token->value);
        advance();
        return node;
    }
    if (current_token->type == TOKEN_IDENT) {
        ASTNode *node = ast_node_create(AST_IDENT);
        if (current_token->value) node->name = strdup(current_token->value);
        advance();
        return node;
    }
    if (current_token->type == TOKEN_LPAREN) {
        advance();
        ASTNode *expr = parse_expression();
        expect(TOKEN_RPAREN);
        return expr;
    }
    
    fprintf(stderr, "[CPE] Parse error line %d: unexpected '%s'\n",
            current_token->line, current_token->value ? current_token->value : "EOF");
    exit(1);
}

/* ===========================================================================
 * TODO 6: parse_multiply
 * ===========================================================================*/
static ASTNode *parse_multiply(void)
{
    ASTNode *left = parse_primary();
    while (current_token && (current_token->type == TOKEN_STAR || current_token->type == TOKEN_SLASH)) {
        ASTNode *op_node = ast_node_create(AST_BINARY_OP);
        if (current_token->value) op_node->op = strdup(current_token->value);
        advance();
        ASTNode *right = parse_primary();
        ast_node_add_child(op_node, left);
        ast_node_add_child(op_node, right);
        left = op_node;
    }
    return left;
}

/* ===========================================================================
 * TODO 7: parse_addition
 * ===========================================================================*/
static ASTNode *parse_addition(void)
{
    ASTNode *left = parse_multiply();
    while (current_token && (current_token->type == TOKEN_PLUS || current_token->type == TOKEN_MINUS)) {
        ASTNode *op_node = ast_node_create(AST_BINARY_OP);
        if (current_token->value) op_node->op = strdup(current_token->value);
        advance();
        ASTNode *right = parse_multiply();
        ast_node_add_child(op_node, left);
        ast_node_add_child(op_node, right);
        left = op_node;
    }
    return left;
}

/* ===========================================================================
 * TODO 8: parse_comparison
 * ===========================================================================*/
static ASTNode *parse_comparison(void)
{
    ASTNode *left = parse_addition();
    while (current_token && (current_token->type == TOKEN_GT || 
                             current_token->type == TOKEN_LT || 
                             current_token->type == TOKEN_EQEQ)) {
        ASTNode *op_node = ast_node_create(AST_BINARY_OP);
        if (current_token->value) op_node->op = strdup(current_token->value);
        advance();
        ASTNode *right = parse_addition();
        ast_node_add_child(op_node, left);
        ast_node_add_child(op_node, right);
        left = op_node;
    }
    return left;
}

/* ===========================================================================
 * TODO 9: parse_expression
 * ===========================================================================*/
static ASTNode *parse_expression(void)
{
    return parse_comparison();
}

/* ===========================================================================
 * TODO 10: parse_statement
 * ===========================================================================*/
static ASTNode *parse_statement(void)
{
    if (!current_token) return NULL;

    if (current_token->type == TOKEN_SET) {
        advance(); 
        
        if (current_token->type != TOKEN_IDENT) {
            fprintf(stderr, "[CPE] Parse error line %d: expected identifier after 'set'\n", current_token->line);
            exit(1);
        }
        
        char *var_name = strdup(current_token->value);
        advance(); 
        
        if (current_token->type == TOKEN_AS) {
            advance(); 
            ASTNode *decl_node = ast_node_create(AST_VAR_DECL);
            decl_node->name = var_name;
            
            if (current_token->type == TOKEN_IDENT) {
                decl_node->var_type = strdup(current_token->value);
                advance();
            }
            
            expect(TOKEN_TO);
            ast_node_add_child(decl_node, parse_expression());
            return decl_node;
            
        } else if (current_token->type == TOKEN_TO) {
            advance(); 
            ASTNode *assign_node = ast_node_create(AST_ASSIGN);
            assign_node->name = var_name;
            ast_node_add_child(assign_node, parse_expression());
            return assign_node;
            
        } else {
            fprintf(stderr, "[CPE] Parse error line %d: expected 'as' or 'to'\n", current_token->line);
            free(var_name);
            exit(1);
        }
    }
    
    if (current_token->type == TOKEN_SHOW) {
        advance();
        ASTNode *node = ast_node_create(AST_PRINT);
        ast_node_add_child(node, parse_expression());
        return node;
    }
    
    if (current_token->type == TOKEN_IF) {
        advance();
        ASTNode *node = ast_node_create(AST_IF);
        
        ast_node_add_child(node, parse_expression());
        
        expect(TOKEN_THEN); 
        
        ASTNode *then_block = ast_node_create(AST_BLOCK);
        skip_newlines();
        while (current_token && current_token->type != TOKEN_END && current_token->type != TOKEN_ELSE) {
            ast_node_add_child(then_block, parse_statement());
            skip_newlines();
        }
        ast_node_add_child(node, then_block);
        
        if (current_token && current_token->type == TOKEN_ELSE) {
            advance();
            ASTNode *else_block = ast_node_create(AST_BLOCK);
            skip_newlines();
            while (current_token && current_token->type != TOKEN_END) {
                ast_node_add_child(else_block, parse_statement());
                skip_newlines();
            }
            ast_node_add_child(node, else_block);
        }
        
        expect(TOKEN_END);
        return node;
    }
    
    if (current_token->type == TOKEN_WHILE) {
        advance();
        ASTNode *node = ast_node_create(AST_WHILE);
        
        ast_node_add_child(node, parse_expression());
        
        expect(TOKEN_DO); 
        
        ASTNode *body_block = ast_node_create(AST_BLOCK);
        skip_newlines();
        while (current_token && current_token->type != TOKEN_END) {
            ast_node_add_child(body_block, parse_statement());
            skip_newlines();
        }
        ast_node_add_child(node, body_block);
        
        expect(TOKEN_END);
        return node;
    }
    
    fprintf(stderr, "[CPE] Parse error line %d: unexpected statement token '%s'\n", 
            current_token->line, current_token->value ? current_token->value : "");
    exit(1);
}

/* ===========================================================================
 * TODO 11: parser_parse
 * ===========================================================================*/
ASTNode *parser_parse(TokenList *tokens)
{
    if (!tokens || !tokens->head) return NULL;
    
    current_token = tokens->head;
    ASTNode *program = ast_node_create(AST_PROGRAM);
    
    skip_newlines();
    while (current_token && current_token->type != TOKEN_EOF) {
        ast_node_add_child(program, parse_statement());
        skip_newlines();
    }
    
    return program;
}