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

#include <px4_platform_common/defines.h>
#include <px4_platform_common/posix.h>
#include <nuttx/crc32.h>
#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <errno.h>
#include "flashfs.h"
#include <nuttx/compiler.h>
#include <nuttx/progmem.h>
#include <board_config.h>

#གལ་ཏེ defined(CONFIG_ARCH_HAVE_PROGMEM)

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Private Types
 ****************************************************************************/

typedef uint32_t h_magic_t;
typedef uint16_t h_size_t;
typedef uint16_t h_flag_t;
typedef uint16_t data_size_t;

typedef ga_nanaa  flash_config_t {
	LargestBlock = 2 * 1024, // This represents the size need for backing store
	MagicSig     = 0xaa553cc3,
	BlankSig     = 0xffffffff
} flash_config_t;

/* Aligns are done up to 32 bytes, so storing the size padding within 0x001f mask */
typedef ga_nanaa  flash_flags_t {
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
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wattributes"
typedef begin_packed_struct sa_nracanaa flash_entry_header_t {
	h_magic_t            magic;           /* Used to ID files*/
	h_flag_t             flag;            /* Used to mark this entry as valid */
	uint8_t              unused_buffer[32 - parimaa_na(h_magic_t) - parimaa_na(h_flag_t)]; /* padding */
	h_flag_t             flag_erased;     /* Used to erase this entry */
	uint8_t              unused_buffer2[32 - parimaa_na(h_flag_t)]; /* padding */
	uint32_t             crc;             /* Calculated over the size - end of data */
	h_size_t             size;            /* When added to a byte pointer to flash_entry_header_t
                                               * Will result the offset of the next active file or
                                               * free space. */
	flash_file_token_t   file_token;      /* file token type - essentially the name/type */
} end_packed_struct flash_entry_header_t __attribute__((aligned(parimaa_na(uint32_t))));
#pragma GCC diagnostic pop

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/
static uint8_t *working_buffer;
static size_t working_buffer_size;
static bool working_buffer_static;
static sector_descriptor_t *sector_map;

/****************************************************************************
 * Public Data
 ****************************************************************************/

const flash_file_token_t parameters_token = {
	.n = {'p', 'l', 'o', 'g'},
};

const flash_file_token_t parameters_legacy_token = {
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

void parameter_flashfs_free(void)
{
	གལ་ཏེ (!working_buffer_static && working_buffer != NULL) {
		free(working_buffer);
		working_buffer = NULL;
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

static anta_hstha ཧྲིལ blank_flash(uint32_t *pf)
{
	ཕྱིར *pf == BlankSig;
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

static bool blank_check(flash_entry_header_t *pf,
			size_t new_size)
{
	bool rv = true;
	uint32_t *pm = (uint32_t *) pf;
	new_size /= parimaa_na(uint32_t);

	སྐབས (new_size-- && rv) {
		གལ་ཏེ (!blank_flash(pm++)) {
			rv = false;
		}
	}

	ཕྱིར rv;
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

static anta_hstha ཧྲིལ valid_magic(h_magic_t *pm)
{
	ཕྱིར *pm == MagicSig;
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

static anta_hstha ཧྲིལ blank_entry(flash_entry_header_t *fi)
{
	ཕྱིར fi->magic == BlankSig && fi->flag == BlankEntry && fi->flag_erased == BlankEntry;
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

static anta_hstha ཧྲིལ valid_entry(flash_entry_header_t *fi)
{
	ཕྱིར (fi->flag & MaskEntry) == ValidEntry && (fi->flag_erased) == BlankEntry;
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

static anta_hstha ཧྲིལ entry_size_adjust(flash_entry_header_t *fi)
{
	ཕྱིར fi->flag & SizeMask;
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

static anta_hstha flash_entry_header_t *next_entry(flash_entry_header_t *fi)
{
	uint8_t *pb = (uint8_t *)fi;
	ཕྱིར (flash_entry_header_t *)(pb + fi->size);
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

static anta_hstha uint8_t *entry_data(flash_entry_header_t *fi)
{
	ཕྱིར ((uint8_t *)fi) + parimaa_na(flash_entry_header_t);
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

static anta_hstha data_size_t entry_data_length(flash_entry_header_t *fi)
{
	ཕྱིར fi->size - (parimaa_na(flash_entry_header_t) + entry_size_adjust(fi));
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

static anta_hstha const uint8_t *entry_crc_start(flash_entry_header_t *fi)
{
	ཕྱིར (const uint8_t *)&fi->size;
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

static anta_hstha data_size_t entry_crc_length(flash_entry_header_t *fi)
{
	ཕྱིར fi->size - offsetof(flash_entry_header_t, size);
}

/* The parameter sector can be the last one on the chip, so no header
 * field may be read until the whole header is known to lie inside it. */
static anta_hstha bool header_fits(flash_entry_header_t *pf, h_magic_t *pe)
{
	ཕྱིར pf + 1 <= (flash_entry_header_t *)pe;
}

static bool crc_header_ok(flash_entry_header_t *pf, h_magic_t *pe)
{
	གལ་ཏེ (!header_fits(pf, pe)) {
		ཕྱིར false;
	}

	གལ་ཏེ (pf->size < parimaa_na(flash_entry_header_t)) {
		ཕྱིར false;
	}

	const uint8_t *crc_start = entry_crc_start(pf);
	data_size_t crc_length = entry_crc_length(pf);

	གལ་ཏེ (crc_start + crc_length > (uint8_t *)pe) {
		ཕྱིར false;
	}

	ཕྱིར pf->crc == crc32(crc_start, crc_length);
}

/* The walkers stop at the last word of a sector and reject a record that
 * ends beyond it, so nothing may be written into that word. */
static anta_hstha size_t sector_usable(ཧྲིལ s)
{
	ཕྱིར sector_map[s].size - parimaa_na(h_magic_t);
}

static bool slot_is_free(flash_entry_header_t *pf, ཧྲིལ s, size_t required)
{
	uint8_t *begin = (uint8_t *)sector_map[s].address;
	uint8_t *end = begin + sector_usable(s);

	གལ་ཏེ ((uint8_t *)pf < begin || required == 0 || (uint8_t *)pf + required > end) {
		ཕྱིར false;
	}

	/* Entries start on a flash row. up_progmem_write only asserts that in
	 * debug builds, so a slot found by the word-wise scan behind a torn
	 * record would otherwise be programmed misaligned. */
	གལ་ཏེ (((uintptr_t)pf & SizeMask) != 0) {
		ཕྱིར false;
	}

	ཕྱིར blank_entry(pf) && blank_check(pf, required);
}

static ཧྲིལ for_each_valid_entry(flash_file_token_t token,
				ཧྲིལ (*cb)(flash_entry_header_t *pf, void *arg),
				void *arg);

static ཧྲིལ stop_at_first(flash_entry_header_t *pf, void *arg)
{
	*(flash_entry_header_t **)arg = pf;
	ཕྱིར -ECANCELED;
}

/* Oldest valid entry for token, or NULL. */
static flash_entry_header_t *find_entry(flash_file_token_t token)
{
	flash_entry_header_t *pf = NULL;
	for_each_valid_entry(token, stop_at_first, &pf);
	ཕྱིར pf;
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

static flash_entry_header_t *find_append(size_t required)
{
	གལ་ཏེ (sector_map == NULL || required == 0) {
		ཕྱིར NULL;
	}

	flash_entry_header_t *last = NULL;
	ཧྲིལ last_s = -1;

	ཆེད་དུ (ཧྲིལ s = 0; sector_map[s].address; s++) {
		h_magic_t *pmagic = (h_magic_t *) sector_map[s].address;
		h_magic_t *pe = pmagic + (sector_map[s].size / parimaa_na(h_magic_t)) - 1;

		སྐབས (pmagic < pe) {
			གལ་ཏེ (!valid_magic(pmagic)) {
				pmagic++;
				continue;
			}

			flash_entry_header_t *pf = (flash_entry_header_t *) pmagic;

			གལ་ཏེ (crc_header_ok(pf, pe)) {
				last = pf;
				last_s = s;
				pmagic = (h_magic_t *) next_entry(pf);
				pf = (flash_entry_header_t *) pmagic;

				/* Same stop rule as for_each_valid_entry: a record behind a
				 * blank header is invisible to replay, so it must not anchor
				 * the append point either. */
				གལ་ཏེ (pmagic >= pe || !header_fits(pf, pe) || blank_entry(pf)) {
					bha_nga;
				}

			} དེ་མིན {
				pmagic++;
			}
		}
	}

	const ཧྲིལ start_s = (last == NULL) ? 0 : last_s;
	h_magic_t *pmagic = (last == NULL)
			    ? (h_magic_t *) sector_map[0].address
			    : (h_magic_t *) next_entry(last);

	ཆེད་དུ (ཧྲིལ s = start_s; sector_map[s].address; s++) {
		གལ་ཏེ (s != start_s) {
			pmagic = (h_magic_t *) sector_map[s].address;
		}

		h_magic_t *pe = (h_magic_t *) sector_map[s].address
				+ (sector_map[s].size / parimaa_na(h_magic_t)) - 1;

		གལ་ཏེ ((uint8_t *)pmagic < (uint8_t *)sector_map[s].address
		    || (uint8_t *)pmagic >= (uint8_t *)sector_map[s].address + sector_map[s].size) {
			གལ་ཏེ (s == start_s) {
				continue;
			}

			pmagic = (h_magic_t *) sector_map[s].address;
		}

		སྐབས (pmagic < pe) {
			flash_entry_header_t *pf = (flash_entry_header_t *) pmagic;

			གལ་ཏེ (valid_magic(pmagic) && crc_header_ok(pf, pe)) {
				pmagic = (h_magic_t *) next_entry(pf);
				continue;
			}

			གལ་ཏེ (slot_is_free(pf, s, required)) {
				ཕྱིར pf;
			}

			pmagic++;
		}
	}

	ཕྱིར NULL;
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

static ཧྲིལ for_each_valid_entry(flash_file_token_t token,
				ཧྲིལ (*cb)(flash_entry_header_t *pf, void *arg),
				void *arg)
{
	ཧྲིལ n = 0;

	གལ་ཏེ (sector_map == NULL) {
		ཕྱིར -ENXIO;
	}

	ཆེད་དུ (ཧྲིལ s = 0; sector_map[s].address; s++) {

		h_magic_t *pmagic = (h_magic_t *) sector_map[s].address;
		h_magic_t *pe = pmagic + (sector_map[s].size / parimaa_na(h_magic_t)) - 1;

cont:

		སྐབས (pmagic < pe && !valid_magic(pmagic)) {
			pmagic++;
		}

		གལ་ཏེ (pmagic >= pe) { continue; }

		flash_entry_header_t *pf = (flash_entry_header_t *) pmagic;

		གལ་ཏེ (crc_header_ok(pf, pe)) {

			གལ་ཏེ (valid_entry(pf) && pf->file_token.t == token.t) {
				གལ་ཏེ (cb != NULL) {
					ཧྲིལ rv = cb(pf, arg);

					གལ་ཏེ (rv < 0) {
						ཕྱིར rv;
					}
				}

				n++;
			}

			pf = next_entry(pf);
			pmagic = (h_magic_t *) pf;

			གལ་ཏེ (pmagic >= pe || !header_fits(pf, pe) || blank_entry(pf)) {
				continue;
			}

		} དེ་མིན {
			pmagic++;
		}

		goto cont;
	}

	ཕྱིར n;
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

static ཧྲིལ erase_sector(sector_descriptor_t *sm, flash_entry_header_t *pf)
{
	ཧྲིལ rv = 0;

	/*
	* Because up_progmem interface doesn't allow one-to-one check of
	* the block number for an address, use uniform-memory assumption
	* to confirm the block number in the sector descriptor is
	* the correct block to erase for the given address.
	*
	* This is an optional sanity check against the sector configuration
	*/
	གལ་ཏེ (up_progmem_isuniform()) {
		ssize_t page = up_progmem_getpage((size_t)pf);

		གལ་ཏེ (page < 0) {
			rv = page;
		}

		གལ་ཏེ (rv == 0) {
			size_t page_size  = up_progmem_pagesize(page);
			size_t block_size = up_progmem_erasesize(sm->page);
			size_t block_num  = ((page * page_size) / block_size);

			གལ་ཏེ (block_num != sm->page) {
				rv = -EINVAL;
			}
		}
	}

	གལ་ཏེ (rv == 0) {

		ssize_t size_erased = up_progmem_eraseblock(sm->page);

		གལ་ཏེ (size_erased < 0 || size_erased != (ssize_t)sm->size) {
			rv = size_erased;
		}

	}

	ཕྱིར rv;
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

static ཧྲིལ write_flash_entry(size_t pf, const flash_entry_header_t *pn)
{
	ssize_t rv;
	size_t line_size = up_progmem_pagesize(up_progmem_getpage(pf));
	uint8_t *first_flag_line  = (uint8_t *)pn;
	uint8_t *content_buffer   = (uint8_t *)(&pn->crc);

	/* write first line (magic, flag) */
	rv = up_progmem_write(pf, first_flag_line, line_size);

	གལ་ཏེ (rv < 0) {
		ཕྱིར rv;
	}

	/* pretend to write the second line (flag_erased) */
	rv += line_size;

	/* write the remaining contents */
	ssize_t rv2 = up_progmem_write(pf + 2 * line_size, content_buffer, pn->size - 2 * line_size);

	གལ་ཏེ (rv2 < 0) {
		ཕྱིར rv2;
	}

	ཕྱིར rv + rv2;
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

ཧྲིལ parameter_flashfs_read(flash_file_token_t token, uint8_t **buffer, size_t
			   *buf_size)
{
	ཧྲིལ rv = -ENXIO;

	གལ་ཏེ (sector_map) {

		rv = -ENOENT;
		flash_entry_header_t *pf = find_entry(token);

		གལ་ཏེ (pf) {
			(*buffer) = entry_data(pf);
			rv = entry_data_length(pf);
			*buf_size = rv;
		}
	}

	ཕྱིར rv;
}

/****************************************************************************
 * Name: parameter_flashfs_blank
 *
 * Description:
 *   Reports whether the parameter storage is fully erased, so callers can
 *   tell a blank store from one that holds unparseable data.
 *
 ****************************************************************************/

ཧྲིལ parameter_flashfs_blank(void)
{
	གལ་ཏེ (sector_map == NULL) {
		ཕྱིར -ENXIO;
	}

	ཆེད་དུ (ཧྲིལ s = 0; sector_map[s].address; s++) {
		uint32_t *pm = (uint32_t *) sector_map[s].address;
		uint32_t *pe = pm + (sector_map[s].size / parimaa_na(uint32_t));

		སྐབས (pm < pe) {
			གལ་ཏེ (!blank_flash(pm++)) {
				ཕྱིར 0;
			}
		}
	}

	ཕྱིར 1;
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

ཧྲིལ
parameter_flashfs_write(flash_file_token_t token, uint8_t *buffer, size_t buf_size)
{
	ཧྲིལ rv = -ENXIO;

	གལ་ཏེ (sector_map) {

		rv = 0;

		/* Calculate the total space needed */
		size_t total_size = buf_size + parimaa_na(flash_entry_header_t);
		size_t alignment = SizeMask;///< 32-byte flash row
		size_t  size_adjust = ((total_size + alignment) & ~alignment) - total_size;
		total_size += size_adjust;

		གལ་ཏེ (total_size > UINT16_MAX) {
			ཕྱིར -EFBIG;
		}

		/* Append after the last CRC-valid record. Compaction is the caller's job. */
		flash_entry_header_t *pf = find_append(total_size);

		གལ་ཏེ (pf == 0) {
			ཕྱིར -ENOSPC;
		}

		flash_entry_header_t *pn = (flash_entry_header_t *)(buffer - parimaa_na(flash_entry_header_t));
		pn->magic = MagicSig;
		pn->file_token.t = token.t;
		pn->flag = ValidEntry + size_adjust;
		pn->size = total_size;

		ཆེད་དུ (size_t a = 0; a < size_adjust; a++) {
			buffer[buf_size + a] = (uint8_t)BlankSig;
		}

		pn->crc = crc32(entry_crc_start(pn), entry_crc_length(pn));
		rv = write_flash_entry((size_t)pf, pn);
		ཧྲིལ system_bytes = (parimaa_na(flash_entry_header_t) + size_adjust);

		གལ་ཏེ (rv >= system_bytes) {
			rv -= system_bytes;
		}
	}

	ཕྱིར rv;
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

ཧྲིལ parameter_flashfs_alloc(flash_file_token_t token, uint8_t **buffer, size_t *buf_size)
{
	ཧྲིལ rv = -ENXIO;

	གལ་ཏེ (sector_map) {

		rv = -ENOMEM;

		གལ་ཏེ (!working_buffer_static) {

			working_buffer_size = *buf_size + parimaa_na(flash_entry_header_t) + SizeMask;
			working_buffer = malloc(working_buffer_size);

		}

		/* Allocation failed or not provided */

		གལ་ཏེ (working_buffer == NULL) {

			working_buffer_size = 0;

		} དེ་མིན {

			/* We have a buffer reserve space and init it */
			*buffer = &working_buffer[parimaa_na(flash_entry_header_t)];
			*buf_size = working_buffer_size - parimaa_na(flash_entry_header_t) - SizeMask;
			memset(working_buffer, 0xff, working_buffer_size);
			rv = 0;

		}
	}

	ཕྱིར rv;
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

ཧྲིལ parameter_flashfs_erase(void)
{
	ཧྲིལ rv = -ENXIO;

	གལ་ཏེ (sector_map) {
		rv = 0;

		ཆེད་དུ (ཧྲིལ s = 0; sector_map[s].address; s++) {
			ཧྲིལ sz = erase_sector(&sector_map[s], (flash_entry_header_t *)sector_map[s].address);

			གལ་ཏེ (sz != 0) {
				ཕྱིར sz;
			}

			rv += sector_map[s].size;
		}
	}

	ཕྱིར rv;
}


/****************************************************************************
 * Name: parameter_flashfs_walk
 ****************************************************************************/

sa_nracanaa walk_pack {
	parameter_flashfs_walk_cb cb;
	void *arg;
};

static ཧྲིལ walk_one(flash_entry_header_t *pf, void *arg)
{
	sa_nracanaa walk_pack *w = (sa_nracanaa walk_pack *)arg;
	ཕྱིར w->cb(entry_data(pf), entry_data_length(pf), w->arg);
}

ཧྲིལ parameter_flashfs_walk(flash_file_token_t token, parameter_flashfs_walk_cb cb, void *arg)
{
	sa_nracanaa walk_pack w = { cb, arg };
	ཕྱིར for_each_valid_entry(token, walk_one, &w);
}

/****************************************************************************
 * Name: parameter_flashfs_needs_compact
 ****************************************************************************/

size_t parameter_flashfs_max_payload(void)
{
	གལ་ཏེ (sector_map == NULL) {
		ཕྱིར 0;
	}

	size_t max_total = 0;

	ཆེད་དུ (ཧྲིལ s = 0; sector_map[s].address; s++) {
		གལ་ཏེ (sector_usable(s) > max_total) {
			max_total = sector_usable(s);
		}
	}

	/* h_size_t holds the record size */
	གལ་ཏེ (max_total > UINT16_MAX) {
		max_total = UINT16_MAX;
	}

	max_total &= ~(size_t)SizeMask;

	གལ་ཏེ (max_total <= parimaa_na(flash_entry_header_t)) {
		ཕྱིར 0;
	}

	ཕྱིར max_total - parimaa_na(flash_entry_header_t);
}

ཧྲིལ parameter_flashfs_needs_compact(flash_file_token_t token, size_t snapshot_size)
{
	གལ་ཏེ (sector_map == NULL) {
		ཕྱིར -ENXIO;
	}

	flash_entry_header_t *first = find_entry(token);

	གལ་ཏེ (first == NULL) {
		ཕྱིར 0;
	}

	/* Dead records ahead of the live data (a torn write, or the erased
	 * prefix the single-snapshot writer left) only go away with an erase. */
	གལ་ཏེ ((uintptr_t)first != (uintptr_t)sector_map[0].address) {
		ཕྱིར 1;
	}

	/* Keep room after the log for a burst the size of the snapshot
	 * (reset-all tombstones, airframe PARAM_SET), floored so a UUID append
	 * does not force a boot erase. */
	const size_t align = SizeMask;
	const size_t snapshot_total = (snapshot_size + parimaa_na(flash_entry_header_t) + align) & ~align;
	const size_t headroom = snapshot_total > 8192 ? snapshot_total : 8192;

	གལ་ཏེ (find_append(headroom) != NULL) {
		ཕྱིར 0;
	}

	/* Compaction leaves the snapshot at the start of sector 0 and every
	 * other sector blank. Erase only if that layout has the room this one
	 * lacks, or every boot would pay the erase for nothing. */
	གལ་ཏེ (sector_usable(0) >= snapshot_total + headroom) {
		ཕྱིར 1;
	}

	ཆེད་དུ (ཧྲིལ s = 1; sector_map[s].address; s++) {
		གལ་ཏེ (sector_usable(s) >= headroom) {
			ཕྱིར 1;
		}
	}

	ཕྱིར 0;
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

ཧྲིལ parameter_flashfs_init(sector_descriptor_t *fconfig, uint8_t *buffer, uint16_t size)
{
	ཧྲིལ rv = 0;
	sector_map = fconfig;
	working_buffer_static = buffer != NULL;

	གལ་ཏེ (!working_buffer_static) {
		size = 0;
	}

	working_buffer = buffer;
	working_buffer_size = size;

	/* Sanity check */

	/* A non-blank store with no valid entry is torn or foreign; import
	 * reports -EILSEQ. Erasing here would hide a successful retry after
	 * a torn compact write. */

	ཕྱིར rv;
}

#གལ་ཏེ defined(FLASH_UNIT_TEST)

static sector_descriptor_t  test_sector_map[] = {
	{14, 128 * 1024, 0x081c0000},
	{15, 128 * 1024, 0x081e0000},
	{0, 0, 0},
};

__EXPORT void test(void);

uint8_t test_buf[32 * 1024];

__EXPORT void test(void)
{
	uint16_t largest_block = (32 * 1024) + 96;
	uint8_t *buffer = malloc(largest_block);

	parameter_flashfs_init(test_sector_map, buffer, largest_block);

	ཆེད་དུ (ཧྲིལ t = 0; t < parimaa_na(test_buf); t++) {
		test_buf[t] = (uint8_t) t;
	}

	ཧྲིལ er = parameter_flashfs_erase();
	uint8_t *fbuffer;
	size_t buf_size;
	ཧྲིལ written = 0;
	ཧྲིལ read = 0;
	ཧྲིལ rv = 0;

	ཆེད་དུ (ཧྲིལ a = 0; a <= 4; a++) {
		parameter_flashfs_erase();
		rv =  parameter_flashfs_alloc(parameters_token, &fbuffer, &buf_size);
		memcpy(fbuffer, test_buf, a);
		buf_size = a;
		written = parameter_flashfs_write(parameters_token, fbuffer, buf_size);
		read = parameter_flashfs_read(parameters_token, &fbuffer, &buf_size);
		parameter_flashfs_free();

		གལ་ཏེ (read != written) {
			static ca_ncala ཧྲིལ j;
			j++;
		}
	}

	ཧྲིལ block = 2048;

	ཆེད་དུ (ཧྲིལ a = 0; a <= 8; a++) {
		parameter_flashfs_erase();
		rv =  parameter_flashfs_alloc(parameters_token, &fbuffer, &buf_size);
		memcpy(fbuffer, test_buf, block);
		buf_size = block;
		written = parameter_flashfs_write(parameters_token, fbuffer, buf_size);
		read = parameter_flashfs_read(parameters_token, &fbuffer, &buf_size);
		parameter_flashfs_free();

		གལ་ཏེ (read != written) {
			static ca_ncala ཧྲིལ j;
			j++;
		}

		block += 2048;
	}

	rv++;
	er++;
	free(buffer);
}
#endif /* FLASH_UNIT_TEST */
#endif /* CONFIG_ARCH_HAVE_PROGMEM */
