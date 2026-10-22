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

#అన్తర్భావ "bl_update_flash.h"

#అన్తర్భావ <px4_platform_common/px4_config.h>

#అన్తర్భావ <stdint.h>
#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <unistd.h>
#అన్తర్భావ <fcntl.h>
#అన్తర్భావ <sched.h>
#అన్తర్భావ <sys/stat.h>

#అన్తర్భావ <arch/board/board.h>

#అన్తర్భావ <nuttx/progmem.h>

#ఒకవేళ defined(BL_UPDATE_WATCHDOG)
#  include <drivers/drv_watchdog.h>
#లేకపోతే
#  define watchdog_pet()
#అయితే_ముగింపు

#ఒకవేళ defined(CONFIG_ARCH_CHIP_STM32H7)
#  define BL_FILE_కొలత_హద్దు	128*1024
#  define STM_RAM_పునాది        STM32_AXISRAM_పునాది
#  define పుట_కొలత_MATTERS   1
#లేదావేళ defined(CONFIG_ARCH_CHIP_STM32F7)
#  define BL_FILE_కొలత_హద్దు	32*1024
#  define STM_RAM_పునాది        STM32_SRAM_పునాది
#లేకపోతే
#  define BL_FILE_కొలత_హద్దు  16384
#  define STM_RAM_పునాది        STM32_SRAM_పునాది
#అయితే_ముగింపు

నామాకాశ bl_update
{

#ఒకవేళ defined (CONFIG_STM32_STM32F4XXX) || defined (CONFIG_ARCH_CHIP_STM32F7) || \
    defined (CONFIG_ARCH_CHIP_STM32H7)

స్థిర ఫలితం_2 write_flash(స్థిరరాశి uint8_t *buf, size_t image_కొలత)
{
	uint8_t *base = (uint8_t *) PX4_FLASH_పునాది;

	ఒకవేళ (memcmp(base, buf, image_కొలత) == 0) {
		తిరిగి ఫలితం_2::Unchanged;
	}

	/* prevent other tasks from running while we do this */
	sched_తాళం();

	/* The IWDG may already be running (CAN nodes start it in the bootloader)
	 * and nothing else gets to pet it while the scheduler is locked. */
	watchdog_pet();
	స్థిరరాశి తార్కిక erased = పైకి_progmem_eraseblock(0) >= 0;
	watchdog_pet();

	/* Program even if the erase reported an error: the old bootloader is
	 * already damaged, and a failed program reports it below. */
	స్థిరరాశి ssize_t కొలత = పైకి_progmem_write((size_t) base, buf, image_కొలత);
	watchdog_pet();

	/* re-lock the flash control register */
	stm32_flash_తాళం();

	sched_unlock();

	ఒకవేళ (కొలత != (ssize_t) image_కొలత) {
		తిరిగి erased ? ఫలితం_2::కార్యక్రమం_Failed : ఫలితం_2::EraseFailed;
	}

	ఒకవేళ (memcmp(base, buf, image_కొలత) != 0) {
		తిరిగి ఫలితం_2::VerifyFailed;
	}

	తిరిగి ఫలితం_2::Updated;
}

ఫలితం_2 flash(స్థిరరాశి అక్షరం *మార్గం)
{
	పూర్ణాంకం fd = open(మార్గం, O_RDONLY);

	ఒకవేళ (fd < 0) {
		తిరిగి ఫలితం_2::OpenFailed;
	}

	నిర్మాణం stat s;

	ఒకవేళ (fstat(fd, &s) != 0) {
		close(fd);
		తిరిగి ఫలితం_2::OpenFailed;
	}

	ఒకవేళ (s.st_size > BL_FILE_కొలత_హద్దు) {
		close(fd);
		తిరిగి ఫలితం_2::TooLarge;
	}

	స్థిరరాశి size_t file_కొలత = s.st_size;

	/* up_progmem_write() rejects a size that is not a multiple of its
	 * program unit, and by then the old bootloader is erased. */
#ఒకవేళ defined(పుట_కొలత_MATTERS)
	స్థిరరాశి size_t align_ముసుగు = పైకి_progmem_pagesize(0) - 1;
#లేకపోతే
	స్థిరరాశి size_t align_ముసుగు = పరిమాణం(uint32_t) - 1;
#అయితే_ముగింపు
	స్థిరరాశి size_t image_కొలత = (file_కొలత + align_ముసుగు) & ~align_ముసుగు;

	uint8_t *buf = (uint8_t *)జ్ఞాపకం_ఇవ్వు(image_కొలత);

	ఒకవేళ (buf == శూన్యసూచిక) {
		close(fd);
		తిరిగి ఫలితం_2::No_జ్ఞాపకం;
	}

	memset(buf, 0xff, image_కొలత);

	స్థిరరాశి ssize_t బైట్లు_read = పఠ(fd, buf, file_కొలత);
	close(fd);

	ఒకవేళ (బైట్లు_read != (ssize_t) file_కొలత) {
		విడుదల(buf);
		తిరిగి ఫలితం_2::ReadFailed;
	}

	స్థిరరాశి uint32_t *hdr = (స్థిరరాశి uint32_t *)buf;

	ఒకవేళ ((hdr[0] < STM_RAM_పునాది) ||			/* stack not below RAM */
	    (hdr[0] > (STM_RAM_పునాది + (128 * 1024))) ||	/* stack not above RAM */
	    (hdr[1] < PX4_FLASH_పునాది) ||			/* entrypoint not below flash */
	    ((hdr[1] - PX4_FLASH_పునాది) > BL_FILE_కొలత_హద్దు)) {	/* entrypoint not outside bootloader */
		విడుదల(buf);
		తిరిగి ఫలితం_2::InvalidImage;
	}

	స్థిరరాశి ఫలితం_2 ఫలితం = write_flash(buf, image_కొలత);
	విడుదల(buf);
	తిరిగి ఫలితం;
}

#లేకపోతే

ఫలితం_2 flash(స్థిరరాశి అక్షరం *మార్గం)
{
	తిరిగి ఫలితం_2::NotSupported;
}

#అయితే_ముగింపు

స్థిరరాశి అక్షరం *ఫలితం_str(ఫలితం_2 ఫలితం)
{
	ఎంపిక (ఫలితం) {
	స్థితి ఫలితం_2::Updated:
		తిరిగి "bootloader updated";

	స్థితి ఫలితం_2::Unchanged:
		తిరిగి "bootloader unchanged";

	స్థితి ఫలితం_2::NotSupported:
		తిరిగి "not supported on this hardware";

	స్థితి ఫలితం_2::OpenFailed:
		తిరిగి "cannot open bootloader file";

	స్థితి ఫలితం_2::TooLarge:
		తిరిగి "bootloader file too large";

	స్థితి ఫలితం_2::No_జ్ఞాపకం:
		తిరిగి "out of memory";

	స్థితి ఫలితం_2::ReadFailed:
		తిరిగి "bootloader file read error";

	స్థితి ఫలితం_2::InvalidImage:
		తిరిగి "not a bootloader image";

	స్థితి ఫలితం_2::EraseFailed:
		తిరిగి "flash erase failed, retry before rebooting";

	స్థితి ఫలితం_2::కార్యక్రమం_Failed:
		తిరిగి "flash program failed, retry before rebooting";

	స్థితి ఫలితం_2::VerifyFailed:
		తిరిగి "verify failed, retry before rebooting";
	}

	తిరిగి "unknown error";
}

} // namespace bl_update
