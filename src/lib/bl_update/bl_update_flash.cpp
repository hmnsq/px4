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

#もし defined(BL_UPDATE_WATCHDOG)
#  include <drivers/drv_watchdog.h>
#否则
#  define watchdog_pet()
#endif

#もし defined(CONFIG_ARCH_CHIP_STM32H7)
#  define BL_FILE_SIZE_LIMIT	128*1024
#  define STM_RAM_BASE        STM32_AXISRAM_BASE
#  define PAGE_SIZE_MATTERS   1
#elif defined(CONFIG_ARCH_CHIP_STM32F7)
#  define BL_FILE_SIZE_LIMIT	32*1024
#  define STM_RAM_BASE        STM32_SRAM_BASE
#否则
#  define BL_FILE_SIZE_LIMIT  16384
#  define STM_RAM_BASE        STM32_SRAM_BASE
#endif

namespace bl_update
{

#もし defined (CONFIG_STM32_STM32F4XXX) || defined (CONFIG_ARCH_CHIP_STM32F7) || \
    defined (CONFIG_ARCH_CHIP_STM32H7)

static Result write_flash(const uint8_t *buf, size_t image_size)
{
	uint8_t *base = (uint8_t *) PX4_FLASH_BASE;

	もし (memcmp(base, buf, image_size) == 0) {
		戻る Result::Unchanged;
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

	もし (size != (ssize_t) image_size) {
		戻る erased ? Result::ProgramFailed : Result::EraseFailed;
	}

	もし (memcmp(base, buf, image_size) != 0) {
		戻る Result::VerifyFailed;
	}

	戻る Result::Updated;
}

Result flash(const char *path)
{
	整数 fd = open(path, O_RDONLY);

	もし (fd < 0) {
		戻る Result::OpenFailed;
	}

	struct stat s;

	もし (fstat(fd, &s) != 0) {
		close(fd);
		戻る Result::OpenFailed;
	}

	もし (s.st_size > BL_FILE_SIZE_LIMIT) {
		close(fd);
		戻る Result::TooLarge;
	}

	const size_t file_size = s.st_size;

	/* up_progmem_write() rejects a size that is not a multiple of its
	 * program unit, and by then the old bootloader is erased. */
#もし defined(PAGE_SIZE_MATTERS)
	const size_t align_mask = up_progmem_pagesize(0) - 1;
#否则
	const size_t align_mask = sizeof(uint32_t) - 1;
#endif
	const size_t image_size = (file_size + align_mask) & ~align_mask;

	uint8_t *buf = (uint8_t *)malloc(image_size);

	もし (buf == nullptr) {
		close(fd);
		戻る Result::NoMemory;
	}

	memset(buf, 0xff, image_size);

	const ssize_t bytes_read = read(fd, buf, file_size);
	close(fd);

	もし (bytes_read != (ssize_t) file_size) {
		free(buf);
		戻る Result::ReadFailed;
	}

	const uint32_t *hdr = (const uint32_t *)buf;

	もし ((hdr[0] < STM_RAM_BASE) ||			/* stack not below RAM */
	    (hdr[0] > (STM_RAM_BASE + (128 * 1024))) ||	/* stack not above RAM */
	    (hdr[1] < PX4_FLASH_BASE) ||			/* entrypoint not below flash */
	    ((hdr[1] - PX4_FLASH_BASE) > BL_FILE_SIZE_LIMIT)) {	/* entrypoint not outside bootloader */
		free(buf);
		戻る Result::InvalidImage;
	}

	const Result result = write_flash(buf, image_size);
	free(buf);
	戻る result;
}

#否则

Result flash(const char *path)
{
	戻る Result::NotSupported;
}

#endif

const char *result_str(Result result)
{
	switch (result) {
	case Result::Updated:
		戻る "bootloader updated";

	case Result::Unchanged:
		戻る "bootloader unchanged";

	case Result::NotSupported:
		戻る "not supported on this hardware";

	case Result::OpenFailed:
		戻る "cannot open bootloader file";

	case Result::TooLarge:
		戻る "bootloader file too large";

	case Result::NoMemory:
		戻る "out of memory";

	case Result::ReadFailed:
		戻る "bootloader file read error";

	case Result::InvalidImage:
		戻る "not a bootloader image";

	case Result::EraseFailed:
		戻る "flash erase failed, retry before rebooting";

	case Result::ProgramFailed:
		戻る "flash program failed, retry before rebooting";

	case Result::VerifyFailed:
		戻る "verify failed, retry before rebooting";
	}

	戻る "unknown error";
}

} // namespace bl_update
