/****************************************************************************
 *
 *   Copyright (c) 2012-2014 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/**
 * @file otp.h
 * One TIme Programmable ( OTP ) Flash routine/s.
 *
 * @author Lorenz Meier <lm@inf.ethz.ch>
 * @author David "Buzz" Bussenschutt <davidbuzz@gmail.com>
 *
 */

#جيڪڏهن_وصف_نه_ٿيل OTP_H_
#وصف_ڪريو OTP_H_

#وصف_ڪريو ADDR_OTP_شروعات			0x1FFF7800
#وصف_ڪريو ADDR_OTP_تالو_شروعات		0x1FFF7A00

#وصف_ڪريو OTP_تالو_LOCKED			0x00
#وصف_ڪريو OTP_تالو_UNLOCKED		0xFF



#ضمن <unistd.h>
#ضمن <stdint.h>
#ضمن <stdio.h>

// possible flash statuses
#وصف_ڪريو F_BUSY 1
#وصف_ڪريو F_ERROR_WRP 2
#وصف_ڪريو F_ERROR_پروگرام 3
#وصف_ڪريو F_ERROR_OPERATION 4
#وصف_ڪريو F_COMPLETE 5

تعريفنوع بنية {
	متطاير بنانشان ڊگهو accesscontrol;      // 0x00
	متطاير بنانشان ڊگهو ڪنجي;     //  0x04
	متطاير بنانشان ڊگهو optionkey;  //  0x08
	متطاير بنانشان ڊگهو حالت;       // 0x0C
	متطاير بنانشان ڊگهو control;       // 0x10
	متطاير بنانشان ڊگهو optioncontrol;    //0x14
} flash_registers;

#وصف_ڪريو PERIPH_بنياد           ((بنانشان ڊگهو)0x40000000) //Peripheral base address
#وصف_ڪريو AHB1_PERIPH_بنياد       (PERIPH_بنياد + 0x00020000)
#وصف_ڪريو F_R_بنياد          (AHB1_PERIPH_بنياد + 0x3C00)
#وصف_ڪريو FLASH               ((flash_registers *) F_R_بنياد)

#وصف_ڪريو F_BSY ((بنانشان ڊگهو)0x00010000) //FLASH Busy flag bit
#وصف_ڪريو F_OPERR ((بنانشان ڊگهو)0x00000002) //FLASH operation Error flag bit
#وصف_ڪريو F_WRPERR ((بنانشان ڊگهو)0x00000010) //FLASH Write protected error flag bit
#وصف_ڪريو CR_PSIZE_نقاب ((بنانشان ڊگهو)0xFFFFFCFF)
#وصف_ڪريو F_PSIZE_لفظ ((بنانشان ڊگهو)0x00000200)
#وصف_ڪريو F_PSIZE_BYTE ((بنانشان ڊگهو)0x00000000)
#وصف_ڪريو F_CR_PG ((بنانشان ڊگهو)0x00000001) // a bit in the F_CR register
#وصف_ڪريو F_CR_تالو ((بنانشان ڊگهو)0x80000000) // also another bit.

#وصف_ڪريو F_KEY1 ((بنانشان ڊگهو)0x45670123)
#وصف_ڪريو F_KEY2 ((بنانشان ڊگهو)0xCDEF89AB)
#وصف_ڪريو IS_F_پتو(پتو_3) ((((پتو_3) >= 0x08000000) && ((پتو_3) < 0x080FFFFF)) || (((پتو_3) >= 0x1FFF7800) && ((پتو_3) < 0x1FFF7A0F)))



#هدايت pack(ڌڪو, 1)

/*
 * The OTP area is divided into 16 OTP data blocks of 32 bytes and one lock OTP block of 16 bytes.
 * The OTP data and lock blocks cannot be erased. The lock block contains 16 bytes LOCKBi (0 ≤ i ≤ 15)
 * to lock the corresponding OTP data block (blocks 0 to 15). Each OTP data block can be programmed
 * until the value 0x00 is programmed in the corresponding OTP lock byte. The lock bytes must only
 * contain 0x00 and 0xFF values, otherwise the OTP bytes might not be taken into account correctly.
 */

بنية otp {
	// first 32 bytes =  the '0' Block
	حرف		id[4];		///4 bytes < 'P' 'X' '4' '\n'
	uint8_t		id_type;	///1 byte < 0 for USB VID, 1 for generic VID
	uint32_t	vid;        ///4 bytes
	uint32_t	pid;        ///4 bytes
	حرف        اڻ_استعمال_ٿيل[19];  ///19 bytes
	// Cert-of-Auth is next 4 blocks ie 1-4  ( where zero is first block )
	حرف        signature[128];
	// insert extras here
	uint32_t	تالو_بائيٽون[4];
};

بنية otp_تالو {
	uint8_t		تالو_بائيٽون[16];
};
#هدايت pack(ڪڍو)

#وصف_ڪريو ADDR_F_سائيز		0x1FFF7A22

#هدايت pack(ڌڪو, 1)
اتحاد udid {
	uint32_t	serial[3];
	uint8_t  معلومات[12];
};
#هدايت pack(ڪڍو)

__BEGIN_DECLS

__EXPORT فراغ F_unlock(فراغ);
__EXPORT فراغ F_تالو(فراغ);
__EXPORT صحيح val_read(فراغ *dest, متطاير ثابت فراغ *src, صحيح بائيٽون);
__EXPORT صحيح val_write(متطاير فراغ *dest, ثابت فراغ *src, صحيح بائيٽون);
__EXPORT صحيح write_otp(uint8_t id_type, uint32_t vid, uint32_t pid, حرف *signature);
__EXPORT صحيح تالو_otp(فراغ);


__EXPORT صحيح F_write_byte(بنانشان ڊگهو پتو_2, uint8_t معلومات_4);
__EXPORT صحيح F_write_لفظ(بنانشان ڊگهو پتو_2, uint32_t معلومات_4);

__END_DECLS

#جيڪڏهن_پورو
