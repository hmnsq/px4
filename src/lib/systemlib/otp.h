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

#که_نه_تعریف_شوی OTP_H_
#تعریف_کړه OTP_H_

#تعریف_کړه ADDR_OTP_پیل			0x1FFF7800
#تعریف_کړه ADDR_OTP_قلف_پیل		0x1FFF7A00

#تعریف_کړه OTP_قلف_LOCKED			0x00
#تعریف_کړه OTP_قلف_UNLOCKED		0xFF



#ضمن <unistd.h>
#ضمن <stdint.h>
#ضمن <stdio.h>

// possible flash statuses
#تعریف_کړه F_BUSY 1
#تعریف_کړه F_ERROR_WRP 2
#تعریف_کړه F_ERROR_پروګرام 3
#تعریف_کړه F_ERROR_OPERATION 4
#تعریف_کړه F_COMPLETE 5

تعريفنوع بنية {
	متطاير بینښه اوږد accesscontrol;      // 0x00
	متطاير بینښه اوږد کلی;     //  0x04
	متطاير بینښه اوږد optionkey;  //  0x08
	متطاير بینښه اوږد حالت;       // 0x0C
	متطاير بینښه اوږد control;       // 0x10
	متطاير بینښه اوږد optioncontrol;    //0x14
} flash_registers;

#تعریف_کړه PERIPH_بنسټ           ((بینښه اوږد)0x40000000) //Peripheral base address
#تعریف_کړه AHB1_PERIPH_بنسټ       (PERIPH_بنسټ + 0x00020000)
#تعریف_کړه F_R_بنسټ          (AHB1_PERIPH_بنسټ + 0x3C00)
#تعریف_کړه FLASH               ((flash_registers *) F_R_بنسټ)

#تعریف_کړه F_BSY ((بینښه اوږد)0x00010000) //FLASH Busy flag bit
#تعریف_کړه F_OPERR ((بینښه اوږد)0x00000002) //FLASH operation Error flag bit
#تعریف_کړه F_WRPERR ((بینښه اوږد)0x00000010) //FLASH Write protected error flag bit
#تعریف_کړه CR_PSIZE_ماسک ((بینښه اوږد)0xFFFFFCFF)
#تعریف_کړه F_PSIZE_کلمه ((بینښه اوږد)0x00000200)
#تعریف_کړه F_PSIZE_BYTE ((بینښه اوږد)0x00000000)
#تعریف_کړه F_CR_PG ((بینښه اوږد)0x00000001) // a bit in the F_CR register
#تعریف_کړه F_CR_قلف ((بینښه اوږد)0x80000000) // also another bit.

#تعریف_کړه F_KEY1 ((بینښه اوږد)0x45670123)
#تعریف_کړه F_KEY2 ((بینښه اوږد)0xCDEF89AB)
#تعریف_کړه IS_F_پته(پته_3) ((((پته_3) >= 0x08000000) && ((پته_3) < 0x080FFFFF)) || (((پته_3) >= 0x1FFF7800) && ((پته_3) < 0x1FFF7A0F)))



#لارښوونه pack(ټېل, 1)

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
	حرف        ناکارول_شوی[19];  ///19 bytes
	// Cert-of-Auth is next 4 blocks ie 1-4  ( where zero is first block )
	حرف        signature[128];
	// insert extras here
	uint32_t	قلف_بایټونه[4];
};

بنية otp_قلف {
	uint8_t		قلف_بایټونه[16];
};
#لارښوونه pack(راوباسه)

#تعریف_کړه ADDR_F_اندازه		0x1FFF7A22

#لارښوونه pack(ټېل, 1)
اتحاد udid {
	uint32_t	serial[3];
	uint8_t  معلومات[12];
};
#لارښوونه pack(راوباسه)

__BEGIN_DECLS

__EXPORT فراغ F_unlock(فراغ);
__EXPORT فراغ F_قلف(فراغ);
__EXPORT صحيح val_read(فراغ *dest, متطاير ثابت فراغ *src, صحيح بایټونه);
__EXPORT صحيح val_write(متطاير فراغ *dest, ثابت فراغ *src, صحيح بایټونه);
__EXPORT صحيح write_otp(uint8_t id_type, uint32_t vid, uint32_t pid, حرف *signature);
__EXPORT صحيح قلف_otp(فراغ);


__EXPORT صحيح F_write_byte(بینښه اوږد پته_2, uint8_t معلومات_4);
__EXPORT صحيح F_write_کلمه(بینښه اوږد پته_2, uint32_t معلومات_4);

__END_DECLS

#د_که_پای
