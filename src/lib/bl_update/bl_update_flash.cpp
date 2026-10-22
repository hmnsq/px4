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

#ಆದರೆ defined(BL_UPDATE_WATCHDOG)
#  include <drivers/drv_watchdog.h>
#ಇಲ್ಲದಿದ್ದರೆ
#  define watchdog_pet()
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(CONFIG_ARCH_CHIP_STM32H7)
#  define BL_FILE_ಗಾತ್ರ_ಎಲ್ಲೆ	128*1024
#  define STM_RAM_ತಳ        STM32_AXISRAM_ತಳ
#  define ಪುಟ_ಗಾತ್ರ_MATTERS   1
#ಅಥವಾಆದರೆ defined(CONFIG_ARCH_CHIP_STM32F7)
#  define BL_FILE_ಗಾತ್ರ_ಎಲ್ಲೆ	32*1024
#  define STM_RAM_ತಳ        STM32_SRAM_ತಳ
#ಇಲ್ಲದಿದ್ದರೆ
#  define BL_FILE_ಗಾತ್ರ_ಎಲ್ಲೆ  16384
#  define STM_RAM_ತಳ        STM32_SRAM_ತಳ
#ಆದರೆ_ಅಂತ್ಯ

ನಾಮಾಕಾಶ bl_update
{

#ಆದರೆ defined (CONFIG_STM32_STM32F4XXX) || defined (CONFIG_ARCH_CHIP_STM32F7) || \
    defined (CONFIG_ARCH_CHIP_STM32H7)

ಸ್ಥಿರ ಫಲಿತಾಂಶ_2 write_flash(ಸ್ಥಿರಾಂಕ uint8_t *buf, size_t image_ಗಾತ್ರ)
{
	uint8_t *base = (uint8_t *) PX4_FLASH_ತಳ;

	ಆದರೆ (memcmp(base, buf, image_ಗಾತ್ರ) == 0) {
		ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ_2::Unchanged;
	}

	/* prevent other tasks from running while we do this */
	sched_ಬೀಗ();

	/* The IWDG may already be running (CAN nodes start it in the bootloader)
	 * and nothing else gets to pet it while the scheduler is locked. */
	watchdog_pet();
	ಸ್ಥಿರಾಂಕ ತಾರ್ಕಿಕ erased = ಮೇಲೆ_progmem_eraseblock(0) >= 0;
	watchdog_pet();

	/* Program even if the erase reported an error: the old bootloader is
	 * already damaged, and a failed program reports it below. */
	ಸ್ಥಿರಾಂಕ ssize_t ಗಾತ್ರ_2 = ಮೇಲೆ_progmem_write((size_t) base, buf, image_ಗಾತ್ರ);
	watchdog_pet();

	/* re-lock the flash control register */
	stm32_flash_ಬೀಗ();

	sched_unlock();

	ಆದರೆ (ಗಾತ್ರ_2 != (ssize_t) image_ಗಾತ್ರ) {
		ಹಿಂತಿರುಗಿ erased ? ಫಲಿತಾಂಶ_2::ಕಾರ್ಯಕ್ರಮ_Failed : ಫಲಿತಾಂಶ_2::EraseFailed;
	}

	ಆದರೆ (memcmp(base, buf, image_ಗಾತ್ರ) != 0) {
		ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ_2::VerifyFailed;
	}

	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ_2::Updated;
}

ಫಲಿತಾಂಶ_2 flash(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ)
{
	ಪೂರ್ಣಾಂಕ fd = open(ಮಾರ್ಗ, O_RDONLY);

	ಆದರೆ (fd < 0) {
		ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ_2::OpenFailed;
	}

	ರಚನೆ stat s;

	ಆದರೆ (fstat(fd, &s) != 0) {
		close(fd);
		ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ_2::OpenFailed;
	}

	ಆದರೆ (s.st_size > BL_FILE_ಗಾತ್ರ_ಎಲ್ಲೆ) {
		close(fd);
		ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ_2::TooLarge;
	}

	ಸ್ಥಿರಾಂಕ size_t file_ಗಾತ್ರ = s.st_size;

	/* up_progmem_write() rejects a size that is not a multiple of its
	 * program unit, and by then the old bootloader is erased. */
#ಆದರೆ defined(ಪುಟ_ಗಾತ್ರ_MATTERS)
	ಸ್ಥಿರಾಂಕ size_t align_ಮುಖವಾಡ = ಮೇಲೆ_progmem_pagesize(0) - 1;
#ಇಲ್ಲದಿದ್ದರೆ
	ಸ್ಥಿರಾಂಕ size_t align_ಮುಖವಾಡ = ಗಾತ್ರ(uint32_t) - 1;
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿರಾಂಕ size_t image_ಗಾತ್ರ = (file_ಗಾತ್ರ + align_ಮುಖವಾಡ) & ~align_ಮುಖವಾಡ;

	uint8_t *buf = (uint8_t *)ಸ್ಮರಣೆ_ಕೊಡು(image_ಗಾತ್ರ);

	ಆದರೆ (buf == ಶೂನ್ಯಸೂಚಕ) {
		close(fd);
		ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ_2::No_ಸ್ಮರಣೆ;
	}

	memset(buf, 0xff, image_ಗಾತ್ರ);

	ಸ್ಥಿರಾಂಕ ssize_t ಬೈಟುಗಳು_read = ಪಠ(fd, buf, file_ಗಾತ್ರ);
	close(fd);

	ಆದರೆ (ಬೈಟುಗಳು_read != (ssize_t) file_ಗಾತ್ರ) {
		ಬಿಡುಗಡೆ(buf);
		ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ_2::ReadFailed;
	}

	ಸ್ಥಿರಾಂಕ uint32_t *hdr = (ಸ್ಥಿರಾಂಕ uint32_t *)buf;

	ಆದರೆ ((hdr[0] < STM_RAM_ತಳ) ||			/* stack not below RAM */
	    (hdr[0] > (STM_RAM_ತಳ + (128 * 1024))) ||	/* stack not above RAM */
	    (hdr[1] < PX4_FLASH_ತಳ) ||			/* entrypoint not below flash */
	    ((hdr[1] - PX4_FLASH_ತಳ) > BL_FILE_ಗಾತ್ರ_ಎಲ್ಲೆ)) {	/* entrypoint not outside bootloader */
		ಬಿಡುಗಡೆ(buf);
		ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ_2::InvalidImage;
	}

	ಸ್ಥಿರಾಂಕ ಫಲಿತಾಂಶ_2 ಫಲಿತಾಂಶ = write_flash(buf, image_ಗಾತ್ರ);
	ಬಿಡುಗಡೆ(buf);
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

#ಇಲ್ಲದಿದ್ದರೆ

ಫಲಿತಾಂಶ_2 flash(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ)
{
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ_2::NotSupported;
}

#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಫಲಿತಾಂಶ_str(ಫಲಿತಾಂಶ_2 ಫಲಿತಾಂಶ)
{
	ಆಯ್ಕೆ (ಫಲಿತಾಂಶ) {
	ಸ್ಥಿತಿ ಫಲಿತಾಂಶ_2::Updated:
		ಹಿಂತಿರುಗಿ "bootloader updated";

	ಸ್ಥಿತಿ ಫಲಿತಾಂಶ_2::Unchanged:
		ಹಿಂತಿರುಗಿ "bootloader unchanged";

	ಸ್ಥಿತಿ ಫಲಿತಾಂಶ_2::NotSupported:
		ಹಿಂತಿರುಗಿ "not supported on this hardware";

	ಸ್ಥಿತಿ ಫಲಿತಾಂಶ_2::OpenFailed:
		ಹಿಂತಿರುಗಿ "cannot open bootloader file";

	ಸ್ಥಿತಿ ಫಲಿತಾಂಶ_2::TooLarge:
		ಹಿಂತಿರುಗಿ "bootloader file too large";

	ಸ್ಥಿತಿ ಫಲಿತಾಂಶ_2::No_ಸ್ಮರಣೆ:
		ಹಿಂತಿರುಗಿ "out of memory";

	ಸ್ಥಿತಿ ಫಲಿತಾಂಶ_2::ReadFailed:
		ಹಿಂತಿರುಗಿ "bootloader file read error";

	ಸ್ಥಿತಿ ಫಲಿತಾಂಶ_2::InvalidImage:
		ಹಿಂತಿರುಗಿ "not a bootloader image";

	ಸ್ಥಿತಿ ಫಲಿತಾಂಶ_2::EraseFailed:
		ಹಿಂತಿರುಗಿ "flash erase failed, retry before rebooting";

	ಸ್ಥಿತಿ ಫಲಿತಾಂಶ_2::ಕಾರ್ಯಕ್ರಮ_Failed:
		ಹಿಂತಿರುಗಿ "flash program failed, retry before rebooting";

	ಸ್ಥಿತಿ ಫಲಿತಾಂಶ_2::VerifyFailed:
		ಹಿಂತಿರುಗಿ "verify failed, retry before rebooting";
	}

	ಹಿಂತಿರುಗಿ "unknown error";
}

} // namespace bl_update
