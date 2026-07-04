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
  movq $500, %rdi
  movq $8, %rsi
  call calloc@PLT
  movq %rax, -8(%rbp)
  movq $0, %rax
  movq %rax, -16(%rbp)
for_0:
  movq -16(%rbp), %rax
  pushq %rax
  movq $500, %rax
  movq %rax, %rcx
  popq %rax
  cmpq %rcx, %rax
  movq $0, %rax
  setl %al
  movzbq %al, %rax
  cmpq $0, %rax
  je endfor_0
  movq $500, %rax
  pushq %rax
  movq -16(%rbp), %rax
  movq %rax, %rcx
  popq %rax
  subq %rcx, %rax
  pushq %rax
  movq -8(%rbp), %rax
  pushq %rax
  movq -16(%rbp), %rax
  movq %rax, %rdi
  popq %rax
  leaq (%rax, %rdi, 8), %rax
  movq %rax, %r10
  popq %rax
  movq %rax, (%r10)
forpost_0:
  movq -16(%rbp), %rax
  addq $1, %rax
  movq %rax, -16(%rbp)
  jmp for_0
endfor_0:
  movq $0, %rax
  movq %rax, -16(%rbp)
for_1:
  movq -16(%rbp), %rax
  pushq %rax
  movq $500, %rax
  movq %rax, %rcx
  popq %rax
  cmpq %rcx, %rax
  movq $0, %rax
  setl %al
  movzbq %al, %rax
  cmpq $0, %rax
  je endfor_1
  movq $0, %rax
  movq %rax, -24(%rbp)
for_2:
  movq -24(%rbp), %rax
  pushq %rax
  movq $499, %rax
  movq %rax, %rcx
  popq %rax
  cmpq %rcx, %rax
  movq $0, %rax
  setl %al
  movzbq %al, %rax
  cmpq $0, %rax
  je endfor_2
  movq -8(%rbp), %rax
  pushq %rax
  movq -24(%rbp), %rax
  movq %rax, %rdi
  popq %rax
  leaq (%rax, %rdi, 8), %rax
  movq (%rax), %rax
  pushq %rax
  movq -8(%rbp), %rax
  pushq %rax
  movq -24(%rbp), %rax
  pushq %rax
  movq $1, %rax
  movq %rax, %rcx
  popq %rax
  addq %rcx, %rax
  movq %rax, %rdi
  popq %rax
  leaq (%rax, %rdi, 8), %rax
  movq (%rax), %rax
  movq %rax, %rcx
  popq %rax
  cmpq %rcx, %rax
  movq $0, %rax
  setg %al
  movzbq %al, %rax
  cmpq $0, %rax
  je else_3
  movq -8(%rbp), %rax
  pushq %rax
  movq -24(%rbp), %rax
  movq %rax, %rdi
  popq %rax
  leaq (%rax, %rdi, 8), %rax
  movq (%rax), %rax
  movq %rax, -32(%rbp)
  movq -8(%rbp), %rax
  pushq %rax
  movq -24(%rbp), %rax
  pushq %rax
  movq $1, %rax
  movq %rax, %rcx
  popq %rax
  addq %rcx, %rax
  movq %rax, %rdi
  popq %rax
  leaq (%rax, %rdi, 8), %rax
  movq (%rax), %rax
  pushq %rax
  movq -8(%rbp), %rax
  pushq %rax
  movq -24(%rbp), %rax
  movq %rax, %rdi
  popq %rax
  leaq (%rax, %rdi, 8), %rax
  movq %rax, %r10
  popq %rax
  movq %rax, (%r10)
  movq -32(%rbp), %rax
  pushq %rax
  movq -8(%rbp), %rax
  pushq %rax
  movq -24(%rbp), %rax
  pushq %rax
  movq $1, %rax
  movq %rax, %rcx
  popq %rax
  addq %rcx, %rax
  movq %rax, %rdi
  popq %rax
  leaq (%rax, %rdi, 8), %rax
  movq %rax, %r10
  popq %rax
  movq %rax, (%r10)
  jmp endif_3
else_3:
endif_3:
forpost_2:
  movq -24(%rbp), %rax
  addq $1, %rax
  movq %rax, -24(%rbp)
  jmp for_2
endfor_2:
forpost_1:
  movq -16(%rbp), %rax
  addq $1, %rax
  movq %rax, -16(%rbp)
  jmp for_1
endfor_1:
  movq $0, %rax
.end_main:
  leave
  ret

.section .note.GNU-stack,"",@progbits
