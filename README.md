# Ada to MIPS Compiler

A complete compiler that translates a subset of Ada programming language to MIPS assembly code.

## Features

- **Lexical Analysis**: Tokenizes Ada source code
- **Syntactic Analysis**: Builds Abstract Syntax Tree (AST)
- **Semantic Analysis**: Type checking and symbol table management
- **Code Generation**: Generates valid MIPS assembly code

## Supported Ada Features

### Data Types
- Integer
- String
- Boolean (from relational/logical operations)

### Statements
- Variable assignments
- `Put_Line` (output)
- `Get_Line` (input)
- `if-then-else` statements
- `while` loops

### Expressions
- Arithmetic operators: `+`, `-`, `*`, `/`
- Relational operators: `<`, `>`, `<=`, `>=`, `=`, `/=`
- Logical operators: `and`, `or`, `not`

## Building

```bash
make
```

This will create the compiler binary at `build/ada_compiler`.

## Usage

```bash
./build/ada_compiler <input.ada> [-o output.asm]
```

If `-o` is not specified, output will be written to `output.asm`.

### Example

```bash
./build/ada_compiler examples/test_basic.ada -o basic.asm
```

## Running Generated Code

### Using SPIM

```bash
spim -file output.asm
```

### Using MARS

```bash
java -jar mars.jar output.asm
```

## Example Programs

See the `examples/` directory for sample Ada programs:

- `test_basic.ada` - Basic arithmetic and output
- `test_arithmetic.ada` - All arithmetic operations
- `test_control.ada` - Control structures (if/while)

## Architecture

```
.
├── ada_parser/
│   └── src/
│       ├── lexer.c/h          - Lexical analyzer
│       ├── parser.c/h         - Syntax analyzer
│       ├── ast.c/h            - Abstract Syntax Tree
│       ├── semantic.c/h       - Semantic analyzer
│       ├── symbol_table.c/h   - Symbol table management
│       ├── mips_codegen.c/h   - MIPS code generator
│       ├── register_alloc.c/h - Register allocator
│       └── main.c             - Main compiler driver
├── examples/                   - Example Ada programs
├── Makefile                    - Build configuration
└── README.md                   - This file
```

## Compilation Pipeline

1. **Lexical Analysis**: Source code → Tokens
2. **Syntax Analysis**: Tokens → AST
3. **Semantic Analysis**: AST → Symbol Table + Type Checking
4. **Code Generation**: AST + Symbol Table → MIPS Assembly

## Testing

Run the test suite:

```bash
make test
```

This will compile all example programs and verify the compiler works correctly.

## Clean

Remove build artifacts:

```bash
make clean
```

## MIPS Assembly Details

The generated MIPS code follows these conventions:

- **$t0-$t9**: Temporary registers for expression evaluation
- **$fp**: Frame pointer for local variables
- **$sp**: Stack pointer
- **Syscalls**:
  - `1`: print_int
  - `4`: print_string
  - `5`: read_int
  - `10`: exit

## Example Ada Program

```ada
procedure Example is
begin
    x := 10;
    y := 20;
    sum := x + y;
    Put_Line("Sum is:");
    Put_Line(sum);
end Example;
```

## Generated MIPS Output

```mips
.data
str0: .asciiz "Sum is:"
newline: .asciiz "\n"

.text
.globl main

main:
    # Procedure prologue
    addi $sp, $sp, -4
    sw $fp, 0($sp)
    move $fp, $sp
    addi $sp, $sp, -12

    # x := 10
    li $t0, 10
    sw $t0, 0($fp)
    
    # y := 20
    li $t0, 20
    sw $t0, 4($fp)
    
    # sum := x + y
    lw $t0, 0($fp)
    lw $t1, 4($fp)
    add $t0, $t0, $t1
    sw $t0, 8($fp)
    
    # Put_Line("Sum is:")
    la $t0, str0
    move $a0, $t0
    li $v0, 4
    syscall
    la $a0, newline
    li $v0, 4
    syscall
    
    # Put_Line(sum)
    lw $t0, 8($fp)
    move $a0, $t0
    li $v0, 1
    syscall
    la $a0, newline
    li $v0, 4
    syscall

    # Procedure epilogue
    addi $sp, $sp, 12
    lw $fp, 0($sp)
    addi $sp, $sp, 4
    li $v0, 10
    syscall
```

## License

This project was developed as part of a compilers course at UFRGS.

## References

- Based on the architecture of [Gvalandro/KotlinCompiler](https://github.com/Gvalandro/KotlinCompiler)
- MIPS Assembly Programming Guide
- Compilers: Principles, Techniques, and Tools (Dragon Book)
