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

#अन्तर्भाव "bl_update_flash.h"

#अन्तर्भाव <px4_platform_common/px4_config.h>

#अन्तर्भाव <stdint.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <unistd.h>
#अन्तर्भाव <fcntl.h>
#अन्तर्भाव <sched.h>
#अन्तर्भाव <sys/stat.h>

#अन्तर्भाव <arch/board/board.h>

#अन्तर्भाव <nuttx/progmem.h>

#जइ defined(BL_UPDATE_WATCHDOG)
#  include <drivers/drv_watchdog.h>
#अण्णहा
#  define watchdog_pet()
#endif

#जइ defined(CONFIG_ARCH_CHIP_STM32H7)
#  define BL_FILE_SIZE_LIMIT	128*1024
#  define STM_RAM_BASE        STM32_AXISRAM_BASE
#  define PAGE_SIZE_MATTERS   1
#नोचेद्यदि defined(CONFIG_ARCH_CHIP_STM32F7)
#  define BL_FILE_SIZE_LIMIT	32*1024
#  define STM_RAM_BASE        STM32_SRAM_BASE
#अण्णहा
#  define BL_FILE_SIZE_LIMIT  16384
#  define STM_RAM_BASE        STM32_SRAM_BASE
#endif

नामाकाश bl_update
{

#जइ defined (CONFIG_STM32_STM32F4XXX) || defined (CONFIG_ARCH_CHIP_STM32F7) || \
    defined (CONFIG_ARCH_CHIP_STM32H7)

स्थिर Result write_flash(ध्रुव uint8_t *buf, size_t image_size)
{
	uint8_t *base = (uint8_t *) PX4_FLASH_BASE;

	जइ (memcmp(base, buf, image_size) == 0) {
		प्रत्यागम Result::Unchanged;
	}

	/* prevent other tasks from running while we do this */
	sched_lock();

	/* The IWDG may already be running (CAN nodes start it in the bootloader)
	 * and nothing else gets to pet it while the scheduler is locked. */
	watchdog_pet();
	ध्रुव तार्किक erased = up_progmem_eraseblock(0) >= 0;
	watchdog_pet();

	/* Program even if the erase reported an error: the old bootloader is
	 * already damaged, and a failed program reports it below. */
	ध्रुव ssize_t size = up_progmem_write((size_t) base, buf, image_size);
	watchdog_pet();

	/* re-lock the flash control register */
	stm32_flash_lock();

	sched_unlock();

	जइ (size != (ssize_t) image_size) {
		प्रत्यागम erased ? Result::ProgramFailed : Result::EraseFailed;
	}

	जइ (memcmp(base, buf, image_size) != 0) {
		प्रत्यागम Result::VerifyFailed;
	}

	प्रत्यागम Result::Updated;
}

Result flash(ध्रुव वर्ण *path)
{
	पूर्णाङ्क fd = open(path, O_RDONLY);

	जइ (fd < 0) {
		प्रत्यागम Result::OpenFailed;
	}

	संरचना stat s;

	जइ (fstat(fd, &s) != 0) {
		close(fd);
		प्रत्यागम Result::OpenFailed;
	}

	जइ (s.st_size > BL_FILE_SIZE_LIMIT) {
		close(fd);
		प्रत्यागम Result::TooLarge;
	}

	ध्रुव size_t file_size = s.st_size;

	/* up_progmem_write() rejects a size that is not a multiple of its
	 * program unit, and by then the old bootloader is erased. */
#जइ defined(PAGE_SIZE_MATTERS)
	ध्रुव size_t align_mask = up_progmem_pagesize(0) - 1;
#अण्णहा
	ध्रुव size_t align_mask = परिमाण(uint32_t) - 1;
#endif
	ध्रुव size_t image_size = (file_size + align_mask) & ~align_mask;

	uint8_t *buf = (uint8_t *)malloc(image_size);

	जइ (buf == nullptr) {
		close(fd);
		प्रत्यागम Result::NoMemory;
	}

	memset(buf, 0xff, image_size);

	ध्रुव ssize_t bytes_read = पठ(fd, buf, file_size);
	close(fd);

	जइ (bytes_read != (ssize_t) file_size) {
		मुक्त(buf);
		प्रत्यागम Result::ReadFailed;
	}

	ध्रुव uint32_t *hdr = (ध्रुव uint32_t *)buf;

	जइ ((hdr[0] < STM_RAM_BASE) ||			/* stack not below RAM */
	    (hdr[0] > (STM_RAM_BASE + (128 * 1024))) ||	/* stack not above RAM */
	    (hdr[1] < PX4_FLASH_BASE) ||			/* entrypoint not below flash */
	    ((hdr[1] - PX4_FLASH_BASE) > BL_FILE_SIZE_LIMIT)) {	/* entrypoint not outside bootloader */
		मुक्त(buf);
		प्रत्यागम Result::InvalidImage;
	}

	ध्रुव Result result = write_flash(buf, image_size);
	मुक्त(buf);
	प्रत्यागम result;
}

#अण्णहा

Result flash(ध्रुव वर्ण *path)
{
	प्रत्यागम Result::NotSupported;
}

#endif

ध्रुव वर्ण *result_str(Result result)
{
	विकल्प (result) {
	पक्ष Result::Updated:
		प्रत्यागम "bootloader updated";

	पक्ष Result::Unchanged:
		प्रत्यागम "bootloader unchanged";

	पक्ष Result::NotSupported:
		प्रत्यागम "not supported on this hardware";

	पक्ष Result::OpenFailed:
		प्रत्यागम "cannot open bootloader file";

	पक्ष Result::TooLarge:
		प्रत्यागम "bootloader file too large";

	पक्ष Result::NoMemory:
		प्रत्यागम "out of memory";

	पक्ष Result::ReadFailed:
		प्रत्यागम "bootloader file read error";

	पक्ष Result::InvalidImage:
		प्रत्यागम "not a bootloader image";

	पक्ष Result::EraseFailed:
		प्रत्यागम "flash erase failed, retry before rebooting";

	पक्ष Result::ProgramFailed:
		प्रत्यागम "flash program failed, retry before rebooting";

	पक्ष Result::VerifyFailed:
		प्रत्यागम "verify failed, retry before rebooting";
	}

	प्रत्यागम "unknown error";
}

} // namespace bl_update
