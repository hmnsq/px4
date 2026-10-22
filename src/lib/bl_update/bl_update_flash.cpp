/****************************************************************************
 *
 *   Copyright (c) 2012-2026 PX4 Development Team. All rights reserved.
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

#அந்தர்பாவ "bl_update_flash.h"

#அந்தர்பாவ <px4_platform_common/px4_config.h>

#அந்தர்பாவ <stdint.h>
#அந்தர்பாவ <stdlib.h>
#அந்தர்பாவ <string.h>
#அந்தர்பாவ <unistd.h>
#அந்தர்பாவ <fcntl.h>
#அந்தர்பாவ <sched.h>
#அந்தர்பாவ <sys/stat.h>

#அந்தர்பாவ <arch/board/board.h>

#அந்தர்பாவ <nuttx/progmem.h>

#எனில் defined(BL_UPDATE_WATCHDOG)
#  include <drivers/drv_watchdog.h>
#இல்லையெனில்
#  define watchdog_pet()
#endif

#எனில் defined(CONFIG_ARCH_CHIP_STM32H7)
#  define BL_FILE_SIZE_LIMIT	128*1024
#  define STM_RAM_BASE        STM32_AXISRAM_BASE
#  define PAGE_SIZE_MATTERS   1
#அல்லதெனில் defined(CONFIG_ARCH_CHIP_STM32F7)
#  define BL_FILE_SIZE_LIMIT	32*1024
#  define STM_RAM_BASE        STM32_SRAM_BASE
#இல்லையெனில்
#  define BL_FILE_SIZE_LIMIT  16384
#  define STM_RAM_BASE        STM32_SRAM_BASE
#endif

நாமாகாஶ bl_update
{

#எனில் defined (CONFIG_STM32_STM32F4XXX) || defined (CONFIG_ARCH_CHIP_STM32F7) || \
    defined (CONFIG_ARCH_CHIP_STM32H7)

நிலையான Result write_flash(மாறிலி uint8_t *buf, size_t image_size)
{
	uint8_t *base = (uint8_t *) PX4_FLASH_BASE;

	எனில் (memcmp(base, buf, image_size) == 0) {
		திரும்பு Result::Unchanged;
	}

	/* prevent other tasks from running while we do this */
	sched_lock();

	/* The IWDG may already be running (CAN nodes start it in the bootloader)
	 * and nothing else gets to pet it while the scheduler is locked. */
	watchdog_pet();
	மாறிலி தருக்க erased = up_progmem_eraseblock(0) >= 0;
	watchdog_pet();

	/* Program even if the erase reported an error: the old bootloader is
	 * already damaged, and a failed program reports it below. */
	மாறிலி ssize_t size = up_progmem_write((size_t) base, buf, image_size);
	watchdog_pet();

	/* re-lock the flash control register */
	stm32_flash_lock();

	sched_unlock();

	எனில் (size != (ssize_t) image_size) {
		திரும்பு erased ? Result::ProgramFailed : Result::EraseFailed;
	}

	எனில் (memcmp(base, buf, image_size) != 0) {
		திரும்பு Result::VerifyFailed;
	}

	திரும்பு Result::Updated;
}

Result flash(மாறிலி எழுத்து *path)
{
	முழு fd = open(path, O_RDONLY);

	எனில் (fd < 0) {
		திரும்பு Result::OpenFailed;
	}

	கட்டமைப்பு stat s;

	எனில் (fstat(fd, &s) != 0) {
		close(fd);
		திரும்பு Result::OpenFailed;
	}

	எனில் (s.st_size > BL_FILE_SIZE_LIMIT) {
		close(fd);
		திரும்பு Result::TooLarge;
	}

	மாறிலி size_t file_size = s.st_size;

	/* up_progmem_write() rejects a size that is not a multiple of its
	 * program unit, and by then the old bootloader is erased. */
#எனில் defined(PAGE_SIZE_MATTERS)
	மாறிலி size_t align_mask = up_progmem_pagesize(0) - 1;
#இல்லையெனில்
	மாறிலி size_t align_mask = அளவு(uint32_t) - 1;
#endif
	மாறிலி size_t image_size = (file_size + align_mask) & ~align_mask;

	uint8_t *buf = (uint8_t *)malloc(image_size);

	எனில் (buf == nullptr) {
		close(fd);
		திரும்பு Result::NoMemory;
	}

	memset(buf, 0xff, image_size);

	மாறிலி ssize_t bytes_read = பட(fd, buf, file_size);
	close(fd);

	எனில் (bytes_read != (ssize_t) file_size) {
		விடுவி(buf);
		திரும்பு Result::ReadFailed;
	}

	மாறிலி uint32_t *hdr = (மாறிலி uint32_t *)buf;

	எனில் ((hdr[0] < STM_RAM_BASE) ||			/* stack not below RAM */
	    (hdr[0] > (STM_RAM_BASE + (128 * 1024))) ||	/* stack not above RAM */
	    (hdr[1] < PX4_FLASH_BASE) ||			/* entrypoint not below flash */
	    ((hdr[1] - PX4_FLASH_BASE) > BL_FILE_SIZE_LIMIT)) {	/* entrypoint not outside bootloader */
		விடுவி(buf);
		திரும்பு Result::InvalidImage;
	}

	மாறிலி Result result = write_flash(buf, image_size);
	விடுவி(buf);
	திரும்பு result;
}

#இல்லையெனில்

Result flash(மாறிலி எழுத்து *path)
{
	திரும்பு Result::NotSupported;
}

#endif

மாறிலி எழுத்து *result_str(Result result)
{
	தேர்வு (result) {
	நிலை Result::Updated:
		திரும்பு "bootloader updated";

	நிலை Result::Unchanged:
		திரும்பு "bootloader unchanged";

	நிலை Result::NotSupported:
		திரும்பு "not supported on this hardware";

	நிலை Result::OpenFailed:
		திரும்பு "cannot open bootloader file";

	நிலை Result::TooLarge:
		திரும்பு "bootloader file too large";

	நிலை Result::NoMemory:
		திரும்பு "out of memory";

	நிலை Result::ReadFailed:
		திரும்பு "bootloader file read error";

	நிலை Result::InvalidImage:
		திரும்பு "not a bootloader image";

	நிலை Result::EraseFailed:
		திரும்பு "flash erase failed, retry before rebooting";

	நிலை Result::ProgramFailed:
		திரும்பு "flash program failed, retry before rebooting";

	நிலை Result::VerifyFailed:
		திரும்பு "verify failed, retry before rebooting";
	}

	திரும்பு "unknown error";
}

} // namespace bl_update
