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
#ہدایت once
/****************************************************************************
 * Included Files
 ****************************************************************************/
#شامل <px4_platform_common/px4_config.h>
#شامل <px4_arch/micro_hal.h>
#شامل <systemlib/px4_macros.h>
#شامل <nuttx/irq.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#اگر defined(HAS_BBSRAM)

#شامل <stm32_bbsram.h>
قسمتعریف ساخت bbsramd_s dump_s;

#تعریف_کرو HARDFAULT_REBOOT_FILENO 0
#تعریف_کرو HARDFAULT_REBOOT_راستہ BBSRAM_راستہ "" STRINGIFY(HARDFAULT_REBOOT_FILENO)
#تعریف_کرو HARDFAULT_ULOG_FILENO 3
#تعریف_کرو HARDFAULT_ULOG_راستہ BBSRAM_راستہ "" STRINGIFY(HARDFAULT_ULOG_FILENO)
#تعریف_کرو HARDFAULT_FILENO 4
#تعریف_کرو HARDFAULT_راستہ BBSRAM_راستہ "" STRINGIFY(HARDFAULT_FILENO)

#تعریف_کرو HARDFAULT_MAX_ULOG_FILE_LEN 64 /* must be large enough to store the full path to the log file */

#تعریف_کرو BBSRAM_سائز_FN0 (حجم(صحیح))
#تعریف_کرو BBSRAM_سائز_FN1 384     /* greater then 2.5 times the size of vehicle_status_s */
#تعریف_کرو BBSRAM_سائز_FN2 384     /* greater then 2.5 times the size of vehicle_status_s */
#تعریف_کرو BBSRAM_سائز_FN3 HARDFAULT_MAX_ULOG_FILE_LEN
#تعریف_کرو BBSRAM_سائز_FN4 -1

/* The following guides in the amount of the user and interrupt stack
 * data we can save. The amount of storage left will dictate the actual
 * number of entries of the user stack data saved. If it is too big
 * It will be truncated by the call to stm32_bbsram_savepanic
 */
#تعریف_کرو BBSRAM_سرخی_سائز 20 /* This is an assumption */
#تعریف_کرو BBSRAM_مستعمل ((5*BBSRAM_سرخی_سائز)+(BBSRAM_سائز_FN0+BBSRAM_سائز_FN1+BBSRAM_سائز_FN2+BBSRAM_سائز_FN3))
#تعریف_کرو BBSRAM_REAMINING (PX4_BBSRAM_سائز-BBSRAM_مستعمل)
#اگر CONFIG_ARCH_INTERRUPTSTACK <= 3
#  define BBSRAM_نمبر_STACKS 1
#ورنہ
#  define BBSRAM_نمبر_STACKS 2
#اگر_ختم
#تعریف_کرو BBSRAM_FIXED_ELEMENTS_سائز (حجم(info_s))
#تعریف_کرو BBSRAM_LEFTOVER (BBSRAM_REAMINING-BBSRAM_FIXED_ELEMENTS_سائز)

#تعریف_کرو CONFIG_ISTACK_سائز (BBSRAM_LEFTOVER/BBSRAM_نمبر_STACKS/حجم(ڈھیر_لفظ_t))
#تعریف_کرو CONFIG_USTACK_سائز (BBSRAM_LEFTOVER/BBSRAM_نمبر_STACKS/حجم(ڈھیر_لفظ_t))

/* The path to the Battery Backed up SRAM */
#تعریف_کرو BBSRAM_راستہ "/fs/bbr"
/* The sizes of the files to create (-1) use rest of BBSRAM memory */
#تعریف_کرو BSRAM_FILE_SIZES { \
		BBSRAM_سائز_FN0,   /* For Time stamp only */                  \
		BBSRAM_سائز_FN1,   /* For Current Flight Parameters Copy A */ \
		BBSRAM_سائز_FN2,   /* For Current Flight Parameters Copy B */ \
		BBSRAM_سائز_FN3,   /* For the latest ULog file path */        \
		BBSRAM_سائز_FN4,   /* For the Panic Log use rest of space */  \
		0                  /* End of table marker */                  \
	}
#وگرنہ_اگر defined(HAS_PROGMEM)

قسمتعریف ساخت progmem_s dump_s;

#اگر CONFIG_ARCH_INTERRUPTSTACK <= 3
#  define PROGMEM_نمبر_STACKS 1
#ورنہ
#  define PROGMEM_نمبر_STACKS 2
#اگر_ختم
#تعریف_کرو PROGMEM_DUMP_FIXED_ELEMENTS_سائز (حجم(info_s))
#تعریف_کرو PROGMEM_DUMP_LEFTOVER (PROGMEM_DUMP_ڈھیر_سائز-PROGMEM_DUMP_FIXED_ELEMENTS_سائز)

#تعریف_کرو CONFIG_ISTACK_سائز (PROGMEM_DUMP_LEFTOVER/PROGMEM_نمبر_STACKS/حجم(ڈھیر_لفظ_t))
#تعریف_کرو CONFIG_USTACK_سائز (PROGMEM_DUMP_LEFTOVER/PROGMEM_نمبر_STACKS/حجم(ڈھیر_لفظ_t))

#تعریف_کرو HARDFAULT_ULOG_FILENO 2
#تعریف_کرو HARDFAULT_ULOG_راستہ PROGMEM_راستہ "" STRINGIFY(HARDFAULT_ULOG_FILENO)
#تعریف_کرو HARDFAULT_FILENO 3
#تعریف_کرو HARDFAULT_راستہ PROGMEM_راستہ "" STRINGIFY(HARDFAULT_FILENO)

#تعریف_کرو HARDFAULT_MAX_ULOG_FILE_LEN 64 /* must be large enough to store the full path to the log file */

#تعریف_کرو PROGMEM_سائز_FN0 384     /* greater then 2.5 times the size of vehicle_status_s */
#تعریف_کرو PROGMEM_سائز_FN1 384     /* greater then 2.5 times the size of vehicle_status_s */
#تعریف_کرو PROGMEM_سائز_FN2 HARDFAULT_MAX_ULOG_FILE_LEN
#تعریف_کرو PROGMEM_سائز_FN3 -1

/* The following guides in the amount of the user and interrupt stack
 * data we can save. The amount of storage left will dictate the actual
 * number of entries of the user stack data saved. If it is too big
 * It will be truncated by the call to stm32_bbsram_savepanic
 */

#تعریف_کرو PROGMEM_مستعمل ((5*PROGMEM_سرخی_سائز)+(PROGMEM_سائز_FN0+PROGMEM_سائز_FN1+PROGMEM_سائز_FN2+PROGMEM_سائز_FN3))
#تعریف_کرو PROGMEM_REAMINING (PX4_PROGMEM_سائز-PROGMEM_مستعمل)
#اگر CONFIG_ARCH_INTERRUPTSTACK <= 3
#  define PROGMEM_نمبر_STACKS 1
#ورنہ
#  define PROGMEM_نمبر_STACKS 2
#اگر_ختم
#تعریف_کرو PROGMEM_FIXED_ELEMENTS_سائز (حجم(info_s))
#تعریف_کرو PROGMEM_LEFTOVER (PROGMEM_REAMINING-PROGMEM_FIXED_ELEMENTS_سائز)

#تعریف_کرو PROGMEM_FILE_گنتی 4
/* The path to the Battery Backed up SRAM */
#تعریف_کرو PROGMEM_راستہ "/mnt/hardfault"
/* The sizes of the files to create (-1) use rest of BBSRAM memory */
#تعریف_کرو PROGMEM_FILE_SIZES { \
		PROGMEM_سائز_FN0,   /* For Current Flight Parameters Copy A */ \
		PROGMEM_سائز_FN1,   /* For Current Flight Parameters Copy B */ \
		PROGMEM_سائز_FN2,  /* For the latest ULog file path */        \
		PROGMEM_سائز_FN3,   /* For the Panic Log use rest of space */  \
		0                  /* End of table marker */                  \
	}
#وگرنہ_اگر defined(HAS_SSARC)

قسمتعریف ساخت ssarc_s dump_s;

#تعریف_کرو HARDFAULT_REBOOT_FILENO 0
#تعریف_کرو HARDFAULT_REBOOT_راستہ SSARC_DUMP_راستہ "" STRINGIFY(HARDFAULT_REBOOT_FILENO)
#تعریف_کرو HARDFAULT_ULOG_FILENO 3
#تعریف_کرو HARDFAULT_ULOG_راستہ SSARC_DUMP_راستہ "" STRINGIFY(HARDFAULT_ULOG_FILENO)
#تعریف_کرو HARDFAULT_FILENO 4
#تعریف_کرو HARDFAULT_راستہ SSARC_DUMP_راستہ "" STRINGIFY(HARDFAULT_FILENO)

#تعریف_کرو HARDFAULT_MAX_ULOG_FILE_LEN 64 /* must be large enough to store the full path to the log file */

#تعریف_کرو SSARC_DUMP_سائز_FN0 ((((حجم(صحیح)) / PX4_SSARC_بلاک_ڈیٹا) + 1) * PX4_SSARC_بلاک_ڈیٹا)
#تعریف_کرو SSARC_DUMP_سائز_FN1 (((384 / PX4_SSARC_بلاک_ڈیٹا) + 1) * PX4_SSARC_بلاک_ڈیٹا)     /* greater then 2.5 times the size of vehicle_status_s */
#تعریف_کرو SSARC_DUMP_سائز_FN2 (((384 / PX4_SSARC_بلاک_ڈیٹا) + 1) * PX4_SSARC_بلاک_ڈیٹا)     /* greater then 2.5 times the size of vehicle_status_s */
#تعریف_کرو SSARC_DUMP_سائز_FN3 (((HARDFAULT_MAX_ULOG_FILE_LEN / PX4_SSARC_بلاک_ڈیٹا) + 1) * PX4_SSARC_بلاک_ڈیٹا)
#تعریف_کرو SSARC_DUMP_سائز_FN4 -1

/* The following guides in the amount of the user and interrupt stack
 * data we can save. The amount of storage left will dictate the actual
 * number of entries of the user stack data saved. If it is too big
 * It will be truncated by the call to savepanic
 */
#تعریف_کرو SSARC_DUMP_سرخی_سائز PX4_SSARC_سرخی_سائز + 32 /* This is an assumption */
#تعریف_کرو SSARC_DUMP_مستعمل ((5*SSARC_DUMP_سرخی_سائز)+(SSARC_DUMP_سائز_FN0+SSARC_DUMP_سائز_FN1+SSARC_DUMP_سائز_FN2+SSARC_DUMP_سائز_FN3))
#تعریف_کرو SSARC_DUMP_بقایا (PX4_SSARC_DUMP_سائز-SSARC_DUMP_مستعمل)
#اگر CONFIG_ARCH_INTERRUPTSTACK <= 3
#  define SSARC_DUMP_نمبر_STACKS 1
#ورنہ
#  define SSARC_DUMP_نمبر_STACKS 2
#اگر_ختم
#تعریف_کرو SSARC_DUMP_FIXED_ELEMENTS_سائز (حجم(info_s))
#تعریف_کرو SSARC_DUMP_LEFTOVER (SSARC_DUMP_بقایا-SSARC_DUMP_FIXED_ELEMENTS_سائز)

#تعریف_کرو CONFIG_ISTACK_سائز (SSARC_DUMP_LEFTOVER/SSARC_DUMP_نمبر_STACKS/حجم(ڈھیر_لفظ_t))
#تعریف_کرو CONFIG_USTACK_سائز (SSARC_DUMP_LEFTOVER/SSARC_DUMP_نمبر_STACKS/حجم(ڈھیر_لفظ_t))

#تعریف_کرو SSARC_DUMP_FILE_گنتی 5
/* The path to the Battery Backed up SRAM */
#تعریف_کرو SSARC_DUMP_راستہ "/fs/ssarc"
/* The sizes of the files to create (-1) use rest of BBSRAM memory */
#تعریف_کرو SSARC_DUMP_FILE_SIZES { \
		SSARC_DUMP_سائز_FN0,   /* For Time stamp only */                  \
		SSARC_DUMP_سائز_FN1,   /* For Current Flight Parameters Copy A */ \
		SSARC_DUMP_سائز_FN2,   /* For Current Flight Parameters Copy B */ \
		SSARC_DUMP_سائز_FN3,   /* For the latest ULog file path */        \
		SSARC_DUMP_سائز_FN4,   /* For the Panic Log use rest of space */  \
		0                  /* End of table marker */                  \
	}
#ورنہ /* HAS_SSARC */

#تعریف_کرو CONFIG_ISTACK_سائز 0
#تعریف_کرو CONFIG_USTACK_سائز 0

#اگر_ختم

/* For Assert keep this much of the file name*/
#تعریف_کرو MAX_FILE_راستہ_لمبائی 40


/* Fixed size strings
 * To change a format add the number of chars not represented by the format
 * Specifier to the xxxx_NUM definei.e %Y is YYYY so add 2 and %s is -2
 * Also xxxxTIME_FMT need to match in size. See CCASERT in hardfault_log.c
 */
#تعریف_کرو LOG_راستہ_بنیاد       CONFIG_BOARD_جڑ_راستہ "/"
#تعریف_کرو LOG_راستہ_بنیاد_LEN    ((array_سائز(LOG_راستہ_بنیاد))-1)

#تعریف_کرو LOG_NAME_FMT        "fault_%s.log"
#تعریف_کرو LOG_NAME_عدد         (     -2    )
#تعریف_کرو LOG_NAME_LEN         ((array_سائز(LOG_NAME_FMT)-1) + LOG_NAME_عدد)

#تعریف_کرو وقت_FMT             "%Y_%m_%d_%H_%M_%S"
#تعریف_کرو وقت_FMT_عدد         (2+ 0+ 0+ 0+ 0+ 0)
#تعریف_کرو وقت_FMT_LEN         (((array_سائز(وقت_FMT)-1) + وقت_FMT_عدد))

#تعریف_کرو LOG_راستہ_LEN         ((LOG_راستہ_بنیاد_LEN + LOG_NAME_LEN + وقت_FMT_LEN))

#تعریف_کرو سرخی_وقت_FMT      "%Y-%m-%d-%H:%M:%S"
#تعریف_کرو سرخی_وقت_FMT_عدد  (2+ 0+ 0+ 0+ 0+ 0)
#تعریف_کرو سرخی_وقت_FMT_LEN  (((array_سائز(سرخی_وقت_FMT)-1) + سرخی_وقت_FMT_عدد))

/* Select which format to use. On a terminal the details are at the bottom
 * and in a file they are at the top
 */
#تعریف_کرو HARDFAULT_DISPLAY_FORMAT 1
#تعریف_کرو HARDFAULT_FILE_FORMAT    0

/****************************************************************************
 * Public Types
 ****************************************************************************/

/* Used for stack frame storage */
قسمتعریف uint32_t ڈھیر_لفظ_t;

/* Stack related data */

قسمتعریف ساخت {
	uint32_t sp;
	uint32_t چوٹی;
	uint32_t سائز;

} ڈھیر_s;

قسمتعریف ساخت {
	ڈھیر_s user;
#اگر CONFIG_ARCH_INTERRUPTSTACK > 3
	ڈھیر_s مداخلت;
#اگر_ختم

} px4_ڈھیر_t;

/* Not Used for reference only */

قسمتعریف ساخت {
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
قسمتعریف شمار {
	eRegsPresent          = 0x01,
	e_User_ڈھیر_Present     = 0x02,
	e_Int_ڈھیر_Present      = 0x04,
	eFaultRegPresent      = 0x08,
	e_Invalid_User_ڈھیر_Ptr  = 0x20,
	e_Invalid_Int_ڈھیر_Prt   = 0x40,
} fault_flags_t;

قسمتعریف ساخت {
	uint32_t              cfsr;  /* 0x0d28 Configurable fault status register */
	uint32_t              hfsr;  /* 0x0d2c Hard fault status register */
	uint32_t              dfsr;  /* 0x0d30 Debug fault status register */
	uint32_t              mmfsr; /* 0x0d34 Mem manage address register */
	uint32_t              bfsr;  /* 0x0d38 Bus fault address register */
	uint32_t              afsr;  /* 0x0d3c Auxiliary fault status register */
#اگر defined(CONFIG_ARCH_CORTEXM7)
	uint32_t              abfsr; /* 0x0fa8 Auxiliary Bus Fault Status Register */
#اگر_ختم

} fault_regs_s;

قسمتعریف ساخت {
	fault_flags_t         flags;                  /* What is in the dump */
	uintptr_t             current_regs;           /* Used to validate the dump */
	صحیح                   lineno;                 /* __LINE__ to up_assert */
	صحیح                   pid;                    /* Process ID */
	uint32_t              regs[XCPTCONTEXT_REGS]; /* Interrupt register save area */
	fault_regs_s          fault_regs;             /* NVIC status */
	px4_ڈھیر_t           stacks;                 /* Stack info */
#اگر CONFIG_TASK_NAME_سائز > 0
	حرف                  name[CONFIG_TASK_NAME_سائز + 1]; /* Task name (with NULL
													* terminator) */
#اگر_ختم
	حرف                  filename[MAX_FILE_راستہ_لمبائی]; /* the Last of chars in
												  * __FILE__ to up_assert */
} info_s;

قسمتعریف ساخت {
	info_s    info;                       /* The info */
#اگر CONFIG_ARCH_INTERRUPTSTACK > 3      /* The amount of stack data is compile time
									 * sized backed on what is left after the
									 * other BBSRAM files are defined
									 * The order is such that only the
									 * ustack should be truncated
									 */
	ڈھیر_لفظ_t istack[CONFIG_USTACK_سائز];
#اگر_ختم
	ڈھیر_لفظ_t ustack[CONFIG_ISTACK_سائز];
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
صحیح hardfault_check_حالت(حرف *caller) weak_function;

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
صحیح hardfault_write(حرف *caller, صحیح fd, صحیح format, bool rearm) weak_function;

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
صحیح hardfault_rearm(حرف *caller) weak_function;

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
صحیح hardfault_increment_reboot(حرف *caller, bool reset) weak_function;

__END_DECLS
