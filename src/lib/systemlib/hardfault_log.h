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
#निर्देशः once
/****************************************************************************
 * Included Files
 ****************************************************************************/
#अन्तर्भाव <px4_platform_common/px4_config.h>
#अन्तर्भाव <px4_arch/micro_hal.h>
#अन्तर्भाव <systemlib/px4_macros.h>
#अन्तर्भाव <nuttx/irq.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#यदि defined(HAS_BBSRAM)

#अन्तर्भाव <stm32_bbsram.h>
प्रकारनिर्देश संरचना bbsramd_s dump_s;

#परिभाषय HARDFAULT_REBOOT_FILENO 0
#परिभाषय HARDFAULT_REBOOT_मार्गः BBSRAM_मार्गः "" STRINGIFY(HARDFAULT_REBOOT_FILENO)
#परिभाषय HARDFAULT_ULOG_FILENO 3
#परिभाषय HARDFAULT_ULOG_मार्गः BBSRAM_मार्गः "" STRINGIFY(HARDFAULT_ULOG_FILENO)
#परिभाषय HARDFAULT_FILENO 4
#परिभाषय HARDFAULT_मार्गः BBSRAM_मार्गः "" STRINGIFY(HARDFAULT_FILENO)

#परिभाषय HARDFAULT_MAX_ULOG_FILE_LEN 64 /* must be large enough to store the full path to the log file */

#परिभाषय BBSRAM_आकार_FN0 (परिमाण(पूर्णाङ्क))
#परिभाषय BBSRAM_आकार_FN1 384     /* greater then 2.5 times the size of vehicle_status_s */
#परिभाषय BBSRAM_आकार_FN2 384     /* greater then 2.5 times the size of vehicle_status_s */
#परिभाषय BBSRAM_आकार_FN3 HARDFAULT_MAX_ULOG_FILE_LEN
#परिभाषय BBSRAM_आकार_FN4 -1

/* The following guides in the amount of the user and interrupt stack
 * data we can save. The amount of storage left will dictate the actual
 * number of entries of the user stack data saved. If it is too big
 * It will be truncated by the call to stm32_bbsram_savepanic
 */
#परिभाषय BBSRAM_शीर्षकम्_आकार 20 /* This is an assumption */
#परिभाषय BBSRAM_प्रयुक्तम् ((5*BBSRAM_शीर्षकम्_आकार)+(BBSRAM_आकार_FN0+BBSRAM_आकार_FN1+BBSRAM_आकार_FN2+BBSRAM_आकार_FN3))
#परिभाषय BBSRAM_REAMINING (PX4_BBSRAM_आकार-BBSRAM_प्रयुक्तम्)
#यदि CONFIG_ARCH_INTERRUPTSTACK <= 3
#  define BBSRAM_सङ्ख्या_STACKS 1
#अन्यथा
#  define BBSRAM_सङ्ख्या_STACKS 2
#यदि_अन्तः
#परिभाषय BBSRAM_FIXED_ELEMENTS_आकार (परिमाण(info_s))
#परिभाषय BBSRAM_LEFTOVER (BBSRAM_REAMINING-BBSRAM_FIXED_ELEMENTS_आकार)

#परिभाषय CONFIG_ISTACK_आकार (BBSRAM_LEFTOVER/BBSRAM_सङ्ख्या_STACKS/परिमाण(राशिः_word_t))
#परिभाषय CONFIG_USTACK_आकार (BBSRAM_LEFTOVER/BBSRAM_सङ्ख्या_STACKS/परिमाण(राशिः_word_t))

/* The path to the Battery Backed up SRAM */
#परिभाषय BBSRAM_मार्गः "/fs/bbr"
/* The sizes of the files to create (-1) use rest of BBSRAM memory */
#परिभाषय BSRAM_FILE_SIZES { \
		BBSRAM_आकार_FN0,   /* For Time stamp only */                  \
		BBSRAM_आकार_FN1,   /* For Current Flight Parameters Copy A */ \
		BBSRAM_आकार_FN2,   /* For Current Flight Parameters Copy B */ \
		BBSRAM_आकार_FN3,   /* For the latest ULog file path */        \
		BBSRAM_आकार_FN4,   /* For the Panic Log use rest of space */  \
		0                  /* End of table marker */                  \
	}
#नोचेद्यदि defined(HAS_PROGMEM)

प्रकारनिर्देश संरचना progmem_s dump_s;

#यदि CONFIG_ARCH_INTERRUPTSTACK <= 3
#  define PROGMEM_सङ्ख्या_STACKS 1
#अन्यथा
#  define PROGMEM_सङ्ख्या_STACKS 2
#यदि_अन्तः
#परिभाषय PROGMEM_DUMP_FIXED_ELEMENTS_आकार (परिमाण(info_s))
#परिभाषय PROGMEM_DUMP_LEFTOVER (PROGMEM_DUMP_राशिः_आकार-PROGMEM_DUMP_FIXED_ELEMENTS_आकार)

#परिभाषय CONFIG_ISTACK_आकार (PROGMEM_DUMP_LEFTOVER/PROGMEM_सङ्ख्या_STACKS/परिमाण(राशिः_word_t))
#परिभाषय CONFIG_USTACK_आकार (PROGMEM_DUMP_LEFTOVER/PROGMEM_सङ्ख्या_STACKS/परिमाण(राशिः_word_t))

#परिभाषय HARDFAULT_ULOG_FILENO 2
#परिभाषय HARDFAULT_ULOG_मार्गः PROGMEM_मार्गः "" STRINGIFY(HARDFAULT_ULOG_FILENO)
#परिभाषय HARDFAULT_FILENO 3
#परिभाषय HARDFAULT_मार्गः PROGMEM_मार्गः "" STRINGIFY(HARDFAULT_FILENO)

#परिभाषय HARDFAULT_MAX_ULOG_FILE_LEN 64 /* must be large enough to store the full path to the log file */

#परिभाषय PROGMEM_आकार_FN0 384     /* greater then 2.5 times the size of vehicle_status_s */
#परिभाषय PROGMEM_आकार_FN1 384     /* greater then 2.5 times the size of vehicle_status_s */
#परिभाषय PROGMEM_आकार_FN2 HARDFAULT_MAX_ULOG_FILE_LEN
#परिभाषय PROGMEM_आकार_FN3 -1

/* The following guides in the amount of the user and interrupt stack
 * data we can save. The amount of storage left will dictate the actual
 * number of entries of the user stack data saved. If it is too big
 * It will be truncated by the call to stm32_bbsram_savepanic
 */

#परिभाषय PROGMEM_प्रयुक्तम् ((5*PROGMEM_शीर्षकम्_आकार)+(PROGMEM_आकार_FN0+PROGMEM_आकार_FN1+PROGMEM_आकार_FN2+PROGMEM_आकार_FN3))
#परिभाषय PROGMEM_REAMINING (PX4_PROGMEM_आकार-PROGMEM_प्रयुक्तम्)
#यदि CONFIG_ARCH_INTERRUPTSTACK <= 3
#  define PROGMEM_सङ्ख्या_STACKS 1
#अन्यथा
#  define PROGMEM_सङ्ख्या_STACKS 2
#यदि_अन्तः
#परिभाषय PROGMEM_FIXED_ELEMENTS_आकार (परिमाण(info_s))
#परिभाषय PROGMEM_LEFTOVER (PROGMEM_REAMINING-PROGMEM_FIXED_ELEMENTS_आकार)

#परिभाषय PROGMEM_FILE_गणनम् 4
/* The path to the Battery Backed up SRAM */
#परिभाषय PROGMEM_मार्गः "/mnt/hardfault"
/* The sizes of the files to create (-1) use rest of BBSRAM memory */
#परिभाषय PROGMEM_FILE_SIZES { \
		PROGMEM_आकार_FN0,   /* For Current Flight Parameters Copy A */ \
		PROGMEM_आकार_FN1,   /* For Current Flight Parameters Copy B */ \
		PROGMEM_आकार_FN2,  /* For the latest ULog file path */        \
		PROGMEM_आकार_FN3,   /* For the Panic Log use rest of space */  \
		0                  /* End of table marker */                  \
	}
#नोचेद्यदि defined(HAS_SSARC)

प्रकारनिर्देश संरचना ssarc_s dump_s;

#परिभाषय HARDFAULT_REBOOT_FILENO 0
#परिभाषय HARDFAULT_REBOOT_मार्गः SSARC_DUMP_मार्गः "" STRINGIFY(HARDFAULT_REBOOT_FILENO)
#परिभाषय HARDFAULT_ULOG_FILENO 3
#परिभाषय HARDFAULT_ULOG_मार्गः SSARC_DUMP_मार्गः "" STRINGIFY(HARDFAULT_ULOG_FILENO)
#परिभाषय HARDFAULT_FILENO 4
#परिभाषय HARDFAULT_मार्गः SSARC_DUMP_मार्गः "" STRINGIFY(HARDFAULT_FILENO)

#परिभाषय HARDFAULT_MAX_ULOG_FILE_LEN 64 /* must be large enough to store the full path to the log file */

#परिभाषय SSARC_DUMP_आकार_FN0 ((((परिमाण(पूर्णाङ्क)) / PX4_SSARC_खण्डम्_दत्त) + 1) * PX4_SSARC_खण्डम्_दत्त)
#परिभाषय SSARC_DUMP_आकार_FN1 (((384 / PX4_SSARC_खण्डम्_दत्त) + 1) * PX4_SSARC_खण्डम्_दत्त)     /* greater then 2.5 times the size of vehicle_status_s */
#परिभाषय SSARC_DUMP_आकार_FN2 (((384 / PX4_SSARC_खण्डम्_दत्त) + 1) * PX4_SSARC_खण्डम्_दत्त)     /* greater then 2.5 times the size of vehicle_status_s */
#परिभाषय SSARC_DUMP_आकार_FN3 (((HARDFAULT_MAX_ULOG_FILE_LEN / PX4_SSARC_खण्डम्_दत्त) + 1) * PX4_SSARC_खण्डम्_दत्त)
#परिभाषय SSARC_DUMP_आकार_FN4 -1

/* The following guides in the amount of the user and interrupt stack
 * data we can save. The amount of storage left will dictate the actual
 * number of entries of the user stack data saved. If it is too big
 * It will be truncated by the call to savepanic
 */
#परिभाषय SSARC_DUMP_शीर्षकम्_आकार PX4_SSARC_शीर्षकम्_आकार + 32 /* This is an assumption */
#परिभाषय SSARC_DUMP_प्रयुक्तम् ((5*SSARC_DUMP_शीर्षकम्_आकार)+(SSARC_DUMP_आकार_FN0+SSARC_DUMP_आकार_FN1+SSARC_DUMP_आकार_FN2+SSARC_DUMP_आकार_FN3))
#परिभाषय SSARC_DUMP_REMAINING (PX4_SSARC_DUMP_आकार-SSARC_DUMP_प्रयुक्तम्)
#यदि CONFIG_ARCH_INTERRUPTSTACK <= 3
#  define SSARC_DUMP_सङ्ख्या_STACKS 1
#अन्यथा
#  define SSARC_DUMP_सङ्ख्या_STACKS 2
#यदि_अन्तः
#परिभाषय SSARC_DUMP_FIXED_ELEMENTS_आकार (परिमाण(info_s))
#परिभाषय SSARC_DUMP_LEFTOVER (SSARC_DUMP_REMAINING-SSARC_DUMP_FIXED_ELEMENTS_आकार)

#परिभाषय CONFIG_ISTACK_आकार (SSARC_DUMP_LEFTOVER/SSARC_DUMP_सङ्ख्या_STACKS/परिमाण(राशिः_word_t))
#परिभाषय CONFIG_USTACK_आकार (SSARC_DUMP_LEFTOVER/SSARC_DUMP_सङ्ख्या_STACKS/परिमाण(राशिः_word_t))

#परिभाषय SSARC_DUMP_FILE_गणनम् 5
/* The path to the Battery Backed up SRAM */
#परिभाषय SSARC_DUMP_मार्गः "/fs/ssarc"
/* The sizes of the files to create (-1) use rest of BBSRAM memory */
#परिभाषय SSARC_DUMP_FILE_SIZES { \
		SSARC_DUMP_आकार_FN0,   /* For Time stamp only */                  \
		SSARC_DUMP_आकार_FN1,   /* For Current Flight Parameters Copy A */ \
		SSARC_DUMP_आकार_FN2,   /* For Current Flight Parameters Copy B */ \
		SSARC_DUMP_आकार_FN3,   /* For the latest ULog file path */        \
		SSARC_DUMP_आकार_FN4,   /* For the Panic Log use rest of space */  \
		0                  /* End of table marker */                  \
	}
#अन्यथा /* HAS_SSARC */

#परिभाषय CONFIG_ISTACK_आकार 0
#परिभाषय CONFIG_USTACK_आकार 0

#यदि_अन्तः

/* For Assert keep this much of the file name*/
#परिभाषय MAX_FILE_मार्गः_दीर्घता 40


/* Fixed size strings
 * To change a format add the number of chars not represented by the format
 * Specifier to the xxxx_NUM definei.e %Y is YYYY so add 2 and %s is -2
 * Also xxxxTIME_FMT need to match in size. See CCASERT in hardfault_log.c
 */
#परिभाषय LOG_मार्गः_आधारः       CONFIG_BOARD_मूलम्_मार्गः "/"
#परिभाषय LOG_मार्गः_आधारः_LEN    ((array_आकार(LOG_मार्गः_आधारः))-1)

#परिभाषय LOG_NAME_FMT        "fault_%s.log"
#परिभाषय LOG_NAME_अङ्कः         (     -2    )
#परिभाषय LOG_NAME_LEN         ((array_आकार(LOG_NAME_FMT)-1) + LOG_NAME_अङ्कः)

#परिभाषय TIME_FMT             "%Y_%m_%d_%H_%M_%S"
#परिभाषय TIME_FMT_अङ्कः         (2+ 0+ 0+ 0+ 0+ 0)
#परिभाषय TIME_FMT_LEN         (((array_आकार(TIME_FMT)-1) + TIME_FMT_अङ्कः))

#परिभाषय LOG_मार्गः_LEN         ((LOG_मार्गः_आधारः_LEN + LOG_NAME_LEN + TIME_FMT_LEN))

#परिभाषय शीर्षकम्_TIME_FMT      "%Y-%m-%d-%H:%M:%S"
#परिभाषय शीर्षकम्_TIME_FMT_अङ्कः  (2+ 0+ 0+ 0+ 0+ 0)
#परिभाषय शीर्षकम्_TIME_FMT_LEN  (((array_आकार(शीर्षकम्_TIME_FMT)-1) + शीर्षकम्_TIME_FMT_अङ्कः))

/* Select which format to use. On a terminal the details are at the bottom
 * and in a file they are at the top
 */
#परिभाषय HARDFAULT_DISPLAY_FORMAT 1
#परिभाषय HARDFAULT_FILE_FORMAT    0

/****************************************************************************
 * Public Types
 ****************************************************************************/

/* Used for stack frame storage */
प्रकारनिर्देश uint32_t राशिः_word_t;

/* Stack related data */

प्रकारनिर्देश संरचना {
	uint32_t sp;
	uint32_t शिखरम्;
	uint32_t आकार;

} राशिः_s;

प्रकारनिर्देश संरचना {
	राशिः_s user;
#यदि CONFIG_ARCH_INTERRUPTSTACK > 3
	राशिः_s व्यवधानम्;
#यदि_अन्तः

} px4_राशिः_t;

/* Not Used for reference only */

प्रकारनिर्देश संरचना {
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
प्रकारनिर्देश गणना {
	eRegsPresent          = 0x01,
	e_User_राशिः_Present     = 0x02,
	e_Int_राशिः_Present      = 0x04,
	eFaultRegPresent      = 0x08,
	e_Invalid_User_राशिः_Ptr  = 0x20,
	e_Invalid_Int_राशिः_Prt   = 0x40,
} fault_flags_t;

प्रकारनिर्देश संरचना {
	uint32_t              cfsr;  /* 0x0d28 Configurable fault status register */
	uint32_t              hfsr;  /* 0x0d2c Hard fault status register */
	uint32_t              dfsr;  /* 0x0d30 Debug fault status register */
	uint32_t              mmfsr; /* 0x0d34 Mem manage address register */
	uint32_t              bfsr;  /* 0x0d38 Bus fault address register */
	uint32_t              afsr;  /* 0x0d3c Auxiliary fault status register */
#यदि defined(CONFIG_ARCH_CORTEXM7)
	uint32_t              abfsr; /* 0x0fa8 Auxiliary Bus Fault Status Register */
#यदि_अन्तः

} fault_regs_s;

प्रकारनिर्देश संरचना {
	fault_flags_t         flags;                  /* What is in the dump */
	uintptr_t             current_regs;           /* Used to validate the dump */
	पूर्णाङ्क                   lineno;                 /* __LINE__ to up_assert */
	पूर्णाङ्क                   pid;                    /* Process ID */
	uint32_t              regs[XCPTCONTEXT_REGS]; /* Interrupt register save area */
	fault_regs_s          fault_regs;             /* NVIC status */
	px4_राशिः_t           stacks;                 /* Stack info */
#यदि CONFIG_TASK_NAME_आकार > 0
	वर्ण                  name[CONFIG_TASK_NAME_आकार + 1]; /* Task name (with NULL
													* terminator) */
#यदि_अन्तः
	वर्ण                  filename[MAX_FILE_मार्गः_दीर्घता]; /* the Last of chars in
												  * __FILE__ to up_assert */
} info_s;

प्रकारनिर्देश संरचना {
	info_s    info;                       /* The info */
#यदि CONFIG_ARCH_INTERRUPTSTACK > 3      /* The amount of stack data is compile time
									 * sized backed on what is left after the
									 * other BBSRAM files are defined
									 * The order is such that only the
									 * ustack should be truncated
									 */
	राशिः_word_t istack[CONFIG_USTACK_आकार];
#यदि_अन्तः
	राशिः_word_t ustack[CONFIG_ISTACK_आकार];
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
पूर्णाङ्क hardfault_check_स्थितिः(वर्ण *caller) weak_function;

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
पूर्णाङ्क hardfault_write(वर्ण *caller, पूर्णाङ्क fd, पूर्णाङ्क format, bool rearm) weak_function;

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
पूर्णाङ्क hardfault_rearm(वर्ण *caller) weak_function;

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
पूर्णाङ्क hardfault_increment_reboot(वर्ण *caller, bool reset) weak_function;

__END_DECLS
