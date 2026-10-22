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

#ଅନ୍ତର୍ଭାଵ "bl_update_flash.h"

#ଅନ୍ତର୍ଭାଵ <px4_platform_common/px4_config.h>

#ଅନ୍ତର୍ଭାଵ <stdint.h>
#ଅନ୍ତର୍ଭାଵ <stdlib.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <unistd.h>
#ଅନ୍ତର୍ଭାଵ <fcntl.h>
#ଅନ୍ତର୍ଭାଵ <sched.h>
#ଅନ୍ତର୍ଭାଵ <sys/stat.h>

#ଅନ୍ତର୍ଭାଵ <arch/board/board.h>

#ଅନ୍ତର୍ଭାଵ <nuttx/progmem.h>

#ଯଦି defined(BL_UPDATE_WATCHDOG)
#  include <drivers/drv_watchdog.h>
#ଅନ୍ୟଥା
#  define watchdog_pet()
#endif

#ଯଦି defined(CONFIG_ARCH_CHIP_STM32H7)
#  define BL_FILE_SIZE_LIMIT	128*1024
#  define STM_RAM_BASE        STM32_AXISRAM_BASE
#  define PAGE_SIZE_MATTERS   1
#ନଚେତଯଦି defined(CONFIG_ARCH_CHIP_STM32F7)
#  define BL_FILE_SIZE_LIMIT	32*1024
#  define STM_RAM_BASE        STM32_SRAM_BASE
#ଅନ୍ୟଥା
#  define BL_FILE_SIZE_LIMIT  16384
#  define STM_RAM_BASE        STM32_SRAM_BASE
#endif

ନାମାକାଶ bl_update
{

#ଯଦି defined (CONFIG_STM32_STM32F4XXX) || defined (CONFIG_ARCH_CHIP_STM32F7) || \
    defined (CONFIG_ARCH_CHIP_STM32H7)

ସ୍ଥିର Result write_flash(ଅଚଳ uint8_t *buf, size_t image_size)
{
	uint8_t *base = (uint8_t *) PX4_FLASH_BASE;

	ଯଦି (memcmp(base, buf, image_size) == 0) {
		ଫେରନ୍ତୁ Result::Unchanged;
	}

	/* prevent other tasks from running while we do this */
	sched_lock();

	/* The IWDG may already be running (CAN nodes start it in the bootloader)
	 * and nothing else gets to pet it while the scheduler is locked. */
	watchdog_pet();
	ଅଚଳ ତାର୍କିକ erased = up_progmem_eraseblock(0) >= 0;
	watchdog_pet();

	/* Program even if the erase reported an error: the old bootloader is
	 * already damaged, and a failed program reports it below. */
	ଅଚଳ ssize_t size = up_progmem_write((size_t) base, buf, image_size);
	watchdog_pet();

	/* re-lock the flash control register */
	stm32_flash_lock();

	sched_unlock();

	ଯଦି (size != (ssize_t) image_size) {
		ଫେରନ୍ତୁ erased ? Result::ProgramFailed : Result::EraseFailed;
	}

	ଯଦି (memcmp(base, buf, image_size) != 0) {
		ଫେରନ୍ତୁ Result::VerifyFailed;
	}

	ଫେରନ୍ତୁ Result::Updated;
}

Result flash(ଅଚଳ ଅକ୍ଷର *path)
{
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = open(path, O_RDONLY);

	ଯଦି (fd < 0) {
		ଫେରନ୍ତୁ Result::OpenFailed;
	}

	ସଂରଚନା stat s;

	ଯଦି (fstat(fd, &s) != 0) {
		close(fd);
		ଫେରନ୍ତୁ Result::OpenFailed;
	}

	ଯଦି (s.st_size > BL_FILE_SIZE_LIMIT) {
		close(fd);
		ଫେରନ୍ତୁ Result::TooLarge;
	}

	ଅଚଳ size_t file_size = s.st_size;

	/* up_progmem_write() rejects a size that is not a multiple of its
	 * program unit, and by then the old bootloader is erased. */
#ଯଦି defined(PAGE_SIZE_MATTERS)
	ଅଚଳ size_t align_mask = up_progmem_pagesize(0) - 1;
#ଅନ୍ୟଥା
	ଅଚଳ size_t align_mask = ଆକାର(uint32_t) - 1;
#endif
	ଅଚଳ size_t image_size = (file_size + align_mask) & ~align_mask;

	uint8_t *buf = (uint8_t *)malloc(image_size);

	ଯଦି (buf == nullptr) {
		close(fd);
		ଫେରନ୍ତୁ Result::NoMemory;
	}

	memset(buf, 0xff, image_size);

	ଅଚଳ ssize_t bytes_read = ପଠ(fd, buf, file_size);
	close(fd);

	ଯଦି (bytes_read != (ssize_t) file_size) {
		ମୁକ୍ତ(buf);
		ଫେରନ୍ତୁ Result::ReadFailed;
	}

	ଅଚଳ uint32_t *hdr = (ଅଚଳ uint32_t *)buf;

	ଯଦି ((hdr[0] < STM_RAM_BASE) ||			/* stack not below RAM */
	    (hdr[0] > (STM_RAM_BASE + (128 * 1024))) ||	/* stack not above RAM */
	    (hdr[1] < PX4_FLASH_BASE) ||			/* entrypoint not below flash */
	    ((hdr[1] - PX4_FLASH_BASE) > BL_FILE_SIZE_LIMIT)) {	/* entrypoint not outside bootloader */
		ମୁକ୍ତ(buf);
		ଫେରନ୍ତୁ Result::InvalidImage;
	}

	ଅଚଳ Result result = write_flash(buf, image_size);
	ମୁକ୍ତ(buf);
	ଫେରନ୍ତୁ result;
}

#ଅନ୍ୟଥା

Result flash(ଅଚଳ ଅକ୍ଷର *path)
{
	ଫେରନ୍ତୁ Result::NotSupported;
}

#endif

ଅଚଳ ଅକ୍ଷର *result_str(Result result)
{
	ବାଛ (result) {
	ସ୍ଥିତି Result::Updated:
		ଫେରନ୍ତୁ "bootloader updated";

	ସ୍ଥିତି Result::Unchanged:
		ଫେରନ୍ତୁ "bootloader unchanged";

	ସ୍ଥିତି Result::NotSupported:
		ଫେରନ୍ତୁ "not supported on this hardware";

	ସ୍ଥିତି Result::OpenFailed:
		ଫେରନ୍ତୁ "cannot open bootloader file";

	ସ୍ଥିତି Result::TooLarge:
		ଫେରନ୍ତୁ "bootloader file too large";

	ସ୍ଥିତି Result::NoMemory:
		ଫେରନ୍ତୁ "out of memory";

	ସ୍ଥିତି Result::ReadFailed:
		ଫେରନ୍ତୁ "bootloader file read error";

	ସ୍ଥିତି Result::InvalidImage:
		ଫେରନ୍ତୁ "not a bootloader image";

	ସ୍ଥିତି Result::EraseFailed:
		ଫେରନ୍ତୁ "flash erase failed, retry before rebooting";

	ସ୍ଥିତି Result::ProgramFailed:
		ଫେରନ୍ତୁ "flash program failed, retry before rebooting";

	ସ୍ଥିତି Result::VerifyFailed:
		ଫେରନ୍ତୁ "verify failed, retry before rebooting";
	}

	ଫେରନ୍ତୁ "unknown error";
}

} // namespace bl_update
