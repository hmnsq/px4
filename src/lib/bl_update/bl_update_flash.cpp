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
#यदि_अन्तः

#यदि defined(CONFIG_ARCH_CHIP_STM32H7)
#  define BL_FILE_आकार_सीमा	128*1024
#  define STM_RAM_आधारः        STM32_AXISRAM_आधारः
#  define PAGE_आकार_MATTERS   1
#नोचेद्यदि defined(CONFIG_ARCH_CHIP_STM32F7)
#  define BL_FILE_आकार_सीमा	32*1024
#  define STM_RAM_आधारः        STM32_SRAM_आधारः
#अन्यथा
#  define BL_FILE_आकार_सीमा  16384
#  define STM_RAM_आधारः        STM32_SRAM_आधारः
#यदि_अन्तः

नामाकाश bl_update
{

#यदि defined (CONFIG_STM32_STM32F4XXX) || defined (CONFIG_ARCH_CHIP_STM32F7) || \
    defined (CONFIG_ARCH_CHIP_STM32H7)

स्थिर परिणाम_2 write_flash(ध्रुव uint8_t *buf, size_t image_आकार)
{
	uint8_t *base = (uint8_t *) PX4_FLASH_आधारः;

	यदि (memcmp(base, buf, image_आकार) == 0) {
		प्रत्यागम परिणाम_2::Unchanged;
	}

	/* prevent other tasks from running while we do this */
	sched_तालकम्();

	/* The IWDG may already be running (CAN nodes start it in the bootloader)
	 * and nothing else gets to pet it while the scheduler is locked. */
	watchdog_pet();
	ध्रुव तार्किक erased = ऊर्ध्वम्_progmem_eraseblock(0) >= 0;
	watchdog_pet();

	/* Program even if the erase reported an error: the old bootloader is
	 * already damaged, and a failed program reports it below. */
	ध्रुव ssize_t आकार = ऊर्ध्वम्_progmem_write((size_t) base, buf, image_आकार);
	watchdog_pet();

	/* re-lock the flash control register */
	stm32_flash_तालकम्();

	sched_unlock();

	यदि (आकार != (ssize_t) image_आकार) {
		प्रत्यागम erased ? परिणाम_2::क्रमादेशः_Failed : परिणाम_2::EraseFailed;
	}

	यदि (memcmp(base, buf, image_आकार) != 0) {
		प्रत्यागम परिणाम_2::VerifyFailed;
	}

	प्रत्यागम परिणाम_2::Updated;
}

परिणाम_2 flash(ध्रुव वर्ण *मार्गः)
{
	पूर्णाङ्क fd = उद्घाटय(मार्गः, O_RDONLY);

	यदि (fd < 0) {
		प्रत्यागम परिणाम_2::OpenFailed;
	}

	संरचना stat s;

	यदि (fstat(fd, &s) != 0) {
		close(fd);
		प्रत्यागम परिणाम_2::OpenFailed;
	}

	यदि (s.st_size > BL_FILE_आकार_सीमा) {
		close(fd);
		प्रत्यागम परिणाम_2::TooLarge;
	}

	ध्रुव size_t file_आकार = s.st_size;

	/* up_progmem_write() rejects a size that is not a multiple of its
	 * program unit, and by then the old bootloader is erased. */
#यदि defined(PAGE_आकार_MATTERS)
	ध्रुव size_t align_आवरणम् = ऊर्ध्वम्_progmem_pagesize(0) - 1;
#अन्यथा
	ध्रुव size_t align_आवरणम् = परिमाण(uint32_t) - 1;
#यदि_अन्तः
	ध्रुव size_t image_आकार = (file_आकार + align_आवरणम्) & ~align_आवरणम्;

	uint8_t *buf = (uint8_t *)स्मृतिं_देहि(image_आकार);

	यदि (buf == शून्यसूचकः) {
		close(fd);
		प्रत्यागम परिणाम_2::No_स्मृतिः;
	}

	स्मृतिं_पूरय(buf, 0xff, image_आकार);

	ध्रुव ssize_t अष्टकानि_read = पठ(fd, buf, file_आकार);
	close(fd);

	यदि (अष्टकानि_read != (ssize_t) file_आकार) {
		मुक्त(buf);
		प्रत्यागम परिणाम_2::ReadFailed;
	}

	ध्रुव uint32_t *hdr = (ध्रुव uint32_t *)buf;

	यदि ((hdr[0] < STM_RAM_आधारः) ||			/* stack not below RAM */
	    (hdr[0] > (STM_RAM_आधारः + (128 * 1024))) ||	/* stack not above RAM */
	    (hdr[1] < PX4_FLASH_आधारः) ||			/* entrypoint not below flash */
	    ((hdr[1] - PX4_FLASH_आधारः) > BL_FILE_आकार_सीमा)) {	/* entrypoint not outside bootloader */
		मुक्त(buf);
		प्रत्यागम परिणाम_2::InvalidImage;
	}

	ध्रुव परिणाम_2 परिणाम = write_flash(buf, image_आकार);
	मुक्त(buf);
	प्रत्यागम परिणाम;
}

#अन्यथा

परिणाम_2 flash(ध्रुव वर्ण *मार्गः)
{
	प्रत्यागम परिणाम_2::NotSupported;
}

#यदि_अन्तः

ध्रुव वर्ण *परिणाम_str(परिणाम_2 परिणाम)
{
	विकल्प (परिणाम) {
	पक्ष परिणाम_2::Updated:
		प्रत्यागम "bootloader updated";

	पक्ष परिणाम_2::Unchanged:
		प्रत्यागम "bootloader unchanged";

	पक्ष परिणाम_2::NotSupported:
		प्रत्यागम "not supported on this hardware";

	पक्ष परिणाम_2::OpenFailed:
		प्रत्यागम "cannot open bootloader file";

	पक्ष परिणाम_2::TooLarge:
		प्रत्यागम "bootloader file too large";

	पक्ष परिणाम_2::No_स्मृतिः:
		प्रत्यागम "out of memory";

	पक्ष परिणाम_2::ReadFailed:
		प्रत्यागम "bootloader file read error";

	पक्ष परिणाम_2::InvalidImage:
		प्रत्यागम "not a bootloader image";

	पक्ष परिणाम_2::EraseFailed:
		प्रत्यागम "flash erase failed, retry before rebooting";

	पक्ष परिणाम_2::क्रमादेशः_Failed:
		प्रत्यागम "flash program failed, retry before rebooting";

	पक्ष परिणाम_2::VerifyFailed:
		प्रत्यागम "verify failed, retry before rebooting";
	}

	प्रत्यागम "unknown error";
}

} // namespace bl_update
