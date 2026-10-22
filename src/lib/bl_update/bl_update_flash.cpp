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

#यदि defined(BL_UPDATE_WATCHDOG)
#  include <drivers/drv_watchdog.h>
#अन्यथा
#  define watchdog_pet()
#endif

#यदि defined(CONFIG_ARCH_CHIP_STM32H7)
#  define BL_FILE_SIZE_LIMIT	128*1024
#  define STM_RAM_BASE        STM32_AXISRAM_BASE
#  define PAGE_SIZE_MATTERS   1
#अथवायदि defined(CONFIG_ARCH_CHIP_STM32F7)
#  define BL_FILE_SIZE_LIMIT	32*1024
#  define STM_RAM_BASE        STM32_SRAM_BASE
#अन्यथा
#  define BL_FILE_SIZE_LIMIT  16384
#  define STM_RAM_BASE        STM32_SRAM_BASE
#endif

नामाकाश bl_update
{

#यदि defined (CONFIG_STM32_STM32F4XXX) || defined (CONFIG_ARCH_CHIP_STM32F7) || \
    defined (CONFIG_ARCH_CHIP_STM32H7)

स्थिर Result write_flash(अचर uint8_t *buf, माप_प्रकार image_size)
{
	uint8_t *base = (uint8_t *) PX4_FLASH_BASE;

	यदि (स_भेद(base, buf, image_size) == 0) {
		फर्काऊ Result::Unchanged;
	}

	/* prevent other tasks from running while we do this */
	sched_lock();

	/* The IWDG may already be running (CAN nodes start it in the bootloader)
	 * and nothing else gets to pet it while the scheduler is locked. */
	watchdog_pet();
	अचर तार्किक erased = up_progmem_eraseblock(0) >= 0;
	watchdog_pet();

	/* Program even if the erase reported an error: the old bootloader is
	 * already damaged, and a failed program reports it below. */
	अचर ssize_t size = up_progmem_write((माप_प्रकार) base, buf, image_size);
	watchdog_pet();

	/* re-lock the flash control register */
	stm32_flash_lock();

	sched_unlock();

	यदि (size != (ssize_t) image_size) {
		फर्काऊ erased ? Result::ProgramFailed : Result::EraseFailed;
	}

	यदि (स_भेद(base, buf, image_size) != 0) {
		फर्काऊ Result::VerifyFailed;
	}

	फर्काऊ Result::Updated;
}

Result flash(अचर वर्ण *path)
{
	पूर्णाङ्क fd = खोलो(path, O_RDONLY);

	यदि (fd < 0) {
		फर्काऊ Result::OpenFailed;
	}

	संरचना stat s;

	यदि (ख_स्थिति(fd, &s) != 0) {
		बंद(fd);
		फर्काऊ Result::OpenFailed;
	}

	यदि (s.st_size > BL_FILE_SIZE_LIMIT) {
		बंद(fd);
		फर्काऊ Result::TooLarge;
	}

	अचर माप_प्रकार file_size = s.st_size;

	/* up_progmem_write() rejects a size that is not a multiple of its
	 * program unit, and by then the old bootloader is erased. */
#यदि defined(PAGE_SIZE_MATTERS)
	अचर माप_प्रकार align_mask = up_progmem_pagesize(0) - 1;
#अन्यथा
	अचर माप_प्रकार align_mask = परिमाण(uint32_t) - 1;
#endif
	अचर माप_प्रकार image_size = (file_size + align_mask) & ~align_mask;

	uint8_t *buf = (uint8_t *)दो_स्मृति(image_size);

	यदि (buf == nullptr) {
		बंद(fd);
		फर्काऊ Result::NoMemory;
	}

	स_रखो(buf, 0xff, image_size);

	अचर ssize_t bytes_read = पठ(fd, buf, file_size);
	बंद(fd);

	यदि (bytes_read != (ssize_t) file_size) {
		मुक्त(buf);
		फर्काऊ Result::ReadFailed;
	}

	अचर uint32_t *hdr = (अचर uint32_t *)buf;

	यदि ((hdr[0] < STM_RAM_BASE) ||			/* stack not below RAM */
	    (hdr[0] > (STM_RAM_BASE + (128 * 1024))) ||	/* stack not above RAM */
	    (hdr[1] < PX4_FLASH_BASE) ||			/* entrypoint not below flash */
	    ((hdr[1] - PX4_FLASH_BASE) > BL_FILE_SIZE_LIMIT)) {	/* entrypoint not outside bootloader */
		मुक्त(buf);
		फर्काऊ Result::InvalidImage;
	}

	अचर Result result = write_flash(buf, image_size);
	मुक्त(buf);
	फर्काऊ result;
}

#अन्यथा

Result flash(अचर वर्ण *path)
{
	फर्काऊ Result::NotSupported;
}

#endif

अचर वर्ण *result_str(Result result)
{
	विकल्प (result) {
	पक्ष Result::Updated:
		फर्काऊ "bootloader updated";

	पक्ष Result::Unchanged:
		फर्काऊ "bootloader unchanged";

	पक्ष Result::NotSupported:
		फर्काऊ "not supported on this hardware";

	पक्ष Result::OpenFailed:
		फर्काऊ "cannot open bootloader file";

	पक्ष Result::TooLarge:
		फर्काऊ "bootloader file too large";

	पक्ष Result::NoMemory:
		फर्काऊ "out of memory";

	पक्ष Result::ReadFailed:
		फर्काऊ "bootloader file read error";

	पक्ष Result::InvalidImage:
		फर्काऊ "not a bootloader image";

	पक्ष Result::EraseFailed:
		फर्काऊ "flash erase failed, retry before rebooting";

	पक्ष Result::ProgramFailed:
		फर्काऊ "flash program failed, retry before rebooting";

	पक्ष Result::VerifyFailed:
		फर्काऊ "verify failed, retry before rebooting";
	}

	फर्काऊ "unknown error";
}

} // namespace bl_update
