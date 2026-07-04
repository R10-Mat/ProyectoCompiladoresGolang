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
  subq $576, %rsp
  call __init_globals
  movq $100, %rdi
  movq $8, %rsi
  call calloc@PLT
  movq %rax, -8(%rbp)
  movq $100, %rdi
  movq $8, %rsi
  call calloc@PLT
  movq %rax, -16(%rbp)
  movq $100, %rdi
  movq $8, %rsi
  call calloc@PLT
  movq %rax, -24(%rbp)
  movq $0, %rax
  movq %rax, -32(%rbp)
for_0:
  movq -32(%rbp), %rax
  pushq %rax
  movq $100, %rax
  movq %rax, %rcx
  popq %rax
  cmpq %rcx, %rax
  movq $0, %rax
  setl %al
  movzbq %al, %rax
  cmpq $0, %rax
  je endfor_0
  movq -32(%rbp), %rax
  pushq %rax
  movq -8(%rbp), %rax
  pushq %rax
  movq -32(%rbp), %rax
  movq %rax, %rdi
  popq %rax
  leaq (%rax, %rdi, 8), %rax
  movq %rax, %r10
  popq %rax
  movq %rax, (%r10)
  movq $100, %rax
  pushq %rax
  movq -32(%rbp), %rax
  movq %rax, %rcx
  popq %rax
  subq %rcx, %rax
  pushq %rax
  movq -16(%rbp), %rax
  pushq %rax
  movq -32(%rbp), %rax
  movq %rax, %rdi
  popq %rax
  leaq (%rax, %rdi, 8), %rax
  movq %rax, %r10
  popq %rax
  movq %rax, (%r10)
  movq $0, %rax
  pushq %rax
  movq -24(%rbp), %rax
  pushq %rax
  movq -32(%rbp), %rax
  movq %rax, %rdi
  popq %rax
  leaq (%rax, %rdi, 8), %rax
  movq %rax, %r10
  popq %rax
  movq %rax, (%r10)
forpost_0:
  movq -32(%rbp), %rax
  addq $1, %rax
  movq %rax, -32(%rbp)
  jmp for_0
endfor_0:
  movq $0, %rax
  movq %rax, -32(%rbp)
for_1:
  movq -32(%rbp), %rax
  pushq %rax
  movq $10, %rax
  movq %rax, %rcx
  popq %rax
  cmpq %rcx, %rax
  movq $0, %rax
  setl %al
  movzbq %al, %rax
  cmpq $0, %rax
  je endfor_1
  movq $0, %rax
  movq %rax, -40(%rbp)
for_2:
  movq -40(%rbp), %rax
  pushq %rax
  movq $10, %rax
  movq %rax, %rcx
  popq %rax
  cmpq %rcx, %rax
  movq $0, %rax
  setl %al
  movzbq %al, %rax
  cmpq $0, %rax
  je endfor_2
  movq $0, %rax
  movq %rax, -56(%rbp)
  movq $0, %rax
  movq %rax, -48(%rbp)
for_3:
  movq -48(%rbp), %rax
  pushq %rax
  movq $10, %rax
  movq %rax, %rcx
  popq %rax
  cmpq %rcx, %rax
  movq $0, %rax
  setl %al
  movzbq %al, %rax
  cmpq $0, %rax
  je endfor_3
  movq -56(%rbp), %rax
  pushq %rax
  movq -8(%rbp), %rax
  pushq %rax
  movq -32(%rbp), %rax
  pushq %rax
  movq $10, %rax
  movq %rax, %rcx
  popq %rax
  imulq %rcx, %rax
  pushq %rax
  movq -48(%rbp), %rax
  movq %rax, %rcx
  popq %rax
  addq %rcx, %rax
  movq %rax, %rdi
  popq %rax
  leaq (%rax, %rdi, 8), %rax
  movq (%rax), %rax
  pushq %rax
  movq -16(%rbp), %rax
  pushq %rax
  movq -48(%rbp), %rax
  pushq %rax
  movq $10, %rax
  movq %rax, %rcx
  popq %rax
  imulq %rcx, %rax
  pushq %rax
  movq -40(%rbp), %rax
  movq %rax, %rcx
  popq %rax
  addq %rcx, %rax
  movq %rax, %rdi
  popq %rax
  leaq (%rax, %rdi, 8), %rax
  movq (%rax), %rax
  movq %rax, %rcx
  popq %rax
  imulq %rcx, %rax
  movq %rax, %rcx
  popq %rax
  addq %rcx, %rax
  movq %rax, -56(%rbp)
forpost_3:
  movq -48(%rbp), %rax
  addq $1, %rax
  movq %rax, -48(%rbp)
  jmp for_3
endfor_3:
  movq -56(%rbp), %rax
  pushq %rax
  movq -24(%rbp), %rax
  pushq %rax
  movq -32(%rbp), %rax
  pushq %rax
  movq $10, %rax
  movq %rax, %rcx
  popq %rax
  imulq %rcx, %rax
  pushq %rax
  movq -40(%rbp), %rax
  movq %rax, %rcx
  popq %rax
  addq %rcx, %rax
  movq %rax, %rdi
  popq %rax
  leaq (%rax, %rdi, 8), %rax
  movq %rax, %r10
  popq %rax
  movq %rax, (%r10)
forpost_2:
  movq -40(%rbp), %rax
  addq $1, %rax
  movq %rax, -40(%rbp)
  jmp for_2
endfor_2:
forpost_1:
  movq -32(%rbp), %rax
  addq $1, %rax
  movq %rax, -32(%rbp)
  jmp for_1
endfor_1:
  movq $0, %rax
.end_main:
  leave
  ret

.section .note.GNU-stack,"",@progbits
