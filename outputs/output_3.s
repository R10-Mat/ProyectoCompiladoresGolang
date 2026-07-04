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
  movq $2, %rdi
  movq $8, %rsi
  call calloc@PLT
  movq %rax, -8(%rbp)
  movq $0, %rax
  pushq %rax
  movq -8(%rbp), %rax
  movq %rax, %r10
  popq %rax
  movq %rax, (%r10)
  movq $0, %rax
  pushq %rax
  movq -8(%rbp), %rax
  addq $8, %rax
  movq %rax, %r10
  popq %rax
  movq %rax, (%r10)
  movq -8(%rbp), %rax
  movq %rax, -16(%rbp)
  movq $0, %rax
  movq %rax, -24(%rbp)
for_0:
  movq -24(%rbp), %rax
  pushq %rax
  movq $500000, %rax
  movq %rax, %rcx
  popq %rax
  cmpq %rcx, %rax
  movq $0, %rax
  setl %al
  movzbq %al, %rax
  cmpq $0, %rax
  je endfor_0
  movq -16(%rbp), %rax
  movq 0(%rax), %rax
  pushq %rax
  movq -24(%rbp), %rax
  movq %rax, %rcx
  popq %rax
  addq %rcx, %rax
  pushq %rax
  movq -16(%rbp), %rax
  movq %rax, %r10
  popq %rax
  movq %rax, (%r10)
  movq -16(%rbp), %rax
  movq 8(%rax), %rax
  pushq %rax
  movq -24(%rbp), %rax
  pushq %rax
  movq $2, %rax
  movq %rax, %rcx
  popq %rax
  cqto
  idivq %rcx
  movq %rax, %rcx
  popq %rax
  subq %rcx, %rax
  pushq %rax
  movq -16(%rbp), %rax
  addq $8, %rax
  movq %rax, %r10
  popq %rax
  movq %rax, (%r10)
forpost_0:
  movq -24(%rbp), %rax
  addq $1, %rax
  movq %rax, -24(%rbp)
  jmp for_0
endfor_0:
  movq $0, %rax
.end_main:
  leave
  ret

.section .note.GNU-stack,"",@progbits
