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

#اگر_غیر_تعریف_شدہ OTP_H_
#تعریف_کرو OTP_H_

#تعریف_کرو ADDR_OTP_آغاز			0x1FFF7800
#تعریف_کرو ADDR_OTP_تالا_آغاز		0x1FFF7A00

#تعریف_کرو OTP_تالا_LOCKED			0x00
#تعریف_کرو OTP_تالا_UNLOCKED		0xFF



#شامل <unistd.h>
#شامل <stdint.h>
#شامل <stdio.h>

// possible flash statuses
#تعریف_کرو F_BUSY 1
#تعریف_کرو F_ERROR_WRP 2
#تعریف_کرو F_ERROR_پروگرام 3
#تعریف_کرو F_ERROR_OPERATION 4
#تعریف_کرو F_COMPLETE 5

قسمتعریف ساخت {
	متلون غیرعلامتی طویل accesscontrol;      // 0x00
	متلون غیرعلامتی طویل کلید;     //  0x04
	متلون غیرعلامتی طویل optionkey;  //  0x08
	متلون غیرعلامتی طویل حالت_2;       // 0x0C
	متلون غیرعلامتی طویل control;       // 0x10
	متلون غیرعلامتی طویل optioncontrol;    //0x14
} flash_registers;

#تعریف_کرو PERIPH_بنیاد           ((غیرعلامتی طویل)0x40000000) //Peripheral base address
#تعریف_کرو AHB1_PERIPH_بنیاد       (PERIPH_بنیاد + 0x00020000)
#تعریف_کرو F_R_بنیاد          (AHB1_PERIPH_بنیاد + 0x3C00)
#تعریف_کرو FLASH               ((flash_registers *) F_R_بنیاد)

#تعریف_کرو F_BSY ((غیرعلامتی طویل)0x00010000) //FLASH Busy flag bit
#تعریف_کرو F_OPERR ((غیرعلامتی طویل)0x00000002) //FLASH operation Error flag bit
#تعریف_کرو F_WRPERR ((غیرعلامتی طویل)0x00000010) //FLASH Write protected error flag bit
#تعریف_کرو CR_PSIZE_نقاب ((غیرعلامتی طویل)0xFFFFFCFF)
#تعریف_کرو F_PSIZE_لفظ ((غیرعلامتی طویل)0x00000200)
#تعریف_کرو F_PSIZE_BYTE ((غیرعلامتی طویل)0x00000000)
#تعریف_کرو F_CR_PG ((غیرعلامتی طویل)0x00000001) // a bit in the F_CR register
#تعریف_کرو F_CR_تالا ((غیرعلامتی طویل)0x80000000) // also another bit.

#تعریف_کرو F_KEY1 ((غیرعلامتی طویل)0x45670123)
#تعریف_کرو F_KEY2 ((غیرعلامتی طویل)0xCDEF89AB)
#تعریف_کرو IS_F_پتہ(پتہ_3) ((((پتہ_3) >= 0x08000000) && ((پتہ_3) < 0x080FFFFF)) || (((پتہ_3) >= 0x1FFF7800) && ((پتہ_3) < 0x1FFF7A0F)))



#ہدایت pack(ڈالو, 1)

/*
 * The OTP area is divided into 16 OTP data blocks of 32 bytes and one lock OTP block of 16 bytes.
 * The OTP data and lock blocks cannot be erased. The lock block contains 16 bytes LOCKBi (0 ≤ i ≤ 15)
 * to lock the corresponding OTP data block (blocks 0 to 15). Each OTP data block can be programmed
 * until the value 0x00 is programmed in the corresponding OTP lock byte. The lock bytes must only
 * contain 0x00 and 0xFF values, otherwise the OTP bytes might not be taken into account correctly.
 */

ساخت otp {
	// first 32 bytes =  the '0' Block
	حرف		id[4];		///4 bytes < 'P' 'X' '4' '\n'
	uint8_t		id_type;	///1 byte < 0 for USB VID, 1 for generic VID
	uint32_t	vid;        ///4 bytes
	uint32_t	pid;        ///4 bytes
	حرف        غیر_مستعمل[19];  ///19 bytes
	// Cert-of-Auth is next 4 blocks ie 1-4  ( where zero is first block )
	حرف        signature[128];
	// insert extras here
	uint32_t	تالا_بائٹیں[4];
};

ساخت otp_تالا {
	uint8_t		تالا_بائٹیں[16];
};
#ہدایت pack(نکالو)

#تعریف_کرو ADDR_F_سائز		0x1FFF7A22

#ہدایت pack(ڈالو, 1)
اتحاد udid {
	uint32_t	serial[3];
	uint8_t  ڈیٹا[12];
};
#ہدایت pack(نکالو)

__BEGIN_DECLS

__EXPORT خالی F_unlock(خالی);
__EXPORT خالی F_تالا(خالی);
__EXPORT صحیح val_read(خالی *dest, متلون ثابت خالی *src, صحیح بائٹیں);
__EXPORT صحیح val_write(متلون خالی *dest, ثابت خالی *src, صحیح بائٹیں);
__EXPORT صحیح write_otp(uint8_t id_type, uint32_t vid, uint32_t pid, حرف *signature);
__EXPORT صحیح تالا_otp(خالی);


__EXPORT صحیح F_write_byte(غیرعلامتی طویل پتہ_2, uint8_t ڈیٹا_4);
__EXPORT صحیح F_write_لفظ(غیرعلامتی طویل پتہ_2, uint32_t ڈیٹا_4);

__END_DECLS

#اگر_ختم
