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

#অন্তর্ভাব <px4_platform_common/defines.h>
#অন্তর্ভাব <px4_platform_common/posix.h>
#অন্তর্ভাব <nuttx/crc32.h>
#অন্তর্ভাব <stddef.h>
#অন্তর্ভাব <string.h>
#অন্তর্ভাব <stdbool.h>
#অন্তর্ভাব <stdint.h>
#অন্তর্ভাব <stdlib.h>
#অন্তর্ভাব <errno.h>
#অন্তর্ভাব "flashfs.h"
#অন্তর্ভাব <nuttx/compiler.h>
#অন্তর্ভাব <nuttx/progmem.h>
#অন্তর্ভাব <board_config.h>

#যদি defined(CONFIG_ARCH_HAVE_PROGMEM)

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Private Types
 ****************************************************************************/

প্রকারলেখ uint32_t h_magic_t;
প্রকারলেখ uint16_t h_আকার_t;
প্রকারলেখ uint16_t h_flag_t;
প্রকারলেখ uint16_t তথ্য_আকার_t;

প্রকারলেখ গণনা  flash_config_t {
	LargestBlock = 2 * 1024, // This represents the size need for backing store
	MagicSig     = 0xaa553cc3,
	BlankSig     = 0xffffffff
} flash_config_t;

/* Aligns are done up to 32 bytes, so storing the size padding within 0x001f mask */
প্রকারলেখ গণনা  flash_flags_t {
	আকার_Mask      = 0x001f,
	MaskEntry     = ~আকার_Mask,
	BlankEntry    = (h_flag_t)BlankSig,
	ValidEntry    = (0xa5ac & ~আকার_Mask),
	ErasedEntry   = 0x0000,
} flash_flags_t;

/* The struct flash_entry_header_t will be 32-byte aligned
 * The Size will be the actual length of the header plus the data
 * and any padding needed to have the size be an even multiple of
 * the 32-byte flash line
 */
#pragma GCC diagnostic ঠেলো
#pragma GCC diagnostic ignored "-Wattributes"
প্রকারলেখ begin_packed_struct কাঠামো flash_entry_header_t {
	h_magic_t            magic;           /* Used to ID files*/
	h_flag_t             flag;            /* Used to mark this entry as valid */
	uint8_t              unused_বাফার[32 - আকার(h_magic_t) - আকার(h_flag_t)]; /* padding */
	h_flag_t             flag_erased;     /* Used to erase this entry */
	uint8_t              unused_buffer2[32 - আকার(h_flag_t)]; /* padding */
	uint32_t             crc;             /* Calculated over the size - end of data */
	h_আকার_t             আকার_2;            /* When added to a byte pointer to flash_entry_header_t
                                               * Will result the offset of the next active file or
                                               * free space. */
	flash_file_token_t   file_token;      /* file token type - essentially the name/type */
} end_packed_struct flash_entry_header_t __attribute__((aligned(আকার(uint32_t))));
#pragma GCC diagnostic বের_করো

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/
স্থির uint8_t *working_বাফার;
স্থির size_t working_বাফার_আকার;
স্থির bool working_বাফার_static;
স্থির sector_descriptor_t *sector_map;

/****************************************************************************
 * Public Data
 ****************************************************************************/

ধ্রুবক flash_file_token_t parameters_token = {
	.n = {'p', 'l', 'o', 'g'},
};

ধ্রুবক flash_file_token_t parameters_legacy_token = {
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

শূন্য parameter_flashfs_free(শূন্য)
{
	যদি (!working_বাফার_static && working_বাফার != ফাঁকা) {
		মুক্ত(working_বাফার);
		working_বাফার = ফাঁকা;
		working_বাফার_আকার = 0;
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

স্থির অন্তঃরেখ পূর্ণসংখ্যা blank_flash(uint32_t *pf)
{
	ফেরত *pf == BlankSig;
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

স্থির bool blank_check(flash_entry_header_t *pf,
			size_t new_আকার)
{
	bool rv = true;
	uint32_t *pm = (uint32_t *) pf;
	new_আকার /= আকার(uint32_t);

	যখন (new_আকার-- && rv) {
		যদি (!blank_flash(pm++)) {
			rv = false;
		}
	}

	ফেরত rv;
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

স্থির অন্তঃরেখ পূর্ণসংখ্যা valid_magic(h_magic_t *pm)
{
	ফেরত *pm == MagicSig;
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

স্থির অন্তঃরেখ পূর্ণসংখ্যা blank_entry(flash_entry_header_t *fi)
{
	ফেরত fi->magic == BlankSig && fi->flag == BlankEntry && fi->flag_erased == BlankEntry;
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

স্থির অন্তঃরেখ পূর্ণসংখ্যা valid_entry(flash_entry_header_t *fi)
{
	ফেরত (fi->flag & MaskEntry) == ValidEntry && (fi->flag_erased) == BlankEntry;
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

স্থির অন্তঃরেখ পূর্ণসংখ্যা entry_আকার_adjust(flash_entry_header_t *fi)
{
	ফেরত fi->flag & আকার_Mask;
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

স্থির অন্তঃরেখ flash_entry_header_t *পরবর্তী_entry(flash_entry_header_t *fi)
{
	uint8_t *pb = (uint8_t *)fi;
	ফেরত (flash_entry_header_t *)(pb + fi->আকার_2);
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

স্থির অন্তঃরেখ uint8_t *entry_তথ্য(flash_entry_header_t *fi)
{
	ফেরত ((uint8_t *)fi) + আকার(flash_entry_header_t);
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

স্থির অন্তঃরেখ তথ্য_আকার_t entry_তথ্য_দৈর্ঘ্য(flash_entry_header_t *fi)
{
	ফেরত fi->আকার_2 - (আকার(flash_entry_header_t) + entry_আকার_adjust(fi));
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

স্থির অন্তঃরেখ ধ্রুবক uint8_t *entry_crc_start(flash_entry_header_t *fi)
{
	ফেরত (ধ্রুবক uint8_t *)&fi->আকার_2;
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

স্থির অন্তঃরেখ তথ্য_আকার_t entry_crc_দৈর্ঘ্য(flash_entry_header_t *fi)
{
	ফেরত fi->আকার_2 - offsetof(flash_entry_header_t, আকার_2);
}

/* The parameter sector can be the last one on the chip, so no header
 * field may be read until the whole header is known to lie inside it. */
স্থির অন্তঃরেখ bool header_fits(flash_entry_header_t *pf, h_magic_t *pe)
{
	ফেরত pf + 1 <= (flash_entry_header_t *)pe;
}

স্থির bool crc_header_ok(flash_entry_header_t *pf, h_magic_t *pe)
{
	যদি (!header_fits(pf, pe)) {
		ফেরত false;
	}

	যদি (pf->আকার_2 < আকার(flash_entry_header_t)) {
		ফেরত false;
	}

	ধ্রুবক uint8_t *crc_start = entry_crc_start(pf);
	তথ্য_আকার_t crc_দৈর্ঘ্য = entry_crc_দৈর্ঘ্য(pf);

	যদি (crc_start + crc_দৈর্ঘ্য > (uint8_t *)pe) {
		ফেরত false;
	}

	ফেরত pf->crc == crc32(crc_start, crc_দৈর্ঘ্য);
}

/* The walkers stop at the last word of a sector and reject a record that
 * ends beyond it, so nothing may be written into that word. */
স্থির অন্তঃরেখ size_t sector_usable(পূর্ণসংখ্যা s)
{
	ফেরত sector_map[s].আকার_2 - আকার(h_magic_t);
}

স্থির bool slot_is_free(flash_entry_header_t *pf, পূর্ণসংখ্যা s, size_t required)
{
	uint8_t *begin = (uint8_t *)sector_map[s].address;
	uint8_t *end = begin + sector_usable(s);

	যদি ((uint8_t *)pf < begin || required == 0 || (uint8_t *)pf + required > end) {
		ফেরত false;
	}

	/* Entries start on a flash row. up_progmem_write only asserts that in
	 * debug builds, so a slot found by the word-wise scan behind a torn
	 * record would otherwise be programmed misaligned. */
	যদি (((uintptr_t)pf & আকার_Mask) != 0) {
		ফেরত false;
	}

	ফেরত blank_entry(pf) && blank_check(pf, required);
}

স্থির পূর্ণসংখ্যা for_each_valid_entry(flash_file_token_t token,
				পূর্ণসংখ্যা (*cb)(flash_entry_header_t *pf, শূন্য *arg),
				শূন্য *arg);

স্থির পূর্ণসংখ্যা stop_at_first(flash_entry_header_t *pf, শূন্য *arg)
{
	*(flash_entry_header_t **)arg = pf;
	ফেরত -ECANCELED;
}

/* Oldest valid entry for token, or NULL. */
স্থির flash_entry_header_t *find_entry(flash_file_token_t token)
{
	flash_entry_header_t *pf = ফাঁকা;
	for_each_valid_entry(token, stop_at_first, &pf);
	ফেরত pf;
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

স্থির flash_entry_header_t *find_append(size_t required)
{
	যদি (sector_map == ফাঁকা || required == 0) {
		ফেরত ফাঁকা;
	}

	flash_entry_header_t *last = ফাঁকা;
	পূর্ণসংখ্যা last_s = -1;

	জন্য (পূর্ণসংখ্যা s = 0; sector_map[s].address; s++) {
		h_magic_t *pmagic = (h_magic_t *) sector_map[s].address;
		h_magic_t *pe = pmagic + (sector_map[s].আকার_2 / আকার(h_magic_t)) - 1;

		যখন (pmagic < pe) {
			যদি (!valid_magic(pmagic)) {
				pmagic++;
				চালু;
			}

			flash_entry_header_t *pf = (flash_entry_header_t *) pmagic;

			যদি (crc_header_ok(pf, pe)) {
				last = pf;
				last_s = s;
				pmagic = (h_magic_t *) পরবর্তী_entry(pf);
				pf = (flash_entry_header_t *) pmagic;

				/* Same stop rule as for_each_valid_entry: a record behind a
				 * blank header is invisible to replay, so it must not anchor
				 * the append point either. */
				যদি (pmagic >= pe || !header_fits(pf, pe) || blank_entry(pf)) {
					বিরতি;
				}

			} অন্যথা {
				pmagic++;
			}
		}
	}

	ধ্রুবক পূর্ণসংখ্যা start_s = (last == ফাঁকা) ? 0 : last_s;
	h_magic_t *pmagic = (last == ফাঁকা)
			    ? (h_magic_t *) sector_map[0].address
			    : (h_magic_t *) পরবর্তী_entry(last);

	জন্য (পূর্ণসংখ্যা s = start_s; sector_map[s].address; s++) {
		যদি (s != start_s) {
			pmagic = (h_magic_t *) sector_map[s].address;
		}

		h_magic_t *pe = (h_magic_t *) sector_map[s].address
				+ (sector_map[s].আকার_2 / আকার(h_magic_t)) - 1;

		যদি ((uint8_t *)pmagic < (uint8_t *)sector_map[s].address
		    || (uint8_t *)pmagic >= (uint8_t *)sector_map[s].address + sector_map[s].আকার_2) {
			যদি (s == start_s) {
				চালু;
			}

			pmagic = (h_magic_t *) sector_map[s].address;
		}

		যখন (pmagic < pe) {
			flash_entry_header_t *pf = (flash_entry_header_t *) pmagic;

			যদি (valid_magic(pmagic) && crc_header_ok(pf, pe)) {
				pmagic = (h_magic_t *) পরবর্তী_entry(pf);
				চালু;
			}

			যদি (slot_is_free(pf, s, required)) {
				ফেরত pf;
			}

			pmagic++;
		}
	}

	ফেরত ফাঁকা;
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

স্থির পূর্ণসংখ্যা for_each_valid_entry(flash_file_token_t token,
				পূর্ণসংখ্যা (*cb)(flash_entry_header_t *pf, শূন্য *arg),
				শূন্য *arg)
{
	পূর্ণসংখ্যা n = 0;

	যদি (sector_map == ফাঁকা) {
		ফেরত -ENXIO;
	}

	জন্য (পূর্ণসংখ্যা s = 0; sector_map[s].address; s++) {

		h_magic_t *pmagic = (h_magic_t *) sector_map[s].address;
		h_magic_t *pe = pmagic + (sector_map[s].আকার_2 / আকার(h_magic_t)) - 1;

cont:

		যখন (pmagic < pe && !valid_magic(pmagic)) {
			pmagic++;
		}

		যদি (pmagic >= pe) { চালু; }

		flash_entry_header_t *pf = (flash_entry_header_t *) pmagic;

		যদি (crc_header_ok(pf, pe)) {

			যদি (valid_entry(pf) && pf->file_token.t == token.t) {
				যদি (cb != ফাঁকা) {
					পূর্ণসংখ্যা rv = cb(pf, arg);

					যদি (rv < 0) {
						ফেরত rv;
					}
				}

				n++;
			}

			pf = পরবর্তী_entry(pf);
			pmagic = (h_magic_t *) pf;

			যদি (pmagic >= pe || !header_fits(pf, pe) || blank_entry(pf)) {
				চালু;
			}

		} অন্যথা {
			pmagic++;
		}

		যাও cont;
	}

	ফেরত n;
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

স্থির পূর্ণসংখ্যা erase_sector(sector_descriptor_t *sm, flash_entry_header_t *pf)
{
	পূর্ণসংখ্যা rv = 0;

	/*
	* Because up_progmem interface doesn't allow one-to-one check of
	* the block number for an address, use uniform-memory assumption
	* to confirm the block number in the sector descriptor is
	* the correct block to erase for the given address.
	*
	* This is an optional sanity check against the sector configuration
	*/
	যদি (up_progmem_isuniform()) {
		ssize_t page = up_progmem_getpage((size_t)pf);

		যদি (page < 0) {
			rv = page;
		}

		যদি (rv == 0) {
			size_t page_আকার  = up_progmem_pagesize(page);
			size_t block_আকার = up_progmem_erasesize(sm->page);
			size_t block_অঙ্ক  = ((page * page_আকার) / block_আকার);

			যদি (block_অঙ্ক != sm->page) {
				rv = -EINVAL;
			}
		}
	}

	যদি (rv == 0) {

		ssize_t আকার_erased = up_progmem_eraseblock(sm->page);

		যদি (আকার_erased < 0 || আকার_erased != (ssize_t)sm->আকার_2) {
			rv = আকার_erased;
		}

	}

	ফেরত rv;
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

স্থির পূর্ণসংখ্যা write_flash_entry(size_t pf, ধ্রুবক flash_entry_header_t *pn)
{
	ssize_t rv;
	size_t সারি_আকার = up_progmem_pagesize(up_progmem_getpage(pf));
	uint8_t *first_flag_সারি  = (uint8_t *)pn;
	uint8_t *content_বাফার   = (uint8_t *)(&pn->crc);

	/* write first line (magic, flag) */
	rv = up_progmem_write(pf, first_flag_সারি, সারি_আকার);

	যদি (rv < 0) {
		ফেরত rv;
	}

	/* pretend to write the second line (flag_erased) */
	rv += সারি_আকার;

	/* write the remaining contents */
	ssize_t rv2 = up_progmem_write(pf + 2 * সারি_আকার, content_বাফার, pn->আকার_2 - 2 * সারি_আকার);

	যদি (rv2 < 0) {
		ফেরত rv2;
	}

	ফেরত rv + rv2;
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

পূর্ণসংখ্যা parameter_flashfs_read(flash_file_token_t token, uint8_t **বাফার_2, size_t
			   *buf_আকার)
{
	পূর্ণসংখ্যা rv = -ENXIO;

	যদি (sector_map) {

		rv = -ENOENT;
		flash_entry_header_t *pf = find_entry(token);

		যদি (pf) {
			(*বাফার_2) = entry_তথ্য(pf);
			rv = entry_তথ্য_দৈর্ঘ্য(pf);
			*buf_আকার = rv;
		}
	}

	ফেরত rv;
}

/****************************************************************************
 * Name: parameter_flashfs_blank
 *
 * Description:
 *   Reports whether the parameter storage is fully erased, so callers can
 *   tell a blank store from one that holds unparseable data.
 *
 ****************************************************************************/

পূর্ণসংখ্যা parameter_flashfs_blank(শূন্য)
{
	যদি (sector_map == ফাঁকা) {
		ফেরত -ENXIO;
	}

	জন্য (পূর্ণসংখ্যা s = 0; sector_map[s].address; s++) {
		uint32_t *pm = (uint32_t *) sector_map[s].address;
		uint32_t *pe = pm + (sector_map[s].আকার_2 / আকার(uint32_t));

		যখন (pm < pe) {
			যদি (!blank_flash(pm++)) {
				ফেরত 0;
			}
		}
	}

	ফেরত 1;
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

পূর্ণসংখ্যা
parameter_flashfs_write(flash_file_token_t token, uint8_t *বাফার_2, size_t buf_আকার)
{
	পূর্ণসংখ্যা rv = -ENXIO;

	যদি (sector_map) {

		rv = 0;

		/* Calculate the total space needed */
		size_t যোগ_আকার = buf_আকার + আকার(flash_entry_header_t);
		size_t alignment = আকার_Mask;///< 32-byte flash row
		size_t  আকার_adjust = ((যোগ_আকার + alignment) & ~alignment) - যোগ_আকার;
		যোগ_আকার += আকার_adjust;

		যদি (যোগ_আকার > UINT16_MAX) {
			ফেরত -EFBIG;
		}

		/* Append after the last CRC-valid record. Compaction is the caller's job. */
		flash_entry_header_t *pf = find_append(যোগ_আকার);

		যদি (pf == 0) {
			ফেরত -ENOSPC;
		}

		flash_entry_header_t *pn = (flash_entry_header_t *)(বাফার_2 - আকার(flash_entry_header_t));
		pn->magic = MagicSig;
		pn->file_token.t = token.t;
		pn->flag = ValidEntry + আকার_adjust;
		pn->আকার_2 = যোগ_আকার;

		জন্য (size_t a = 0; a < আকার_adjust; a++) {
			বাফার_2[buf_আকার + a] = (uint8_t)BlankSig;
		}

		pn->crc = crc32(entry_crc_start(pn), entry_crc_দৈর্ঘ্য(pn));
		rv = write_flash_entry((size_t)pf, pn);
		পূর্ণসংখ্যা system_bytes = (আকার(flash_entry_header_t) + আকার_adjust);

		যদি (rv >= system_bytes) {
			rv -= system_bytes;
		}
	}

	ফেরত rv;
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

পূর্ণসংখ্যা parameter_flashfs_alloc(flash_file_token_t token, uint8_t **বাফার_2, size_t *buf_আকার)
{
	পূর্ণসংখ্যা rv = -ENXIO;

	যদি (sector_map) {

		rv = -ENOMEM;

		যদি (!working_বাফার_static) {

			working_বাফার_আকার = *buf_আকার + আকার(flash_entry_header_t) + আকার_Mask;
			working_বাফার = malloc(working_বাফার_আকার);

		}

		/* Allocation failed or not provided */

		যদি (working_বাফার == ফাঁকা) {

			working_বাফার_আকার = 0;

		} অন্যথা {

			/* We have a buffer reserve space and init it */
			*বাফার_2 = &working_বাফার[আকার(flash_entry_header_t)];
			*buf_আকার = working_বাফার_আকার - আকার(flash_entry_header_t) - আকার_Mask;
			memset(working_বাফার, 0xff, working_বাফার_আকার);
			rv = 0;

		}
	}

	ফেরত rv;
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

পূর্ণসংখ্যা parameter_flashfs_erase(শূন্য)
{
	পূর্ণসংখ্যা rv = -ENXIO;

	যদি (sector_map) {
		rv = 0;

		জন্য (পূর্ণসংখ্যা s = 0; sector_map[s].address; s++) {
			পূর্ণসংখ্যা sz = erase_sector(&sector_map[s], (flash_entry_header_t *)sector_map[s].address);

			যদি (sz != 0) {
				ফেরত sz;
			}

			rv += sector_map[s].আকার_2;
		}
	}

	ফেরত rv;
}


/****************************************************************************
 * Name: parameter_flashfs_walk
 ****************************************************************************/

কাঠামো walk_pack {
	parameter_flashfs_walk_cb cb;
	শূন্য *arg;
};

স্থির পূর্ণসংখ্যা walk_one(flash_entry_header_t *pf, শূন্য *arg)
{
	কাঠামো walk_pack *w = (কাঠামো walk_pack *)arg;
	ফেরত w->cb(entry_তথ্য(pf), entry_তথ্য_দৈর্ঘ্য(pf), w->arg);
}

পূর্ণসংখ্যা parameter_flashfs_walk(flash_file_token_t token, parameter_flashfs_walk_cb cb, শূন্য *arg)
{
	কাঠামো walk_pack w = { cb, arg };
	ফেরত for_each_valid_entry(token, walk_one, &w);
}

/****************************************************************************
 * Name: parameter_flashfs_needs_compact
 ****************************************************************************/

size_t parameter_flashfs_max_payload(শূন্য)
{
	যদি (sector_map == ফাঁকা) {
		ফেরত 0;
	}

	size_t max_যোগ = 0;

	জন্য (পূর্ণসংখ্যা s = 0; sector_map[s].address; s++) {
		যদি (sector_usable(s) > max_যোগ) {
			max_যোগ = sector_usable(s);
		}
	}

	/* h_size_t holds the record size */
	যদি (max_যোগ > UINT16_MAX) {
		max_যোগ = UINT16_MAX;
	}

	max_যোগ &= ~(size_t)আকার_Mask;

	যদি (max_যোগ <= আকার(flash_entry_header_t)) {
		ফেরত 0;
	}

	ফেরত max_যোগ - আকার(flash_entry_header_t);
}

পূর্ণসংখ্যা parameter_flashfs_needs_compact(flash_file_token_t token, size_t snapshot_আকার)
{
	যদি (sector_map == ফাঁকা) {
		ফেরত -ENXIO;
	}

	flash_entry_header_t *first = find_entry(token);

	যদি (first == ফাঁকা) {
		ফেরত 0;
	}

	/* Dead records ahead of the live data (a torn write, or the erased
	 * prefix the single-snapshot writer left) only go away with an erase. */
	যদি ((uintptr_t)first != (uintptr_t)sector_map[0].address) {
		ফেরত 1;
	}

	/* Keep room after the log for a burst the size of the snapshot
	 * (reset-all tombstones, airframe PARAM_SET), floored so a UUID append
	 * does not force a boot erase. */
	ধ্রুবক size_t align = আকার_Mask;
	ধ্রুবক size_t snapshot_যোগ = (snapshot_আকার + আকার(flash_entry_header_t) + align) & ~align;
	ধ্রুবক size_t headroom = snapshot_যোগ > 8192 ? snapshot_যোগ : 8192;

	যদি (find_append(headroom) != ফাঁকা) {
		ফেরত 0;
	}

	/* Compaction leaves the snapshot at the start of sector 0 and every
	 * other sector blank. Erase only if that layout has the room this one
	 * lacks, or every boot would pay the erase for nothing. */
	যদি (sector_usable(0) >= snapshot_যোগ + headroom) {
		ফেরত 1;
	}

	জন্য (পূর্ণসংখ্যা s = 1; sector_map[s].address; s++) {
		যদি (sector_usable(s) >= headroom) {
			ফেরত 1;
		}
	}

	ফেরত 0;
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

পূর্ণসংখ্যা parameter_flashfs_init(sector_descriptor_t *fconfig, uint8_t *বাফার_2, uint16_t আকার_2)
{
	পূর্ণসংখ্যা rv = 0;
	sector_map = fconfig;
	working_বাফার_static = বাফার_2 != ফাঁকা;

	যদি (!working_বাফার_static) {
		আকার_2 = 0;
	}

	working_বাফার = বাফার_2;
	working_বাফার_আকার = আকার_2;

	/* Sanity check */

	/* A non-blank store with no valid entry is torn or foreign; import
	 * reports -EILSEQ. Erasing here would hide a successful retry after
	 * a torn compact write. */

	ফেরত rv;
}

#যদি defined(FLASH_UNIT_TEST)

স্থির sector_descriptor_t  test_sector_map[] = {
	{14, 128 * 1024, 0x081c0000},
	{15, 128 * 1024, 0x081e0000},
	{0, 0, 0},
};

__EXPORT শূন্য test(শূন্য);

uint8_t test_buf[32 * 1024];

__EXPORT শূন্য test(শূন্য)
{
	uint16_t largest_block = (32 * 1024) + 96;
	uint8_t *বাফার_2 = malloc(largest_block);

	parameter_flashfs_init(test_sector_map, বাফার_2, largest_block);

	জন্য (পূর্ণসংখ্যা t = 0; t < আকার(test_buf); t++) {
		test_buf[t] = (uint8_t) t;
	}

	পূর্ণসংখ্যা er = parameter_flashfs_erase();
	uint8_t *fbuffer;
	size_t buf_আকার;
	পূর্ণসংখ্যা written = 0;
	পূর্ণসংখ্যা পঠ = 0;
	পূর্ণসংখ্যা rv = 0;

	জন্য (পূর্ণসংখ্যা a = 0; a <= 4; a++) {
		parameter_flashfs_erase();
		rv =  parameter_flashfs_alloc(parameters_token, &fbuffer, &buf_আকার);
		memcpy(fbuffer, test_buf, a);
		buf_আকার = a;
		written = parameter_flashfs_write(parameters_token, fbuffer, buf_আকার);
		পঠ = parameter_flashfs_read(parameters_token, &fbuffer, &buf_আকার);
		parameter_flashfs_free();

		যদি (পঠ != written) {
			স্থির অস্থির পূর্ণসংখ্যা j;
			j++;
		}
	}

	পূর্ণসংখ্যা block = 2048;

	জন্য (পূর্ণসংখ্যা a = 0; a <= 8; a++) {
		parameter_flashfs_erase();
		rv =  parameter_flashfs_alloc(parameters_token, &fbuffer, &buf_আকার);
		memcpy(fbuffer, test_buf, block);
		buf_আকার = block;
		written = parameter_flashfs_write(parameters_token, fbuffer, buf_আকার);
		পঠ = parameter_flashfs_read(parameters_token, &fbuffer, &buf_আকার);
		parameter_flashfs_free();

		যদি (পঠ != written) {
			স্থির অস্থির পূর্ণসংখ্যা j;
			j++;
		}

		block += 2048;
	}

	rv++;
	er++;
	মুক্ত(বাফার_2);
}
#endif /* FLASH_UNIT_TEST */
#endif /* CONFIG_ARCH_HAVE_PROGMEM */
