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

.globl Entidad_recibirDano
Entidad_recibirDano:
  pushq %rbp
  movq %rsp, %rbp
  subq $528, %rsp
  movq %rdi, -8(%rbp)
  movq %rsi, -16(%rbp)
  movq -8(%rbp), %rax
  movq 16(%rax), %rax
  pushq %rax
  movq -16(%rbp), %rax
  movq %rax, %rcx
  popq %rax
  subq %rcx, %rax
  pushq %rax
  movq -8(%rbp), %rax
  addq $16, %rax
  movq %rax, %r10
  popq %rax
  movq %rax, (%r10)
  movq -8(%rbp), %rax
  movq 16(%rax), %rax
  pushq %rax
  movq $0, %rax
  movq %rax, %rcx
  popq %rax
  cmpq %rcx, %rax
  movq $0, %rax
  setl %al
  movzbq %al, %rax
  cmpq $0, %rax
  je else_0
  movq $0, %rax
  pushq %rax
  movq -8(%rbp), %rax
  addq $16, %rax
  movq %rax, %r10
  popq %rax
  movq %rax, (%r10)
  jmp endif_0
else_0:
endif_0:
  movq $0, %rax
.end_Entidad_recibirDano:
  leave
  ret

.globl Entidad_mover
Entidad_mover:
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
.end_Entidad_mover:
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
  movq $1000, %rax
  pushq %rax
  movq -8(%rbp), %rax
  addq $16, %rax
  movq %rax, %r10
  popq %rax
  movq %rax, (%r10)
  movq -8(%rbp), %rax
  movq %rax, -16(%rbp)
  movq $0, %rax
  movq %rax, -24(%rbp)
for_1:
  movq -24(%rbp), %rax
  pushq %rax
  movq $50000, %rax
  movq %rax, %rcx
  popq %rax
  cmpq %rcx, %rax
  movq $0, %rax
  setl %al
  movzbq %al, %rax
  cmpq $0, %rax
  je endfor_1
  movq -16(%rbp), %rax
  pushq %rax
  movq $2, %rax
  pushq %rax
  movq $3, %rax
  pushq %rax
  popq %rdx
  popq %rsi
  popq %rdi
  call Entidad_mover
  movq -16(%rbp), %rax
  pushq %rax
  movq $1, %rax
  pushq %rax
  popq %rsi
  popq %rdi
  call Entidad_recibirDano
forpost_1:
  movq -24(%rbp), %rax
  addq $1, %rax
  movq %rax, -24(%rbp)
  jmp for_1
endfor_1:
  movq $0, %rax
.end_main:
  leave
  ret

.section .note.GNU-stack,"",@progbits
