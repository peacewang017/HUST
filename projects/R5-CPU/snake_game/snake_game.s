	.file	"snake_game.c"
	.option nopic
	.attribute arch, "rv32i2p0"
	.attribute unaligned_access, 0
	.attribute stack_align, 16
	.text
	.globl	__modsi3
	.align	2
	.globl	main
	.type	main, @function
main:
	addi	sp,sp,-368
	sw	ra,364(sp)
	sw	s0,360(sp)
	sw	s1,356(sp)
	addi	s0,sp,368
	sw	zero,-368(s0)
	sw	zero,-364(s0)
	sw	zero,-360(s0)
	li	a5,-1
	sw	a5,-356(s0)
	li	a5,-1
	sw	a5,-352(s0)
	sw	zero,-348(s0)
	sw	zero,-344(s0)
	li	a5,1
	sw	a5,-340(s0)
	li	a5,1
	sw	a5,-336(s0)
	sw	zero,-332(s0)
.L58:
	li	a5,1
	sw	a5,-48(s0)
	j	.L2
.L3:
	lw	a5,-48(s0)
	slli	a5,a5,3
	addi	a5,a5,-16
	add	a5,a5,s0
	sw	zero,-312(a5)
	lw	a5,-48(s0)
	slli	a5,a5,3
	addi	a5,a5,-16
	add	a5,a5,s0
	sw	zero,-308(a5)
	lw	a5,-48(s0)
	addi	a5,a5,1
	sw	a5,-48(s0)
.L2:
	lw	a4,-48(s0)
	li	a5,30
	ble	a4,a5,.L3
	li	a5,5
	sw	a5,-32(s0)
	li	a5,3
	sw	a5,-36(s0)
	sw	zero,-40(s0)
	sw	zero,-44(s0)
	li	a5,0
	sw	zero,0(a5)
	li	a5,1
	sw	a5,-20(s0)
	li	a5,1
	sw	a5,-24(s0)
	li	a5,1
	sw	a5,-28(s0)
	lw	a5,-20(s0)
	slli	a5,a5,3
	addi	a5,a5,-16
	add	a5,a5,s0
	li	a4,6
	sw	a4,-312(a5)
	lw	a5,-20(s0)
	slli	a5,a5,3
	addi	a5,a5,-16
	add	a5,a5,s0
	li	a4,8
	sw	a4,-308(a5)
	li	a5,1
	sw	a5,-52(s0)
	j	.L4
.L5:
	lw	a5,-52(s0)
	slli	a5,a5,2
	addi	a5,a5,8
	sw	zero,0(a5)
	lw	a5,-52(s0)
	addi	a5,a5,1
	sw	a5,-52(s0)
.L4:
	lw	a4,-52(s0)
	li	a5,192
	ble	a4,a5,.L5
	lw	a5,-32(s0)
	addi	a5,a5,-1
	slli	a4,a5,4
	lw	a5,-36(s0)
	add	a5,a4,a5
	slli	a5,a5,2
	addi	a5,a5,8
	li	a4,2
	sw	a4,0(a5)
	lw	a5,-20(s0)
	slli	a5,a5,3
	addi	a5,a5,-16
	add	a5,a5,s0
	lw	a5,-312(a5)
	addi	a5,a5,-1
	slli	a4,a5,4
	lw	a5,-20(s0)
	slli	a5,a5,3
	addi	a5,a5,-16
	add	a5,a5,s0
	lw	a5,-308(a5)
	add	a5,a4,a5
	slli	a5,a5,2
	addi	a5,a5,8
	li	a4,1
	sw	a4,0(a5)
	li	a5,4
	sw	zero,0(a5)
.L57:
	li	a5,4
	lw	a5,0(a5)
	beq	a5,zero,.L57
	lw	a5,-20(s0)
	slli	a5,a5,3
	addi	a5,a5,-16
	add	a5,a5,s0
	lw	a4,-312(a5)
	li	a5,0
	lw	a5,0(a5)
	slli	a5,a5,3
	addi	a5,a5,-16
	add	a5,a5,s0
	lw	a5,-352(a5)
	add	a5,a4,a5
	sw	a5,-68(s0)
	lw	a5,-20(s0)
	slli	a5,a5,3
	addi	a5,a5,-16
	add	a5,a5,s0
	lw	a4,-308(a5)
	li	a5,0
	lw	a5,0(a5)
	slli	a5,a5,3
	addi	a5,a5,-16
	add	a5,a5,s0
	lw	a5,-348(a5)
	add	a5,a4,a5
	sw	a5,-72(s0)
	lw	a5,-20(s0)
	addi	a5,a5,29
	li	a1,30
	mv	a0,a5
	call	__modsi3
	mv	a5,a0
	beq	a5,zero,.L7
	lw	a5,-20(s0)
	addi	a5,a5,29
	li	a1,30
	mv	a0,a5
	call	__modsi3
	mv	a5,a0
	j	.L8
.L7:
	li	a5,30
.L8:
	sw	a5,-20(s0)
	lw	a5,-20(s0)
	slli	a5,a5,3
	addi	a5,a5,-16
	add	a5,a5,s0
	lw	a4,-68(s0)
	sw	a4,-312(a5)
	lw	a5,-20(s0)
	slli	a5,a5,3
	addi	a5,a5,-16
	add	a5,a5,s0
	lw	a4,-72(s0)
	sw	a4,-308(a5)
	lw	a5,-24(s0)
	slli	a5,a5,3
	addi	a5,a5,-16
	add	a5,a5,s0
	lw	a5,-312(a5)
	addi	a5,a5,-1
	slli	a4,a5,4
	lw	a5,-24(s0)
	slli	a5,a5,3
	addi	a5,a5,-16
	add	a5,a5,s0
	lw	a5,-308(a5)
	add	a5,a4,a5
	slli	a5,a5,2
	addi	a5,a5,8
	sw	zero,0(a5)
	lw	a5,-24(s0)
	slli	a5,a5,3
	addi	a5,a5,-16
	add	a5,a5,s0
	sw	zero,-312(a5)
	lw	a5,-24(s0)
	slli	a5,a5,3
	addi	a5,a5,-16
	add	a5,a5,s0
	sw	zero,-308(a5)
	lw	a5,-24(s0)
	addi	a5,a5,29
	li	a1,30
	mv	a0,a5
	call	__modsi3
	mv	a5,a0
	beq	a5,zero,.L9
	lw	a5,-24(s0)
	addi	a5,a5,29
	li	a1,30
	mv	a0,a5
	call	__modsi3
	mv	a5,a0
	j	.L10
.L9:
	li	a5,30
.L10:
	sw	a5,-24(s0)
	lw	a5,-20(s0)
	slli	a5,a5,3
	addi	a5,a5,-16
	add	a5,a5,s0
	lw	a5,-312(a5)
	sw	a5,-76(s0)
	lw	a5,-20(s0)
	slli	a5,a5,3
	addi	a5,a5,-16
	add	a5,a5,s0
	lw	a5,-308(a5)
	sw	a5,-80(s0)
	lw	a4,-28(s0)
	li	a5,4
	ble	a4,a5,.L11
	lw	a5,-20(s0)
	addi	a5,a5,1
	li	a1,30
	mv	a0,a5
	call	__modsi3
	mv	a5,a0
	beq	a5,zero,.L12
	lw	a5,-20(s0)
	addi	a5,a5,1
	li	a1,30
	mv	a0,a5
	call	__modsi3
	mv	a5,a0
	j	.L13
.L12:
	li	a5,30
.L13:
	sw	a5,-56(s0)
	j	.L14
.L20:
	lw	a5,-56(s0)
	slli	a5,a5,3
	addi	a5,a5,-16
	add	a5,a5,s0
	lw	a5,-312(a5)
	lw	a4,-76(s0)
	bne	a4,a5,.L15
	lw	a5,-56(s0)
	slli	a5,a5,3
	addi	a5,a5,-16
	add	a5,a5,s0
	lw	a5,-308(a5)
	lw	a4,-80(s0)
	bne	a4,a5,.L15
	li	a5,1
	sw	a5,-44(s0)
	j	.L11
.L15:
	lw	a5,-56(s0)
	addi	a5,a5,1
	li	a1,30
	mv	a0,a5
	call	__modsi3
	mv	a5,a0
	beq	a5,zero,.L16
	lw	a5,-56(s0)
	addi	a5,a5,1
	li	a1,30
	mv	a0,a5
	call	__modsi3
	mv	a5,a0
	j	.L17
.L16:
	li	a5,30
.L17:
	sw	a5,-56(s0)
.L14:
	lw	a5,-24(s0)
	addi	a5,a5,1
	li	a1,30
	mv	a0,a5
	call	__modsi3
	mv	a5,a0
	beq	a5,zero,.L18
	lw	a5,-24(s0)
	addi	a5,a5,1
	li	a1,30
	mv	a0,a5
	call	__modsi3
	mv	a5,a0
	j	.L19
.L18:
	li	a5,30
.L19:
	lw	a4,-56(s0)
	bne	a5,a4,.L20
.L11:
	lw	a5,-44(s0)
	bne	a5,zero,.L59
	lw	a5,-76(s0)
	ble	a5,zero,.L58
	lw	a4,-76(s0)
	li	a5,12
	bgt	a4,a5,.L58
	lw	a5,-80(s0)
	ble	a5,zero,.L58
	lw	a4,-80(s0)
	li	a5,16
	bgt	a4,a5,.L58
	lw	a4,-76(s0)
	lw	a5,-32(s0)
	bne	a4,a5,.L23
	lw	a4,-80(s0)
	lw	a5,-36(s0)
	bne	a4,a5,.L23
	li	a5,1
	sw	a5,-40(s0)
	lw	a5,-28(s0)
	addi	a5,a5,1
	sw	a5,-28(s0)
	lw	a4,-28(s0)
	li	a5,30
	beq	a4,a5,.L60
	lw	a4,-28(s0)
	li	a5,2
	bne	a4,a5,.L25
	li	a5,0
	lw	a5,0(a5)
	slli	a5,a5,3
	addi	a5,a5,-16
	add	a5,a5,s0
	lw	a5,-352(a5)
	neg	a5,a5
	sw	a5,-60(s0)
	li	a5,0
	lw	a5,0(a5)
	slli	a5,a5,3
	addi	a5,a5,-16
	add	a5,a5,s0
	lw	a5,-348(a5)
	neg	a5,a5
	sw	a5,-64(s0)
	j	.L26
.L25:
	lw	a5,-24(s0)
	slli	a5,a5,3
	addi	a5,a5,-16
	add	a5,a5,s0
	lw	s1,-312(a5)
	lw	a5,-24(s0)
	addi	a5,a5,29
	li	a1,30
	mv	a0,a5
	call	__modsi3
	mv	a5,a0
	beq	a5,zero,.L27
	lw	a5,-24(s0)
	addi	a5,a5,29
	li	a1,30
	mv	a0,a5
	call	__modsi3
	mv	a5,a0
	j	.L28
.L27:
	li	a5,30
.L28:
	slli	a5,a5,3
	addi	a5,a5,-16
	add	a5,a5,s0
	lw	a5,-312(a5)
	sub	a5,s1,a5
	sw	a5,-60(s0)
	lw	a5,-24(s0)
	slli	a5,a5,3
	addi	a5,a5,-16
	add	a5,a5,s0
	lw	s1,-308(a5)
	lw	a5,-24(s0)
	addi	a5,a5,29
	li	a1,30
	mv	a0,a5
	call	__modsi3
	mv	a5,a0
	beq	a5,zero,.L29
	lw	a5,-24(s0)
	addi	a5,a5,29
	li	a1,30
	mv	a0,a5
	call	__modsi3
	mv	a5,a0
	j	.L30
.L29:
	li	a5,30
.L30:
	slli	a5,a5,3
	addi	a5,a5,-16
	add	a5,a5,s0
	lw	a5,-308(a5)
	sub	a5,s1,a5
	sw	a5,-64(s0)
.L26:
	lw	a5,-24(s0)
	addi	a5,a5,1
	li	a1,30
	mv	a0,a5
	call	__modsi3
	mv	a5,a0
	beq	a5,zero,.L31
	lw	a5,-24(s0)
	addi	a5,a5,1
	li	a1,30
	mv	a0,a5
	call	__modsi3
	mv	a5,a0
	j	.L32
.L31:
	li	a5,30
.L32:
	sw	a5,-24(s0)
	lw	a5,-24(s0)
	addi	a5,a5,29
	li	a1,30
	mv	a0,a5
	call	__modsi3
	mv	a5,a0
	beq	a5,zero,.L33
	lw	a5,-24(s0)
	addi	a5,a5,29
	li	a1,30
	mv	a0,a5
	call	__modsi3
	mv	a5,a0
	j	.L34
.L33:
	li	a5,30
.L34:
	slli	a5,a5,3
	addi	a5,a5,-16
	add	a5,a5,s0
	lw	a4,-312(a5)
	lw	a5,-60(s0)
	add	a5,a4,a5
	li	a1,12
	mv	a0,a5
	call	__modsi3
	mv	a5,a0
	beq	a5,zero,.L35
	lw	a5,-24(s0)
	addi	a5,a5,29
	li	a1,30
	mv	a0,a5
	call	__modsi3
	mv	a5,a0
	beq	a5,zero,.L36
	lw	a5,-24(s0)
	addi	a5,a5,29
	li	a1,30
	mv	a0,a5
	call	__modsi3
	mv	a5,a0
	j	.L37
.L36:
	li	a5,30
.L37:
	slli	a5,a5,3
	addi	a5,a5,-16
	add	a5,a5,s0
	lw	a4,-312(a5)
	lw	a5,-60(s0)
	add	a5,a4,a5
	li	a1,12
	mv	a0,a5
	call	__modsi3
	mv	a5,a0
	mv	a4,a5
	j	.L38
.L35:
	li	a4,12
.L38:
	lw	a5,-24(s0)
	slli	a5,a5,3
	addi	a5,a5,-16
	add	a5,a5,s0
	sw	a4,-312(a5)
	lw	a5,-24(s0)
	addi	a5,a5,29
	li	a1,30
	mv	a0,a5
	call	__modsi3
	mv	a5,a0
	beq	a5,zero,.L39
	lw	a5,-24(s0)
	addi	a5,a5,29
	li	a1,30
	mv	a0,a5
	call	__modsi3
	mv	a5,a0
	j	.L40
.L39:
	li	a5,30
.L40:
	slli	a5,a5,3
	addi	a5,a5,-16
	add	a5,a5,s0
	lw	a4,-308(a5)
	lw	a5,-64(s0)
	add	a5,a4,a5
	andi	a5,a5,15
	beq	a5,zero,.L41
	lw	a5,-24(s0)
	addi	a5,a5,29
	li	a1,30
	mv	a0,a5
	call	__modsi3
	mv	a5,a0
	beq	a5,zero,.L42
	lw	a5,-24(s0)
	addi	a5,a5,29
	li	a1,30
	mv	a0,a5
	call	__modsi3
	mv	a5,a0
	j	.L43
.L42:
	li	a5,30
.L43:
	slli	a5,a5,3
	addi	a5,a5,-16
	add	a5,a5,s0
	lw	a4,-308(a5)
	lw	a5,-64(s0)
	add	a4,a4,a5
	srai	a5,a4,31
	srli	a5,a5,28
	add	a4,a4,a5
	andi	a4,a4,15
	sub	a5,a4,a5
	mv	a4,a5
	j	.L44
.L41:
	li	a4,16
.L44:
	lw	a5,-24(s0)
	slli	a5,a5,3
	addi	a5,a5,-16
	add	a5,a5,s0
	sw	a4,-308(a5)
.L23:
	lw	a4,-40(s0)
	li	a5,1
	bne	a4,a5,.L45
	lw	a5,-24(s0)
	slli	a5,a5,3
	addi	a5,a5,-16
	add	a5,a5,s0
	lw	a5,-312(a5)
	addi	a5,a5,-1
	slli	a4,a5,4
	lw	a5,-24(s0)
	slli	a5,a5,3
	addi	a5,a5,-16
	add	a5,a5,s0
	lw	a5,-308(a5)
	add	a5,a4,a5
	slli	a5,a5,2
	addi	a5,a5,8
	li	a4,1
	sw	a4,0(a5)
.L45:
	lw	a5,-20(s0)
	slli	a5,a5,3
	addi	a5,a5,-16
	add	a5,a5,s0
	lw	a5,-312(a5)
	addi	a5,a5,-1
	slli	a4,a5,4
	lw	a5,-20(s0)
	slli	a5,a5,3
	addi	a5,a5,-16
	add	a5,a5,s0
	lw	a5,-308(a5)
	add	a5,a4,a5
	slli	a5,a5,2
	addi	a5,a5,8
	li	a4,1
	sw	a4,0(a5)
	lw	a4,-40(s0)
	li	a5,1
	bne	a4,a5,.L46
	lw	a5,-32(s0)
	addi	a5,a5,7
	li	a1,12
	mv	a0,a5
	call	__modsi3
	mv	a5,a0
	beq	a5,zero,.L47
	lw	a5,-32(s0)
	addi	a5,a5,7
	li	a1,12
	mv	a0,a5
	call	__modsi3
	mv	a5,a0
	j	.L48
.L47:
	li	a5,12
.L48:
	sw	a5,-32(s0)
	lw	a5,-36(s0)
	addi	a5,a5,8
	andi	a5,a5,15
	beq	a5,zero,.L49
	lw	a5,-36(s0)
	addi	a4,a5,8
	srai	a5,a4,31
	srli	a5,a5,28
	add	a4,a4,a5
	andi	a4,a4,15
	sub	a5,a4,a5
	j	.L50
.L49:
	li	a5,16
.L50:
	sw	a5,-36(s0)
	j	.L51
.L56:
	lw	a5,-32(s0)
	addi	a5,a5,7
	li	a1,12
	mv	a0,a5
	call	__modsi3
	mv	a5,a0
	beq	a5,zero,.L52
	lw	a5,-32(s0)
	addi	a5,a5,7
	li	a1,12
	mv	a0,a5
	call	__modsi3
	mv	a5,a0
	j	.L53
.L52:
	li	a5,12
.L53:
	sw	a5,-32(s0)
	lw	a5,-36(s0)
	addi	a5,a5,8
	andi	a5,a5,15
	beq	a5,zero,.L54
	lw	a5,-36(s0)
	addi	a4,a5,8
	srai	a5,a4,31
	srli	a5,a5,28
	add	a4,a4,a5
	andi	a4,a4,15
	sub	a5,a4,a5
	j	.L55
.L54:
	li	a5,16
.L55:
	sw	a5,-36(s0)
.L51:
	lw	a5,-32(s0)
	addi	a5,a5,-1
	slli	a4,a5,4
	lw	a5,-36(s0)
	add	a5,a4,a5
	slli	a5,a5,2
	addi	a5,a5,8
	lw	a4,0(a5)
	li	a5,1
	beq	a4,a5,.L56
	lw	a5,-32(s0)
	addi	a5,a5,-1
	slli	a4,a5,4
	lw	a5,-36(s0)
	add	a5,a4,a5
	slli	a5,a5,2
	addi	a5,a5,8
	li	a4,2
	sw	a4,0(a5)
	sw	zero,-40(s0)
.L46:
	li	a5,4
	sw	zero,0(a5)
	j	.L57
.L59:
	nop
	j	.L58
.L60:
	nop
	j	.L58
	.size	main, .-main
	.ident	"GCC: (GNU) 11.1.0"