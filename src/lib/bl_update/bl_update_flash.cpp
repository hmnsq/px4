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

#ਅਨ੍ਤਰ੍ਭਾਵ "bl_update_flash.h"

#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/px4_config.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <stdint.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <fcntl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sched.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/stat.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <arch/board/board.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <nuttx/progmem.h>

#ਜੇ defined(BL_UPDATE_WATCHDOG)
#  include <drivers/drv_watchdog.h>
#ਨਹੀਂਤਾਂ
#  define watchdog_pet()
#ਜੇ_ਅੰਤ

#ਜੇ defined(CONFIG_ARCH_CHIP_STM32H7)
#  define BL_FILE_ਮਾਪ_ਹੱਦਬੰਦੀ	128*1024
#  define STM_RAM_ਅਧਾਰ        STM32_AXISRAM_ਅਧਾਰ
#  define ਪੰਨਾ_ਮਾਪ_MATTERS   1
#ਜਾਂਜੇ defined(CONFIG_ARCH_CHIP_STM32F7)
#  define BL_FILE_ਮਾਪ_ਹੱਦਬੰਦੀ	32*1024
#  define STM_RAM_ਅਧਾਰ        STM32_SRAM_ਅਧਾਰ
#ਨਹੀਂਤਾਂ
#  define BL_FILE_ਮਾਪ_ਹੱਦਬੰਦੀ  16384
#  define STM_RAM_ਅਧਾਰ        STM32_SRAM_ਅਧਾਰ
#ਜੇ_ਅੰਤ

ਨਾਮਾਕਾਸ਼ bl_update
{

#ਜੇ defined (CONFIG_STM32_STM32F4XXX) || defined (CONFIG_ARCH_CHIP_STM32F7) || \
    defined (CONFIG_ARCH_CHIP_STM32H7)

ਸਥਿਰ ਨਤੀਜਾ_2 write_flash(ਅਚਲ uint8_t *buf, size_t image_ਮਾਪ)
{
	uint8_t *base = (uint8_t *) PX4_FLASH_ਅਧਾਰ;

	ਜੇ (memcmp(base, buf, image_ਮਾਪ) == 0) {
		ਵਾਪਸ ਨਤੀਜਾ_2::Unchanged;
	}

	/* prevent other tasks from running while we do this */
	sched_ਤਾਲਾ();

	/* The IWDG may already be running (CAN nodes start it in the bootloader)
	 * and nothing else gets to pet it while the scheduler is locked. */
	watchdog_pet();
	ਅਚਲ ਤਾਰਕਿਕ erased = ਉੱਪਰ_progmem_eraseblock(0) >= 0;
	watchdog_pet();

	/* Program even if the erase reported an error: the old bootloader is
	 * already damaged, and a failed program reports it below. */
	ਅਚਲ ssize_t ਮਾਪ = ਉੱਪਰ_progmem_write((size_t) base, buf, image_ਮਾਪ);
	watchdog_pet();

	/* re-lock the flash control register */
	stm32_flash_ਤਾਲਾ();

	sched_unlock();

	ਜੇ (ਮਾਪ != (ssize_t) image_ਮਾਪ) {
		ਵਾਪਸ erased ? ਨਤੀਜਾ_2::ਪ੍ਰੋਗਰਾਮ_Failed : ਨਤੀਜਾ_2::EraseFailed;
	}

	ਜੇ (memcmp(base, buf, image_ਮਾਪ) != 0) {
		ਵਾਪਸ ਨਤੀਜਾ_2::VerifyFailed;
	}

	ਵਾਪਸ ਨਤੀਜਾ_2::Updated;
}

ਨਤੀਜਾ_2 flash(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ)
{
	ਪੂਰਨ fd = open(ਰਸਤਾ, O_RDONLY);

	ਜੇ (fd < 0) {
		ਵਾਪਸ ਨਤੀਜਾ_2::OpenFailed;
	}

	ਬਣਤਰ stat s;

	ਜੇ (fstat(fd, &s) != 0) {
		close(fd);
		ਵਾਪਸ ਨਤੀਜਾ_2::OpenFailed;
	}

	ਜੇ (s.st_size > BL_FILE_ਮਾਪ_ਹੱਦਬੰਦੀ) {
		close(fd);
		ਵਾਪਸ ਨਤੀਜਾ_2::TooLarge;
	}

	ਅਚਲ size_t file_ਮਾਪ = s.st_size;

	/* up_progmem_write() rejects a size that is not a multiple of its
	 * program unit, and by then the old bootloader is erased. */
#ਜੇ defined(ਪੰਨਾ_ਮਾਪ_MATTERS)
	ਅਚਲ size_t align_ਨਕਾਬ = ਉੱਪਰ_progmem_pagesize(0) - 1;
#ਨਹੀਂਤਾਂ
	ਅਚਲ size_t align_ਨਕਾਬ = ਆਕਾਰ(uint32_t) - 1;
#ਜੇ_ਅੰਤ
	ਅਚਲ size_t image_ਮਾਪ = (file_ਮਾਪ + align_ਨਕਾਬ) & ~align_ਨਕਾਬ;

	uint8_t *buf = (uint8_t *)ਸਮ੍ਰਿਤੀ_ਦਿਓ(image_ਮਾਪ);

	ਜੇ (buf == ਖਾਲੀ_ਸੂਚਕ) {
		close(fd);
		ਵਾਪਸ ਨਤੀਜਾ_2::No_ਸਮ੍ਰਿਤੀ;
	}

	memset(buf, 0xff, image_ਮਾਪ);

	ਅਚਲ ssize_t ਬਾਈਟਾਂ_read = ਪਠ(fd, buf, file_ਮਾਪ);
	close(fd);

	ਜੇ (ਬਾਈਟਾਂ_read != (ssize_t) file_ਮਾਪ) {
		ਮੁਕਤ(buf);
		ਵਾਪਸ ਨਤੀਜਾ_2::ReadFailed;
	}

	ਅਚਲ uint32_t *hdr = (ਅਚਲ uint32_t *)buf;

	ਜੇ ((hdr[0] < STM_RAM_ਅਧਾਰ) ||			/* stack not below RAM */
	    (hdr[0] > (STM_RAM_ਅਧਾਰ + (128 * 1024))) ||	/* stack not above RAM */
	    (hdr[1] < PX4_FLASH_ਅਧਾਰ) ||			/* entrypoint not below flash */
	    ((hdr[1] - PX4_FLASH_ਅਧਾਰ) > BL_FILE_ਮਾਪ_ਹੱਦਬੰਦੀ)) {	/* entrypoint not outside bootloader */
		ਮੁਕਤ(buf);
		ਵਾਪਸ ਨਤੀਜਾ_2::InvalidImage;
	}

	ਅਚਲ ਨਤੀਜਾ_2 ਨਤੀਜਾ = write_flash(buf, image_ਮਾਪ);
	ਮੁਕਤ(buf);
	ਵਾਪਸ ਨਤੀਜਾ;
}

#ਨਹੀਂਤਾਂ

ਨਤੀਜਾ_2 flash(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ)
{
	ਵਾਪਸ ਨਤੀਜਾ_2::NotSupported;
}

#ਜੇ_ਅੰਤ

ਅਚਲ ਅੱਖਰ *ਨਤੀਜਾ_str(ਨਤੀਜਾ_2 ਨਤੀਜਾ)
{
	ਚੋਣ (ਨਤੀਜਾ) {
	ਹਾਲਤ ਨਤੀਜਾ_2::Updated:
		ਵਾਪਸ "bootloader updated";

	ਹਾਲਤ ਨਤੀਜਾ_2::Unchanged:
		ਵਾਪਸ "bootloader unchanged";

	ਹਾਲਤ ਨਤੀਜਾ_2::NotSupported:
		ਵਾਪਸ "not supported on this hardware";

	ਹਾਲਤ ਨਤੀਜਾ_2::OpenFailed:
		ਵਾਪਸ "cannot open bootloader file";

	ਹਾਲਤ ਨਤੀਜਾ_2::TooLarge:
		ਵਾਪਸ "bootloader file too large";

	ਹਾਲਤ ਨਤੀਜਾ_2::No_ਸਮ੍ਰਿਤੀ:
		ਵਾਪਸ "out of memory";

	ਹਾਲਤ ਨਤੀਜਾ_2::ReadFailed:
		ਵਾਪਸ "bootloader file read error";

	ਹਾਲਤ ਨਤੀਜਾ_2::InvalidImage:
		ਵਾਪਸ "not a bootloader image";

	ਹਾਲਤ ਨਤੀਜਾ_2::EraseFailed:
		ਵਾਪਸ "flash erase failed, retry before rebooting";

	ਹਾਲਤ ਨਤੀਜਾ_2::ਪ੍ਰੋਗਰਾਮ_Failed:
		ਵਾਪਸ "flash program failed, retry before rebooting";

	ਹਾਲਤ ਨਤੀਜਾ_2::VerifyFailed:
		ਵਾਪਸ "verify failed, retry before rebooting";
	}

	ਵਾਪਸ "unknown error";
}

} // namespace bl_update
