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
#ಸೂಚನೆ once
/****************************************************************************
 * Included Files
 ****************************************************************************/
#ಅನ್ತರ್ಭಾವ <px4_platform_common/px4_config.h>
#ಅನ್ತರ್ಭಾವ <px4_arch/micro_hal.h>
#ಅನ್ತರ್ಭಾವ <systemlib/px4_macros.h>
#ಅನ್ತರ್ಭಾವ <nuttx/irq.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#ಆದರೆ defined(HAS_BBSRAM)

#ಅನ್ತರ್ಭಾವ <stm32_bbsram.h>
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ bbsramd_s dump_s;

#ವ್ಯಾಖ್ಯಾನಿಸು HARDFAULT_REBOOT_FILENO 0
#ವ್ಯಾಖ್ಯಾನಿಸು HARDFAULT_REBOOT_ಮಾರ್ಗ BBSRAM_ಮಾರ್ಗ "" STRINGIFY(HARDFAULT_REBOOT_FILENO)
#ವ್ಯಾಖ್ಯಾನಿಸು HARDFAULT_ULOG_FILENO 3
#ವ್ಯಾಖ್ಯಾನಿಸು HARDFAULT_ULOG_ಮಾರ್ಗ BBSRAM_ಮಾರ್ಗ "" STRINGIFY(HARDFAULT_ULOG_FILENO)
#ವ್ಯಾಖ್ಯಾನಿಸು HARDFAULT_FILENO 4
#ವ್ಯಾಖ್ಯಾನಿಸು HARDFAULT_ಮಾರ್ಗ BBSRAM_ಮಾರ್ಗ "" STRINGIFY(HARDFAULT_FILENO)

#ವ್ಯಾಖ್ಯಾನಿಸು HARDFAULT_MAX_ULOG_FILE_LEN 64 /* must be large enough to store the full path to the log file */

#ವ್ಯಾಖ್ಯಾನಿಸು BBSRAM_ಗಾತ್ರ_FN0 (ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ))
#ವ್ಯಾಖ್ಯಾನಿಸು BBSRAM_ಗಾತ್ರ_FN1 384     /* greater then 2.5 times the size of vehicle_status_s */
#ವ್ಯಾಖ್ಯಾನಿಸು BBSRAM_ಗಾತ್ರ_FN2 384     /* greater then 2.5 times the size of vehicle_status_s */
#ವ್ಯಾಖ್ಯಾನಿಸು BBSRAM_ಗಾತ್ರ_FN3 HARDFAULT_MAX_ULOG_FILE_LEN
#ವ್ಯಾಖ್ಯಾನಿಸು BBSRAM_ಗಾತ್ರ_FN4 -1

/* The following guides in the amount of the user and interrupt stack
 * data we can save. The amount of storage left will dictate the actual
 * number of entries of the user stack data saved. If it is too big
 * It will be truncated by the call to stm32_bbsram_savepanic
 */
#ವ್ಯಾಖ್ಯಾನಿಸು BBSRAM_ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ 20 /* This is an assumption */
#ವ್ಯಾಖ್ಯಾನಿಸು BBSRAM_ಬಳಸಿದ ((5*BBSRAM_ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ)+(BBSRAM_ಗಾತ್ರ_FN0+BBSRAM_ಗಾತ್ರ_FN1+BBSRAM_ಗಾತ್ರ_FN2+BBSRAM_ಗಾತ್ರ_FN3))
#ವ್ಯಾಖ್ಯಾನಿಸು BBSRAM_REAMINING (PX4_BBSRAM_ಗಾತ್ರ-BBSRAM_ಬಳಸಿದ)
#ಆದರೆ CONFIG_ARCH_INTERRUPTSTACK <= 3
#  define BBSRAM_ಸಂಖ್ಯೆ_STACKS 1
#ಇಲ್ಲದಿದ್ದರೆ
#  define BBSRAM_ಸಂಖ್ಯೆ_STACKS 2
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು BBSRAM_FIXED_ELEMENTS_ಗಾತ್ರ (ಗಾತ್ರ(info_s))
#ವ್ಯಾಖ್ಯಾನಿಸು BBSRAM_LEFTOVER (BBSRAM_REAMINING-BBSRAM_FIXED_ELEMENTS_ಗಾತ್ರ)

#ವ್ಯಾಖ್ಯಾನಿಸು CONFIG_ISTACK_ಗಾತ್ರ (BBSRAM_LEFTOVER/BBSRAM_ಸಂಖ್ಯೆ_STACKS/ಗಾತ್ರ(ರಾಶಿ_ಶಬ್ದ_t))
#ವ್ಯಾಖ್ಯಾನಿಸು CONFIG_USTACK_ಗಾತ್ರ (BBSRAM_LEFTOVER/BBSRAM_ಸಂಖ್ಯೆ_STACKS/ಗಾತ್ರ(ರಾಶಿ_ಶಬ್ದ_t))

/* The path to the Battery Backed up SRAM */
#ವ್ಯಾಖ್ಯಾನಿಸು BBSRAM_ಮಾರ್ಗ "/fs/bbr"
/* The sizes of the files to create (-1) use rest of BBSRAM memory */
#ವ್ಯಾಖ್ಯಾನಿಸು BSRAM_FILE_SIZES { \
		BBSRAM_ಗಾತ್ರ_FN0,   /* For Time stamp only */                  \
		BBSRAM_ಗಾತ್ರ_FN1,   /* For Current Flight Parameters Copy A */ \
		BBSRAM_ಗಾತ್ರ_FN2,   /* For Current Flight Parameters Copy B */ \
		BBSRAM_ಗಾತ್ರ_FN3,   /* For the latest ULog file path */        \
		BBSRAM_ಗಾತ್ರ_FN4,   /* For the Panic Log use rest of space */  \
		0                  /* End of table marker */                  \
	}
#ಅಥವಾಆದರೆ defined(HAS_PROGMEM)

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ progmem_s dump_s;

#ಆದರೆ CONFIG_ARCH_INTERRUPTSTACK <= 3
#  define PROGMEM_ಸಂಖ್ಯೆ_STACKS 1
#ಇಲ್ಲದಿದ್ದರೆ
#  define PROGMEM_ಸಂಖ್ಯೆ_STACKS 2
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು PROGMEM_DUMP_FIXED_ELEMENTS_ಗಾತ್ರ (ಗಾತ್ರ(info_s))
#ವ್ಯಾಖ್ಯಾನಿಸು PROGMEM_DUMP_LEFTOVER (PROGMEM_DUMP_ರಾಶಿ_ಗಾತ್ರ-PROGMEM_DUMP_FIXED_ELEMENTS_ಗಾತ್ರ)

#ವ್ಯಾಖ್ಯಾನಿಸು CONFIG_ISTACK_ಗಾತ್ರ (PROGMEM_DUMP_LEFTOVER/PROGMEM_ಸಂಖ್ಯೆ_STACKS/ಗಾತ್ರ(ರಾಶಿ_ಶಬ್ದ_t))
#ವ್ಯಾಖ್ಯಾನಿಸು CONFIG_USTACK_ಗಾತ್ರ (PROGMEM_DUMP_LEFTOVER/PROGMEM_ಸಂಖ್ಯೆ_STACKS/ಗಾತ್ರ(ರಾಶಿ_ಶಬ್ದ_t))

#ವ್ಯಾಖ್ಯಾನಿಸು HARDFAULT_ULOG_FILENO 2
#ವ್ಯಾಖ್ಯಾನಿಸು HARDFAULT_ULOG_ಮಾರ್ಗ PROGMEM_ಮಾರ್ಗ "" STRINGIFY(HARDFAULT_ULOG_FILENO)
#ವ್ಯಾಖ್ಯಾನಿಸು HARDFAULT_FILENO 3
#ವ್ಯಾಖ್ಯಾನಿಸು HARDFAULT_ಮಾರ್ಗ PROGMEM_ಮಾರ್ಗ "" STRINGIFY(HARDFAULT_FILENO)

#ವ್ಯಾಖ್ಯಾನಿಸು HARDFAULT_MAX_ULOG_FILE_LEN 64 /* must be large enough to store the full path to the log file */

#ವ್ಯಾಖ್ಯಾನಿಸು PROGMEM_ಗಾತ್ರ_FN0 384     /* greater then 2.5 times the size of vehicle_status_s */
#ವ್ಯಾಖ್ಯಾನಿಸು PROGMEM_ಗಾತ್ರ_FN1 384     /* greater then 2.5 times the size of vehicle_status_s */
#ವ್ಯಾಖ್ಯಾನಿಸು PROGMEM_ಗಾತ್ರ_FN2 HARDFAULT_MAX_ULOG_FILE_LEN
#ವ್ಯಾಖ್ಯಾನಿಸು PROGMEM_ಗಾತ್ರ_FN3 -1

/* The following guides in the amount of the user and interrupt stack
 * data we can save. The amount of storage left will dictate the actual
 * number of entries of the user stack data saved. If it is too big
 * It will be truncated by the call to stm32_bbsram_savepanic
 */

#ವ್ಯಾಖ್ಯಾನಿಸು PROGMEM_ಬಳಸಿದ ((5*PROGMEM_ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ)+(PROGMEM_ಗಾತ್ರ_FN0+PROGMEM_ಗಾತ್ರ_FN1+PROGMEM_ಗಾತ್ರ_FN2+PROGMEM_ಗಾತ್ರ_FN3))
#ವ್ಯಾಖ್ಯಾನಿಸು PROGMEM_REAMINING (PX4_PROGMEM_ಗಾತ್ರ-PROGMEM_ಬಳಸಿದ)
#ಆದರೆ CONFIG_ARCH_INTERRUPTSTACK <= 3
#  define PROGMEM_ಸಂಖ್ಯೆ_STACKS 1
#ಇಲ್ಲದಿದ್ದರೆ
#  define PROGMEM_ಸಂಖ್ಯೆ_STACKS 2
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು PROGMEM_FIXED_ELEMENTS_ಗಾತ್ರ (ಗಾತ್ರ(info_s))
#ವ್ಯಾಖ್ಯಾನಿಸು PROGMEM_LEFTOVER (PROGMEM_REAMINING-PROGMEM_FIXED_ELEMENTS_ಗಾತ್ರ)

#ವ್ಯಾಖ್ಯಾನಿಸು PROGMEM_FILE_ಎಣಿಕೆ 4
/* The path to the Battery Backed up SRAM */
#ವ್ಯಾಖ್ಯಾನಿಸು PROGMEM_ಮಾರ್ಗ "/mnt/hardfault"
/* The sizes of the files to create (-1) use rest of BBSRAM memory */
#ವ್ಯಾಖ್ಯಾನಿಸು PROGMEM_FILE_SIZES { \
		PROGMEM_ಗಾತ್ರ_FN0,   /* For Current Flight Parameters Copy A */ \
		PROGMEM_ಗಾತ್ರ_FN1,   /* For Current Flight Parameters Copy B */ \
		PROGMEM_ಗಾತ್ರ_FN2,  /* For the latest ULog file path */        \
		PROGMEM_ಗಾತ್ರ_FN3,   /* For the Panic Log use rest of space */  \
		0                  /* End of table marker */                  \
	}
#ಅಥವಾಆದರೆ defined(HAS_SSARC)

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ ssarc_s dump_s;

#ವ್ಯಾಖ್ಯಾನಿಸು HARDFAULT_REBOOT_FILENO 0
#ವ್ಯಾಖ್ಯಾನಿಸು HARDFAULT_REBOOT_ಮಾರ್ಗ SSARC_DUMP_ಮಾರ್ಗ "" STRINGIFY(HARDFAULT_REBOOT_FILENO)
#ವ್ಯಾಖ್ಯಾನಿಸು HARDFAULT_ULOG_FILENO 3
#ವ್ಯಾಖ್ಯಾನಿಸು HARDFAULT_ULOG_ಮಾರ್ಗ SSARC_DUMP_ಮಾರ್ಗ "" STRINGIFY(HARDFAULT_ULOG_FILENO)
#ವ್ಯಾಖ್ಯಾನಿಸು HARDFAULT_FILENO 4
#ವ್ಯಾಖ್ಯಾನಿಸು HARDFAULT_ಮಾರ್ಗ SSARC_DUMP_ಮಾರ್ಗ "" STRINGIFY(HARDFAULT_FILENO)

#ವ್ಯಾಖ್ಯಾನಿಸು HARDFAULT_MAX_ULOG_FILE_LEN 64 /* must be large enough to store the full path to the log file */

#ವ್ಯಾಖ್ಯಾನಿಸು SSARC_DUMP_ಗಾತ್ರ_FN0 ((((ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ)) / PX4_SSARC_ಖಂಡ_ದತ್ತಾಂಶ) + 1) * PX4_SSARC_ಖಂಡ_ದತ್ತಾಂಶ)
#ವ್ಯಾಖ್ಯಾನಿಸು SSARC_DUMP_ಗಾತ್ರ_FN1 (((384 / PX4_SSARC_ಖಂಡ_ದತ್ತಾಂಶ) + 1) * PX4_SSARC_ಖಂಡ_ದತ್ತಾಂಶ)     /* greater then 2.5 times the size of vehicle_status_s */
#ವ್ಯಾಖ್ಯಾನಿಸು SSARC_DUMP_ಗಾತ್ರ_FN2 (((384 / PX4_SSARC_ಖಂಡ_ದತ್ತಾಂಶ) + 1) * PX4_SSARC_ಖಂಡ_ದತ್ತಾಂಶ)     /* greater then 2.5 times the size of vehicle_status_s */
#ವ್ಯಾಖ್ಯಾನಿಸು SSARC_DUMP_ಗಾತ್ರ_FN3 (((HARDFAULT_MAX_ULOG_FILE_LEN / PX4_SSARC_ಖಂಡ_ದತ್ತಾಂಶ) + 1) * PX4_SSARC_ಖಂಡ_ದತ್ತಾಂಶ)
#ವ್ಯಾಖ್ಯಾನಿಸು SSARC_DUMP_ಗಾತ್ರ_FN4 -1

/* The following guides in the amount of the user and interrupt stack
 * data we can save. The amount of storage left will dictate the actual
 * number of entries of the user stack data saved. If it is too big
 * It will be truncated by the call to savepanic
 */
#ವ್ಯಾಖ್ಯಾನಿಸು SSARC_DUMP_ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ PX4_SSARC_ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ + 32 /* This is an assumption */
#ವ್ಯಾಖ್ಯಾನಿಸು SSARC_DUMP_ಬಳಸಿದ ((5*SSARC_DUMP_ಶೀರ್ಷಿಕೆ_ಗಾತ್ರ)+(SSARC_DUMP_ಗಾತ್ರ_FN0+SSARC_DUMP_ಗಾತ್ರ_FN1+SSARC_DUMP_ಗಾತ್ರ_FN2+SSARC_DUMP_ಗಾತ್ರ_FN3))
#ವ್ಯಾಖ್ಯಾನಿಸು SSARC_DUMP_ಉಳಿದ (PX4_SSARC_DUMP_ಗಾತ್ರ-SSARC_DUMP_ಬಳಸಿದ)
#ಆದರೆ CONFIG_ARCH_INTERRUPTSTACK <= 3
#  define SSARC_DUMP_ಸಂಖ್ಯೆ_STACKS 1
#ಇಲ್ಲದಿದ್ದರೆ
#  define SSARC_DUMP_ಸಂಖ್ಯೆ_STACKS 2
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿಸು SSARC_DUMP_FIXED_ELEMENTS_ಗಾತ್ರ (ಗಾತ್ರ(info_s))
#ವ್ಯಾಖ್ಯಾನಿಸು SSARC_DUMP_LEFTOVER (SSARC_DUMP_ಉಳಿದ-SSARC_DUMP_FIXED_ELEMENTS_ಗಾತ್ರ)

#ವ್ಯಾಖ್ಯಾನಿಸು CONFIG_ISTACK_ಗಾತ್ರ (SSARC_DUMP_LEFTOVER/SSARC_DUMP_ಸಂಖ್ಯೆ_STACKS/ಗಾತ್ರ(ರಾಶಿ_ಶಬ್ದ_t))
#ವ್ಯಾಖ್ಯಾನಿಸು CONFIG_USTACK_ಗಾತ್ರ (SSARC_DUMP_LEFTOVER/SSARC_DUMP_ಸಂಖ್ಯೆ_STACKS/ಗಾತ್ರ(ರಾಶಿ_ಶಬ್ದ_t))

#ವ್ಯಾಖ್ಯಾನಿಸು SSARC_DUMP_FILE_ಎಣಿಕೆ 5
/* The path to the Battery Backed up SRAM */
#ವ್ಯಾಖ್ಯಾನಿಸು SSARC_DUMP_ಮಾರ್ಗ "/fs/ssarc"
/* The sizes of the files to create (-1) use rest of BBSRAM memory */
#ವ್ಯಾಖ್ಯಾನಿಸು SSARC_DUMP_FILE_SIZES { \
		SSARC_DUMP_ಗಾತ್ರ_FN0,   /* For Time stamp only */                  \
		SSARC_DUMP_ಗಾತ್ರ_FN1,   /* For Current Flight Parameters Copy A */ \
		SSARC_DUMP_ಗಾತ್ರ_FN2,   /* For Current Flight Parameters Copy B */ \
		SSARC_DUMP_ಗಾತ್ರ_FN3,   /* For the latest ULog file path */        \
		SSARC_DUMP_ಗಾತ್ರ_FN4,   /* For the Panic Log use rest of space */  \
		0                  /* End of table marker */                  \
	}
#ಇಲ್ಲದಿದ್ದರೆ /* HAS_SSARC */

#ವ್ಯಾಖ್ಯಾನಿಸು CONFIG_ISTACK_ಗಾತ್ರ 0
#ವ್ಯಾಖ್ಯಾನಿಸು CONFIG_USTACK_ಗಾತ್ರ 0

#ಆದರೆ_ಅಂತ್ಯ

/* For Assert keep this much of the file name*/
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_FILE_ಮಾರ್ಗ_ಉದ್ದಳತೆ 40


/* Fixed size strings
 * To change a format add the number of chars not represented by the format
 * Specifier to the xxxx_NUM definei.e %Y is YYYY so add 2 and %s is -2
 * Also xxxxTIME_FMT need to match in size. See CCASERT in hardfault_log.c
 */
#ವ್ಯಾಖ್ಯಾನಿಸು LOG_ಮಾರ್ಗ_ತಳ       CONFIG_BOARD_ಬೇರು_ಮಾರ್ಗ "/"
#ವ್ಯಾಖ್ಯಾನಿಸು LOG_ಮಾರ್ಗ_ತಳ_LEN    ((array_ಗಾತ್ರ(LOG_ಮಾರ್ಗ_ತಳ))-1)

#ವ್ಯಾಖ್ಯಾನಿಸು LOG_NAME_FMT        "fault_%s.log"
#ವ್ಯಾಖ್ಯಾನಿಸು LOG_NAME_ಅಂಕೆ         (     -2    )
#ವ್ಯಾಖ್ಯಾನಿಸು LOG_NAME_LEN         ((array_ಗಾತ್ರ(LOG_NAME_FMT)-1) + LOG_NAME_ಅಂಕೆ)

#ವ್ಯಾಖ್ಯಾನಿಸು ಸಮಯ_FMT             "%Y_%m_%d_%H_%M_%S"
#ವ್ಯಾಖ್ಯಾನಿಸು ಸಮಯ_FMT_ಅಂಕೆ         (2+ 0+ 0+ 0+ 0+ 0)
#ವ್ಯಾಖ್ಯಾನಿಸು ಸಮಯ_FMT_LEN         (((array_ಗಾತ್ರ(ಸಮಯ_FMT)-1) + ಸಮಯ_FMT_ಅಂಕೆ))

#ವ್ಯಾಖ್ಯಾನಿಸು LOG_ಮಾರ್ಗ_LEN         ((LOG_ಮಾರ್ಗ_ತಳ_LEN + LOG_NAME_LEN + ಸಮಯ_FMT_LEN))

#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_ಸಮಯ_FMT      "%Y-%m-%d-%H:%M:%S"
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_ಸಮಯ_FMT_ಅಂಕೆ  (2+ 0+ 0+ 0+ 0+ 0)
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_ಸಮಯ_FMT_LEN  (((array_ಗಾತ್ರ(ಶೀರ್ಷಿಕೆ_ಸಮಯ_FMT)-1) + ಶೀರ್ಷಿಕೆ_ಸಮಯ_FMT_ಅಂಕೆ))

/* Select which format to use. On a terminal the details are at the bottom
 * and in a file they are at the top
 */
#ವ್ಯಾಖ್ಯಾನಿಸು HARDFAULT_DISPLAY_FORMAT 1
#ವ್ಯಾಖ್ಯಾನಿಸು HARDFAULT_FILE_FORMAT    0

/****************************************************************************
 * Public Types
 ****************************************************************************/

/* Used for stack frame storage */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ uint32_t ರಾಶಿ_ಶಬ್ದ_t;

/* Stack related data */

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
	uint32_t sp;
	uint32_t ತುದಿ;
	uint32_t ಗಾತ್ರ_2;

} ರಾಶಿ_s;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
	ರಾಶಿ_s user;
#ಆದರೆ CONFIG_ARCH_INTERRUPTSTACK > 3
	ರಾಶಿ_s ಅಡಚಣೆ;
#ಆದರೆ_ಅಂತ್ಯ

} px4_ರಾಶಿ_t;

/* Not Used for reference only */

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
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
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
	eRegsPresent          = 0x01,
	e_User_ರಾಶಿ_Present     = 0x02,
	e_Int_ರಾಶಿ_Present      = 0x04,
	eFaultRegPresent      = 0x08,
	e_Invalid_User_ರಾಶಿ_Ptr  = 0x20,
	e_Invalid_Int_ರಾಶಿ_Prt   = 0x40,
} fault_flags_t;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
	uint32_t              cfsr;  /* 0x0d28 Configurable fault status register */
	uint32_t              hfsr;  /* 0x0d2c Hard fault status register */
	uint32_t              dfsr;  /* 0x0d30 Debug fault status register */
	uint32_t              mmfsr; /* 0x0d34 Mem manage address register */
	uint32_t              bfsr;  /* 0x0d38 Bus fault address register */
	uint32_t              afsr;  /* 0x0d3c Auxiliary fault status register */
#ಆದರೆ defined(CONFIG_ARCH_CORTEXM7)
	uint32_t              abfsr; /* 0x0fa8 Auxiliary Bus Fault Status Register */
#ಆದರೆ_ಅಂತ್ಯ

} fault_regs_s;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
	fault_flags_t         flags;                  /* What is in the dump */
	uintptr_t             current_regs;           /* Used to validate the dump */
	ಪೂರ್ಣಾಂಕ                   lineno;                 /* __LINE__ to up_assert */
	ಪೂರ್ಣಾಂಕ                   pid;                    /* Process ID */
	uint32_t              regs[XCPTCONTEXT_REGS]; /* Interrupt register save area */
	fault_regs_s          fault_regs;             /* NVIC status */
	px4_ರಾಶಿ_t           stacks;                 /* Stack info */
#ಆದರೆ CONFIG_TASK_NAME_ಗಾತ್ರ > 0
	ಅಕ್ಷರ                  name[CONFIG_TASK_NAME_ಗಾತ್ರ + 1]; /* Task name (with NULL
													* terminator) */
#ಆದರೆ_ಅಂತ್ಯ
	ಅಕ್ಷರ                  filename[MAX_FILE_ಮಾರ್ಗ_ಉದ್ದಳತೆ]; /* the Last of chars in
												  * __FILE__ to up_assert */
} info_s;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
	info_s    info;                       /* The info */
#ಆದರೆ CONFIG_ARCH_INTERRUPTSTACK > 3      /* The amount of stack data is compile time
									 * sized backed on what is left after the
									 * other BBSRAM files are defined
									 * The order is such that only the
									 * ustack should be truncated
									 */
	ರಾಶಿ_ಶಬ್ದ_t istack[CONFIG_USTACK_ಗಾತ್ರ];
#ಆದರೆ_ಅಂತ್ಯ
	ರಾಶಿ_ಶಬ್ದ_t ustack[CONFIG_ISTACK_ಗಾತ್ರ];
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
ಪೂರ್ಣಾಂಕ hardfault_check_ಸ್ಥಿತಿ(ಅಕ್ಷರ *caller) weak_function;

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
ಪೂರ್ಣಾಂಕ hardfault_write(ಅಕ್ಷರ *caller, ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ format, bool rearm) weak_function;

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
ಪೂರ್ಣಾಂಕ hardfault_rearm(ಅಕ್ಷರ *caller) weak_function;

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
ಪೂರ್ಣಾಂಕ hardfault_increment_reboot(ಅಕ್ಷರ *caller, bool reset) weak_function;

__END_DECLS
