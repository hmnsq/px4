/****************************************************************************
 *
 *   Copyright (c) 2022 PX4 Development Team. All rights reserved.
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
 * @file flashfs32.c
 *
 * Global flash based parameter store.
 *
 * This provides the mechanisms to interface to the PX4
 * parameter system but replace the IO with non file based flash
 * i/o routines. So that the code my be implemented on a SMALL memory
 * foot print device.
 *
 * It was made to provide flashfs functionality on the STM32H7 chip,
 * which uses 32-byte flash lines that cannot be overwritten because of
 * on-board CRC functionality.
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

#إذا defined(CONFIG_ARCH_HAVE_PROGMEM)

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Private Types
 ****************************************************************************/

تعريفنوع uint32_t h_magic_t;
تعريفنوع uint16_t h_ناپ_t;
تعريفنوع uint16_t h_flag_t;
تعريفنوع uint16_t انکڑے_ناپ_t;

تعريفنوع تعداد  flash_config_t {
	سبھ_توں_وڈا_بلاک = 2 * 1024, // This represents the size need for backing store
	MagicSig     = 0xaa553cc3,
	BlankSig     = 0xffffffff
} flash_config_t;

/* Aligns are done up to 32 bytes, so storing the size padding within 0x001f mask */
تعريفنوع تعداد  flash_flags_t {
	ناپ_نقاب      = 0x001f,
	نقاب_اندراج     = ~ناپ_نقاب,
	Blank_اندراج    = (h_flag_t)BlankSig,
	Valid_اندراج    = (0xa5ac & ~ناپ_نقاب),
	Erased_اندراج   = 0x0000,
} flash_flags_t;

/* The struct flash_entry_header_t will be 32-byte aligned
 * The Size will be the actual length of the header plus the data
 * and any padding needed to have the size be an even multiple of
 * the 32-byte flash line
 */
#ہدایت GCC diagnostic ڈالو
#ہدایت GCC diagnostic ignored "-Wattributes"
تعريفنوع begin_packed_struct بنية flash_اندراج_سرخی_t {
	h_magic_t            magic;           /* Used to ID files*/
	h_flag_t             flag;            /* Used to mark this entry as valid */
	uint8_t              بن_ورتیا_بفر[32 - حجم(h_magic_t) - حجم(h_flag_t)]; /* padding */
	h_flag_t             flag_erased;     /* Used to erase this entry */
	uint8_t              بن_ورتیا_buffer2[32 - حجم(h_flag_t)]; /* padding */
	uint32_t             چکری_جانچ;             /* Calculated over the size - end of data */
	h_ناپ_t             ناپ;            /* When added to a byte pointer to flash_entry_header_t
                                               * Will result the offset of the next active file or
                                               * free space. */
	flash_file_token_t   file_token;      /* file token type - essentially the name/type */
} سرا_packed_struct flash_اندراج_سرخی_t __attribute__((aligned(حجم(uint32_t))));
#ہدایت GCC diagnostic نکالو

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/
ساكن uint8_t *working_بفر;
ساكن size_t working_بفر_ناپ;
ساكن bool working_بفر_static;
ساكن sector_descriptor_t *sector_map;

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
	إذا (!working_بفر_static && working_بفر != عدم) {
		حرر(working_بفر);
		working_بفر = عدم;
		working_بفر_ناپ = 0;
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

ساكن bool blank_check(flash_اندراج_سرخی_t *pf,
			size_t new_ناپ)
{
	bool rv = true;
	uint32_t *pm = (uint32_t *) pf;
	new_ناپ /= حجم(uint32_t);

	طالما (new_ناپ-- && rv) {
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

ساكن مضمن صحيح blank_اندراج(flash_اندراج_سرخی_t *fi)
{
	أرجع fi->magic == BlankSig && fi->flag == Blank_اندراج && fi->flag_erased == Blank_اندراج;
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

ساكن مضمن صحيح valid_اندراج(flash_اندراج_سرخی_t *fi)
{
	أرجع (fi->flag & نقاب_اندراج) == Valid_اندراج && (fi->flag_erased) == Blank_اندراج;
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

ساكن مضمن صحيح اندراج_ناپ_adjust(flash_اندراج_سرخی_t *fi)
{
	أرجع fi->flag & ناپ_نقاب;
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

ساكن مضمن flash_اندراج_سرخی_t *اگلا_اندراج(flash_اندراج_سرخی_t *fi)
{
	uint8_t *pb = (uint8_t *)fi;
	أرجع (flash_اندراج_سرخی_t *)(pb + fi->ناپ);
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

ساكن مضمن uint8_t *اندراج_انکڑے(flash_اندراج_سرخی_t *fi)
{
	أرجع ((uint8_t *)fi) + حجم(flash_اندراج_سرخی_t);
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

ساكن مضمن انکڑے_ناپ_t اندراج_انکڑے_لمائی(flash_اندراج_سرخی_t *fi)
{
	أرجع fi->ناپ - (حجم(flash_اندراج_سرخی_t) + اندراج_ناپ_adjust(fi));
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

ساكن مضمن ثابت uint8_t *اندراج_چکری_جانچ_شروع(flash_اندراج_سرخی_t *fi)
{
	أرجع (ثابت uint8_t *)&fi->ناپ;
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

ساكن مضمن انکڑے_ناپ_t اندراج_چکری_جانچ_لمائی(flash_اندراج_سرخی_t *fi)
{
	أرجع fi->ناپ - offsetof(flash_اندراج_سرخی_t, ناپ);
}

/* The parameter sector can be the last one on the chip, so no header
 * field may be read until the whole header is known to lie inside it. */
ساكن مضمن bool سرخی_سماندا(flash_اندراج_سرخی_t *pf, h_magic_t *pe)
{
	أرجع pf + 1 <= (flash_اندراج_سرخی_t *)pe;
}

ساكن bool چکری_جانچ_سرخی_ok(flash_اندراج_سرخی_t *pf, h_magic_t *pe)
{
	إذا (!سرخی_سماندا(pf, pe)) {
		أرجع false;
	}

	إذا (pf->ناپ < حجم(flash_اندراج_سرخی_t)) {
		أرجع false;
	}

	ثابت uint8_t *چکری_جانچ_شروع = اندراج_چکری_جانچ_شروع(pf);
	انکڑے_ناپ_t چکری_جانچ_لمائی = اندراج_چکری_جانچ_لمائی(pf);

	إذا (چکری_جانچ_شروع + چکری_جانچ_لمائی > (uint8_t *)pe) {
		أرجع false;
	}

	أرجع pf->چکری_جانچ == چکری_جانچ_32(چکری_جانچ_شروع, چکری_جانچ_لمائی);
}

/* The walkers stop at the last word of a sector and reject a record that
 * ends beyond it, so nothing may be written into that word. */
ساكن مضمن size_t sector_usable(صحيح s)
{
	أرجع sector_map[s].ناپ - حجم(h_magic_t);
}

ساكن bool تھاں_is_free(flash_اندراج_سرخی_t *pf, صحيح s, size_t required)
{
	uint8_t *begin = (uint8_t *)sector_map[s].پتہ;
	uint8_t *سرا = begin + sector_usable(s);

	إذا ((uint8_t *)pf < begin || required == 0 || (uint8_t *)pf + required > سرا) {
		أرجع false;
	}

	/* Entries start on a flash row. up_progmem_write only asserts that in
	 * debug builds, so a slot found by the word-wise scan behind a torn
	 * record would otherwise be programmed misaligned. */
	إذا (((uintptr_t)pf & ناپ_نقاب) != 0) {
		أرجع false;
	}

	أرجع blank_اندراج(pf) && blank_check(pf, required);
}

ساكن صحيح for_each_valid_اندراج(flash_file_token_t token,
				صحيح (*cb)(flash_اندراج_سرخی_t *pf, فراغ *arg),
				فراغ *arg);

ساكن صحيح stop_at_پہلا(flash_اندراج_سرخی_t *pf, فراغ *arg)
{
	*(flash_اندراج_سرخی_t **)arg = pf;
	أرجع -ECANCELED;
}

/* Oldest valid entry for token, or NULL. */
ساكن flash_اندراج_سرخی_t *لبھو_اندراج(flash_file_token_t token)
{
	flash_اندراج_سرخی_t *pf = عدم;
	for_each_valid_اندراج(token, stop_at_پہلا, &pf);
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

ساكن flash_اندراج_سرخی_t *لبھو_append(size_t required)
{
	إذا (sector_map == عدم || required == 0) {
		أرجع عدم;
	}

	flash_اندراج_سرخی_t *آخری = عدم;
	صحيح آخری_s = -1;

	لكل (صحيح s = 0; sector_map[s].پتہ; s++) {
		h_magic_t *pmagic = (h_magic_t *) sector_map[s].پتہ;
		h_magic_t *pe = pmagic + (sector_map[s].ناپ / حجم(h_magic_t)) - 1;

		طالما (pmagic < pe) {
			إذا (!valid_magic(pmagic)) {
				pmagic++;
				استمر;
			}

			flash_اندراج_سرخی_t *pf = (flash_اندراج_سرخی_t *) pmagic;

			إذا (چکری_جانچ_سرخی_ok(pf, pe)) {
				آخری = pf;
				آخری_s = s;
				pmagic = (h_magic_t *) اگلا_اندراج(pf);
				pf = (flash_اندراج_سرخی_t *) pmagic;

				/* Same stop rule as for_each_valid_entry: a record behind a
				 * blank header is invisible to replay, so it must not anchor
				 * the append point either. */
				إذا (pmagic >= pe || !سرخی_سماندا(pf, pe) || blank_اندراج(pf)) {
					اكسر;
				}

			} وإلا {
				pmagic++;
			}
		}
	}

	ثابت صحيح شروع_s = (آخری == عدم) ? 0 : آخری_s;
	h_magic_t *pmagic = (آخری == عدم)
			    ? (h_magic_t *) sector_map[0].پتہ
			    : (h_magic_t *) اگلا_اندراج(آخری);

	لكل (صحيح s = شروع_s; sector_map[s].پتہ; s++) {
		إذا (s != شروع_s) {
			pmagic = (h_magic_t *) sector_map[s].پتہ;
		}

		h_magic_t *pe = (h_magic_t *) sector_map[s].پتہ
				+ (sector_map[s].ناپ / حجم(h_magic_t)) - 1;

		إذا ((uint8_t *)pmagic < (uint8_t *)sector_map[s].پتہ
		    || (uint8_t *)pmagic >= (uint8_t *)sector_map[s].پتہ + sector_map[s].ناپ) {
			إذا (s == شروع_s) {
				استمر;
			}

			pmagic = (h_magic_t *) sector_map[s].پتہ;
		}

		طالما (pmagic < pe) {
			flash_اندراج_سرخی_t *pf = (flash_اندراج_سرخی_t *) pmagic;

			إذا (valid_magic(pmagic) && چکری_جانچ_سرخی_ok(pf, pe)) {
				pmagic = (h_magic_t *) اگلا_اندراج(pf);
				استمر;
			}

			إذا (تھاں_is_free(pf, s, required)) {
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

ساكن صحيح for_each_valid_اندراج(flash_file_token_t token,
				صحيح (*cb)(flash_اندراج_سرخی_t *pf, فراغ *arg),
				فراغ *arg)
{
	صحيح n = 0;

	إذا (sector_map == عدم) {
		أرجع -ENXIO;
	}

	لكل (صحيح s = 0; sector_map[s].پتہ; s++) {

		h_magic_t *pmagic = (h_magic_t *) sector_map[s].پتہ;
		h_magic_t *pe = pmagic + (sector_map[s].ناپ / حجم(h_magic_t)) - 1;

cont:

		طالما (pmagic < pe && !valid_magic(pmagic)) {
			pmagic++;
		}

		إذا (pmagic >= pe) { استمر; }

		flash_اندراج_سرخی_t *pf = (flash_اندراج_سرخی_t *) pmagic;

		إذا (چکری_جانچ_سرخی_ok(pf, pe)) {

			إذا (valid_اندراج(pf) && pf->file_token.t == token.t) {
				إذا (cb != عدم) {
					صحيح rv = cb(pf, arg);

					إذا (rv < 0) {
						أرجع rv;
					}
				}

				n++;
			}

			pf = اگلا_اندراج(pf);
			pmagic = (h_magic_t *) pf;

			إذا (pmagic >= pe || !سرخی_سماندا(pf, pe) || blank_اندراج(pf)) {
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
 *   erases the sector using the pointer to the
 *   flash_entry_header_t as a sanity check
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

ساكن صحيح erase_sector(sector_descriptor_t *sm, flash_اندراج_سرخی_t *pf)
{
	صحيح rv = 0;

	/*
	* Because up_progmem interface doesn't allow one-to-one check of
	* the block number for an address, use uniform-memory assumption
	* to confirm the block number in the sector descriptor is
	* the correct block to erase for the given address.
	*
	* This is an optional sanity check against the sector configuration
	*/
	إذا (اتے_progmem_isuniform()) {
		ssize_t ورقہ = اتے_progmem_getpage((size_t)pf);

		إذا (ورقہ < 0) {
			rv = ورقہ;
		}

		إذا (rv == 0) {
			size_t ورقہ_ناپ  = اتے_progmem_pagesize(ورقہ);
			size_t بلاک_ناپ = اتے_progmem_erasesize(sm->ورقہ);
			size_t بلاک_عدد  = ((ورقہ * ورقہ_ناپ) / بلاک_ناپ);

			إذا (بلاک_عدد != sm->ورقہ) {
				rv = -EINVAL;
			}
		}
	}

	إذا (rv == 0) {

		ssize_t ناپ_erased = اتے_progmem_eraseblock(sm->ورقہ);

		إذا (ناپ_erased < 0 || ناپ_erased != (ssize_t)sm->ناپ) {
			rv = ناپ_erased;
		}

	}

	أرجع rv;
}

/****************************************************************************
 * Name: write_flash_entry
 *
 * Description:
 *   Writes a 32-byte-aligned flash entry to flash memory
 *   Notably, does not write the flag_erased line, but reports
 *   the line as written regardless; the line's contents must remain
 *   unwritten to avoid possible CRC errors in hardware later while erasing.
 *
 * Input Parameters:
 *   pf       - Address in flash to which to write
 *   pn       - Pointer to flash_entry_header_t to which to write
 *
 * Returned value:
 *   On success the number of bytes written (including the unwritten
 *     flag_erased line). On Error, a negative value of errno
 *
 ****************************************************************************/

ساكن صحيح write_flash_اندراج(size_t pf, ثابت flash_اندراج_سرخی_t *pn)
{
	ssize_t rv;
	size_t سطر_ناپ = اتے_progmem_pagesize(اتے_progmem_getpage(pf));
	uint8_t *پہلا_flag_سطر  = (uint8_t *)pn;
	uint8_t *content_بفر   = (uint8_t *)(&pn->چکری_جانچ);

	/* write first line (magic, flag) */
	rv = اتے_progmem_write(pf, پہلا_flag_سطر, سطر_ناپ);

	إذا (rv < 0) {
		أرجع rv;
	}

	/* pretend to write the second line (flag_erased) */
	rv += سطر_ناپ;

	/* write the remaining contents */
	ssize_t rv2 = اتے_progmem_write(pf + 2 * سطر_ناپ, content_بفر, pn->ناپ - 2 * سطر_ناپ);

	إذا (rv2 < 0) {
		أرجع rv2;
	}

	أرجع rv + rv2;
}//write_flash_entry

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

صحيح parameter_flashfs_read(flash_file_token_t token, uint8_t **بفر_2, size_t
			   *buf_ناپ)
{
	صحيح rv = -ENXIO;

	إذا (sector_map) {

		rv = -ENOENT;
		flash_اندراج_سرخی_t *pf = لبھو_اندراج(token);

		إذا (pf) {
			(*بفر_2) = اندراج_انکڑے(pf);
			rv = اندراج_انکڑے_لمائی(pf);
			*buf_ناپ = rv;
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

	لكل (صحيح s = 0; sector_map[s].پتہ; s++) {
		uint32_t *pm = (uint32_t *) sector_map[s].پتہ;
		uint32_t *pe = pm + (sector_map[s].ناپ / حجم(uint32_t));

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
parameter_flashfs_write(flash_file_token_t token, uint8_t *بفر_2, size_t buf_ناپ)
{
	صحيح rv = -ENXIO;

	إذا (sector_map) {

		rv = 0;

		/* Calculate the total space needed */
		size_t کل_ناپ = buf_ناپ + حجم(flash_اندراج_سرخی_t);
		size_t alignment = ناپ_نقاب;///< 32-byte flash row
		size_t  ناپ_adjust = ((کل_ناپ + alignment) & ~alignment) - کل_ناپ;
		کل_ناپ += ناپ_adjust;

		إذا (کل_ناپ > UINT16_MAX) {
			أرجع -EFBIG;
		}

		/* Append after the last CRC-valid record. Compaction is the caller's job. */
		flash_اندراج_سرخی_t *pf = لبھو_append(کل_ناپ);

		إذا (pf == 0) {
			أرجع -ENOSPC;
		}

		flash_اندراج_سرخی_t *pn = (flash_اندراج_سرخی_t *)(بفر_2 - حجم(flash_اندراج_سرخی_t));
		pn->magic = MagicSig;
		pn->file_token.t = token.t;
		pn->flag = Valid_اندراج + ناپ_adjust;
		pn->ناپ = کل_ناپ;

		لكل (size_t a = 0; a < ناپ_adjust; a++) {
			بفر_2[buf_ناپ + a] = (uint8_t)BlankSig;
		}

		pn->چکری_جانچ = چکری_جانچ_32(اندراج_چکری_جانچ_شروع(pn), اندراج_چکری_جانچ_لمائی(pn));
		rv = write_flash_اندراج((size_t)pf, pn);
		صحيح system_بائٹاں = (حجم(flash_اندراج_سرخی_t) + ناپ_adjust);

		إذا (rv >= system_بائٹاں) {
			rv -= system_بائٹاں;
		}
	}

	أرجع rv;
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

صحيح parameter_flashfs_alloc(flash_file_token_t token, uint8_t **بفر_2, size_t *buf_ناپ)
{
	صحيح rv = -ENXIO;

	إذا (sector_map) {

		rv = -ENOMEM;

		إذا (!working_بفر_static) {

			working_بفر_ناپ = *buf_ناپ + حجم(flash_اندراج_سرخی_t) + ناپ_نقاب;
			working_بفر = حافظہ_دیو(working_بفر_ناپ);

		}

		/* Allocation failed or not provided */

		إذا (working_بفر == عدم) {

			working_بفر_ناپ = 0;

		} وإلا {

			/* We have a buffer reserve space and init it */
			*بفر_2 = &working_بفر[حجم(flash_اندراج_سرخی_t)];
			*buf_ناپ = working_بفر_ناپ - حجم(flash_اندراج_سرخی_t) - ناپ_نقاب;
			memset(working_بفر, 0xff, working_بفر_ناپ);
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

		لكل (صحيح s = 0; sector_map[s].پتہ; s++) {
			صحيح sz = erase_sector(&sector_map[s], (flash_اندراج_سرخی_t *)sector_map[s].پتہ);

			إذا (sz != 0) {
				أرجع sz;
			}

			rv += sector_map[s].ناپ;
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

ساكن صحيح walk_one(flash_اندراج_سرخی_t *pf, فراغ *arg)
{
	بنية walk_pack *w = (بنية walk_pack *)arg;
	أرجع w->cb(اندراج_انکڑے(pf), اندراج_انکڑے_لمائی(pf), w->arg);
}

صحيح parameter_flashfs_walk(flash_file_token_t token, parameter_flashfs_walk_cb cb, فراغ *arg)
{
	بنية walk_pack w = { cb, arg };
	أرجع for_each_valid_اندراج(token, walk_one, &w);
}

/****************************************************************************
 * Name: parameter_flashfs_needs_compact
 ****************************************************************************/

size_t parameter_flashfs_max_payload(فراغ)
{
	إذا (sector_map == عدم) {
		أرجع 0;
	}

	size_t max_کل = 0;

	لكل (صحيح s = 0; sector_map[s].پتہ; s++) {
		إذا (sector_usable(s) > max_کل) {
			max_کل = sector_usable(s);
		}
	}

	/* h_size_t holds the record size */
	إذا (max_کل > UINT16_MAX) {
		max_کل = UINT16_MAX;
	}

	max_کل &= ~(size_t)ناپ_نقاب;

	إذا (max_کل <= حجم(flash_اندراج_سرخی_t)) {
		أرجع 0;
	}

	أرجع max_کل - حجم(flash_اندراج_سرخی_t);
}

صحيح parameter_flashfs_needs_compact(flash_file_token_t token, size_t snapshot_ناپ)
{
	إذا (sector_map == عدم) {
		أرجع -ENXIO;
	}

	flash_اندراج_سرخی_t *پہلا = لبھو_اندراج(token);

	إذا (پہلا == عدم) {
		أرجع 0;
	}

	/* Dead records ahead of the live data (a torn write, or the erased
	 * prefix the single-snapshot writer left) only go away with an erase. */
	إذا ((uintptr_t)پہلا != (uintptr_t)sector_map[0].پتہ) {
		أرجع 1;
	}

	/* Keep room after the log for a burst the size of the snapshot
	 * (reset-all tombstones, airframe PARAM_SET), floored so a UUID append
	 * does not force a boot erase. */
	ثابت size_t align = ناپ_نقاب;
	ثابت size_t snapshot_کل = (snapshot_ناپ + حجم(flash_اندراج_سرخی_t) + align) & ~align;
	ثابت size_t headroom = snapshot_کل > 8192 ? snapshot_کل : 8192;

	إذا (لبھو_append(headroom) != عدم) {
		أرجع 0;
	}

	/* Compaction leaves the snapshot at the start of sector 0 and every
	 * other sector blank. Erase only if that layout has the room this one
	 * lacks, or every boot would pay the erase for nothing. */
	إذا (sector_usable(0) >= snapshot_کل + headroom) {
		أرجع 1;
	}

	لكل (صحيح s = 1; sector_map[s].پتہ; s++) {
		إذا (sector_usable(s) >= headroom) {
			أرجع 1;
		}
	}

	أرجع 0;
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

صحيح parameter_flashfs_init(sector_descriptor_t *fconfig, uint8_t *بفر_2, uint16_t ناپ)
{
	صحيح rv = 0;
	sector_map = fconfig;
	working_بفر_static = بفر_2 != عدم;

	إذا (!working_بفر_static) {
		ناپ = 0;
	}

	working_بفر = بفر_2;
	working_بفر_ناپ = ناپ;

	/* Sanity check */

	/* A non-blank store with no valid entry is torn or foreign; import
	 * reports -EILSEQ. Erasing here would hide a successful retry after
	 * a torn compact write. */

	أرجع rv;
}

#إذا defined(FLASH_UNIT_TEST)

ساكن sector_descriptor_t  test_sector_map[] = {
	{14, 128 * 1024, 0x081c0000},
	{15, 128 * 1024, 0x081e0000},
	{0, 0, 0},
};

__EXPORT فراغ test(فراغ);

uint8_t test_buf[32 * 1024];

__EXPORT فراغ test(فراغ)
{
	uint16_t سبھ_توں_وڈا_بلاک_2 = (32 * 1024) + 96;
	uint8_t *بفر_2 = حافظہ_دیو(سبھ_توں_وڈا_بلاک_2);

	parameter_flashfs_init(test_sector_map, بفر_2, سبھ_توں_وڈا_بلاک_2);

	لكل (صحيح t = 0; t < حجم(test_buf); t++) {
		test_buf[t] = (uint8_t) t;
	}

	صحيح er = parameter_flashfs_erase();
	uint8_t *fbuffer;
	size_t buf_ناپ;
	صحيح written = 0;
	صحيح اقرأ = 0;
	صحيح rv = 0;

	لكل (صحيح a = 0; a <= 4; a++) {
		parameter_flashfs_erase();
		rv =  parameter_flashfs_alloc(parameters_token, &fbuffer, &buf_ناپ);
		حافظہ_نقل(fbuffer, test_buf, a);
		buf_ناپ = a;
		written = parameter_flashfs_write(parameters_token, fbuffer, buf_ناپ);
		اقرأ = parameter_flashfs_read(parameters_token, &fbuffer, &buf_ناپ);
		parameter_flashfs_free();

		إذا (اقرأ != written) {
			ساكن متطاير صحيح j;
			j++;
		}
	}

	صحيح بلاک_2 = 2048;

	لكل (صحيح a = 0; a <= 8; a++) {
		parameter_flashfs_erase();
		rv =  parameter_flashfs_alloc(parameters_token, &fbuffer, &buf_ناپ);
		حافظہ_نقل(fbuffer, test_buf, بلاک_2);
		buf_ناپ = بلاک_2;
		written = parameter_flashfs_write(parameters_token, fbuffer, buf_ناپ);
		اقرأ = parameter_flashfs_read(parameters_token, &fbuffer, &buf_ناپ);
		parameter_flashfs_free();

		إذا (اقرأ != written) {
			ساكن متطاير صحيح j;
			j++;
		}

		بلاک_2 += 2048;
	}

	rv++;
	er++;
	حرر(بفر_2);
}
#جے_انت /* FLASH_UNIT_TEST */
#جے_انت /* CONFIG_ARCH_HAVE_PROGMEM */
