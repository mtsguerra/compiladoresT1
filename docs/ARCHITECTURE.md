# Ada to MIPS Compiler - Architecture Documentation

## Overview

This compiler translates a subset of Ada programming language to MIPS assembly code. It follows a traditional four-phase compiler architecture based on the design principles from the [Gvalandro/KotlinCompiler](https://github.com/Gvalandro/KotlinCompiler) project, adapted from Haskell/Kotlin to C/Ada.

## Compilation Pipeline

```
Ada Source Code
      ↓
[Lexical Analysis] → Tokens
      ↓
[Syntax Analysis] → Abstract Syntax Tree (AST)
      ↓
[Semantic Analysis] → Symbol Table + Type Checking
      ↓
[Code Generation] → MIPS Assembly
```

## Module Architecture

### 1. Lexer Module (`lexer.c/h`)

**Purpose**: Converts source code into a stream of tokens.

**Key Components**:
- `Lexer` structure: Maintains source position, line, and column
- `Token` structure: Represents a lexical unit (type + value + location)
- Token types: Keywords, identifiers, literals, operators, delimiters

**Supported Tokens**:
- Keywords: `procedure`, `is`, `begin`, `end`, `if`, `then`, `else`, `while`, `loop`
- I/O: `Put_Line`, `Get_Line`
- Operators: `:=`, `+`, `-`, `*`, `/`, `=`, `/=`, `<`, `>`, `<=`, `>=`
- Logical: `and`, `or`, `not`
- Literals: integers, strings
- Identifiers: variable and procedure names

**Implementation Details**:
- Single-pass lexical analysis
- Handles Ada comments (-- to end of line)
- Preserves location information for error reporting

### 2. Parser Module (`parser.c/h`)

**Purpose**: Builds an Abstract Syntax Tree from tokens using recursive descent parsing.

**Key Components**:
- `Parser` structure: Holds lexer reference and current token
- Parsing functions for each grammar production

**Grammar (Simplified)**:
```
program        → procedure
procedure      → PROCEDURE identifier IS BEGIN statements END identifier ;
statements     → statement*
statement      → assignment | if_stmt | while_stmt | put_line | get_line
assignment     → identifier := expression ;
if_stmt        → IF expression THEN statements (ELSE statements)? END IF ;
while_stmt     → WHILE expression LOOP statements END LOOP ;
expression     → logical_expr
logical_expr   → relational_expr ((AND | OR) relational_expr)*
relational_expr → additive_expr ((< | > | <= | >= | = | /=) additive_expr)?
additive_expr  → multiplicative_expr ((+ | -) multiplicative_expr)*
multiplicative_expr → unary_expr ((* | /) unary_expr)*
unary_expr     → (NOT | -)? primary
primary        → integer | string | identifier | (expression)
```

**Error Handling**:
- Syntax errors reported with line and column information
- Parser attempts to continue after errors where possible

### 3. AST Module (`ast.c/h`)

**Purpose**: Defines the Abstract Syntax Tree data structure and operations.

**Node Types**:
- `AST_PROGRAM`: Root node
- `AST_PROCEDURE`: Procedure definition
- `AST_BLOCK`: Sequence of statements
- `AST_ASSIGNMENT`: Variable assignment
- `AST_IF_STATEMENT`: Conditional statement
- `AST_WHILE_STATEMENT`: Loop statement
- `AST_PUT_LINE`: Output statement
- `AST_GET_LINE`: Input statement
- `AST_BINARY_OP`: Binary operation (arithmetic, relational, logical)
- `AST_UNARY_OP`: Unary operation (negation, NOT)
- `AST_INTEGER`: Integer literal
- `AST_STRING`: String literal
- `AST_IDENTIFIER`: Variable reference

**Design**:
- Tagged union structure for efficient memory usage
- Constructor functions for each node type
- Pretty-printing function for debugging
- Memory management with recursive deallocation

### 4. Symbol Table Module (`symbol_table.c/h`)

**Purpose**: Manages variable and procedure declarations with scope support.

**Key Components**:
- `SymbolTable` structure: Linked list of symbols with parent scope reference
- `Symbol` structure: Name, type, memory offset
- Scope management: Enter/exit scope operations

**Symbol Types**:
- `SYMBOL_INTEGER`: Integer variables
- `SYMBOL_STRING`: String variables
- `SYMBOL_BOOLEAN`: Boolean values (from expressions)
- `SYMBOL_PROCEDURE`: Procedure names
- `SYMBOL_FUNCTION`: Function names (for future extension)

**Features**:
- Nested scope support
- Name resolution with parent scope lookup
- Duplicate declaration detection
- Memory offset calculation for stack allocation

### 5. Semantic Analysis Module (`semantic.c/h`)

**Purpose**: Type checking and semantic validation.

**Key Features**:
- Type inference for expressions
- Type compatibility checking for assignments
- Implicit variable declaration (variables created on first use)
- Condition type validation (must be boolean)

**Type Rules**:
1. Arithmetic operators (+, -, *, /) require integer operands
2. Relational operators (<, >, <=, >=, =, /=) require same-type operands, return boolean
3. Logical operators (and, or) require boolean operands
4. NOT operator requires boolean operand
5. Unary minus requires integer operand

**Error Detection**:
- Use before declaration (if explicit declarations were enabled)
- Type mismatches in operations
- Type mismatches in assignments
- Invalid condition types in control structures

### 6. Register Allocator Module (`register_alloc.c/h`)

**Purpose**: Manages MIPS temporary registers during code generation.

**Design**:
- Pool of 10 temporary registers ($t0-$t9)
- Simple acquire/release mechanism
- Availability tracking

**Limitations**:
- No register spilling (expressions limited to ~10 temporaries)
- No register coalescing or optimization
- Sufficient for typical simple expressions

**Future Improvements**:
- Implement register spilling to memory
- Add register preference hints
- Implement graph coloring allocation

### 7. MIPS Code Generator Module (`mips_codegen.c/h`)

**Purpose**: Generates MIPS assembly code from AST.

**Key Components**:
- `MIPSCodeGen` structure: Output file, symbol table, register allocator
- Code emission functions for each statement/expression type

**Generated Code Structure**:
```mips
.data
    # String literals
    str0: .asciiz "Hello"
    str1: .asciiz "World"
    newline: .asciiz "\n"

.text
.globl main
main:
    # Procedure prologue
    addi $sp, $sp, -4      # Save frame pointer
    sw $fp, 0($sp)
    move $fp, $sp
    addi $sp, $sp, -N      # Allocate N bytes for local variables
    
    # ... generated code ...
    
    # Procedure epilogue
    addi $sp, $sp, N       # Deallocate locals
    lw $fp, 0($sp)         # Restore frame pointer
    addi $sp, $sp, 4
    li $v0, 10             # Exit syscall
    syscall
```

**Code Generation Strategies**:

#### Expressions
- Post-order traversal (evaluate operands first)
- Each expression returns its result register
- Registers released after use

#### Assignments
1. Evaluate right-hand side expression
2. Store result to variable's stack offset
3. Release register

#### Control Structures
- Generate unique labels for branches
- Use `beqz` (branch if zero) for conditionals
- Use `j` (jump) for unconditional branches

#### I/O Operations
- `Put_Line(integer)`: syscall 1 (print_int)
- `Put_Line(string)`: syscall 4 (print_string)
- `Get_Line(identifier)`: syscall 5 (read_int)
- Always print newline after output

### 8. Main Driver (`main.c`)

**Purpose**: Orchestrates the compilation pipeline.

**Workflow**:
1. Read source file
2. Create lexer and tokenize (with debug output)
3. Create parser and build AST (with debug output)
4. Create semantic analyzer and validate
5. Generate MIPS code to output file
6. Report success or errors

**Command Line**:
```bash
ada_compiler input.ada [-o output.asm]
```

## Memory Layout

### Stack Frame Layout
```
High Memory
    +------------------+
    | Saved $fp        | ← $fp (of caller)
    +------------------+
    | Local Variable 0 | ← $fp (current)
    +------------------+
    | Local Variable 1 | ← $fp - 4
    +------------------+
    | Local Variable 2 | ← $fp - 8
    +------------------+
    | ...              |
    +------------------+ ← $sp
Low Memory
```

- Each variable occupies 4 bytes
- Variables accessed via offset from $fp
- Stack grows downward (toward lower addresses)

### Register Usage

**Temporaries** ($t0-$t9):
- Used for expression evaluation
- Not preserved across procedure calls
- Managed by register allocator

**Saved** ($s0-$s7):
- Reserved for future use (local variable caching)
- Must be preserved across calls

**Special**:
- `$fp`: Frame pointer (local variable base)
- `$sp`: Stack pointer (top of stack)
- `$a0-$a3`: Arguments for syscalls
- `$v0-$v1`: Return values / syscall numbers

## MIPS Syscalls Used

| Number | Name        | Arguments      | Returns |
|--------|-------------|----------------|---------|
| 1      | print_int   | $a0 = integer  | -       |
| 4      | print_string| $a0 = address  | -       |
| 5      | read_int    | -              | $v0     |
| 10     | exit        | -              | -       |

## Optimization Opportunities

### Current Implementation
- No optimization pass
- Naive code generation
- No dead code elimination
- No constant folding

### Potential Optimizations
1. **Constant Folding**: Evaluate constant expressions at compile time
2. **Dead Code Elimination**: Remove unreachable code
3. **Register Allocation**: Better register usage, reduce memory traffic
4. **Peephole Optimization**: Improve instruction sequences
5. **Common Subexpression Elimination**: Avoid redundant calculations

## Testing Strategy

### Unit Testing
- Each module can be tested independently
- Lexer: Token stream validation
- Parser: AST structure verification
- Semantic: Type checking validation
- Codegen: Assembly output verification

### Integration Testing
- End-to-end compilation of example programs
- SPIM/MARS execution validation
- Correctness verification via output comparison

### Test Files
Located in `examples/`:
- `test_basic.ada`: Basic operations
- `test_arithmetic.ada`: All arithmetic operators
- `test_control.ada`: Control structures

## Build System

### Makefile Targets
- `make`: Build compiler
- `make test`: Run test suite
- `make clean`: Remove build artifacts

### Build Process
1. Compile each module to object file
2. Link object files to create executable
3. Place binary in `build/` directory

## Error Handling

### Compilation Errors
- **Lexical errors**: Invalid characters, unterminated strings
- **Syntax errors**: Unexpected tokens, malformed statements
- **Semantic errors**: Type mismatches, undefined variables
- **Code generation errors**: Register exhaustion, internal errors

### Error Reporting
- Include file location (line, column)
- Descriptive error messages
- Continue after recoverable errors
- Abort on fatal errors

## Limitations and Future Work

### Current Limitations
1. No explicit type declarations (uses implicit declaration)
2. No functions with return values
3. No arrays or records
4. No for loops or case statements
5. No procedure parameters
6. Limited to integer and string types
7. No separate compilation or modules

### Future Enhancements
1. Add explicit variable declarations with types
2. Implement functions with return statements
3. Add for loops (iteration)
4. Add case/when statements (pattern matching)
5. Support procedure/function parameters
6. Add arrays and composite types
7. Implement separate compilation
8. Add optimizations
9. Improve error recovery
10. Add debugging information generation

## References

1. [Gvalandro/KotlinCompiler](https://github.com/Gvalandro/KotlinCompiler) - Original architecture inspiration
2. MIPS Assembly Language Programming - Robert Britton
3. Compilers: Principles, Techniques, and Tools (Dragon Book) - Aho, Lam, Sethi, Ullman
4. SPIM Simulator Documentation
5. Ada Language Reference Manual (for syntax reference)
