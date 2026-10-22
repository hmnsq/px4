/****************************************************************************
 *
 *   Copyright (C) 2015 PX4 Development Team. All rights reserved.
 *   Author: @author David Sidrane <david_s5@nscdg.com>
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
#ਹਦਾਇਤ once
/****************************************************************************
 * Included Files
 ****************************************************************************/
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/px4_config.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_arch/micro_hal.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <systemlib/px4_macros.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <nuttx/irq.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#ਜੇ defined(HAS_BBSRAM)

#ਅਨ੍ਤਰ੍ਭਾਵ <stm32_bbsram.h>
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ bbsramd_s dump_s;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HARDFAULT_REBOOT_FILENO 0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HARDFAULT_REBOOT_ਰਸਤਾ BBSRAM_ਰਸਤਾ "" STRINGIFY(HARDFAULT_REBOOT_FILENO)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HARDFAULT_ULOG_FILENO 3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HARDFAULT_ULOG_ਰਸਤਾ BBSRAM_ਰਸਤਾ "" STRINGIFY(HARDFAULT_ULOG_FILENO)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HARDFAULT_FILENO 4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HARDFAULT_ਰਸਤਾ BBSRAM_ਰਸਤਾ "" STRINGIFY(HARDFAULT_FILENO)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HARDFAULT_MAX_ULOG_FILE_LEN 64 /* must be large enough to store the full path to the log file */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BBSRAM_ਮਾਪ_FN0 (ਆਕਾਰ(ਪੂਰਨ))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BBSRAM_ਮਾਪ_FN1 384     /* greater then 2.5 times the size of vehicle_status_s */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BBSRAM_ਮਾਪ_FN2 384     /* greater then 2.5 times the size of vehicle_status_s */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BBSRAM_ਮਾਪ_FN3 HARDFAULT_MAX_ULOG_FILE_LEN
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BBSRAM_ਮਾਪ_FN4 -1

/* The following guides in the amount of the user and interrupt stack
 * data we can save. The amount of storage left will dictate the actual
 * number of entries of the user stack data saved. If it is too big
 * It will be truncated by the call to stm32_bbsram_savepanic
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BBSRAM_ਸਿਰਲੇਖ_ਮਾਪ 20 /* This is an assumption */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BBSRAM_ਵਰਤਿਆ ((5*BBSRAM_ਸਿਰਲੇਖ_ਮਾਪ)+(BBSRAM_ਮਾਪ_FN0+BBSRAM_ਮਾਪ_FN1+BBSRAM_ਮਾਪ_FN2+BBSRAM_ਮਾਪ_FN3))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BBSRAM_REAMINING (PX4_BBSRAM_ਮਾਪ-BBSRAM_ਵਰਤਿਆ)
#ਜੇ CONFIG_ARCH_INTERRUPTSTACK <= 3
#  define BBSRAM_ਸੰਖਿਆ_STACKS 1
#ਨਹੀਂਤਾਂ
#  define BBSRAM_ਸੰਖਿਆ_STACKS 2
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BBSRAM_FIXED_ELEMENTS_ਮਾਪ (ਆਕਾਰ(info_s))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BBSRAM_LEFTOVER (BBSRAM_REAMINING-BBSRAM_FIXED_ELEMENTS_ਮਾਪ)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONFIG_ISTACK_ਮਾਪ (BBSRAM_LEFTOVER/BBSRAM_ਸੰਖਿਆ_STACKS/ਆਕਾਰ(ਢੇਰ_ਸ਼ਬਦ_t))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONFIG_USTACK_ਮਾਪ (BBSRAM_LEFTOVER/BBSRAM_ਸੰਖਿਆ_STACKS/ਆਕਾਰ(ਢੇਰ_ਸ਼ਬਦ_t))

/* The path to the Battery Backed up SRAM */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BBSRAM_ਰਸਤਾ "/fs/bbr"
/* The sizes of the files to create (-1) use rest of BBSRAM memory */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BSRAM_FILE_SIZES { \
		BBSRAM_ਮਾਪ_FN0,   /* For Time stamp only */                  \
		BBSRAM_ਮਾਪ_FN1,   /* For Current Flight Parameters Copy A */ \
		BBSRAM_ਮਾਪ_FN2,   /* For Current Flight Parameters Copy B */ \
		BBSRAM_ਮਾਪ_FN3,   /* For the latest ULog file path */        \
		BBSRAM_ਮਾਪ_FN4,   /* For the Panic Log use rest of space */  \
		0                  /* End of table marker */                  \
	}
#ਜਾਂਜੇ defined(HAS_PROGMEM)

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ progmem_s dump_s;

#ਜੇ CONFIG_ARCH_INTERRUPTSTACK <= 3
#  define PROGMEM_ਸੰਖਿਆ_STACKS 1
#ਨਹੀਂਤਾਂ
#  define PROGMEM_ਸੰਖਿਆ_STACKS 2
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROGMEM_DUMP_FIXED_ELEMENTS_ਮਾਪ (ਆਕਾਰ(info_s))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROGMEM_DUMP_LEFTOVER (PROGMEM_DUMP_ਢੇਰ_ਮਾਪ-PROGMEM_DUMP_FIXED_ELEMENTS_ਮਾਪ)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONFIG_ISTACK_ਮਾਪ (PROGMEM_DUMP_LEFTOVER/PROGMEM_ਸੰਖਿਆ_STACKS/ਆਕਾਰ(ਢੇਰ_ਸ਼ਬਦ_t))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONFIG_USTACK_ਮਾਪ (PROGMEM_DUMP_LEFTOVER/PROGMEM_ਸੰਖਿਆ_STACKS/ਆਕਾਰ(ਢੇਰ_ਸ਼ਬਦ_t))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HARDFAULT_ULOG_FILENO 2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HARDFAULT_ULOG_ਰਸਤਾ PROGMEM_ਰਸਤਾ "" STRINGIFY(HARDFAULT_ULOG_FILENO)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HARDFAULT_FILENO 3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HARDFAULT_ਰਸਤਾ PROGMEM_ਰਸਤਾ "" STRINGIFY(HARDFAULT_FILENO)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HARDFAULT_MAX_ULOG_FILE_LEN 64 /* must be large enough to store the full path to the log file */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROGMEM_ਮਾਪ_FN0 384     /* greater then 2.5 times the size of vehicle_status_s */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROGMEM_ਮਾਪ_FN1 384     /* greater then 2.5 times the size of vehicle_status_s */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROGMEM_ਮਾਪ_FN2 HARDFAULT_MAX_ULOG_FILE_LEN
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROGMEM_ਮਾਪ_FN3 -1

/* The following guides in the amount of the user and interrupt stack
 * data we can save. The amount of storage left will dictate the actual
 * number of entries of the user stack data saved. If it is too big
 * It will be truncated by the call to stm32_bbsram_savepanic
 */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROGMEM_ਵਰਤਿਆ ((5*PROGMEM_ਸਿਰਲੇਖ_ਮਾਪ)+(PROGMEM_ਮਾਪ_FN0+PROGMEM_ਮਾਪ_FN1+PROGMEM_ਮਾਪ_FN2+PROGMEM_ਮਾਪ_FN3))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROGMEM_REAMINING (PX4_PROGMEM_ਮਾਪ-PROGMEM_ਵਰਤਿਆ)
#ਜੇ CONFIG_ARCH_INTERRUPTSTACK <= 3
#  define PROGMEM_ਸੰਖਿਆ_STACKS 1
#ਨਹੀਂਤਾਂ
#  define PROGMEM_ਸੰਖਿਆ_STACKS 2
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROGMEM_FIXED_ELEMENTS_ਮਾਪ (ਆਕਾਰ(info_s))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROGMEM_LEFTOVER (PROGMEM_REAMINING-PROGMEM_FIXED_ELEMENTS_ਮਾਪ)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROGMEM_FILE_ਗਿਣਤੀ 4
/* The path to the Battery Backed up SRAM */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROGMEM_ਰਸਤਾ "/mnt/hardfault"
/* The sizes of the files to create (-1) use rest of BBSRAM memory */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PROGMEM_FILE_SIZES { \
		PROGMEM_ਮਾਪ_FN0,   /* For Current Flight Parameters Copy A */ \
		PROGMEM_ਮਾਪ_FN1,   /* For Current Flight Parameters Copy B */ \
		PROGMEM_ਮਾਪ_FN2,  /* For the latest ULog file path */        \
		PROGMEM_ਮਾਪ_FN3,   /* For the Panic Log use rest of space */  \
		0                  /* End of table marker */                  \
	}
#ਜਾਂਜੇ defined(HAS_SSARC)

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ ssarc_s dump_s;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HARDFAULT_REBOOT_FILENO 0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HARDFAULT_REBOOT_ਰਸਤਾ SSARC_DUMP_ਰਸਤਾ "" STRINGIFY(HARDFAULT_REBOOT_FILENO)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HARDFAULT_ULOG_FILENO 3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HARDFAULT_ULOG_ਰਸਤਾ SSARC_DUMP_ਰਸਤਾ "" STRINGIFY(HARDFAULT_ULOG_FILENO)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HARDFAULT_FILENO 4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HARDFAULT_ਰਸਤਾ SSARC_DUMP_ਰਸਤਾ "" STRINGIFY(HARDFAULT_FILENO)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HARDFAULT_MAX_ULOG_FILE_LEN 64 /* must be large enough to store the full path to the log file */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSARC_DUMP_ਮਾਪ_FN0 ((((ਆਕਾਰ(ਪੂਰਨ)) / PX4_SSARC_ਖੰਡ_ਅੰਕੜੇ) + 1) * PX4_SSARC_ਖੰਡ_ਅੰਕੜੇ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSARC_DUMP_ਮਾਪ_FN1 (((384 / PX4_SSARC_ਖੰਡ_ਅੰਕੜੇ) + 1) * PX4_SSARC_ਖੰਡ_ਅੰਕੜੇ)     /* greater then 2.5 times the size of vehicle_status_s */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSARC_DUMP_ਮਾਪ_FN2 (((384 / PX4_SSARC_ਖੰਡ_ਅੰਕੜੇ) + 1) * PX4_SSARC_ਖੰਡ_ਅੰਕੜੇ)     /* greater then 2.5 times the size of vehicle_status_s */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSARC_DUMP_ਮਾਪ_FN3 (((HARDFAULT_MAX_ULOG_FILE_LEN / PX4_SSARC_ਖੰਡ_ਅੰਕੜੇ) + 1) * PX4_SSARC_ਖੰਡ_ਅੰਕੜੇ)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSARC_DUMP_ਮਾਪ_FN4 -1

/* The following guides in the amount of the user and interrupt stack
 * data we can save. The amount of storage left will dictate the actual
 * number of entries of the user stack data saved. If it is too big
 * It will be truncated by the call to savepanic
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSARC_DUMP_ਸਿਰਲੇਖ_ਮਾਪ PX4_SSARC_ਸਿਰਲੇਖ_ਮਾਪ + 32 /* This is an assumption */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSARC_DUMP_ਵਰਤਿਆ ((5*SSARC_DUMP_ਸਿਰਲੇਖ_ਮਾਪ)+(SSARC_DUMP_ਮਾਪ_FN0+SSARC_DUMP_ਮਾਪ_FN1+SSARC_DUMP_ਮਾਪ_FN2+SSARC_DUMP_ਮਾਪ_FN3))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSARC_DUMP_ਬਚਿਆ (PX4_SSARC_DUMP_ਮਾਪ-SSARC_DUMP_ਵਰਤਿਆ)
#ਜੇ CONFIG_ARCH_INTERRUPTSTACK <= 3
#  define SSARC_DUMP_ਸੰਖਿਆ_STACKS 1
#ਨਹੀਂਤਾਂ
#  define SSARC_DUMP_ਸੰਖਿਆ_STACKS 2
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSARC_DUMP_FIXED_ELEMENTS_ਮਾਪ (ਆਕਾਰ(info_s))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSARC_DUMP_LEFTOVER (SSARC_DUMP_ਬਚਿਆ-SSARC_DUMP_FIXED_ELEMENTS_ਮਾਪ)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONFIG_ISTACK_ਮਾਪ (SSARC_DUMP_LEFTOVER/SSARC_DUMP_ਸੰਖਿਆ_STACKS/ਆਕਾਰ(ਢੇਰ_ਸ਼ਬਦ_t))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONFIG_USTACK_ਮਾਪ (SSARC_DUMP_LEFTOVER/SSARC_DUMP_ਸੰਖਿਆ_STACKS/ਆਕਾਰ(ਢੇਰ_ਸ਼ਬਦ_t))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSARC_DUMP_FILE_ਗਿਣਤੀ 5
/* The path to the Battery Backed up SRAM */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSARC_DUMP_ਰਸਤਾ "/fs/ssarc"
/* The sizes of the files to create (-1) use rest of BBSRAM memory */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SSARC_DUMP_FILE_SIZES { \
		SSARC_DUMP_ਮਾਪ_FN0,   /* For Time stamp only */                  \
		SSARC_DUMP_ਮਾਪ_FN1,   /* For Current Flight Parameters Copy A */ \
		SSARC_DUMP_ਮਾਪ_FN2,   /* For Current Flight Parameters Copy B */ \
		SSARC_DUMP_ਮਾਪ_FN3,   /* For the latest ULog file path */        \
		SSARC_DUMP_ਮਾਪ_FN4,   /* For the Panic Log use rest of space */  \
		0                  /* End of table marker */                  \
	}
#ਨਹੀਂਤਾਂ /* HAS_SSARC */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONFIG_ISTACK_ਮਾਪ 0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CONFIG_USTACK_ਮਾਪ 0

#ਜੇ_ਅੰਤ

/* For Assert keep this much of the file name*/
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_FILE_ਰਸਤਾ_ਲੰਮਾਈ 40


/* Fixed size strings
 * To change a format add the number of chars not represented by the format
 * Specifier to the xxxx_NUM definei.e %Y is YYYY so add 2 and %s is -2
 * Also xxxxTIME_FMT need to match in size. See CCASERT in hardfault_log.c
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOG_ਰਸਤਾ_ਅਧਾਰ       CONFIG_BOARD_ਜੜ੍ਹ_ਰਸਤਾ "/"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOG_ਰਸਤਾ_ਅਧਾਰ_LEN    ((array_ਮਾਪ(LOG_ਰਸਤਾ_ਅਧਾਰ))-1)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOG_NAME_FMT        "fault_%s.log"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOG_NAME_ਅੰਕ         (     -2    )
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOG_NAME_LEN         ((array_ਮਾਪ(LOG_NAME_FMT)-1) + LOG_NAME_ਅੰਕ)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਮਾਂ_FMT             "%Y_%m_%d_%H_%M_%S"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਮਾਂ_FMT_ਅੰਕ         (2+ 0+ 0+ 0+ 0+ 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਮਾਂ_FMT_LEN         (((array_ਮਾਪ(ਸਮਾਂ_FMT)-1) + ਸਮਾਂ_FMT_ਅੰਕ))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ LOG_ਰਸਤਾ_LEN         ((LOG_ਰਸਤਾ_ਅਧਾਰ_LEN + LOG_NAME_LEN + ਸਮਾਂ_FMT_LEN))

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_ਸਮਾਂ_FMT      "%Y-%m-%d-%H:%M:%S"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_ਸਮਾਂ_FMT_ਅੰਕ  (2+ 0+ 0+ 0+ 0+ 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_ਸਮਾਂ_FMT_LEN  (((array_ਮਾਪ(ਸਿਰਲੇਖ_ਸਮਾਂ_FMT)-1) + ਸਿਰਲੇਖ_ਸਮਾਂ_FMT_ਅੰਕ))

/* Select which format to use. On a terminal the details are at the bottom
 * and in a file they are at the top
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HARDFAULT_DISPLAY_FORMAT 1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HARDFAULT_FILE_FORMAT    0

/****************************************************************************
 * Public Types
 ****************************************************************************/

/* Used for stack frame storage */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ uint32_t ਢੇਰ_ਸ਼ਬਦ_t;

/* Stack related data */

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
	uint32_t sp;
	uint32_t ਸਿਖਰ;
	uint32_t ਮਾਪ;

} ਢੇਰ_s;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
	ਢੇਰ_s user;
#ਜੇ CONFIG_ARCH_INTERRUPTSTACK > 3
	ਢੇਰ_s ਰੁਕਾਵਟ;
#ਜੇ_ਅੰਤ

} px4_ਢੇਰ_t;

/* Not Used for reference only */

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
	uint32_t r0;
	uint32_t r1;
	uint32_t r2;
	uint32_t r3;
	uint32_t r4;
	uint32_t r5;
	uint32_t r6;
	uint32_t r7;
	uint32_t r8;
	uint32_t r9;
	uint32_t r10;
	uint32_t r11;
	uint32_t r12;
	uint32_t sp;
	uint32_t lr;
	uint32_t pc;
	uint32_t xpsr;
	uint32_t d0;
	uint32_t d1;
	uint32_t d2;
	uint32_t d3;
	uint32_t d4;
	uint32_t d5;
	uint32_t d6;
	uint32_t d7;
	uint32_t d8;
	uint32_t d9;
	uint32_t d10;
	uint32_t d11;
	uint32_t d12;
	uint32_t d13;
	uint32_t d14;
	uint32_t d15;
	uint32_t fpscr;
	uint32_t sp_main;
	uint32_t sp_process;
	uint32_t apsr;
	uint32_t ipsr;
	uint32_t epsr;
	uint32_t primask;
	uint32_t basepri;
	uint32_t faultmask;
	uint32_t control;
	uint32_t s0;
	uint32_t s1;
	uint32_t s2;
	uint32_t s3;
	uint32_t s4;
	uint32_t s5;
	uint32_t s6;
	uint32_t s7;
	uint32_t s8;
	uint32_t s9;
	uint32_t s10;
	uint32_t s11;
	uint32_t s12;
	uint32_t s13;
	uint32_t s14;
	uint32_t s15;
	uint32_t s16;
	uint32_t s17;
	uint32_t s18;
	uint32_t s19;
	uint32_t s20;
	uint32_t s21;
	uint32_t s22;
	uint32_t s23;
	uint32_t s24;
	uint32_t s25;
	uint32_t s26;
	uint32_t s27;
	uint32_t s28;
	uint32_t s29;
	uint32_t s30;
	uint32_t s31;
} proc_regs_s;


/* Flags to identify what is in the dump */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
	eRegsPresent          = 0x01,
	e_User_ਢੇਰ_Present     = 0x02,
	e_Int_ਢੇਰ_Present      = 0x04,
	eFaultRegPresent      = 0x08,
	e_Invalid_User_ਢੇਰ_Ptr  = 0x20,
	e_Invalid_Int_ਢੇਰ_Prt   = 0x40,
} fault_flags_t;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
	uint32_t              cfsr;  /* 0x0d28 Configurable fault status register */
	uint32_t              hfsr;  /* 0x0d2c Hard fault status register */
	uint32_t              dfsr;  /* 0x0d30 Debug fault status register */
	uint32_t              mmfsr; /* 0x0d34 Mem manage address register */
	uint32_t              bfsr;  /* 0x0d38 Bus fault address register */
	uint32_t              afsr;  /* 0x0d3c Auxiliary fault status register */
#ਜੇ defined(CONFIG_ARCH_CORTEXM7)
	uint32_t              abfsr; /* 0x0fa8 Auxiliary Bus Fault Status Register */
#ਜੇ_ਅੰਤ

} fault_regs_s;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
	fault_flags_t         flags;                  /* What is in the dump */
	uintptr_t             current_regs;           /* Used to validate the dump */
	ਪੂਰਨ                   lineno;                 /* __LINE__ to up_assert */
	ਪੂਰਨ                   pid;                    /* Process ID */
	uint32_t              regs[XCPTCONTEXT_REGS]; /* Interrupt register save area */
	fault_regs_s          fault_regs;             /* NVIC status */
	px4_ਢੇਰ_t           stacks;                 /* Stack info */
#ਜੇ CONFIG_TASK_NAME_ਮਾਪ > 0
	ਅੱਖਰ                  name[CONFIG_TASK_NAME_ਮਾਪ + 1]; /* Task name (with NULL
													* terminator) */
#ਜੇ_ਅੰਤ
	ਅੱਖਰ                  filename[MAX_FILE_ਰਸਤਾ_ਲੰਮਾਈ]; /* the Last of chars in
												  * __FILE__ to up_assert */
} info_s;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ {
	info_s    info;                       /* The info */
#ਜੇ CONFIG_ARCH_INTERRUPTSTACK > 3      /* The amount of stack data is compile time
									 * sized backed on what is left after the
									 * other BBSRAM files are defined
									 * The order is such that only the
									 * ustack should be truncated
									 */
	ਢੇਰ_ਸ਼ਬਦ_t istack[CONFIG_USTACK_ਮਾਪ];
#ਜੇ_ਅੰਤ
	ਢੇਰ_ਸ਼ਬਦ_t ustack[CONFIG_ISTACK_ਮਾਪ];
} fullcontext_s;

__BEGIN_DECLS
/****************************************************************************
 * Name: hardfault_check_status
 *
 * Description:
 *      Check the status of the BBSRAM hard fault file which can be in
 *      one of two states Armed, Valid or Broken.
 *
 *      Armed - The file in the armed state is not accessible in the fs.
 *              The act of unlinking it is what arms it.
 *
 *      Valid - The file contains data from a hard fault
 *
 * Inputs:
 *   - caller:  A label to display in syslog output
 *
 *  Returned Value:
 *   -ENOENT    Armed - The file is in the armed state
 *    OK        Valid - The file contains data from a fault that has not
 *                      been committed to disk (@see write_hardfault()).
 *   -  Any < 0 Broken - Should not happen
 *
 ****************************************************************************/
ਪੂਰਨ hardfault_check_ਸਥਿਤੀ(ਅੱਖਰ *caller) weak_function;

/****************************************************************************
 * Name: hardfault_write
 *
 * Description:
 *      Will parse and write a human readable output of the data
 *      in the BBSRAM file. Once
 *
 *
 * Inputs:
 *   - caller:  A label to display in syslog output
 *   - fd:      An FD to write the data to
 *   - format:  Select which format to use.
 *
 *              HARDFAULT_DISPLAY_FORMAT  On the console the details are
 *                                        at the bottom
 *              HARDFAULT_FILE_FORMAT     In a file details are at the top
 *                                        of the log file
 *
 *    - rearm: If true will move the file to the Armed state, if
 *             false the file is not armed and can be read again
 *
 *  Returned Value:
 *
 *    OK  or errno
 *
 *
 ****************************************************************************/
ਪੂਰਨ hardfault_write(ਅੱਖਰ *caller, ਪੂਰਨ fd, ਪੂਰਨ format, bool rearm) weak_function;

/****************************************************************************
 * Name: hardfault_rearm
 *
 * Description:
 *      Will move the file to the Armed state
 *
 *
 * Inputs:
 *   - caller:  A label to display in syslog output
 *
 *  Returned Value:
 *
 *    OK  or errno
 *
 *
 ****************************************************************************/
ਪੂਰਨ hardfault_rearm(ਅੱਖਰ *caller) weak_function;

/****************************************************************************
 * Name: hardfault_increment_reboot
 *
 * Description:
 *      Will increment the reboot counter. The reboot counter will help
 *      detect reboot loops.
 *
 *
 * Inputs:
 *   - caller:  A label to display in syslog output
 *   - reset :  when set will reset the reboot counter to 0.
 *
 *  Returned Value:
 *
 *    The current value of the reboot counter (after increment/reset) or errno < 0.
 *
 *
 ****************************************************************************/
ਪੂਰਨ hardfault_increment_reboot(ਅੱਖਰ *caller, bool reset) weak_function;

__END_DECLS
