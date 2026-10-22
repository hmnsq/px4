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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ OTP_H_
#ವ್ಯಾಖ್ಯಾನಿಸು OTP_H_

#ವ್ಯಾಖ್ಯಾನಿಸು ADDR_OTP_ಆರಂಭ			0x1FFF7800
#ವ್ಯಾಖ್ಯಾನಿಸು ADDR_OTP_ಬೀಗ_ಆರಂಭ		0x1FFF7A00

#ವ್ಯಾಖ್ಯಾನಿಸು OTP_ಬೀಗ_LOCKED			0x00
#ವ್ಯಾಖ್ಯಾನಿಸು OTP_ಬೀಗ_UNLOCKED		0xFF



#ಅನ್ತರ್ಭಾವ <unistd.h>
#ಅನ್ತರ್ಭಾವ <stdint.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>

// possible flash statuses
#ವ್ಯಾಖ್ಯಾನಿಸು F_BUSY 1
#ವ್ಯಾಖ್ಯಾನಿಸು F_ERROR_WRP 2
#ವ್ಯಾಖ್ಯಾನಿಸು F_ERROR_ಕಾರ್ಯಕ್ರಮ 3
#ವ್ಯಾಖ್ಯಾನಿಸು F_ERROR_OPERATION 4
#ವ್ಯಾಖ್ಯಾನಿಸು F_COMPLETE 5

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
	ಅಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ accesscontrol;      // 0x00
	ಅಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಕೀಲಿ;     //  0x04
	ಅಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ optionkey;  //  0x08
	ಅಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ಸ್ಥಿತಿ_2;       // 0x0C
	ಅಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ control;       // 0x10
	ಅಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ optioncontrol;    //0x14
} flash_registers;

#ವ್ಯಾಖ್ಯಾನಿಸು PERIPH_ತಳ           ((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)0x40000000) //Peripheral base address
#ವ್ಯಾಖ್ಯಾನಿಸು AHB1_PERIPH_ತಳ       (PERIPH_ತಳ + 0x00020000)
#ವ್ಯಾಖ್ಯಾನಿಸು F_R_ತಳ          (AHB1_PERIPH_ತಳ + 0x3C00)
#ವ್ಯಾಖ್ಯಾನಿಸು FLASH               ((flash_registers *) F_R_ತಳ)

#ವ್ಯಾಖ್ಯಾನಿಸು F_BSY ((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)0x00010000) //FLASH Busy flag bit
#ವ್ಯಾಖ್ಯಾನಿಸು F_OPERR ((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)0x00000002) //FLASH operation Error flag bit
#ವ್ಯಾಖ್ಯಾನಿಸು F_WRPERR ((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)0x00000010) //FLASH Write protected error flag bit
#ವ್ಯಾಖ್ಯಾನಿಸು CR_PSIZE_ಮುಖವಾಡ ((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)0xFFFFFCFF)
#ವ್ಯಾಖ್ಯಾನಿಸು F_PSIZE_ಶಬ್ದ ((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)0x00000200)
#ವ್ಯಾಖ್ಯಾನಿಸು F_PSIZE_BYTE ((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)0x00000000)
#ವ್ಯಾಖ್ಯಾನಿಸು F_CR_PG ((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)0x00000001) // a bit in the F_CR register
#ವ್ಯಾಖ್ಯಾನಿಸು F_CR_ಬೀಗ ((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)0x80000000) // also another bit.

#ವ್ಯಾಖ್ಯಾನಿಸು F_KEY1 ((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)0x45670123)
#ವ್ಯಾಖ್ಯಾನಿಸು F_KEY2 ((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)0xCDEF89AB)
#ವ್ಯಾಖ್ಯಾನಿಸು IS_F_ವಿಳಾಸ(ವಿಳಾಸ_3) ((((ವಿಳಾಸ_3) >= 0x08000000) && ((ವಿಳಾಸ_3) < 0x080FFFFF)) || (((ವಿಳಾಸ_3) >= 0x1FFF7800) && ((ವಿಳಾಸ_3) < 0x1FFF7A0F)))



#ಸೂಚನೆ pack(ತಳ್ಳು, 1)

/*
 * The OTP area is divided into 16 OTP data blocks of 32 bytes and one lock OTP block of 16 bytes.
 * The OTP data and lock blocks cannot be erased. The lock block contains 16 bytes LOCKBi (0 ≤ i ≤ 15)
 * to lock the corresponding OTP data block (blocks 0 to 15). Each OTP data block can be programmed
 * until the value 0x00 is programmed in the corresponding OTP lock byte. The lock bytes must only
 * contain 0x00 and 0xFF values, otherwise the OTP bytes might not be taken into account correctly.
 */

ರಚನೆ otp {
	// first 32 bytes =  the '0' Block
	ಅಕ್ಷರ		id[4];		///4 bytes < 'P' 'X' '4' '\n'
	uint8_t		id_type;	///1 byte < 0 for USB VID, 1 for generic VID
	uint32_t	vid;        ///4 bytes
	uint32_t	pid;        ///4 bytes
	ಅಕ್ಷರ        ಬಳಸದ[19];  ///19 bytes
	// Cert-of-Auth is next 4 blocks ie 1-4  ( where zero is first block )
	ಅಕ್ಷರ        signature[128];
	// insert extras here
	uint32_t	ಬೀಗ_ಬೈಟುಗಳು[4];
};

ರಚನೆ otp_ಬೀಗ {
	uint8_t		ಬೀಗ_ಬೈಟುಗಳು[16];
};
#ಸೂಚನೆ pack(ತೆಗೆ_2)

#ವ್ಯಾಖ್ಯಾನಿಸು ADDR_F_ಗಾತ್ರ		0x1FFF7A22

#ಸೂಚನೆ pack(ತಳ್ಳು, 1)
ಒಕ್ಕೂಟ udid {
	uint32_t	serial[3];
	uint8_t  ದತ್ತಾಂಶ[12];
};
#ಸೂಚನೆ pack(ತೆಗೆ_2)

__BEGIN_DECLS

__EXPORT ಶೂನ್ಯ F_unlock(ಶೂನ್ಯ);
__EXPORT ಶೂನ್ಯ F_ಬೀಗ(ಶೂನ್ಯ);
__EXPORT ಪೂರ್ಣಾಂಕ val_read(ಶೂನ್ಯ *dest, ಅಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *src, ಪೂರ್ಣಾಂಕ ಬೈಟುಗಳು);
__EXPORT ಪೂರ್ಣಾಂಕ val_write(ಅಸ್ಥಿರ ಶೂನ್ಯ *dest, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *src, ಪೂರ್ಣಾಂಕ ಬೈಟುಗಳು);
__EXPORT ಪೂರ್ಣಾಂಕ write_otp(uint8_t id_type, uint32_t vid, uint32_t pid, ಅಕ್ಷರ *signature);
__EXPORT ಪೂರ್ಣಾಂಕ ಬೀಗ_otp(ಶೂನ್ಯ);


__EXPORT ಪೂರ್ಣಾಂಕ F_write_byte(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ವಿಳಾಸ_2, uint8_t ದತ್ತಾಂಶ_4);
__EXPORT ಪೂರ್ಣಾಂಕ F_write_ಶಬ್ದ(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ವಿಳಾಸ_2, uint32_t ದತ್ತಾಂಶ_4);

__END_DECLS

#ಆದರೆ_ಅಂತ್ಯ
