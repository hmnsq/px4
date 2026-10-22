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
#సూచన once
/****************************************************************************
 * Included Files
 ****************************************************************************/
#అన్తర్భావ <px4_platform_common/px4_config.h>
#అన్తర్భావ <px4_arch/micro_hal.h>
#అన్తర్భావ <systemlib/px4_macros.h>
#అన్తర్భావ <nuttx/irq.h>

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

#ఒకవేళ defined(HAS_BBSRAM)

#అన్తర్భావ <stm32_bbsram.h>
రకనిర్వచనం నిర్మాణం bbsramd_s dump_s;

#నిర్వచించు HARDFAULT_REBOOT_FILENO 0
#నిర్వచించు HARDFAULT_REBOOT_మార్గం BBSRAM_మార్గం "" STRINGIFY(HARDFAULT_REBOOT_FILENO)
#నిర్వచించు HARDFAULT_ULOG_FILENO 3
#నిర్వచించు HARDFAULT_ULOG_మార్గం BBSRAM_మార్గం "" STRINGIFY(HARDFAULT_ULOG_FILENO)
#నిర్వచించు HARDFAULT_FILENO 4
#నిర్వచించు HARDFAULT_మార్గం BBSRAM_మార్గం "" STRINGIFY(HARDFAULT_FILENO)

#నిర్వచించు HARDFAULT_MAX_ULOG_FILE_LEN 64 /* must be large enough to store the full path to the log file */

#నిర్వచించు BBSRAM_కొలత_FN0 (పరిమాణం(పూర్ణాంకం))
#నిర్వచించు BBSRAM_కొలత_FN1 384     /* greater then 2.5 times the size of vehicle_status_s */
#నిర్వచించు BBSRAM_కొలత_FN2 384     /* greater then 2.5 times the size of vehicle_status_s */
#నిర్వచించు BBSRAM_కొలత_FN3 HARDFAULT_MAX_ULOG_FILE_LEN
#నిర్వచించు BBSRAM_కొలత_FN4 -1

/* The following guides in the amount of the user and interrupt stack
 * data we can save. The amount of storage left will dictate the actual
 * number of entries of the user stack data saved. If it is too big
 * It will be truncated by the call to stm32_bbsram_savepanic
 */
#నిర్వచించు BBSRAM_శీర్షిక_కొలత 20 /* This is an assumption */
#నిర్వచించు BBSRAM_వాడిన ((5*BBSRAM_శీర్షిక_కొలత)+(BBSRAM_కొలత_FN0+BBSRAM_కొలత_FN1+BBSRAM_కొలత_FN2+BBSRAM_కొలత_FN3))
#నిర్వచించు BBSRAM_REAMINING (PX4_BBSRAM_కొలత-BBSRAM_వాడిన)
#ఒకవేళ CONFIG_ARCH_INTERRUPTSTACK <= 3
#  define BBSRAM_సంఖ్య_STACKS 1
#లేకపోతే
#  define BBSRAM_సంఖ్య_STACKS 2
#అయితే_ముగింపు
#నిర్వచించు BBSRAM_FIXED_ELEMENTS_కొలత (పరిమాణం(info_s))
#నిర్వచించు BBSRAM_LEFTOVER (BBSRAM_REAMINING-BBSRAM_FIXED_ELEMENTS_కొలత)

#నిర్వచించు CONFIG_ISTACK_కొలత (BBSRAM_LEFTOVER/BBSRAM_సంఖ్య_STACKS/పరిమాణం(దొంతర_మాట_t))
#నిర్వచించు CONFIG_USTACK_కొలత (BBSRAM_LEFTOVER/BBSRAM_సంఖ్య_STACKS/పరిమాణం(దొంతర_మాట_t))

/* The path to the Battery Backed up SRAM */
#నిర్వచించు BBSRAM_మార్గం "/fs/bbr"
/* The sizes of the files to create (-1) use rest of BBSRAM memory */
#నిర్వచించు BSRAM_FILE_SIZES { \
		BBSRAM_కొలత_FN0,   /* For Time stamp only */                  \
		BBSRAM_కొలత_FN1,   /* For Current Flight Parameters Copy A */ \
		BBSRAM_కొలత_FN2,   /* For Current Flight Parameters Copy B */ \
		BBSRAM_కొలత_FN3,   /* For the latest ULog file path */        \
		BBSRAM_కొలత_FN4,   /* For the Panic Log use rest of space */  \
		0                  /* End of table marker */                  \
	}
#లేదావేళ defined(HAS_PROGMEM)

రకనిర్వచనం నిర్మాణం progmem_s dump_s;

#ఒకవేళ CONFIG_ARCH_INTERRUPTSTACK <= 3
#  define PROGMEM_సంఖ్య_STACKS 1
#లేకపోతే
#  define PROGMEM_సంఖ్య_STACKS 2
#అయితే_ముగింపు
#నిర్వచించు PROGMEM_DUMP_FIXED_ELEMENTS_కొలత (పరిమాణం(info_s))
#నిర్వచించు PROGMEM_DUMP_LEFTOVER (PROGMEM_DUMP_దొంతర_కొలత-PROGMEM_DUMP_FIXED_ELEMENTS_కొలత)

#నిర్వచించు CONFIG_ISTACK_కొలత (PROGMEM_DUMP_LEFTOVER/PROGMEM_సంఖ్య_STACKS/పరిమాణం(దొంతర_మాట_t))
#నిర్వచించు CONFIG_USTACK_కొలత (PROGMEM_DUMP_LEFTOVER/PROGMEM_సంఖ్య_STACKS/పరిమాణం(దొంతర_మాట_t))

#నిర్వచించు HARDFAULT_ULOG_FILENO 2
#నిర్వచించు HARDFAULT_ULOG_మార్గం PROGMEM_మార్గం "" STRINGIFY(HARDFAULT_ULOG_FILENO)
#నిర్వచించు HARDFAULT_FILENO 3
#నిర్వచించు HARDFAULT_మార్గం PROGMEM_మార్గం "" STRINGIFY(HARDFAULT_FILENO)

#నిర్వచించు HARDFAULT_MAX_ULOG_FILE_LEN 64 /* must be large enough to store the full path to the log file */

#నిర్వచించు PROGMEM_కొలత_FN0 384     /* greater then 2.5 times the size of vehicle_status_s */
#నిర్వచించు PROGMEM_కొలత_FN1 384     /* greater then 2.5 times the size of vehicle_status_s */
#నిర్వచించు PROGMEM_కొలత_FN2 HARDFAULT_MAX_ULOG_FILE_LEN
#నిర్వచించు PROGMEM_కొలత_FN3 -1

/* The following guides in the amount of the user and interrupt stack
 * data we can save. The amount of storage left will dictate the actual
 * number of entries of the user stack data saved. If it is too big
 * It will be truncated by the call to stm32_bbsram_savepanic
 */

#నిర్వచించు PROGMEM_వాడిన ((5*PROGMEM_శీర్షిక_కొలత)+(PROGMEM_కొలత_FN0+PROGMEM_కొలత_FN1+PROGMEM_కొలత_FN2+PROGMEM_కొలత_FN3))
#నిర్వచించు PROGMEM_REAMINING (PX4_PROGMEM_కొలత-PROGMEM_వాడిన)
#ఒకవేళ CONFIG_ARCH_INTERRUPTSTACK <= 3
#  define PROGMEM_సంఖ్య_STACKS 1
#లేకపోతే
#  define PROGMEM_సంఖ్య_STACKS 2
#అయితే_ముగింపు
#నిర్వచించు PROGMEM_FIXED_ELEMENTS_కొలత (పరిమాణం(info_s))
#నిర్వచించు PROGMEM_LEFTOVER (PROGMEM_REAMINING-PROGMEM_FIXED_ELEMENTS_కొలత)

#నిర్వచించు PROGMEM_FILE_లెక్క 4
/* The path to the Battery Backed up SRAM */
#నిర్వచించు PROGMEM_మార్గం "/mnt/hardfault"
/* The sizes of the files to create (-1) use rest of BBSRAM memory */
#నిర్వచించు PROGMEM_FILE_SIZES { \
		PROGMEM_కొలత_FN0,   /* For Current Flight Parameters Copy A */ \
		PROGMEM_కొలత_FN1,   /* For Current Flight Parameters Copy B */ \
		PROGMEM_కొలత_FN2,  /* For the latest ULog file path */        \
		PROGMEM_కొలత_FN3,   /* For the Panic Log use rest of space */  \
		0                  /* End of table marker */                  \
	}
#లేదావేళ defined(HAS_SSARC)

రకనిర్వచనం నిర్మాణం ssarc_s dump_s;

#నిర్వచించు HARDFAULT_REBOOT_FILENO 0
#నిర్వచించు HARDFAULT_REBOOT_మార్గం SSARC_DUMP_మార్గం "" STRINGIFY(HARDFAULT_REBOOT_FILENO)
#నిర్వచించు HARDFAULT_ULOG_FILENO 3
#నిర్వచించు HARDFAULT_ULOG_మార్గం SSARC_DUMP_మార్గం "" STRINGIFY(HARDFAULT_ULOG_FILENO)
#నిర్వచించు HARDFAULT_FILENO 4
#నిర్వచించు HARDFAULT_మార్గం SSARC_DUMP_మార్గం "" STRINGIFY(HARDFAULT_FILENO)

#నిర్వచించు HARDFAULT_MAX_ULOG_FILE_LEN 64 /* must be large enough to store the full path to the log file */

#నిర్వచించు SSARC_DUMP_కొలత_FN0 ((((పరిమాణం(పూర్ణాంకం)) / PX4_SSARC_ఖండం_దత్తాంశం) + 1) * PX4_SSARC_ఖండం_దత్తాంశం)
#నిర్వచించు SSARC_DUMP_కొలత_FN1 (((384 / PX4_SSARC_ఖండం_దత్తాంశం) + 1) * PX4_SSARC_ఖండం_దత్తాంశం)     /* greater then 2.5 times the size of vehicle_status_s */
#నిర్వచించు SSARC_DUMP_కొలత_FN2 (((384 / PX4_SSARC_ఖండం_దత్తాంశం) + 1) * PX4_SSARC_ఖండం_దత్తాంశం)     /* greater then 2.5 times the size of vehicle_status_s */
#నిర్వచించు SSARC_DUMP_కొలత_FN3 (((HARDFAULT_MAX_ULOG_FILE_LEN / PX4_SSARC_ఖండం_దత్తాంశం) + 1) * PX4_SSARC_ఖండం_దత్తాంశం)
#నిర్వచించు SSARC_DUMP_కొలత_FN4 -1

/* The following guides in the amount of the user and interrupt stack
 * data we can save. The amount of storage left will dictate the actual
 * number of entries of the user stack data saved. If it is too big
 * It will be truncated by the call to savepanic
 */
#నిర్వచించు SSARC_DUMP_శీర్షిక_కొలత PX4_SSARC_శీర్షిక_కొలత + 32 /* This is an assumption */
#నిర్వచించు SSARC_DUMP_వాడిన ((5*SSARC_DUMP_శీర్షిక_కొలత)+(SSARC_DUMP_కొలత_FN0+SSARC_DUMP_కొలత_FN1+SSARC_DUMP_కొలత_FN2+SSARC_DUMP_కొలత_FN3))
#నిర్వచించు SSARC_DUMP_మిగిలిన (PX4_SSARC_DUMP_కొలత-SSARC_DUMP_వాడిన)
#ఒకవేళ CONFIG_ARCH_INTERRUPTSTACK <= 3
#  define SSARC_DUMP_సంఖ్య_STACKS 1
#లేకపోతే
#  define SSARC_DUMP_సంఖ్య_STACKS 2
#అయితే_ముగింపు
#నిర్వచించు SSARC_DUMP_FIXED_ELEMENTS_కొలత (పరిమాణం(info_s))
#నిర్వచించు SSARC_DUMP_LEFTOVER (SSARC_DUMP_మిగిలిన-SSARC_DUMP_FIXED_ELEMENTS_కొలత)

#నిర్వచించు CONFIG_ISTACK_కొలత (SSARC_DUMP_LEFTOVER/SSARC_DUMP_సంఖ్య_STACKS/పరిమాణం(దొంతర_మాట_t))
#నిర్వచించు CONFIG_USTACK_కొలత (SSARC_DUMP_LEFTOVER/SSARC_DUMP_సంఖ్య_STACKS/పరిమాణం(దొంతర_మాట_t))

#నిర్వచించు SSARC_DUMP_FILE_లెక్క 5
/* The path to the Battery Backed up SRAM */
#నిర్వచించు SSARC_DUMP_మార్గం "/fs/ssarc"
/* The sizes of the files to create (-1) use rest of BBSRAM memory */
#నిర్వచించు SSARC_DUMP_FILE_SIZES { \
		SSARC_DUMP_కొలత_FN0,   /* For Time stamp only */                  \
		SSARC_DUMP_కొలత_FN1,   /* For Current Flight Parameters Copy A */ \
		SSARC_DUMP_కొలత_FN2,   /* For Current Flight Parameters Copy B */ \
		SSARC_DUMP_కొలత_FN3,   /* For the latest ULog file path */        \
		SSARC_DUMP_కొలత_FN4,   /* For the Panic Log use rest of space */  \
		0                  /* End of table marker */                  \
	}
#లేకపోతే /* HAS_SSARC */

#నిర్వచించు CONFIG_ISTACK_కొలత 0
#నిర్వచించు CONFIG_USTACK_కొలత 0

#అయితే_ముగింపు

/* For Assert keep this much of the file name*/
#నిర్వచించు MAX_FILE_మార్గం_నిడివి 40


/* Fixed size strings
 * To change a format add the number of chars not represented by the format
 * Specifier to the xxxx_NUM definei.e %Y is YYYY so add 2 and %s is -2
 * Also xxxxTIME_FMT need to match in size. See CCASERT in hardfault_log.c
 */
#నిర్వచించు LOG_మార్గం_పునాది       CONFIG_BOARD_మూలం_మార్గం "/"
#నిర్వచించు LOG_మార్గం_పునాది_LEN    ((array_కొలత(LOG_మార్గం_పునాది))-1)

#నిర్వచించు LOG_NAME_FMT        "fault_%s.log"
#నిర్వచించు LOG_NAME_అంకె         (     -2    )
#నిర్వచించు LOG_NAME_LEN         ((array_కొలత(LOG_NAME_FMT)-1) + LOG_NAME_అంకె)

#నిర్వచించు సమయం_FMT             "%Y_%m_%d_%H_%M_%S"
#నిర్వచించు సమయం_FMT_అంకె         (2+ 0+ 0+ 0+ 0+ 0)
#నిర్వచించు సమయం_FMT_LEN         (((array_కొలత(సమయం_FMT)-1) + సమయం_FMT_అంకె))

#నిర్వచించు LOG_మార్గం_LEN         ((LOG_మార్గం_పునాది_LEN + LOG_NAME_LEN + సమయం_FMT_LEN))

#నిర్వచించు శీర్షిక_సమయం_FMT      "%Y-%m-%d-%H:%M:%S"
#నిర్వచించు శీర్షిక_సమయం_FMT_అంకె  (2+ 0+ 0+ 0+ 0+ 0)
#నిర్వచించు శీర్షిక_సమయం_FMT_LEN  (((array_కొలత(శీర్షిక_సమయం_FMT)-1) + శీర్షిక_సమయం_FMT_అంకె))

/* Select which format to use. On a terminal the details are at the bottom
 * and in a file they are at the top
 */
#నిర్వచించు HARDFAULT_DISPLAY_FORMAT 1
#నిర్వచించు HARDFAULT_FILE_FORMAT    0

/****************************************************************************
 * Public Types
 ****************************************************************************/

/* Used for stack frame storage */
రకనిర్వచనం uint32_t దొంతర_మాట_t;

/* Stack related data */

రకనిర్వచనం నిర్మాణం {
	uint32_t sp;
	uint32_t శిఖరం;
	uint32_t కొలత;

} దొంతర_s;

రకనిర్వచనం నిర్మాణం {
	దొంతర_s user;
#ఒకవేళ CONFIG_ARCH_INTERRUPTSTACK > 3
	దొంతర_s అంతరాయం;
#అయితే_ముగింపు

} px4_దొంతర_t;

/* Not Used for reference only */

రకనిర్వచనం నిర్మాణం {
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
రకనిర్వచనం గణన {
	eRegsPresent          = 0x01,
	e_User_దొంతర_Present     = 0x02,
	e_Int_దొంతర_Present      = 0x04,
	eFaultRegPresent      = 0x08,
	e_Invalid_User_దొంతర_Ptr  = 0x20,
	e_Invalid_Int_దొంతర_Prt   = 0x40,
} fault_flags_t;

రకనిర్వచనం నిర్మాణం {
	uint32_t              cfsr;  /* 0x0d28 Configurable fault status register */
	uint32_t              hfsr;  /* 0x0d2c Hard fault status register */
	uint32_t              dfsr;  /* 0x0d30 Debug fault status register */
	uint32_t              mmfsr; /* 0x0d34 Mem manage address register */
	uint32_t              bfsr;  /* 0x0d38 Bus fault address register */
	uint32_t              afsr;  /* 0x0d3c Auxiliary fault status register */
#ఒకవేళ defined(CONFIG_ARCH_CORTEXM7)
	uint32_t              abfsr; /* 0x0fa8 Auxiliary Bus Fault Status Register */
#అయితే_ముగింపు

} fault_regs_s;

రకనిర్వచనం నిర్మాణం {
	fault_flags_t         flags;                  /* What is in the dump */
	uintptr_t             current_regs;           /* Used to validate the dump */
	పూర్ణాంకం                   lineno;                 /* __LINE__ to up_assert */
	పూర్ణాంకం                   pid;                    /* Process ID */
	uint32_t              regs[XCPTCONTEXT_REGS]; /* Interrupt register save area */
	fault_regs_s          fault_regs;             /* NVIC status */
	px4_దొంతర_t           stacks;                 /* Stack info */
#ఒకవేళ CONFIG_TASK_NAME_కొలత > 0
	అక్షరం                  name[CONFIG_TASK_NAME_కొలత + 1]; /* Task name (with NULL
													* terminator) */
#అయితే_ముగింపు
	అక్షరం                  filename[MAX_FILE_మార్గం_నిడివి]; /* the Last of chars in
												  * __FILE__ to up_assert */
} info_s;

రకనిర్వచనం నిర్మాణం {
	info_s    info;                       /* The info */
#ఒకవేళ CONFIG_ARCH_INTERRUPTSTACK > 3      /* The amount of stack data is compile time
									 * sized backed on what is left after the
									 * other BBSRAM files are defined
									 * The order is such that only the
									 * ustack should be truncated
									 */
	దొంతర_మాట_t istack[CONFIG_USTACK_కొలత];
#అయితే_ముగింపు
	దొంతర_మాట_t ustack[CONFIG_ISTACK_కొలత];
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
పూర్ణాంకం hardfault_check_స్థితి(అక్షరం *caller) weak_function;

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
పూర్ణాంకం hardfault_write(అక్షరం *caller, పూర్ణాంకం fd, పూర్ణాంకం format, bool rearm) weak_function;

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
పూర్ణాంకం hardfault_rearm(అక్షరం *caller) weak_function;

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
పూర్ణాంకం hardfault_increment_reboot(అక్షరం *caller, bool reset) weak_function;

__END_DECLS
