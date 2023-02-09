.text

.set push
.set noreorder

.global ee_flush_caches
.ent ee_flush_caches
ee_flush_caches:
                        li          $t0, 0
flushI_Cache:
                        sync.l
                        cache       0x14, 0($t0)
                        sync.l
                        cache       0x14, 1($t0)
                        sync.l

                        addiu       $t0, $t0, 0x40
                        sltiu       $v1, $t0, 0x1000
                        bnez        $v1, flushI_Cache
                        nop

                        li          $t0, 0
flushD_Cache:
                        nop
                        cache       0x07, 0($t0)
                        cache       0x07, 1($t0)
                        nop

                        addiu       $t0, $t0, 0x40
                        sltiu       $v1, $t0, 0x2000
                        bnez        $v1, flushD_Cache
                        nop

                        cache       0x0C, 0($zero)
                        nop

                        jr          $ra
                        nop
.end ee_flush_caches

.global u64_to_u128
.ent u64_to_u128
u64_to_u128:
                        jr          $ra
                        daddu       $v0, $a0, $zero
.end u64_to_u128


.global get_u32
.ent get_u32
get_u32:
                        beqz        $a1, _get_u32_done
                        move        $v0, $a0

                        li          $v1, 1
                        beq         $a1, $v1, _get_u32_done
                        dsrl32      $v0, $a0, 0

                        pcpyud      $a0, $a0, $zero

                        li          $v1, 2
                        beq         $a1, $v1, _get_u32_done
                        move        $v0, $a0

                        dsrl32      $v0, $a0, 0

_get_u32_done:
                        jr          $ra
                        nop
.end get_u32


.global poke64
.ent poke64
poke64:
                        sw          $a1, 0x00($a0)
                        dsrl32      $a1, $a1, 0
                        jr          $ra
                        sw          $a1, 0x04($a0)
.end poke64

.global poke128
.ent poke128
poke128:
                        jr          $ra
                        sq          $a1, 0($a0)
.end poke128

.global strtou128
.ent strtou128
strtou128:

                        addiu   $sp, $sp, -0x30
                        sd      $ra, 0x00($sp)
                        sd      $s0, 0x08($sp)
                        sd      $s1, 0x10($sp)
                        sd      $s2, 0x18($sp)
                        sd      $s3, 0x20($sp)

                        li      $s3, 10                                 ; base = 10;
                        dsll32  $s3, $s3, 0
                        dsrl32  $s3, $s3, 0

                        move    $s0, $a0

                        por     $s1, $zero, $zero                       ; rv = 0;

                        lbu     $t0, 0($s0)
                        li      $t1, 0x30 # '0'
                        bne     $t0, $t1, _not_hex
                        li      $t1, 0x78 # 'x'

                        lbu     $t0, 1($s0)
                        bne     $t0, $t1, _not_hex
                        nop

                        li      $s3, 16                                 ; base = 16;
                        dsll32  $s3, $s3, 0
                        dsrl32  $s3, $s3, 0

                        addiu   $s0, $s0, 2                             // str += 2; // skip over the "0x"
_not_hex:

_l1:
                        lbu     $s2, 0($s0)
                        beqz    $s2, _done
                        addiu   $s0, $s0, 1

                        por     $a0, $zero, $s1
                        jal     mult128
                        por     $a1, $zero, $s3

                        por     $s1, $zero, $v0

                        sltiu   $v0, $s2, 0x30 # '0'
                        bnez    $v0, _bad_char

                        sltiu   $v0, $s2, 0x3A # ':'
                        beqz    $v0, _not_deci
                        sltiu   $v0, $s2, 0x41 # 'A'

                        bgez    $0, _cont
                        addiu   $v0, $s2, -0x30 # '0'

_not_deci:
                        bnez    $v0, _bad_char
                        sltiu   $v0, $s2, 0x47 # 'G'

                        beqz    $v0, _not_uc_hex
                        sltiu   $v0, $s2, 0x61 # 'a'

                        addiu   $v0, $s2, -0x41 # 'A'
                        bgez    $0, _cont
                        addiu   $v0, $v0, 10

_not_uc_hex:
                        bnez    $v0, _bad_char
                        sltiu   $v0, $s2, 0x67 # 'g'

                        beqz    $v0, _bad_char
                        nop

                        addiu   $v0, $s2, -0x61 # 'a'
                        addiu   $v0, $v0, 10

_cont:
                        por     $a0, $zero, $s1
                        dsll32  $a1, $v0, 0
                        jal     add128
                        dsrl32  $a1, $a1, 0

                        bgez    $0, _l1
                        por     $s1, $zero, $v0
_bad_char:
                        li      $s1, -1

_done:
                        por     $v0, $zero, $s1

                        ld      $ra, 0x00($sp)
                        ld      $s0, 0x08($sp)
                        ld      $s1, 0x10($sp)
                        ld      $s2, 0x18($sp)
                        ld      $s3, 0x20($sp)
                        jr      $ra
                        addiu   $sp, $sp, 0x30
.end strtou128

#define TERM_STACK_SIZE (8 * 1024)

.global term_open
.ent term_open
term_open:
                        la      $t0, _term_is_open
                        lw      $t1, 0($t0)
                        bnez    $t1, _term_open_done
                        li      $t1, 1

                        sw      $t1, 0($t0)

                        la      $t0, (_term_stack + TERM_STACK_SIZE - 0x10)
                        sd      $sp, 0x00($t0)
                        sd      $ra, 0x08($t0)

                        jal     terminal
                        move    $sp, $t0

                        ld      $ra, 0x08($sp)
                        ld      $sp, 0x00($sp)

                        la      $t0, _term_is_open
                        sw      $zero, 0($t0)

_term_open_done:
                        jr      $ra
                        nop
.end term_open

.set pop
