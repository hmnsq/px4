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

#અન્તર્ભાવ "bl_update_flash.h"

#અન્તર્ભાવ <px4_platform_common/px4_config.h>

#અન્તર્ભાવ <stdint.h>
#અન્તર્ભાવ <stdlib.h>
#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <unistd.h>
#અન્તર્ભાવ <fcntl.h>
#અન્તર્ભાવ <sched.h>
#અન્તર્ભાવ <sys/stat.h>

#અન્તર્ભાવ <arch/board/board.h>

#અન્તર્ભાવ <nuttx/progmem.h>

#જો defined(BL_UPDATE_WATCHDOG)
#  include <drivers/drv_watchdog.h>
#નહીંતર
#  define watchdog_pet()
#endif

#જો defined(CONFIG_ARCH_CHIP_STM32H7)
#  define BL_FILE_SIZE_LIMIT	128*1024
#  define STM_RAM_BASE        STM32_AXISRAM_BASE
#  define PAGE_SIZE_MATTERS   1
#અથવાજો defined(CONFIG_ARCH_CHIP_STM32F7)
#  define BL_FILE_SIZE_LIMIT	32*1024
#  define STM_RAM_BASE        STM32_SRAM_BASE
#નહીંતર
#  define BL_FILE_SIZE_LIMIT  16384
#  define STM_RAM_BASE        STM32_SRAM_BASE
#endif

નામાકાશ bl_update
{

#જો defined (CONFIG_STM32_STM32F4XXX) || defined (CONFIG_ARCH_CHIP_STM32F7) || \
    defined (CONFIG_ARCH_CHIP_STM32H7)

સ્થિર Result write_flash(અચલ uint8_t *buf, size_t image_size)
{
	uint8_t *base = (uint8_t *) PX4_FLASH_BASE;

	જો (memcmp(base, buf, image_size) == 0) {
		પાછા Result::Unchanged;
	}

	/* prevent other tasks from running while we do this */
	sched_lock();

	/* The IWDG may already be running (CAN nodes start it in the bootloader)
	 * and nothing else gets to pet it while the scheduler is locked. */
	watchdog_pet();
	અચલ તાર્કિક erased = up_progmem_eraseblock(0) >= 0;
	watchdog_pet();

	/* Program even if the erase reported an error: the old bootloader is
	 * already damaged, and a failed program reports it below. */
	અચલ ssize_t size = up_progmem_write((size_t) base, buf, image_size);
	watchdog_pet();

	/* re-lock the flash control register */
	stm32_flash_lock();

	sched_unlock();

	જો (size != (ssize_t) image_size) {
		પાછા erased ? Result::ProgramFailed : Result::EraseFailed;
	}

	જો (memcmp(base, buf, image_size) != 0) {
		પાછા Result::VerifyFailed;
	}

	પાછા Result::Updated;
}

Result flash(અચલ અક્ષર *path)
{
	પૂર્ણાંક fd = open(path, O_RDONLY);

	જો (fd < 0) {
		પાછા Result::OpenFailed;
	}

	સંરચના stat s;

	જો (fstat(fd, &s) != 0) {
		close(fd);
		પાછા Result::OpenFailed;
	}

	જો (s.st_size > BL_FILE_SIZE_LIMIT) {
		close(fd);
		પાછા Result::TooLarge;
	}

	અચલ size_t file_size = s.st_size;

	/* up_progmem_write() rejects a size that is not a multiple of its
	 * program unit, and by then the old bootloader is erased. */
#જો defined(PAGE_SIZE_MATTERS)
	અચલ size_t align_mask = up_progmem_pagesize(0) - 1;
#નહીંતર
	અચલ size_t align_mask = કદ(uint32_t) - 1;
#endif
	અચલ size_t image_size = (file_size + align_mask) & ~align_mask;

	uint8_t *buf = (uint8_t *)malloc(image_size);

	જો (buf == nullptr) {
		close(fd);
		પાછા Result::NoMemory;
	}

	memset(buf, 0xff, image_size);

	અચલ ssize_t bytes_read = પઠ(fd, buf, file_size);
	close(fd);

	જો (bytes_read != (ssize_t) file_size) {
		મુક્ત(buf);
		પાછા Result::ReadFailed;
	}

	અચલ uint32_t *hdr = (અચલ uint32_t *)buf;

	જો ((hdr[0] < STM_RAM_BASE) ||			/* stack not below RAM */
	    (hdr[0] > (STM_RAM_BASE + (128 * 1024))) ||	/* stack not above RAM */
	    (hdr[1] < PX4_FLASH_BASE) ||			/* entrypoint not below flash */
	    ((hdr[1] - PX4_FLASH_BASE) > BL_FILE_SIZE_LIMIT)) {	/* entrypoint not outside bootloader */
		મુક્ત(buf);
		પાછા Result::InvalidImage;
	}

	અચલ Result result = write_flash(buf, image_size);
	મુક્ત(buf);
	પાછા result;
}

#નહીંતર

Result flash(અચલ અક્ષર *path)
{
	પાછા Result::NotSupported;
}

#endif

અચલ અક્ષર *result_str(Result result)
{
	પસંદગી (result) {
	સ્થિતિ Result::Updated:
		પાછા "bootloader updated";

	સ્થિતિ Result::Unchanged:
		પાછા "bootloader unchanged";

	સ્થિતિ Result::NotSupported:
		પાછા "not supported on this hardware";

	સ્થિતિ Result::OpenFailed:
		પાછા "cannot open bootloader file";

	સ્થિતિ Result::TooLarge:
		પાછા "bootloader file too large";

	સ્થિતિ Result::NoMemory:
		પાછા "out of memory";

	સ્થિતિ Result::ReadFailed:
		પાછા "bootloader file read error";

	સ્થિતિ Result::InvalidImage:
		પાછા "not a bootloader image";

	સ્થિતિ Result::EraseFailed:
		પાછા "flash erase failed, retry before rebooting";

	સ્થિતિ Result::ProgramFailed:
		પાછા "flash program failed, retry before rebooting";

	સ્થિતિ Result::VerifyFailed:
		પાછા "verify failed, retry before rebooting";
	}

	પાછા "unknown error";
}

} // namespace bl_update
