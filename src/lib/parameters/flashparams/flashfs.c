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

#अन्तर्भाव <px4_platform_common/defines.h>
#अन्तर्भाव <px4_platform_common/posix.h>
#अन्तर्भाव <nuttx/crc32.h>
#अन्तर्भाव <stddef.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <stdbool.h>
#अन्तर्भाव <stdint.h>
#अन्तर्भाव <stdlib.h>
#अन्तर्भाव <errno.h>
#अन्तर्भाव "flashfs.h"
#अन्तर्भाव <nuttx/compiler.h>
#अन्तर्भाव <nuttx/progmem.h>
#अन्तर्भाव <board_config.h>

#जइ defined(CONFIG_ARCH_HAVE_PROGMEM) || defined(BOARD_USE_EXTERNAL_FLASH)

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Private Types
 ****************************************************************************/

प्रकारनिर्देश uint32_t h_magic_t;
प्रकारनिर्देश uint16_t h_size_t;
प्रकारनिर्देश uint16_t h_flag_t;
प्रकारनिर्देश uint16_t data_size_t;

प्रकारनिर्देश गणना  flash_config_t {
	LargestBlock = 2 * 1024, // This represents the size need for backing store
	MagicSig     = 0xaa553cc3,
	BlankSig     = 0xffffffff
} flash_config_t;

प्रकारनिर्देश गणना  flash_flags_t {
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
#pragma GCC diagnostic प्रक्षिप
#pragma GCC diagnostic ignored "-Wattributes"
प्रकारनिर्देश begin_packed_struct संरचना flash_entry_header_t {
	h_magic_t            magic;           /* Used to ID files*/
	h_flag_t             flag;            /* Used to erase this entry */
	uint32_t             crc;             /* Calculated over the size - end of data */
	h_size_t             size;            /* When added to a byte pointer to flash_entry_header_t
                                               * Will result the offset of the next active file or
                                               * free space. */
	flash_file_token_t   file_token;      /* file token type - essentially the name/type */
} end_packed_struct flash_entry_header_t __attribute__((aligned(परिमाण(uint32_t))));
#pragma GCC diagnostic निष्कासय

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/
स्थिर uint8_t *working_buffer;
स्थिर size_t working_buffer_size;
स्थिर bool working_buffer_static;
स्थिर sector_descriptor_t *sector_map;
स्थिर पूर्णाङ्क last_erased;

/****************************************************************************
 * Public Data
 ****************************************************************************/

ध्रुव flash_file_token_t parameters_token = {
	.n = {'p', 'l', 'o', 'g'},
};

ध्रुव flash_file_token_t parameters_legacy_token = {
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

शून्य parameter_flashfs_free(शून्य)
{
	जइ (!working_buffer_static && working_buffer != रिक्त) {
		मुक्त(working_buffer);
		working_buffer = रिक्त;
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

स्थिर अन्तःस्थ पूर्णाङ्क blank_flash(uint32_t *pf)
{
	प्रत्यागम *pf == BlankSig;
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

स्थिर bool blank_check(flash_entry_header_t *pf,
			size_t new_size)
{
	bool rv = true;
	uint32_t *pm = (uint32_t *) pf;
	new_size /= परिमाण(uint32_t);

	जाव (new_size-- && rv) {
		जइ (!blank_flash(pm++)) {
			rv = false;
		}
	}

	प्रत्यागम rv;
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

स्थिर अन्तःस्थ पूर्णाङ्क valid_magic(h_magic_t *pm)
{
	प्रत्यागम *pm == MagicSig;
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

स्थिर अन्तःस्थ पूर्णाङ्क blank_entry(flash_entry_header_t *fi)
{
	प्रत्यागम fi->magic == BlankSig  &&  fi->flag == BlankEntry;
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

स्थिर अन्तःस्थ पूर्णाङ्क valid_entry(flash_entry_header_t *fi)
{
	प्रत्यागम (fi->flag & MaskEntry) == ValidEntry;
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

स्थिर अन्तःस्थ पूर्णाङ्क entry_size_adjust(flash_entry_header_t *fi)
{
	प्रत्यागम fi->flag & SizeMask;
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

स्थिर अन्तःस्थ flash_entry_header_t *next_entry(flash_entry_header_t *fi)
{
	uint8_t *pb = (uint8_t *)fi;
	प्रत्यागम (flash_entry_header_t *)(pb + fi->size);
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

स्थिर अन्तःस्थ uint8_t *entry_data(flash_entry_header_t *fi)
{
	प्रत्यागम ((uint8_t *)fi) + परिमाण(flash_entry_header_t);
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

स्थिर अन्तःस्थ data_size_t entry_data_length(flash_entry_header_t *fi)
{
	प्रत्यागम fi->size - (परिमाण(flash_entry_header_t) + entry_size_adjust(fi));
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

स्थिर अन्तःस्थ ध्रुव uint8_t *entry_crc_start(flash_entry_header_t *fi)
{
	प्रत्यागम (ध्रुव uint8_t *)&fi->size;
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

स्थिर अन्तःस्थ data_size_t entry_crc_length(flash_entry_header_t *fi)
{
	प्रत्यागम fi->size - offsetof(flash_entry_header_t, size);
}

/* The parameter sector can be the last one on the chip, so no header
 * field may be read until the whole header is known to lie inside it. */
स्थिर अन्तःस्थ bool header_fits(flash_entry_header_t *pf, h_magic_t *pe)
{
	प्रत्यागम pf + 1 <= (flash_entry_header_t *)pe;
}

स्थिर bool crc_header_ok(flash_entry_header_t *pf, h_magic_t *pe)
{
	जइ (!header_fits(pf, pe)) {
		प्रत्यागम false;
	}

	जइ (pf->size < परिमाण(flash_entry_header_t)) {
		प्रत्यागम false;
	}

	ध्रुव uint8_t *crc_start = entry_crc_start(pf);
	data_size_t crc_length = entry_crc_length(pf);

	जइ (crc_start + crc_length > (uint8_t *)pe) {
		प्रत्यागम false;
	}

	प्रत्यागम pf->crc == crc32(crc_start, crc_length);
}

/* The walkers stop at the last word of a sector and reject a record that
 * ends beyond it, so nothing may be written into that word. */
स्थिर अन्तःस्थ size_t sector_usable(पूर्णाङ्क s)
{
	प्रत्यागम sector_map[s].size - परिमाण(h_magic_t);
}

स्थिर bool slot_is_free(flash_entry_header_t *pf, पूर्णाङ्क s, size_t required)
{
	uint8_t *begin = (uint8_t *)sector_map[s].address;
	uint8_t *end = begin + sector_usable(s);

	जइ ((uint8_t *)pf < begin || required == 0 || (uint8_t *)pf + required > end) {
		प्रत्यागम false;
	}

	/* Entries start on a flash row. up_progmem_write only asserts that in
	 * debug builds, so a slot found by the word-wise scan behind a torn
	 * record would otherwise be programmed misaligned. */
	जइ (((uintptr_t)pf & SizeMask) != 0) {
		प्रत्यागम false;
	}

	प्रत्यागम blank_entry(pf) && blank_check(pf, required);
}

स्थिर पूर्णाङ्क for_each_valid_entry(flash_file_token_t token,
				पूर्णाङ्क (*cb)(flash_entry_header_t *pf, शून्य *arg),
				शून्य *arg);

स्थिर पूर्णाङ्क stop_at_first(flash_entry_header_t *pf, शून्य *arg)
{
	*(flash_entry_header_t **)arg = pf;
	प्रत्यागम -ECANCELED;
}

/* Oldest valid entry for token, or NULL. */
स्थिर flash_entry_header_t *find_entry(flash_file_token_t token)
{
	flash_entry_header_t *pf = रिक्त;
	for_each_valid_entry(token, stop_at_first, &pf);
	प्रत्यागम pf;
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

स्थिर flash_entry_header_t *find_append(size_t required)
{
	जइ (sector_map == रिक्त || required == 0) {
		प्रत्यागम रिक्त;
	}

	flash_entry_header_t *last = रिक्त;
	पूर्णाङ्क last_s = -1;

	प्रति (पूर्णाङ्क s = 0; sector_map[s].address; s++) {
		h_magic_t *pmagic = (h_magic_t *) sector_map[s].address;
		h_magic_t *pe = pmagic + (sector_map[s].size / परिमाण(h_magic_t)) - 1;

		जाव (pmagic < pe) {
			जइ (!valid_magic(pmagic)) {
				pmagic++;
				अनुवर्त;
			}

			flash_entry_header_t *pf = (flash_entry_header_t *) pmagic;

			जइ (crc_header_ok(pf, pe)) {
				last = pf;
				last_s = s;
				pmagic = (h_magic_t *) next_entry(pf);
				pf = (flash_entry_header_t *) pmagic;

				/* Same stop rule as for_each_valid_entry: a record behind a
				 * blank header is invisible to replay, so it must not anchor
				 * the append point either. */
				जइ (pmagic >= pe || !header_fits(pf, pe) || blank_entry(pf)) {
					भङ्ग;
				}

			} अण्णहा {
				pmagic++;
			}
		}
	}

	ध्रुव पूर्णाङ्क start_s = (last == रिक्त) ? 0 : last_s;
	h_magic_t *pmagic = (last == रिक्त)
			    ? (h_magic_t *) sector_map[0].address
			    : (h_magic_t *) next_entry(last);

	प्रति (पूर्णाङ्क s = start_s; sector_map[s].address; s++) {
		जइ (s != start_s) {
			pmagic = (h_magic_t *) sector_map[s].address;
		}

		h_magic_t *pe = (h_magic_t *) sector_map[s].address
				+ (sector_map[s].size / परिमाण(h_magic_t)) - 1;

		जइ ((uint8_t *)pmagic < (uint8_t *)sector_map[s].address
		    || (uint8_t *)pmagic >= (uint8_t *)sector_map[s].address + sector_map[s].size) {
			जइ (s == start_s) {
				अनुवर्त;
			}

			pmagic = (h_magic_t *) sector_map[s].address;
		}

		जाव (pmagic < pe) {
			flash_entry_header_t *pf = (flash_entry_header_t *) pmagic;

			जइ (valid_magic(pmagic) && crc_header_ok(pf, pe)) {
				pmagic = (h_magic_t *) next_entry(pf);
				अनुवर्त;
			}

			जइ (slot_is_free(pf, s, required)) {
				प्रत्यागम pf;
			}

			pmagic++;
		}
	}

	प्रत्यागम रिक्त;
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

स्थिर पूर्णाङ्क for_each_valid_entry(flash_file_token_t token,
				पूर्णाङ्क (*cb)(flash_entry_header_t *pf, शून्य *arg),
				शून्य *arg)
{
	पूर्णाङ्क n = 0;

	जइ (sector_map == रिक्त) {
		प्रत्यागम -ENXIO;
	}

	प्रति (पूर्णाङ्क s = 0; sector_map[s].address; s++) {

		h_magic_t *pmagic = (h_magic_t *) sector_map[s].address;
		h_magic_t *pe = pmagic + (sector_map[s].size / परिमाण(h_magic_t)) - 1;

cont:

		जाव (pmagic < pe && !valid_magic(pmagic)) {
			pmagic++;
		}

		जइ (pmagic >= pe) { अनुवर्त; }

		flash_entry_header_t *pf = (flash_entry_header_t *) pmagic;

		जइ (crc_header_ok(pf, pe)) {

			जइ (valid_entry(pf) && pf->file_token.t == token.t) {
				जइ (cb != रिक्त) {
					पूर्णाङ्क rv = cb(pf, arg);

					जइ (rv < 0) {
						प्रत्यागम rv;
					}
				}

				n++;
			}

			pf = next_entry(pf);
			pmagic = (h_magic_t *) pf;

			जइ (pmagic >= pe || !header_fits(pf, pe) || blank_entry(pf)) {
				अनुवर्त;
			}

		} अण्णहा {
			pmagic++;
		}

		गच्छ cont;
	}

	प्रत्यागम n;
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

स्थिर पूर्णाङ्क erase_sector(sector_descriptor_t *sm, flash_entry_header_t *pf)
{
	पूर्णाङ्क rv = 0;
#जइ defined(BOARD_USE_EXTERNAL_FLASH)
	ssize_t block = up_progmem_ext_getpage((size_t)pf);
#अण्णहा
	ssize_t block = up_progmem_getpage((size_t)pf);
#endif

	जइ (block > 0 && block == sm->page) {
		last_erased = sm->page;
#जइ defined(BOARD_USE_EXTERNAL_FLASH)
		ssize_t size = up_progmem_ext_eraseblock(block);
#अण्णहा
		ssize_t size = up_progmem_eraseblock(block);
#endif

		जइ (size < 0 || size != (ssize_t)sm->size) {
			rv = size;
		}
	}

	प्रत्यागम rv;
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

पूर्णाङ्क parameter_flashfs_read(flash_file_token_t token, uint8_t **buffer, size_t
			   *buf_size)
{
	पूर्णाङ्क rv = -ENXIO;

	जइ (sector_map) {

		rv = -ENOENT;
		flash_entry_header_t *pf = find_entry(token);

		जइ (pf) {
			(*buffer) = entry_data(pf);
			rv = entry_data_length(pf);
			*buf_size = rv;
		}
	}

	प्रत्यागम rv;
}

/****************************************************************************
 * Name: parameter_flashfs_blank
 *
 * Description:
 *   Reports whether the parameter storage is fully erased, so callers can
 *   tell a blank store from one that holds unparseable data.
 *
 ****************************************************************************/

पूर्णाङ्क parameter_flashfs_blank(शून्य)
{
	जइ (sector_map == रिक्त) {
		प्रत्यागम -ENXIO;
	}

	प्रति (पूर्णाङ्क s = 0; sector_map[s].address; s++) {
		uint32_t *pm = (uint32_t *) sector_map[s].address;
		uint32_t *pe = pm + (sector_map[s].size / परिमाण(uint32_t));

		जाव (pm < pe) {
			जइ (!blank_flash(pm++)) {
				प्रत्यागम 0;
			}
		}
	}

	प्रत्यागम 1;
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

पूर्णाङ्क
parameter_flashfs_write(flash_file_token_t token, uint8_t *buffer, size_t buf_size)
{
	पूर्णाङ्क rv = -ENXIO;

	जइ (sector_map) {

		rv = 0;

		/* Calculate the total space needed */

		size_t total_size = buf_size + परिमाण(flash_entry_header_t);
		size_t alignment = परिमाण(h_magic_t) - 1;
		size_t  size_adjust = ((total_size + alignment) & ~alignment) - total_size;
		total_size += size_adjust;

		जइ (total_size > UINT16_MAX) {
			प्रत्यागम -EFBIG;
		}

		/* Append after the last CRC-valid record. Compaction is the caller's job. */
		flash_entry_header_t *pf = find_append(total_size);

		जइ (pf == 0) {
			प्रत्यागम -ENOSPC;
		}

		flash_entry_header_t *pn = (flash_entry_header_t *)(buffer - परिमाण(flash_entry_header_t));
		pn->magic = MagicSig;
		pn->file_token.t = token.t;
		pn->flag = ValidEntry + size_adjust;
		pn->size = total_size;

		प्रति (size_t a = 0; a < size_adjust; a++) {
			buffer[buf_size + a] = (uint8_t)BlankSig;
		}

		pn->crc = crc32(entry_crc_start(pn), entry_crc_length(pn));
#जइ defined(BOARD_USE_EXTERNAL_FLASH)
		rv = up_progmem_ext_write((size_t) pf, pn, pn->size);
#अण्णहा
		rv = up_progmem_write((size_t) pf, pn, pn->size);
#endif
		पूर्णाङ्क system_bytes = (परिमाण(flash_entry_header_t) + size_adjust);

		जइ (rv >= system_bytes) {
			rv -= system_bytes;
		}
	}

	प्रत्यागम rv;
}

/****************************************************************************
 * Name: parameter_flashfs_walk
 ****************************************************************************/

संरचना walk_pack {
	parameter_flashfs_walk_cb cb;
	शून्य *arg;
};

स्थिर पूर्णाङ्क walk_one(flash_entry_header_t *pf, शून्य *arg)
{
	संरचना walk_pack *w = (संरचना walk_pack *)arg;
	प्रत्यागम w->cb(entry_data(pf), entry_data_length(pf), w->arg);
}

पूर्णाङ्क parameter_flashfs_walk(flash_file_token_t token, parameter_flashfs_walk_cb cb, शून्य *arg)
{
	संरचना walk_pack w = { cb, arg };
	प्रत्यागम for_each_valid_entry(token, walk_one, &w);
}

/****************************************************************************
 * Name: parameter_flashfs_needs_compact
 ****************************************************************************/

size_t parameter_flashfs_max_payload(शून्य)
{
	जइ (sector_map == रिक्त) {
		प्रत्यागम 0;
	}

	size_t max_total = 0;

	प्रति (पूर्णाङ्क s = 0; sector_map[s].address; s++) {
		जइ (sector_usable(s) > max_total) {
			max_total = sector_usable(s);
		}
	}

	/* h_size_t holds the record size */
	जइ (max_total > UINT16_MAX) {
		max_total = UINT16_MAX;
	}

	max_total &= ~(size_t)SizeMask;

	जइ (max_total <= परिमाण(flash_entry_header_t)) {
		प्रत्यागम 0;
	}

	प्रत्यागम max_total - परिमाण(flash_entry_header_t);
}

पूर्णाङ्क parameter_flashfs_needs_compact(flash_file_token_t token, size_t snapshot_size)
{
	जइ (sector_map == रिक्त) {
		प्रत्यागम -ENXIO;
	}

	flash_entry_header_t *first = find_entry(token);

	जइ (first == रिक्त) {
		प्रत्यागम 0;
	}

	/* Dead records ahead of the live data (a torn write, or the erased
	 * prefix the single-snapshot writer left) only go away with an erase. */
	जइ ((uintptr_t)first != (uintptr_t)sector_map[0].address) {
		प्रत्यागम 1;
	}

	/* Keep room after the log for a burst the size of the snapshot
	 * (reset-all tombstones, airframe PARAM_SET), floored so a UUID append
	 * does not force a boot erase. */
	ध्रुव size_t align = परिमाण(h_magic_t) - 1;
	ध्रुव size_t snapshot_total = (snapshot_size + परिमाण(flash_entry_header_t) + align) & ~align;
	ध्रुव size_t headroom = snapshot_total > 8192 ? snapshot_total : 8192;

	जइ (find_append(headroom) != रिक्त) {
		प्रत्यागम 0;
	}

	/* Compaction leaves the snapshot at the start of sector 0 and every
	 * other sector blank. Erase only if that layout has the room this one
	 * lacks, or every boot would pay the erase for nothing. */
	जइ (sector_usable(0) >= snapshot_total + headroom) {
		प्रत्यागम 1;
	}

	प्रति (पूर्णाङ्क s = 1; sector_map[s].address; s++) {
		जइ (sector_usable(s) >= headroom) {
			प्रत्यागम 1;
		}
	}

	प्रत्यागम 0;
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

पूर्णाङ्क parameter_flashfs_alloc(flash_file_token_t token, uint8_t **buffer, size_t *buf_size)
{
	पूर्णाङ्क rv = -ENXIO;

	जइ (sector_map) {

		rv = -ENOMEM;

		जइ (!working_buffer_static) {

			/* parameter_flashfs_write pads the entry to a word in place */
			working_buffer_size = *buf_size + परिमाण(flash_entry_header_t) + SizeMask;
			working_buffer = malloc(working_buffer_size);

		}

		/* Allocation failed or not provided */

		जइ (working_buffer == रिक्त) {

			working_buffer_size = 0;

		} अण्णहा {

			/* We have a buffer reserve space and init it */
			*buffer = &working_buffer[परिमाण(flash_entry_header_t)];
			*buf_size = working_buffer_size - परिमाण(flash_entry_header_t) - SizeMask;
			memset(working_buffer, 0xff, working_buffer_size);
			rv = 0;

		}
	}

	प्रत्यागम rv;
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

पूर्णाङ्क parameter_flashfs_erase(शून्य)
{
	पूर्णाङ्क rv = -ENXIO;

	जइ (sector_map) {
		rv = 0;

		प्रति (पूर्णाङ्क s = 0; sector_map[s].address; s++) {
			पूर्णाङ्क sz = erase_sector(&sector_map[s], (flash_entry_header_t *)sector_map[s].address);

			जइ (sz != 0) {
				प्रत्यागम sz;
			}

			rv += sector_map[s].size;
		}
	}

	प्रत्यागम rv;
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

पूर्णाङ्क parameter_flashfs_init(sector_descriptor_t *fconfig, uint8_t *buffer, uint16_t size)
{
	पूर्णाङ्क rv = 0;
	sector_map = fconfig;
	working_buffer_static = buffer != रिक्त;

	जइ (!working_buffer_static) {
		size = 0;
	}

	working_buffer = buffer;
	working_buffer_size = size;
	last_erased = -1;

	/* Sanity check */

	/* A non-blank store with no valid entry is torn or foreign; import
	 * reports -EILSEQ. Erasing here would hide a successful retry after
	 * a torn write. */

	प्रत्यागम rv;
}

#जइ defined(FLASH_UNIT_TEST)

स्थिर sector_descriptor_t  test_sector_map[] = {
	{1, 16 * 1024, 0x08004000},
	{2, 16 * 1024, 0x08008000},
	{0, 0, 0},
};

__EXPORT शून्य test(शून्य);

uint8_t test_buf[32 * 1024];

__EXPORT शून्य test(शून्य)
{
	uint16_t largest_block = (32 * 1024) + 32;
	uint8_t *buffer = malloc(largest_block);

	parameter_flashfs_init(test_sector_map, buffer, largest_block);

	प्रति (पूर्णाङ्क t = 0; t < परिमाण(test_buf); t++) {
		test_buf[t] = (uint8_t) t;
	}

	पूर्णाङ्क er = parameter_flashfs_erase();
	uint8_t *fbuffer;
	size_t buf_size;
	पूर्णाङ्क written = 0;
	पूर्णाङ्क पठ = 0;
	पूर्णाङ्क rv = 0;

	प्रति (पूर्णाङ्क a = 0; a <= 4; a++) {
		parameter_flashfs_erase();
		rv =  parameter_flashfs_alloc(parameters_token, &fbuffer, &buf_size);
		memcpy(fbuffer, test_buf, a);
		buf_size = a;
		written = parameter_flashfs_write(parameters_token, fbuffer, buf_size);
		पठ = parameter_flashfs_read(parameters_token, &fbuffer, &buf_size);
		parameter_flashfs_free();

		जइ (पठ != written) {
			स्थिर चञ्चल पूर्णाङ्क j;
			j++;
		}
	}

	पूर्णाङ्क block = 2048;

	प्रति (पूर्णाङ्क a = 0; a <= 8; a++) {
		parameter_flashfs_erase();
		rv =  parameter_flashfs_alloc(parameters_token, &fbuffer, &buf_size);
		memcpy(fbuffer, test_buf, block);
		buf_size = block;
		written = parameter_flashfs_write(parameters_token, fbuffer, buf_size);
		पठ = parameter_flashfs_read(parameters_token, &fbuffer, &buf_size);
		parameter_flashfs_free();

		जइ (पठ != written) {
			स्थिर चञ्चल पूर्णाङ्क j;
			j++;
		}

		block += 2048;
	}

	rv++;
	er++;
	मुक्त(buffer);
}
#endif /* FLASH_UNIT_TEST */
#endif /* CONFIG_ARCH_HAVE_PROGMEM */
