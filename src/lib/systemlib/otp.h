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

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ OTP_H_
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OTP_H_

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ADDR_OTP_ਸ਼ੁਰੂ			0x1FFF7800
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ADDR_OTP_ਤਾਲਾ_ਸ਼ੁਰੂ		0x1FFF7A00

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OTP_ਤਾਲਾ_LOCKED			0x00
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OTP_ਤਾਲਾ_UNLOCKED		0xFF



#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdint.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>

// possible flash statuses
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ F_BUSY 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ F_ERROR_WRP 2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ F_ERROR_ਪ੍ਰੋਗਰਾਮ 3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ F_ERROR_OPERATION 4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ F_COMPLETE 5

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
	ਅਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ accesscontrol;      // 0x00
	ਅਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਕੁੰਜੀ;     //  0x04
	ਅਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ optionkey;  //  0x08
	ਅਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਸਥਿਤੀ;       // 0x0C
	ਅਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ control;       // 0x10
	ਅਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ optioncontrol;    //0x14
} flash_registers;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PERIPH_ਅਧਾਰ           ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)0x40000000) //Peripheral base address
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AHB1_PERIPH_ਅਧਾਰ       (PERIPH_ਅਧਾਰ + 0x00020000)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ F_R_ਅਧਾਰ          (AHB1_PERIPH_ਅਧਾਰ + 0x3C00)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FLASH               ((flash_registers *) F_R_ਅਧਾਰ)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ F_BSY ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)0x00010000) //FLASH Busy flag bit
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ F_OPERR ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)0x00000002) //FLASH operation Error flag bit
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ F_WRPERR ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)0x00000010) //FLASH Write protected error flag bit
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CR_PSIZE_ਨਕਾਬ ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)0xFFFFFCFF)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ F_PSIZE_ਸ਼ਬਦ ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)0x00000200)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ F_PSIZE_BYTE ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)0x00000000)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ F_CR_PG ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)0x00000001) // a bit in the F_CR register
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ F_CR_ਤਾਲਾ ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)0x80000000) // also another bit.

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ F_KEY1 ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)0x45670123)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ F_KEY2 ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)0xCDEF89AB)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ IS_F_ਪਤਾ(ਪਤਾ_3) ((((ਪਤਾ_3) >= 0x08000000) && ((ਪਤਾ_3) < 0x080FFFFF)) || (((ਪਤਾ_3) >= 0x1FFF7800) && ((ਪਤਾ_3) < 0x1FFF7A0F)))



#ਹਦਾਇਤ pack(ਧੱਕੋ, 1)

/*
 * The OTP area is divided into 16 OTP data blocks of 32 bytes and one lock OTP block of 16 bytes.
 * The OTP data and lock blocks cannot be erased. The lock block contains 16 bytes LOCKBi (0 ≤ i ≤ 15)
 * to lock the corresponding OTP data block (blocks 0 to 15). Each OTP data block can be programmed
 * until the value 0x00 is programmed in the corresponding OTP lock byte. The lock bytes must only
 * contain 0x00 and 0xFF values, otherwise the OTP bytes might not be taken into account correctly.
 */

ਬਣਤਰ otp {
	// first 32 bytes =  the '0' Block
	ਅੱਖਰ		id[4];		///4 bytes < 'P' 'X' '4' '\n'
	uint8_t		id_type;	///1 byte < 0 for USB VID, 1 for generic VID
	uint32_t	vid;        ///4 bytes
	uint32_t	pid;        ///4 bytes
	ਅੱਖਰ        ਅਣਵਰਤਿਆ[19];  ///19 bytes
	// Cert-of-Auth is next 4 blocks ie 1-4  ( where zero is first block )
	ਅੱਖਰ        signature[128];
	// insert extras here
	uint32_t	ਤਾਲਾ_ਬਾਈਟਾਂ[4];
};

ਬਣਤਰ otp_ਤਾਲਾ {
	uint8_t		ਤਾਲਾ_ਬਾਈਟਾਂ[16];
};
#ਹਦਾਇਤ pack(ਕੱਢੋ)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ADDR_F_ਮਾਪ		0x1FFF7A22

#ਹਦਾਇਤ pack(ਧੱਕੋ, 1)
ਸੰਘ udid {
	uint32_t	serial[3];
	uint8_t  ਅੰਕੜੇ[12];
};
#ਹਦਾਇਤ pack(ਕੱਢੋ)

__BEGIN_DECLS

__EXPORT ਖਾਲੀ F_unlock(ਖਾਲੀ);
__EXPORT ਖਾਲੀ F_ਤਾਲਾ(ਖਾਲੀ);
__EXPORT ਪੂਰਨ val_read(ਖਾਲੀ *dest, ਅਸਥਿਰ ਅਚਲ ਖਾਲੀ *src, ਪੂਰਨ ਬਾਈਟਾਂ);
__EXPORT ਪੂਰਨ val_write(ਅਸਥਿਰ ਖਾਲੀ *dest, ਅਚਲ ਖਾਲੀ *src, ਪੂਰਨ ਬਾਈਟਾਂ);
__EXPORT ਪੂਰਨ write_otp(uint8_t id_type, uint32_t vid, uint32_t pid, ਅੱਖਰ *signature);
__EXPORT ਪੂਰਨ ਤਾਲਾ_otp(ਖਾਲੀ);


__EXPORT ਪੂਰਨ F_write_byte(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਪਤਾ_2, uint8_t ਅੰਕੜੇ_4);
__EXPORT ਪੂਰਨ F_write_ਸ਼ਬਦ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ ਪਤਾ_2, uint32_t ਅੰਕੜੇ_4);

__END_DECLS

#ਜੇ_ਅੰਤ
