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

#जर defined(BL_UPDATE_WATCHDOG)
#  include <drivers/drv_watchdog.h>
#अन्यथा
#  define watchdog_pet()
#endif

#जर defined(CONFIG_ARCH_CHIP_STM32H7)
#  define BL_FILE_SIZE_LIMIT	128*1024
#  define STM_RAM_BASE        STM32_AXISRAM_BASE
#  define PAGE_SIZE_MATTERS   1
#किंवाजर defined(CONFIG_ARCH_CHIP_STM32F7)
#  define BL_FILE_SIZE_LIMIT	32*1024
#  define STM_RAM_BASE        STM32_SRAM_BASE
#अन्यथा
#  define BL_FILE_SIZE_LIMIT  16384
#  define STM_RAM_BASE        STM32_SRAM_BASE
#endif

नामाकाश bl_update
{

#जर defined (CONFIG_STM32_STM32F4XXX) || defined (CONFIG_ARCH_CHIP_STM32F7) || \
    defined (CONFIG_ARCH_CHIP_STM32H7)

स्थिर Result write_flash(स्थिरांक uint8_t *buf, माप_प्रकार image_size)
{
	uint8_t *base = (uint8_t *) PX4_FLASH_BASE;

	जर (स_भेद(base, buf, image_size) == 0) {
		परत Result::Unchanged;
	}

	/* prevent other tasks from running while we do this */
	sched_lock();

	/* The IWDG may already be running (CAN nodes start it in the bootloader)
	 * and nothing else gets to pet it while the scheduler is locked. */
	watchdog_pet();
	स्थिरांक तार्किक erased = up_progmem_eraseblock(0) >= 0;
	watchdog_pet();

	/* Program even if the erase reported an error: the old bootloader is
	 * already damaged, and a failed program reports it below. */
	स्थिरांक ssize_t size = up_progmem_write((माप_प्रकार) base, buf, image_size);
	watchdog_pet();

	/* re-lock the flash control register */
	stm32_flash_lock();

	sched_unlock();

	जर (size != (ssize_t) image_size) {
		परत erased ? Result::ProgramFailed : Result::EraseFailed;
	}

	जर (स_भेद(base, buf, image_size) != 0) {
		परत Result::VerifyFailed;
	}

	परत Result::Updated;
}

Result flash(स्थिरांक अक्षर *path)
{
	पूर्णांक fd = खोलो(path, O_RDONLY);

	जर (fd < 0) {
		परत Result::OpenFailed;
	}

	संरचना stat s;

	जर (ख_स्थिति(fd, &s) != 0) {
		बंद(fd);
		परत Result::OpenFailed;
	}

	जर (s.st_size > BL_FILE_SIZE_LIMIT) {
		बंद(fd);
		परत Result::TooLarge;
	}

	स्थिरांक माप_प्रकार file_size = s.st_size;

	/* up_progmem_write() rejects a size that is not a multiple of its
	 * program unit, and by then the old bootloader is erased. */
#जर defined(PAGE_SIZE_MATTERS)
	स्थिरांक माप_प्रकार align_mask = up_progmem_pagesize(0) - 1;
#अन्यथा
	स्थिरांक माप_प्रकार align_mask = आकार(uint32_t) - 1;
#endif
	स्थिरांक माप_प्रकार image_size = (file_size + align_mask) & ~align_mask;

	uint8_t *buf = (uint8_t *)दो_स्मृति(image_size);

	जर (buf == nullptr) {
		बंद(fd);
		परत Result::NoMemory;
	}

	स_रखो(buf, 0xff, image_size);

	स्थिरांक ssize_t bytes_read = पठ(fd, buf, file_size);
	बंद(fd);

	जर (bytes_read != (ssize_t) file_size) {
		मुक्त(buf);
		परत Result::ReadFailed;
	}

	स्थिरांक uint32_t *hdr = (स्थिरांक uint32_t *)buf;

	जर ((hdr[0] < STM_RAM_BASE) ||			/* stack not below RAM */
	    (hdr[0] > (STM_RAM_BASE + (128 * 1024))) ||	/* stack not above RAM */
	    (hdr[1] < PX4_FLASH_BASE) ||			/* entrypoint not below flash */
	    ((hdr[1] - PX4_FLASH_BASE) > BL_FILE_SIZE_LIMIT)) {	/* entrypoint not outside bootloader */
		मुक्त(buf);
		परत Result::InvalidImage;
	}

	स्थिरांक Result result = write_flash(buf, image_size);
	मुक्त(buf);
	परत result;
}

#अन्यथा

Result flash(स्थिरांक अक्षर *path)
{
	परत Result::NotSupported;
}

#endif

स्थिरांक अक्षर *result_str(Result result)
{
	निवड (result) {
	स्थिती Result::Updated:
		परत "bootloader updated";

	स्थिती Result::Unchanged:
		परत "bootloader unchanged";

	स्थिती Result::NotSupported:
		परत "not supported on this hardware";

	स्थिती Result::OpenFailed:
		परत "cannot open bootloader file";

	स्थिती Result::TooLarge:
		परत "bootloader file too large";

	स्थिती Result::NoMemory:
		परत "out of memory";

	स्थिती Result::ReadFailed:
		परत "bootloader file read error";

	स्थिती Result::InvalidImage:
		परत "not a bootloader image";

	स्थिती Result::EraseFailed:
		परत "flash erase failed, retry before rebooting";

	स्थिती Result::ProgramFailed:
		परत "flash program failed, retry before rebooting";

	स्थिती Result::VerifyFailed:
		परत "verify failed, retry before rebooting";
	}

	परत "unknown error";
}

} // namespace bl_update
