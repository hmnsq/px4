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

#شامل "bl_update_flash.h"

#شامل <px4_platform_common/px4_config.h>

#شامل <stdint.h>
#شامل <stdlib.h>
#شامل <string.h>
#شامل <unistd.h>
#شامل <fcntl.h>
#شامل <sched.h>
#شامل <sys/stat.h>

#شامل <arch/board/board.h>

#شامل <nuttx/progmem.h>

#اگر defined(BL_UPDATE_WATCHDOG)
#  include <drivers/drv_watchdog.h>
#وگرنه
#  define watchdog_pet()
#endif

#اگر defined(CONFIG_ARCH_CHIP_STM32H7)
#  define BL_FILE_SIZE_LIMIT	128*1024
#  define STM_RAM_BASE        STM32_AXISRAM_BASE
#  define PAGE_SIZE_MATTERS   1
#وگرنهاگر defined(CONFIG_ARCH_CHIP_STM32F7)
#  define BL_FILE_SIZE_LIMIT	32*1024
#  define STM_RAM_BASE        STM32_SRAM_BASE
#وگرنه
#  define BL_FILE_SIZE_LIMIT  16384
#  define STM_RAM_BASE        STM32_SRAM_BASE
#endif

فضاینام bl_update
{

#اگر defined (CONFIG_STM32_STM32F4XXX) || defined (CONFIG_ARCH_CHIP_STM32F7) || \
    defined (CONFIG_ARCH_CHIP_STM32H7)

ایستا Result write_flash(ثابت uint8_t *buf, size_t image_size)
{
	uint8_t *base = (uint8_t *) PX4_FLASH_BASE;

	اگر (memcmp(base, buf, image_size) == 0) {
		بازگشت Result::Unchanged;
	}

	/* prevent other tasks from running while we do this */
	sched_lock();

	/* The IWDG may already be running (CAN nodes start it in the bootloader)
	 * and nothing else gets to pet it while the scheduler is locked. */
	watchdog_pet();
	ثابت منطقی erased = up_progmem_eraseblock(0) >= 0;
	watchdog_pet();

	/* Program even if the erase reported an error: the old bootloader is
	 * already damaged, and a failed program reports it below. */
	ثابت ssize_t size = up_progmem_write((size_t) base, buf, image_size);
	watchdog_pet();

	/* re-lock the flash control register */
	stm32_flash_lock();

	sched_unlock();

	اگر (size != (ssize_t) image_size) {
		بازگشت erased ? Result::ProgramFailed : Result::EraseFailed;
	}

	اگر (memcmp(base, buf, image_size) != 0) {
		بازگشت Result::VerifyFailed;
	}

	بازگشت Result::Updated;
}

Result flash(ثابت نویسه *path)
{
	صحیح fd = open(path, O_RDONLY);

	اگر (fd < 0) {
		بازگشت Result::OpenFailed;
	}

	ساختار stat s;

	اگر (fstat(fd, &s) != 0) {
		close(fd);
		بازگشت Result::OpenFailed;
	}

	اگر (s.st_size > BL_FILE_SIZE_LIMIT) {
		close(fd);
		بازگشت Result::TooLarge;
	}

	ثابت size_t file_size = s.st_size;

	/* up_progmem_write() rejects a size that is not a multiple of its
	 * program unit, and by then the old bootloader is erased. */
#اگر defined(PAGE_SIZE_MATTERS)
	ثابت size_t align_mask = up_progmem_pagesize(0) - 1;
#وگرنه
	ثابت size_t align_mask = اندازه(uint32_t) - 1;
#endif
	ثابت size_t image_size = (file_size + align_mask) & ~align_mask;

	uint8_t *buf = (uint8_t *)malloc(image_size);

	اگر (buf == nullptr) {
		close(fd);
		بازگشت Result::NoMemory;
	}

	memset(buf, 0xff, image_size);

	ثابت ssize_t bytes_read = اقرأ(fd, buf, file_size);
	close(fd);

	اگر (bytes_read != (ssize_t) file_size) {
		آزاد(buf);
		بازگشت Result::ReadFailed;
	}

	ثابت uint32_t *hdr = (ثابت uint32_t *)buf;

	اگر ((hdr[0] < STM_RAM_BASE) ||			/* stack not below RAM */
	    (hdr[0] > (STM_RAM_BASE + (128 * 1024))) ||	/* stack not above RAM */
	    (hdr[1] < PX4_FLASH_BASE) ||			/* entrypoint not below flash */
	    ((hdr[1] - PX4_FLASH_BASE) > BL_FILE_SIZE_LIMIT)) {	/* entrypoint not outside bootloader */
		آزاد(buf);
		بازگشت Result::InvalidImage;
	}

	ثابت Result result = write_flash(buf, image_size);
	آزاد(buf);
	بازگشت result;
}

#وگرنه

Result flash(ثابت نویسه *path)
{
	بازگشت Result::NotSupported;
}

#endif

ثابت نویسه *result_str(Result result)
{
	انتخاب (result) {
	حالت Result::Updated:
		بازگشت "bootloader updated";

	حالت Result::Unchanged:
		بازگشت "bootloader unchanged";

	حالت Result::NotSupported:
		بازگشت "not supported on this hardware";

	حالت Result::OpenFailed:
		بازگشت "cannot open bootloader file";

	حالت Result::TooLarge:
		بازگشت "bootloader file too large";

	حالت Result::NoMemory:
		بازگشت "out of memory";

	حالت Result::ReadFailed:
		بازگشت "bootloader file read error";

	حالت Result::InvalidImage:
		بازگشت "not a bootloader image";

	حالت Result::EraseFailed:
		بازگشت "flash erase failed, retry before rebooting";

	حالت Result::ProgramFailed:
		بازگشت "flash program failed, retry before rebooting";

	حالت Result::VerifyFailed:
		بازگشت "verify failed, retry before rebooting";
	}

	بازگشت "unknown error";
}

} // namespace bl_update
