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

#ಅನ್ತರ್ಭಾವ "bl_update_flash.h"

#ಅನ್ತರ್ಭಾವ <px4_platform_common/px4_config.h>

#ಅನ್ತರ್ಭಾವ <stdint.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>
#ಅನ್ತರ್ಭಾವ <fcntl.h>
#ಅನ್ತರ್ಭಾವ <sched.h>
#ಅನ್ತರ್ಭಾವ <sys/stat.h>

#ಅನ್ತರ್ಭಾವ <arch/board/board.h>

#ಅನ್ತರ್ಭಾವ <nuttx/progmem.h>

#ಯದಿ defined(BL_UPDATE_WATCHDOG)
#  include <drivers/drv_watchdog.h>
#ಅನ್ಯಥಾ
#  define watchdog_pet()
#endif

#ಯದಿ defined(CONFIG_ARCH_CHIP_STM32H7)
#  define BL_FILE_SIZE_LIMIT	128*1024
#  define STM_RAM_BASE        STM32_AXISRAM_BASE
#  define PAGE_SIZE_MATTERS   1
#ನೋಚೇದ್ಯದಿ defined(CONFIG_ARCH_CHIP_STM32F7)
#  define BL_FILE_SIZE_LIMIT	32*1024
#  define STM_RAM_BASE        STM32_SRAM_BASE
#ಅನ್ಯಥಾ
#  define BL_FILE_SIZE_LIMIT  16384
#  define STM_RAM_BASE        STM32_SRAM_BASE
#endif

ನಾಮಾಕಾಶ bl_update
{

#ಯದಿ defined (CONFIG_STM32_STM32F4XXX) || defined (CONFIG_ARCH_CHIP_STM32F7) || \
    defined (CONFIG_ARCH_CHIP_STM32H7)

ಸ್ಥಿರ Result write_flash(ಧ್ರುವ uint8_t *buf, size_t image_size)
{
	uint8_t *base = (uint8_t *) PX4_FLASH_BASE;

	ಯದಿ (memcmp(base, buf, image_size) == 0) {
		ಪ್ರತ್ಯಾಗಮ Result::Unchanged;
	}

	/* prevent other tasks from running while we do this */
	sched_lock();

	/* The IWDG may already be running (CAN nodes start it in the bootloader)
	 * and nothing else gets to pet it while the scheduler is locked. */
	watchdog_pet();
	ಧ್ರುವ ತಾರ್ಕಿಕ erased = up_progmem_eraseblock(0) >= 0;
	watchdog_pet();

	/* Program even if the erase reported an error: the old bootloader is
	 * already damaged, and a failed program reports it below. */
	ಧ್ರುವ ssize_t size = up_progmem_write((size_t) base, buf, image_size);
	watchdog_pet();

	/* re-lock the flash control register */
	stm32_flash_lock();

	sched_unlock();

	ಯದಿ (size != (ssize_t) image_size) {
		ಪ್ರತ್ಯಾಗಮ erased ? Result::ProgramFailed : Result::EraseFailed;
	}

	ಯದಿ (memcmp(base, buf, image_size) != 0) {
		ಪ್ರತ್ಯಾಗಮ Result::VerifyFailed;
	}

	ಪ್ರತ್ಯಾಗಮ Result::Updated;
}

Result flash(ಧ್ರುವ ವರ್ಣ *path)
{
	ಪೂರ್ಣಾಙ್ಕ fd = open(path, O_RDONLY);

	ಯದಿ (fd < 0) {
		ಪ್ರತ್ಯಾಗಮ Result::OpenFailed;
	}

	ಸಂರಚನಾ stat s;

	ಯದಿ (fstat(fd, &s) != 0) {
		close(fd);
		ಪ್ರತ್ಯಾಗಮ Result::OpenFailed;
	}

	ಯದಿ (s.st_size > BL_FILE_SIZE_LIMIT) {
		close(fd);
		ಪ್ರತ್ಯಾಗಮ Result::TooLarge;
	}

	ಧ್ರುವ size_t file_size = s.st_size;

	/* up_progmem_write() rejects a size that is not a multiple of its
	 * program unit, and by then the old bootloader is erased. */
#ಯದಿ defined(PAGE_SIZE_MATTERS)
	ಧ್ರುವ size_t align_mask = up_progmem_pagesize(0) - 1;
#ಅನ್ಯಥಾ
	ಧ್ರುವ size_t align_mask = ಪರಿಮಾಣ(uint32_t) - 1;
#endif
	ಧ್ರುವ size_t image_size = (file_size + align_mask) & ~align_mask;

	uint8_t *buf = (uint8_t *)malloc(image_size);

	ಯದಿ (buf == nullptr) {
		close(fd);
		ಪ್ರತ್ಯಾಗಮ Result::NoMemory;
	}

	memset(buf, 0xff, image_size);

	ಧ್ರುವ ssize_t bytes_read = ಪಠ(fd, buf, file_size);
	close(fd);

	ಯದಿ (bytes_read != (ssize_t) file_size) {
		ಮುಕ್ತ(buf);
		ಪ್ರತ್ಯಾಗಮ Result::ReadFailed;
	}

	ಧ್ರುವ uint32_t *hdr = (ಧ್ರುವ uint32_t *)buf;

	ಯದಿ ((hdr[0] < STM_RAM_BASE) ||			/* stack not below RAM */
	    (hdr[0] > (STM_RAM_BASE + (128 * 1024))) ||	/* stack not above RAM */
	    (hdr[1] < PX4_FLASH_BASE) ||			/* entrypoint not below flash */
	    ((hdr[1] - PX4_FLASH_BASE) > BL_FILE_SIZE_LIMIT)) {	/* entrypoint not outside bootloader */
		ಮುಕ್ತ(buf);
		ಪ್ರತ್ಯಾಗಮ Result::InvalidImage;
	}

	ಧ್ರುವ Result result = write_flash(buf, image_size);
	ಮುಕ್ತ(buf);
	ಪ್ರತ್ಯಾಗಮ result;
}

#ಅನ್ಯಥಾ

Result flash(ಧ್ರುವ ವರ್ಣ *path)
{
	ಪ್ರತ್ಯಾಗಮ Result::NotSupported;
}

#endif

ಧ್ರುವ ವರ್ಣ *result_str(Result result)
{
	ವಿಕಲ್ಪ (result) {
	ಪಕ್ಷ Result::Updated:
		ಪ್ರತ್ಯಾಗಮ "bootloader updated";

	ಪಕ್ಷ Result::Unchanged:
		ಪ್ರತ್ಯಾಗಮ "bootloader unchanged";

	ಪಕ್ಷ Result::NotSupported:
		ಪ್ರತ್ಯಾಗಮ "not supported on this hardware";

	ಪಕ್ಷ Result::OpenFailed:
		ಪ್ರತ್ಯಾಗಮ "cannot open bootloader file";

	ಪಕ್ಷ Result::TooLarge:
		ಪ್ರತ್ಯಾಗಮ "bootloader file too large";

	ಪಕ್ಷ Result::NoMemory:
		ಪ್ರತ್ಯಾಗಮ "out of memory";

	ಪಕ್ಷ Result::ReadFailed:
		ಪ್ರತ್ಯಾಗಮ "bootloader file read error";

	ಪಕ್ಷ Result::InvalidImage:
		ಪ್ರತ್ಯಾಗಮ "not a bootloader image";

	ಪಕ್ಷ Result::EraseFailed:
		ಪ್ರತ್ಯಾಗಮ "flash erase failed, retry before rebooting";

	ಪಕ್ಷ Result::ProgramFailed:
		ಪ್ರತ್ಯಾಗಮ "flash program failed, retry before rebooting";

	ಪಕ್ಷ Result::VerifyFailed:
		ಪ್ರತ್ಯಾಗಮ "verify failed, retry before rebooting";
	}

	ಪ್ರತ್ಯಾಗಮ "unknown error";
}

} // namespace bl_update
