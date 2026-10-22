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

#شامل <px4_platform_common/defines.h>
#شامل <px4_platform_common/posix.h>
#شامل <nuttx/crc32.h>
#شامل <stddef.h>
#شامل <string.h>
#شامل <stdbool.h>
#شامل <stdint.h>
#شامل <stdlib.h>
#شامل <errno.h>
#شامل "flashfs.h"
#شامل <nuttx/compiler.h>
#شامل <nuttx/progmem.h>
#شامل <board_config.h>

#اگر defined(CONFIG_ARCH_HAVE_PROGMEM) || defined(BOARD_USE_EXTERNAL_FLASH)

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Private Types
 ****************************************************************************/

قسمتعریف uint32_t h_magic_t;
قسمتعریف uint16_t h_سائز_t;
قسمتعریف uint16_t h_flag_t;
قسمتعریف uint16_t ڈیٹا_سائز_t;

قسمتعریف شمار  flash_config_t {
	سب_سے_بڑا_بلاک = 2 * 1024, // This represents the size need for backing store
	MagicSig     = 0xaa553cc3,
	BlankSig     = 0xffffffff
} flash_config_t;

قسمتعریف شمار  flash_flags_t {
	سائز_نقاب      = 0x0003,
	نقاب_اندراج     = ~سائز_نقاب,
	Blank_اندراج    = (h_flag_t)BlankSig,
	Valid_اندراج    = (0xa5ac & ~سائز_نقاب),
	Erased_اندراج   = 0x0000,
} flash_flags_t;


/* The struct flash_entry_header_t will be sizeof(uint32_t) aligned
 * The Size will be the actual length of the header plus the data
 * and any padding needed to have the size be an even multiple of
 * sizeof(uint32_t)
 */
#ہدایت GCC diagnostic ڈالو
#ہدایت GCC diagnostic ignored "-Wattributes"
قسمتعریف begin_packed_struct ساخت flash_اندراج_سرخی_t {
	h_magic_t            magic;           /* Used to ID files*/
	h_flag_t             flag;            /* Used to erase this entry */
	uint32_t             چکری_جانچ;             /* Calculated over the size - end of data */
	h_سائز_t             سائز;            /* When added to a byte pointer to flash_entry_header_t
                                               * Will result the offset of the next active file or
                                               * free space. */
	flash_file_token_t   file_token;      /* file token type - essentially the name/type */
} انتہا_packed_struct flash_اندراج_سرخی_t __attribute__((aligned(حجم(uint32_t))));
#ہدایت GCC diagnostic نکالو

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/
ساکن uint8_t *working_بفر;
ساکن size_t working_بفر_سائز;
ساکن bool working_بفر_static;
ساکن sector_descriptor_t *sector_map;
ساکن صحیح آخری_erased;

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

خالی parameter_flashfs_free(خالی)
{
	اگر (!working_بفر_static && working_بفر != تہی) {
		آزاد(working_بفر);
		working_بفر = تہی;
		working_بفر_سائز = 0;
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

ساکن اندرونی صحیح blank_flash(uint32_t *pf)
{
	واپس *pf == BlankSig;
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

ساکن bool blank_check(flash_اندراج_سرخی_t *pf,
			size_t new_سائز)
{
	bool rv = true;
	uint32_t *pm = (uint32_t *) pf;
	new_سائز /= حجم(uint32_t);

	جب (new_سائز-- && rv) {
		اگر (!blank_flash(pm++)) {
			rv = false;
		}
	}

	واپس rv;
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

ساکن اندرونی صحیح valid_magic(h_magic_t *pm)
{
	واپس *pm == MagicSig;
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

ساکن اندرونی صحیح blank_اندراج(flash_اندراج_سرخی_t *fi)
{
	واپس fi->magic == BlankSig  &&  fi->flag == Blank_اندراج;
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

ساکن اندرونی صحیح valid_اندراج(flash_اندراج_سرخی_t *fi)
{
	واپس (fi->flag & نقاب_اندراج) == Valid_اندراج;
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

ساکن اندرونی صحیح اندراج_سائز_adjust(flash_اندراج_سرخی_t *fi)
{
	واپس fi->flag & سائز_نقاب;
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

ساکن اندرونی flash_اندراج_سرخی_t *اگلا_اندراج(flash_اندراج_سرخی_t *fi)
{
	uint8_t *pb = (uint8_t *)fi;
	واپس (flash_اندراج_سرخی_t *)(pb + fi->سائز);
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

ساکن اندرونی uint8_t *اندراج_ڈیٹا(flash_اندراج_سرخی_t *fi)
{
	واپس ((uint8_t *)fi) + حجم(flash_اندراج_سرخی_t);
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

ساکن اندرونی ڈیٹا_سائز_t اندراج_ڈیٹا_لمبائی(flash_اندراج_سرخی_t *fi)
{
	واپس fi->سائز - (حجم(flash_اندراج_سرخی_t) + اندراج_سائز_adjust(fi));
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

ساکن اندرونی ثابت uint8_t *اندراج_چکری_جانچ_آغاز(flash_اندراج_سرخی_t *fi)
{
	واپس (ثابت uint8_t *)&fi->سائز;
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

ساکن اندرونی ڈیٹا_سائز_t اندراج_چکری_جانچ_لمبائی(flash_اندراج_سرخی_t *fi)
{
	واپس fi->سائز - offsetof(flash_اندراج_سرخی_t, سائز);
}

/* The parameter sector can be the last one on the chip, so no header
 * field may be read until the whole header is known to lie inside it. */
ساکن اندرونی bool سرخی_سماتا(flash_اندراج_سرخی_t *pf, h_magic_t *pe)
{
	واپس pf + 1 <= (flash_اندراج_سرخی_t *)pe;
}

ساکن bool چکری_جانچ_سرخی_ok(flash_اندراج_سرخی_t *pf, h_magic_t *pe)
{
	اگر (!سرخی_سماتا(pf, pe)) {
		واپس false;
	}

	اگر (pf->سائز < حجم(flash_اندراج_سرخی_t)) {
		واپس false;
	}

	ثابت uint8_t *چکری_جانچ_آغاز = اندراج_چکری_جانچ_آغاز(pf);
	ڈیٹا_سائز_t چکری_جانچ_لمبائی = اندراج_چکری_جانچ_لمبائی(pf);

	اگر (چکری_جانچ_آغاز + چکری_جانچ_لمبائی > (uint8_t *)pe) {
		واپس false;
	}

	واپس pf->چکری_جانچ == چکری_جانچ_32(چکری_جانچ_آغاز, چکری_جانچ_لمبائی);
}

/* The walkers stop at the last word of a sector and reject a record that
 * ends beyond it, so nothing may be written into that word. */
ساکن اندرونی size_t sector_usable(صحیح s)
{
	واپس sector_map[s].سائز - حجم(h_magic_t);
}

ساکن bool خانہ_is_free(flash_اندراج_سرخی_t *pf, صحیح s, size_t required)
{
	uint8_t *begin = (uint8_t *)sector_map[s].پتہ;
	uint8_t *انتہا = begin + sector_usable(s);

	اگر ((uint8_t *)pf < begin || required == 0 || (uint8_t *)pf + required > انتہا) {
		واپس false;
	}

	/* Entries start on a flash row. up_progmem_write only asserts that in
	 * debug builds, so a slot found by the word-wise scan behind a torn
	 * record would otherwise be programmed misaligned. */
	اگر (((uintptr_t)pf & سائز_نقاب) != 0) {
		واپس false;
	}

	واپس blank_اندراج(pf) && blank_check(pf, required);
}

ساکن صحیح for_each_valid_اندراج(flash_file_token_t token,
				صحیح (*cb)(flash_اندراج_سرخی_t *pf, خالی *arg),
				خالی *arg);

ساکن صحیح stop_at_پہلا(flash_اندراج_سرخی_t *pf, خالی *arg)
{
	*(flash_اندراج_سرخی_t **)arg = pf;
	واپس -ECANCELED;
}

/* Oldest valid entry for token, or NULL. */
ساکن flash_اندراج_سرخی_t *ڈھونڈو_اندراج(flash_file_token_t token)
{
	flash_اندراج_سرخی_t *pf = تہی;
	for_each_valid_اندراج(token, stop_at_پہلا, &pf);
	واپس pf;
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

ساکن flash_اندراج_سرخی_t *ڈھونڈو_append(size_t required)
{
	اگر (sector_map == تہی || required == 0) {
		واپس تہی;
	}

	flash_اندراج_سرخی_t *آخری = تہی;
	صحیح آخری_s = -1;

	برائے (صحیح s = 0; sector_map[s].پتہ; s++) {
		h_magic_t *pmagic = (h_magic_t *) sector_map[s].پتہ;
		h_magic_t *pe = pmagic + (sector_map[s].سائز / حجم(h_magic_t)) - 1;

		جب (pmagic < pe) {
			اگر (!valid_magic(pmagic)) {
				pmagic++;
				جاری;
			}

			flash_اندراج_سرخی_t *pf = (flash_اندراج_سرخی_t *) pmagic;

			اگر (چکری_جانچ_سرخی_ok(pf, pe)) {
				آخری = pf;
				آخری_s = s;
				pmagic = (h_magic_t *) اگلا_اندراج(pf);
				pf = (flash_اندراج_سرخی_t *) pmagic;

				/* Same stop rule as for_each_valid_entry: a record behind a
				 * blank header is invisible to replay, so it must not anchor
				 * the append point either. */
				اگر (pmagic >= pe || !سرخی_سماتا(pf, pe) || blank_اندراج(pf)) {
					توڑو;
				}

			} ورنہ {
				pmagic++;
			}
		}
	}

	ثابت صحیح آغاز_s = (آخری == تہی) ? 0 : آخری_s;
	h_magic_t *pmagic = (آخری == تہی)
			    ? (h_magic_t *) sector_map[0].پتہ
			    : (h_magic_t *) اگلا_اندراج(آخری);

	برائے (صحیح s = آغاز_s; sector_map[s].پتہ; s++) {
		اگر (s != آغاز_s) {
			pmagic = (h_magic_t *) sector_map[s].پتہ;
		}

		h_magic_t *pe = (h_magic_t *) sector_map[s].پتہ
				+ (sector_map[s].سائز / حجم(h_magic_t)) - 1;

		اگر ((uint8_t *)pmagic < (uint8_t *)sector_map[s].پتہ
		    || (uint8_t *)pmagic >= (uint8_t *)sector_map[s].پتہ + sector_map[s].سائز) {
			اگر (s == آغاز_s) {
				جاری;
			}

			pmagic = (h_magic_t *) sector_map[s].پتہ;
		}

		جب (pmagic < pe) {
			flash_اندراج_سرخی_t *pf = (flash_اندراج_سرخی_t *) pmagic;

			اگر (valid_magic(pmagic) && چکری_جانچ_سرخی_ok(pf, pe)) {
				pmagic = (h_magic_t *) اگلا_اندراج(pf);
				جاری;
			}

			اگر (خانہ_is_free(pf, s, required)) {
				واپس pf;
			}

			pmagic++;
		}
	}

	واپس تہی;
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

ساکن صحیح for_each_valid_اندراج(flash_file_token_t token,
				صحیح (*cb)(flash_اندراج_سرخی_t *pf, خالی *arg),
				خالی *arg)
{
	صحیح n = 0;

	اگر (sector_map == تہی) {
		واپس -ENXIO;
	}

	برائے (صحیح s = 0; sector_map[s].پتہ; s++) {

		h_magic_t *pmagic = (h_magic_t *) sector_map[s].پتہ;
		h_magic_t *pe = pmagic + (sector_map[s].سائز / حجم(h_magic_t)) - 1;

cont:

		جب (pmagic < pe && !valid_magic(pmagic)) {
			pmagic++;
		}

		اگر (pmagic >= pe) { جاری; }

		flash_اندراج_سرخی_t *pf = (flash_اندراج_سرخی_t *) pmagic;

		اگر (چکری_جانچ_سرخی_ok(pf, pe)) {

			اگر (valid_اندراج(pf) && pf->file_token.t == token.t) {
				اگر (cb != تہی) {
					صحیح rv = cb(pf, arg);

					اگر (rv < 0) {
						واپس rv;
					}
				}

				n++;
			}

			pf = اگلا_اندراج(pf);
			pmagic = (h_magic_t *) pf;

			اگر (pmagic >= pe || !سرخی_سماتا(pf, pe) || blank_اندراج(pf)) {
				جاری;
			}

		} ورنہ {
			pmagic++;
		}

		جاؤ cont;
	}

	واپس n;
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

ساکن صحیح erase_sector(sector_descriptor_t *sm, flash_اندراج_سرخی_t *pf)
{
	صحیح rv = 0;
#اگر defined(BOARD_USE_EXTERNAL_FLASH)
	ssize_t بلاک_2 = اوپر_progmem_ext_getpage((size_t)pf);
#ورنہ
	ssize_t بلاک_2 = اوپر_progmem_getpage((size_t)pf);
#اگر_ختم

	اگر (بلاک_2 > 0 && بلاک_2 == sm->صفحہ) {
		آخری_erased = sm->صفحہ;
#اگر defined(BOARD_USE_EXTERNAL_FLASH)
		ssize_t سائز = اوپر_progmem_ext_eraseblock(بلاک_2);
#ورنہ
		ssize_t سائز = اوپر_progmem_eraseblock(بلاک_2);
#اگر_ختم

		اگر (سائز < 0 || سائز != (ssize_t)sm->سائز) {
			rv = سائز;
		}
	}

	واپس rv;
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

صحیح parameter_flashfs_read(flash_file_token_t token, uint8_t **بفر_2, size_t
			   *buf_سائز)
{
	صحیح rv = -ENXIO;

	اگر (sector_map) {

		rv = -ENOENT;
		flash_اندراج_سرخی_t *pf = ڈھونڈو_اندراج(token);

		اگر (pf) {
			(*بفر_2) = اندراج_ڈیٹا(pf);
			rv = اندراج_ڈیٹا_لمبائی(pf);
			*buf_سائز = rv;
		}
	}

	واپس rv;
}

/****************************************************************************
 * Name: parameter_flashfs_blank
 *
 * Description:
 *   Reports whether the parameter storage is fully erased, so callers can
 *   tell a blank store from one that holds unparseable data.
 *
 ****************************************************************************/

صحیح parameter_flashfs_blank(خالی)
{
	اگر (sector_map == تہی) {
		واپس -ENXIO;
	}

	برائے (صحیح s = 0; sector_map[s].پتہ; s++) {
		uint32_t *pm = (uint32_t *) sector_map[s].پتہ;
		uint32_t *pe = pm + (sector_map[s].سائز / حجم(uint32_t));

		جب (pm < pe) {
			اگر (!blank_flash(pm++)) {
				واپس 0;
			}
		}
	}

	واپس 1;
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

صحیح
parameter_flashfs_write(flash_file_token_t token, uint8_t *بفر_2, size_t buf_سائز)
{
	صحیح rv = -ENXIO;

	اگر (sector_map) {

		rv = 0;

		/* Calculate the total space needed */

		size_t کل_سائز = buf_سائز + حجم(flash_اندراج_سرخی_t);
		size_t alignment = حجم(h_magic_t) - 1;
		size_t  سائز_adjust = ((کل_سائز + alignment) & ~alignment) - کل_سائز;
		کل_سائز += سائز_adjust;

		اگر (کل_سائز > UINT16_MAX) {
			واپس -EFBIG;
		}

		/* Append after the last CRC-valid record. Compaction is the caller's job. */
		flash_اندراج_سرخی_t *pf = ڈھونڈو_append(کل_سائز);

		اگر (pf == 0) {
			واپس -ENOSPC;
		}

		flash_اندراج_سرخی_t *pn = (flash_اندراج_سرخی_t *)(بفر_2 - حجم(flash_اندراج_سرخی_t));
		pn->magic = MagicSig;
		pn->file_token.t = token.t;
		pn->flag = Valid_اندراج + سائز_adjust;
		pn->سائز = کل_سائز;

		برائے (size_t a = 0; a < سائز_adjust; a++) {
			بفر_2[buf_سائز + a] = (uint8_t)BlankSig;
		}

		pn->چکری_جانچ = چکری_جانچ_32(اندراج_چکری_جانچ_آغاز(pn), اندراج_چکری_جانچ_لمبائی(pn));
#اگر defined(BOARD_USE_EXTERNAL_FLASH)
		rv = اوپر_progmem_ext_write((size_t) pf, pn, pn->سائز);
#ورنہ
		rv = اوپر_progmem_write((size_t) pf, pn, pn->سائز);
#اگر_ختم
		صحیح system_بائٹیں = (حجم(flash_اندراج_سرخی_t) + سائز_adjust);

		اگر (rv >= system_بائٹیں) {
			rv -= system_بائٹیں;
		}
	}

	واپس rv;
}

/****************************************************************************
 * Name: parameter_flashfs_walk
 ****************************************************************************/

ساخت walk_pack {
	parameter_flashfs_walk_cb cb;
	خالی *arg;
};

ساکن صحیح walk_one(flash_اندراج_سرخی_t *pf, خالی *arg)
{
	ساخت walk_pack *w = (ساخت walk_pack *)arg;
	واپس w->cb(اندراج_ڈیٹا(pf), اندراج_ڈیٹا_لمبائی(pf), w->arg);
}

صحیح parameter_flashfs_walk(flash_file_token_t token, parameter_flashfs_walk_cb cb, خالی *arg)
{
	ساخت walk_pack w = { cb, arg };
	واپس for_each_valid_اندراج(token, walk_one, &w);
}

/****************************************************************************
 * Name: parameter_flashfs_needs_compact
 ****************************************************************************/

size_t parameter_flashfs_max_payload(خالی)
{
	اگر (sector_map == تہی) {
		واپس 0;
	}

	size_t max_کل = 0;

	برائے (صحیح s = 0; sector_map[s].پتہ; s++) {
		اگر (sector_usable(s) > max_کل) {
			max_کل = sector_usable(s);
		}
	}

	/* h_size_t holds the record size */
	اگر (max_کل > UINT16_MAX) {
		max_کل = UINT16_MAX;
	}

	max_کل &= ~(size_t)سائز_نقاب;

	اگر (max_کل <= حجم(flash_اندراج_سرخی_t)) {
		واپس 0;
	}

	واپس max_کل - حجم(flash_اندراج_سرخی_t);
}

صحیح parameter_flashfs_needs_compact(flash_file_token_t token, size_t snapshot_سائز)
{
	اگر (sector_map == تہی) {
		واپس -ENXIO;
	}

	flash_اندراج_سرخی_t *پہلا = ڈھونڈو_اندراج(token);

	اگر (پہلا == تہی) {
		واپس 0;
	}

	/* Dead records ahead of the live data (a torn write, or the erased
	 * prefix the single-snapshot writer left) only go away with an erase. */
	اگر ((uintptr_t)پہلا != (uintptr_t)sector_map[0].پتہ) {
		واپس 1;
	}

	/* Keep room after the log for a burst the size of the snapshot
	 * (reset-all tombstones, airframe PARAM_SET), floored so a UUID append
	 * does not force a boot erase. */
	ثابت size_t align = حجم(h_magic_t) - 1;
	ثابت size_t snapshot_کل = (snapshot_سائز + حجم(flash_اندراج_سرخی_t) + align) & ~align;
	ثابت size_t headroom = snapshot_کل > 8192 ? snapshot_کل : 8192;

	اگر (ڈھونڈو_append(headroom) != تہی) {
		واپس 0;
	}

	/* Compaction leaves the snapshot at the start of sector 0 and every
	 * other sector blank. Erase only if that layout has the room this one
	 * lacks, or every boot would pay the erase for nothing. */
	اگر (sector_usable(0) >= snapshot_کل + headroom) {
		واپس 1;
	}

	برائے (صحیح s = 1; sector_map[s].پتہ; s++) {
		اگر (sector_usable(s) >= headroom) {
			واپس 1;
		}
	}

	واپس 0;
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

صحیح parameter_flashfs_alloc(flash_file_token_t token, uint8_t **بفر_2, size_t *buf_سائز)
{
	صحیح rv = -ENXIO;

	اگر (sector_map) {

		rv = -ENOMEM;

		اگر (!working_بفر_static) {

			/* parameter_flashfs_write pads the entry to a word in place */
			working_بفر_سائز = *buf_سائز + حجم(flash_اندراج_سرخی_t) + سائز_نقاب;
			working_بفر = حافظہ_دو(working_بفر_سائز);

		}

		/* Allocation failed or not provided */

		اگر (working_بفر == تہی) {

			working_بفر_سائز = 0;

		} ورنہ {

			/* We have a buffer reserve space and init it */
			*بفر_2 = &working_بفر[حجم(flash_اندراج_سرخی_t)];
			*buf_سائز = working_بفر_سائز - حجم(flash_اندراج_سرخی_t) - سائز_نقاب;
			memset(working_بفر, 0xff, working_بفر_سائز);
			rv = 0;

		}
	}

	واپس rv;
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

صحیح parameter_flashfs_erase(خالی)
{
	صحیح rv = -ENXIO;

	اگر (sector_map) {
		rv = 0;

		برائے (صحیح s = 0; sector_map[s].پتہ; s++) {
			صحیح sz = erase_sector(&sector_map[s], (flash_اندراج_سرخی_t *)sector_map[s].پتہ);

			اگر (sz != 0) {
				واپس sz;
			}

			rv += sector_map[s].سائز;
		}
	}

	واپس rv;
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

صحیح parameter_flashfs_init(sector_descriptor_t *fconfig, uint8_t *بفر_2, uint16_t سائز)
{
	صحیح rv = 0;
	sector_map = fconfig;
	working_بفر_static = بفر_2 != تہی;

	اگر (!working_بفر_static) {
		سائز = 0;
	}

	working_بفر = بفر_2;
	working_بفر_سائز = سائز;
	آخری_erased = -1;

	/* Sanity check */

	/* A non-blank store with no valid entry is torn or foreign; import
	 * reports -EILSEQ. Erasing here would hide a successful retry after
	 * a torn write. */

	واپس rv;
}

#اگر defined(FLASH_UNIT_TEST)

ساکن sector_descriptor_t  test_sector_map[] = {
	{1, 16 * 1024, 0x08004000},
	{2, 16 * 1024, 0x08008000},
	{0, 0, 0},
};

__EXPORT خالی test(خالی);

uint8_t test_buf[32 * 1024];

__EXPORT خالی test(خالی)
{
	uint16_t سب_سے_بڑا_بلاک_2 = (32 * 1024) + 32;
	uint8_t *بفر_2 = حافظہ_دو(سب_سے_بڑا_بلاک_2);

	parameter_flashfs_init(test_sector_map, بفر_2, سب_سے_بڑا_بلاک_2);

	برائے (صحیح t = 0; t < حجم(test_buf); t++) {
		test_buf[t] = (uint8_t) t;
	}

	صحیح er = parameter_flashfs_erase();
	uint8_t *fbuffer;
	size_t buf_سائز;
	صحیح written = 0;
	صحیح اقرأ = 0;
	صحیح rv = 0;

	برائے (صحیح a = 0; a <= 4; a++) {
		parameter_flashfs_erase();
		rv =  parameter_flashfs_alloc(parameters_token, &fbuffer, &buf_سائز);
		حافظہ_نقل(fbuffer, test_buf, a);
		buf_سائز = a;
		written = parameter_flashfs_write(parameters_token, fbuffer, buf_سائز);
		اقرأ = parameter_flashfs_read(parameters_token, &fbuffer, &buf_سائز);
		parameter_flashfs_free();

		اگر (اقرأ != written) {
			ساکن متلون صحیح j;
			j++;
		}
	}

	صحیح بلاک_2 = 2048;

	برائے (صحیح a = 0; a <= 8; a++) {
		parameter_flashfs_erase();
		rv =  parameter_flashfs_alloc(parameters_token, &fbuffer, &buf_سائز);
		حافظہ_نقل(fbuffer, test_buf, بلاک_2);
		buf_سائز = بلاک_2;
		written = parameter_flashfs_write(parameters_token, fbuffer, buf_سائز);
		اقرأ = parameter_flashfs_read(parameters_token, &fbuffer, &buf_سائز);
		parameter_flashfs_free();

		اگر (اقرأ != written) {
			ساکن متلون صحیح j;
			j++;
		}

		بلاک_2 += 2048;
	}

	rv++;
	er++;
	آزاد(بفر_2);
}
#اگر_ختم /* FLASH_UNIT_TEST */
#اگر_ختم /* CONFIG_ARCH_HAVE_PROGMEM */
