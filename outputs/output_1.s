.data
print_fmt_ld: .string "%ld \n"
print_fmt_f: .string "%f \n"
print_fmt_s: .string "%s \n"
nl_fmt: .string "\n"
x: .zero 8
inferred: .zero 8
global: .zero 8
B: .zero 8
y: .zero 8
A: .zero 8
Pi: .zero 8

.text

__init_globals:
  pushq %rbp
  movq %rsp, %rbp
  movq $3, %rax
  cvtsi2sdq %rax, %xmm0
  movsd %xmm0, Pi(%rip)
  movq $1, %rax
  movq %rax, A(%rip)
  movq $2, %rax
  movq %rax, B(%rip)
  movq $5, %rax
  movq %rax, inferred(%rip)
  movq $1, %rax
  movq %rax, x(%rip)
  movq $2, %rax
  movq %rax, y(%rip)
  leave
  ret

.globl main
main:
  pushq %rbp
  movq %rsp, %rbp
  subq $576, %rsp
  call __init_globals
  leaq global(%rip), %rax
  movq %rax, -8(%rbp)
  movq $0, %rax
  movq %rax, -16(%rbp)
  movq $0, %rax
  movq %rax, -16(%rbp)
for_0:
  movq -16(%rbp), %rax
  pushq %rax
  movq $10, %rax
  movq %rax, %rcx
  popq %rax
  cmpq %rcx, %rax
  movq $0, %rax
  setl %al
  movzbq %al, %rax
  cmpq $0, %rax
  je endfor_0
  movq -16(%rbp), %rax
  pushq %rax
  movq $5, %rax
  movq %rax, %rcx
  popq %rax
  cmpq %rcx, %rax
  movq $0, %rax
  sete %al
  movzbq %al, %rax
  cmpq $0, %rax
  je else_1
  jmp endfor_0
  jmp endif_1
else_1:
  jmp forpost_0
endif_1:
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
  movq $0, %rax
  movq %rax, -16(%rbp)
  movq $1, %rax
  movq %rax, y(%rip)
for_2:
  movq -16(%rbp), %rax
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
  movq y(%rip), %rax
  pushq %rax
  movq $1, %rax
  movq %rax, %rcx
  popq %rax
  addq %rcx, %rax
  movq %rax, y(%rip)
forpost_2:
  movq -16(%rbp), %rax
  pushq %rax
  movq $1, %rax
  movq %rax, %rcx
  popq %rax
  addq %rcx, %rax
  movq %rax, -16(%rbp)
  jmp for_2
endfor_2:
for_3:
  movq x(%rip), %rax
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
  movq x(%rip), %rax
  pushq %rax
  movq $1, %rax
  movq %rax, %rcx
  popq %rax
  addq %rcx, %rax
  movq %rax, x(%rip)
  jmp for_3
endfor_3:
for_4:
  jmp endfor_4
  jmp for_4
endfor_4:
  movq x(%rip), %rax
  movq %rax, %r10
  movq $1, %rax
  cmpq %rax, %r10
  je case_5_0
  jmp default_5
case_5_0:
  movq $1, %rax
  movq %rax, y(%rip)
  jmp endswitch_5
default_5:
  movq $2, %rax
  movq %rax, y(%rip)
  jmp endswitch_5
endswitch_5:
  movq $3, %rdi
  movq $8, %rsi
  call calloc@PLT
  movq %rax, -24(%rbp)
  movq -8(%rbp), %rax
  movq 0(%rax), %rax
  movq %rax, -32(%rbp)
  leaq x(%rip), %rax
  movq %rax, -40(%rbp)
  movq -24(%rbp), %rax
  pushq %rax
  movq $0, %rax
  movq %rax, %rdi
  popq %rax
  leaq (%rax, %rdi, 8), %rax
  movq (%rax), %rax
  movq %rax, -48(%rbp)
  movq $2, %rdi
  movq $8, %rsi
  call calloc@PLT
  movq %rax, -56(%rbp)
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
  movq %rax, -56(%rbp)
  movq $0, %rax
  jmp .end_main
  movq $0, %rax
.end_main:
  leave
  ret

.globl add
add:
  pushq %rbp
  movq %rsp, %rbp
  subq $528, %rsp
  movq %rdi, -8(%rbp)
  movq %rsi, -16(%rbp)
  movq -8(%rbp), %rax
  pushq %rax
  movq -16(%rbp), %rax
  movq %rax, %rcx
  popq %rax
  addq %rcx, %rax
  jmp .end_add
  movq $0, %rax
.end_add:
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
  movq $0, %rax
.end_Point_Move:
  leave
  ret

.section .note.GNU-stack,"",@progbits
