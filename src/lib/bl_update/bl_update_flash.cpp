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

#অন্তর্ভাৱ "bl_update_flash.h"

#অন্তর্ভাৱ <px4_platform_common/px4_config.h>

#অন্তর্ভাৱ <stdint.h>
#অন্তর্ভাৱ <stdlib.h>
#অন্তর্ভাৱ <string.h>
#অন্তর্ভাৱ <unistd.h>
#অন্তর্ভাৱ <fcntl.h>
#অন্তর্ভাৱ <sched.h>
#অন্তর্ভাৱ <sys/stat.h>

#অন্তর্ভাৱ <arch/board/board.h>

#অন্তর্ভাৱ <nuttx/progmem.h>

#যদি defined(BL_UPDATE_WATCHDOG)
#  include <drivers/drv_watchdog.h>
#অন্যথা
#  define watchdog_pet()
#endif

#যদি defined(CONFIG_ARCH_CHIP_STM32H7)
#  define BL_FILE_SIZE_LIMIT	128*1024
#  define STM_RAM_BASE        STM32_AXISRAM_BASE
#  define PAGE_SIZE_MATTERS   1
#নোচেদ্যদি defined(CONFIG_ARCH_CHIP_STM32F7)
#  define BL_FILE_SIZE_LIMIT	32*1024
#  define STM_RAM_BASE        STM32_SRAM_BASE
#অন্যথা
#  define BL_FILE_SIZE_LIMIT  16384
#  define STM_RAM_BASE        STM32_SRAM_BASE
#endif

নামাকাশ bl_update
{

#যদি defined (CONFIG_STM32_STM32F4XXX) || defined (CONFIG_ARCH_CHIP_STM32F7) || \
    defined (CONFIG_ARCH_CHIP_STM32H7)

স্থির Result write_flash(ধ্রুৱ uint8_t *buf, size_t image_size)
{
	uint8_t *base = (uint8_t *) PX4_FLASH_BASE;

	যদি (memcmp(base, buf, image_size) == 0) {
		ঘূৰাই Result::Unchanged;
	}

	/* prevent other tasks from running while we do this */
	sched_lock();

	/* The IWDG may already be running (CAN nodes start it in the bootloader)
	 * and nothing else gets to pet it while the scheduler is locked. */
	watchdog_pet();
	ধ্রুৱ তার্কিক erased = up_progmem_eraseblock(0) >= 0;
	watchdog_pet();

	/* Program even if the erase reported an error: the old bootloader is
	 * already damaged, and a failed program reports it below. */
	ধ্রুৱ ssize_t size = up_progmem_write((size_t) base, buf, image_size);
	watchdog_pet();

	/* re-lock the flash control register */
	stm32_flash_lock();

	sched_unlock();

	যদি (size != (ssize_t) image_size) {
		ঘূৰাই erased ? Result::ProgramFailed : Result::EraseFailed;
	}

	যদি (memcmp(base, buf, image_size) != 0) {
		ঘূৰাই Result::VerifyFailed;
	}

	ঘূৰাই Result::Updated;
}

Result flash(ধ্রুৱ ৱর্ণ *path)
{
	পূর্ণাঙ্ক fd = open(path, O_RDONLY);

	যদি (fd < 0) {
		ঘূৰাই Result::OpenFailed;
	}

	সংরচনা stat s;

	যদি (fstat(fd, &s) != 0) {
		close(fd);
		ঘূৰাই Result::OpenFailed;
	}

	যদি (s.st_size > BL_FILE_SIZE_LIMIT) {
		close(fd);
		ঘূৰাই Result::TooLarge;
	}

	ধ্রুৱ size_t file_size = s.st_size;

	/* up_progmem_write() rejects a size that is not a multiple of its
	 * program unit, and by then the old bootloader is erased. */
#যদি defined(PAGE_SIZE_MATTERS)
	ধ্রুৱ size_t align_mask = up_progmem_pagesize(0) - 1;
#অন্যথা
	ধ্রুৱ size_t align_mask = পরিমাণ(uint32_t) - 1;
#endif
	ধ্রুৱ size_t image_size = (file_size + align_mask) & ~align_mask;

	uint8_t *buf = (uint8_t *)malloc(image_size);

	যদি (buf == nullptr) {
		close(fd);
		ঘূৰাই Result::NoMemory;
	}

	memset(buf, 0xff, image_size);

	ধ্রুৱ ssize_t bytes_read = পঠ(fd, buf, file_size);
	close(fd);

	যদি (bytes_read != (ssize_t) file_size) {
		মুক্ত(buf);
		ঘূৰাই Result::ReadFailed;
	}

	ধ্রুৱ uint32_t *hdr = (ধ্রুৱ uint32_t *)buf;

	যদি ((hdr[0] < STM_RAM_BASE) ||			/* stack not below RAM */
	    (hdr[0] > (STM_RAM_BASE + (128 * 1024))) ||	/* stack not above RAM */
	    (hdr[1] < PX4_FLASH_BASE) ||			/* entrypoint not below flash */
	    ((hdr[1] - PX4_FLASH_BASE) > BL_FILE_SIZE_LIMIT)) {	/* entrypoint not outside bootloader */
		মুক্ত(buf);
		ঘূৰাই Result::InvalidImage;
	}

	ধ্রুৱ Result result = write_flash(buf, image_size);
	মুক্ত(buf);
	ঘূৰাই result;
}

#অন্যথা

Result flash(ধ্রুৱ ৱর্ণ *path)
{
	ঘূৰাই Result::NotSupported;
}

#endif

ধ্রুৱ ৱর্ণ *result_str(Result result)
{
	ৱিকল্প (result) {
	পক্ষ Result::Updated:
		ঘূৰাই "bootloader updated";

	পক্ষ Result::Unchanged:
		ঘূৰাই "bootloader unchanged";

	পক্ষ Result::NotSupported:
		ঘূৰাই "not supported on this hardware";

	পক্ষ Result::OpenFailed:
		ঘূৰাই "cannot open bootloader file";

	পক্ষ Result::TooLarge:
		ঘূৰাই "bootloader file too large";

	পক্ষ Result::NoMemory:
		ঘূৰাই "out of memory";

	পক্ষ Result::ReadFailed:
		ঘূৰাই "bootloader file read error";

	পক্ষ Result::InvalidImage:
		ঘূৰাই "not a bootloader image";

	পক্ষ Result::EraseFailed:
		ঘূৰাই "flash erase failed, retry before rebooting";

	পক্ষ Result::ProgramFailed:
		ঘূৰাই "flash program failed, retry before rebooting";

	পক্ষ Result::VerifyFailed:
		ঘূৰাই "verify failed, retry before rebooting";
	}

	ঘূৰাই "unknown error";
}

} // namespace bl_update
