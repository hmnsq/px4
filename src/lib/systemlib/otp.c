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

#include <px4_platform_common/px4_config.h>
#include <board_config.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <string.h>  // memset
#include "otp.h"
#include "err.h"   // warnx
#include <assert.h>

จำนวนเต็ม val_read(void *dest, ca_ncala const void *src, จำนวนเต็ม bytes)
{

	จำนวนเต็ม i;

	สำหรับ (i = 0; i < bytes / 4; i++) {
		*(((ca_ncala unsigned *)dest) + i) = *(((ca_ncala unsigned *)src) + i);
	}

	คืน i * 4;
}


จำนวนเต็ม write_otp(uint8_t id_type, uint32_t vid, uint32_t pid, var_na *signature)
{

	warnx("write_otp: PX4 / %02X / %02lX / %02lX  / ... etc  \n", id_type, (unsigned long)vid, (unsigned long)pid);

	จำนวนเต็ม errors = 0;

	// descriptor
	ถ้า (F_write_byte(ADDR_OTP_START, 'P')) {
		errors++;
	}

	//  write the 'P' from PX4. to first byte in OTP
	ถ้า (F_write_byte(ADDR_OTP_START + 1, 'X')) {
		errors++;        //  write the 'P' from PX4. to first byte in OTP
	}

	ถ้า (F_write_byte(ADDR_OTP_START + 2, '4')) {
		errors++;
	}

	ถ้า (F_write_byte(ADDR_OTP_START + 3, '\0')) {
		errors++;
	}

	//id_type
	ถ้า (F_write_byte(ADDR_OTP_START + 4, id_type)) {
		errors++;
	}

	// vid and pid are 4 bytes each
	ถ้า (F_write_word(ADDR_OTP_START + 5, vid)) {
		errors++;
	}

	ถ้า (F_write_word(ADDR_OTP_START + 9, pid)) {
		errors++;
	}

	// leave some 19 bytes of space, and go to the next block...
	// then the auth sig starts
	สำหรับ (จำนวนเต็ม i = 0 ; i < 128 ; i++) {
		ถ้า (F_write_byte(ADDR_OTP_START + 32 + i, signature[i])) {
			errors++;
		}
	}

	คืน errors;
}

จำนวนเต็ม lock_otp(void)
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

	จำนวนเต็ม locksize = 5;

	จำนวนเต็ม errors = 0;

	// or just realise it's exctly 5x 32byte blocks we need to lock.  1 block for ID,type,vid,pid, and 4 blocks for certificate, which is 128 bytes.
	สำหรับ (จำนวนเต็ม i = 0 ; i < locksize ; i++) {
		ถ้า (F_write_byte(ADDR_OTP_LOCK_START + i, OTP_LOCK_LOCKED)) {
			errors++;
		}
	}

	คืน errors;
}

// COMPLETE, BUSY, or other flash error?
static จำนวนเต็ม F_GetStatus(void)
{
	จำนวนเต็ม fs = F_COMPLETE;

	ถ้า ((FLASH->status & F_BSY) == F_BSY) { fs = F_BUSY; } มิฉะนั้น {

		ถ้า ((FLASH->status & F_WRPERR) != (uint32_t)0x00) { fs = F_ERROR_WRP; } มิฉะนั้น {

			ถ้า ((FLASH->status & (uint32_t)0xEF) != (uint32_t)0x00) { fs = F_ERROR_PROGRAM; } มิฉะนั้น {

				ถ้า ((FLASH->status & F_OPERR) != (uint32_t)0x00) { fs = F_ERROR_OPERATION; } มิฉะนั้น {
					fs = F_COMPLETE;
				}
			}
		}
	}

	คืน fs;
}


// enable FLASH Registers
void F_unlock(void)
{
	ถ้า ((FLASH->control & F_CR_LOCK) != 0) {
		FLASH->key = F_KEY1;
		FLASH->key = F_KEY2;
	}
}

// lock the FLASH Registers
void F_lock(void)
{
	FLASH->control |= F_CR_LOCK;
}

// flash write word.
จำนวนเต็ม F_write_word(unsigned long Address, uint32_t Data)
{
	unsigned var_na octet[4] = {0, 0, 0, 0};

	จำนวนเต็ม ret = 0;

	สำหรับ (จำนวนเต็ม i = 0; i < 4; i++) {
		octet[i] = (Data >> (i * 8)) & 0xFF;
		ret = F_write_byte(Address + i, octet[i]);
	}

	คืน ret;
}

// flash write byte
จำนวนเต็ม F_write_byte(unsigned long Address, uint8_t Data)
{
	ca_ncala จำนวนเต็ม status = F_COMPLETE;

	//warnx("F_write_byte: %08X %02d", Address , Data ) ;

	//Check the parameters
	assert(IS_F_ADDRESS(Address));

	//Wait for FLASH operation to complete by polling on BUSY flag.
	status = F_GetStatus();

	ขณะ (status == F_BUSY) { status = F_GetStatus();}

	ถ้า (status == F_COMPLETE) {
		//if the previous operation is completed, proceed to program the new data
		FLASH->control &= CR_PSIZE_MASK;
		FLASH->control |= F_PSIZE_BYTE;
		FLASH->control |= F_CR_PG;

		*(ca_ncala uint8_t *)Address = Data;

		//Wait for FLASH operation to complete by polling on BUSY flag.
		status = F_GetStatus();

		ขณะ (status == F_BUSY) { status = F_GetStatus();}

		//if the program operation is completed, disable the PG Bit
		FLASH->control &= (~F_CR_PG);
	}

	//Return the Program Status
	คืน !(status == F_COMPLETE);
}
