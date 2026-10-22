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
#جے_انت

#إذا defined(CONFIG_ARCH_CHIP_STM32H7)
#  define BL_FILE_ناپ_حد_بندی	128*1024
#  define STM_RAM_ادھار        STM32_AXISRAM_ادھار
#  define ورقہ_ناپ_MATTERS   1
#وإلاإذا defined(CONFIG_ARCH_CHIP_STM32F7)
#  define BL_FILE_ناپ_حد_بندی	32*1024
#  define STM_RAM_ادھار        STM32_SRAM_ادھار
#وإلا
#  define BL_FILE_ناپ_حد_بندی  16384
#  define STM_RAM_ادھار        STM32_SRAM_ادھار
#جے_انت

نطاق bl_update
{

#إذا defined (CONFIG_STM32_STM32F4XXX) || defined (CONFIG_ARCH_CHIP_STM32F7) || \
    defined (CONFIG_ARCH_CHIP_STM32H7)

ساكن نتیجہ_2 write_flash(ثابت uint8_t *buf, size_t image_ناپ)
{
	uint8_t *base = (uint8_t *) PX4_FLASH_ادھار;

	إذا (memcmp(base, buf, image_ناپ) == 0) {
		أرجع نتیجہ_2::Unchanged;
	}

	/* prevent other tasks from running while we do this */
	sched_جندرا();

	/* The IWDG may already be running (CAN nodes start it in the bootloader)
	 * and nothing else gets to pet it while the scheduler is locked. */
	watchdog_pet();
	ثابت منطقي erased = اتے_progmem_eraseblock(0) >= 0;
	watchdog_pet();

	/* Program even if the erase reported an error: the old bootloader is
	 * already damaged, and a failed program reports it below. */
	ثابت ssize_t ناپ = اتے_progmem_write((size_t) base, buf, image_ناپ);
	watchdog_pet();

	/* re-lock the flash control register */
	stm32_flash_جندرا();

	sched_unlock();

	إذا (ناپ != (ssize_t) image_ناپ) {
		أرجع erased ? نتیجہ_2::پروگرام_Failed : نتیجہ_2::EraseFailed;
	}

	إذا (memcmp(base, buf, image_ناپ) != 0) {
		أرجع نتیجہ_2::VerifyFailed;
	}

	أرجع نتیجہ_2::Updated;
}

نتیجہ_2 flash(ثابت حرف *رستہ)
{
	صحيح fd = open(رستہ, O_RDONLY);

	إذا (fd < 0) {
		أرجع نتیجہ_2::OpenFailed;
	}

	بنية stat s;

	إذا (fstat(fd, &s) != 0) {
		close(fd);
		أرجع نتیجہ_2::OpenFailed;
	}

	إذا (s.st_size > BL_FILE_ناپ_حد_بندی) {
		close(fd);
		أرجع نتیجہ_2::TooLarge;
	}

	ثابت size_t file_ناپ = s.st_size;

	/* up_progmem_write() rejects a size that is not a multiple of its
	 * program unit, and by then the old bootloader is erased. */
#إذا defined(ورقہ_ناپ_MATTERS)
	ثابت size_t align_نقاب = اتے_progmem_pagesize(0) - 1;
#وإلا
	ثابت size_t align_نقاب = حجم(uint32_t) - 1;
#جے_انت
	ثابت size_t image_ناپ = (file_ناپ + align_نقاب) & ~align_نقاب;

	uint8_t *buf = (uint8_t *)حافظہ_دیو(image_ناپ);

	إذا (buf == خالی_سوچک) {
		close(fd);
		أرجع نتیجہ_2::No_حافظہ;
	}

	memset(buf, 0xff, image_ناپ);

	ثابت ssize_t بائٹاں_read = اقرأ(fd, buf, file_ناپ);
	close(fd);

	إذا (بائٹاں_read != (ssize_t) file_ناپ) {
		حرر(buf);
		أرجع نتیجہ_2::ReadFailed;
	}

	ثابت uint32_t *hdr = (ثابت uint32_t *)buf;

	إذا ((hdr[0] < STM_RAM_ادھار) ||			/* stack not below RAM */
	    (hdr[0] > (STM_RAM_ادھار + (128 * 1024))) ||	/* stack not above RAM */
	    (hdr[1] < PX4_FLASH_ادھار) ||			/* entrypoint not below flash */
	    ((hdr[1] - PX4_FLASH_ادھار) > BL_FILE_ناپ_حد_بندی)) {	/* entrypoint not outside bootloader */
		حرر(buf);
		أرجع نتیجہ_2::InvalidImage;
	}

	ثابت نتیجہ_2 نتیجہ = write_flash(buf, image_ناپ);
	حرر(buf);
	أرجع نتیجہ;
}

#وإلا

نتیجہ_2 flash(ثابت حرف *رستہ)
{
	أرجع نتیجہ_2::NotSupported;
}

#جے_انت

ثابت حرف *نتیجہ_str(نتیجہ_2 نتیجہ)
{
	بدل (نتیجہ) {
	حالة نتیجہ_2::Updated:
		أرجع "bootloader updated";

	حالة نتیجہ_2::Unchanged:
		أرجع "bootloader unchanged";

	حالة نتیجہ_2::NotSupported:
		أرجع "not supported on this hardware";

	حالة نتیجہ_2::OpenFailed:
		أرجع "cannot open bootloader file";

	حالة نتیجہ_2::TooLarge:
		أرجع "bootloader file too large";

	حالة نتیجہ_2::No_حافظہ:
		أرجع "out of memory";

	حالة نتیجہ_2::ReadFailed:
		أرجع "bootloader file read error";

	حالة نتیجہ_2::InvalidImage:
		أرجع "not a bootloader image";

	حالة نتیجہ_2::EraseFailed:
		أرجع "flash erase failed, retry before rebooting";

	حالة نتیجہ_2::پروگرام_Failed:
		أرجع "flash program failed, retry before rebooting";

	حالة نتیجہ_2::VerifyFailed:
		أرجع "verify failed, retry before rebooting";
	}

	أرجع "unknown error";
}

} // namespace bl_update
