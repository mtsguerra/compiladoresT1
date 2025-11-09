#ifndef AST_H
#define AST_H

#include <stdlib.h>
#include <string.h>

// AST, tipos de nós
typedef enum {
    AST_PROGRAM,
    AST_PROCEDURE,
    AST_BLOCK,
    AST_ASSIGNMENT,
    AST_IF_STATEMENT,
    AST_WHILE_STATEMENT,
    AST_PUT_LINE,
    AST_GET_LINE,
    AST_BINARY_OP,
    AST_UNARY_OP,
    AST_INTEGER,
    AST_STRING,
    AST_IDENTIFIER
} ASTNodeType;

typedef struct ASTNode {
    ASTNodeType type;
    union {
        // Programa
        struct {
            struct ASTNode *procedure;
        } program;

        // Procedimento
        struct {
            char *name;
            struct ASTNode *block;
        } procedure;

        // Block, lista de statements
        struct {
            struct ASTNode **statements;
            int count;
        } block;

        // Atribuição: identificador recebe expressão
        struct {
            char *identifier;
            struct ASTNode *expression;
        } assignment;

        // If statement
        struct {
            struct ASTNode *condition;
            struct ASTNode *then_block;
            struct ASTNode *else_block; // possivelmente nulo
        } if_stmt;

        // While loop
        struct {
            struct ASTNode *condition;
            struct ASTNode *body;
        } while_stmt;

        // Put_Line, output
        struct {
            struct ASTNode *expression;
        } put_line;

        // Get_Line, input
        struct {
            char *identifier;
        } get_line;

        // Operação binária
        struct {
            char *operator;
            struct ASTNode *left;
            struct ASTNode *right;
        } binary_op;

        // Operação unária
        struct {
            char *operator;
            struct ASTNode *operand;
        } unary_op;

        // Literais
        struct {
            int value;
        } integer;

        // String literal
        struct {
            char *value;
        } string;

        // Identificador
        struct {
            char *name;
        } identifier;
    } data;
} ASTNode;

// Protótipos dos construtores
ASTNode* ast_create_program(ASTNode *procedure);
ASTNode* ast_create_procedure(const char *name, ASTNode *block);
ASTNode* ast_create_block(ASTNode **statements, int count);
ASTNode* ast_create_assignment(const char *identifier, ASTNode *expression);
ASTNode* ast_create_if(ASTNode *condition, ASTNode *then_block, ASTNode *else_block);
ASTNode* ast_create_while(ASTNode *condition, ASTNode *body);
ASTNode* ast_create_put_line(ASTNode *expression);
ASTNode* ast_create_get_line(const char *identifier);
ASTNode* ast_create_binary_op(const char *operator, ASTNode *left, ASTNode *right);
ASTNode* ast_create_unary_op(const char *operator, ASTNode *operand);
ASTNode* ast_create_integer(int value);
ASTNode* ast_create_string(const char *value);
ASTNode* ast_create_identifier(const char *name);

// Auxiliares
void ast_free(ASTNode *node);
void ast_print(ASTNode *node, int indent);

#endif