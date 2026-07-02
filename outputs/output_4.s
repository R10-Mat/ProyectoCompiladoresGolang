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

.globl Point_Move
Point_Move:
  pushq %rbp
  movq %rsp, %rbp
  subq $544, %rsp
  movq %rdi, -8(%rbp)
  movq %rsi, -16(%rbp)
  movq %rdx, -24(%rbp)
  movq -8(%rbp), %rax
  movq 0(%rax), %rax
  pushq %rax
  movq -16(%rbp), %rax
  movq %rax, %rcx
  popq %rax
  addq %rcx, %rax
  pushq %rax
  movq -8(%rbp), %rax
  movq %rax, %r10
  popq %rax
  movq %rax, (%r10)
  movq -8(%rbp), %rax
  movq 8(%rax), %rax
  pushq %rax
  movq -24(%rbp), %rax
  movq %rax, %rcx
  popq %rax
  addq %rcx, %rax
  pushq %rax
  movq -8(%rbp), %rax
  addq $8, %rax
  movq %rax, %r10
  popq %rax
  movq %rax, (%r10)
  movq $0, %rax
.end_Point_Move:
  leave
  ret

.globl main
main:
  pushq %rbp
  movq %rsp, %rbp
  subq $528, %rsp
  call __init_globals
  movq $2, %rdi
  movq $8, %rsi
  call calloc@PLT
  movq %rax, -8(%rbp)
  movq $2, %rdi
  movq $8, %rsi
  call calloc@PLT
  pushq %rax
  movq $1, %rax
  movq (%rsp), %r10
  movq %rax, 0(%r10)
  movq $2, %rax
  movq (%rsp), %r10
  movq %rax, 8(%r10)
  popq %rax
  movq %rax, -8(%rbp)
  movq -8(%rbp), %rax
  movq %rax, -16(%rbp)
  movq -16(%rbp), %rax
  pushq %rax
  movq $10, %rax
  pushq %rax
  movq $20, %rax
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  call Point_Move
  movq -8(%rbp), %rax
  movq 0(%rax), %rax
  movq %rax, %rsi
  leaq print_fmt_ld(%rip), %rdi
  movq $0, %rax
  call printf@PLT
  leaq nl_fmt(%rip), %rdi
  movq $0, %rax
  call printf@PLT
  movq -8(%rbp), %rax
  movq 8(%rax), %rax
  movq %rax, %rsi
  leaq print_fmt_ld(%rip), %rdi
  movq $0, %rax
  call printf@PLT
  leaq nl_fmt(%rip), %rdi
  movq $0, %rax
  call printf@PLT
  movq $0, %rax
.end_main:
  leave
  ret

.section .note.GNU-stack,"",@progbits
