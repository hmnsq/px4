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

#অন্তর্ভাব "bl_update_flash.h"

#অন্তর্ভাব <px4_platform_common/px4_config.h>

#অন্তর্ভাব <stdint.h>
#অন্তর্ভাব <stdlib.h>
#অন্তর্ভাব <string.h>
#অন্তর্ভাব <unistd.h>
#অন্তর্ভাব <fcntl.h>
#অন্তর্ভাব <sched.h>
#অন্তর্ভাব <sys/stat.h>

#অন্তর্ভাব <arch/board/board.h>

#অন্তর্ভাব <nuttx/progmem.h>

#যদি defined(BL_UPDATE_WATCHDOG)
#  include <drivers/drv_watchdog.h>
#অন্যথা
#  define watchdog_pet()
#endif

#যদি defined(CONFIG_ARCH_CHIP_STM32H7)
#  define BL_FILE_আকার_LIMIT	128*1024
#  define STM_RAM_BASE        STM32_AXISRAM_BASE
#  define PAGE_আকার_MATTERS   1
#নতুবা_যদি defined(CONFIG_ARCH_CHIP_STM32F7)
#  define BL_FILE_আকার_LIMIT	32*1024
#  define STM_RAM_BASE        STM32_SRAM_BASE
#অন্যথা
#  define BL_FILE_আকার_LIMIT  16384
#  define STM_RAM_BASE        STM32_SRAM_BASE
#endif

নামাকাশ bl_update
{

#যদি defined (CONFIG_STM32_STM32F4XXX) || defined (CONFIG_ARCH_CHIP_STM32F7) || \
    defined (CONFIG_ARCH_CHIP_STM32H7)

স্থির ফলাফল_2 write_flash(ধ্রুবক uint8_t *buf, size_t image_আকার)
{
	uint8_t *base = (uint8_t *) PX4_FLASH_BASE;

	যদি (memcmp(base, buf, image_আকার) == 0) {
		ফেরত ফলাফল_2::Unchanged;
	}

	/* prevent other tasks from running while we do this */
	sched_lock();

	/* The IWDG may already be running (CAN nodes start it in the bootloader)
	 * and nothing else gets to pet it while the scheduler is locked. */
	watchdog_pet();
	ধ্রুবক যৌক্তিক erased = up_progmem_eraseblock(0) >= 0;
	watchdog_pet();

	/* Program even if the erase reported an error: the old bootloader is
	 * already damaged, and a failed program reports it below. */
	ধ্রুবক ssize_t আকার_2 = up_progmem_write((size_t) base, buf, image_আকার);
	watchdog_pet();

	/* re-lock the flash control register */
	stm32_flash_lock();

	sched_unlock();

	যদি (আকার_2 != (ssize_t) image_আকার) {
		ফেরত erased ? ফলাফল_2::ProgramFailed : ফলাফল_2::EraseFailed;
	}

	যদি (memcmp(base, buf, image_আকার) != 0) {
		ফেরত ফলাফল_2::VerifyFailed;
	}

	ফেরত ফলাফল_2::Updated;
}

ফলাফল_2 flash(ধ্রুবক অক্ষর *path)
{
	পূর্ণসংখ্যা fd = open(path, O_RDONLY);

	যদি (fd < 0) {
		ফেরত ফলাফল_2::OpenFailed;
	}

	কাঠামো stat s;

	যদি (fstat(fd, &s) != 0) {
		close(fd);
		ফেরত ফলাফল_2::OpenFailed;
	}

	যদি (s.st_size > BL_FILE_আকার_LIMIT) {
		close(fd);
		ফেরত ফলাফল_2::TooLarge;
	}

	ধ্রুবক size_t file_আকার = s.st_size;

	/* up_progmem_write() rejects a size that is not a multiple of its
	 * program unit, and by then the old bootloader is erased. */
#যদি defined(PAGE_আকার_MATTERS)
	ধ্রুবক size_t align_mask = up_progmem_pagesize(0) - 1;
#অন্যথা
	ধ্রুবক size_t align_mask = আকার(uint32_t) - 1;
#endif
	ধ্রুবক size_t image_আকার = (file_আকার + align_mask) & ~align_mask;

	uint8_t *buf = (uint8_t *)malloc(image_আকার);

	যদি (buf == nullptr) {
		close(fd);
		ফেরত ফলাফল_2::NoMemory;
	}

	memset(buf, 0xff, image_আকার);

	ধ্রুবক ssize_t bytes_read = পঠ(fd, buf, file_আকার);
	close(fd);

	যদি (bytes_read != (ssize_t) file_আকার) {
		মুক্ত(buf);
		ফেরত ফলাফল_2::ReadFailed;
	}

	ধ্রুবক uint32_t *hdr = (ধ্রুবক uint32_t *)buf;

	যদি ((hdr[0] < STM_RAM_BASE) ||			/* stack not below RAM */
	    (hdr[0] > (STM_RAM_BASE + (128 * 1024))) ||	/* stack not above RAM */
	    (hdr[1] < PX4_FLASH_BASE) ||			/* entrypoint not below flash */
	    ((hdr[1] - PX4_FLASH_BASE) > BL_FILE_আকার_LIMIT)) {	/* entrypoint not outside bootloader */
		মুক্ত(buf);
		ফেরত ফলাফল_2::InvalidImage;
	}

	ধ্রুবক ফলাফল_2 ফলাফল = write_flash(buf, image_আকার);
	মুক্ত(buf);
	ফেরত ফলাফল;
}

#অন্যথা

ফলাফল_2 flash(ধ্রুবক অক্ষর *path)
{
	ফেরত ফলাফল_2::NotSupported;
}

#endif

ধ্রুবক অক্ষর *ফলাফল_str(ফলাফল_2 ফলাফল)
{
	নির্বাচন (ফলাফল) {
	ক্ষেত্র ফলাফল_2::Updated:
		ফেরত "bootloader updated";

	ক্ষেত্র ফলাফল_2::Unchanged:
		ফেরত "bootloader unchanged";

	ক্ষেত্র ফলাফল_2::NotSupported:
		ফেরত "not supported on this hardware";

	ক্ষেত্র ফলাফল_2::OpenFailed:
		ফেরত "cannot open bootloader file";

	ক্ষেত্র ফলাফল_2::TooLarge:
		ফেরত "bootloader file too large";

	ক্ষেত্র ফলাফল_2::NoMemory:
		ফেরত "out of memory";

	ক্ষেত্র ফলাফল_2::ReadFailed:
		ফেরত "bootloader file read error";

	ক্ষেত্র ফলাফল_2::InvalidImage:
		ফেরত "not a bootloader image";

	ক্ষেত্র ফলাফল_2::EraseFailed:
		ফেরত "flash erase failed, retry before rebooting";

	ক্ষেত্র ফলাফল_2::ProgramFailed:
		ফেরত "flash program failed, retry before rebooting";

	ক্ষেত্র ফলাফল_2::VerifyFailed:
		ফেরত "verify failed, retry before rebooting";
	}

	ফেরত "unknown error";
}

} // namespace bl_update
