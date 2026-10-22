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

#ضمن "bl_update_flash.h"

#ضمن <px4_platform_common/px4_config.h>

#ضمن <stdint.h>
#ضمن <stdlib.h>
#ضمن <string.h>
#ضمن <unistd.h>
#ضمن <fcntl.h>
#ضمن <sched.h>
#ضمن <sys/stat.h>

#ضمن <arch/board/board.h>

#ضمن <nuttx/progmem.h>

#ܐܢ defined(BL_UPDATE_WATCHDOG)
#  include <drivers/drv_watchdog.h>
#ܐܚܪܢܐ
#  define watchdog_pet()
#endif

#ܐܢ defined(CONFIG_ARCH_CHIP_STM32H7)
#  define BL_FILE_SIZE_LIMIT	128*1024
#  define STM_RAM_BASE        STM32_AXISRAM_BASE
#  define PAGE_SIZE_MATTERS   1
#وإلاإذا defined(CONFIG_ARCH_CHIP_STM32F7)
#  define BL_FILE_SIZE_LIMIT	32*1024
#  define STM_RAM_BASE        STM32_SRAM_BASE
#ܐܚܪܢܐ
#  define BL_FILE_SIZE_LIMIT  16384
#  define STM_RAM_BASE        STM32_SRAM_BASE
#endif

نطاق bl_update
{

#ܐܢ defined (CONFIG_STM32_STM32F4XXX) || defined (CONFIG_ARCH_CHIP_STM32F7) || \
    defined (CONFIG_ARCH_CHIP_STM32H7)

ساكن Result write_flash(ثابت uint8_t *buf, size_t image_size)
{
	uint8_t *base = (uint8_t *) PX4_FLASH_BASE;

	ܐܢ (memcmp(base, buf, image_size) == 0) {
		ܦܢܐ Result::Unchanged;
	}

	/* prevent other tasks from running while we do this */
	sched_lock();

	/* The IWDG may already be running (CAN nodes start it in the bootloader)
	 * and nothing else gets to pet it while the scheduler is locked. */
	watchdog_pet();
	ثابت منطقي erased = up_progmem_eraseblock(0) >= 0;
	watchdog_pet();

	/* Program even if the erase reported an error: the old bootloader is
	 * already damaged, and a failed program reports it below. */
	ثابت ssize_t size = up_progmem_write((size_t) base, buf, image_size);
	watchdog_pet();

	/* re-lock the flash control register */
	stm32_flash_lock();

	sched_unlock();

	ܐܢ (size != (ssize_t) image_size) {
		ܦܢܐ erased ? Result::ProgramFailed : Result::EraseFailed;
	}

	ܐܢ (memcmp(base, buf, image_size) != 0) {
		ܦܢܐ Result::VerifyFailed;
	}

	ܦܢܐ Result::Updated;
}

Result flash(ثابت حرف *path)
{
	صحيح fd = open(path, O_RDONLY);

	ܐܢ (fd < 0) {
		ܦܢܐ Result::OpenFailed;
	}

	بنية stat s;

	ܐܢ (fstat(fd, &s) != 0) {
		close(fd);
		ܦܢܐ Result::OpenFailed;
	}

	ܐܢ (s.st_size > BL_FILE_SIZE_LIMIT) {
		close(fd);
		ܦܢܐ Result::TooLarge;
	}

	ثابت size_t file_size = s.st_size;

	/* up_progmem_write() rejects a size that is not a multiple of its
	 * program unit, and by then the old bootloader is erased. */
#ܐܢ defined(PAGE_SIZE_MATTERS)
	ثابت size_t align_mask = up_progmem_pagesize(0) - 1;
#ܐܚܪܢܐ
	ثابت size_t align_mask = حجم(uint32_t) - 1;
#endif
	ثابت size_t image_size = (file_size + align_mask) & ~align_mask;

	uint8_t *buf = (uint8_t *)malloc(image_size);

	ܐܢ (buf == nullptr) {
		close(fd);
		ܦܢܐ Result::NoMemory;
	}

	memset(buf, 0xff, image_size);

	ثابت ssize_t bytes_read = اقرأ(fd, buf, file_size);
	close(fd);

	ܐܢ (bytes_read != (ssize_t) file_size) {
		حرر(buf);
		ܦܢܐ Result::ReadFailed;
	}

	ثابت uint32_t *hdr = (ثابت uint32_t *)buf;

	ܐܢ ((hdr[0] < STM_RAM_BASE) ||			/* stack not below RAM */
	    (hdr[0] > (STM_RAM_BASE + (128 * 1024))) ||	/* stack not above RAM */
	    (hdr[1] < PX4_FLASH_BASE) ||			/* entrypoint not below flash */
	    ((hdr[1] - PX4_FLASH_BASE) > BL_FILE_SIZE_LIMIT)) {	/* entrypoint not outside bootloader */
		حرر(buf);
		ܦܢܐ Result::InvalidImage;
	}

	ثابت Result result = write_flash(buf, image_size);
	حرر(buf);
	ܦܢܐ result;
}

#ܐܚܪܢܐ

Result flash(ثابت حرف *path)
{
	ܦܢܐ Result::NotSupported;
}

#endif

ثابت حرف *result_str(Result result)
{
	بدل (result) {
	حالة Result::Updated:
		ܦܢܐ "bootloader updated";

	حالة Result::Unchanged:
		ܦܢܐ "bootloader unchanged";

	حالة Result::NotSupported:
		ܦܢܐ "not supported on this hardware";

	حالة Result::OpenFailed:
		ܦܢܐ "cannot open bootloader file";

	حالة Result::TooLarge:
		ܦܢܐ "bootloader file too large";

	حالة Result::NoMemory:
		ܦܢܐ "out of memory";

	حالة Result::ReadFailed:
		ܦܢܐ "bootloader file read error";

	حالة Result::InvalidImage:
		ܦܢܐ "not a bootloader image";

	حالة Result::EraseFailed:
		ܦܢܐ "flash erase failed, retry before rebooting";

	حالة Result::ProgramFailed:
		ܦܢܐ "flash program failed, retry before rebooting";

	حالة Result::VerifyFailed:
		ܦܢܐ "verify failed, retry before rebooting";
	}

	ܦܢܐ "unknown error";
}

} // namespace bl_update
