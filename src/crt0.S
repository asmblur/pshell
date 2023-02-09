#define	ARG_SIZ     256 + 16*4 + 1*4

.set noat
.set noreorder
.global ENTRYPOINT
.global _start
.ent	_start
.text

                        nop
                        nop
ENTRYPOINT:
_start:
                        la      $at, __sp
                        sw      $sp, 0x00($at)

                        la      $at, _return
                        sw      $ra, 0x00($at)
// clear .bss

zerobss:
                        la      $v0, _fbss
                        la      $v1, _end
zerobss_loop:
                        nop
                        nop
                        nop
                        sq      $zero, 0x0000($v0)
                        sltu	$at, $v0, $v1
                        bne	$at, $zero, zerobss_loop
                        addiu	$v0, $v0, 16

// initialize main thread
                        li      $a0, 0
                        jal 	main
                        li      $a1, 0

                        la      $at, _return
                        lw      $ra, 0x00($at)

                        la      $at, __sp
                        jr      $ra
                        lw      $sp, 0x00($at)

.end _start

/**************************************/
.align	3
.global _exit
.ent	_exit
_exit:
                        j       Exit
                        move    $a0, $zero
.end    _exit

.align  3
.ent    _root
_root:
                        addiu   $v1, $zero, 35                            # Exit()
                        syscall
.end	_root

.local  _return
_return:                .word   0

.local  __sp
__sp:                   .word   0

.bss
.align  6
_args:                  .space  ARG_SIZ
