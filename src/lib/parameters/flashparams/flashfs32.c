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

#اگر defined(CONFIG_ARCH_HAVE_PROGMEM)

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Private Types
 ****************************************************************************/

تعریفنوع uint32_t h_magic_t;
تعریفنوع uint16_t h_size_t;
تعریفنوع uint16_t h_flag_t;
تعریفنوع uint16_t data_size_t;

تعریفنوع شمارش  flash_config_t {
	LargestBlock = 2 * 1024, // This represents the size need for backing store
	MagicSig     = 0xaa553cc3,
	BlankSig     = 0xffffffff
} flash_config_t;

/* Aligns are done up to 32 bytes, so storing the size padding within 0x001f mask */
تعریفنوع شمارش  flash_flags_t {
	SizeMask      = 0x001f,
	MaskEntry     = ~SizeMask,
	BlankEntry    = (h_flag_t)BlankSig,
	ValidEntry    = (0xa5ac & ~SizeMask),
	ErasedEntry   = 0x0000,
} flash_flags_t;

/* The struct flash_entry_header_t will be 32-byte aligned
 * The Size will be the actual length of the header plus the data
 * and any padding needed to have the size be an even multiple of
 * the 32-byte flash line
 */
#pragma GCC diagnostic افزودن
#pragma GCC diagnostic ignored "-Wattributes"
تعریفنوع begin_packed_struct ساختار flash_entry_header_t {
	h_magic_t            magic;           /* Used to ID files*/
	h_flag_t             flag;            /* Used to mark this entry as valid */
	uint8_t              unused_buffer[32 - اندازه(h_magic_t) - اندازه(h_flag_t)]; /* padding */
	h_flag_t             flag_erased;     /* Used to erase this entry */
	uint8_t              unused_buffer2[32 - اندازه(h_flag_t)]; /* padding */
	uint32_t             crc;             /* Calculated over the size - end of data */
	h_size_t             size;            /* When added to a byte pointer to flash_entry_header_t
                                               * Will result the offset of the next active file or
                                               * free space. */
	flash_file_token_t   file_token;      /* file token type - essentially the name/type */
} end_packed_struct flash_entry_header_t __attribute__((aligned(اندازه(uint32_t))));
#pragma GCC diagnostic برداشت

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/
ایستا uint8_t *working_buffer;
ایستا size_t working_buffer_size;
ایستا bool working_buffer_static;
ایستا sector_descriptor_t *sector_map;

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

تهی parameter_flashfs_free(تهی)
{
	اگر (!working_buffer_static && working_buffer != پوچ) {
		آزاد(working_buffer);
		working_buffer = پوچ;
		working_buffer_size = 0;
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

ایستا درونخطی صحیح blank_flash(uint32_t *pf)
{
	بازگشت *pf == BlankSig;
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

ایستا bool blank_check(flash_entry_header_t *pf,
			size_t new_size)
{
	bool rv = true;
	uint32_t *pm = (uint32_t *) pf;
	new_size /= اندازه(uint32_t);

	وقتی (new_size-- && rv) {
		اگر (!blank_flash(pm++)) {
			rv = false;
		}
	}

	بازگشت rv;
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

ایستا درونخطی صحیح valid_magic(h_magic_t *pm)
{
	بازگشت *pm == MagicSig;
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

ایستا درونخطی صحیح blank_entry(flash_entry_header_t *fi)
{
	بازگشت fi->magic == BlankSig && fi->flag == BlankEntry && fi->flag_erased == BlankEntry;
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

ایستا درونخطی صحیح valid_entry(flash_entry_header_t *fi)
{
	بازگشت (fi->flag & MaskEntry) == ValidEntry && (fi->flag_erased) == BlankEntry;
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

ایستا درونخطی صحیح entry_size_adjust(flash_entry_header_t *fi)
{
	بازگشت fi->flag & SizeMask;
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

ایستا درونخطی flash_entry_header_t *next_entry(flash_entry_header_t *fi)
{
	uint8_t *pb = (uint8_t *)fi;
	بازگشت (flash_entry_header_t *)(pb + fi->size);
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

ایستا درونخطی uint8_t *entry_data(flash_entry_header_t *fi)
{
	بازگشت ((uint8_t *)fi) + اندازه(flash_entry_header_t);
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

ایستا درونخطی data_size_t entry_data_length(flash_entry_header_t *fi)
{
	بازگشت fi->size - (اندازه(flash_entry_header_t) + entry_size_adjust(fi));
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

ایستا درونخطی ثابت uint8_t *entry_crc_start(flash_entry_header_t *fi)
{
	بازگشت (ثابت uint8_t *)&fi->size;
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

ایستا درونخطی data_size_t entry_crc_length(flash_entry_header_t *fi)
{
	بازگشت fi->size - offsetof(flash_entry_header_t, size);
}

/* The parameter sector can be the last one on the chip, so no header
 * field may be read until the whole header is known to lie inside it. */
ایستا درونخطی bool header_fits(flash_entry_header_t *pf, h_magic_t *pe)
{
	بازگشت pf + 1 <= (flash_entry_header_t *)pe;
}

ایستا bool crc_header_ok(flash_entry_header_t *pf, h_magic_t *pe)
{
	اگر (!header_fits(pf, pe)) {
		بازگشت false;
	}

	اگر (pf->size < اندازه(flash_entry_header_t)) {
		بازگشت false;
	}

	ثابت uint8_t *crc_start = entry_crc_start(pf);
	data_size_t crc_length = entry_crc_length(pf);

	اگر (crc_start + crc_length > (uint8_t *)pe) {
		بازگشت false;
	}

	بازگشت pf->crc == crc32(crc_start, crc_length);
}

/* The walkers stop at the last word of a sector and reject a record that
 * ends beyond it, so nothing may be written into that word. */
ایستا درونخطی size_t sector_usable(صحیح s)
{
	بازگشت sector_map[s].size - اندازه(h_magic_t);
}

ایستا bool slot_is_free(flash_entry_header_t *pf, صحیح s, size_t required)
{
	uint8_t *begin = (uint8_t *)sector_map[s].address;
	uint8_t *end = begin + sector_usable(s);

	اگر ((uint8_t *)pf < begin || required == 0 || (uint8_t *)pf + required > end) {
		بازگشت false;
	}

	/* Entries start on a flash row. up_progmem_write only asserts that in
	 * debug builds, so a slot found by the word-wise scan behind a torn
	 * record would otherwise be programmed misaligned. */
	اگر (((uintptr_t)pf & SizeMask) != 0) {
		بازگشت false;
	}

	بازگشت blank_entry(pf) && blank_check(pf, required);
}

ایستا صحیح for_each_valid_entry(flash_file_token_t token,
				صحیح (*cb)(flash_entry_header_t *pf, تهی *arg),
				تهی *arg);

ایستا صحیح stop_at_first(flash_entry_header_t *pf, تهی *arg)
{
	*(flash_entry_header_t **)arg = pf;
	بازگشت -ECANCELED;
}

/* Oldest valid entry for token, or NULL. */
ایستا flash_entry_header_t *find_entry(flash_file_token_t token)
{
	flash_entry_header_t *pf = پوچ;
	for_each_valid_entry(token, stop_at_first, &pf);
	بازگشت pf;
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

ایستا flash_entry_header_t *find_append(size_t required)
{
	اگر (sector_map == پوچ || required == 0) {
		بازگشت پوچ;
	}

	flash_entry_header_t *last = پوچ;
	صحیح last_s = -1;

	برای (صحیح s = 0; sector_map[s].address; s++) {
		h_magic_t *pmagic = (h_magic_t *) sector_map[s].address;
		h_magic_t *pe = pmagic + (sector_map[s].size / اندازه(h_magic_t)) - 1;

		وقتی (pmagic < pe) {
			اگر (!valid_magic(pmagic)) {
				pmagic++;
				ادامه;
			}

			flash_entry_header_t *pf = (flash_entry_header_t *) pmagic;

			اگر (crc_header_ok(pf, pe)) {
				last = pf;
				last_s = s;
				pmagic = (h_magic_t *) next_entry(pf);
				pf = (flash_entry_header_t *) pmagic;

				/* Same stop rule as for_each_valid_entry: a record behind a
				 * blank header is invisible to replay, so it must not anchor
				 * the append point either. */
				اگر (pmagic >= pe || !header_fits(pf, pe) || blank_entry(pf)) {
					بشکن;
				}

			} وگرنه {
				pmagic++;
			}
		}
	}

	ثابت صحیح start_s = (last == پوچ) ? 0 : last_s;
	h_magic_t *pmagic = (last == پوچ)
			    ? (h_magic_t *) sector_map[0].address
			    : (h_magic_t *) next_entry(last);

	برای (صحیح s = start_s; sector_map[s].address; s++) {
		اگر (s != start_s) {
			pmagic = (h_magic_t *) sector_map[s].address;
		}

		h_magic_t *pe = (h_magic_t *) sector_map[s].address
				+ (sector_map[s].size / اندازه(h_magic_t)) - 1;

		اگر ((uint8_t *)pmagic < (uint8_t *)sector_map[s].address
		    || (uint8_t *)pmagic >= (uint8_t *)sector_map[s].address + sector_map[s].size) {
			اگر (s == start_s) {
				ادامه;
			}

			pmagic = (h_magic_t *) sector_map[s].address;
		}

		وقتی (pmagic < pe) {
			flash_entry_header_t *pf = (flash_entry_header_t *) pmagic;

			اگر (valid_magic(pmagic) && crc_header_ok(pf, pe)) {
				pmagic = (h_magic_t *) next_entry(pf);
				ادامه;
			}

			اگر (slot_is_free(pf, s, required)) {
				بازگشت pf;
			}

			pmagic++;
		}
	}

	بازگشت پوچ;
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

ایستا صحیح for_each_valid_entry(flash_file_token_t token,
				صحیح (*cb)(flash_entry_header_t *pf, تهی *arg),
				تهی *arg)
{
	صحیح n = 0;

	اگر (sector_map == پوچ) {
		بازگشت -ENXIO;
	}

	برای (صحیح s = 0; sector_map[s].address; s++) {

		h_magic_t *pmagic = (h_magic_t *) sector_map[s].address;
		h_magic_t *pe = pmagic + (sector_map[s].size / اندازه(h_magic_t)) - 1;

cont:

		وقتی (pmagic < pe && !valid_magic(pmagic)) {
			pmagic++;
		}

		اگر (pmagic >= pe) { ادامه; }

		flash_entry_header_t *pf = (flash_entry_header_t *) pmagic;

		اگر (crc_header_ok(pf, pe)) {

			اگر (valid_entry(pf) && pf->file_token.t == token.t) {
				اگر (cb != پوچ) {
					صحیح rv = cb(pf, arg);

					اگر (rv < 0) {
						بازگشت rv;
					}
				}

				n++;
			}

			pf = next_entry(pf);
			pmagic = (h_magic_t *) pf;

			اگر (pmagic >= pe || !header_fits(pf, pe) || blank_entry(pf)) {
				ادامه;
			}

		} وگرنه {
			pmagic++;
		}

		برو cont;
	}

	بازگشت n;
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

ایستا صحیح erase_sector(sector_descriptor_t *sm, flash_entry_header_t *pf)
{
	صحیح rv = 0;

	/*
	* Because up_progmem interface doesn't allow one-to-one check of
	* the block number for an address, use uniform-memory assumption
	* to confirm the block number in the sector descriptor is
	* the correct block to erase for the given address.
	*
	* This is an optional sanity check against the sector configuration
	*/
	اگر (up_progmem_isuniform()) {
		ssize_t page = up_progmem_getpage((size_t)pf);

		اگر (page < 0) {
			rv = page;
		}

		اگر (rv == 0) {
			size_t page_size  = up_progmem_pagesize(page);
			size_t block_size = up_progmem_erasesize(sm->page);
			size_t block_رقم  = ((page * page_size) / block_size);

			اگر (block_رقم != sm->page) {
				rv = -EINVAL;
			}
		}
	}

	اگر (rv == 0) {

		ssize_t size_erased = up_progmem_eraseblock(sm->page);

		اگر (size_erased < 0 || size_erased != (ssize_t)sm->size) {
			rv = size_erased;
		}

	}

	بازگشت rv;
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

ایستا صحیح write_flash_entry(size_t pf, ثابت flash_entry_header_t *pn)
{
	ssize_t rv;
	size_t سطر_size = up_progmem_pagesize(up_progmem_getpage(pf));
	uint8_t *first_flag_سطر  = (uint8_t *)pn;
	uint8_t *content_buffer   = (uint8_t *)(&pn->crc);

	/* write first line (magic, flag) */
	rv = up_progmem_write(pf, first_flag_سطر, سطر_size);

	اگر (rv < 0) {
		بازگشت rv;
	}

	/* pretend to write the second line (flag_erased) */
	rv += سطر_size;

	/* write the remaining contents */
	ssize_t rv2 = up_progmem_write(pf + 2 * سطر_size, content_buffer, pn->size - 2 * سطر_size);

	اگر (rv2 < 0) {
		بازگشت rv2;
	}

	بازگشت rv + rv2;
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

صحیح parameter_flashfs_read(flash_file_token_t token, uint8_t **buffer, size_t
			   *buf_size)
{
	صحیح rv = -ENXIO;

	اگر (sector_map) {

		rv = -ENOENT;
		flash_entry_header_t *pf = find_entry(token);

		اگر (pf) {
			(*buffer) = entry_data(pf);
			rv = entry_data_length(pf);
			*buf_size = rv;
		}
	}

	بازگشت rv;
}

/****************************************************************************
 * Name: parameter_flashfs_blank
 *
 * Description:
 *   Reports whether the parameter storage is fully erased, so callers can
 *   tell a blank store from one that holds unparseable data.
 *
 ****************************************************************************/

صحیح parameter_flashfs_blank(تهی)
{
	اگر (sector_map == پوچ) {
		بازگشت -ENXIO;
	}

	برای (صحیح s = 0; sector_map[s].address; s++) {
		uint32_t *pm = (uint32_t *) sector_map[s].address;
		uint32_t *pe = pm + (sector_map[s].size / اندازه(uint32_t));

		وقتی (pm < pe) {
			اگر (!blank_flash(pm++)) {
				بازگشت 0;
			}
		}
	}

	بازگشت 1;
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
parameter_flashfs_write(flash_file_token_t token, uint8_t *buffer, size_t buf_size)
{
	صحیح rv = -ENXIO;

	اگر (sector_map) {

		rv = 0;

		/* Calculate the total space needed */
		size_t total_size = buf_size + اندازه(flash_entry_header_t);
		size_t alignment = SizeMask;///< 32-byte flash row
		size_t  size_adjust = ((total_size + alignment) & ~alignment) - total_size;
		total_size += size_adjust;

		اگر (total_size > UINT16_MAX) {
			بازگشت -EFBIG;
		}

		/* Append after the last CRC-valid record. Compaction is the caller's job. */
		flash_entry_header_t *pf = find_append(total_size);

		اگر (pf == 0) {
			بازگشت -ENOSPC;
		}

		flash_entry_header_t *pn = (flash_entry_header_t *)(buffer - اندازه(flash_entry_header_t));
		pn->magic = MagicSig;
		pn->file_token.t = token.t;
		pn->flag = ValidEntry + size_adjust;
		pn->size = total_size;

		برای (size_t a = 0; a < size_adjust; a++) {
			buffer[buf_size + a] = (uint8_t)BlankSig;
		}

		pn->crc = crc32(entry_crc_start(pn), entry_crc_length(pn));
		rv = write_flash_entry((size_t)pf, pn);
		صحیح system_bytes = (اندازه(flash_entry_header_t) + size_adjust);

		اگر (rv >= system_bytes) {
			rv -= system_bytes;
		}
	}

	بازگشت rv;
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

صحیح parameter_flashfs_alloc(flash_file_token_t token, uint8_t **buffer, size_t *buf_size)
{
	صحیح rv = -ENXIO;

	اگر (sector_map) {

		rv = -ENOMEM;

		اگر (!working_buffer_static) {

			working_buffer_size = *buf_size + اندازه(flash_entry_header_t) + SizeMask;
			working_buffer = malloc(working_buffer_size);

		}

		/* Allocation failed or not provided */

		اگر (working_buffer == پوچ) {

			working_buffer_size = 0;

		} وگرنه {

			/* We have a buffer reserve space and init it */
			*buffer = &working_buffer[اندازه(flash_entry_header_t)];
			*buf_size = working_buffer_size - اندازه(flash_entry_header_t) - SizeMask;
			memset(working_buffer, 0xff, working_buffer_size);
			rv = 0;

		}
	}

	بازگشت rv;
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

صحیح parameter_flashfs_erase(تهی)
{
	صحیح rv = -ENXIO;

	اگر (sector_map) {
		rv = 0;

		برای (صحیح s = 0; sector_map[s].address; s++) {
			صحیح sz = erase_sector(&sector_map[s], (flash_entry_header_t *)sector_map[s].address);

			اگر (sz != 0) {
				بازگشت sz;
			}

			rv += sector_map[s].size;
		}
	}

	بازگشت rv;
}


/****************************************************************************
 * Name: parameter_flashfs_walk
 ****************************************************************************/

ساختار walk_pack {
	parameter_flashfs_walk_cb cb;
	تهی *arg;
};

ایستا صحیح walk_one(flash_entry_header_t *pf, تهی *arg)
{
	ساختار walk_pack *w = (ساختار walk_pack *)arg;
	بازگشت w->cb(entry_data(pf), entry_data_length(pf), w->arg);
}

صحیح parameter_flashfs_walk(flash_file_token_t token, parameter_flashfs_walk_cb cb, تهی *arg)
{
	ساختار walk_pack w = { cb, arg };
	بازگشت for_each_valid_entry(token, walk_one, &w);
}

/****************************************************************************
 * Name: parameter_flashfs_needs_compact
 ****************************************************************************/

size_t parameter_flashfs_max_payload(تهی)
{
	اگر (sector_map == پوچ) {
		بازگشت 0;
	}

	size_t max_total = 0;

	برای (صحیح s = 0; sector_map[s].address; s++) {
		اگر (sector_usable(s) > max_total) {
			max_total = sector_usable(s);
		}
	}

	/* h_size_t holds the record size */
	اگر (max_total > UINT16_MAX) {
		max_total = UINT16_MAX;
	}

	max_total &= ~(size_t)SizeMask;

	اگر (max_total <= اندازه(flash_entry_header_t)) {
		بازگشت 0;
	}

	بازگشت max_total - اندازه(flash_entry_header_t);
}

صحیح parameter_flashfs_needs_compact(flash_file_token_t token, size_t snapshot_size)
{
	اگر (sector_map == پوچ) {
		بازگشت -ENXIO;
	}

	flash_entry_header_t *first = find_entry(token);

	اگر (first == پوچ) {
		بازگشت 0;
	}

	/* Dead records ahead of the live data (a torn write, or the erased
	 * prefix the single-snapshot writer left) only go away with an erase. */
	اگر ((uintptr_t)first != (uintptr_t)sector_map[0].address) {
		بازگشت 1;
	}

	/* Keep room after the log for a burst the size of the snapshot
	 * (reset-all tombstones, airframe PARAM_SET), floored so a UUID append
	 * does not force a boot erase. */
	ثابت size_t align = SizeMask;
	ثابت size_t snapshot_total = (snapshot_size + اندازه(flash_entry_header_t) + align) & ~align;
	ثابت size_t headroom = snapshot_total > 8192 ? snapshot_total : 8192;

	اگر (find_append(headroom) != پوچ) {
		بازگشت 0;
	}

	/* Compaction leaves the snapshot at the start of sector 0 and every
	 * other sector blank. Erase only if that layout has the room this one
	 * lacks, or every boot would pay the erase for nothing. */
	اگر (sector_usable(0) >= snapshot_total + headroom) {
		بازگشت 1;
	}

	برای (صحیح s = 1; sector_map[s].address; s++) {
		اگر (sector_usable(s) >= headroom) {
			بازگشت 1;
		}
	}

	بازگشت 0;
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

صحیح parameter_flashfs_init(sector_descriptor_t *fconfig, uint8_t *buffer, uint16_t size)
{
	صحیح rv = 0;
	sector_map = fconfig;
	working_buffer_static = buffer != پوچ;

	اگر (!working_buffer_static) {
		size = 0;
	}

	working_buffer = buffer;
	working_buffer_size = size;

	/* Sanity check */

	/* A non-blank store with no valid entry is torn or foreign; import
	 * reports -EILSEQ. Erasing here would hide a successful retry after
	 * a torn compact write. */

	بازگشت rv;
}

#اگر defined(FLASH_UNIT_TEST)

ایستا sector_descriptor_t  test_sector_map[] = {
	{14, 128 * 1024, 0x081c0000},
	{15, 128 * 1024, 0x081e0000},
	{0, 0, 0},
};

__EXPORT تهی test(تهی);

uint8_t test_buf[32 * 1024];

__EXPORT تهی test(تهی)
{
	uint16_t largest_block = (32 * 1024) + 96;
	uint8_t *buffer = malloc(largest_block);

	parameter_flashfs_init(test_sector_map, buffer, largest_block);

	برای (صحیح t = 0; t < اندازه(test_buf); t++) {
		test_buf[t] = (uint8_t) t;
	}

	صحیح er = parameter_flashfs_erase();
	uint8_t *fbuffer;
	size_t buf_size;
	صحیح written = 0;
	صحیح اقرأ = 0;
	صحیح rv = 0;

	برای (صحیح a = 0; a <= 4; a++) {
		parameter_flashfs_erase();
		rv =  parameter_flashfs_alloc(parameters_token, &fbuffer, &buf_size);
		memcpy(fbuffer, test_buf, a);
		buf_size = a;
		written = parameter_flashfs_write(parameters_token, fbuffer, buf_size);
		اقرأ = parameter_flashfs_read(parameters_token, &fbuffer, &buf_size);
		parameter_flashfs_free();

		اگر (اقرأ != written) {
			ایستا فرار صحیح j;
			j++;
		}
	}

	صحیح block = 2048;

	برای (صحیح a = 0; a <= 8; a++) {
		parameter_flashfs_erase();
		rv =  parameter_flashfs_alloc(parameters_token, &fbuffer, &buf_size);
		memcpy(fbuffer, test_buf, block);
		buf_size = block;
		written = parameter_flashfs_write(parameters_token, fbuffer, buf_size);
		اقرأ = parameter_flashfs_read(parameters_token, &fbuffer, &buf_size);
		parameter_flashfs_free();

		اگر (اقرأ != written) {
			ایستا فرار صحیح j;
			j++;
		}

		block += 2048;
	}

	rv++;
	er++;
	آزاد(buffer);
}
#endif /* FLASH_UNIT_TEST */
#endif /* CONFIG_ARCH_HAVE_PROGMEM */
