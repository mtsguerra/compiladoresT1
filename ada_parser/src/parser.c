#include "parser.h"
#include <stdio.h>

static void error(Parser *parser, const char *message) {
    fprintf(stderr, "Parse error at line %d, column %d: %s\n",
            parser->current_token->line,
            parser->current_token->column,
            message);
    fprintf(stderr, "Current token: %s (%s)\n",
            token_type_to_string(parser->current_token->type),
            parser->current_token->value);
    exit(1);
}

static void advance(Parser *parser) {
    token_free(parser->current_token);
    parser->current_token = lexer_next_token(parser->lexer);
}

static void expect(Parser *parser, TokenType type) {
    if (parser->current_token->type != type) {
        char msg[256];
        sprintf(msg, "Expected %s, got %s",
                token_type_to_string(type),
                token_type_to_string(parser->current_token->type));
        error(parser, msg);
    }
    advance(parser);
}

Parser* parser_create(Lexer *lexer) {
    Parser *parser = (Parser*)malloc(sizeof(Parser));
    parser->lexer = lexer;
    parser->current_token = lexer_next_token(lexer);
    return parser;
}

void parser_free(Parser *parser) {
    if (parser) {
        token_free(parser->current_token);
        free(parser);
    }
}

// Declarações das funções de parsing
static ASTNode* parse_expression(Parser *parser);
static ASTNode* parse_statement(Parser *parser);
static ASTNode* parse_block(Parser *parser);

// Parse expressão primária (números, identificadores, parênteses, Put_Line, NOT)
static ASTNode* parse_primary(Parser *parser) {
    Token *token = parser->current_token;

    if (token->type == TOKEN_INTEGER) {
        int value = atoi(token->value);
        advance(parser);
        return ast_create_integer(value);
    }

    if (token->type == TOKEN_STRING) {
        char *value = strdup(token->value);
        advance(parser);
        ASTNode *node = ast_create_string(value);
        free(value);
        return node;
    }

    if (token->type == TOKEN_IDENTIFIER) {
        char *name = strdup(token->value);
        advance(parser);
        ASTNode *node = ast_create_identifier(name);
        free(name);
        return node;
    }

    if (token->type == TOKEN_LPAREN) {
        advance(parser);
        ASTNode *expr = parse_expression(parser);
        expect(parser, TOKEN_RPAREN);
        return expr;
    }

    if (token->type == TOKEN_PUT_LINE) {
        advance(parser);
        expect(parser, TOKEN_LPAREN);
        ASTNode *expr = parse_expression(parser);
        expect(parser, TOKEN_RPAREN);
        return ast_create_put_line(expr);
    }

    if (token->type == TOKEN_NOT) {
        advance(parser);
        ASTNode *operand = parse_primary(parser);
        return ast_create_unary_op("NOT", operand);
    }

    error(parser, "Unexpected token in expression");
    return NULL;
}

// Parse expressão multiplicativa (*, /)
static ASTNode* parse_multiplicative(Parser *parser) {
    ASTNode *left = parse_primary(parser);

    while (parser->current_token->type == TOKEN_MULTIPLY ||
           parser->current_token->type == TOKEN_DIVIDE) {
        char *op = strdup(parser->current_token->value);
        advance(parser);
        ASTNode *right = parse_primary(parser);
        left = ast_create_binary_op(op, left, right);
        free(op);
    }

    return left;
}

// Parse expressão aditiva (+, -)
static ASTNode* parse_additive(Parser *parser) {
    ASTNode *left = parse_multiplicative(parser);

    while (parser->current_token->type == TOKEN_PLUS ||
           parser->current_token->type == TOKEN_MINUS) {
        char *op = strdup(parser->current_token->value);
        advance(parser);
        ASTNode *right = parse_multiplicative(parser);
        left = ast_create_binary_op(op, left, right);
        free(op);
    }

    return left;
}

// Parse expressão relacional (<, <=, >, >=, =, /=)
static ASTNode* parse_relational(Parser *parser) {
    ASTNode *left = parse_additive(parser);

    while (parser->current_token->type == TOKEN_LESS ||
           parser->current_token->type == TOKEN_LESS_EQUAL ||
           parser->current_token->type == TOKEN_GREATER ||
           parser->current_token->type == TOKEN_GREATER_EQUAL ||
           parser->current_token->type == TOKEN_EQUAL ||
           parser->current_token->type == TOKEN_NOT_EQUAL) {
        char *op = strdup(parser->current_token->value);
        advance(parser);
        ASTNode *right = parse_additive(parser);
        left = ast_create_binary_op(op, left, right);
        free(op);
    }

    return left;
}

// Parse expressão lógica AND
static ASTNode* parse_and(Parser *parser) {
    ASTNode *left = parse_relational(parser);

    while (parser->current_token->type == TOKEN_AND) {
        advance(parser);
        ASTNode *right = parse_relational(parser);
        left = ast_create_binary_op("AND", left, right);
    }

    return left;
}

// Parse expressão lógica OR
static ASTNode* parse_or(Parser *parser) {
    ASTNode *left = parse_and(parser);

    while (parser->current_token->type == TOKEN_OR) {
        advance(parser);
        ASTNode *right = parse_and(parser);
        left = ast_create_binary_op("OR", left, right);
    }

    return left;
}

// Parse expressão
static ASTNode* parse_expression(Parser *parser) {
    return parse_or(parser);
}

// Parse declaração de atribuição
static ASTNode* parse_assignment(Parser *parser) {
    char *identifier = strdup(parser->current_token->value);
    expect(parser, TOKEN_IDENTIFIER);
    expect(parser, TOKEN_ASSIGN);
    ASTNode *expr = parse_expression(parser);
    expect(parser, TOKEN_SEMICOLON);

    ASTNode *node = ast_create_assignment(identifier, expr);
    free(identifier);
    return node;
}

// Parse declaração if
static ASTNode* parse_if(Parser *parser) {
    expect(parser, TOKEN_IF);
    ASTNode *condition = parse_expression(parser);
    expect(parser, TOKEN_THEN);

    ASTNode *then_block = parse_block(parser);

    ASTNode *else_block = NULL;
    if (parser->current_token->type == TOKEN_ELSE) {
        advance(parser);
        else_block = parse_block(parser);
    }

    expect(parser, TOKEN_END);
    expect(parser, TOKEN_IF);
    expect(parser, TOKEN_SEMICOLON);

    return ast_create_if(condition, then_block, else_block);
}

// Parse while loop
static ASTNode* parse_while(Parser *parser) {
    expect(parser, TOKEN_WHILE);
    ASTNode *condition = parse_expression(parser);
    expect(parser, TOKEN_LOOP);

    ASTNode *body = parse_block(parser);

    expect(parser, TOKEN_END);
    expect(parser, TOKEN_LOOP);
    expect(parser, TOKEN_SEMICOLON);

    return ast_create_while(condition, body);
}

// Parse declaração Put_Line
static ASTNode* parse_put_line(Parser *parser) {
    expect(parser, TOKEN_PUT_LINE);
    expect(parser, TOKEN_LPAREN);
    ASTNode *expr = parse_expression(parser);
    expect(parser, TOKEN_RPAREN);
    expect(parser, TOKEN_SEMICOLON);

    return ast_create_put_line(expr);
}

// Parse declaração Get_Line
static ASTNode* parse_get_line(Parser *parser) {
    expect(parser, TOKEN_GET_LINE);
    expect(parser, TOKEN_LPAREN);
    char *identifier = strdup(parser->current_token->value);
    expect(parser, TOKEN_IDENTIFIER);
    expect(parser, TOKEN_RPAREN);
    expect(parser, TOKEN_SEMICOLON);

    ASTNode *node = ast_create_get_line(identifier);
    free(identifier);
    return node;
}

// Parse declaração (assignment, if, while, put_line, get_line)
static ASTNode* parse_statement(Parser *parser) {
    Token *token = parser->current_token;

    if (token->type == TOKEN_IDENTIFIER) {
        return parse_assignment(parser);
    }

    if (token->type == TOKEN_IF) {
        return parse_if(parser);
    }

    if (token->type == TOKEN_WHILE) {
        return parse_while(parser);
    }

    if (token->type == TOKEN_PUT_LINE) {
        return parse_put_line(parser);
    }

    if (token->type == TOKEN_GET_LINE) {
        return parse_get_line(parser);
    }

    error(parser, "Expected statement");
    return NULL;
}

// Parse block de declarações
static ASTNode* parse_block(Parser *parser) {
    ASTNode **statements = NULL;
    int count = 0;
    int capacity = 10;

    statements = (ASTNode**)malloc(capacity * sizeof(ASTNode*));

    while (parser->current_token->type != TOKEN_END &&
           parser->current_token->type != TOKEN_ELSE &&
           parser->current_token->type != TOKEN_EOF) {

        if (count >= capacity) {
            capacity *= 2;
            statements = (ASTNode**)realloc(statements, capacity * sizeof(ASTNode*));
        }

        statements[count++] = parse_statement(parser);
    }

    return ast_create_block(statements, count);
}

// Parse procedimento principal
static ASTNode* parse_procedure(Parser *parser) {
    expect(parser, TOKEN_PROCEDURE);

    char *name = strdup(parser->current_token->value);
    expect(parser, TOKEN_IDENTIFIER);

    expect(parser, TOKEN_IS);
    expect(parser, TOKEN_BEGIN);

    ASTNode *block = parse_block(parser);

    expect(parser, TOKEN_END);

    // Opcionalmente, o nome do procedimento pode ser repetido
    if (parser->current_token->type == TOKEN_IDENTIFIER) {
        advance(parser);
    }

    expect(parser, TOKEN_SEMICOLON);

    ASTNode *node = ast_create_procedure(name, block);
    free(name);
    return node;
}

// Parse programa completo
ASTNode* parser_parse(Parser *parser) {
    ASTNode *procedure = parse_procedure(parser);

    if (parser->current_token->type != TOKEN_EOF) {
        error(parser, "Expected end of file");
    }

    return ast_create_program(procedure);
}