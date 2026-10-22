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

#నిర్వచితం_కాకపోతే OTP_H_
#నిర్వచించు OTP_H_

#నిర్వచించు ADDR_OTP_ప్రారంభం			0x1FFF7800
#నిర్వచించు ADDR_OTP_తాళం_ప్రారంభం		0x1FFF7A00

#నిర్వచించు OTP_తాళం_LOCKED			0x00
#నిర్వచించు OTP_తాళం_UNLOCKED		0xFF



#అన్తర్భావ <unistd.h>
#అన్తర్భావ <stdint.h>
#అన్తర్భావ <stdio.h>

// possible flash statuses
#నిర్వచించు F_BUSY 1
#నిర్వచించు F_ERROR_WRP 2
#నిర్వచించు F_ERROR_కార్యక్రమం 3
#నిర్వచించు F_ERROR_OPERATION 4
#నిర్వచించు F_COMPLETE 5

రకనిర్వచనం నిర్మాణం {
	అస్థిర గుర్తులేని దీర్ఘ accesscontrol;      // 0x00
	అస్థిర గుర్తులేని దీర్ఘ కీలకం;     //  0x04
	అస్థిర గుర్తులేని దీర్ఘ optionkey;  //  0x08
	అస్థిర గుర్తులేని దీర్ఘ స్థితి_2;       // 0x0C
	అస్థిర గుర్తులేని దీర్ఘ control;       // 0x10
	అస్థిర గుర్తులేని దీర్ఘ optioncontrol;    //0x14
} flash_registers;

#నిర్వచించు PERIPH_పునాది           ((గుర్తులేని దీర్ఘ)0x40000000) //Peripheral base address
#నిర్వచించు AHB1_PERIPH_పునాది       (PERIPH_పునాది + 0x00020000)
#నిర్వచించు F_R_పునాది          (AHB1_PERIPH_పునాది + 0x3C00)
#నిర్వచించు FLASH               ((flash_registers *) F_R_పునాది)

#నిర్వచించు F_BSY ((గుర్తులేని దీర్ఘ)0x00010000) //FLASH Busy flag bit
#నిర్వచించు F_OPERR ((గుర్తులేని దీర్ఘ)0x00000002) //FLASH operation Error flag bit
#నిర్వచించు F_WRPERR ((గుర్తులేని దీర్ఘ)0x00000010) //FLASH Write protected error flag bit
#నిర్వచించు CR_PSIZE_ముసుగు ((గుర్తులేని దీర్ఘ)0xFFFFFCFF)
#నిర్వచించు F_PSIZE_మాట ((గుర్తులేని దీర్ఘ)0x00000200)
#నిర్వచించు F_PSIZE_BYTE ((గుర్తులేని దీర్ఘ)0x00000000)
#నిర్వచించు F_CR_PG ((గుర్తులేని దీర్ఘ)0x00000001) // a bit in the F_CR register
#నిర్వచించు F_CR_తాళం ((గుర్తులేని దీర్ఘ)0x80000000) // also another bit.

#నిర్వచించు F_KEY1 ((గుర్తులేని దీర్ఘ)0x45670123)
#నిర్వచించు F_KEY2 ((గుర్తులేని దీర్ఘ)0xCDEF89AB)
#నిర్వచించు IS_F_చిరునామా(చిరునామా_3) ((((చిరునామా_3) >= 0x08000000) && ((చిరునామా_3) < 0x080FFFFF)) || (((చిరునామా_3) >= 0x1FFF7800) && ((చిరునామా_3) < 0x1FFF7A0F)))



#సూచన pack(నెట్టు, 1)

/*
 * The OTP area is divided into 16 OTP data blocks of 32 bytes and one lock OTP block of 16 bytes.
 * The OTP data and lock blocks cannot be erased. The lock block contains 16 bytes LOCKBi (0 ≤ i ≤ 15)
 * to lock the corresponding OTP data block (blocks 0 to 15). Each OTP data block can be programmed
 * until the value 0x00 is programmed in the corresponding OTP lock byte. The lock bytes must only
 * contain 0x00 and 0xFF values, otherwise the OTP bytes might not be taken into account correctly.
 */

నిర్మాణం otp {
	// first 32 bytes =  the '0' Block
	అక్షరం		id[4];		///4 bytes < 'P' 'X' '4' '\n'
	uint8_t		id_type;	///1 byte < 0 for USB VID, 1 for generic VID
	uint32_t	vid;        ///4 bytes
	uint32_t	pid;        ///4 bytes
	అక్షరం        వాడని[19];  ///19 bytes
	// Cert-of-Auth is next 4 blocks ie 1-4  ( where zero is first block )
	అక్షరం        signature[128];
	// insert extras here
	uint32_t	తాళం_బైట్లు[4];
};

నిర్మాణం otp_తాళం {
	uint8_t		తాళం_బైట్లు[16];
};
#సూచన pack(తీయి)

#నిర్వచించు ADDR_F_కొలత		0x1FFF7A22

#సూచన pack(నెట్టు, 1)
సంఘం udid {
	uint32_t	serial[3];
	uint8_t  దత్తాంశం[12];
};
#సూచన pack(తీయి)

__BEGIN_DECLS

__EXPORT శూన్య F_unlock(శూన్య);
__EXPORT శూన్య F_తాళం(శూన్య);
__EXPORT పూర్ణాంకం val_read(శూన్య *dest, అస్థిర స్థిరరాశి శూన్య *src, పూర్ణాంకం బైట్లు);
__EXPORT పూర్ణాంకం val_write(అస్థిర శూన్య *dest, స్థిరరాశి శూన్య *src, పూర్ణాంకం బైట్లు);
__EXPORT పూర్ణాంకం write_otp(uint8_t id_type, uint32_t vid, uint32_t pid, అక్షరం *signature);
__EXPORT పూర్ణాంకం తాళం_otp(శూన్య);


__EXPORT పూర్ణాంకం F_write_byte(గుర్తులేని దీర్ఘ చిరునామా_2, uint8_t దత్తాంశం_4);
__EXPORT పూర్ణాంకం F_write_మాట(గుర్తులేని దీర్ఘ చిరునామా_2, uint32_t దత్తాంశం_4);

__END_DECLS

#అయితే_ముగింపు
