#include "lexer.h"

Lexer* lexer_create(const char *source) {
    Lexer *lexer = (Lexer*)malloc(sizeof(Lexer));
    lexer->source = strdup(source);
    lexer->position = 0;
    lexer->line = 1;
    lexer->column = 1;
    lexer->current_char = lexer->source[0];
    return lexer;
}

void lexer_free(Lexer *lexer) {
    if (lexer) {
        free(lexer->source);
        free(lexer);
    }
}

void token_free(Token *token) {
    if (token) {
        free(token->value);
        free(token);
    }
}

static void advance(Lexer *lexer) {
    if (lexer->current_char == '\n') {
        lexer->line++;
        lexer->column = 1;
    } else {
        lexer->column++;
    }

    lexer->position++;
    if (lexer->position < strlen(lexer->source)) {
        lexer->current_char = lexer->source[lexer->position];
    } else {
        lexer->current_char = '\0';
    }
}

static char peek(Lexer *lexer, int offset) {
    int pos = lexer->position + offset;
    if (pos < strlen(lexer->source)) {
        return lexer->source[pos];
    }
    return '\0';
}

static void skip_whitespace(Lexer *lexer) {
    while (lexer->current_char != '\0' && isspace(lexer->current_char)) {
        advance(lexer);
    }
}

static void skip_comment(Lexer *lexer) {
    // Comentários começam com --
    if (lexer->current_char == '-' && peek(lexer, 1) == '-') {
        while (lexer->current_char != '\0' && lexer->current_char != '\n') {
            advance(lexer);
        }
    }
}

static Token* create_token(TokenType type, const char *value, int line, int column) {
    Token *token = (Token*)malloc(sizeof(Token));
    token->type = type;
    token->value = strdup(value);
    token->line = line;
    token->column = column;
    return token;
}

static Token* read_number(Lexer *lexer) {
    int start_col = lexer->column;
    char buffer[256];
    int i = 0;

    while (lexer->current_char != '\0' && isdigit(lexer->current_char)) {
        buffer[i++] = lexer->current_char;
        advance(lexer);
    }
    buffer[i] = '\0';

    return create_token(TOKEN_INTEGER, buffer, lexer->line, start_col);
}

static Token* read_identifier(Lexer *lexer) {
    int start_col = lexer->column;
    char buffer[256];
    int i = 0;

    while (lexer->current_char != '\0' &&
           (isalnum(lexer->current_char) || lexer->current_char == '_')) {
        buffer[i++] = lexer->current_char;
        advance(lexer);
    }
    buffer[i] = '\0';

    // Converte para maiúsculas, evitando erros por maiúsculas/minúsculas
    char upper_buffer[256];
    for (int j = 0; buffer[j]; j++) {
        upper_buffer[j] = toupper(buffer[j]);
    }
    upper_buffer[i] = '\0';

    // Checa palavras-chave
    if (strcmp(upper_buffer, "PROCEDURE") == 0) return create_token(TOKEN_PROCEDURE, buffer, lexer->line, start_col);
    if (strcmp(upper_buffer, "IS") == 0) return create_token(TOKEN_IS, buffer, lexer->line, start_col);
    if (strcmp(upper_buffer, "BEGIN") == 0) return create_token(TOKEN_BEGIN, buffer, lexer->line, start_col);
    if (strcmp(upper_buffer, "END") == 0) return create_token(TOKEN_END, buffer, lexer->line, start_col);
    if (strcmp(upper_buffer, "IF") == 0) return create_token(TOKEN_IF, buffer, lexer->line, start_col);
    if (strcmp(upper_buffer, "THEN") == 0) return create_token(TOKEN_THEN, buffer, lexer->line, start_col);
    if (strcmp(upper_buffer, "ELSE") == 0) return create_token(TOKEN_ELSE, buffer, lexer->line, start_col);
    if (strcmp(upper_buffer, "WHILE") == 0) return create_token(TOKEN_WHILE, buffer, lexer->line, start_col);
    if (strcmp(upper_buffer, "LOOP") == 0) return create_token(TOKEN_LOOP, buffer, lexer->line, start_col);
    if (strcmp(upper_buffer, "PUT_LINE") == 0) return create_token(TOKEN_PUT_LINE, buffer, lexer->line, start_col);
    if (strcmp(upper_buffer, "GET_LINE") == 0) return create_token(TOKEN_GET_LINE, buffer, lexer->line, start_col);
    if (strcmp(upper_buffer, "AND") == 0) return create_token(TOKEN_AND, buffer, lexer->line, start_col);
    if (strcmp(upper_buffer, "OR") == 0) return create_token(TOKEN_OR, buffer, lexer->line, start_col);
    if (strcmp(upper_buffer, "NOT") == 0) return create_token(TOKEN_NOT, buffer, lexer->line, start_col);

    return create_token(TOKEN_IDENTIFIER, buffer, lexer->line, start_col);
}

static Token* read_string(Lexer *lexer) {
    int start_col = lexer->column;
    char buffer[1024];
    int i = 0;

    advance(lexer); // pula abertura "

    while (lexer->current_char != '\0' && lexer->current_char != '"') {
        buffer[i++] = lexer->current_char;
        advance(lexer);
    }

    if (lexer->current_char == '"') {
        advance(lexer); // pula fechamento "
    }

    buffer[i] = '\0';
    return create_token(TOKEN_STRING, buffer, lexer->line, start_col);
}

Token* lexer_next_token(Lexer *lexer) {
    while (lexer->current_char != '\0') {
        if (isspace(lexer->current_char)) {
            skip_whitespace(lexer);
            continue;
        }

        if (lexer->current_char == '-' && peek(lexer, 1) == '-') {
            skip_comment(lexer);
            continue;
        }

        if (isdigit(lexer->current_char)) {
            return read_number(lexer);
        }

        if (isalpha(lexer->current_char) || lexer->current_char == '_') {
            return read_identifier(lexer);
        }

        if (lexer->current_char == '"') {
            return read_string(lexer);
        }

        int col = lexer->column;

        // Caracteres >= dois
        if (lexer->current_char == ':' && peek(lexer, 1) == '=') {
            advance(lexer);
            advance(lexer);
            return create_token(TOKEN_ASSIGN, ":=", lexer->line, col);
        }

        if (lexer->current_char == '/' && peek(lexer, 1) == '=') {
            advance(lexer);
            advance(lexer);
            return create_token(TOKEN_NOT_EQUAL, "/=", lexer->line, col);
        }

        if (lexer->current_char == '<' && peek(lexer, 1) == '=') {
            advance(lexer);
            advance(lexer);
            return create_token(TOKEN_LESS_EQUAL, "<=", lexer->line, col);
        }

        if (lexer->current_char == '>' && peek(lexer, 1) == '=') {
            advance(lexer);
            advance(lexer);
            return create_token(TOKEN_GREATER_EQUAL, ">=", lexer->line, col);
        }

        // Caracteres únicos
        char current = lexer->current_char;
        advance(lexer);

        switch (current) {
            case '+': return create_token(TOKEN_PLUS, "+", lexer->line, col);
            case '-': return create_token(TOKEN_MINUS, "-", lexer->line, col);
            case '*': return create_token(TOKEN_MULTIPLY, "*", lexer->line, col);
            case '/': return create_token(TOKEN_DIVIDE, "/", lexer->line, col);
            case '=': return create_token(TOKEN_EQUAL, "=", lexer->line, col);
            case '<': return create_token(TOKEN_LESS, "<", lexer->line, col);
            case '>': return create_token(TOKEN_GREATER, ">", lexer->line, col);
            case '(': return create_token(TOKEN_LPAREN, "(", lexer->line, col);
            case ')': return create_token(TOKEN_RPAREN, ")", lexer->line, col);
            case ';': return create_token(TOKEN_SEMICOLON, ";", lexer->line, col);
            case ':': return create_token(TOKEN_COLON, ":", lexer->line, col);
            case ',': return create_token(TOKEN_COMMA, ",", lexer->line, col);
            default:
                return create_token(TOKEN_ERROR, &current, lexer->line, col);
        }
    }

    return create_token(TOKEN_EOF, "", lexer->line, lexer->column);
}

const char* token_type_to_string(TokenType type) {
    switch (type) {
        case TOKEN_PROCEDURE: return "PROCEDURE";
        case TOKEN_IS: return "IS";
        case TOKEN_BEGIN: return "BEGIN";
        case TOKEN_END: return "END";
        case TOKEN_IF: return "IF";
        case TOKEN_THEN: return "THEN";
        case TOKEN_ELSE: return "ELSE";
        case TOKEN_WHILE: return "WHILE";
        case TOKEN_LOOP: return "LOOP";
        case TOKEN_PUT_LINE: return "PUT_LINE";
        case TOKEN_GET_LINE: return "GET_LINE";
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_INTEGER: return "INTEGER";
        case TOKEN_STRING: return "STRING";
        case TOKEN_ASSIGN: return "ASSIGN";
        case TOKEN_PLUS: return "PLUS";
        case TOKEN_MINUS: return "MINUS";
        case TOKEN_MULTIPLY: return "MULTIPLY";
        case TOKEN_DIVIDE: return "DIVIDE";
        case TOKEN_EQUAL: return "EQUAL";
        case TOKEN_NOT_EQUAL: return "NOT_EQUAL";
        case TOKEN_LESS: return "LESS";
        case TOKEN_LESS_EQUAL: return "LESS_EQUAL";
        case TOKEN_GREATER: return "GREATER";
        case TOKEN_GREATER_EQUAL: return "GREATER_EQUAL";
        case TOKEN_AND: return "AND";
        case TOKEN_OR: return "OR";
        case TOKEN_NOT: return "NOT";
        case TOKEN_LPAREN: return "LPAREN";
        case TOKEN_RPAREN: return "RPAREN";
        case TOKEN_SEMICOLON: return "SEMICOLON";
        case TOKEN_COLON: return "COLON";
        case TOKEN_COMMA: return "COMMA";
        case TOKEN_EOF: return "EOF";
        case TOKEN_ERROR: return "ERROR";
        default: return "UNKNOWN";
    }
}