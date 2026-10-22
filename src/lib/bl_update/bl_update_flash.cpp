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

#ضمن "bl_update_flash.h"

#ضمن <px4_platform_common/px4_config.h>

#ضمن <stdint.h>
#ضمن <stdlib.h>
#ضمن <string.h>
#ضمن <unistd.h>
#ضمن <fcntl.h>
#ضمن <sched.h>
#ضمن <sys/stat.h>

#ضمن <arch/board/board.h>

#ضمن <nuttx/progmem.h>

#إذا defined(BL_UPDATE_WATCHDOG)
#  include <drivers/drv_watchdog.h>
#وإلا
#  define watchdog_pet()
#جيڪڏهن_پورو

#إذا defined(CONFIG_ARCH_CHIP_STM32H7)
#  define BL_FILE_سائيز_حد_بندي	128*1024
#  define STM_RAM_بنياد        STM32_AXISRAM_بنياد
#  define صفحو_سائيز_MATTERS   1
#وإلاإذا defined(CONFIG_ARCH_CHIP_STM32F7)
#  define BL_FILE_سائيز_حد_بندي	32*1024
#  define STM_RAM_بنياد        STM32_SRAM_بنياد
#وإلا
#  define BL_FILE_سائيز_حد_بندي  16384
#  define STM_RAM_بنياد        STM32_SRAM_بنياد
#جيڪڏهن_پورو

نطاق bl_update
{

#إذا defined (CONFIG_STM32_STM32F4XXX) || defined (CONFIG_ARCH_CHIP_STM32F7) || \
    defined (CONFIG_ARCH_CHIP_STM32H7)

ساكن نتيجو_2 write_flash(ثابت uint8_t *buf, size_t image_سائيز)
{
	uint8_t *base = (uint8_t *) PX4_FLASH_بنياد;

	إذا (memcmp(base, buf, image_سائيز) == 0) {
		أرجع نتيجو_2::Unchanged;
	}

	/* prevent other tasks from running while we do this */
	sched_تالو();

	/* The IWDG may already be running (CAN nodes start it in the bootloader)
	 * and nothing else gets to pet it while the scheduler is locked. */
	watchdog_pet();
	ثابت منطقي erased = مٿي_progmem_eraseblock(0) >= 0;
	watchdog_pet();

	/* Program even if the erase reported an error: the old bootloader is
	 * already damaged, and a failed program reports it below. */
	ثابت ssize_t سائيز = مٿي_progmem_write((size_t) base, buf, image_سائيز);
	watchdog_pet();

	/* re-lock the flash control register */
	stm32_flash_تالو();

	sched_unlock();

	إذا (سائيز != (ssize_t) image_سائيز) {
		أرجع erased ? نتيجو_2::پروگرام_Failed : نتيجو_2::EraseFailed;
	}

	إذا (memcmp(base, buf, image_سائيز) != 0) {
		أرجع نتيجو_2::VerifyFailed;
	}

	أرجع نتيجو_2::Updated;
}

نتيجو_2 flash(ثابت حرف *رستو)
{
	صحيح fd = open(رستو, O_RDONLY);

	إذا (fd < 0) {
		أرجع نتيجو_2::OpenFailed;
	}

	بنية stat s;

	إذا (fstat(fd, &s) != 0) {
		close(fd);
		أرجع نتيجو_2::OpenFailed;
	}

	إذا (s.st_size > BL_FILE_سائيز_حد_بندي) {
		close(fd);
		أرجع نتيجو_2::TooLarge;
	}

	ثابت size_t file_سائيز = s.st_size;

	/* up_progmem_write() rejects a size that is not a multiple of its
	 * program unit, and by then the old bootloader is erased. */
#إذا defined(صفحو_سائيز_MATTERS)
	ثابت size_t align_نقاب = مٿي_progmem_pagesize(0) - 1;
#وإلا
	ثابت size_t align_نقاب = حجم(uint32_t) - 1;
#جيڪڏهن_پورو
	ثابت size_t image_سائيز = (file_سائيز + align_نقاب) & ~align_نقاب;

	uint8_t *buf = (uint8_t *)يادگيري_ڏيو(image_سائيز);

	إذا (buf == خالي_اشارو) {
		close(fd);
		أرجع نتيجو_2::No_يادگيري;
	}

	memset(buf, 0xff, image_سائيز);

	ثابت ssize_t بائيٽون_read = اقرأ(fd, buf, file_سائيز);
	close(fd);

	إذا (بائيٽون_read != (ssize_t) file_سائيز) {
		حرر(buf);
		أرجع نتيجو_2::ReadFailed;
	}

	ثابت uint32_t *hdr = (ثابت uint32_t *)buf;

	إذا ((hdr[0] < STM_RAM_بنياد) ||			/* stack not below RAM */
	    (hdr[0] > (STM_RAM_بنياد + (128 * 1024))) ||	/* stack not above RAM */
	    (hdr[1] < PX4_FLASH_بنياد) ||			/* entrypoint not below flash */
	    ((hdr[1] - PX4_FLASH_بنياد) > BL_FILE_سائيز_حد_بندي)) {	/* entrypoint not outside bootloader */
		حرر(buf);
		أرجع نتيجو_2::InvalidImage;
	}

	ثابت نتيجو_2 نتيجو = write_flash(buf, image_سائيز);
	حرر(buf);
	أرجع نتيجو;
}

#وإلا

نتيجو_2 flash(ثابت حرف *رستو)
{
	أرجع نتيجو_2::NotSupported;
}

#جيڪڏهن_پورو

ثابت حرف *نتيجو_str(نتيجو_2 نتيجو)
{
	بدل (نتيجو) {
	حالة نتيجو_2::Updated:
		أرجع "bootloader updated";

	حالة نتيجو_2::Unchanged:
		أرجع "bootloader unchanged";

	حالة نتيجو_2::NotSupported:
		أرجع "not supported on this hardware";

	حالة نتيجو_2::OpenFailed:
		أرجع "cannot open bootloader file";

	حالة نتيجو_2::TooLarge:
		أرجع "bootloader file too large";

	حالة نتيجو_2::No_يادگيري:
		أرجع "out of memory";

	حالة نتيجو_2::ReadFailed:
		أرجع "bootloader file read error";

	حالة نتيجو_2::InvalidImage:
		أرجع "not a bootloader image";

	حالة نتيجو_2::EraseFailed:
		أرجع "flash erase failed, retry before rebooting";

	حالة نتيجو_2::پروگرام_Failed:
		أرجع "flash program failed, retry before rebooting";

	حالة نتيجو_2::VerifyFailed:
		أرجع "verify failed, retry before rebooting";
	}

	أرجع "unknown error";
}

} // namespace bl_update
