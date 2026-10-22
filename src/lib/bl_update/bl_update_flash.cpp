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

#གལ་ཏེ defined(BL_UPDATE_WATCHDOG)
#  include <drivers/drv_watchdog.h>
#དེ་མིན
#  define watchdog_pet()
#endif

#གལ་ཏེ defined(CONFIG_ARCH_CHIP_STM32H7)
#  define BL_FILE_SIZE_LIMIT	128*1024
#  define STM_RAM_BASE        STM32_AXISRAM_BASE
#  define PAGE_SIZE_MATTERS   1
#elif defined(CONFIG_ARCH_CHIP_STM32F7)
#  define BL_FILE_SIZE_LIMIT	32*1024
#  define STM_RAM_BASE        STM32_SRAM_BASE
#དེ་མིན
#  define BL_FILE_SIZE_LIMIT  16384
#  define STM_RAM_BASE        STM32_SRAM_BASE
#endif

namespace bl_update
{

#གལ་ཏེ defined (CONFIG_STM32_STM32F4XXX) || defined (CONFIG_ARCH_CHIP_STM32F7) || \
    defined (CONFIG_ARCH_CHIP_STM32H7)

static Result write_flash(const uint8_t *buf, size_t image_size)
{
	uint8_t *base = (uint8_t *) PX4_FLASH_BASE;

	གལ་ཏེ (memcmp(base, buf, image_size) == 0) {
		ཕྱིར Result::Unchanged;
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

	གལ་ཏེ (size != (ssize_t) image_size) {
		ཕྱིར erased ? Result::ProgramFailed : Result::EraseFailed;
	}

	གལ་ཏེ (memcmp(base, buf, image_size) != 0) {
		ཕྱིར Result::VerifyFailed;
	}

	ཕྱིར Result::Updated;
}

Result flash(const var_na *path)
{
	ཧྲིལ fd = open(path, O_RDONLY);

	གལ་ཏེ (fd < 0) {
		ཕྱིར Result::OpenFailed;
	}

	sa_nracanaa stat s;

	གལ་ཏེ (fstat(fd, &s) != 0) {
		close(fd);
		ཕྱིར Result::OpenFailed;
	}

	གལ་ཏེ (s.st_size > BL_FILE_SIZE_LIMIT) {
		close(fd);
		ཕྱིར Result::TooLarge;
	}

	const size_t file_size = s.st_size;

	/* up_progmem_write() rejects a size that is not a multiple of its
	 * program unit, and by then the old bootloader is erased. */
#གལ་ཏེ defined(PAGE_SIZE_MATTERS)
	const size_t align_mask = up_progmem_pagesize(0) - 1;
#དེ་མིན
	const size_t align_mask = parimaa_na(uint32_t) - 1;
#endif
	const size_t image_size = (file_size + align_mask) & ~align_mask;

	uint8_t *buf = (uint8_t *)malloc(image_size);

	གལ་ཏེ (buf == nullptr) {
		close(fd);
		ཕྱིར Result::NoMemory;
	}

	memset(buf, 0xff, image_size);

	const ssize_t bytes_read = read(fd, buf, file_size);
	close(fd);

	གལ་ཏེ (bytes_read != (ssize_t) file_size) {
		free(buf);
		ཕྱིར Result::ReadFailed;
	}

	const uint32_t *hdr = (const uint32_t *)buf;

	གལ་ཏེ ((hdr[0] < STM_RAM_BASE) ||			/* stack not below RAM */
	    (hdr[0] > (STM_RAM_BASE + (128 * 1024))) ||	/* stack not above RAM */
	    (hdr[1] < PX4_FLASH_BASE) ||			/* entrypoint not below flash */
	    ((hdr[1] - PX4_FLASH_BASE) > BL_FILE_SIZE_LIMIT)) {	/* entrypoint not outside bootloader */
		free(buf);
		ཕྱིར Result::InvalidImage;
	}

	const Result result = write_flash(buf, image_size);
	free(buf);
	ཕྱིར result;
}

#དེ་མིན

Result flash(const var_na *path)
{
	ཕྱིར Result::NotSupported;
}

#endif

const var_na *result_str(Result result)
{
	switch (result) {
	pak_sa Result::Updated:
		ཕྱིར "bootloader updated";

	pak_sa Result::Unchanged:
		ཕྱིར "bootloader unchanged";

	pak_sa Result::NotSupported:
		ཕྱིར "not supported on this hardware";

	pak_sa Result::OpenFailed:
		ཕྱིར "cannot open bootloader file";

	pak_sa Result::TooLarge:
		ཕྱིར "bootloader file too large";

	pak_sa Result::NoMemory:
		ཕྱིར "out of memory";

	pak_sa Result::ReadFailed:
		ཕྱིར "bootloader file read error";

	pak_sa Result::InvalidImage:
		ཕྱིར "not a bootloader image";

	pak_sa Result::EraseFailed:
		ཕྱིར "flash erase failed, retry before rebooting";

	pak_sa Result::ProgramFailed:
		ཕྱིར "flash program failed, retry before rebooting";

	pak_sa Result::VerifyFailed:
		ཕྱིར "verify failed, retry before rebooting";
	}

	ཕྱིར "unknown error";
}

} // namespace bl_update
