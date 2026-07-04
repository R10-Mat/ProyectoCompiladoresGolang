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

.globl collatz
collatz:
  pushq %rbp
  movq %rsp, %rbp
  subq $528, %rsp
  movq %rdi, -8(%rbp)
  movq $0, %rax
  movq %rax, -16(%rbp)
for_0:
  movq -8(%rbp), %rax
  pushq %rax
  movq $1, %rax
  movq %rax, %rcx
  popq %rax
  cmpq %rcx, %rax
  movq $0, %rax
  setg %al
  movzbq %al, %rax
  cmpq $0, %rax
  je endfor_0
  movq -8(%rbp), %rax
  pushq %rax
  movq $2, %rax
  movq %rax, %rcx
  popq %rax
  cqto
  idivq %rcx
  movq %rdx, %rax
  pushq %rax
  movq $0, %rax
  movq %rax, %rcx
  popq %rax
  cmpq %rcx, %rax
  movq $0, %rax
  sete %al
  movzbq %al, %rax
  cmpq $0, %rax
  je else_1
  movq -8(%rbp), %rax
  pushq %rax
  movq $2, %rax
  movq %rax, %rcx
  popq %rax
  cqto
  idivq %rcx
  movq %rax, -8(%rbp)
  jmp endif_1
else_1:
  movq $3, %rax
  pushq %rax
  movq -8(%rbp), %rax
  movq %rax, %rcx
  popq %rax
  imulq %rcx, %rax
  pushq %rax
  movq $1, %rax
  movq %rax, %rcx
  popq %rax
  addq %rcx, %rax
  movq %rax, -8(%rbp)
endif_1:
  movq -16(%rbp), %rax
  addq $1, %rax
  movq %rax, -16(%rbp)
  jmp for_0
endfor_0:
  movq -16(%rbp), %rax
  jmp .end_collatz
  movq $0, %rax
.end_collatz:
  leave
  ret

.globl main
main:
  pushq %rbp
  movq %rsp, %rbp
  subq $544, %rsp
  call __init_globals
  movq $0, %rax
  movq %rax, -8(%rbp)
  movq $1, %rax
  movq %rax, -24(%rbp)
for_2:
  movq -24(%rbp), %rax
  pushq %rax
  movq $500, %rax
  movq %rax, %rcx
  popq %rax
  cmpq %rcx, %rax
  movq $0, %rax
  setl %al
  movzbq %al, %rax
  cmpq $0, %rax
  je endfor_2
  movq -24(%rbp), %rax
  pushq %rax
  popq %rdi
  call collatz
  movq %rax, -16(%rbp)
  movq -16(%rbp), %rax
  pushq %rax
  movq -8(%rbp), %rax
  movq %rax, %rcx
  popq %rax
  cmpq %rcx, %rax
  movq $0, %rax
  setg %al
  movzbq %al, %rax
  cmpq $0, %rax
  je else_3
  movq -16(%rbp), %rax
  movq %rax, -8(%rbp)
  jmp endif_3
else_3:
endif_3:
forpost_2:
  movq -24(%rbp), %rax
  addq $1, %rax
  movq %rax, -24(%rbp)
  jmp for_2
endfor_2:
  movq $0, %rax
.end_main:
  leave
  ret

.section .note.GNU-stack,"",@progbits
