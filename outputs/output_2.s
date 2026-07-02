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
.data
flt_lit_0: .double 1.5
.text
  movsd flt_lit_0(%rip), %xmm0
  movsd %xmm0, -8(%rbp)
.data
flt_lit_1: .double 2.5
.text
  movsd flt_lit_1(%rip), %xmm0
  movsd %xmm0, -16(%rbp)
  movsd -8(%rbp), %xmm0
  subq $8, %rsp
  movsd %xmm0, (%rsp)
  movsd -16(%rbp), %xmm0
  movsd %xmm0, %xmm1
  movsd (%rsp), %xmm0
  addq $8, %rsp
  addsd %xmm1, %xmm0
  movsd %xmm0, -24(%rbp)
  movsd -24(%rbp), %xmm0
  leaq print_fmt_f(%rip), %rdi
  movq $1, %rax
  call printf@PLT
  leaq nl_fmt(%rip), %rdi
  movq $0, %rax
  call printf@PLT
  movsd -24(%rbp), %xmm0
  subq $8, %rsp
  movsd %xmm0, (%rsp)
.data
flt_lit_3: .double 3.0
.text
  movsd flt_lit_3(%rip), %xmm0
  movsd %xmm0, %xmm1
  movsd (%rsp), %xmm0
  addq $8, %rsp
  comisd %xmm1, %xmm0
  movq $0, %rax
  seta %al
  movzbq %al, %rax
  cmpq $0, %rax
  je else_2
  movq $1, %rax
  movq %rax, %rsi
  leaq print_fmt_ld(%rip), %rdi
  movq $0, %rax
  call printf@PLT
  leaq nl_fmt(%rip), %rdi
  movq $0, %rax
  call printf@PLT
  jmp endif_2
else_2:
endif_2:
  movq $0, %rax
.end_main:
  leave
  ret

.section .note.GNU-stack,"",@progbits
