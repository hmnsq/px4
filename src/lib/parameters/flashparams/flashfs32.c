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

#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/defines.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/posix.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <nuttx/crc32.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stddef.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdbool.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdint.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <errno.h>
#ਅਨ੍ਤਰ੍ਭਾਵ "flashfs.h"
#ਅਨ੍ਤਰ੍ਭਾਵ <nuttx/compiler.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <nuttx/progmem.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <board_config.h>

#ਜੇ defined(CONFIG_ARCH_HAVE_PROGMEM)

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Private Types
 ****************************************************************************/

ਕਿਸਮਪਰਿਭਾਸ਼ਾ uint32_t h_magic_t;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ uint16_t h_ਮਾਪ_t;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ uint16_t h_flag_t;
ਕਿਸਮਪਰਿਭਾਸ਼ਾ uint16_t ਅੰਕੜੇ_ਮਾਪ_t;

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ  flash_config_t {
	ਸਭ_ਤੋਂ_ਵੱਡਾ_ਖੰਡ = 2 * 1024, // This represents the size need for backing store
	MagicSig     = 0xaa553cc3,
	BlankSig     = 0xffffffff
} flash_config_t;

/* Aligns are done up to 32 bytes, so storing the size padding within 0x001f mask */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ  flash_flags_t {
	ਮਾਪ_ਨਕਾਬ      = 0x001f,
	ਨਕਾਬ_ਇੰਦਰਾਜ     = ~ਮਾਪ_ਨਕਾਬ,
	Blank_ਇੰਦਰਾਜ    = (h_flag_t)BlankSig,
	Valid_ਇੰਦਰਾਜ    = (0xa5ac & ~ਮਾਪ_ਨਕਾਬ),
	Erased_ਇੰਦਰਾਜ   = 0x0000,
} flash_flags_t;

/* The struct flash_entry_header_t will be 32-byte aligned
 * The Size will be the actual length of the header plus the data
 * and any padding needed to have the size be an even multiple of
 * the 32-byte flash line
 */
#ਹਦਾਇਤ GCC diagnostic ਧੱਕੋ
#ਹਦਾਇਤ GCC diagnostic ignored "-Wattributes"
ਕਿਸਮਪਰਿਭਾਸ਼ਾ begin_packed_struct ਬਣਤਰ flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t {
	h_magic_t            magic;           /* Used to ID files*/
	h_flag_t             flag;            /* Used to mark this entry as valid */
	uint8_t              ਅਣਵਰਤਿਆ_ਬਫਰ[32 - ਆਕਾਰ(h_magic_t) - ਆਕਾਰ(h_flag_t)]; /* padding */
	h_flag_t             flag_erased;     /* Used to erase this entry */
	uint8_t              ਅਣਵਰਤਿਆ_buffer2[32 - ਆਕਾਰ(h_flag_t)]; /* padding */
	uint32_t             ਚੱਕਰੀ_ਜਾਂਚ;             /* Calculated over the size - end of data */
	h_ਮਾਪ_t             ਮਾਪ;            /* When added to a byte pointer to flash_entry_header_t
                                               * Will result the offset of the next active file or
                                               * free space. */
	flash_file_token_t   file_token;      /* file token type - essentially the name/type */
} ਸਿਰਾ_packed_struct flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t __attribute__((aligned(ਆਕਾਰ(uint32_t))));
#ਹਦਾਇਤ GCC diagnostic ਕੱਢੋ

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/
ਸਥਿਰ uint8_t *working_ਬਫਰ;
ਸਥਿਰ size_t working_ਬਫਰ_ਮਾਪ;
ਸਥਿਰ bool working_ਬਫਰ_static;
ਸਥਿਰ sector_descriptor_t *sector_map;

/****************************************************************************
 * Public Data
 ****************************************************************************/

ਅਚਲ flash_file_token_t parameters_token = {
	.n = {'p', 'l', 'o', 'g'},
};

ਅਚਲ flash_file_token_t parameters_legacy_token = {
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

ਖਾਲੀ parameter_flashfs_free(ਖਾਲੀ)
{
	ਜੇ (!working_ਬਫਰ_static && working_ਬਫਰ != ਨਿਰਾ) {
		ਮੁਕਤ(working_ਬਫਰ);
		working_ਬਫਰ = ਨਿਰਾ;
		working_ਬਫਰ_ਮਾਪ = 0;
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

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ blank_flash(uint32_t *pf)
{
	ਵਾਪਸ *pf == BlankSig;
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

ਸਥਿਰ bool blank_check(flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t *pf,
			size_t new_ਮਾਪ)
{
	bool rv = true;
	uint32_t *pm = (uint32_t *) pf;
	new_ਮਾਪ /= ਆਕਾਰ(uint32_t);

	ਜਦੋਂ (new_ਮਾਪ-- && rv) {
		ਜੇ (!blank_flash(pm++)) {
			rv = false;
		}
	}

	ਵਾਪਸ rv;
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

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ valid_magic(h_magic_t *pm)
{
	ਵਾਪਸ *pm == MagicSig;
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

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ blank_ਇੰਦਰਾਜ(flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t *fi)
{
	ਵਾਪਸ fi->magic == BlankSig && fi->flag == Blank_ਇੰਦਰਾਜ && fi->flag_erased == Blank_ਇੰਦਰਾਜ;
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

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ valid_ਇੰਦਰਾਜ(flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t *fi)
{
	ਵਾਪਸ (fi->flag & ਨਕਾਬ_ਇੰਦਰਾਜ) == Valid_ਇੰਦਰਾਜ && (fi->flag_erased) == Blank_ਇੰਦਰਾਜ;
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

ਸਥਿਰ ਸਤਰਅੰਦਰ ਪੂਰਨ ਇੰਦਰਾਜ_ਮਾਪ_adjust(flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t *fi)
{
	ਵਾਪਸ fi->flag & ਮਾਪ_ਨਕਾਬ;
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

ਸਥਿਰ ਸਤਰਅੰਦਰ flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t *ਅਗਲਾ_ਇੰਦਰਾਜ(flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t *fi)
{
	uint8_t *pb = (uint8_t *)fi;
	ਵਾਪਸ (flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t *)(pb + fi->ਮਾਪ);
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

ਸਥਿਰ ਸਤਰਅੰਦਰ uint8_t *ਇੰਦਰਾਜ_ਅੰਕੜੇ(flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t *fi)
{
	ਵਾਪਸ ((uint8_t *)fi) + ਆਕਾਰ(flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t);
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

ਸਥਿਰ ਸਤਰਅੰਦਰ ਅੰਕੜੇ_ਮਾਪ_t ਇੰਦਰਾਜ_ਅੰਕੜੇ_ਲੰਮਾਈ(flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t *fi)
{
	ਵਾਪਸ fi->ਮਾਪ - (ਆਕਾਰ(flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t) + ਇੰਦਰਾਜ_ਮਾਪ_adjust(fi));
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

ਸਥਿਰ ਸਤਰਅੰਦਰ ਅਚਲ uint8_t *ਇੰਦਰਾਜ_ਚੱਕਰੀ_ਜਾਂਚ_ਸ਼ੁਰੂ(flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t *fi)
{
	ਵਾਪਸ (ਅਚਲ uint8_t *)&fi->ਮਾਪ;
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

ਸਥਿਰ ਸਤਰਅੰਦਰ ਅੰਕੜੇ_ਮਾਪ_t ਇੰਦਰਾਜ_ਚੱਕਰੀ_ਜਾਂਚ_ਲੰਮਾਈ(flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t *fi)
{
	ਵਾਪਸ fi->ਮਾਪ - offsetof(flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t, ਮਾਪ);
}

/* The parameter sector can be the last one on the chip, so no header
 * field may be read until the whole header is known to lie inside it. */
ਸਥਿਰ ਸਤਰਅੰਦਰ bool ਸਿਰਲੇਖ_ਸਮਾਉਂਦਾ(flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t *pf, h_magic_t *pe)
{
	ਵਾਪਸ pf + 1 <= (flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t *)pe;
}

ਸਥਿਰ bool ਚੱਕਰੀ_ਜਾਂਚ_ਸਿਰਲੇਖ_ok(flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t *pf, h_magic_t *pe)
{
	ਜੇ (!ਸਿਰਲੇਖ_ਸਮਾਉਂਦਾ(pf, pe)) {
		ਵਾਪਸ false;
	}

	ਜੇ (pf->ਮਾਪ < ਆਕਾਰ(flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t)) {
		ਵਾਪਸ false;
	}

	ਅਚਲ uint8_t *ਚੱਕਰੀ_ਜਾਂਚ_ਸ਼ੁਰੂ = ਇੰਦਰਾਜ_ਚੱਕਰੀ_ਜਾਂਚ_ਸ਼ੁਰੂ(pf);
	ਅੰਕੜੇ_ਮਾਪ_t ਚੱਕਰੀ_ਜਾਂਚ_ਲੰਮਾਈ = ਇੰਦਰਾਜ_ਚੱਕਰੀ_ਜਾਂਚ_ਲੰਮਾਈ(pf);

	ਜੇ (ਚੱਕਰੀ_ਜਾਂਚ_ਸ਼ੁਰੂ + ਚੱਕਰੀ_ਜਾਂਚ_ਲੰਮਾਈ > (uint8_t *)pe) {
		ਵਾਪਸ false;
	}

	ਵਾਪਸ pf->ਚੱਕਰੀ_ਜਾਂਚ == ਚੱਕਰੀ_ਜਾਂਚ_32(ਚੱਕਰੀ_ਜਾਂਚ_ਸ਼ੁਰੂ, ਚੱਕਰੀ_ਜਾਂਚ_ਲੰਮਾਈ);
}

/* The walkers stop at the last word of a sector and reject a record that
 * ends beyond it, so nothing may be written into that word. */
ਸਥਿਰ ਸਤਰਅੰਦਰ size_t sector_usable(ਪੂਰਨ s)
{
	ਵਾਪਸ sector_map[s].ਮਾਪ - ਆਕਾਰ(h_magic_t);
}

ਸਥਿਰ bool ਥਾਂ_is_free(flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t *pf, ਪੂਰਨ s, size_t required)
{
	uint8_t *begin = (uint8_t *)sector_map[s].ਪਤਾ;
	uint8_t *ਸਿਰਾ = begin + sector_usable(s);

	ਜੇ ((uint8_t *)pf < begin || required == 0 || (uint8_t *)pf + required > ਸਿਰਾ) {
		ਵਾਪਸ false;
	}

	/* Entries start on a flash row. up_progmem_write only asserts that in
	 * debug builds, so a slot found by the word-wise scan behind a torn
	 * record would otherwise be programmed misaligned. */
	ਜੇ (((uintptr_t)pf & ਮਾਪ_ਨਕਾਬ) != 0) {
		ਵਾਪਸ false;
	}

	ਵਾਪਸ blank_ਇੰਦਰਾਜ(pf) && blank_check(pf, required);
}

ਸਥਿਰ ਪੂਰਨ for_each_valid_ਇੰਦਰਾਜ(flash_file_token_t token,
				ਪੂਰਨ (*cb)(flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t *pf, ਖਾਲੀ *arg),
				ਖਾਲੀ *arg);

ਸਥਿਰ ਪੂਰਨ stop_at_ਪਹਿਲਾ(flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t *pf, ਖਾਲੀ *arg)
{
	*(flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t **)arg = pf;
	ਵਾਪਸ -ECANCELED;
}

/* Oldest valid entry for token, or NULL. */
ਸਥਿਰ flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t *ਲੱਭੋ_ਇੰਦਰਾਜ(flash_file_token_t token)
{
	flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t *pf = ਨਿਰਾ;
	for_each_valid_ਇੰਦਰਾਜ(token, stop_at_ਪਹਿਲਾ, &pf);
	ਵਾਪਸ pf;
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

ਸਥਿਰ flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t *ਲੱਭੋ_append(size_t required)
{
	ਜੇ (sector_map == ਨਿਰਾ || required == 0) {
		ਵਾਪਸ ਨਿਰਾ;
	}

	flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t *ਆਖਰੀ = ਨਿਰਾ;
	ਪੂਰਨ ਆਖਰੀ_s = -1;

	ਲਈ (ਪੂਰਨ s = 0; sector_map[s].ਪਤਾ; s++) {
		h_magic_t *pmagic = (h_magic_t *) sector_map[s].ਪਤਾ;
		h_magic_t *pe = pmagic + (sector_map[s].ਮਾਪ / ਆਕਾਰ(h_magic_t)) - 1;

		ਜਦੋਂ (pmagic < pe) {
			ਜੇ (!valid_magic(pmagic)) {
				pmagic++;
				ਜਾਰੀ;
			}

			flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t *pf = (flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t *) pmagic;

			ਜੇ (ਚੱਕਰੀ_ਜਾਂਚ_ਸਿਰਲੇਖ_ok(pf, pe)) {
				ਆਖਰੀ = pf;
				ਆਖਰੀ_s = s;
				pmagic = (h_magic_t *) ਅਗਲਾ_ਇੰਦਰਾਜ(pf);
				pf = (flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t *) pmagic;

				/* Same stop rule as for_each_valid_entry: a record behind a
				 * blank header is invisible to replay, so it must not anchor
				 * the append point either. */
				ਜੇ (pmagic >= pe || !ਸਿਰਲੇਖ_ਸਮਾਉਂਦਾ(pf, pe) || blank_ਇੰਦਰਾਜ(pf)) {
					ਰੋਕੋ;
				}

			} ਨਹੀਂਤਾਂ {
				pmagic++;
			}
		}
	}

	ਅਚਲ ਪੂਰਨ ਸ਼ੁਰੂ_s = (ਆਖਰੀ == ਨਿਰਾ) ? 0 : ਆਖਰੀ_s;
	h_magic_t *pmagic = (ਆਖਰੀ == ਨਿਰਾ)
			    ? (h_magic_t *) sector_map[0].ਪਤਾ
			    : (h_magic_t *) ਅਗਲਾ_ਇੰਦਰਾਜ(ਆਖਰੀ);

	ਲਈ (ਪੂਰਨ s = ਸ਼ੁਰੂ_s; sector_map[s].ਪਤਾ; s++) {
		ਜੇ (s != ਸ਼ੁਰੂ_s) {
			pmagic = (h_magic_t *) sector_map[s].ਪਤਾ;
		}

		h_magic_t *pe = (h_magic_t *) sector_map[s].ਪਤਾ
				+ (sector_map[s].ਮਾਪ / ਆਕਾਰ(h_magic_t)) - 1;

		ਜੇ ((uint8_t *)pmagic < (uint8_t *)sector_map[s].ਪਤਾ
		    || (uint8_t *)pmagic >= (uint8_t *)sector_map[s].ਪਤਾ + sector_map[s].ਮਾਪ) {
			ਜੇ (s == ਸ਼ੁਰੂ_s) {
				ਜਾਰੀ;
			}

			pmagic = (h_magic_t *) sector_map[s].ਪਤਾ;
		}

		ਜਦੋਂ (pmagic < pe) {
			flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t *pf = (flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t *) pmagic;

			ਜੇ (valid_magic(pmagic) && ਚੱਕਰੀ_ਜਾਂਚ_ਸਿਰਲੇਖ_ok(pf, pe)) {
				pmagic = (h_magic_t *) ਅਗਲਾ_ਇੰਦਰਾਜ(pf);
				ਜਾਰੀ;
			}

			ਜੇ (ਥਾਂ_is_free(pf, s, required)) {
				ਵਾਪਸ pf;
			}

			pmagic++;
		}
	}

	ਵਾਪਸ ਨਿਰਾ;
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

ਸਥਿਰ ਪੂਰਨ for_each_valid_ਇੰਦਰਾਜ(flash_file_token_t token,
				ਪੂਰਨ (*cb)(flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t *pf, ਖਾਲੀ *arg),
				ਖਾਲੀ *arg)
{
	ਪੂਰਨ n = 0;

	ਜੇ (sector_map == ਨਿਰਾ) {
		ਵਾਪਸ -ENXIO;
	}

	ਲਈ (ਪੂਰਨ s = 0; sector_map[s].ਪਤਾ; s++) {

		h_magic_t *pmagic = (h_magic_t *) sector_map[s].ਪਤਾ;
		h_magic_t *pe = pmagic + (sector_map[s].ਮਾਪ / ਆਕਾਰ(h_magic_t)) - 1;

cont:

		ਜਦੋਂ (pmagic < pe && !valid_magic(pmagic)) {
			pmagic++;
		}

		ਜੇ (pmagic >= pe) { ਜਾਰੀ; }

		flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t *pf = (flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t *) pmagic;

		ਜੇ (ਚੱਕਰੀ_ਜਾਂਚ_ਸਿਰਲੇਖ_ok(pf, pe)) {

			ਜੇ (valid_ਇੰਦਰਾਜ(pf) && pf->file_token.t == token.t) {
				ਜੇ (cb != ਨਿਰਾ) {
					ਪੂਰਨ rv = cb(pf, arg);

					ਜੇ (rv < 0) {
						ਵਾਪਸ rv;
					}
				}

				n++;
			}

			pf = ਅਗਲਾ_ਇੰਦਰਾਜ(pf);
			pmagic = (h_magic_t *) pf;

			ਜੇ (pmagic >= pe || !ਸਿਰਲੇਖ_ਸਮਾਉਂਦਾ(pf, pe) || blank_ਇੰਦਰਾਜ(pf)) {
				ਜਾਰੀ;
			}

		} ਨਹੀਂਤਾਂ {
			pmagic++;
		}

		ਜਾਓ cont;
	}

	ਵਾਪਸ n;
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

ਸਥਿਰ ਪੂਰਨ erase_sector(sector_descriptor_t *sm, flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t *pf)
{
	ਪੂਰਨ rv = 0;

	/*
	* Because up_progmem interface doesn't allow one-to-one check of
	* the block number for an address, use uniform-memory assumption
	* to confirm the block number in the sector descriptor is
	* the correct block to erase for the given address.
	*
	* This is an optional sanity check against the sector configuration
	*/
	ਜੇ (ਉੱਪਰ_progmem_isuniform()) {
		ssize_t ਪੰਨਾ = ਉੱਪਰ_progmem_getpage((size_t)pf);

		ਜੇ (ਪੰਨਾ < 0) {
			rv = ਪੰਨਾ;
		}

		ਜੇ (rv == 0) {
			size_t ਪੰਨਾ_ਮਾਪ  = ਉੱਪਰ_progmem_pagesize(ਪੰਨਾ);
			size_t ਖੰਡ_ਮਾਪ = ਉੱਪਰ_progmem_erasesize(sm->ਪੰਨਾ);
			size_t ਖੰਡ_ਅੰਕ  = ((ਪੰਨਾ * ਪੰਨਾ_ਮਾਪ) / ਖੰਡ_ਮਾਪ);

			ਜੇ (ਖੰਡ_ਅੰਕ != sm->ਪੰਨਾ) {
				rv = -EINVAL;
			}
		}
	}

	ਜੇ (rv == 0) {

		ssize_t ਮਾਪ_erased = ਉੱਪਰ_progmem_eraseblock(sm->ਪੰਨਾ);

		ਜੇ (ਮਾਪ_erased < 0 || ਮਾਪ_erased != (ssize_t)sm->ਮਾਪ) {
			rv = ਮਾਪ_erased;
		}

	}

	ਵਾਪਸ rv;
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

ਸਥਿਰ ਪੂਰਨ write_flash_ਇੰਦਰਾਜ(size_t pf, ਅਚਲ flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t *pn)
{
	ssize_t rv;
	size_t ਸਤਰ_ਮਾਪ = ਉੱਪਰ_progmem_pagesize(ਉੱਪਰ_progmem_getpage(pf));
	uint8_t *ਪਹਿਲਾ_flag_ਸਤਰ  = (uint8_t *)pn;
	uint8_t *content_ਬਫਰ   = (uint8_t *)(&pn->ਚੱਕਰੀ_ਜਾਂਚ);

	/* write first line (magic, flag) */
	rv = ਉੱਪਰ_progmem_write(pf, ਪਹਿਲਾ_flag_ਸਤਰ, ਸਤਰ_ਮਾਪ);

	ਜੇ (rv < 0) {
		ਵਾਪਸ rv;
	}

	/* pretend to write the second line (flag_erased) */
	rv += ਸਤਰ_ਮਾਪ;

	/* write the remaining contents */
	ssize_t rv2 = ਉੱਪਰ_progmem_write(pf + 2 * ਸਤਰ_ਮਾਪ, content_ਬਫਰ, pn->ਮਾਪ - 2 * ਸਤਰ_ਮਾਪ);

	ਜੇ (rv2 < 0) {
		ਵਾਪਸ rv2;
	}

	ਵਾਪਸ rv + rv2;
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

ਪੂਰਨ parameter_flashfs_read(flash_file_token_t token, uint8_t **ਬਫਰ_2, size_t
			   *buf_ਮਾਪ)
{
	ਪੂਰਨ rv = -ENXIO;

	ਜੇ (sector_map) {

		rv = -ENOENT;
		flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t *pf = ਲੱਭੋ_ਇੰਦਰਾਜ(token);

		ਜੇ (pf) {
			(*ਬਫਰ_2) = ਇੰਦਰਾਜ_ਅੰਕੜੇ(pf);
			rv = ਇੰਦਰਾਜ_ਅੰਕੜੇ_ਲੰਮਾਈ(pf);
			*buf_ਮਾਪ = rv;
		}
	}

	ਵਾਪਸ rv;
}

/****************************************************************************
 * Name: parameter_flashfs_blank
 *
 * Description:
 *   Reports whether the parameter storage is fully erased, so callers can
 *   tell a blank store from one that holds unparseable data.
 *
 ****************************************************************************/

ਪੂਰਨ parameter_flashfs_blank(ਖਾਲੀ)
{
	ਜੇ (sector_map == ਨਿਰਾ) {
		ਵਾਪਸ -ENXIO;
	}

	ਲਈ (ਪੂਰਨ s = 0; sector_map[s].ਪਤਾ; s++) {
		uint32_t *pm = (uint32_t *) sector_map[s].ਪਤਾ;
		uint32_t *pe = pm + (sector_map[s].ਮਾਪ / ਆਕਾਰ(uint32_t));

		ਜਦੋਂ (pm < pe) {
			ਜੇ (!blank_flash(pm++)) {
				ਵਾਪਸ 0;
			}
		}
	}

	ਵਾਪਸ 1;
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

ਪੂਰਨ
parameter_flashfs_write(flash_file_token_t token, uint8_t *ਬਫਰ_2, size_t buf_ਮਾਪ)
{
	ਪੂਰਨ rv = -ENXIO;

	ਜੇ (sector_map) {

		rv = 0;

		/* Calculate the total space needed */
		size_t ਕੁੱਲ_ਮਾਪ = buf_ਮਾਪ + ਆਕਾਰ(flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t);
		size_t alignment = ਮਾਪ_ਨਕਾਬ;///< 32-byte flash row
		size_t  ਮਾਪ_adjust = ((ਕੁੱਲ_ਮਾਪ + alignment) & ~alignment) - ਕੁੱਲ_ਮਾਪ;
		ਕੁੱਲ_ਮਾਪ += ਮਾਪ_adjust;

		ਜੇ (ਕੁੱਲ_ਮਾਪ > UINT16_MAX) {
			ਵਾਪਸ -EFBIG;
		}

		/* Append after the last CRC-valid record. Compaction is the caller's job. */
		flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t *pf = ਲੱਭੋ_append(ਕੁੱਲ_ਮਾਪ);

		ਜੇ (pf == 0) {
			ਵਾਪਸ -ENOSPC;
		}

		flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t *pn = (flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t *)(ਬਫਰ_2 - ਆਕਾਰ(flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t));
		pn->magic = MagicSig;
		pn->file_token.t = token.t;
		pn->flag = Valid_ਇੰਦਰਾਜ + ਮਾਪ_adjust;
		pn->ਮਾਪ = ਕੁੱਲ_ਮਾਪ;

		ਲਈ (size_t a = 0; a < ਮਾਪ_adjust; a++) {
			ਬਫਰ_2[buf_ਮਾਪ + a] = (uint8_t)BlankSig;
		}

		pn->ਚੱਕਰੀ_ਜਾਂਚ = ਚੱਕਰੀ_ਜਾਂਚ_32(ਇੰਦਰਾਜ_ਚੱਕਰੀ_ਜਾਂਚ_ਸ਼ੁਰੂ(pn), ਇੰਦਰਾਜ_ਚੱਕਰੀ_ਜਾਂਚ_ਲੰਮਾਈ(pn));
		rv = write_flash_ਇੰਦਰਾਜ((size_t)pf, pn);
		ਪੂਰਨ system_ਬਾਈਟਾਂ = (ਆਕਾਰ(flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t) + ਮਾਪ_adjust);

		ਜੇ (rv >= system_ਬਾਈਟਾਂ) {
			rv -= system_ਬਾਈਟਾਂ;
		}
	}

	ਵਾਪਸ rv;
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

ਪੂਰਨ parameter_flashfs_alloc(flash_file_token_t token, uint8_t **ਬਫਰ_2, size_t *buf_ਮਾਪ)
{
	ਪੂਰਨ rv = -ENXIO;

	ਜੇ (sector_map) {

		rv = -ENOMEM;

		ਜੇ (!working_ਬਫਰ_static) {

			working_ਬਫਰ_ਮਾਪ = *buf_ਮਾਪ + ਆਕਾਰ(flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t) + ਮਾਪ_ਨਕਾਬ;
			working_ਬਫਰ = ਸਮ੍ਰਿਤੀ_ਦਿਓ(working_ਬਫਰ_ਮਾਪ);

		}

		/* Allocation failed or not provided */

		ਜੇ (working_ਬਫਰ == ਨਿਰਾ) {

			working_ਬਫਰ_ਮਾਪ = 0;

		} ਨਹੀਂਤਾਂ {

			/* We have a buffer reserve space and init it */
			*ਬਫਰ_2 = &working_ਬਫਰ[ਆਕਾਰ(flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t)];
			*buf_ਮਾਪ = working_ਬਫਰ_ਮਾਪ - ਆਕਾਰ(flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t) - ਮਾਪ_ਨਕਾਬ;
			memset(working_ਬਫਰ, 0xff, working_ਬਫਰ_ਮਾਪ);
			rv = 0;

		}
	}

	ਵਾਪਸ rv;
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

ਪੂਰਨ parameter_flashfs_erase(ਖਾਲੀ)
{
	ਪੂਰਨ rv = -ENXIO;

	ਜੇ (sector_map) {
		rv = 0;

		ਲਈ (ਪੂਰਨ s = 0; sector_map[s].ਪਤਾ; s++) {
			ਪੂਰਨ sz = erase_sector(&sector_map[s], (flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t *)sector_map[s].ਪਤਾ);

			ਜੇ (sz != 0) {
				ਵਾਪਸ sz;
			}

			rv += sector_map[s].ਮਾਪ;
		}
	}

	ਵਾਪਸ rv;
}


/****************************************************************************
 * Name: parameter_flashfs_walk
 ****************************************************************************/

ਬਣਤਰ walk_pack {
	parameter_flashfs_walk_cb cb;
	ਖਾਲੀ *arg;
};

ਸਥਿਰ ਪੂਰਨ walk_one(flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t *pf, ਖਾਲੀ *arg)
{
	ਬਣਤਰ walk_pack *w = (ਬਣਤਰ walk_pack *)arg;
	ਵਾਪਸ w->cb(ਇੰਦਰਾਜ_ਅੰਕੜੇ(pf), ਇੰਦਰਾਜ_ਅੰਕੜੇ_ਲੰਮਾਈ(pf), w->arg);
}

ਪੂਰਨ parameter_flashfs_walk(flash_file_token_t token, parameter_flashfs_walk_cb cb, ਖਾਲੀ *arg)
{
	ਬਣਤਰ walk_pack w = { cb, arg };
	ਵਾਪਸ for_each_valid_ਇੰਦਰਾਜ(token, walk_one, &w);
}

/****************************************************************************
 * Name: parameter_flashfs_needs_compact
 ****************************************************************************/

size_t parameter_flashfs_max_payload(ਖਾਲੀ)
{
	ਜੇ (sector_map == ਨਿਰਾ) {
		ਵਾਪਸ 0;
	}

	size_t max_ਕੁੱਲ = 0;

	ਲਈ (ਪੂਰਨ s = 0; sector_map[s].ਪਤਾ; s++) {
		ਜੇ (sector_usable(s) > max_ਕੁੱਲ) {
			max_ਕੁੱਲ = sector_usable(s);
		}
	}

	/* h_size_t holds the record size */
	ਜੇ (max_ਕੁੱਲ > UINT16_MAX) {
		max_ਕੁੱਲ = UINT16_MAX;
	}

	max_ਕੁੱਲ &= ~(size_t)ਮਾਪ_ਨਕਾਬ;

	ਜੇ (max_ਕੁੱਲ <= ਆਕਾਰ(flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t)) {
		ਵਾਪਸ 0;
	}

	ਵਾਪਸ max_ਕੁੱਲ - ਆਕਾਰ(flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t);
}

ਪੂਰਨ parameter_flashfs_needs_compact(flash_file_token_t token, size_t snapshot_ਮਾਪ)
{
	ਜੇ (sector_map == ਨਿਰਾ) {
		ਵਾਪਸ -ENXIO;
	}

	flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t *ਪਹਿਲਾ = ਲੱਭੋ_ਇੰਦਰਾਜ(token);

	ਜੇ (ਪਹਿਲਾ == ਨਿਰਾ) {
		ਵਾਪਸ 0;
	}

	/* Dead records ahead of the live data (a torn write, or the erased
	 * prefix the single-snapshot writer left) only go away with an erase. */
	ਜੇ ((uintptr_t)ਪਹਿਲਾ != (uintptr_t)sector_map[0].ਪਤਾ) {
		ਵਾਪਸ 1;
	}

	/* Keep room after the log for a burst the size of the snapshot
	 * (reset-all tombstones, airframe PARAM_SET), floored so a UUID append
	 * does not force a boot erase. */
	ਅਚਲ size_t align = ਮਾਪ_ਨਕਾਬ;
	ਅਚਲ size_t snapshot_ਕੁੱਲ = (snapshot_ਮਾਪ + ਆਕਾਰ(flash_ਇੰਦਰਾਜ_ਸਿਰਲੇਖ_t) + align) & ~align;
	ਅਚਲ size_t headroom = snapshot_ਕੁੱਲ > 8192 ? snapshot_ਕੁੱਲ : 8192;

	ਜੇ (ਲੱਭੋ_append(headroom) != ਨਿਰਾ) {
		ਵਾਪਸ 0;
	}

	/* Compaction leaves the snapshot at the start of sector 0 and every
	 * other sector blank. Erase only if that layout has the room this one
	 * lacks, or every boot would pay the erase for nothing. */
	ਜੇ (sector_usable(0) >= snapshot_ਕੁੱਲ + headroom) {
		ਵਾਪਸ 1;
	}

	ਲਈ (ਪੂਰਨ s = 1; sector_map[s].ਪਤਾ; s++) {
		ਜੇ (sector_usable(s) >= headroom) {
			ਵਾਪਸ 1;
		}
	}

	ਵਾਪਸ 0;
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

ਪੂਰਨ parameter_flashfs_init(sector_descriptor_t *fconfig, uint8_t *ਬਫਰ_2, uint16_t ਮਾਪ)
{
	ਪੂਰਨ rv = 0;
	sector_map = fconfig;
	working_ਬਫਰ_static = ਬਫਰ_2 != ਨਿਰਾ;

	ਜੇ (!working_ਬਫਰ_static) {
		ਮਾਪ = 0;
	}

	working_ਬਫਰ = ਬਫਰ_2;
	working_ਬਫਰ_ਮਾਪ = ਮਾਪ;

	/* Sanity check */

	/* A non-blank store with no valid entry is torn or foreign; import
	 * reports -EILSEQ. Erasing here would hide a successful retry after
	 * a torn compact write. */

	ਵਾਪਸ rv;
}

#ਜੇ defined(FLASH_UNIT_TEST)

ਸਥਿਰ sector_descriptor_t  test_sector_map[] = {
	{14, 128 * 1024, 0x081c0000},
	{15, 128 * 1024, 0x081e0000},
	{0, 0, 0},
};

__EXPORT ਖਾਲੀ test(ਖਾਲੀ);

uint8_t test_buf[32 * 1024];

__EXPORT ਖਾਲੀ test(ਖਾਲੀ)
{
	uint16_t ਸਭ_ਤੋਂ_ਵੱਡਾ_ਖੰਡ_2 = (32 * 1024) + 96;
	uint8_t *ਬਫਰ_2 = ਸਮ੍ਰਿਤੀ_ਦਿਓ(ਸਭ_ਤੋਂ_ਵੱਡਾ_ਖੰਡ_2);

	parameter_flashfs_init(test_sector_map, ਬਫਰ_2, ਸਭ_ਤੋਂ_ਵੱਡਾ_ਖੰਡ_2);

	ਲਈ (ਪੂਰਨ t = 0; t < ਆਕਾਰ(test_buf); t++) {
		test_buf[t] = (uint8_t) t;
	}

	ਪੂਰਨ er = parameter_flashfs_erase();
	uint8_t *fbuffer;
	size_t buf_ਮਾਪ;
	ਪੂਰਨ written = 0;
	ਪੂਰਨ ਪਠ = 0;
	ਪੂਰਨ rv = 0;

	ਲਈ (ਪੂਰਨ a = 0; a <= 4; a++) {
		parameter_flashfs_erase();
		rv =  parameter_flashfs_alloc(parameters_token, &fbuffer, &buf_ਮਾਪ);
		ਸਮ੍ਰਿਤੀ_ਨਕਲ(fbuffer, test_buf, a);
		buf_ਮਾਪ = a;
		written = parameter_flashfs_write(parameters_token, fbuffer, buf_ਮਾਪ);
		ਪਠ = parameter_flashfs_read(parameters_token, &fbuffer, &buf_ਮਾਪ);
		parameter_flashfs_free();

		ਜੇ (ਪਠ != written) {
			ਸਥਿਰ ਅਸਥਿਰ ਪੂਰਨ j;
			j++;
		}
	}

	ਪੂਰਨ ਖੰਡ_2 = 2048;

	ਲਈ (ਪੂਰਨ a = 0; a <= 8; a++) {
		parameter_flashfs_erase();
		rv =  parameter_flashfs_alloc(parameters_token, &fbuffer, &buf_ਮਾਪ);
		ਸਮ੍ਰਿਤੀ_ਨਕਲ(fbuffer, test_buf, ਖੰਡ_2);
		buf_ਮਾਪ = ਖੰਡ_2;
		written = parameter_flashfs_write(parameters_token, fbuffer, buf_ਮਾਪ);
		ਪਠ = parameter_flashfs_read(parameters_token, &fbuffer, &buf_ਮਾਪ);
		parameter_flashfs_free();

		ਜੇ (ਪਠ != written) {
			ਸਥਿਰ ਅਸਥਿਰ ਪੂਰਨ j;
			j++;
		}

		ਖੰਡ_2 += 2048;
	}

	rv++;
	er++;
	ਮੁਕਤ(ਬਫਰ_2);
}
#ਜੇ_ਅੰਤ /* FLASH_UNIT_TEST */
#ਜੇ_ਅੰਤ /* CONFIG_ARCH_HAVE_PROGMEM */
