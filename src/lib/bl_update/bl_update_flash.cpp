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
#د_که_پای

#إذا defined(CONFIG_ARCH_CHIP_STM32H7)
#  define BL_FILE_اندازه_حد	128*1024
#  define STM_RAM_بنسټ        STM32_AXISRAM_بنسټ
#  define پاڼه_اندازه_MATTERS   1
#وإلاإذا defined(CONFIG_ARCH_CHIP_STM32F7)
#  define BL_FILE_اندازه_حد	32*1024
#  define STM_RAM_بنسټ        STM32_SRAM_بنسټ
#وإلا
#  define BL_FILE_اندازه_حد  16384
#  define STM_RAM_بنسټ        STM32_SRAM_بنسټ
#د_که_پای

نطاق bl_update
{

#إذا defined (CONFIG_STM32_STM32F4XXX) || defined (CONFIG_ARCH_CHIP_STM32F7) || \
    defined (CONFIG_ARCH_CHIP_STM32H7)

ساكن پایله_2 write_flash(ثابت uint8_t *buf, size_t image_اندازه)
{
	uint8_t *base = (uint8_t *) PX4_FLASH_بنسټ;

	إذا (memcmp(base, buf, image_اندازه) == 0) {
		أرجع پایله_2::Unchanged;
	}

	/* prevent other tasks from running while we do this */
	sched_قلف();

	/* The IWDG may already be running (CAN nodes start it in the bootloader)
	 * and nothing else gets to pet it while the scheduler is locked. */
	watchdog_pet();
	ثابت منطقي erased = پورته_progmem_eraseblock(0) >= 0;
	watchdog_pet();

	/* Program even if the erase reported an error: the old bootloader is
	 * already damaged, and a failed program reports it below. */
	ثابت ssize_t اندازه = پورته_progmem_write((size_t) base, buf, image_اندازه);
	watchdog_pet();

	/* re-lock the flash control register */
	stm32_flash_قلف();

	sched_unlock();

	إذا (اندازه != (ssize_t) image_اندازه) {
		أرجع erased ? پایله_2::پروګرام_Failed : پایله_2::EraseFailed;
	}

	إذا (memcmp(base, buf, image_اندازه) != 0) {
		أرجع پایله_2::VerifyFailed;
	}

	أرجع پایله_2::Updated;
}

پایله_2 flash(ثابت حرف *لار)
{
	صحيح fd = open(لار, O_RDONLY);

	إذا (fd < 0) {
		أرجع پایله_2::OpenFailed;
	}

	بنية stat s;

	إذا (fstat(fd, &s) != 0) {
		close(fd);
		أرجع پایله_2::OpenFailed;
	}

	إذا (s.st_size > BL_FILE_اندازه_حد) {
		close(fd);
		أرجع پایله_2::TooLarge;
	}

	ثابت size_t file_اندازه = s.st_size;

	/* up_progmem_write() rejects a size that is not a multiple of its
	 * program unit, and by then the old bootloader is erased. */
#إذا defined(پاڼه_اندازه_MATTERS)
	ثابت size_t align_ماسک = پورته_progmem_pagesize(0) - 1;
#وإلا
	ثابت size_t align_ماسک = حجم(uint32_t) - 1;
#د_که_پای
	ثابت size_t image_اندازه = (file_اندازه + align_ماسک) & ~align_ماسک;

	uint8_t *buf = (uint8_t *)حافظه_ورکړه(image_اندازه);

	إذا (buf == تش_اشاره) {
		close(fd);
		أرجع پایله_2::No_حافظه;
	}

	memset(buf, 0xff, image_اندازه);

	ثابت ssize_t بایټونه_read = اقرأ(fd, buf, file_اندازه);
	close(fd);

	إذا (بایټونه_read != (ssize_t) file_اندازه) {
		حرر(buf);
		أرجع پایله_2::ReadFailed;
	}

	ثابت uint32_t *hdr = (ثابت uint32_t *)buf;

	إذا ((hdr[0] < STM_RAM_بنسټ) ||			/* stack not below RAM */
	    (hdr[0] > (STM_RAM_بنسټ + (128 * 1024))) ||	/* stack not above RAM */
	    (hdr[1] < PX4_FLASH_بنسټ) ||			/* entrypoint not below flash */
	    ((hdr[1] - PX4_FLASH_بنسټ) > BL_FILE_اندازه_حد)) {	/* entrypoint not outside bootloader */
		حرر(buf);
		أرجع پایله_2::InvalidImage;
	}

	ثابت پایله_2 پایله = write_flash(buf, image_اندازه);
	حرر(buf);
	أرجع پایله;
}

#وإلا

پایله_2 flash(ثابت حرف *لار)
{
	أرجع پایله_2::NotSupported;
}

#د_که_پای

ثابت حرف *پایله_str(پایله_2 پایله)
{
	بدل (پایله) {
	حالة پایله_2::Updated:
		أرجع "bootloader updated";

	حالة پایله_2::Unchanged:
		أرجع "bootloader unchanged";

	حالة پایله_2::NotSupported:
		أرجع "not supported on this hardware";

	حالة پایله_2::OpenFailed:
		أرجع "cannot open bootloader file";

	حالة پایله_2::TooLarge:
		أرجع "bootloader file too large";

	حالة پایله_2::No_حافظه:
		أرجع "out of memory";

	حالة پایله_2::ReadFailed:
		أرجع "bootloader file read error";

	حالة پایله_2::InvalidImage:
		أرجع "not a bootloader image";

	حالة پایله_2::EraseFailed:
		أرجع "flash erase failed, retry before rebooting";

	حالة پایله_2::پروګرام_Failed:
		أرجع "flash program failed, retry before rebooting";

	حالة پایله_2::VerifyFailed:
		أرجع "verify failed, retry before rebooting";
	}

	أرجع "unknown error";
}

} // namespace bl_update
