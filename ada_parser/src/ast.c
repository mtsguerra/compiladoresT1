#define _GNU_SOURCE
#include "ast.h"
#include <stdio.h>

ASTNode* ast_create_program(ASTNode *procedure) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = AST_PROGRAM;
    node->data.program.procedure = procedure;
    return node;
}

ASTNode* ast_create_procedure(const char *name, ASTNode *block) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = AST_PROCEDURE;
    node->data.procedure.name = strdup(name);
    node->data.procedure.block = block;
    return node;
}

ASTNode* ast_create_block(ASTNode **statements, int count) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = AST_BLOCK;
    node->data.block.statements = statements;
    node->data.block.count = count;
    return node;
}

ASTNode* ast_create_assignment(const char *identifier, ASTNode *expression) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = AST_ASSIGNMENT;
    node->data.assignment.identifier = strdup(identifier);
    node->data.assignment.expression = expression;
    return node;
}

ASTNode* ast_create_if(ASTNode *condition, ASTNode *then_block, ASTNode *else_block) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = AST_IF_STATEMENT;
    node->data.if_stmt.condition = condition;
    node->data.if_stmt.then_block = then_block;
    node->data.if_stmt.else_block = else_block;
    return node;
}

ASTNode* ast_create_while(ASTNode *condition, ASTNode *body) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = AST_WHILE_STATEMENT;
    node->data.while_stmt.condition = condition;
    node->data.while_stmt.body = body;
    return node;
}

ASTNode* ast_create_put_line(ASTNode *expression) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = AST_PUT_LINE;
    node->data.put_line.expression = expression;
    return node;
}

ASTNode* ast_create_get_line(const char *identifier) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = AST_GET_LINE;
    node->data.get_line.identifier = strdup(identifier);
    return node;
}

ASTNode* ast_create_binary_op(const char *operator, ASTNode *left, ASTNode *right) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = AST_BINARY_OP;
    node->data.binary_op.operator = strdup(operator);
    node->data.binary_op.left = left;
    node->data.binary_op.right = right;
    return node;
}

ASTNode* ast_create_unary_op(const char *operator, ASTNode *operand) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = AST_UNARY_OP;
    node->data.unary_op.operator = strdup(operator);
    node->data.unary_op.operand = operand;
    return node;
}

ASTNode* ast_create_integer(int value) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = AST_INTEGER;
    node->data.integer.value = value;
    return node;
}

ASTNode* ast_create_string(const char *value) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = AST_STRING;
    node->data.string.value = strdup(value);
    return node;
}

ASTNode* ast_create_identifier(const char *name) {
    ASTNode *node = (ASTNode*)malloc(sizeof(ASTNode));
    node->type = AST_IDENTIFIER;
    node->data.identifier.name = strdup(name);
    return node;
}

void ast_free(ASTNode *node) {
    if (!node) return;

    switch (node->type) {
        case AST_PROGRAM:
            ast_free(node->data.program.procedure);
            break;
        case AST_PROCEDURE:
            free(node->data.procedure.name);
            ast_free(node->data.procedure.block);
            break;
        case AST_BLOCK:
            for (int i = 0; i < node->data.block.count; i++) {
                ast_free(node->data.block.statements[i]);
            }
            free(node->data.block.statements);
            break;
        case AST_ASSIGNMENT:
            free(node->data.assignment.identifier);
            ast_free(node->data.assignment.expression);
            break;
        case AST_IF_STATEMENT:
            ast_free(node->data.if_stmt.condition);
            ast_free(node->data.if_stmt.then_block);
            ast_free(node->data.if_stmt.else_block);
            break;
        case AST_WHILE_STATEMENT:
            ast_free(node->data.while_stmt.condition);
            ast_free(node->data.while_stmt.body);
            break;
        case AST_PUT_LINE:
            ast_free(node->data.put_line.expression);
            break;
        case AST_GET_LINE:
            free(node->data.get_line.identifier);
            break;
        case AST_BINARY_OP:
            free(node->data.binary_op.operator);
            ast_free(node->data.binary_op.left);
            ast_free(node->data.binary_op.right);
            break;
        case AST_UNARY_OP:
            free(node->data.unary_op.operator);
            ast_free(node->data.unary_op.operand);
            break;
        case AST_STRING:
            free(node->data.string.value);
            break;
        case AST_IDENTIFIER:
            free(node->data.identifier.name);
            break;
        default:
            break;
    }

    free(node);
}

static void print_indent(int indent) {
    for (int i = 0; i < indent; i++) {
        printf("  ");
    }
}

void ast_print(ASTNode *node, int indent) {
    if (!node) {
        print_indent(indent);
        printf("(null)\n");
        return;
    }

    print_indent(indent);

    switch (node->type) {
        case AST_PROGRAM:
            printf("Program\n");
            ast_print(node->data.program.procedure, indent + 1);
            break;
        case AST_PROCEDURE:
            printf("Procedure: %s\n", node->data.procedure.name);
            ast_print(node->data.procedure.block, indent + 1);
            break;
        case AST_BLOCK:
            printf("Block (%d statements)\n", node->data.block.count);
            for (int i = 0; i < node->data.block.count; i++) {
                ast_print(node->data.block.statements[i], indent + 1);
            }
            break;
        case AST_ASSIGNMENT:
            printf("Assignment: %s :=\n", node->data.assignment.identifier);
            ast_print(node->data.assignment.expression, indent + 1);
            break;
        case AST_IF_STATEMENT:
            printf("If\n");
            print_indent(indent + 1);
            printf("Condition:\n");
            ast_print(node->data.if_stmt.condition, indent + 2);
            print_indent(indent + 1);
            printf("Then:\n");
            ast_print(node->data.if_stmt.then_block, indent + 2);
            if (node->data.if_stmt.else_block) {
                print_indent(indent + 1);
                printf("Else:\n");
                ast_print(node->data.if_stmt.else_block, indent + 2);
            }
            break;
        case AST_WHILE_STATEMENT:
            printf("While\n");
            print_indent(indent + 1);
            printf("Condition:\n");
            ast_print(node->data.while_stmt.condition, indent + 2);
            print_indent(indent + 1);
            printf("Body:\n");
            ast_print(node->data.while_stmt.body, indent + 2);
            break;
        case AST_PUT_LINE:
            printf("Put_Line\n");
            ast_print(node->data.put_line.expression, indent + 1);
            break;
        case AST_GET_LINE:
            printf("Get_Line: %s\n", node->data.get_line.identifier);
            break;
        case AST_BINARY_OP:
            printf("BinaryOp: %s\n", node->data.binary_op.operator);
            ast_print(node->data.binary_op.left, indent + 1);
            ast_print(node->data.binary_op.right, indent + 1);
            break;
        case AST_UNARY_OP:
            printf("UnaryOp: %s\n", node->data.unary_op.operator);
            ast_print(node->data.unary_op.operand, indent + 1);
            break;
        case AST_INTEGER:
            printf("Integer: %d\n", node->data.integer.value);
            break;
        case AST_STRING:
            printf("String: \"%s\"\n", node->data.string.value);
            break;
        case AST_IDENTIFIER:
            printf("Identifier: %s\n", node->data.identifier.name);
            break;
    }
}