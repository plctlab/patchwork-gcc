/* Copyright (C) 1995-2022 Free Software Foundation, Inc.

This file is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 3, or (at your option) any
later version.

This file is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

Under Section 7 of GPL version 3, you are granted additional
permissions described in the GCC Runtime Library Exception, version
3.1, as published by the Free Software Foundation.

You should have received a copy of the GNU General Public License and
a copy of the GCC Runtime Library Exception along with this program;
see the files COPYING3 and COPYING.RUNTIME respectively.  If not, see
<http://www.gnu.org/licenses/>.  */


#ifdef L_lshrdi3

	FUNC_START lshrdi3
	FUNC_ALIAS aeabi_llsr lshrdi3
	
#ifdef __thumb__
	lsrs	al, r2
	movs	r3, ah
	lsrs	ah, r2
	mov	ip, r3
	subs	r2, #32
	lsrs	r3, r2
	orrs	al, r3
	negs	r2, r2
	mov	r3, ip
	lsls	r3, r2
	orrs	al, r3
	RET
#else
	subs	r3, r2, #32
	rsb	ip, r2, #32
	movmi	al, al, lsr r2
	movpl	al, ah, lsr r3
	orrmi	al, al, ah, lsl ip
	mov	ah, ah, lsr r2
	RET
#endif
	FUNC_END aeabi_llsr
	FUNC_END lshrdi3

#endif
	
#ifdef L_ashrdi3
	
	FUNC_START ashrdi3
	FUNC_ALIAS aeabi_lasr ashrdi3
	
#ifdef __thumb__
	lsrs	al, r2
	movs	r3, ah
	asrs	ah, r2
	subs	r2, #32
	@ If r2 is negative at this point the following step would OR
	@ the sign bit into all of AL.  That's not what we want...
	bmi	1f
	mov	ip, r3
	asrs	r3, r2
	orrs	al, r3
	mov	r3, ip
1:
	negs	r2, r2
	lsls	r3, r2
	orrs	al, r3
	RET
#else
	subs	r3, r2, #32
	rsb	ip, r2, #32
	movmi	al, al, lsr r2
	movpl	al, ah, asr r3
	orrmi	al, al, ah, lsl ip
	mov	ah, ah, asr r2
	RET
#endif

	FUNC_END aeabi_lasr
	FUNC_END ashrdi3

#endif

#ifdef L_ashldi3

	FUNC_START ashldi3
	FUNC_ALIAS aeabi_llsl ashldi3
	
#ifdef __thumb__
	lsls	ah, r2
	movs	r3, al
	lsls	al, r2
	mov	ip, r3
	subs	r2, #32
	lsls	r3, r2
	orrs	ah, r3
	negs	r2, r2
	mov	r3, ip
	lsrs	r3, r2
	orrs	ah, r3
	RET
#else
	subs	r3, r2, #32
	rsb	ip, r2, #32
	movmi	ah, ah, lsl r2
	movpl	ah, al, lsl r3
	orrmi	ah, ah, al, lsr ip
	mov	al, al, lsl r2
	RET
#endif
	FUNC_END aeabi_llsl
	FUNC_END ashldi3

#endif

