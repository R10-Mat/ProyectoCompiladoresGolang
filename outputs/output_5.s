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
  pushq %rax
  movq $10, %rax
  movq (%rsp), %r10
  movq %rax, 0(%r10)
  movq $20, %rax
  movq (%rsp), %r10
  movq %rax, 8(%r10)
  movq $30, %rax
  movq (%rsp), %r10
  movq %rax, 16(%r10)
  popq %rax
  movq %rax, -8(%rbp)
  movq $0, %rax
  movq %rax, -16(%rbp)
  movq $0, %rax
  movq %rax, -24(%rbp)
  movq $0, %rax
  movq %rax, -16(%rbp)
for_0:
  movq -16(%rbp), %rax
  pushq %rax
  movq $3, %rax
  movq %rax, %rcx
  popq %rax
  cmpq %rcx, %rax
  movq $0, %rax
  setl %al
  movzbq %al, %rax
  cmpq $0, %rax
  je endfor_0
  movq -24(%rbp), %rax
  pushq %rax
  movq -8(%rbp), %rax
  pushq %rax
  movq -16(%rbp), %rax
  movq %rax, %rdi
  popq %rax
  leaq (%rax, %rdi, 8), %rax
  movq (%rax), %rax
  movq %rax, %rcx
  popq %rax
  addq %rcx, %rax
  movq %rax, -24(%rbp)
forpost_0:
  movq -16(%rbp), %rax
  pushq %rax
  movq $1, %rax
  movq %rax, %rcx
  popq %rax
  addq %rcx, %rax
  movq %rax, -16(%rbp)
  jmp for_0
endfor_0:
  movq -24(%rbp), %rax
  movq %rax, %rsi
  leaq print_fmt_ld(%rip), %rdi
  movq $0, %rax
  call printf@PLT
  leaq nl_fmt(%rip), %rdi
  movq $0, %rax
  call printf@PLT
  movq $99, %rax
  pushq %rax
  movq -8(%rbp), %rax
  pushq %rax
  movq $1, %rax
  movq %rax, %rdi
  popq %rax
  leaq (%rax, %rdi, 8), %rax
  movq %rax, %r10
  popq %rax
  movq %rax, (%r10)
  movq -8(%rbp), %rax
  pushq %rax
  movq $1, %rax
  movq %rax, %rdi
  popq %rax
  leaq (%rax, %rdi, 8), %rax
  movq (%rax), %rax
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
