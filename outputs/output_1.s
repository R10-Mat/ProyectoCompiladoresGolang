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

.globl fibonacci
fibonacci:
  pushq %rbp
  movq %rsp, %rbp
  subq $528, %rsp
  movq %rdi, -8(%rbp)
  movq -8(%rbp), %rax
  pushq %rax
  movq $1, %rax
  movq %rax, %rcx
  popq %rax
  cmpq %rcx, %rax
  movq $0, %rax
  setle %al
  movzbq %al, %rax
  cmpq $0, %rax
  je else_0
  movq -8(%rbp), %rax
  jmp .end_fibonacci
  jmp endif_0
else_0:
endif_0:
  movq -8(%rbp), %rax
  pushq %rax
  movq $1, %rax
  movq %rax, %rcx
  popq %rax
  subq %rcx, %rax
  pushq %rax
  popq %rdi
  call fibonacci
  pushq %rax
  movq -8(%rbp), %rax
  pushq %rax
  movq $2, %rax
  movq %rax, %rcx
  popq %rax
  subq %rcx, %rax
  pushq %rax
  popq %rdi
  call fibonacci
  movq %rax, %rcx
  popq %rax
  addq %rcx, %rax
  jmp .end_fibonacci
  movq $0, %rax
.end_fibonacci:
  leave
  ret

.globl main
main:
  pushq %rbp
  movq %rsp, %rbp
  subq $512, %rsp
  call __init_globals
  movq $35, %rax
  pushq %rax
  popq %rdi
  call fibonacci
  movq $0, %rax
.end_main:
  leave
  ret

.section .note.GNU-stack,"",@progbits
