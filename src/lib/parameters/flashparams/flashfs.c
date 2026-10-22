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

#অন্তর্ভাৱ <px4_platform_common/defines.h>
#অন্তর্ভাৱ <px4_platform_common/posix.h>
#অন্তর্ভাৱ <nuttx/crc32.h>
#অন্তর্ভাৱ <stddef.h>
#অন্তর্ভাৱ <string.h>
#অন্তর্ভাৱ <stdbool.h>
#অন্তর্ভাৱ <stdint.h>
#অন্তর্ভাৱ <stdlib.h>
#অন্তর্ভাৱ <errno.h>
#অন্তর্ভাৱ "flashfs.h"
#অন্তর্ভাৱ <nuttx/compiler.h>
#অন্তর্ভাৱ <nuttx/progmem.h>
#অন্তর্ভাৱ <board_config.h>

#যদি defined(CONFIG_ARCH_HAVE_PROGMEM) || defined(BOARD_USE_EXTERNAL_FLASH)

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Private Types
 ****************************************************************************/

প্রকারনির্দেশ uint32_t h_magic_t;
প্রকারনির্দেশ uint16_t h_size_t;
প্রকারনির্দেশ uint16_t h_flag_t;
প্রকারনির্দেশ uint16_t data_size_t;

প্রকারনির্দেশ গণনা  flash_config_t {
	LargestBlock = 2 * 1024, // This represents the size need for backing store
	MagicSig     = 0xaa553cc3,
	BlankSig     = 0xffffffff
} flash_config_t;

প্রকারনির্দেশ গণনা  flash_flags_t {
	SizeMask      = 0x0003,
	MaskEntry     = ~SizeMask,
	BlankEntry    = (h_flag_t)BlankSig,
	ValidEntry    = (0xa5ac & ~SizeMask),
	ErasedEntry   = 0x0000,
} flash_flags_t;


/* The struct flash_entry_header_t will be sizeof(uint32_t) aligned
 * The Size will be the actual length of the header plus the data
 * and any padding needed to have the size be an even multiple of
 * sizeof(uint32_t)
 */
#pragma GCC diagnostic ঠেলো
#pragma GCC diagnostic ignored "-Wattributes"
প্রকারনির্দেশ begin_packed_struct সংরচনা flash_entry_header_t {
	h_magic_t            magic;           /* Used to ID files*/
	h_flag_t             flag;            /* Used to erase this entry */
	uint32_t             crc;             /* Calculated over the size - end of data */
	h_size_t             size;            /* When added to a byte pointer to flash_entry_header_t
                                               * Will result the offset of the next active file or
                                               * free space. */
	flash_file_token_t   file_token;      /* file token type - essentially the name/type */
} end_packed_struct flash_entry_header_t __attribute__((aligned(পরিমাণ(uint32_t))));
#pragma GCC diagnostic বেৰ_কৰো

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/
স্থির uint8_t *working_buffer;
স্থির size_t working_buffer_size;
স্থির bool working_buffer_static;
স্থির sector_descriptor_t *sector_map;
স্থির পূর্ণাঙ্ক last_erased;

/****************************************************************************
 * Public Data
 ****************************************************************************/

ধ্রুৱ flash_file_token_t parameters_token = {
	.n = {'p', 'l', 'o', 'g'},
};

ধ্রুৱ flash_file_token_t parameters_legacy_token = {
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
	যদি (!working_buffer_static && working_buffer != রিক্ত) {
		মুক্ত(working_buffer);
		working_buffer = রিক্ত;
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

স্থির অন্তঃস্থ পূর্ণাঙ্ক blank_flash(uint32_t *pf)
{
	ঘূৰাই *pf == BlankSig;
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
			size_t new_size)
{
	bool rv = true;
	uint32_t *pm = (uint32_t *) pf;
	new_size /= পরিমাণ(uint32_t);

	যেতিয়ালৈ (new_size-- && rv) {
		যদি (!blank_flash(pm++)) {
			rv = false;
		}
	}

	ঘূৰাই rv;
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

স্থির অন্তঃস্থ পূর্ণাঙ্ক valid_magic(h_magic_t *pm)
{
	ঘূৰাই *pm == MagicSig;
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

স্থির অন্তঃস্থ পূর্ণাঙ্ক blank_entry(flash_entry_header_t *fi)
{
	ঘূৰাই fi->magic == BlankSig  &&  fi->flag == BlankEntry;
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

স্থির অন্তঃস্থ পূর্ণাঙ্ক valid_entry(flash_entry_header_t *fi)
{
	ঘূৰাই (fi->flag & MaskEntry) == ValidEntry;
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

স্থির অন্তঃস্থ পূর্ণাঙ্ক entry_size_adjust(flash_entry_header_t *fi)
{
	ঘূৰাই fi->flag & SizeMask;
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

স্থির অন্তঃস্থ flash_entry_header_t *next_entry(flash_entry_header_t *fi)
{
	uint8_t *pb = (uint8_t *)fi;
	ঘূৰাই (flash_entry_header_t *)(pb + fi->size);
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

স্থির অন্তঃস্থ uint8_t *entry_data(flash_entry_header_t *fi)
{
	ঘূৰাই ((uint8_t *)fi) + পরিমাণ(flash_entry_header_t);
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

স্থির অন্তঃস্থ data_size_t entry_data_length(flash_entry_header_t *fi)
{
	ঘূৰাই fi->size - (পরিমাণ(flash_entry_header_t) + entry_size_adjust(fi));
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

স্থির অন্তঃস্থ ধ্রুৱ uint8_t *entry_crc_start(flash_entry_header_t *fi)
{
	ঘূৰাই (ধ্রুৱ uint8_t *)&fi->size;
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

স্থির অন্তঃস্থ data_size_t entry_crc_length(flash_entry_header_t *fi)
{
	ঘূৰাই fi->size - offsetof(flash_entry_header_t, size);
}

/* The parameter sector can be the last one on the chip, so no header
 * field may be read until the whole header is known to lie inside it. */
স্থির অন্তঃস্থ bool header_fits(flash_entry_header_t *pf, h_magic_t *pe)
{
	ঘূৰাই pf + 1 <= (flash_entry_header_t *)pe;
}

স্থির bool crc_header_ok(flash_entry_header_t *pf, h_magic_t *pe)
{
	যদি (!header_fits(pf, pe)) {
		ঘূৰাই false;
	}

	যদি (pf->size < পরিমাণ(flash_entry_header_t)) {
		ঘূৰাই false;
	}

	ধ্রুৱ uint8_t *crc_start = entry_crc_start(pf);
	data_size_t crc_length = entry_crc_length(pf);

	যদি (crc_start + crc_length > (uint8_t *)pe) {
		ঘূৰাই false;
	}

	ঘূৰাই pf->crc == crc32(crc_start, crc_length);
}

/* The walkers stop at the last word of a sector and reject a record that
 * ends beyond it, so nothing may be written into that word. */
স্থির অন্তঃস্থ size_t sector_usable(পূর্ণাঙ্ক s)
{
	ঘূৰাই sector_map[s].size - পরিমাণ(h_magic_t);
}

স্থির bool slot_is_free(flash_entry_header_t *pf, পূর্ণাঙ্ক s, size_t required)
{
	uint8_t *begin = (uint8_t *)sector_map[s].address;
	uint8_t *end = begin + sector_usable(s);

	যদি ((uint8_t *)pf < begin || required == 0 || (uint8_t *)pf + required > end) {
		ঘূৰাই false;
	}

	/* Entries start on a flash row. up_progmem_write only asserts that in
	 * debug builds, so a slot found by the word-wise scan behind a torn
	 * record would otherwise be programmed misaligned. */
	যদি (((uintptr_t)pf & SizeMask) != 0) {
		ঘূৰাই false;
	}

	ঘূৰাই blank_entry(pf) && blank_check(pf, required);
}

স্থির পূর্ণাঙ্ক for_each_valid_entry(flash_file_token_t token,
				পূর্ণাঙ্ক (*cb)(flash_entry_header_t *pf, শূন্য *arg),
				শূন্য *arg);

স্থির পূর্ণাঙ্ক stop_at_first(flash_entry_header_t *pf, শূন্য *arg)
{
	*(flash_entry_header_t **)arg = pf;
	ঘূৰাই -ECANCELED;
}

/* Oldest valid entry for token, or NULL. */
স্থির flash_entry_header_t *find_entry(flash_file_token_t token)
{
	flash_entry_header_t *pf = রিক্ত;
	for_each_valid_entry(token, stop_at_first, &pf);
	ঘূৰাই pf;
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
	যদি (sector_map == রিক্ত || required == 0) {
		ঘূৰাই রিক্ত;
	}

	flash_entry_header_t *last = রিক্ত;
	পূর্ণাঙ্ক last_s = -1;

	বাবে (পূর্ণাঙ্ক s = 0; sector_map[s].address; s++) {
		h_magic_t *pmagic = (h_magic_t *) sector_map[s].address;
		h_magic_t *pe = pmagic + (sector_map[s].size / পরিমাণ(h_magic_t)) - 1;

		যেতিয়ালৈ (pmagic < pe) {
			যদি (!valid_magic(pmagic)) {
				pmagic++;
				অনুৱর্ত;
			}

			flash_entry_header_t *pf = (flash_entry_header_t *) pmagic;

			যদি (crc_header_ok(pf, pe)) {
				last = pf;
				last_s = s;
				pmagic = (h_magic_t *) next_entry(pf);
				pf = (flash_entry_header_t *) pmagic;

				/* Same stop rule as for_each_valid_entry: a record behind a
				 * blank header is invisible to replay, so it must not anchor
				 * the append point either. */
				যদি (pmagic >= pe || !header_fits(pf, pe) || blank_entry(pf)) {
					ভঙ্গ;
				}

			} অন্যথা {
				pmagic++;
			}
		}
	}

	ধ্রুৱ পূর্ণাঙ্ক start_s = (last == রিক্ত) ? 0 : last_s;
	h_magic_t *pmagic = (last == রিক্ত)
			    ? (h_magic_t *) sector_map[0].address
			    : (h_magic_t *) next_entry(last);

	বাবে (পূর্ণাঙ্ক s = start_s; sector_map[s].address; s++) {
		যদি (s != start_s) {
			pmagic = (h_magic_t *) sector_map[s].address;
		}

		h_magic_t *pe = (h_magic_t *) sector_map[s].address
				+ (sector_map[s].size / পরিমাণ(h_magic_t)) - 1;

		যদি ((uint8_t *)pmagic < (uint8_t *)sector_map[s].address
		    || (uint8_t *)pmagic >= (uint8_t *)sector_map[s].address + sector_map[s].size) {
			যদি (s == start_s) {
				অনুৱর্ত;
			}

			pmagic = (h_magic_t *) sector_map[s].address;
		}

		যেতিয়ালৈ (pmagic < pe) {
			flash_entry_header_t *pf = (flash_entry_header_t *) pmagic;

			যদি (valid_magic(pmagic) && crc_header_ok(pf, pe)) {
				pmagic = (h_magic_t *) next_entry(pf);
				অনুৱর্ত;
			}

			যদি (slot_is_free(pf, s, required)) {
				ঘূৰাই pf;
			}

			pmagic++;
		}
	}

	ঘূৰাই রিক্ত;
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

স্থির পূর্ণাঙ্ক for_each_valid_entry(flash_file_token_t token,
				পূর্ণাঙ্ক (*cb)(flash_entry_header_t *pf, শূন্য *arg),
				শূন্য *arg)
{
	পূর্ণাঙ্ক n = 0;

	যদি (sector_map == রিক্ত) {
		ঘূৰাই -ENXIO;
	}

	বাবে (পূর্ণাঙ্ক s = 0; sector_map[s].address; s++) {

		h_magic_t *pmagic = (h_magic_t *) sector_map[s].address;
		h_magic_t *pe = pmagic + (sector_map[s].size / পরিমাণ(h_magic_t)) - 1;

cont:

		যেতিয়ালৈ (pmagic < pe && !valid_magic(pmagic)) {
			pmagic++;
		}

		যদি (pmagic >= pe) { অনুৱর্ত; }

		flash_entry_header_t *pf = (flash_entry_header_t *) pmagic;

		যদি (crc_header_ok(pf, pe)) {

			যদি (valid_entry(pf) && pf->file_token.t == token.t) {
				যদি (cb != রিক্ত) {
					পূর্ণাঙ্ক rv = cb(pf, arg);

					যদি (rv < 0) {
						ঘূৰাই rv;
					}
				}

				n++;
			}

			pf = next_entry(pf);
			pmagic = (h_magic_t *) pf;

			যদি (pmagic >= pe || !header_fits(pf, pe) || blank_entry(pf)) {
				অনুৱর্ত;
			}

		} অন্যথা {
			pmagic++;
		}

		গচ্ছ cont;
	}

	ঘূৰাই n;
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

স্থির পূর্ণাঙ্ক erase_sector(sector_descriptor_t *sm, flash_entry_header_t *pf)
{
	পূর্ণাঙ্ক rv = 0;
#যদি defined(BOARD_USE_EXTERNAL_FLASH)
	ssize_t block = up_progmem_ext_getpage((size_t)pf);
#অন্যথা
	ssize_t block = up_progmem_getpage((size_t)pf);
#endif

	যদি (block > 0 && block == sm->page) {
		last_erased = sm->page;
#যদি defined(BOARD_USE_EXTERNAL_FLASH)
		ssize_t size = up_progmem_ext_eraseblock(block);
#অন্যথা
		ssize_t size = up_progmem_eraseblock(block);
#endif

		যদি (size < 0 || size != (ssize_t)sm->size) {
			rv = size;
		}
	}

	ঘূৰাই rv;
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

পূর্ণাঙ্ক parameter_flashfs_read(flash_file_token_t token, uint8_t **buffer, size_t
			   *buf_size)
{
	পূর্ণাঙ্ক rv = -ENXIO;

	যদি (sector_map) {

		rv = -ENOENT;
		flash_entry_header_t *pf = find_entry(token);

		যদি (pf) {
			(*buffer) = entry_data(pf);
			rv = entry_data_length(pf);
			*buf_size = rv;
		}
	}

	ঘূৰাই rv;
}

/****************************************************************************
 * Name: parameter_flashfs_blank
 *
 * Description:
 *   Reports whether the parameter storage is fully erased, so callers can
 *   tell a blank store from one that holds unparseable data.
 *
 ****************************************************************************/

পূর্ণাঙ্ক parameter_flashfs_blank(শূন্য)
{
	যদি (sector_map == রিক্ত) {
		ঘূৰাই -ENXIO;
	}

	বাবে (পূর্ণাঙ্ক s = 0; sector_map[s].address; s++) {
		uint32_t *pm = (uint32_t *) sector_map[s].address;
		uint32_t *pe = pm + (sector_map[s].size / পরিমাণ(uint32_t));

		যেতিয়ালৈ (pm < pe) {
			যদি (!blank_flash(pm++)) {
				ঘূৰাই 0;
			}
		}
	}

	ঘূৰাই 1;
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

পূর্ণাঙ্ক
parameter_flashfs_write(flash_file_token_t token, uint8_t *buffer, size_t buf_size)
{
	পূর্ণাঙ্ক rv = -ENXIO;

	যদি (sector_map) {

		rv = 0;

		/* Calculate the total space needed */

		size_t total_size = buf_size + পরিমাণ(flash_entry_header_t);
		size_t alignment = পরিমাণ(h_magic_t) - 1;
		size_t  size_adjust = ((total_size + alignment) & ~alignment) - total_size;
		total_size += size_adjust;

		যদি (total_size > UINT16_MAX) {
			ঘূৰাই -EFBIG;
		}

		/* Append after the last CRC-valid record. Compaction is the caller's job. */
		flash_entry_header_t *pf = find_append(total_size);

		যদি (pf == 0) {
			ঘূৰাই -ENOSPC;
		}

		flash_entry_header_t *pn = (flash_entry_header_t *)(buffer - পরিমাণ(flash_entry_header_t));
		pn->magic = MagicSig;
		pn->file_token.t = token.t;
		pn->flag = ValidEntry + size_adjust;
		pn->size = total_size;

		বাবে (size_t a = 0; a < size_adjust; a++) {
			buffer[buf_size + a] = (uint8_t)BlankSig;
		}

		pn->crc = crc32(entry_crc_start(pn), entry_crc_length(pn));
#যদি defined(BOARD_USE_EXTERNAL_FLASH)
		rv = up_progmem_ext_write((size_t) pf, pn, pn->size);
#অন্যথা
		rv = up_progmem_write((size_t) pf, pn, pn->size);
#endif
		পূর্ণাঙ্ক system_bytes = (পরিমাণ(flash_entry_header_t) + size_adjust);

		যদি (rv >= system_bytes) {
			rv -= system_bytes;
		}
	}

	ঘূৰাই rv;
}

/****************************************************************************
 * Name: parameter_flashfs_walk
 ****************************************************************************/

সংরচনা walk_pack {
	parameter_flashfs_walk_cb cb;
	শূন্য *arg;
};

স্থির পূর্ণাঙ্ক walk_one(flash_entry_header_t *pf, শূন্য *arg)
{
	সংরচনা walk_pack *w = (সংরচনা walk_pack *)arg;
	ঘূৰাই w->cb(entry_data(pf), entry_data_length(pf), w->arg);
}

পূর্ণাঙ্ক parameter_flashfs_walk(flash_file_token_t token, parameter_flashfs_walk_cb cb, শূন্য *arg)
{
	সংরচনা walk_pack w = { cb, arg };
	ঘূৰাই for_each_valid_entry(token, walk_one, &w);
}

/****************************************************************************
 * Name: parameter_flashfs_needs_compact
 ****************************************************************************/

size_t parameter_flashfs_max_payload(শূন্য)
{
	যদি (sector_map == রিক্ত) {
		ঘূৰাই 0;
	}

	size_t max_total = 0;

	বাবে (পূর্ণাঙ্ক s = 0; sector_map[s].address; s++) {
		যদি (sector_usable(s) > max_total) {
			max_total = sector_usable(s);
		}
	}

	/* h_size_t holds the record size */
	যদি (max_total > UINT16_MAX) {
		max_total = UINT16_MAX;
	}

	max_total &= ~(size_t)SizeMask;

	যদি (max_total <= পরিমাণ(flash_entry_header_t)) {
		ঘূৰাই 0;
	}

	ঘূৰাই max_total - পরিমাণ(flash_entry_header_t);
}

পূর্ণাঙ্ক parameter_flashfs_needs_compact(flash_file_token_t token, size_t snapshot_size)
{
	যদি (sector_map == রিক্ত) {
		ঘূৰাই -ENXIO;
	}

	flash_entry_header_t *first = find_entry(token);

	যদি (first == রিক্ত) {
		ঘূৰাই 0;
	}

	/* Dead records ahead of the live data (a torn write, or the erased
	 * prefix the single-snapshot writer left) only go away with an erase. */
	যদি ((uintptr_t)first != (uintptr_t)sector_map[0].address) {
		ঘূৰাই 1;
	}

	/* Keep room after the log for a burst the size of the snapshot
	 * (reset-all tombstones, airframe PARAM_SET), floored so a UUID append
	 * does not force a boot erase. */
	ধ্রুৱ size_t align = পরিমাণ(h_magic_t) - 1;
	ধ্রুৱ size_t snapshot_total = (snapshot_size + পরিমাণ(flash_entry_header_t) + align) & ~align;
	ধ্রুৱ size_t headroom = snapshot_total > 8192 ? snapshot_total : 8192;

	যদি (find_append(headroom) != রিক্ত) {
		ঘূৰাই 0;
	}

	/* Compaction leaves the snapshot at the start of sector 0 and every
	 * other sector blank. Erase only if that layout has the room this one
	 * lacks, or every boot would pay the erase for nothing. */
	যদি (sector_usable(0) >= snapshot_total + headroom) {
		ঘূৰাই 1;
	}

	বাবে (পূর্ণাঙ্ক s = 1; sector_map[s].address; s++) {
		যদি (sector_usable(s) >= headroom) {
			ঘূৰাই 1;
		}
	}

	ঘূৰাই 0;
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

পূর্ণাঙ্ক parameter_flashfs_alloc(flash_file_token_t token, uint8_t **buffer, size_t *buf_size)
{
	পূর্ণাঙ্ক rv = -ENXIO;

	যদি (sector_map) {

		rv = -ENOMEM;

		যদি (!working_buffer_static) {

			/* parameter_flashfs_write pads the entry to a word in place */
			working_buffer_size = *buf_size + পরিমাণ(flash_entry_header_t) + SizeMask;
			working_buffer = malloc(working_buffer_size);

		}

		/* Allocation failed or not provided */

		যদি (working_buffer == রিক্ত) {

			working_buffer_size = 0;

		} অন্যথা {

			/* We have a buffer reserve space and init it */
			*buffer = &working_buffer[পরিমাণ(flash_entry_header_t)];
			*buf_size = working_buffer_size - পরিমাণ(flash_entry_header_t) - SizeMask;
			memset(working_buffer, 0xff, working_buffer_size);
			rv = 0;

		}
	}

	ঘূৰাই rv;
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

পূর্ণাঙ্ক parameter_flashfs_erase(শূন্য)
{
	পূর্ণাঙ্ক rv = -ENXIO;

	যদি (sector_map) {
		rv = 0;

		বাবে (পূর্ণাঙ্ক s = 0; sector_map[s].address; s++) {
			পূর্ণাঙ্ক sz = erase_sector(&sector_map[s], (flash_entry_header_t *)sector_map[s].address);

			যদি (sz != 0) {
				ঘূৰাই sz;
			}

			rv += sector_map[s].size;
		}
	}

	ঘূৰাই rv;
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

পূর্ণাঙ্ক parameter_flashfs_init(sector_descriptor_t *fconfig, uint8_t *buffer, uint16_t size)
{
	পূর্ণাঙ্ক rv = 0;
	sector_map = fconfig;
	working_buffer_static = buffer != রিক্ত;

	যদি (!working_buffer_static) {
		size = 0;
	}

	working_buffer = buffer;
	working_buffer_size = size;
	last_erased = -1;

	/* Sanity check */

	/* A non-blank store with no valid entry is torn or foreign; import
	 * reports -EILSEQ. Erasing here would hide a successful retry after
	 * a torn write. */

	ঘূৰাই rv;
}

#যদি defined(FLASH_UNIT_TEST)

স্থির sector_descriptor_t  test_sector_map[] = {
	{1, 16 * 1024, 0x08004000},
	{2, 16 * 1024, 0x08008000},
	{0, 0, 0},
};

__EXPORT শূন্য test(শূন্য);

uint8_t test_buf[32 * 1024];

__EXPORT শূন্য test(শূন্য)
{
	uint16_t largest_block = (32 * 1024) + 32;
	uint8_t *buffer = malloc(largest_block);

	parameter_flashfs_init(test_sector_map, buffer, largest_block);

	বাবে (পূর্ণাঙ্ক t = 0; t < পরিমাণ(test_buf); t++) {
		test_buf[t] = (uint8_t) t;
	}

	পূর্ণাঙ্ক er = parameter_flashfs_erase();
	uint8_t *fbuffer;
	size_t buf_size;
	পূর্ণাঙ্ক written = 0;
	পূর্ণাঙ্ক পঠ = 0;
	পূর্ণাঙ্ক rv = 0;

	বাবে (পূর্ণাঙ্ক a = 0; a <= 4; a++) {
		parameter_flashfs_erase();
		rv =  parameter_flashfs_alloc(parameters_token, &fbuffer, &buf_size);
		memcpy(fbuffer, test_buf, a);
		buf_size = a;
		written = parameter_flashfs_write(parameters_token, fbuffer, buf_size);
		পঠ = parameter_flashfs_read(parameters_token, &fbuffer, &buf_size);
		parameter_flashfs_free();

		যদি (পঠ != written) {
			স্থির চঞ্চল পূর্ণাঙ্ক j;
			j++;
		}
	}

	পূর্ণাঙ্ক block = 2048;

	বাবে (পূর্ণাঙ্ক a = 0; a <= 8; a++) {
		parameter_flashfs_erase();
		rv =  parameter_flashfs_alloc(parameters_token, &fbuffer, &buf_size);
		memcpy(fbuffer, test_buf, block);
		buf_size = block;
		written = parameter_flashfs_write(parameters_token, fbuffer, buf_size);
		পঠ = parameter_flashfs_read(parameters_token, &fbuffer, &buf_size);
		parameter_flashfs_free();

		যদি (পঠ != written) {
			স্থির চঞ্চল পূর্ণাঙ্ক j;
			j++;
		}

		block += 2048;
	}

	rv++;
	er++;
	মুক্ত(buffer);
}
#endif /* FLASH_UNIT_TEST */
#endif /* CONFIG_ARCH_HAVE_PROGMEM */
