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

#അന്തര്ഭാവ "bl_update_flash.h"

#അന്തര്ഭാവ <px4_platform_common/px4_config.h>

#അന്തര്ഭാവ <stdint.h>
#അന്തര്ഭാവ <stdlib.h>
#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <unistd.h>
#അന്തര്ഭാവ <fcntl.h>
#അന്തര്ഭാവ <sched.h>
#അന്തര്ഭാവ <sys/stat.h>

#അന്തര്ഭാവ <arch/board/board.h>

#അന്തര്ഭാവ <nuttx/progmem.h>

#എങ്കിൽ defined(BL_UPDATE_WATCHDOG)
#  include <drivers/drv_watchdog.h>
#അല്ലെങ്കിൽ
#  define watchdog_pet()
#endif

#എങ്കിൽ defined(CONFIG_ARCH_CHIP_STM32H7)
#  define BL_FILE_SIZE_LIMIT	128*1024
#  define STM_RAM_BASE        STM32_AXISRAM_BASE
#  define PAGE_SIZE_MATTERS   1
#അല്ലെങ്കിലെങ്കിൽ defined(CONFIG_ARCH_CHIP_STM32F7)
#  define BL_FILE_SIZE_LIMIT	32*1024
#  define STM_RAM_BASE        STM32_SRAM_BASE
#അല്ലെങ്കിൽ
#  define BL_FILE_SIZE_LIMIT  16384
#  define STM_RAM_BASE        STM32_SRAM_BASE
#endif

നാമാകാശ bl_update
{

#എങ്കിൽ defined (CONFIG_STM32_STM32F4XXX) || defined (CONFIG_ARCH_CHIP_STM32F7) || \
    defined (CONFIG_ARCH_CHIP_STM32H7)

സ്ഥിതം Result write_flash(സ്ഥിരം uint8_t *buf, size_t image_size)
{
	uint8_t *base = (uint8_t *) PX4_FLASH_BASE;

	എങ്കിൽ (memcmp(base, buf, image_size) == 0) {
		തിരികെ Result::Unchanged;
	}

	/* prevent other tasks from running while we do this */
	sched_lock();

	/* The IWDG may already be running (CAN nodes start it in the bootloader)
	 * and nothing else gets to pet it while the scheduler is locked. */
	watchdog_pet();
	സ്ഥിരം തർക്കം erased = up_progmem_eraseblock(0) >= 0;
	watchdog_pet();

	/* Program even if the erase reported an error: the old bootloader is
	 * already damaged, and a failed program reports it below. */
	സ്ഥിരം ssize_t size = up_progmem_write((size_t) base, buf, image_size);
	watchdog_pet();

	/* re-lock the flash control register */
	stm32_flash_lock();

	sched_unlock();

	എങ്കിൽ (size != (ssize_t) image_size) {
		തിരികെ erased ? Result::ProgramFailed : Result::EraseFailed;
	}

	എങ്കിൽ (memcmp(base, buf, image_size) != 0) {
		തിരികെ Result::VerifyFailed;
	}

	തിരികെ Result::Updated;
}

Result flash(സ്ഥിരം അക്ഷരം *path)
{
	പൂർണ്ണസംഖ്യ fd = open(path, O_RDONLY);

	എങ്കിൽ (fd < 0) {
		തിരികെ Result::OpenFailed;
	}

	ഘടന stat s;

	എങ്കിൽ (fstat(fd, &s) != 0) {
		close(fd);
		തിരികെ Result::OpenFailed;
	}

	എങ്കിൽ (s.st_size > BL_FILE_SIZE_LIMIT) {
		close(fd);
		തിരികെ Result::TooLarge;
	}

	സ്ഥിരം size_t file_size = s.st_size;

	/* up_progmem_write() rejects a size that is not a multiple of its
	 * program unit, and by then the old bootloader is erased. */
#എങ്കിൽ defined(PAGE_SIZE_MATTERS)
	സ്ഥിരം size_t align_mask = up_progmem_pagesize(0) - 1;
#അല്ലെങ്കിൽ
	സ്ഥിരം size_t align_mask = വലുപ്പം(uint32_t) - 1;
#endif
	സ്ഥിരം size_t image_size = (file_size + align_mask) & ~align_mask;

	uint8_t *buf = (uint8_t *)malloc(image_size);

	എങ്കിൽ (buf == nullptr) {
		close(fd);
		തിരികെ Result::NoMemory;
	}

	memset(buf, 0xff, image_size);

	സ്ഥിരം ssize_t bytes_read = പഠ(fd, buf, file_size);
	close(fd);

	എങ്കിൽ (bytes_read != (ssize_t) file_size) {
		സ്വതന്ത്രം(buf);
		തിരികെ Result::ReadFailed;
	}

	സ്ഥിരം uint32_t *hdr = (സ്ഥിരം uint32_t *)buf;

	എങ്കിൽ ((hdr[0] < STM_RAM_BASE) ||			/* stack not below RAM */
	    (hdr[0] > (STM_RAM_BASE + (128 * 1024))) ||	/* stack not above RAM */
	    (hdr[1] < PX4_FLASH_BASE) ||			/* entrypoint not below flash */
	    ((hdr[1] - PX4_FLASH_BASE) > BL_FILE_SIZE_LIMIT)) {	/* entrypoint not outside bootloader */
		സ്വതന്ത്രം(buf);
		തിരികെ Result::InvalidImage;
	}

	സ്ഥിരം Result result = write_flash(buf, image_size);
	സ്വതന്ത്രം(buf);
	തിരികെ result;
}

#അല്ലെങ്കിൽ

Result flash(സ്ഥിരം അക്ഷരം *path)
{
	തിരികെ Result::NotSupported;
}

#endif

സ്ഥിരം അക്ഷരം *result_str(Result result)
{
	തിരഞ്ഞെടുപ്പ് (result) {
	അവസ്ഥ Result::Updated:
		തിരികെ "bootloader updated";

	അവസ്ഥ Result::Unchanged:
		തിരികെ "bootloader unchanged";

	അവസ്ഥ Result::NotSupported:
		തിരികെ "not supported on this hardware";

	അവസ്ഥ Result::OpenFailed:
		തിരികെ "cannot open bootloader file";

	അവസ്ഥ Result::TooLarge:
		തിരികെ "bootloader file too large";

	അവസ്ഥ Result::NoMemory:
		തിരികെ "out of memory";

	അവസ്ഥ Result::ReadFailed:
		തിരികെ "bootloader file read error";

	അവസ്ഥ Result::InvalidImage:
		തിരികെ "not a bootloader image";

	അവസ്ഥ Result::EraseFailed:
		തിരികെ "flash erase failed, retry before rebooting";

	അവസ്ഥ Result::ProgramFailed:
		തിരികെ "flash program failed, retry before rebooting";

	അവസ്ഥ Result::VerifyFailed:
		തിരികെ "verify failed, retry before rebooting";
	}

	തിരികെ "unknown error";
}

} // namespace bl_update
