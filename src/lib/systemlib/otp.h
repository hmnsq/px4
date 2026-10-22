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

#اگر_تعریف_نشده OTP_H_
#تعریف_کن OTP_H_

#تعریف_کن ADDR_OTP_شروع			0x1FFF7800
#تعریف_کن ADDR_OTP_قفل_شروع		0x1FFF7A00

#تعریف_کن OTP_قفل_LOCKED			0x00
#تعریف_کن OTP_قفل_UNLOCKED		0xFF



#ضمن <unistd.h>
#ضمن <stdint.h>
#ضمن <stdio.h>

// possible flash statuses
#تعریف_کن F_BUSY 1
#تعریف_کن F_ERROR_WRP 2
#تعریف_کن F_ERROR_برنامه 3
#تعریف_کن F_ERROR_OPERATION 4
#تعریف_کن F_COMPLETE 5

تعريفنوع بنية {
	متطاير بیعلامت دراز accesscontrol;      // 0x00
	متطاير بیعلامت دراز کلید;     //  0x04
	متطاير بیعلامت دراز optionkey;  //  0x08
	متطاير بیعلامت دراز وضعیت;       // 0x0C
	متطاير بیعلامت دراز control;       // 0x10
	متطاير بیعلامت دراز optioncontrol;    //0x14
} flash_registers;

#تعریف_کن PERIPH_پایه           ((بیعلامت دراز)0x40000000) //Peripheral base address
#تعریف_کن AHB1_PERIPH_پایه       (PERIPH_پایه + 0x00020000)
#تعریف_کن F_R_پایه          (AHB1_PERIPH_پایه + 0x3C00)
#تعریف_کن FLASH               ((flash_registers *) F_R_پایه)

#تعریف_کن F_BSY ((بیعلامت دراز)0x00010000) //FLASH Busy flag bit
#تعریف_کن F_OPERR ((بیعلامت دراز)0x00000002) //FLASH operation Error flag bit
#تعریف_کن F_WRPERR ((بیعلامت دراز)0x00000010) //FLASH Write protected error flag bit
#تعریف_کن CR_PSIZE_ماسک ((بیعلامت دراز)0xFFFFFCFF)
#تعریف_کن F_PSIZE_کلمه ((بیعلامت دراز)0x00000200)
#تعریف_کن F_PSIZE_BYTE ((بیعلامت دراز)0x00000000)
#تعریف_کن F_CR_PG ((بیعلامت دراز)0x00000001) // a bit in the F_CR register
#تعریف_کن F_CR_قفل ((بیعلامت دراز)0x80000000) // also another bit.

#تعریف_کن F_KEY1 ((بیعلامت دراز)0x45670123)
#تعریف_کن F_KEY2 ((بیعلامت دراز)0xCDEF89AB)
#تعریف_کن IS_F_آدرس(آدرس_3) ((((آدرس_3) >= 0x08000000) && ((آدرس_3) < 0x080FFFFF)) || (((آدرس_3) >= 0x1FFF7800) && ((آدرس_3) < 0x1FFF7A0F)))



#رهنمود pack(افزودن, 1)

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
	حرف        استفادهنشده[19];  ///19 bytes
	// Cert-of-Auth is next 4 blocks ie 1-4  ( where zero is first block )
	حرف        signature[128];
	// insert extras here
	uint32_t	قفل_بایتها[4];
};

بنية otp_قفل {
	uint8_t		قفل_بایتها[16];
};
#رهنمود pack(برداشت)

#تعریف_کن ADDR_F_اندازه		0x1FFF7A22

#رهنمود pack(افزودن, 1)
اتحاد udid {
	uint32_t	serial[3];
	uint8_t  داده[12];
};
#رهنمود pack(برداشت)

__BEGIN_DECLS

__EXPORT فراغ F_unlock(فراغ);
__EXPORT فراغ F_قفل(فراغ);
__EXPORT صحيح val_read(فراغ *dest, متطاير ثابت فراغ *src, صحيح بایتها);
__EXPORT صحيح val_write(متطاير فراغ *dest, ثابت فراغ *src, صحيح بایتها);
__EXPORT صحيح write_otp(uint8_t id_type, uint32_t vid, uint32_t pid, حرف *signature);
__EXPORT صحيح قفل_otp(فراغ);


__EXPORT صحيح F_write_byte(بیعلامت دراز آدرس_2, uint8_t داده_4);
__EXPORT صحيح F_write_کلمه(بیعلامت دراز آدرس_2, uint32_t داده_4);

__END_DECLS

#پایان_اگر
