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
  movq $3, %rdi
  movq $8, %rsi
  call calloc@PLT
  movq %rax, -8(%rbp)
  movq $3, %rdi
  movq $8, %rsi
  call calloc@PLT
  movq %rax, -16(%rbp)
  movq $0, %rax
  movq %rax, -32(%rbp)
  movq $0, %rax
  movq %rax, -24(%rbp)
for_0:
  movq -24(%rbp), %rax
  pushq %rax
  movq $20000, %rax
  movq %rax, %rcx
  popq %rax
  cmpq %rcx, %rax
  movq $0, %rax
  setl %al
  movzbq %al, %rax
  cmpq $0, %rax
  je endfor_0
  movq $3, %rdi
  movq $8, %rsi
  call calloc@PLT
  pushq %rax
  movq -24(%rbp), %rax
  movq (%rsp), %r10
  movq %rax, 0(%r10)
  movq -24(%rbp), %rax
  pushq %rax
  movq $1, %rax
  movq %rax, %rcx
  popq %rax
  addq %rcx, %rax
  movq (%rsp), %r10
  movq %rax, 8(%r10)
  movq -24(%rbp), %rax
  pushq %rax
  movq $2, %rax
  movq %rax, %rcx
  popq %rax
  addq %rcx, %rax
  movq (%rsp), %r10
  movq %rax, 16(%r10)
  popq %rax
  movq %rax, -8(%rbp)
  movq $3, %rdi
  movq $8, %rsi
  call calloc@PLT
  pushq %rax
  movq $2, %rax
  movq (%rsp), %r10
  movq %rax, 0(%r10)
  movq $3, %rax
  movq (%rsp), %r10
  movq %rax, 8(%r10)
  movq $4, %rax
  movq (%rsp), %r10
  movq %rax, 16(%r10)
  popq %rax
  movq %rax, -16(%rbp)
  movq -32(%rbp), %rax
  pushq %rax
  movq -8(%rbp), %rax
  movq 0(%rax), %rax
  pushq %rax
  movq -16(%rbp), %rax
  movq 0(%rax), %rax
  movq %rax, %rcx
  popq %rax
  imulq %rcx, %rax
  pushq %rax
  movq -8(%rbp), %rax
  movq 8(%rax), %rax
  pushq %rax
  movq -16(%rbp), %rax
  movq 8(%rax), %rax
  movq %rax, %rcx
  popq %rax
  imulq %rcx, %rax
  movq %rax, %rcx
  popq %rax
  addq %rcx, %rax
  pushq %rax
  movq -8(%rbp), %rax
  movq 16(%rax), %rax
  pushq %rax
  movq -16(%rbp), %rax
  movq 16(%rax), %rax
  movq %rax, %rcx
  popq %rax
  imulq %rcx, %rax
  movq %rax, %rcx
  popq %rax
  addq %rcx, %rax
  movq %rax, %rcx
  popq %rax
  addq %rcx, %rax
  movq %rax, -32(%rbp)
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
