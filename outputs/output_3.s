.data
print_fmt_ld: .string "%ld \n"
print_fmt_f: .string "%f \n"
print_fmt_s: .string "%s \n"
nl_fmt: .string "\n"

.text

__init_globals:
  pushq %rbp
  movq %rsp, %rbp
  leave
  ret

.globl main
main:
  pushq %rbp
  movq %rsp, %rbp
  subq $544, %rsp
  call __init_globals
.data
str_lit_0: .string "hello"
.text
  leaq str_lit_0(%rip), %rax
  movq %rax, -8(%rbp)
.data
str_lit_1: .string " world"
.text
  leaq str_lit_1(%rip), %rax
  movq %rax, -16(%rbp)
  movq -8(%rbp), %rax
  pushq %rax
  movq -16(%rbp), %rax
  movq %rax, %rsi
  popq %rdi
  call str_concat
  movq %rax, -24(%rbp)
  movq -24(%rbp), %rax
  movq %rax, %rsi
  leaq print_fmt_s(%rip), %rdi
  movq $0, %rax
  call printf@PLT
  leaq nl_fmt(%rip), %rdi
  movq $0, %rax
  call printf@PLT
  movq $0, %rax
.end_main:
  leave
  ret

.globl str_concat
str_concat:
  pushq %rbp
  movq %rsp, %rbp
  pushq %rbx
  pushq %r12
  pushq %r13
  movq %rdi, %r12
  movq %rsi, %r13
  movq %r12, %rdi
  call strlen@PLT
  movq %rax, %rbx
  movq %r13, %rdi
  call strlen@PLT
  addq %rbx, %rax
  addq $1, %rax
  movq %rax, %rdi
  call malloc@PLT
  movq %rax, %rbx
  movq %rbx, %rdi
  movq %r12, %rsi
  call strcpy@PLT
  movq %rbx, %rdi
  movq %r13, %rsi
  call strcat@PLT
  movq %rbx, %rax
  popq %r13
  popq %r12
  popq %rbx
  leave
  ret

.section .note.GNU-stack,"",@progbits
