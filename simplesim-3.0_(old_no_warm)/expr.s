	file	 "expr.i"
data

; cc1 (2.7.2.2) arguments: -O -fdefer-pop -fomit-frame-pointer
; -fcse-follow-jumps -fcse-skip-blocks -fexpensive-optimizations
; -fthread-jumps -fstrength-reduce -funroll-loops -fwritable-strings
; -fpeephole -fforce-mem -ffunction-cse -finline-functions -finline
; -freg-struct-return -fdelayed-branch -frerun-cse-after-loop
; -fschedule-insns -fschedule-insns2 -fcommon -fgnu-linker -m88110 -m88100
; -m88000 -mno-ocs-debug-info -mno-ocs-frame-position -mcheck-zero-division

gcc2_compiled.:
	global	 _do_preexpand_calls
	align	 4
_do_preexpand_calls:
	word	 1
text
	align	 8
	global	 _init_expr_once
_init_expr_once:
	or.u	 r13,r0,hi16(_stack_pointer_rtx)
	ld	 r4,r13,lo16(_stack_pointer_rtx)
	subu	 r31,r31,96
	st	 r1,r31,80
	st.d	 r24,r31,72
	st.d	 r22,r31,64
	st.d	 r20,r31,56
	or	 r2,r0,55
	st.d	 r18,r31,48
	or	 r3,r0,0
	bsr.n	 _gen_rtx
	st.d	 r16,r31,40
@Ltb0:
	or	 r18,r0,r2
	or.u	 r13,r0,hi16(_frame_pointer_rtx)
	or	 r2,r0,55
	ld	 r4,r13,lo16(_frame_pointer_rtx)
	bsr.n	 _gen_rtx
	or	 r3,r0,0
	bsr.n	 _start_sequence
	or	 r20,r0,r2
	or	 r2,r0,40
	or	 r3,r0,0
	bsr.n	 _gen_rtx
	or	 r4,r0,0
	bsr.n	 _emit_insn
	or	 r25,r0,0
	or	 r21,r0,r2
	or	 r16,r0,1
	or.u	 r13,r0,hi16(_direct_load)
	or.u	 r12,r0,hi16(_direct_store)
	ld	 r24,r21,16
	or	 r17,r13,lo16(_direct_load)
	or	 r19,r12,lo16(_direct_store)
	cmp	 r13,r25,0
@L41:
	st.b	 r0,r25,r19
	cmp	 r12,r25,16
	st.b	 r0,r25,r17
	or	 r13,r13,r12
	st.b	 r25,r18,2
	extu	 r12,r13,1<eq>
	xor	 r13,r12,1
	bcnd.n	 eq0,r13,@L25
	st.b	 r25,r20,2
	ld.b	 r13,r25,r17
	br.n	 @L39
	or	 r22,r0,0
	align	 4
@L29:
@L40:
	or	 r13,r13,lo16(_hard_regno_mode_ok)
	ld	 r13,r13[r22]
	ext	 r13,r13,r25
	bb0.n	 (31-31),r13,@L28
	or	 r2,r0,51
	or	 r3,r0,r25
	bsr.n	 _gen_rtx
	or	 r4,r0,r22
	st	 r18,r24,8
	or	 r23,r0,r2
	or	 r2,r0,r24
	or	 r3,r0,r21
	addu	 r4,r31,88
	bsr.n	 _recog
	st	 r23,r24,4
	bcnd	 lt0,r2,@L32
	st.b	 r16,r25,r17
@L32:
	st	 r20,r24,8
	or	 r2,r0,r24
	or	 r3,r0,r21
	addu	 r4,r31,88
	bsr.n	 _recog
	st	 r23,r24,4
	bcnd	 lt0,r2,@L33
	st.b	 r16,r25,r17
@L33:
	st	 r23,r24,8
	or	 r2,r0,r24
	or	 r3,r0,r21
	addu	 r4,r31,88
	bsr.n	 _recog
	st	 r18,r24,4
	bcnd	 lt0,r2,@L34
	st.b	 r16,r25,r19
@L34:
	st	 r23,r24,8
	or	 r2,r0,r24
	or	 r3,r0,r21
	addu	 r4,r31,88
	bsr.n	 _recog
	st	 r20,r24,4
	bcnd	 lt0,r2,@L28
	st.b	 r16,r25,r19
@L28:
	addu	 r22,r22,1
	cmp	 r13,r22,63
	bb0	 le,r13,@L25
	ld.b	 r13,r25,r17
@L39:
	bcnd.n	 eq0,r13,@L40
	or.u	 r13,r0,hi16(_hard_regno_mode_ok)
	ld.b	 r13,r25,r19
	bcnd.n	 eq0,r13,@L29
	or.u	 r13,r0,hi16(_hard_regno_mode_ok)
@L25:
	or.u	 r13,r0,hi16(_movstr_optab)
	or	 r23,r13,lo16(_movstr_optab)
	or	 r13,r0,210
	st	 r13,r23[r25]
	addu	 r25,r25,1
	cmp	 r13,r25,20
	bb1.n	 le,r13,@L41
	cmp	 r13,r25,0
	bsr	 _end_sequence
	or	 r13,r0,77
	st	 r13,r23,16
@Lte0:
	ld	 r1,r31,80
	ld.d	 r24,r31,72
	ld.d	 r22,r31,64
	ld.d	 r20,r31,56
	ld.d	 r18,r31,48
	ld.d	 r16,r31,40
	jmp.n	 r1
	addu	 r31,r31,96

	align	 8
	global	 _init_expr
_init_expr:
	subu	 r31,r31,48
	st	 r1,r31,36
@Ltb1:
	bsr	 _init_queue
@Lte1:
	ld	 r1,r31,36
	or.u	 r13,r0,hi16(_pending_stack_adjust)
	st	 r0,r13,lo16(_pending_stack_adjust)
	or.u	 r13,r0,hi16(_inhibit_defer_pop)
	st	 r0,r13,lo16(_inhibit_defer_pop)
	or.u	 r13,r0,hi16(_cleanups_this_call)
	st	 r0,r13,lo16(_cleanups_this_call)
	or.u	 r13,r0,hi16(_saveregs_value)
	st	 r0,r13,lo16(_saveregs_value)
	or.u	 r13,r0,hi16(_forced_labels)
	addu	 r31,r31,48
	jmp.n	 r1
	st	 r0,r13,lo16(_forced_labels)

	align	 8
	global	 _save_expr_status
_save_expr_status:
	subu	 r31,r31,48
	st	 r25,r31,32
	st	 r1,r31,36
@Ltb2:
	bsr.n	 _emit_queue
	or	 r25,r0,r2
	or.u	 r5,r0,hi16(_pending_stack_adjust)
	or.u	 r4,r0,hi16(_inhibit_defer_pop)
	ld	 r9,r5,lo16(_pending_stack_adjust)
	or.u	 r6,r0,hi16(_cleanups_this_call)
	ld	 r10,r4,lo16(_inhibit_defer_pop)
	or.u	 r7,r0,hi16(_saveregs_value)
	ld	 r11,r6,lo16(_cleanups_this_call)
	or.u	 r8,r0,hi16(_forced_labels)
	ld	 r13,r7,lo16(_saveregs_value)
	ld	 r12,r8,lo16(_forced_labels)
	st	 r9,r25,212
	st	 r10,r25,216
	st	 r11,r25,220
	st	 r13,r25,224
	st	 r12,r25,228
@Lte2:
	ld	 r1,r31,36
	ld	 r25,r31,32
	st	 r0,r5,lo16(_pending_stack_adjust)
	st	 r0,r4,lo16(_inhibit_defer_pop)
	st	 r0,r6,lo16(_cleanups_this_call)
	st	 r0,r7,lo16(_saveregs_value)
	addu	 r31,r31,48
	jmp.n	 r1
	st	 r0,r8,lo16(_forced_labels)

	align	 8
	global	 _restore_expr_status
_restore_expr_status:
@Ltb3:
	ld	 r8,r2,212
	or.u	 r9,r0,hi16(_pending_stack_adjust)
	st	 r8,r9,lo16(_pending_stack_adjust)
	ld	 r8,r2,216
	or.u	 r9,r0,hi16(_inhibit_defer_pop)
	st	 r8,r9,lo16(_inhibit_defer_pop)
	ld	 r8,r2,220
	or.u	 r9,r0,hi16(_cleanups_this_call)
	st	 r8,r9,lo16(_cleanups_this_call)
	ld	 r8,r2,224
	or.u	 r9,r0,hi16(_saveregs_value)
	st	 r8,r9,lo16(_saveregs_value)
	ld	 r8,r2,228
	or.u	 r9,r0,hi16(_forced_labels)
	jmp.n	 r1
	st	 r8,r9,lo16(_forced_labels)
@Lte3:

	align	 8
	global	 _protect_from_queue
_protect_from_queue:
	subu	 r31,r31,64
	st	 r1,r31,48
	st.d	 r24,r31,40
	or	 r24,r0,r2
	st	 r23,r31,36
@Ltb4:
	ld.hu	 r11,r0,r24
	cmp	 r13,r11,59
	bb0.n	 ne,r13,@L59
	cmp	 r13,r11,55
	bb1.n	 ne,r13,@L68
	cmp	 r13,r11,63
	ld.bu	 r13,r24,2
	cmp	 r13,r13,16
	bb0	 ne,r13,@L60
	ld	 r23,r24,4
	ld.hu	 r13,r0,r23
	cmp	 r12,r3,0
	cmp	 r13,r13,59
	and	 r13,r13,r12
	extu	 r13,r13,1<eq>
	bcnd	 eq0,r13,@L60
	ld	 r13,r23,4
	st	 r13,r24,4
	ld	 r13,r23,8
	bcnd	 eq0,r13,@L63
	bsr.n	 _gen_reg_rtx
	ld.bu	 r2,r24,2
	or	 r25,r0,r2
	bsr.n	 _gen_move_insn
	or	 r3,r0,r24
	bsr.n	 _emit_insn_before
	ld	 r3,r23,8
	br.n	 @L67
	or	 r2,r0,r25
	align	 4
@L60:
	cmp	 r13,r11,55
	bb1.n	 ne,r13,@L62
	cmp	 r13,r11,63
	ld	 r2,r24,4
	bsr.n	 _protect_from_queue
	or	 r3,r0,0
	br.n	 @L63
	st	 r2,r24,4
	align	 4
@L62:
@L68:
	cmp	 r12,r11,66
	or	 r13,r13,r12
	extu	 r13,r13,1<eq>
	bcnd	 eq0,r13,@L63
	ld	 r2,r24,4
	bsr.n	 _protect_from_queue
	or	 r3,r0,0
	st	 r2,r24,4
	ld	 r2,r24,8
	bsr.n	 _protect_from_queue
	or	 r3,r0,0
	st	 r2,r24,8
@L63:
	br.n	 @L67
	or	 r2,r0,r24
	align	 4
@L59:
	ld	 r13,r24,8
	bcnd	 ne0,r13,@L65
	ld	 r2,r24,4
	br	 @L67
	align	 4
@L65:
	ld	 r2,r24,12
	bcnd	 ne0,r2,@L67
	ld	 r13,r24,4
	bsr.n	 _gen_reg_rtx
	ld.bu	 r2,r13,2
	ld	 r3,r24,4
	bsr.n	 _gen_move_insn
	st	 r2,r24,12
	bsr.n	 _emit_insn_before
	ld	 r3,r24,8
	ld	 r2,r24,12
@L67:
@Lte4:
	ld	 r1,r31,48
	ld	 r23,r31,36
	ld.d	 r24,r31,40
	jmp.n	 r1
	addu	 r31,r31,64

	align	 8
_queued_subexp_p:
	subu	 r31,r31,64
	st.d	 r24,r31,40
	or	 r25,r0,r2
	st	 r1,r31,48
@Ltb5:
	ld.hu	 r12,r0,r25
	cmp	 r13,r12,64
	bb0.n	 ls,r13,@L92
	cmp	 r13,r12,63
	bb0.n	 lo,r13,@L87
	cmp	 r13,r12,55
	bb0.n	 ne,r13,@L84
	cmp	 r13,r12,59
	bb0.n	 ne,r13,@L93
	or	 r2,r0,1
	br.n	 @L93
	or	 r2,r0,0
	align	 4
@L92:
	cmp	 r13,r12,66
	bb0.n	 ne,r13,@L87
	or	 r2,r0,0
	br	 @L93
	align	 4
@L84:
	ld	 r2,r25,4
	bsr.n	 _queued_subexp_p
	addu	 r1,r1,@L94
@L95:
	align	 4
@L87:
	ld	 r2,r25,4
	bsr.n	 _queued_subexp_p
	or	 r24,r0,0
	bcnd	 ne0,r2,@L89
	bsr.n	 _queued_subexp_p
	ld	 r2,r25,8
	bcnd.n	 eq0,r2,@L93
	or	 r2,r0,r24
@L89:
	or	 r24,r0,1
	or	 r2,r0,r24
@L93:
@Lte5:
	ld	 r1,r31,48
	ld.d	 r24,r31,40
	jmp.n	 r1
	addu	 r31,r31,64
	def	 @L94,@L93-@L95

	align	 8
	global	 _emit_queue
_emit_queue:
	subu	 r31,r31,64
	or.u	 r13,r0,hi16(_pending_chain)
	st.d	 r24,r31,40
	ld	 r25,r13,lo16(_pending_chain)
	bcnd.n	 eq0,r25,@L103
	st	 r1,r31,48
@Ltb6:
	or	 r24,r0,r13
@L104:
	bsr.n	 _emit_insn
	ld	 r2,r25,16
	ld	 r13,r25,20
	st	 r2,r25,8
	or	 r25,r0,r13
	bcnd.n	 ne0,r25,@L104
	st	 r25,r24,lo16(_pending_chain)
@L103:
@Lte6:
	ld	 r1,r31,48
	ld.d	 r24,r31,40
	jmp.n	 r1
	addu	 r31,r31,64

	align	 8
_init_queue:
	or.u	 r13,r0,hi16(_pending_chain)
	ld	 r13,r13,lo16(_pending_chain)
	subu	 r31,r31,48
	bcnd.n	 eq0,r13,@L110
	st	 r1,r31,36
@Ltb7:
	bsr	 _abort
	align	 4
@L110:
@Lte7:
	ld	 r1,r31,36
	jmp.n	 r1
	addu	 r31,r31,48

