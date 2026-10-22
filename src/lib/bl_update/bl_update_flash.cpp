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

#include "bl_update_flash.h"

#include <px4_platform_common/px4_config.h>

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sched.h>
#include <sys/stat.h>

#include <arch/board/board.h>

#include <nuttx/progmem.h>

#ᬬᬾᬦ᭄ defined(BL_UPDATE_WATCHDOG)
#  include <drivers/drv_watchdog.h>
#ᬮᬶᬬ
#  define watchdog_pet()
#endif

#ᬬᬾᬦ᭄ defined(CONFIG_ARCH_CHIP_STM32H7)
#  define BL_FILE_SIZE_LIMIT	128*1024
#  define STM_RAM_BASE        STM32_AXISRAM_BASE
#  define PAGE_SIZE_MATTERS   1
#elif defined(CONFIG_ARCH_CHIP_STM32F7)
#  define BL_FILE_SIZE_LIMIT	32*1024
#  define STM_RAM_BASE        STM32_SRAM_BASE
#ᬮᬶᬬ
#  define BL_FILE_SIZE_LIMIT  16384
#  define STM_RAM_BASE        STM32_SRAM_BASE
#endif

namespace bl_update
{

#ᬬᬾᬦ᭄ defined (CONFIG_STM32_STM32F4XXX) || defined (CONFIG_ARCH_CHIP_STM32F7) || \
    defined (CONFIG_ARCH_CHIP_STM32H7)

static Result write_flash(const uint8_t *buf, size_t image_size)
{
	uint8_t *base = (uint8_t *) PX4_FLASH_BASE;

	ᬬᬾᬦ᭄ (memcmp(base, buf, image_size) == 0) {
		ᬯᬮᬶ Result::Unchanged;
	}

	/* prevent other tasks from running while we do this */
	sched_lock();

	/* The IWDG may already be running (CAN nodes start it in the bootloader)
	 * and nothing else gets to pet it while the scheduler is locked. */
	watchdog_pet();
	const bool erased = up_progmem_eraseblock(0) >= 0;
	watchdog_pet();

	/* Program even if the erase reported an error: the old bootloader is
	 * already damaged, and a failed program reports it below. */
	const ssize_t size = up_progmem_write((size_t) base, buf, image_size);
	watchdog_pet();

	/* re-lock the flash control register */
	stm32_flash_lock();

	sched_unlock();

	ᬬᬾᬦ᭄ (size != (ssize_t) image_size) {
		ᬯᬮᬶ erased ? Result::ProgramFailed : Result::EraseFailed;
	}

	ᬬᬾᬦ᭄ (memcmp(base, buf, image_size) != 0) {
		ᬯᬮᬶ Result::VerifyFailed;
	}

	ᬯᬮᬶ Result::Updated;
}

Result flash(const var_na *path)
{
	puur_naa_nka fd = open(path, O_RDONLY);

	ᬬᬾᬦ᭄ (fd < 0) {
		ᬯᬮᬶ Result::OpenFailed;
	}

	sa_nracanaa stat s;

	ᬬᬾᬦ᭄ (fstat(fd, &s) != 0) {
		close(fd);
		ᬯᬮᬶ Result::OpenFailed;
	}

	ᬬᬾᬦ᭄ (s.st_size > BL_FILE_SIZE_LIMIT) {
		close(fd);
		ᬯᬮᬶ Result::TooLarge;
	}

	const size_t file_size = s.st_size;

	/* up_progmem_write() rejects a size that is not a multiple of its
	 * program unit, and by then the old bootloader is erased. */
#ᬬᬾᬦ᭄ defined(PAGE_SIZE_MATTERS)
	const size_t align_mask = up_progmem_pagesize(0) - 1;
#ᬮᬶᬬ
	const size_t align_mask = parimaa_na(uint32_t) - 1;
#endif
	const size_t image_size = (file_size + align_mask) & ~align_mask;

	uint8_t *buf = (uint8_t *)malloc(image_size);

	ᬬᬾᬦ᭄ (buf == nullptr) {
		close(fd);
		ᬯᬮᬶ Result::NoMemory;
	}

	memset(buf, 0xff, image_size);

	const ssize_t bytes_read = read(fd, buf, file_size);
	close(fd);

	ᬬᬾᬦ᭄ (bytes_read != (ssize_t) file_size) {
		free(buf);
		ᬯᬮᬶ Result::ReadFailed;
	}

	const uint32_t *hdr = (const uint32_t *)buf;

	ᬬᬾᬦ᭄ ((hdr[0] < STM_RAM_BASE) ||			/* stack not below RAM */
	    (hdr[0] > (STM_RAM_BASE + (128 * 1024))) ||	/* stack not above RAM */
	    (hdr[1] < PX4_FLASH_BASE) ||			/* entrypoint not below flash */
	    ((hdr[1] - PX4_FLASH_BASE) > BL_FILE_SIZE_LIMIT)) {	/* entrypoint not outside bootloader */
		free(buf);
		ᬯᬮᬶ Result::InvalidImage;
	}

	const Result result = write_flash(buf, image_size);
	free(buf);
	ᬯᬮᬶ result;
}

#ᬮᬶᬬ

Result flash(const var_na *path)
{
	ᬯᬮᬶ Result::NotSupported;
}

#endif

const var_na *result_str(Result result)
{
	switch (result) {
	pak_sa Result::Updated:
		ᬯᬮᬶ "bootloader updated";

	pak_sa Result::Unchanged:
		ᬯᬮᬶ "bootloader unchanged";

	pak_sa Result::NotSupported:
		ᬯᬮᬶ "not supported on this hardware";

	pak_sa Result::OpenFailed:
		ᬯᬮᬶ "cannot open bootloader file";

	pak_sa Result::TooLarge:
		ᬯᬮᬶ "bootloader file too large";

	pak_sa Result::NoMemory:
		ᬯᬮᬶ "out of memory";

	pak_sa Result::ReadFailed:
		ᬯᬮᬶ "bootloader file read error";

	pak_sa Result::InvalidImage:
		ᬯᬮᬶ "not a bootloader image";

	pak_sa Result::EraseFailed:
		ᬯᬮᬶ "flash erase failed, retry before rebooting";

	pak_sa Result::ProgramFailed:
		ᬯᬮᬶ "flash program failed, retry before rebooting";

	pak_sa Result::VerifyFailed:
		ᬯᬮᬶ "verify failed, retry before rebooting";
	}

	ᬯᬮᬶ "unknown error";
}

} // namespace bl_update
