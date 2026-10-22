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

#ಅನ್ತರ್ಭಾವ <px4_platform_common/defines.h>
#ಅನ್ತರ್ಭಾವ <px4_platform_common/posix.h>
#ಅನ್ತರ್ಭಾವ <nuttx/crc32.h>
#ಅನ್ತರ್ಭಾವ <stddef.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <stdbool.h>
#ಅನ್ತರ್ಭಾವ <stdint.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <errno.h>
#ಅನ್ತರ್ಭಾವ "flashfs.h"
#ಅನ್ತರ್ಭಾವ <nuttx/compiler.h>
#ಅನ್ತರ್ಭಾವ <nuttx/progmem.h>
#ಅನ್ತರ್ಭಾವ <board_config.h>

#ಆದರೆ defined(CONFIG_ARCH_HAVE_PROGMEM)

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Private Types
 ****************************************************************************/

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ uint32_t h_magic_t;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ uint16_t h_ಗಾತ್ರ_t;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ uint16_t h_flag_t;
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ uint16_t ದತ್ತಾಂಶ_ಗಾತ್ರ_t;

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ  flash_config_t {
	ಅತಿದೊಡ್ಡ_ಖಂಡ = 2 * 1024, // This represents the size need for backing store
	MagicSig     = 0xaa553cc3,
	BlankSig     = 0xffffffff
} flash_config_t;

/* Aligns are done up to 32 bytes, so storing the size padding within 0x001f mask */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ  flash_flags_t {
	ಗಾತ್ರ_ಮುಖವಾಡ      = 0x001f,
	ಮುಖವಾಡ_ನಮೂದು     = ~ಗಾತ್ರ_ಮುಖವಾಡ,
	Blank_ನಮೂದು    = (h_flag_t)BlankSig,
	Valid_ನಮೂದು    = (0xa5ac & ~ಗಾತ್ರ_ಮುಖವಾಡ),
	Erased_ನಮೂದು   = 0x0000,
} flash_flags_t;

/* The struct flash_entry_header_t will be 32-byte aligned
 * The Size will be the actual length of the header plus the data
 * and any padding needed to have the size be an even multiple of
 * the 32-byte flash line
 */
#ಸೂಚನೆ GCC diagnostic ತಳ್ಳು
#ಸೂಚನೆ GCC diagnostic ignored "-Wattributes"
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ begin_packed_struct ರಚನೆ flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t {
	h_magic_t            magic;           /* Used to ID files*/
	h_flag_t             flag;            /* Used to mark this entry as valid */
	uint8_t              ಬಳಸದ_ಬಫರ್[32 - ಗಾತ್ರ(h_magic_t) - ಗಾತ್ರ(h_flag_t)]; /* padding */
	h_flag_t             flag_erased;     /* Used to erase this entry */
	uint8_t              ಬಳಸದ_buffer2[32 - ಗಾತ್ರ(h_flag_t)]; /* padding */
	uint32_t             ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ;             /* Calculated over the size - end of data */
	h_ಗಾತ್ರ_t             ಗಾತ್ರ_2;            /* When added to a byte pointer to flash_entry_header_t
                                               * Will result the offset of the next active file or
                                               * free space. */
	flash_file_token_t   file_token;      /* file token type - essentially the name/type */
} ಕೊನೆ_packed_struct flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t __attribute__((aligned(ಗಾತ್ರ(uint32_t))));
#ಸೂಚನೆ GCC diagnostic ತೆಗೆ_2

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/
ಸ್ಥಿರ uint8_t *working_ಬಫರ್;
ಸ್ಥಿರ size_t working_ಬಫರ್_ಗಾತ್ರ;
ಸ್ಥಿರ bool working_ಬಫರ್_static;
ಸ್ಥಿರ sector_descriptor_t *sector_map;

/****************************************************************************
 * Public Data
 ****************************************************************************/

ಸ್ಥಿರಾಂಕ flash_file_token_t parameters_token = {
	.n = {'p', 'l', 'o', 'g'},
};

ಸ್ಥಿರಾಂಕ flash_file_token_t parameters_legacy_token = {
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

ಶೂನ್ಯ parameter_flashfs_free(ಶೂನ್ಯ)
{
	ಆದರೆ (!working_ಬಫರ್_static && working_ಬಫರ್ != ಖಾಲಿ) {
		ಬಿಡುಗಡೆ(working_ಬಫರ್);
		working_ಬಫರ್ = ಖಾಲಿ;
		working_ಬಫರ್_ಗಾತ್ರ = 0;
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

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ blank_flash(uint32_t *pf)
{
	ಹಿಂತಿರುಗಿ *pf == BlankSig;
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

ಸ್ಥಿರ bool blank_check(flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t *pf,
			size_t new_ಗಾತ್ರ)
{
	bool rv = true;
	uint32_t *pm = (uint32_t *) pf;
	new_ಗಾತ್ರ /= ಗಾತ್ರ(uint32_t);

	ಆಗ (new_ಗಾತ್ರ-- && rv) {
		ಆದರೆ (!blank_flash(pm++)) {
			rv = false;
		}
	}

	ಹಿಂತಿರುಗಿ rv;
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

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ valid_magic(h_magic_t *pm)
{
	ಹಿಂತಿರುಗಿ *pm == MagicSig;
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

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ blank_ನಮೂದು(flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t *fi)
{
	ಹಿಂತಿರುಗಿ fi->magic == BlankSig && fi->flag == Blank_ನಮೂದು && fi->flag_erased == Blank_ನಮೂದು;
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

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ valid_ನಮೂದು(flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t *fi)
{
	ಹಿಂತಿರುಗಿ (fi->flag & ಮುಖವಾಡ_ನಮೂದು) == Valid_ನಮೂದು && (fi->flag_erased) == Blank_ನಮೂದು;
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

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಪೂರ್ಣಾಂಕ ನಮೂದು_ಗಾತ್ರ_adjust(flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t *fi)
{
	ಹಿಂತಿರುಗಿ fi->flag & ಗಾತ್ರ_ಮುಖವಾಡ;
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

ಸ್ಥಿರ ಸಾಲೊಳಗೆ flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t *ಮುಂದಿನ_ನಮೂದು(flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t *fi)
{
	uint8_t *pb = (uint8_t *)fi;
	ಹಿಂತಿರುಗಿ (flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t *)(pb + fi->ಗಾತ್ರ_2);
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

ಸ್ಥಿರ ಸಾಲೊಳಗೆ uint8_t *ನಮೂದು_ದತ್ತಾಂಶ(flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t *fi)
{
	ಹಿಂತಿರುಗಿ ((uint8_t *)fi) + ಗಾತ್ರ(flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t);
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

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ದತ್ತಾಂಶ_ಗಾತ್ರ_t ನಮೂದು_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ(flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t *fi)
{
	ಹಿಂತಿರುಗಿ fi->ಗಾತ್ರ_2 - (ಗಾತ್ರ(flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t) + ನಮೂದು_ಗಾತ್ರ_adjust(fi));
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

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ಸ್ಥಿರಾಂಕ uint8_t *ನಮೂದು_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_ಆರಂಭ(flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t *fi)
{
	ಹಿಂತಿರುಗಿ (ಸ್ಥಿರಾಂಕ uint8_t *)&fi->ಗಾತ್ರ_2;
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

ಸ್ಥಿರ ಸಾಲೊಳಗೆ ದತ್ತಾಂಶ_ಗಾತ್ರ_t ನಮೂದು_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_ಉದ್ದಳತೆ(flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t *fi)
{
	ಹಿಂತಿರುಗಿ fi->ಗಾತ್ರ_2 - offsetof(flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t, ಗಾತ್ರ_2);
}

/* The parameter sector can be the last one on the chip, so no header
 * field may be read until the whole header is known to lie inside it. */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ bool ಶೀರ್ಷಿಕೆ_ಹೊಂದುತ್ತದೆ(flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t *pf, h_magic_t *pe)
{
	ಹಿಂತಿರುಗಿ pf + 1 <= (flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t *)pe;
}

ಸ್ಥಿರ bool ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_ಶೀರ್ಷಿಕೆ_ok(flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t *pf, h_magic_t *pe)
{
	ಆದರೆ (!ಶೀರ್ಷಿಕೆ_ಹೊಂದುತ್ತದೆ(pf, pe)) {
		ಹಿಂತಿರುಗಿ false;
	}

	ಆದರೆ (pf->ಗಾತ್ರ_2 < ಗಾತ್ರ(flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t)) {
		ಹಿಂತಿರುಗಿ false;
	}

	ಸ್ಥಿರಾಂಕ uint8_t *ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_ಆರಂಭ = ನಮೂದು_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_ಆರಂಭ(pf);
	ದತ್ತಾಂಶ_ಗಾತ್ರ_t ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_ಉದ್ದಳತೆ = ನಮೂದು_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_ಉದ್ದಳತೆ(pf);

	ಆದರೆ (ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_ಆರಂಭ + ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_ಉದ್ದಳತೆ > (uint8_t *)pe) {
		ಹಿಂತಿರುಗಿ false;
	}

	ಹಿಂತಿರುಗಿ pf->ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ == ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_32(ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_ಆರಂಭ, ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_ಉದ್ದಳತೆ);
}

/* The walkers stop at the last word of a sector and reject a record that
 * ends beyond it, so nothing may be written into that word. */
ಸ್ಥಿರ ಸಾಲೊಳಗೆ size_t sector_usable(ಪೂರ್ಣಾಂಕ s)
{
	ಹಿಂತಿರುಗಿ sector_map[s].ಗಾತ್ರ_2 - ಗಾತ್ರ(h_magic_t);
}

ಸ್ಥಿರ bool ಸ್ಥಾನ_is_free(flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t *pf, ಪೂರ್ಣಾಂಕ s, size_t required)
{
	uint8_t *begin = (uint8_t *)sector_map[s].ವಿಳಾಸ;
	uint8_t *ಕೊನೆ = begin + sector_usable(s);

	ಆದರೆ ((uint8_t *)pf < begin || required == 0 || (uint8_t *)pf + required > ಕೊನೆ) {
		ಹಿಂತಿರುಗಿ false;
	}

	/* Entries start on a flash row. up_progmem_write only asserts that in
	 * debug builds, so a slot found by the word-wise scan behind a torn
	 * record would otherwise be programmed misaligned. */
	ಆದರೆ (((uintptr_t)pf & ಗಾತ್ರ_ಮುಖವಾಡ) != 0) {
		ಹಿಂತಿರುಗಿ false;
	}

	ಹಿಂತಿರುಗಿ blank_ನಮೂದು(pf) && blank_check(pf, required);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ for_each_valid_ನಮೂದು(flash_file_token_t token,
				ಪೂರ್ಣಾಂಕ (*cb)(flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t *pf, ಶೂನ್ಯ *arg),
				ಶೂನ್ಯ *arg);

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ stop_at_ಮೊದಲ(flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t *pf, ಶೂನ್ಯ *arg)
{
	*(flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t **)arg = pf;
	ಹಿಂತಿರುಗಿ -ECANCELED;
}

/* Oldest valid entry for token, or NULL. */
ಸ್ಥಿರ flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t *ಕಂಡುಹಿಡಿ_ನಮೂದು(flash_file_token_t token)
{
	flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t *pf = ಖಾಲಿ;
	for_each_valid_ನಮೂದು(token, stop_at_ಮೊದಲ, &pf);
	ಹಿಂತಿರುಗಿ pf;
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

ಸ್ಥಿರ flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t *ಕಂಡುಹಿಡಿ_append(size_t required)
{
	ಆದರೆ (sector_map == ಖಾಲಿ || required == 0) {
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t *ಕೊನೆಯ = ಖಾಲಿ;
	ಪೂರ್ಣಾಂಕ ಕೊನೆಯ_s = -1;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ s = 0; sector_map[s].ವಿಳಾಸ; s++) {
		h_magic_t *pmagic = (h_magic_t *) sector_map[s].ವಿಳಾಸ;
		h_magic_t *pe = pmagic + (sector_map[s].ಗಾತ್ರ_2 / ಗಾತ್ರ(h_magic_t)) - 1;

		ಆಗ (pmagic < pe) {
			ಆದರೆ (!valid_magic(pmagic)) {
				pmagic++;
				ಮುಂದುವರಿ;
			}

			flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t *pf = (flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t *) pmagic;

			ಆದರೆ (ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_ಶೀರ್ಷಿಕೆ_ok(pf, pe)) {
				ಕೊನೆಯ = pf;
				ಕೊನೆಯ_s = s;
				pmagic = (h_magic_t *) ಮುಂದಿನ_ನಮೂದು(pf);
				pf = (flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t *) pmagic;

				/* Same stop rule as for_each_valid_entry: a record behind a
				 * blank header is invisible to replay, so it must not anchor
				 * the append point either. */
				ಆದರೆ (pmagic >= pe || !ಶೀರ್ಷಿಕೆ_ಹೊಂದುತ್ತದೆ(pf, pe) || blank_ನಮೂದು(pf)) {
					ನಿಲ್ಲಿಸು;
				}

			} ಇಲ್ಲದಿದ್ದರೆ {
				pmagic++;
			}
		}
	}

	ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ ಆರಂಭ_s = (ಕೊನೆಯ == ಖಾಲಿ) ? 0 : ಕೊನೆಯ_s;
	h_magic_t *pmagic = (ಕೊನೆಯ == ಖಾಲಿ)
			    ? (h_magic_t *) sector_map[0].ವಿಳಾಸ
			    : (h_magic_t *) ಮುಂದಿನ_ನಮೂದು(ಕೊನೆಯ);

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ s = ಆರಂಭ_s; sector_map[s].ವಿಳಾಸ; s++) {
		ಆದರೆ (s != ಆರಂಭ_s) {
			pmagic = (h_magic_t *) sector_map[s].ವಿಳಾಸ;
		}

		h_magic_t *pe = (h_magic_t *) sector_map[s].ವಿಳಾಸ
				+ (sector_map[s].ಗಾತ್ರ_2 / ಗಾತ್ರ(h_magic_t)) - 1;

		ಆದರೆ ((uint8_t *)pmagic < (uint8_t *)sector_map[s].ವಿಳಾಸ
		    || (uint8_t *)pmagic >= (uint8_t *)sector_map[s].ವಿಳಾಸ + sector_map[s].ಗಾತ್ರ_2) {
			ಆದರೆ (s == ಆರಂಭ_s) {
				ಮುಂದುವರಿ;
			}

			pmagic = (h_magic_t *) sector_map[s].ವಿಳಾಸ;
		}

		ಆಗ (pmagic < pe) {
			flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t *pf = (flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t *) pmagic;

			ಆದರೆ (valid_magic(pmagic) && ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_ಶೀರ್ಷಿಕೆ_ok(pf, pe)) {
				pmagic = (h_magic_t *) ಮುಂದಿನ_ನಮೂದು(pf);
				ಮುಂದುವರಿ;
			}

			ಆದರೆ (ಸ್ಥಾನ_is_free(pf, s, required)) {
				ಹಿಂತಿರುಗಿ pf;
			}

			pmagic++;
		}
	}

	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
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

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ for_each_valid_ನಮೂದು(flash_file_token_t token,
				ಪೂರ್ಣಾಂಕ (*cb)(flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t *pf, ಶೂನ್ಯ *arg),
				ಶೂನ್ಯ *arg)
{
	ಪೂರ್ಣಾಂಕ n = 0;

	ಆದರೆ (sector_map == ಖಾಲಿ) {
		ಹಿಂತಿರುಗಿ -ENXIO;
	}

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ s = 0; sector_map[s].ವಿಳಾಸ; s++) {

		h_magic_t *pmagic = (h_magic_t *) sector_map[s].ವಿಳಾಸ;
		h_magic_t *pe = pmagic + (sector_map[s].ಗಾತ್ರ_2 / ಗಾತ್ರ(h_magic_t)) - 1;

cont:

		ಆಗ (pmagic < pe && !valid_magic(pmagic)) {
			pmagic++;
		}

		ಆದರೆ (pmagic >= pe) { ಮುಂದುವರಿ; }

		flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t *pf = (flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t *) pmagic;

		ಆದರೆ (ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_ಶೀರ್ಷಿಕೆ_ok(pf, pe)) {

			ಆದರೆ (valid_ನಮೂದು(pf) && pf->file_token.t == token.t) {
				ಆದರೆ (cb != ಖಾಲಿ) {
					ಪೂರ್ಣಾಂಕ rv = cb(pf, arg);

					ಆದರೆ (rv < 0) {
						ಹಿಂತಿರುಗಿ rv;
					}
				}

				n++;
			}

			pf = ಮುಂದಿನ_ನಮೂದು(pf);
			pmagic = (h_magic_t *) pf;

			ಆದರೆ (pmagic >= pe || !ಶೀರ್ಷಿಕೆ_ಹೊಂದುತ್ತದೆ(pf, pe) || blank_ನಮೂದು(pf)) {
				ಮುಂದುವರಿ;
			}

		} ಇಲ್ಲದಿದ್ದರೆ {
			pmagic++;
		}

		ಹೋಗು cont;
	}

	ಹಿಂತಿರುಗಿ n;
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

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ erase_sector(sector_descriptor_t *sm, flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t *pf)
{
	ಪೂರ್ಣಾಂಕ rv = 0;

	/*
	* Because up_progmem interface doesn't allow one-to-one check of
	* the block number for an address, use uniform-memory assumption
	* to confirm the block number in the sector descriptor is
	* the correct block to erase for the given address.
	*
	* This is an optional sanity check against the sector configuration
	*/
	ಆದರೆ (ಮೇಲೆ_progmem_isuniform()) {
		ssize_t ಪುಟ = ಮೇಲೆ_progmem_getpage((size_t)pf);

		ಆದರೆ (ಪುಟ < 0) {
			rv = ಪುಟ;
		}

		ಆದರೆ (rv == 0) {
			size_t ಪುಟ_ಗಾತ್ರ  = ಮೇಲೆ_progmem_pagesize(ಪುಟ);
			size_t ಖಂಡ_ಗಾತ್ರ = ಮೇಲೆ_progmem_erasesize(sm->ಪುಟ);
			size_t ಖಂಡ_ಅಂಕೆ  = ((ಪುಟ * ಪುಟ_ಗಾತ್ರ) / ಖಂಡ_ಗಾತ್ರ);

			ಆದರೆ (ಖಂಡ_ಅಂಕೆ != sm->ಪುಟ) {
				rv = -EINVAL;
			}
		}
	}

	ಆದರೆ (rv == 0) {

		ssize_t ಗಾತ್ರ_erased = ಮೇಲೆ_progmem_eraseblock(sm->ಪುಟ);

		ಆದರೆ (ಗಾತ್ರ_erased < 0 || ಗಾತ್ರ_erased != (ssize_t)sm->ಗಾತ್ರ_2) {
			rv = ಗಾತ್ರ_erased;
		}

	}

	ಹಿಂತಿರುಗಿ rv;
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

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ write_flash_ನಮೂದು(size_t pf, ಸ್ಥಿರಾಂಕ flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t *pn)
{
	ssize_t rv;
	size_t ಸಾಲು_ಗಾತ್ರ = ಮೇಲೆ_progmem_pagesize(ಮೇಲೆ_progmem_getpage(pf));
	uint8_t *ಮೊದಲ_flag_ಸಾಲು  = (uint8_t *)pn;
	uint8_t *content_ಬಫರ್   = (uint8_t *)(&pn->ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ);

	/* write first line (magic, flag) */
	rv = ಮೇಲೆ_progmem_write(pf, ಮೊದಲ_flag_ಸಾಲು, ಸಾಲು_ಗಾತ್ರ);

	ಆದರೆ (rv < 0) {
		ಹಿಂತಿರುಗಿ rv;
	}

	/* pretend to write the second line (flag_erased) */
	rv += ಸಾಲು_ಗಾತ್ರ;

	/* write the remaining contents */
	ssize_t rv2 = ಮೇಲೆ_progmem_write(pf + 2 * ಸಾಲು_ಗಾತ್ರ, content_ಬಫರ್, pn->ಗಾತ್ರ_2 - 2 * ಸಾಲು_ಗಾತ್ರ);

	ಆದರೆ (rv2 < 0) {
		ಹಿಂತಿರುಗಿ rv2;
	}

	ಹಿಂತಿರುಗಿ rv + rv2;
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

ಪೂರ್ಣಾಂಕ parameter_flashfs_read(flash_file_token_t token, uint8_t **ಬಫರ್_2, size_t
			   *buf_ಗಾತ್ರ)
{
	ಪೂರ್ಣಾಂಕ rv = -ENXIO;

	ಆದರೆ (sector_map) {

		rv = -ENOENT;
		flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t *pf = ಕಂಡುಹಿಡಿ_ನಮೂದು(token);

		ಆದರೆ (pf) {
			(*ಬಫರ್_2) = ನಮೂದು_ದತ್ತಾಂಶ(pf);
			rv = ನಮೂದು_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ(pf);
			*buf_ಗಾತ್ರ = rv;
		}
	}

	ಹಿಂತಿರುಗಿ rv;
}

/****************************************************************************
 * Name: parameter_flashfs_blank
 *
 * Description:
 *   Reports whether the parameter storage is fully erased, so callers can
 *   tell a blank store from one that holds unparseable data.
 *
 ****************************************************************************/

ಪೂರ್ಣಾಂಕ parameter_flashfs_blank(ಶೂನ್ಯ)
{
	ಆದರೆ (sector_map == ಖಾಲಿ) {
		ಹಿಂತಿರುಗಿ -ENXIO;
	}

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ s = 0; sector_map[s].ವಿಳಾಸ; s++) {
		uint32_t *pm = (uint32_t *) sector_map[s].ವಿಳಾಸ;
		uint32_t *pe = pm + (sector_map[s].ಗಾತ್ರ_2 / ಗಾತ್ರ(uint32_t));

		ಆಗ (pm < pe) {
			ಆದರೆ (!blank_flash(pm++)) {
				ಹಿಂತಿರುಗಿ 0;
			}
		}
	}

	ಹಿಂತಿರುಗಿ 1;
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

ಪೂರ್ಣಾಂಕ
parameter_flashfs_write(flash_file_token_t token, uint8_t *ಬಫರ್_2, size_t buf_ಗಾತ್ರ)
{
	ಪೂರ್ಣಾಂಕ rv = -ENXIO;

	ಆದರೆ (sector_map) {

		rv = 0;

		/* Calculate the total space needed */
		size_t ಒಟ್ಟು_ಗಾತ್ರ = buf_ಗಾತ್ರ + ಗಾತ್ರ(flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t);
		size_t alignment = ಗಾತ್ರ_ಮುಖವಾಡ;///< 32-byte flash row
		size_t  ಗಾತ್ರ_adjust = ((ಒಟ್ಟು_ಗಾತ್ರ + alignment) & ~alignment) - ಒಟ್ಟು_ಗಾತ್ರ;
		ಒಟ್ಟು_ಗಾತ್ರ += ಗಾತ್ರ_adjust;

		ಆದರೆ (ಒಟ್ಟು_ಗಾತ್ರ > UINT16_MAX) {
			ಹಿಂತಿರುಗಿ -EFBIG;
		}

		/* Append after the last CRC-valid record. Compaction is the caller's job. */
		flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t *pf = ಕಂಡುಹಿಡಿ_append(ಒಟ್ಟು_ಗಾತ್ರ);

		ಆದರೆ (pf == 0) {
			ಹಿಂತಿರುಗಿ -ENOSPC;
		}

		flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t *pn = (flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t *)(ಬಫರ್_2 - ಗಾತ್ರ(flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t));
		pn->magic = MagicSig;
		pn->file_token.t = token.t;
		pn->flag = Valid_ನಮೂದು + ಗಾತ್ರ_adjust;
		pn->ಗಾತ್ರ_2 = ಒಟ್ಟು_ಗಾತ್ರ;

		ಗಾಗಿ (size_t a = 0; a < ಗಾತ್ರ_adjust; a++) {
			ಬಫರ್_2[buf_ಗಾತ್ರ + a] = (uint8_t)BlankSig;
		}

		pn->ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ = ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_32(ನಮೂದು_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_ಆರಂಭ(pn), ನಮೂದು_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_ಉದ್ದಳತೆ(pn));
		rv = write_flash_ನಮೂದು((size_t)pf, pn);
		ಪೂರ್ಣಾಂಕ system_ಬೈಟುಗಳು = (ಗಾತ್ರ(flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t) + ಗಾತ್ರ_adjust);

		ಆದರೆ (rv >= system_ಬೈಟುಗಳು) {
			rv -= system_ಬೈಟುಗಳು;
		}
	}

	ಹಿಂತಿರುಗಿ rv;
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

ಪೂರ್ಣಾಂಕ parameter_flashfs_alloc(flash_file_token_t token, uint8_t **ಬಫರ್_2, size_t *buf_ಗಾತ್ರ)
{
	ಪೂರ್ಣಾಂಕ rv = -ENXIO;

	ಆದರೆ (sector_map) {

		rv = -ENOMEM;

		ಆದರೆ (!working_ಬಫರ್_static) {

			working_ಬಫರ್_ಗಾತ್ರ = *buf_ಗಾತ್ರ + ಗಾತ್ರ(flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t) + ಗಾತ್ರ_ಮುಖವಾಡ;
			working_ಬಫರ್ = ಸ್ಮರಣೆ_ಕೊಡು(working_ಬಫರ್_ಗಾತ್ರ);

		}

		/* Allocation failed or not provided */

		ಆದರೆ (working_ಬಫರ್ == ಖಾಲಿ) {

			working_ಬಫರ್_ಗಾತ್ರ = 0;

		} ಇಲ್ಲದಿದ್ದರೆ {

			/* We have a buffer reserve space and init it */
			*ಬಫರ್_2 = &working_ಬಫರ್[ಗಾತ್ರ(flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t)];
			*buf_ಗಾತ್ರ = working_ಬಫರ್_ಗಾತ್ರ - ಗಾತ್ರ(flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t) - ಗಾತ್ರ_ಮುಖವಾಡ;
			memset(working_ಬಫರ್, 0xff, working_ಬಫರ್_ಗಾತ್ರ);
			rv = 0;

		}
	}

	ಹಿಂತಿರುಗಿ rv;
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

ಪೂರ್ಣಾಂಕ parameter_flashfs_erase(ಶೂನ್ಯ)
{
	ಪೂರ್ಣಾಂಕ rv = -ENXIO;

	ಆದರೆ (sector_map) {
		rv = 0;

		ಗಾಗಿ (ಪೂರ್ಣಾಂಕ s = 0; sector_map[s].ವಿಳಾಸ; s++) {
			ಪೂರ್ಣಾಂಕ sz = erase_sector(&sector_map[s], (flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t *)sector_map[s].ವಿಳಾಸ);

			ಆದರೆ (sz != 0) {
				ಹಿಂತಿರುಗಿ sz;
			}

			rv += sector_map[s].ಗಾತ್ರ_2;
		}
	}

	ಹಿಂತಿರುಗಿ rv;
}


/****************************************************************************
 * Name: parameter_flashfs_walk
 ****************************************************************************/

ರಚನೆ walk_pack {
	parameter_flashfs_walk_cb cb;
	ಶೂನ್ಯ *arg;
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ walk_one(flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t *pf, ಶೂನ್ಯ *arg)
{
	ರಚನೆ walk_pack *w = (ರಚನೆ walk_pack *)arg;
	ಹಿಂತಿರುಗಿ w->cb(ನಮೂದು_ದತ್ತಾಂಶ(pf), ನಮೂದು_ದತ್ತಾಂಶ_ಉದ್ದಳತೆ(pf), w->arg);
}

ಪೂರ್ಣಾಂಕ parameter_flashfs_walk(flash_file_token_t token, parameter_flashfs_walk_cb cb, ಶೂನ್ಯ *arg)
{
	ರಚನೆ walk_pack w = { cb, arg };
	ಹಿಂತಿರುಗಿ for_each_valid_ನಮೂದು(token, walk_one, &w);
}

/****************************************************************************
 * Name: parameter_flashfs_needs_compact
 ****************************************************************************/

size_t parameter_flashfs_max_payload(ಶೂನ್ಯ)
{
	ಆದರೆ (sector_map == ಖಾಲಿ) {
		ಹಿಂತಿರುಗಿ 0;
	}

	size_t max_ಒಟ್ಟು = 0;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ s = 0; sector_map[s].ವಿಳಾಸ; s++) {
		ಆದರೆ (sector_usable(s) > max_ಒಟ್ಟು) {
			max_ಒಟ್ಟು = sector_usable(s);
		}
	}

	/* h_size_t holds the record size */
	ಆದರೆ (max_ಒಟ್ಟು > UINT16_MAX) {
		max_ಒಟ್ಟು = UINT16_MAX;
	}

	max_ಒಟ್ಟು &= ~(size_t)ಗಾತ್ರ_ಮುಖವಾಡ;

	ಆದರೆ (max_ಒಟ್ಟು <= ಗಾತ್ರ(flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t)) {
		ಹಿಂತಿರುಗಿ 0;
	}

	ಹಿಂತಿರುಗಿ max_ಒಟ್ಟು - ಗಾತ್ರ(flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t);
}

ಪೂರ್ಣಾಂಕ parameter_flashfs_needs_compact(flash_file_token_t token, size_t snapshot_ಗಾತ್ರ)
{
	ಆದರೆ (sector_map == ಖಾಲಿ) {
		ಹಿಂತಿರುಗಿ -ENXIO;
	}

	flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t *ಮೊದಲ = ಕಂಡುಹಿಡಿ_ನಮೂದು(token);

	ಆದರೆ (ಮೊದಲ == ಖಾಲಿ) {
		ಹಿಂತಿರುಗಿ 0;
	}

	/* Dead records ahead of the live data (a torn write, or the erased
	 * prefix the single-snapshot writer left) only go away with an erase. */
	ಆದರೆ ((uintptr_t)ಮೊದಲ != (uintptr_t)sector_map[0].ವಿಳಾಸ) {
		ಹಿಂತಿರುಗಿ 1;
	}

	/* Keep room after the log for a burst the size of the snapshot
	 * (reset-all tombstones, airframe PARAM_SET), floored so a UUID append
	 * does not force a boot erase. */
	ಸ್ಥಿರಾಂಕ size_t align = ಗಾತ್ರ_ಮುಖವಾಡ;
	ಸ್ಥಿರಾಂಕ size_t snapshot_ಒಟ್ಟು = (snapshot_ಗಾತ್ರ + ಗಾತ್ರ(flash_ನಮೂದು_ಶೀರ್ಷಿಕೆ_t) + align) & ~align;
	ಸ್ಥಿರಾಂಕ size_t headroom = snapshot_ಒಟ್ಟು > 8192 ? snapshot_ಒಟ್ಟು : 8192;

	ಆದರೆ (ಕಂಡುಹಿಡಿ_append(headroom) != ಖಾಲಿ) {
		ಹಿಂತಿರುಗಿ 0;
	}

	/* Compaction leaves the snapshot at the start of sector 0 and every
	 * other sector blank. Erase only if that layout has the room this one
	 * lacks, or every boot would pay the erase for nothing. */
	ಆದರೆ (sector_usable(0) >= snapshot_ಒಟ್ಟು + headroom) {
		ಹಿಂತಿರುಗಿ 1;
	}

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ s = 1; sector_map[s].ವಿಳಾಸ; s++) {
		ಆದರೆ (sector_usable(s) >= headroom) {
			ಹಿಂತಿರುಗಿ 1;
		}
	}

	ಹಿಂತಿರುಗಿ 0;
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

ಪೂರ್ಣಾಂಕ parameter_flashfs_init(sector_descriptor_t *fconfig, uint8_t *ಬಫರ್_2, uint16_t ಗಾತ್ರ_2)
{
	ಪೂರ್ಣಾಂಕ rv = 0;
	sector_map = fconfig;
	working_ಬಫರ್_static = ಬಫರ್_2 != ಖಾಲಿ;

	ಆದರೆ (!working_ಬಫರ್_static) {
		ಗಾತ್ರ_2 = 0;
	}

	working_ಬಫರ್ = ಬಫರ್_2;
	working_ಬಫರ್_ಗಾತ್ರ = ಗಾತ್ರ_2;

	/* Sanity check */

	/* A non-blank store with no valid entry is torn or foreign; import
	 * reports -EILSEQ. Erasing here would hide a successful retry after
	 * a torn compact write. */

	ಹಿಂತಿರುಗಿ rv;
}

#ಆದರೆ defined(FLASH_UNIT_TEST)

ಸ್ಥಿರ sector_descriptor_t  test_sector_map[] = {
	{14, 128 * 1024, 0x081c0000},
	{15, 128 * 1024, 0x081e0000},
	{0, 0, 0},
};

__EXPORT ಶೂನ್ಯ test(ಶೂನ್ಯ);

uint8_t test_buf[32 * 1024];

__EXPORT ಶೂನ್ಯ test(ಶೂನ್ಯ)
{
	uint16_t ಅತಿದೊಡ್ಡ_ಖಂಡ_2 = (32 * 1024) + 96;
	uint8_t *ಬಫರ್_2 = ಸ್ಮರಣೆ_ಕೊಡು(ಅತಿದೊಡ್ಡ_ಖಂಡ_2);

	parameter_flashfs_init(test_sector_map, ಬಫರ್_2, ಅತಿದೊಡ್ಡ_ಖಂಡ_2);

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ t = 0; t < ಗಾತ್ರ(test_buf); t++) {
		test_buf[t] = (uint8_t) t;
	}

	ಪೂರ್ಣಾಂಕ er = parameter_flashfs_erase();
	uint8_t *fbuffer;
	size_t buf_ಗಾತ್ರ;
	ಪೂರ್ಣಾಂಕ written = 0;
	ಪೂರ್ಣಾಂಕ ಪಠ = 0;
	ಪೂರ್ಣಾಂಕ rv = 0;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ a = 0; a <= 4; a++) {
		parameter_flashfs_erase();
		rv =  parameter_flashfs_alloc(parameters_token, &fbuffer, &buf_ಗಾತ್ರ);
		ಸ್ಮರಣೆ_ನಕಲು(fbuffer, test_buf, a);
		buf_ಗಾತ್ರ = a;
		written = parameter_flashfs_write(parameters_token, fbuffer, buf_ಗಾತ್ರ);
		ಪಠ = parameter_flashfs_read(parameters_token, &fbuffer, &buf_ಗಾತ್ರ);
		parameter_flashfs_free();

		ಆದರೆ (ಪಠ != written) {
			ಸ್ಥಿರ ಅಸ್ಥಿರ ಪೂರ್ಣಾಂಕ j;
			j++;
		}
	}

	ಪೂರ್ಣಾಂಕ ಖಂಡ_2 = 2048;

	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ a = 0; a <= 8; a++) {
		parameter_flashfs_erase();
		rv =  parameter_flashfs_alloc(parameters_token, &fbuffer, &buf_ಗಾತ್ರ);
		ಸ್ಮರಣೆ_ನಕಲು(fbuffer, test_buf, ಖಂಡ_2);
		buf_ಗಾತ್ರ = ಖಂಡ_2;
		written = parameter_flashfs_write(parameters_token, fbuffer, buf_ಗಾತ್ರ);
		ಪಠ = parameter_flashfs_read(parameters_token, &fbuffer, &buf_ಗಾತ್ರ);
		parameter_flashfs_free();

		ಆದರೆ (ಪಠ != written) {
			ಸ್ಥಿರ ಅಸ್ಥಿರ ಪೂರ್ಣಾಂಕ j;
			j++;
		}

		ಖಂಡ_2 += 2048;
	}

	rv++;
	er++;
	ಬಿಡುಗಡೆ(ಬಫರ್_2);
}
#ಆದರೆ_ಅಂತ್ಯ /* FLASH_UNIT_TEST */
#ಆದರೆ_ಅಂತ್ಯ /* CONFIG_ARCH_HAVE_PROGMEM */
