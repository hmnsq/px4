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

#شامل "bl_update_flash.h"

#شامل <px4_platform_common/px4_config.h>

#شامل <stdint.h>
#شامل <stdlib.h>
#شامل <string.h>
#شامل <unistd.h>
#شامل <fcntl.h>
#شامل <sched.h>
#شامل <sys/stat.h>

#شامل <arch/board/board.h>

#شامل <nuttx/progmem.h>

#اگر defined(BL_UPDATE_WATCHDOG)
#  include <drivers/drv_watchdog.h>
#ورنہ
#  define watchdog_pet()
#اگر_ختم

#اگر defined(CONFIG_ARCH_CHIP_STM32H7)
#  define BL_FILE_سائز_حد_بندی	128*1024
#  define STM_RAM_بنیاد        STM32_AXISRAM_بنیاد
#  define صفحہ_سائز_MATTERS   1
#وگرنہ_اگر defined(CONFIG_ARCH_CHIP_STM32F7)
#  define BL_FILE_سائز_حد_بندی	32*1024
#  define STM_RAM_بنیاد        STM32_SRAM_بنیاد
#ورنہ
#  define BL_FILE_سائز_حد_بندی  16384
#  define STM_RAM_بنیاد        STM32_SRAM_بنیاد
#اگر_ختم

نامخلا bl_update
{

#اگر defined (CONFIG_STM32_STM32F4XXX) || defined (CONFIG_ARCH_CHIP_STM32F7) || \
    defined (CONFIG_ARCH_CHIP_STM32H7)

ساکن نتیجہ_2 write_flash(ثابت uint8_t *buf, size_t image_سائز)
{
	uint8_t *base = (uint8_t *) PX4_FLASH_بنیاد;

	اگر (memcmp(base, buf, image_سائز) == 0) {
		واپس نتیجہ_2::Unchanged;
	}

	/* prevent other tasks from running while we do this */
	sched_تالا();

	/* The IWDG may already be running (CAN nodes start it in the bootloader)
	 * and nothing else gets to pet it while the scheduler is locked. */
	watchdog_pet();
	ثابت منطقی erased = اوپر_progmem_eraseblock(0) >= 0;
	watchdog_pet();

	/* Program even if the erase reported an error: the old bootloader is
	 * already damaged, and a failed program reports it below. */
	ثابت ssize_t سائز = اوپر_progmem_write((size_t) base, buf, image_سائز);
	watchdog_pet();

	/* re-lock the flash control register */
	stm32_flash_تالا();

	sched_unlock();

	اگر (سائز != (ssize_t) image_سائز) {
		واپس erased ? نتیجہ_2::پروگرام_Failed : نتیجہ_2::EraseFailed;
	}

	اگر (memcmp(base, buf, image_سائز) != 0) {
		واپس نتیجہ_2::VerifyFailed;
	}

	واپس نتیجہ_2::Updated;
}

نتیجہ_2 flash(ثابت حرف *راستہ)
{
	صحیح fd = open(راستہ, O_RDONLY);

	اگر (fd < 0) {
		واپس نتیجہ_2::OpenFailed;
	}

	ساخت stat s;

	اگر (fstat(fd, &s) != 0) {
		close(fd);
		واپس نتیجہ_2::OpenFailed;
	}

	اگر (s.st_size > BL_FILE_سائز_حد_بندی) {
		close(fd);
		واپس نتیجہ_2::TooLarge;
	}

	ثابت size_t file_سائز = s.st_size;

	/* up_progmem_write() rejects a size that is not a multiple of its
	 * program unit, and by then the old bootloader is erased. */
#اگر defined(صفحہ_سائز_MATTERS)
	ثابت size_t align_نقاب = اوپر_progmem_pagesize(0) - 1;
#ورنہ
	ثابت size_t align_نقاب = حجم(uint32_t) - 1;
#اگر_ختم
	ثابت size_t image_سائز = (file_سائز + align_نقاب) & ~align_نقاب;

	uint8_t *buf = (uint8_t *)حافظہ_دو(image_سائز);

	اگر (buf == خالی_اشاریہ) {
		close(fd);
		واپس نتیجہ_2::No_حافظہ;
	}

	memset(buf, 0xff, image_سائز);

	ثابت ssize_t بائٹیں_read = اقرأ(fd, buf, file_سائز);
	close(fd);

	اگر (بائٹیں_read != (ssize_t) file_سائز) {
		آزاد(buf);
		واپس نتیجہ_2::ReadFailed;
	}

	ثابت uint32_t *hdr = (ثابت uint32_t *)buf;

	اگر ((hdr[0] < STM_RAM_بنیاد) ||			/* stack not below RAM */
	    (hdr[0] > (STM_RAM_بنیاد + (128 * 1024))) ||	/* stack not above RAM */
	    (hdr[1] < PX4_FLASH_بنیاد) ||			/* entrypoint not below flash */
	    ((hdr[1] - PX4_FLASH_بنیاد) > BL_FILE_سائز_حد_بندی)) {	/* entrypoint not outside bootloader */
		آزاد(buf);
		واپس نتیجہ_2::InvalidImage;
	}

	ثابت نتیجہ_2 نتیجہ = write_flash(buf, image_سائز);
	آزاد(buf);
	واپس نتیجہ;
}

#ورنہ

نتیجہ_2 flash(ثابت حرف *راستہ)
{
	واپس نتیجہ_2::NotSupported;
}

#اگر_ختم

ثابت حرف *نتیجہ_str(نتیجہ_2 نتیجہ)
{
	انتخاب (نتیجہ) {
	حالت نتیجہ_2::Updated:
		واپس "bootloader updated";

	حالت نتیجہ_2::Unchanged:
		واپس "bootloader unchanged";

	حالت نتیجہ_2::NotSupported:
		واپس "not supported on this hardware";

	حالت نتیجہ_2::OpenFailed:
		واپس "cannot open bootloader file";

	حالت نتیجہ_2::TooLarge:
		واپس "bootloader file too large";

	حالت نتیجہ_2::No_حافظہ:
		واپس "out of memory";

	حالت نتیجہ_2::ReadFailed:
		واپس "bootloader file read error";

	حالت نتیجہ_2::InvalidImage:
		واپس "not a bootloader image";

	حالت نتیجہ_2::EraseFailed:
		واپس "flash erase failed, retry before rebooting";

	حالت نتیجہ_2::پروگرام_Failed:
		واپس "flash program failed, retry before rebooting";

	حالت نتیجہ_2::VerifyFailed:
		واپس "verify failed, retry before rebooting";
	}

	واپس "unknown error";
}

} // namespace bl_update
