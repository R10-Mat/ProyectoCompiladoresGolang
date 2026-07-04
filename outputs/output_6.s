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

.globl ackermann
ackermann:
  pushq %rbp
  movq %rsp, %rbp
  subq $528, %rsp
  movq %rdi, -8(%rbp)
  movq %rsi, -16(%rbp)
  movq -8(%rbp), %rax
  pushq %rax
  movq $0, %rax
  movq %rax, %rcx
  popq %rax
  cmpq %rcx, %rax
  movq $0, %rax
  sete %al
  movzbq %al, %rax
  cmpq $0, %rax
  je else_0
  movq -16(%rbp), %rax
  pushq %rax
  movq $1, %rax
  movq %rax, %rcx
  popq %rax
  addq %rcx, %rax
  jmp .end_ackermann
  jmp endif_0
else_0:
endif_0:
  movq -8(%rbp), %rax
  pushq %rax
  movq $0, %rax
  movq %rax, %rcx
  popq %rax
  cmpq %rcx, %rax
  movq $0, %rax
  setg %al
  movzbq %al, %rax
  pushq %rax
  movq -16(%rbp), %rax
  pushq %rax
  movq $0, %rax
  movq %rax, %rcx
  popq %rax
  cmpq %rcx, %rax
  movq $0, %rax
  sete %al
  movzbq %al, %rax
  movq %rax, %rcx
  popq %rax
  andq %rcx, %rax
  cmpq $0, %rax
  je else_1
  movq -8(%rbp), %rax
  pushq %rax
  movq $1, %rax
  movq %rax, %rcx
  popq %rax
  subq %rcx, %rax
  pushq %rax
  movq $1, %rax
  pushq %rax
  popq %rsi
  popq %rdi
  call ackermann
  jmp .end_ackermann
  jmp endif_1
else_1:
endif_1:
  movq -8(%rbp), %rax
  pushq %rax
  movq $1, %rax
  movq %rax, %rcx
  popq %rax
  subq %rcx, %rax
  pushq %rax
  movq -8(%rbp), %rax
  pushq %rax
  movq -16(%rbp), %rax
  pushq %rax
  movq $1, %rax
  movq %rax, %rcx
  popq %rax
  subq %rcx, %rax
  pushq %rax
  popq %rsi
  popq %rdi
  call ackermann
  pushq %rax
  popq %rsi
  popq %rdi
  call ackermann
  jmp .end_ackermann
  movq $0, %rax
.end_ackermann:
  leave
  ret

.globl main
main:
  pushq %rbp
  movq %rsp, %rbp
  subq $512, %rsp
  call __init_globals
  movq $3, %rax
  pushq %rax
  movq $4, %rax
  pushq %rax
  popq %rsi
  popq %rdi
  call ackermann
  movq $0, %rax
.end_main:
  leave
  ret

.section .note.GNU-stack,"",@progbits
