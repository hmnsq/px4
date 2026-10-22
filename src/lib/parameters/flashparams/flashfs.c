/****************************************************************************
 *
 *   Copyright (c) 2015 PX4 Development Team. All rights reserved.
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

/**
 * @file flashfs.c
 *
 * Global flash based parameter store.
 *
 * This provides the mechanisms to interface to the PX4
 * parameter system but replace the IO with non file based flash
 * i/o routines. So that the code my be implemented on a SMALL memory
 * foot print device.
 */

#ضمن <px4_platform_common/defines.h>
#ضمن <px4_platform_common/posix.h>
#ضمن <nuttx/crc32.h>
#ضمن <stddef.h>
#ضمن <string.h>
#ضمن <stdbool.h>
#ضمن <stdint.h>
#ضمن <stdlib.h>
#ضمن <errno.h>
#ضمن "flashfs.h"
#ضمن <nuttx/compiler.h>
#ضمن <nuttx/progmem.h>
#ضمن <board_config.h>

#إذا defined(CONFIG_ARCH_HAVE_PROGMEM) || defined(BOARD_USE_EXTERNAL_FLASH)

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Private Types
 ****************************************************************************/

تعريفنوع uint32_t h_magic_t;
تعريفنوع uint16_t h_اندازه_t;
تعريفنوع uint16_t h_flag_t;
تعريفنوع uint16_t داده_اندازه_t;

تعريفنوع تعداد  flash_config_t {
	بزرگترین_بلوک = 2 * 1024, // This represents the size need for backing store
	MagicSig     = 0xaa553cc3,
	BlankSig     = 0xffffffff
} flash_config_t;

تعريفنوع تعداد  flash_flags_t {
	اندازه_ماسک      = 0x0003,
	ماسک_مدخل     = ~اندازه_ماسک,
	Blank_مدخل    = (h_flag_t)BlankSig,
	Valid_مدخل    = (0xa5ac & ~اندازه_ماسک),
	Erased_مدخل   = 0x0000,
} flash_flags_t;


/* The struct flash_entry_header_t will be sizeof(uint32_t) aligned
 * The Size will be the actual length of the header plus the data
 * and any padding needed to have the size be an even multiple of
 * sizeof(uint32_t)
 */
#رهنمود GCC diagnostic افزودن
#رهنمود GCC diagnostic ignored "-Wattributes"
تعريفنوع begin_packed_struct بنية flash_مدخل_سرآیند_t {
	h_magic_t            magic;           /* Used to ID files*/
	h_flag_t             flag;            /* Used to erase this entry */
	uint32_t             بررسی_چرخهای;             /* Calculated over the size - end of data */
	h_اندازه_t             اندازه;            /* When added to a byte pointer to flash_entry_header_t
                                               * Will result the offset of the next active file or
                                               * free space. */
	flash_file_token_t   file_token;      /* file token type - essentially the name/type */
} انتها_packed_struct flash_مدخل_سرآیند_t __attribute__((aligned(حجم(uint32_t))));
#رهنمود GCC diagnostic برداشت

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/
ساكن uint8_t *working_میانگیر;
ساكن size_t working_میانگیر_اندازه;
ساكن bool working_میانگیر_static;
ساكن sector_descriptor_t *sector_map;
ساكن صحيح آخرین_erased;

/****************************************************************************
 * Public Data
 ****************************************************************************/

ثابت flash_file_token_t parameters_token = {
	.n = {'p', 'l', 'o', 'g'},
};

ثابت flash_file_token_t parameters_legacy_token = {
	.n = {'p', 'a', 'r', 'm'},
};

/****************************************************************************
 * Private Functions
 ****************************************************************************/

/****************************************************************************
 * Name: parameter_flashfs_free
 *
 * Description:
 *   Frees  dynamically allocated memory
 *
 *
 ****************************************************************************/

فراغ parameter_flashfs_free(فراغ)
{
	إذا (!working_میانگیر_static && working_میانگیر != عدم) {
		حرر(working_میانگیر);
		working_میانگیر = عدم;
		working_میانگیر_اندازه = 0;
	}
}

/****************************************************************************
 * Name: blank_flash
 *
 * Description:
 *   This helper function returns true if the pointer points to a blank_flash
 *   location
 *
 * Input Parameters:
 *   pf   - A pointer to memory aligned on sizeof(uint32_t) boundaries
 *
 * Returned value:
 *   true if blank
 *
 *
 ****************************************************************************/

ساكن مضمن صحيح blank_flash(uint32_t *pf)
{
	أرجع *pf == BlankSig;
}

/****************************************************************************
 * Name: blank_check
 *
 * Description:
 *   Given a pointer to a flash entry header and a new size
 *
 * Input Parameters:
 *   pf       - A pointer to the current flash entry header
 *   new_size - The total number of bytes to be written
 *
 * Returned value:
 *  true if space is blank, If it is not blank it returns false
 *
 ****************************************************************************/

ساكن bool blank_check(flash_مدخل_سرآیند_t *pf,
			size_t new_اندازه)
{
	bool rv = true;
	uint32_t *pm = (uint32_t *) pf;
	new_اندازه /= حجم(uint32_t);

	طالما (new_اندازه-- && rv) {
		إذا (!blank_flash(pm++)) {
			rv = false;
		}
	}

	أرجع rv;
}

/****************************************************************************
 * Name: valid_magic
 *
 * Description:
 *   This helper function returns true if the pointer points to a valid
 *   magic signature
 *
 * Input Parameters:
 *   pm   - A pointer to memory aligned on sizeof(h_magic_t) boundaries
 *
 * Returned value:
 *   true if magic is valid
 *
 *
 ****************************************************************************/

ساكن مضمن صحيح valid_magic(h_magic_t *pm)
{
	أرجع *pm == MagicSig;
}

/****************************************************************************
 * Name: blank_entry
 *
 * Description:
 *   This helper function returns true if the entry is Blank
 *
 * Input Parameters:
 *   fi            - A pointer to the current flash entry header
 *
 * Returned value:
 *   true if Blank
 *
 *
 ****************************************************************************/

ساكن مضمن صحيح blank_مدخل(flash_مدخل_سرآیند_t *fi)
{
	أرجع fi->magic == BlankSig  &&  fi->flag == Blank_مدخل;
}

/****************************************************************************
 * Name: valid_entry
 *
 * Description:
 *   This helper function returns true if the entry is Blank
 *
 * Input Parameters:
 *   fi            - A pointer to the current flash entry header
 *
 * Returned value:
 *   true if valid_entry
 *
 *
 ****************************************************************************/

ساكن مضمن صحيح valid_مدخل(flash_مدخل_سرآیند_t *fi)
{
	أرجع (fi->flag & ماسک_مدخل) == Valid_مدخل;
}

/****************************************************************************
 * Name: entry_size_adjust
 *
 * Description:
 *   This helper function returns the size adjust
 *
 * Input Parameters:
 *   fi            - A pointer to the current flash entry header
 *
 * Returned value:
 *   true if valid_entry
 *
 *
 ****************************************************************************/

ساكن مضمن صحيح مدخل_اندازه_adjust(flash_مدخل_سرآیند_t *fi)
{
	أرجع fi->flag & اندازه_ماسک;
}

/****************************************************************************
 * Name: next_entry
 *
 * Description:
 *   This helper function advances the flash entry header pointer to the
 *   locations of the next entry.
 *
 * Input Parameters:
 *   fh            - A pointer to the current file header
 *
 * Returned value:
 *                - A pointer to the next file header location
 *
 *
 ****************************************************************************/

ساكن مضمن flash_مدخل_سرآیند_t *بعدی_مدخل(flash_مدخل_سرآیند_t *fi)
{
	uint8_t *pb = (uint8_t *)fi;
	أرجع (flash_مدخل_سرآیند_t *)(pb + fi->اندازه);
}

/****************************************************************************
 * Name: entry_data
 *
 * Description:
 *   This helper function returns a pointer the the data in the entry
 *
 * Input Parameters:
 *   fh            - A pointer to the current file header
 *
 * Returned value:
 *                - A pointer to the next file header location
 *
 *
 ****************************************************************************/

ساكن مضمن uint8_t *مدخل_داده(flash_مدخل_سرآیند_t *fi)
{
	أرجع ((uint8_t *)fi) + حجم(flash_مدخل_سرآیند_t);
}

/****************************************************************************
 * Name: entry_data_length
 *
 * Description:
 *   This helper function returns the size of the user data
 *
 * Input Parameters:
 *   fh            - A pointer to the current file header
 *
 * Returned value:
 *                - The length of the data in the entry
 *
 *
 ****************************************************************************/

ساكن مضمن داده_اندازه_t مدخل_داده_درازی(flash_مدخل_سرآیند_t *fi)
{
	أرجع fi->اندازه - (حجم(flash_مدخل_سرآیند_t) + مدخل_اندازه_adjust(fi));
}

/****************************************************************************
 * Name: entry_crc_start
 *
 * Description:
 *   This helper function returns a const byte pointer to the location
 *   where the CRC is calculated over
 *
 * Input Parameters:
 *   fi  - A pointer to the current file header
 *
 * Returned value:
 * A pointer to the point at which the crc is calculated from.
 *
 *
 ****************************************************************************/

ساكن مضمن ثابت uint8_t *مدخل_بررسی_چرخهای_شروع(flash_مدخل_سرآیند_t *fi)
{
	أرجع (ثابت uint8_t *)&fi->اندازه;
}

/****************************************************************************
 * Name: entry_crc_length
 *
 * Description:
 *   This helper function returns the length of the regone where the CRC is
 *   calculated over
 *
 * Input Parameters:
 *   fi  - A pointer to the current file header
 *
 * Returned value:
 * Number of bytes to to crc
 *
 *
 ****************************************************************************/

ساكن مضمن داده_اندازه_t مدخل_بررسی_چرخهای_درازی(flash_مدخل_سرآیند_t *fi)
{
	أرجع fi->اندازه - offsetof(flash_مدخل_سرآیند_t, اندازه);
}

/* The parameter sector can be the last one on the chip, so no header
 * field may be read until the whole header is known to lie inside it. */
ساكن مضمن bool سرآیند_میگنجد(flash_مدخل_سرآیند_t *pf, h_magic_t *pe)
{
	أرجع pf + 1 <= (flash_مدخل_سرآیند_t *)pe;
}

ساكن bool بررسی_چرخهای_سرآیند_ok(flash_مدخل_سرآیند_t *pf, h_magic_t *pe)
{
	إذا (!سرآیند_میگنجد(pf, pe)) {
		أرجع false;
	}

	إذا (pf->اندازه < حجم(flash_مدخل_سرآیند_t)) {
		أرجع false;
	}

	ثابت uint8_t *بررسی_چرخهای_شروع = مدخل_بررسی_چرخهای_شروع(pf);
	داده_اندازه_t بررسی_چرخهای_درازی = مدخل_بررسی_چرخهای_درازی(pf);

	إذا (بررسی_چرخهای_شروع + بررسی_چرخهای_درازی > (uint8_t *)pe) {
		أرجع false;
	}

	أرجع pf->بررسی_چرخهای == بررسی_چرخهای_32(بررسی_چرخهای_شروع, بررسی_چرخهای_درازی);
}

/* The walkers stop at the last word of a sector and reject a record that
 * ends beyond it, so nothing may be written into that word. */
ساكن مضمن size_t sector_usable(صحيح s)
{
	أرجع sector_map[s].اندازه - حجم(h_magic_t);
}

ساكن bool جایگاه_is_free(flash_مدخل_سرآیند_t *pf, صحيح s, size_t required)
{
	uint8_t *begin = (uint8_t *)sector_map[s].آدرس;
	uint8_t *انتها = begin + sector_usable(s);

	إذا ((uint8_t *)pf < begin || required == 0 || (uint8_t *)pf + required > انتها) {
		أرجع false;
	}

	/* Entries start on a flash row. up_progmem_write only asserts that in
	 * debug builds, so a slot found by the word-wise scan behind a torn
	 * record would otherwise be programmed misaligned. */
	إذا (((uintptr_t)pf & اندازه_ماسک) != 0) {
		أرجع false;
	}

	أرجع blank_مدخل(pf) && blank_check(pf, required);
}

ساكن صحيح for_each_valid_مدخل(flash_file_token_t token,
				صحيح (*cb)(flash_مدخل_سرآیند_t *pf, فراغ *arg),
				فراغ *arg);

ساكن صحيح stop_at_اولی(flash_مدخل_سرآیند_t *pf, فراغ *arg)
{
	*(flash_مدخل_سرآیند_t **)arg = pf;
	أرجع -ECANCELED;
}

/* Oldest valid entry for token, or NULL. */
ساكن flash_مدخل_سرآیند_t *پیدا_کن_مدخل(flash_file_token_t token)
{
	flash_مدخل_سرآیند_t *pf = عدم;
	for_each_valid_مدخل(token, stop_at_اولی, &pf);
	أرجع pf;
}

/****************************************************************************
 * Name: find_append
 *
 * Description:
 *   Return the address of a blank run that sits after every CRC-valid
 *   header in sector-map order. First-fit holes before later live data are
 *   not used; those are fragmentation and belong to compact.
 *
 ****************************************************************************/

ساكن flash_مدخل_سرآیند_t *پیدا_کن_append(size_t required)
{
	إذا (sector_map == عدم || required == 0) {
		أرجع عدم;
	}

	flash_مدخل_سرآیند_t *آخرین = عدم;
	صحيح آخرین_s = -1;

	لكل (صحيح s = 0; sector_map[s].آدرس; s++) {
		h_magic_t *pmagic = (h_magic_t *) sector_map[s].آدرس;
		h_magic_t *pe = pmagic + (sector_map[s].اندازه / حجم(h_magic_t)) - 1;

		طالما (pmagic < pe) {
			إذا (!valid_magic(pmagic)) {
				pmagic++;
				استمر;
			}

			flash_مدخل_سرآیند_t *pf = (flash_مدخل_سرآیند_t *) pmagic;

			إذا (بررسی_چرخهای_سرآیند_ok(pf, pe)) {
				آخرین = pf;
				آخرین_s = s;
				pmagic = (h_magic_t *) بعدی_مدخل(pf);
				pf = (flash_مدخل_سرآیند_t *) pmagic;

				/* Same stop rule as for_each_valid_entry: a record behind a
				 * blank header is invisible to replay, so it must not anchor
				 * the append point either. */
				إذا (pmagic >= pe || !سرآیند_میگنجد(pf, pe) || blank_مدخل(pf)) {
					اكسر;
				}

			} وإلا {
				pmagic++;
			}
		}
	}

	ثابت صحيح شروع_s = (آخرین == عدم) ? 0 : آخرین_s;
	h_magic_t *pmagic = (آخرین == عدم)
			    ? (h_magic_t *) sector_map[0].آدرس
			    : (h_magic_t *) بعدی_مدخل(آخرین);

	لكل (صحيح s = شروع_s; sector_map[s].آدرس; s++) {
		إذا (s != شروع_s) {
			pmagic = (h_magic_t *) sector_map[s].آدرس;
		}

		h_magic_t *pe = (h_magic_t *) sector_map[s].آدرس
				+ (sector_map[s].اندازه / حجم(h_magic_t)) - 1;

		إذا ((uint8_t *)pmagic < (uint8_t *)sector_map[s].آدرس
		    || (uint8_t *)pmagic >= (uint8_t *)sector_map[s].آدرس + sector_map[s].اندازه) {
			إذا (s == شروع_s) {
				استمر;
			}

			pmagic = (h_magic_t *) sector_map[s].آدرس;
		}

		طالما (pmagic < pe) {
			flash_مدخل_سرآیند_t *pf = (flash_مدخل_سرآیند_t *) pmagic;

			إذا (valid_magic(pmagic) && بررسی_چرخهای_سرآیند_ok(pf, pe)) {
				pmagic = (h_magic_t *) بعدی_مدخل(pf);
				استمر;
			}

			إذا (جایگاه_is_free(pf, s, required)) {
				أرجع pf;
			}

			pmagic++;
		}
	}

	أرجع عدم;
}

/****************************************************************************
 * Name: for_each_valid_entry
 *
 * Description:
 *   Visit every CRC-valid, not-erased entry matching token, oldest first.
 *   cb returns 0 to continue, <0 to abort with that errno.
 *   A torn or oversize header advances one magic word; it must not skip
 *   the rest of the sector (later valid records would be invisible).
 *
 ****************************************************************************/

ساكن صحيح for_each_valid_مدخل(flash_file_token_t token,
				صحيح (*cb)(flash_مدخل_سرآیند_t *pf, فراغ *arg),
				فراغ *arg)
{
	صحيح n = 0;

	إذا (sector_map == عدم) {
		أرجع -ENXIO;
	}

	لكل (صحيح s = 0; sector_map[s].آدرس; s++) {

		h_magic_t *pmagic = (h_magic_t *) sector_map[s].آدرس;
		h_magic_t *pe = pmagic + (sector_map[s].اندازه / حجم(h_magic_t)) - 1;

cont:

		طالما (pmagic < pe && !valid_magic(pmagic)) {
			pmagic++;
		}

		إذا (pmagic >= pe) { استمر; }

		flash_مدخل_سرآیند_t *pf = (flash_مدخل_سرآیند_t *) pmagic;

		إذا (بررسی_چرخهای_سرآیند_ok(pf, pe)) {

			إذا (valid_مدخل(pf) && pf->file_token.t == token.t) {
				إذا (cb != عدم) {
					صحيح rv = cb(pf, arg);

					إذا (rv < 0) {
						أرجع rv;
					}
				}

				n++;
			}

			pf = بعدی_مدخل(pf);
			pmagic = (h_magic_t *) pf;

			إذا (pmagic >= pe || !سرآیند_میگنجد(pf, pe) || blank_مدخل(pf)) {
				استمر;
			}

		} وإلا {
			pmagic++;
		}

		اذهب cont;
	}

	أرجع n;
}

/****************************************************************************
 * Name: erase_sector
 *
 * Description:
 *   Given a pointer to sector_descriptor_t, this function
 *   erases the sector and updates the last_erased using
 *   the pointer to the flash_entry_header_t as a sanity check
 *
 * Input Parameters:
 *   sm      - A pointer to the current sector_descriptor_t
 *   pf      - A pointer to the current flash entry header
 *
 * Returned value:
 *  O On Success or a negative errno
 *
 *
 ****************************************************************************/

ساكن صحيح erase_sector(sector_descriptor_t *sm, flash_مدخل_سرآیند_t *pf)
{
	صحيح rv = 0;
#إذا defined(BOARD_USE_EXTERNAL_FLASH)
	ssize_t بلوک_2 = به_بالا_progmem_ext_getpage((size_t)pf);
#وإلا
	ssize_t بلوک_2 = به_بالا_progmem_getpage((size_t)pf);
#پایان_اگر

	إذا (بلوک_2 > 0 && بلوک_2 == sm->صفحه) {
		آخرین_erased = sm->صفحه;
#إذا defined(BOARD_USE_EXTERNAL_FLASH)
		ssize_t اندازه = به_بالا_progmem_ext_eraseblock(بلوک_2);
#وإلا
		ssize_t اندازه = به_بالا_progmem_eraseblock(بلوک_2);
#پایان_اگر

		إذا (اندازه < 0 || اندازه != (ssize_t)sm->اندازه) {
			rv = اندازه;
		}
	}

	أرجع rv;
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/
/****************************************************************************
 * Name: parameter_flashfs_read
 *
 * Description:
 *   This function returns a pointer to the locations of the data associated
 *   with the file token. On successful return *buffer will be set to Flash
 *   location and *buf_size the length of the user data.
 *
 * Input Parameters:
 *   token       - File Token File to read
 *   buffer      - A pointer to a pointer that will receive the address
 *                 in flash of the data of this "files" data
 *   buf_size    - A pointer to receive the number of bytes in the "file"
 *
 * Returned value:
 *   On success number of bytes read or a negative errno value,
 *
 *
 ****************************************************************************/

صحيح parameter_flashfs_read(flash_file_token_t token, uint8_t **میانگیر_2, size_t
			   *buf_اندازه)
{
	صحيح rv = -ENXIO;

	إذا (sector_map) {

		rv = -ENOENT;
		flash_مدخل_سرآیند_t *pf = پیدا_کن_مدخل(token);

		إذا (pf) {
			(*میانگیر_2) = مدخل_داده(pf);
			rv = مدخل_داده_درازی(pf);
			*buf_اندازه = rv;
		}
	}

	أرجع rv;
}

/****************************************************************************
 * Name: parameter_flashfs_blank
 *
 * Description:
 *   Reports whether the parameter storage is fully erased, so callers can
 *   tell a blank store from one that holds unparseable data.
 *
 ****************************************************************************/

صحيح parameter_flashfs_blank(فراغ)
{
	إذا (sector_map == عدم) {
		أرجع -ENXIO;
	}

	لكل (صحيح s = 0; sector_map[s].آدرس; s++) {
		uint32_t *pm = (uint32_t *) sector_map[s].آدرس;
		uint32_t *pe = pm + (sector_map[s].اندازه / حجم(uint32_t));

		طالما (pm < pe) {
			إذا (!blank_flash(pm++)) {
				أرجع 0;
			}
		}
	}

	أرجع 1;
}

/****************************************************************************
 * Name: parameter_flashfs_write
 *
 * Description:
 *   Append a new entry. Previous valid entries are left intact.
 *
 * Input Parameters:
 *   token      - File Token File to read
 *   buffer      - A pointer to a buffer with buf_size bytes to be written
 *                 to the flash. This buffer must be allocated
 *                 with a previous call to parameter_flashfs_alloc
 *   buf_size    - Number of bytes to write
 *
 * Returned value:
 *   On success the number of bytes written On Error a negative value of errno
 *
 ****************************************************************************/

صحيح
parameter_flashfs_write(flash_file_token_t token, uint8_t *میانگیر_2, size_t buf_اندازه)
{
	صحيح rv = -ENXIO;

	إذا (sector_map) {

		rv = 0;

		/* Calculate the total space needed */

		size_t مجموع_اندازه = buf_اندازه + حجم(flash_مدخل_سرآیند_t);
		size_t alignment = حجم(h_magic_t) - 1;
		size_t  اندازه_adjust = ((مجموع_اندازه + alignment) & ~alignment) - مجموع_اندازه;
		مجموع_اندازه += اندازه_adjust;

		إذا (مجموع_اندازه > UINT16_MAX) {
			أرجع -EFBIG;
		}

		/* Append after the last CRC-valid record. Compaction is the caller's job. */
		flash_مدخل_سرآیند_t *pf = پیدا_کن_append(مجموع_اندازه);

		إذا (pf == 0) {
			أرجع -ENOSPC;
		}

		flash_مدخل_سرآیند_t *pn = (flash_مدخل_سرآیند_t *)(میانگیر_2 - حجم(flash_مدخل_سرآیند_t));
		pn->magic = MagicSig;
		pn->file_token.t = token.t;
		pn->flag = Valid_مدخل + اندازه_adjust;
		pn->اندازه = مجموع_اندازه;

		لكل (size_t a = 0; a < اندازه_adjust; a++) {
			میانگیر_2[buf_اندازه + a] = (uint8_t)BlankSig;
		}

		pn->بررسی_چرخهای = بررسی_چرخهای_32(مدخل_بررسی_چرخهای_شروع(pn), مدخل_بررسی_چرخهای_درازی(pn));
#إذا defined(BOARD_USE_EXTERNAL_FLASH)
		rv = به_بالا_progmem_ext_write((size_t) pf, pn, pn->اندازه);
#وإلا
		rv = به_بالا_progmem_write((size_t) pf, pn, pn->اندازه);
#پایان_اگر
		صحيح system_بایتها = (حجم(flash_مدخل_سرآیند_t) + اندازه_adjust);

		إذا (rv >= system_بایتها) {
			rv -= system_بایتها;
		}
	}

	أرجع rv;
}

/****************************************************************************
 * Name: parameter_flashfs_walk
 ****************************************************************************/

بنية walk_pack {
	parameter_flashfs_walk_cb cb;
	فراغ *arg;
};

ساكن صحيح walk_one(flash_مدخل_سرآیند_t *pf, فراغ *arg)
{
	بنية walk_pack *w = (بنية walk_pack *)arg;
	أرجع w->cb(مدخل_داده(pf), مدخل_داده_درازی(pf), w->arg);
}

صحيح parameter_flashfs_walk(flash_file_token_t token, parameter_flashfs_walk_cb cb, فراغ *arg)
{
	بنية walk_pack w = { cb, arg };
	أرجع for_each_valid_مدخل(token, walk_one, &w);
}

/****************************************************************************
 * Name: parameter_flashfs_needs_compact
 ****************************************************************************/

size_t parameter_flashfs_max_payload(فراغ)
{
	إذا (sector_map == عدم) {
		أرجع 0;
	}

	size_t max_مجموع = 0;

	لكل (صحيح s = 0; sector_map[s].آدرس; s++) {
		إذا (sector_usable(s) > max_مجموع) {
			max_مجموع = sector_usable(s);
		}
	}

	/* h_size_t holds the record size */
	إذا (max_مجموع > UINT16_MAX) {
		max_مجموع = UINT16_MAX;
	}

	max_مجموع &= ~(size_t)اندازه_ماسک;

	إذا (max_مجموع <= حجم(flash_مدخل_سرآیند_t)) {
		أرجع 0;
	}

	أرجع max_مجموع - حجم(flash_مدخل_سرآیند_t);
}

صحيح parameter_flashfs_needs_compact(flash_file_token_t token, size_t snapshot_اندازه)
{
	إذا (sector_map == عدم) {
		أرجع -ENXIO;
	}

	flash_مدخل_سرآیند_t *اولی = پیدا_کن_مدخل(token);

	إذا (اولی == عدم) {
		أرجع 0;
	}

	/* Dead records ahead of the live data (a torn write, or the erased
	 * prefix the single-snapshot writer left) only go away with an erase. */
	إذا ((uintptr_t)اولی != (uintptr_t)sector_map[0].آدرس) {
		أرجع 1;
	}

	/* Keep room after the log for a burst the size of the snapshot
	 * (reset-all tombstones, airframe PARAM_SET), floored so a UUID append
	 * does not force a boot erase. */
	ثابت size_t align = حجم(h_magic_t) - 1;
	ثابت size_t snapshot_مجموع = (snapshot_اندازه + حجم(flash_مدخل_سرآیند_t) + align) & ~align;
	ثابت size_t headroom = snapshot_مجموع > 8192 ? snapshot_مجموع : 8192;

	إذا (پیدا_کن_append(headroom) != عدم) {
		أرجع 0;
	}

	/* Compaction leaves the snapshot at the start of sector 0 and every
	 * other sector blank. Erase only if that layout has the room this one
	 * lacks, or every boot would pay the erase for nothing. */
	إذا (sector_usable(0) >= snapshot_مجموع + headroom) {
		أرجع 1;
	}

	لكل (صحيح s = 1; sector_map[s].آدرس; s++) {
		إذا (sector_usable(s) >= headroom) {
			أرجع 1;
		}
	}

	أرجع 0;
}

/****************************************************************************
 * Name: parameter_flashfs_alloc
 *
 * Description:
 *   This function is called to get a buffer to use in a subsequent call
 *   to parameter_flashfs_write. The address returned is advanced into the
 *   buffer to reserve space for the flash entry header.
 *   The caller is responsible to call parameter_flashfs_free after usage.
 *
 * Input Parameters:
 *   token      - File Token File to read (not used)
 *   buffer     - A pointer to return a pointer to Memory of buf_size length
 *   			  suitable for calling parameter_flashfs_write
 *   buf_size   - In: the size needed for the write operation.
 *   			  Out: The maximum number of bytes that can be written to
 *                the buffer
 *
 * Returned value:
 *   On success the number of bytes written On Error a negative value of errno
 *
 ****************************************************************************/

صحيح parameter_flashfs_alloc(flash_file_token_t token, uint8_t **میانگیر_2, size_t *buf_اندازه)
{
	صحيح rv = -ENXIO;

	إذا (sector_map) {

		rv = -ENOMEM;

		إذا (!working_میانگیر_static) {

			/* parameter_flashfs_write pads the entry to a word in place */
			working_میانگیر_اندازه = *buf_اندازه + حجم(flash_مدخل_سرآیند_t) + اندازه_ماسک;
			working_میانگیر = حافظه_بده(working_میانگیر_اندازه);

		}

		/* Allocation failed or not provided */

		إذا (working_میانگیر == عدم) {

			working_میانگیر_اندازه = 0;

		} وإلا {

			/* We have a buffer reserve space and init it */
			*میانگیر_2 = &working_میانگیر[حجم(flash_مدخل_سرآیند_t)];
			*buf_اندازه = working_میانگیر_اندازه - حجم(flash_مدخل_سرآیند_t) - اندازه_ماسک;
			memset(working_میانگیر, 0xff, working_میانگیر_اندازه);
			rv = 0;

		}
	}

	أرجع rv;
}

/****************************************************************************
 * Name: parameter_flashfs_erase
 *
 * Description:
 *   This function erases the sectors that were passed to parameter_flashfs_init
 *
 * Input Parameters:
 *
 * Returned value:
 *   On success the number of bytes erased
 *   On Error a negative value of errno
 *
 ****************************************************************************/

صحيح parameter_flashfs_erase(فراغ)
{
	صحيح rv = -ENXIO;

	إذا (sector_map) {
		rv = 0;

		لكل (صحيح s = 0; sector_map[s].آدرس; s++) {
			صحيح sz = erase_sector(&sector_map[s], (flash_مدخل_سرآیند_t *)sector_map[s].آدرس);

			إذا (sz != 0) {
				أرجع sz;
			}

			rv += sector_map[s].اندازه;
		}
	}

	أرجع rv;
}

/****************************************************************************
 * Name: parameter_flashfs_init
 *
 * Description:
 *   This helper function advances the flash entry header pointer to the
 *   locations of the next entry.
 *
 * Input Parameters:
 *   fconfig      - A pointer to an null entry terminated array of
 *                  flash_file_sector_t
 *    buffer      - A pointer to a memory to make available to callers
 *                  for write operations. When allocated to the caller
 *                  space is reserved in the front for the
 *                  flash_entry_header_t.
 *                  If this is passes as NULL. The buffer will be
 *                  allocated from the heap on calls to
 *                  parameter_flashfs_alloc and fread on calls
 *                  to parameter_flashfs_free
 *
 *   size         - The size of the buffer in bytes. Should be be 0 if buffer
 *                  is NULL
 *
 * Returned value:
 *                - A pointer to the next file header location
 *
 *
 ****************************************************************************/

صحيح parameter_flashfs_init(sector_descriptor_t *fconfig, uint8_t *میانگیر_2, uint16_t اندازه)
{
	صحيح rv = 0;
	sector_map = fconfig;
	working_میانگیر_static = میانگیر_2 != عدم;

	إذا (!working_میانگیر_static) {
		اندازه = 0;
	}

	working_میانگیر = میانگیر_2;
	working_میانگیر_اندازه = اندازه;
	آخرین_erased = -1;

	/* Sanity check */

	/* A non-blank store with no valid entry is torn or foreign; import
	 * reports -EILSEQ. Erasing here would hide a successful retry after
	 * a torn write. */

	أرجع rv;
}

#إذا defined(FLASH_UNIT_TEST)

ساكن sector_descriptor_t  test_sector_map[] = {
	{1, 16 * 1024, 0x08004000},
	{2, 16 * 1024, 0x08008000},
	{0, 0, 0},
};

__EXPORT فراغ test(فراغ);

uint8_t test_buf[32 * 1024];

__EXPORT فراغ test(فراغ)
{
	uint16_t بزرگترین_بلوک_2 = (32 * 1024) + 32;
	uint8_t *میانگیر_2 = حافظه_بده(بزرگترین_بلوک_2);

	parameter_flashfs_init(test_sector_map, میانگیر_2, بزرگترین_بلوک_2);

	لكل (صحيح t = 0; t < حجم(test_buf); t++) {
		test_buf[t] = (uint8_t) t;
	}

	صحيح er = parameter_flashfs_erase();
	uint8_t *fbuffer;
	size_t buf_اندازه;
	صحيح written = 0;
	صحيح اقرأ = 0;
	صحيح rv = 0;

	لكل (صحيح a = 0; a <= 4; a++) {
		parameter_flashfs_erase();
		rv =  parameter_flashfs_alloc(parameters_token, &fbuffer, &buf_اندازه);
		کپی_حافظه(fbuffer, test_buf, a);
		buf_اندازه = a;
		written = parameter_flashfs_write(parameters_token, fbuffer, buf_اندازه);
		اقرأ = parameter_flashfs_read(parameters_token, &fbuffer, &buf_اندازه);
		parameter_flashfs_free();

		إذا (اقرأ != written) {
			ساكن متطاير صحيح j;
			j++;
		}
	}

	صحيح بلوک_2 = 2048;

	لكل (صحيح a = 0; a <= 8; a++) {
		parameter_flashfs_erase();
		rv =  parameter_flashfs_alloc(parameters_token, &fbuffer, &buf_اندازه);
		کپی_حافظه(fbuffer, test_buf, بلوک_2);
		buf_اندازه = بلوک_2;
		written = parameter_flashfs_write(parameters_token, fbuffer, buf_اندازه);
		اقرأ = parameter_flashfs_read(parameters_token, &fbuffer, &buf_اندازه);
		parameter_flashfs_free();

		إذا (اقرأ != written) {
			ساكن متطاير صحيح j;
			j++;
		}

		بلوک_2 += 2048;
	}

	rv++;
	er++;
	حرر(میانگیر_2);
}
#پایان_اگر /* FLASH_UNIT_TEST */
#پایان_اگر /* CONFIG_ARCH_HAVE_PROGMEM */
