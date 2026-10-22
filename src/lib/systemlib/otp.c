/****************************************************************************
 *
 *   Copyright (C) 2012-2013 PX4 Development Team. All rights reserved.
 *   Authors:
 *      Lorenz Meier <lm@inf.ethz.ch>
 *      David "Buzz" Bussenschutt <davidbuzz@gmail.com>
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
 * @file otp.c
 * otp estimation
 *
 * @author Lorenz Meier <lm@inf.ethz.ch>
 * @author David "Buzz" Bussenschutt <davidbuzz@gmail.com>
 *
 */

#अन्तर्भाव <px4_platform_common/px4_config.h>
#अन्तर्भाव <board_config.h>
#अन्तर्भाव <stdio.h>
#अन्तर्भाव <math.h>
#अन्तर्भाव <unistd.h>
#अन्तर्भाव <string.h>  // memset
#अन्तर्भाव "otp.h"
#अन्तर्भाव "err.h"   // warnx
#अन्तर्भाव <assert.h>

पूर्णाङ्क val_read(शून्य *dest, चञ्चल ध्रुव शून्य *src, पूर्णाङ्क bytes)
{

	पूर्णाङ्क i;

	प्रति (i = 0; i < bytes / 4; i++) {
		*(((चञ्चल अचिन्हित *)dest) + i) = *(((चञ्चल अचिन्हित *)src) + i);
	}

	प्रत्यागम i * 4;
}


पूर्णाङ्क write_otp(uint8_t id_type, uint32_t vid, uint32_t pid, वर्ण *signature)
{

	warnx("write_otp: PX4 / %02X / %02lX / %02lX  / ... etc  \n", id_type, (अचिन्हित दीर्घ)vid, (अचिन्हित दीर्घ)pid);

	पूर्णाङ्क errors = 0;

	// descriptor
	यदि (F_write_byte(ADDR_OTP_START, 'P')) {
		errors++;
	}

	//  write the 'P' from PX4. to first byte in OTP
	यदि (F_write_byte(ADDR_OTP_START + 1, 'X')) {
		errors++;        //  write the 'P' from PX4. to first byte in OTP
	}

	यदि (F_write_byte(ADDR_OTP_START + 2, '4')) {
		errors++;
	}

	यदि (F_write_byte(ADDR_OTP_START + 3, '\0')) {
		errors++;
	}

	//id_type
	यदि (F_write_byte(ADDR_OTP_START + 4, id_type)) {
		errors++;
	}

	// vid and pid are 4 bytes each
	यदि (F_write_word(ADDR_OTP_START + 5, vid)) {
		errors++;
	}

	यदि (F_write_word(ADDR_OTP_START + 9, pid)) {
		errors++;
	}

	// leave some 19 bytes of space, and go to the next block...
	// then the auth sig starts
	प्रति (पूर्णाङ्क i = 0 ; i < 128 ; i++) {
		यदि (F_write_byte(ADDR_OTP_START + 32 + i, signature[i])) {
			errors++;
		}
	}

	प्रत्यागम errors;
}

पूर्णाङ्क lock_otp(शून्य)
{
	//determine the required locking size - can only write full lock bytes */
//	int size = sizeof(struct otp) / 32;
//
//	struct otp_lock otp_lock_mem;
//
//	memset(&otp_lock_mem, OTP_LOCK_UNLOCKED, sizeof(otp_lock_mem));
//	for (int i = 0; i < sizeof(otp_lock_mem) / sizeof(otp_lock_mem.lock_bytes[0]); i++)
//		otp_lock_mem.lock_bytes[i] = OTP_LOCK_LOCKED;
	//XXX add the actual call here to write the OTP_LOCK bytes only at final stage
	// val_copy(lock_ptr, &otp_lock_mem, sizeof(otp_lock_mem));

	पूर्णाङ्क locksize = 5;

	पूर्णाङ्क errors = 0;

	// or just realise it's exctly 5x 32byte blocks we need to lock.  1 block for ID,type,vid,pid, and 4 blocks for certificate, which is 128 bytes.
	प्रति (पूर्णाङ्क i = 0 ; i < locksize ; i++) {
		यदि (F_write_byte(ADDR_OTP_LOCK_START + i, OTP_LOCK_LOCKED)) {
			errors++;
		}
	}

	प्रत्यागम errors;
}

// COMPLETE, BUSY, or other flash error?
स्थिर पूर्णाङ्क F_GetStatus(शून्य)
{
	पूर्णाङ्क fs = F_COMPLETE;

	यदि ((FLASH->status & F_BSY) == F_BSY) { fs = F_BUSY; } अन्यथा {

		यदि ((FLASH->status & F_WRPERR) != (uint32_t)0x00) { fs = F_ERROR_WRP; } अन्यथा {

			यदि ((FLASH->status & (uint32_t)0xEF) != (uint32_t)0x00) { fs = F_ERROR_PROGRAM; } अन्यथा {

				यदि ((FLASH->status & F_OPERR) != (uint32_t)0x00) { fs = F_ERROR_OPERATION; } अन्यथा {
					fs = F_COMPLETE;
				}
			}
		}
	}

	प्रत्यागम fs;
}


// enable FLASH Registers
शून्य F_unlock(शून्य)
{
	यदि ((FLASH->control & F_CR_LOCK) != 0) {
		FLASH->कुंजी = F_KEY1;
		FLASH->कुंजी = F_KEY2;
	}
}

// lock the FLASH Registers
शून्य F_lock(शून्य)
{
	FLASH->control |= F_CR_LOCK;
}

// flash write word.
पूर्णाङ्क F_write_word(अचिन्हित दीर्घ Address, uint32_t Data)
{
	अचिन्हित वर्ण octet[4] = {0, 0, 0, 0};

	पूर्णाङ्क ret = 0;

	प्रति (पूर्णाङ्क i = 0; i < 4; i++) {
		octet[i] = (Data >> (i * 8)) & 0xFF;
		ret = F_write_byte(Address + i, octet[i]);
	}

	प्रत्यागम ret;
}

// flash write byte
पूर्णाङ्क F_write_byte(अचिन्हित दीर्घ Address, uint8_t Data)
{
	चञ्चल पूर्णाङ्क status = F_COMPLETE;

	//warnx("F_write_byte: %08X %02d", Address , Data ) ;

	//Check the parameters
	प्रतिज्ञा(IS_F_ADDRESS(Address));

	//Wait for FLASH operation to complete by polling on BUSY flag.
	status = F_GetStatus();

	यावत् (status == F_BUSY) { status = F_GetStatus();}

	यदि (status == F_COMPLETE) {
		//if the previous operation is completed, proceed to program the new data
		FLASH->control &= CR_PSIZE_MASK;
		FLASH->control |= F_PSIZE_BYTE;
		FLASH->control |= F_CR_PG;

		*(चञ्चल uint8_t *)Address = Data;

		//Wait for FLASH operation to complete by polling on BUSY flag.
		status = F_GetStatus();

		यावत् (status == F_BUSY) { status = F_GetStatus();}

		//if the program operation is completed, disable the PG Bit
		FLASH->control &= (~F_CR_PG);
	}

	//Return the Program Status
	प्रत्यागम !(status == F_COMPLETE);
}
