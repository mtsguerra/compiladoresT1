# Generated MIPS Assembly
# Ada to MIPS Compiler

.data
str0: .asciiz "=== Arithmetic Tests ==="
str1: .asciiz "10 + 20 ="
str2: .asciiz "20 - 10 ="
str3: .asciiz "10 * 20 ="
str4: .asciiz "20 / 10 ="
str5: .asciiz "=== Relational Tests ==="
str6: .asciiz "15 > 10: true"
str7: .asciiz "15 < 20: true"
str8: .asciiz "=== Logical Tests ==="
str9: .asciiz "10 < 15 < 20: true"
str10: .asciiz "=== Nested Control Structures ==="
str11: .asciiz "Grade: A"
str12: .asciiz "Grade: B"
str13: .asciiz "Grade: C"
str14: .asciiz "Grade: D"
str15: .asciiz "=== Loop Test ==="
str16: .asciiz "Countdown from 5:"
str17: .asciiz "=== All Tests Complete ==="
newline: .asciiz "\n"

.text
.globl main

main:
    # Procedure prologue
    addi $sp, $sp, -4
    sw $fp, 0($sp)
    move $fp, $sp
    addi $sp, $sp, -40

    li $t0, 10
    sw $t0, 0($fp)
    li $t0, 20
    sw $t0, 4($fp)
    lw $t0, 0($fp)
    lw $t1, 4($fp)
    add $t0, $t0, $t1
    sw $t0, 8($fp)
    lw $t0, 4($fp)
    lw $t1, 0($fp)
    sub $t0, $t0, $t1
    sw $t0, 12($fp)
    lw $t0, 0($fp)
    lw $t1, 4($fp)
    mul $t0, $t0, $t1
    sw $t0, 16($fp)
    lw $t0, 4($fp)
    lw $t1, 0($fp)
    div $t0, $t0, $t1
    sw $t0, 20($fp)
    la $t0, str0
    move $a0, $t0
    li $v0, 4
    syscall
    la $a0, newline
    li $v0, 4
    syscall
    la $t0, str1
    move $a0, $t0
    li $v0, 4
    syscall
    la $a0, newline
    li $v0, 4
    syscall
    lw $t0, 8($fp)
    move $a0, $t0
    li $v0, 1
    syscall
    la $a0, newline
    li $v0, 4
    syscall
    la $t0, str2
    move $a0, $t0
    li $v0, 4
    syscall
    la $a0, newline
    li $v0, 4
    syscall
    lw $t0, 12($fp)
    move $a0, $t0
    li $v0, 1
    syscall
    la $a0, newline
    li $v0, 4
    syscall
    la $t0, str3
    move $a0, $t0
    li $v0, 4
    syscall
    la $a0, newline
    li $v0, 4
    syscall
    lw $t0, 16($fp)
    move $a0, $t0
    li $v0, 1
    syscall
    la $a0, newline
    li $v0, 4
    syscall
    la $t0, str4
    move $a0, $t0
    li $v0, 4
    syscall
    la $a0, newline
    li $v0, 4
    syscall
    lw $t0, 20($fp)
    move $a0, $t0
    li $v0, 1
    syscall
    la $a0, newline
    li $v0, 4
    syscall
    la $t0, str5
    move $a0, $t0
    li $v0, 4
    syscall
    la $a0, newline
    li $v0, 4
    syscall
    li $t0, 15
    sw $t0, 24($fp)
    lw $t0, 24($fp)
    li $t1, 10
    sgt $t0, $t0, $t1
    beqz $t0, L0
    la $t0, str6
    move $a0, $t0
    li $v0, 4
    syscall
    la $a0, newline
    li $v0, 4
    syscall
    j L1
L0:
L1:
    lw $t0, 24($fp)
    li $t1, 20
    slt $t0, $t0, $t1
    beqz $t0, L2
    la $t0, str7
    move $a0, $t0
    li $v0, 4
    syscall
    la $a0, newline
    li $v0, 4
    syscall
    j L3
L2:
L3:
    la $t0, str8
    move $a0, $t0
    li $v0, 4
    syscall
    la $a0, newline
    li $v0, 4
    syscall
    lw $t0, 24($fp)
    li $t1, 10
    sgt $t0, $t0, $t1
    lw $t1, 24($fp)
    li $t2, 20
    slt $t1, $t1, $t2
    sw $t0, 28($fp)
    lw $t0, 28($fp)
    li $t1, 1
    seq $t0, $t0, $t1
    beqz $t0, L4
    la $t0, str9
    move $a0, $t0
    li $v0, 4
    syscall
    la $a0, newline
    li $v0, 4
    syscall
    j L5
L4:
L5:
    la $t0, str10
    move $a0, $t0
    li $v0, 4
    syscall
    la $a0, newline
    li $v0, 4
    syscall
    li $t0, 85
    sw $t0, 32($fp)
    lw $t0, 32($fp)
    li $t1, 90
    sge $t0, $t0, $t1
    beqz $t0, L6
    la $t0, str11
    move $a0, $t0
    li $v0, 4
    syscall
    la $a0, newline
    li $v0, 4
    syscall
    j L7
L6:
    lw $t0, 32($fp)
    li $t1, 80
    sge $t0, $t0, $t1
    beqz $t0, L8
    la $t0, str12
    move $a0, $t0
    li $v0, 4
    syscall
    la $a0, newline
    li $v0, 4
    syscall
    j L9
L8:
    lw $t0, 32($fp)
    li $t1, 70
    sge $t0, $t0, $t1
    beqz $t0, L10
    la $t0, str13
    move $a0, $t0
    li $v0, 4
    syscall
    la $a0, newline
    li $v0, 4
    syscall
    j L11
L10:
    la $t0, str14
    move $a0, $t0
    li $v0, 4
    syscall
    la $a0, newline
    li $v0, 4
    syscall
L11:
L9:
L7:
    la $t0, str15
    move $a0, $t0
    li $v0, 4
    syscall
    la $a0, newline
    li $v0, 4
    syscall
    la $t0, str16
    move $a0, $t0
    li $v0, 4
    syscall
    la $a0, newline
    li $v0, 4
    syscall
    li $t0, 5
    sw $t0, 36($fp)
L12:
    lw $t0, 36($fp)
    li $t1, 0
    sgt $t0, $t0, $t1
    beqz $t0, L13
    lw $t0, 36($fp)
    move $a0, $t0
    li $v0, 1
    syscall
    la $a0, newline
    li $v0, 4
    syscall
    lw $t0, 36($fp)
    li $t1, 1
    sub $t0, $t0, $t1
    sw $t0, 36($fp)
    j L12
L13:
    la $t0, str17
    move $a0, $t0
    li $v0, 4
    syscall
    la $a0, newline
    li $v0, 4
    syscall

    # Procedure epilogue
    addi $sp, $sp, 40
    lw $fp, 0($sp)
    addi $sp, $sp, 4
    li $v0, 10
    syscall
