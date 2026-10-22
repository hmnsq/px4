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

#అన్తర్భావ <px4_platform_common/defines.h>
#అన్తర్భావ <px4_platform_common/posix.h>
#అన్తర్భావ <nuttx/crc32.h>
#అన్తర్భావ <stddef.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <stdbool.h>
#అన్తర్భావ <stdint.h>
#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <errno.h>
#అన్తర్భావ "flashfs.h"
#అన్తర్భావ <nuttx/compiler.h>
#అన్తర్భావ <nuttx/progmem.h>
#అన్తర్భావ <board_config.h>

#ఒకవేళ defined(CONFIG_ARCH_HAVE_PROGMEM) || defined(BOARD_USE_EXTERNAL_FLASH)

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Private Types
 ****************************************************************************/

రకనిర్వచనం uint32_t h_magic_t;
రకనిర్వచనం uint16_t h_కొలత_t;
రకనిర్వచనం uint16_t h_flag_t;
రకనిర్వచనం uint16_t దత్తాంశం_కొలత_t;

రకనిర్వచనం గణన  flash_config_t {
	అతిపెద్ద_ఖండం = 2 * 1024, // This represents the size need for backing store
	MagicSig     = 0xaa553cc3,
	BlankSig     = 0xffffffff
} flash_config_t;

రకనిర్వచనం గణన  flash_flags_t {
	కొలత_ముసుగు      = 0x0003,
	ముసుగు_నమోదు     = ~కొలత_ముసుగు,
	Blank_నమోదు    = (h_flag_t)BlankSig,
	Valid_నమోదు    = (0xa5ac & ~కొలత_ముసుగు),
	Erased_నమోదు   = 0x0000,
} flash_flags_t;


/* The struct flash_entry_header_t will be sizeof(uint32_t) aligned
 * The Size will be the actual length of the header plus the data
 * and any padding needed to have the size be an even multiple of
 * sizeof(uint32_t)
 */
#సూచన GCC diagnostic నెట్టు
#సూచన GCC diagnostic ignored "-Wattributes"
రకనిర్వచనం begin_packed_struct నిర్మాణం flash_నమోదు_శీర్షిక_t {
	h_magic_t            magic;           /* Used to ID files*/
	h_flag_t             flag;            /* Used to erase this entry */
	uint32_t             చక్రీయ_తనిఖీ;             /* Calculated over the size - end of data */
	h_కొలత_t             కొలత;            /* When added to a byte pointer to flash_entry_header_t
                                               * Will result the offset of the next active file or
                                               * free space. */
	flash_file_token_t   file_token;      /* file token type - essentially the name/type */
} చివర_packed_struct flash_నమోదు_శీర్షిక_t __attribute__((aligned(పరిమాణం(uint32_t))));
#సూచన GCC diagnostic తీయి

/****************************************************************************
 * Private Function Prototypes
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/
స్థిర uint8_t *working_బఫరు;
స్థిర size_t working_బఫరు_కొలత;
స్థిర bool working_బఫరు_static;
స్థిర sector_descriptor_t *sector_map;
స్థిర పూర్ణాంకం చివరి_erased;

/****************************************************************************
 * Public Data
 ****************************************************************************/

స్థిరరాశి flash_file_token_t parameters_token = {
	.n = {'p', 'l', 'o', 'g'},
};

స్థిరరాశి flash_file_token_t parameters_legacy_token = {
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

శూన్య parameter_flashfs_free(శూన్య)
{
	ఒకవేళ (!working_బఫరు_static && working_బఫరు != ఖాళీ) {
		విడుదల(working_బఫరు);
		working_బఫరు = ఖాళీ;
		working_బఫరు_కొలత = 0;
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

స్థిర అంతఃవరుస పూర్ణాంకం blank_flash(uint32_t *pf)
{
	తిరిగి *pf == BlankSig;
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

స్థిర bool blank_check(flash_నమోదు_శీర్షిక_t *pf,
			size_t new_కొలత)
{
	bool rv = true;
	uint32_t *pm = (uint32_t *) pf;
	new_కొలత /= పరిమాణం(uint32_t);

	అయితే (new_కొలత-- && rv) {
		ఒకవేళ (!blank_flash(pm++)) {
			rv = false;
		}
	}

	తిరిగి rv;
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

స్థిర అంతఃవరుస పూర్ణాంకం valid_magic(h_magic_t *pm)
{
	తిరిగి *pm == MagicSig;
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

స్థిర అంతఃవరుస పూర్ణాంకం blank_నమోదు(flash_నమోదు_శీర్షిక_t *fi)
{
	తిరిగి fi->magic == BlankSig  &&  fi->flag == Blank_నమోదు;
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

స్థిర అంతఃవరుస పూర్ణాంకం valid_నమోదు(flash_నమోదు_శీర్షిక_t *fi)
{
	తిరిగి (fi->flag & ముసుగు_నమోదు) == Valid_నమోదు;
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

స్థిర అంతఃవరుస పూర్ణాంకం నమోదు_కొలత_adjust(flash_నమోదు_శీర్షిక_t *fi)
{
	తిరిగి fi->flag & కొలత_ముసుగు;
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

స్థిర అంతఃవరుస flash_నమోదు_శీర్షిక_t *తదుపరి_నమోదు(flash_నమోదు_శీర్షిక_t *fi)
{
	uint8_t *pb = (uint8_t *)fi;
	తిరిగి (flash_నమోదు_శీర్షిక_t *)(pb + fi->కొలత);
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

స్థిర అంతఃవరుస uint8_t *నమోదు_దత్తాంశం(flash_నమోదు_శీర్షిక_t *fi)
{
	తిరిగి ((uint8_t *)fi) + పరిమాణం(flash_నమోదు_శీర్షిక_t);
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

స్థిర అంతఃవరుస దత్తాంశం_కొలత_t నమోదు_దత్తాంశం_నిడివి(flash_నమోదు_శీర్షిక_t *fi)
{
	తిరిగి fi->కొలత - (పరిమాణం(flash_నమోదు_శీర్షిక_t) + నమోదు_కొలత_adjust(fi));
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

స్థిర అంతఃవరుస స్థిరరాశి uint8_t *నమోదు_చక్రీయ_తనిఖీ_ప్రారంభం(flash_నమోదు_శీర్షిక_t *fi)
{
	తిరిగి (స్థిరరాశి uint8_t *)&fi->కొలత;
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

స్థిర అంతఃవరుస దత్తాంశం_కొలత_t నమోదు_చక్రీయ_తనిఖీ_నిడివి(flash_నమోదు_శీర్షిక_t *fi)
{
	తిరిగి fi->కొలత - offsetof(flash_నమోదు_శీర్షిక_t, కొలత);
}

/* The parameter sector can be the last one on the chip, so no header
 * field may be read until the whole header is known to lie inside it. */
స్థిర అంతఃవరుస bool శీర్షిక_సరిపోతుంది(flash_నమోదు_శీర్షిక_t *pf, h_magic_t *pe)
{
	తిరిగి pf + 1 <= (flash_నమోదు_శీర్షిక_t *)pe;
}

స్థిర bool చక్రీయ_తనిఖీ_శీర్షిక_ok(flash_నమోదు_శీర్షిక_t *pf, h_magic_t *pe)
{
	ఒకవేళ (!శీర్షిక_సరిపోతుంది(pf, pe)) {
		తిరిగి false;
	}

	ఒకవేళ (pf->కొలత < పరిమాణం(flash_నమోదు_శీర్షిక_t)) {
		తిరిగి false;
	}

	స్థిరరాశి uint8_t *చక్రీయ_తనిఖీ_ప్రారంభం = నమోదు_చక్రీయ_తనిఖీ_ప్రారంభం(pf);
	దత్తాంశం_కొలత_t చక్రీయ_తనిఖీ_నిడివి = నమోదు_చక్రీయ_తనిఖీ_నిడివి(pf);

	ఒకవేళ (చక్రీయ_తనిఖీ_ప్రారంభం + చక్రీయ_తనిఖీ_నిడివి > (uint8_t *)pe) {
		తిరిగి false;
	}

	తిరిగి pf->చక్రీయ_తనిఖీ == చక్రీయ_తనిఖీ_32(చక్రీయ_తనిఖీ_ప్రారంభం, చక్రీయ_తనిఖీ_నిడివి);
}

/* The walkers stop at the last word of a sector and reject a record that
 * ends beyond it, so nothing may be written into that word. */
స్థిర అంతఃవరుస size_t sector_usable(పూర్ణాంకం s)
{
	తిరిగి sector_map[s].కొలత - పరిమాణం(h_magic_t);
}

స్థిర bool స్థానం_is_free(flash_నమోదు_శీర్షిక_t *pf, పూర్ణాంకం s, size_t required)
{
	uint8_t *begin = (uint8_t *)sector_map[s].చిరునామా;
	uint8_t *చివర = begin + sector_usable(s);

	ఒకవేళ ((uint8_t *)pf < begin || required == 0 || (uint8_t *)pf + required > చివర) {
		తిరిగి false;
	}

	/* Entries start on a flash row. up_progmem_write only asserts that in
	 * debug builds, so a slot found by the word-wise scan behind a torn
	 * record would otherwise be programmed misaligned. */
	ఒకవేళ (((uintptr_t)pf & కొలత_ముసుగు) != 0) {
		తిరిగి false;
	}

	తిరిగి blank_నమోదు(pf) && blank_check(pf, required);
}

స్థిర పూర్ణాంకం for_each_valid_నమోదు(flash_file_token_t token,
				పూర్ణాంకం (*cb)(flash_నమోదు_శీర్షిక_t *pf, శూన్య *arg),
				శూన్య *arg);

స్థిర పూర్ణాంకం stop_at_మొదటి(flash_నమోదు_శీర్షిక_t *pf, శూన్య *arg)
{
	*(flash_నమోదు_శీర్షిక_t **)arg = pf;
	తిరిగి -ECANCELED;
}

/* Oldest valid entry for token, or NULL. */
స్థిర flash_నమోదు_శీర్షిక_t *కనుగొను_నమోదు(flash_file_token_t token)
{
	flash_నమోదు_శీర్షిక_t *pf = ఖాళీ;
	for_each_valid_నమోదు(token, stop_at_మొదటి, &pf);
	తిరిగి pf;
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

స్థిర flash_నమోదు_శీర్షిక_t *కనుగొను_append(size_t required)
{
	ఒకవేళ (sector_map == ఖాళీ || required == 0) {
		తిరిగి ఖాళీ;
	}

	flash_నమోదు_శీర్షిక_t *చివరి = ఖాళీ;
	పూర్ణాంకం చివరి_s = -1;

	కొరకు (పూర్ణాంకం s = 0; sector_map[s].చిరునామా; s++) {
		h_magic_t *pmagic = (h_magic_t *) sector_map[s].చిరునామా;
		h_magic_t *pe = pmagic + (sector_map[s].కొలత / పరిమాణం(h_magic_t)) - 1;

		అయితే (pmagic < pe) {
			ఒకవేళ (!valid_magic(pmagic)) {
				pmagic++;
				కొనసాగు;
			}

			flash_నమోదు_శీర్షిక_t *pf = (flash_నమోదు_శీర్షిక_t *) pmagic;

			ఒకవేళ (చక్రీయ_తనిఖీ_శీర్షిక_ok(pf, pe)) {
				చివరి = pf;
				చివరి_s = s;
				pmagic = (h_magic_t *) తదుపరి_నమోదు(pf);
				pf = (flash_నమోదు_శీర్షిక_t *) pmagic;

				/* Same stop rule as for_each_valid_entry: a record behind a
				 * blank header is invisible to replay, so it must not anchor
				 * the append point either. */
				ఒకవేళ (pmagic >= pe || !శీర్షిక_సరిపోతుంది(pf, pe) || blank_నమోదు(pf)) {
					ఆపు;
				}

			} లేకపోతే {
				pmagic++;
			}
		}
	}

	స్థిరరాశి పూర్ణాంకం ప్రారంభం_s = (చివరి == ఖాళీ) ? 0 : చివరి_s;
	h_magic_t *pmagic = (చివరి == ఖాళీ)
			    ? (h_magic_t *) sector_map[0].చిరునామా
			    : (h_magic_t *) తదుపరి_నమోదు(చివరి);

	కొరకు (పూర్ణాంకం s = ప్రారంభం_s; sector_map[s].చిరునామా; s++) {
		ఒకవేళ (s != ప్రారంభం_s) {
			pmagic = (h_magic_t *) sector_map[s].చిరునామా;
		}

		h_magic_t *pe = (h_magic_t *) sector_map[s].చిరునామా
				+ (sector_map[s].కొలత / పరిమాణం(h_magic_t)) - 1;

		ఒకవేళ ((uint8_t *)pmagic < (uint8_t *)sector_map[s].చిరునామా
		    || (uint8_t *)pmagic >= (uint8_t *)sector_map[s].చిరునామా + sector_map[s].కొలత) {
			ఒకవేళ (s == ప్రారంభం_s) {
				కొనసాగు;
			}

			pmagic = (h_magic_t *) sector_map[s].చిరునామా;
		}

		అయితే (pmagic < pe) {
			flash_నమోదు_శీర్షిక_t *pf = (flash_నమోదు_శీర్షిక_t *) pmagic;

			ఒకవేళ (valid_magic(pmagic) && చక్రీయ_తనిఖీ_శీర్షిక_ok(pf, pe)) {
				pmagic = (h_magic_t *) తదుపరి_నమోదు(pf);
				కొనసాగు;
			}

			ఒకవేళ (స్థానం_is_free(pf, s, required)) {
				తిరిగి pf;
			}

			pmagic++;
		}
	}

	తిరిగి ఖాళీ;
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

స్థిర పూర్ణాంకం for_each_valid_నమోదు(flash_file_token_t token,
				పూర్ణాంకం (*cb)(flash_నమోదు_శీర్షిక_t *pf, శూన్య *arg),
				శూన్య *arg)
{
	పూర్ణాంకం n = 0;

	ఒకవేళ (sector_map == ఖాళీ) {
		తిరిగి -ENXIO;
	}

	కొరకు (పూర్ణాంకం s = 0; sector_map[s].చిరునామా; s++) {

		h_magic_t *pmagic = (h_magic_t *) sector_map[s].చిరునామా;
		h_magic_t *pe = pmagic + (sector_map[s].కొలత / పరిమాణం(h_magic_t)) - 1;

cont:

		అయితే (pmagic < pe && !valid_magic(pmagic)) {
			pmagic++;
		}

		ఒకవేళ (pmagic >= pe) { కొనసాగు; }

		flash_నమోదు_శీర్షిక_t *pf = (flash_నమోదు_శీర్షిక_t *) pmagic;

		ఒకవేళ (చక్రీయ_తనిఖీ_శీర్షిక_ok(pf, pe)) {

			ఒకవేళ (valid_నమోదు(pf) && pf->file_token.t == token.t) {
				ఒకవేళ (cb != ఖాళీ) {
					పూర్ణాంకం rv = cb(pf, arg);

					ఒకవేళ (rv < 0) {
						తిరిగి rv;
					}
				}

				n++;
			}

			pf = తదుపరి_నమోదు(pf);
			pmagic = (h_magic_t *) pf;

			ఒకవేళ (pmagic >= pe || !శీర్షిక_సరిపోతుంది(pf, pe) || blank_నమోదు(pf)) {
				కొనసాగు;
			}

		} లేకపోతే {
			pmagic++;
		}

		వెళ్ళు cont;
	}

	తిరిగి n;
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

స్థిర పూర్ణాంకం erase_sector(sector_descriptor_t *sm, flash_నమోదు_శీర్షిక_t *pf)
{
	పూర్ణాంకం rv = 0;
#ఒకవేళ defined(BOARD_USE_EXTERNAL_FLASH)
	ssize_t ఖండం_2 = పైకి_progmem_ext_getpage((size_t)pf);
#లేకపోతే
	ssize_t ఖండం_2 = పైకి_progmem_getpage((size_t)pf);
#అయితే_ముగింపు

	ఒకవేళ (ఖండం_2 > 0 && ఖండం_2 == sm->పుట) {
		చివరి_erased = sm->పుట;
#ఒకవేళ defined(BOARD_USE_EXTERNAL_FLASH)
		ssize_t కొలత = పైకి_progmem_ext_eraseblock(ఖండం_2);
#లేకపోతే
		ssize_t కొలత = పైకి_progmem_eraseblock(ఖండం_2);
#అయితే_ముగింపు

		ఒకవేళ (కొలత < 0 || కొలత != (ssize_t)sm->కొలత) {
			rv = కొలత;
		}
	}

	తిరిగి rv;
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

పూర్ణాంకం parameter_flashfs_read(flash_file_token_t token, uint8_t **బఫరు_2, size_t
			   *buf_కొలత)
{
	పూర్ణాంకం rv = -ENXIO;

	ఒకవేళ (sector_map) {

		rv = -ENOENT;
		flash_నమోదు_శీర్షిక_t *pf = కనుగొను_నమోదు(token);

		ఒకవేళ (pf) {
			(*బఫరు_2) = నమోదు_దత్తాంశం(pf);
			rv = నమోదు_దత్తాంశం_నిడివి(pf);
			*buf_కొలత = rv;
		}
	}

	తిరిగి rv;
}

/****************************************************************************
 * Name: parameter_flashfs_blank
 *
 * Description:
 *   Reports whether the parameter storage is fully erased, so callers can
 *   tell a blank store from one that holds unparseable data.
 *
 ****************************************************************************/

పూర్ణాంకం parameter_flashfs_blank(శూన్య)
{
	ఒకవేళ (sector_map == ఖాళీ) {
		తిరిగి -ENXIO;
	}

	కొరకు (పూర్ణాంకం s = 0; sector_map[s].చిరునామా; s++) {
		uint32_t *pm = (uint32_t *) sector_map[s].చిరునామా;
		uint32_t *pe = pm + (sector_map[s].కొలత / పరిమాణం(uint32_t));

		అయితే (pm < pe) {
			ఒకవేళ (!blank_flash(pm++)) {
				తిరిగి 0;
			}
		}
	}

	తిరిగి 1;
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

పూర్ణాంకం
parameter_flashfs_write(flash_file_token_t token, uint8_t *బఫరు_2, size_t buf_కొలత)
{
	పూర్ణాంకం rv = -ENXIO;

	ఒకవేళ (sector_map) {

		rv = 0;

		/* Calculate the total space needed */

		size_t మొత్తం_కొలత = buf_కొలత + పరిమాణం(flash_నమోదు_శీర్షిక_t);
		size_t alignment = పరిమాణం(h_magic_t) - 1;
		size_t  కొలత_adjust = ((మొత్తం_కొలత + alignment) & ~alignment) - మొత్తం_కొలత;
		మొత్తం_కొలత += కొలత_adjust;

		ఒకవేళ (మొత్తం_కొలత > UINT16_MAX) {
			తిరిగి -EFBIG;
		}

		/* Append after the last CRC-valid record. Compaction is the caller's job. */
		flash_నమోదు_శీర్షిక_t *pf = కనుగొను_append(మొత్తం_కొలత);

		ఒకవేళ (pf == 0) {
			తిరిగి -ENOSPC;
		}

		flash_నమోదు_శీర్షిక_t *pn = (flash_నమోదు_శీర్షిక_t *)(బఫరు_2 - పరిమాణం(flash_నమోదు_శీర్షిక_t));
		pn->magic = MagicSig;
		pn->file_token.t = token.t;
		pn->flag = Valid_నమోదు + కొలత_adjust;
		pn->కొలత = మొత్తం_కొలత;

		కొరకు (size_t a = 0; a < కొలత_adjust; a++) {
			బఫరు_2[buf_కొలత + a] = (uint8_t)BlankSig;
		}

		pn->చక్రీయ_తనిఖీ = చక్రీయ_తనిఖీ_32(నమోదు_చక్రీయ_తనిఖీ_ప్రారంభం(pn), నమోదు_చక్రీయ_తనిఖీ_నిడివి(pn));
#ఒకవేళ defined(BOARD_USE_EXTERNAL_FLASH)
		rv = పైకి_progmem_ext_write((size_t) pf, pn, pn->కొలత);
#లేకపోతే
		rv = పైకి_progmem_write((size_t) pf, pn, pn->కొలత);
#అయితే_ముగింపు
		పూర్ణాంకం system_బైట్లు = (పరిమాణం(flash_నమోదు_శీర్షిక_t) + కొలత_adjust);

		ఒకవేళ (rv >= system_బైట్లు) {
			rv -= system_బైట్లు;
		}
	}

	తిరిగి rv;
}

/****************************************************************************
 * Name: parameter_flashfs_walk
 ****************************************************************************/

నిర్మాణం walk_pack {
	parameter_flashfs_walk_cb cb;
	శూన్య *arg;
};

స్థిర పూర్ణాంకం walk_one(flash_నమోదు_శీర్షిక_t *pf, శూన్య *arg)
{
	నిర్మాణం walk_pack *w = (నిర్మాణం walk_pack *)arg;
	తిరిగి w->cb(నమోదు_దత్తాంశం(pf), నమోదు_దత్తాంశం_నిడివి(pf), w->arg);
}

పూర్ణాంకం parameter_flashfs_walk(flash_file_token_t token, parameter_flashfs_walk_cb cb, శూన్య *arg)
{
	నిర్మాణం walk_pack w = { cb, arg };
	తిరిగి for_each_valid_నమోదు(token, walk_one, &w);
}

/****************************************************************************
 * Name: parameter_flashfs_needs_compact
 ****************************************************************************/

size_t parameter_flashfs_max_payload(శూన్య)
{
	ఒకవేళ (sector_map == ఖాళీ) {
		తిరిగి 0;
	}

	size_t max_మొత్తం = 0;

	కొరకు (పూర్ణాంకం s = 0; sector_map[s].చిరునామా; s++) {
		ఒకవేళ (sector_usable(s) > max_మొత్తం) {
			max_మొత్తం = sector_usable(s);
		}
	}

	/* h_size_t holds the record size */
	ఒకవేళ (max_మొత్తం > UINT16_MAX) {
		max_మొత్తం = UINT16_MAX;
	}

	max_మొత్తం &= ~(size_t)కొలత_ముసుగు;

	ఒకవేళ (max_మొత్తం <= పరిమాణం(flash_నమోదు_శీర్షిక_t)) {
		తిరిగి 0;
	}

	తిరిగి max_మొత్తం - పరిమాణం(flash_నమోదు_శీర్షిక_t);
}

పూర్ణాంకం parameter_flashfs_needs_compact(flash_file_token_t token, size_t snapshot_కొలత)
{
	ఒకవేళ (sector_map == ఖాళీ) {
		తిరిగి -ENXIO;
	}

	flash_నమోదు_శీర్షిక_t *మొదటి = కనుగొను_నమోదు(token);

	ఒకవేళ (మొదటి == ఖాళీ) {
		తిరిగి 0;
	}

	/* Dead records ahead of the live data (a torn write, or the erased
	 * prefix the single-snapshot writer left) only go away with an erase. */
	ఒకవేళ ((uintptr_t)మొదటి != (uintptr_t)sector_map[0].చిరునామా) {
		తిరిగి 1;
	}

	/* Keep room after the log for a burst the size of the snapshot
	 * (reset-all tombstones, airframe PARAM_SET), floored so a UUID append
	 * does not force a boot erase. */
	స్థిరరాశి size_t align = పరిమాణం(h_magic_t) - 1;
	స్థిరరాశి size_t snapshot_మొత్తం = (snapshot_కొలత + పరిమాణం(flash_నమోదు_శీర్షిక_t) + align) & ~align;
	స్థిరరాశి size_t headroom = snapshot_మొత్తం > 8192 ? snapshot_మొత్తం : 8192;

	ఒకవేళ (కనుగొను_append(headroom) != ఖాళీ) {
		తిరిగి 0;
	}

	/* Compaction leaves the snapshot at the start of sector 0 and every
	 * other sector blank. Erase only if that layout has the room this one
	 * lacks, or every boot would pay the erase for nothing. */
	ఒకవేళ (sector_usable(0) >= snapshot_మొత్తం + headroom) {
		తిరిగి 1;
	}

	కొరకు (పూర్ణాంకం s = 1; sector_map[s].చిరునామా; s++) {
		ఒకవేళ (sector_usable(s) >= headroom) {
			తిరిగి 1;
		}
	}

	తిరిగి 0;
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

పూర్ణాంకం parameter_flashfs_alloc(flash_file_token_t token, uint8_t **బఫరు_2, size_t *buf_కొలత)
{
	పూర్ణాంకం rv = -ENXIO;

	ఒకవేళ (sector_map) {

		rv = -ENOMEM;

		ఒకవేళ (!working_బఫరు_static) {

			/* parameter_flashfs_write pads the entry to a word in place */
			working_బఫరు_కొలత = *buf_కొలత + పరిమాణం(flash_నమోదు_శీర్షిక_t) + కొలత_ముసుగు;
			working_బఫరు = జ్ఞాపకం_ఇవ్వు(working_బఫరు_కొలత);

		}

		/* Allocation failed or not provided */

		ఒకవేళ (working_బఫరు == ఖాళీ) {

			working_బఫరు_కొలత = 0;

		} లేకపోతే {

			/* We have a buffer reserve space and init it */
			*బఫరు_2 = &working_బఫరు[పరిమాణం(flash_నమోదు_శీర్షిక_t)];
			*buf_కొలత = working_బఫరు_కొలత - పరిమాణం(flash_నమోదు_శీర్షిక_t) - కొలత_ముసుగు;
			memset(working_బఫరు, 0xff, working_బఫరు_కొలత);
			rv = 0;

		}
	}

	తిరిగి rv;
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

పూర్ణాంకం parameter_flashfs_erase(శూన్య)
{
	పూర్ణాంకం rv = -ENXIO;

	ఒకవేళ (sector_map) {
		rv = 0;

		కొరకు (పూర్ణాంకం s = 0; sector_map[s].చిరునామా; s++) {
			పూర్ణాంకం sz = erase_sector(&sector_map[s], (flash_నమోదు_శీర్షిక_t *)sector_map[s].చిరునామా);

			ఒకవేళ (sz != 0) {
				తిరిగి sz;
			}

			rv += sector_map[s].కొలత;
		}
	}

	తిరిగి rv;
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

పూర్ణాంకం parameter_flashfs_init(sector_descriptor_t *fconfig, uint8_t *బఫరు_2, uint16_t కొలత)
{
	పూర్ణాంకం rv = 0;
	sector_map = fconfig;
	working_బఫరు_static = బఫరు_2 != ఖాళీ;

	ఒకవేళ (!working_బఫరు_static) {
		కొలత = 0;
	}

	working_బఫరు = బఫరు_2;
	working_బఫరు_కొలత = కొలత;
	చివరి_erased = -1;

	/* Sanity check */

	/* A non-blank store with no valid entry is torn or foreign; import
	 * reports -EILSEQ. Erasing here would hide a successful retry after
	 * a torn write. */

	తిరిగి rv;
}

#ఒకవేళ defined(FLASH_UNIT_TEST)

స్థిర sector_descriptor_t  test_sector_map[] = {
	{1, 16 * 1024, 0x08004000},
	{2, 16 * 1024, 0x08008000},
	{0, 0, 0},
};

__EXPORT శూన్య test(శూన్య);

uint8_t test_buf[32 * 1024];

__EXPORT శూన్య test(శూన్య)
{
	uint16_t అతిపెద్ద_ఖండం_2 = (32 * 1024) + 32;
	uint8_t *బఫరు_2 = జ్ఞాపకం_ఇవ్వు(అతిపెద్ద_ఖండం_2);

	parameter_flashfs_init(test_sector_map, బఫరు_2, అతిపెద్ద_ఖండం_2);

	కొరకు (పూర్ణాంకం t = 0; t < పరిమాణం(test_buf); t++) {
		test_buf[t] = (uint8_t) t;
	}

	పూర్ణాంకం er = parameter_flashfs_erase();
	uint8_t *fbuffer;
	size_t buf_కొలత;
	పూర్ణాంకం written = 0;
	పూర్ణాంకం పఠ = 0;
	పూర్ణాంకం rv = 0;

	కొరకు (పూర్ణాంకం a = 0; a <= 4; a++) {
		parameter_flashfs_erase();
		rv =  parameter_flashfs_alloc(parameters_token, &fbuffer, &buf_కొలత);
		జ్ఞాపకం_నకలు(fbuffer, test_buf, a);
		buf_కొలత = a;
		written = parameter_flashfs_write(parameters_token, fbuffer, buf_కొలత);
		పఠ = parameter_flashfs_read(parameters_token, &fbuffer, &buf_కొలత);
		parameter_flashfs_free();

		ఒకవేళ (పఠ != written) {
			స్థిర అస్థిర పూర్ణాంకం j;
			j++;
		}
	}

	పూర్ణాంకం ఖండం_2 = 2048;

	కొరకు (పూర్ణాంకం a = 0; a <= 8; a++) {
		parameter_flashfs_erase();
		rv =  parameter_flashfs_alloc(parameters_token, &fbuffer, &buf_కొలత);
		జ్ఞాపకం_నకలు(fbuffer, test_buf, ఖండం_2);
		buf_కొలత = ఖండం_2;
		written = parameter_flashfs_write(parameters_token, fbuffer, buf_కొలత);
		పఠ = parameter_flashfs_read(parameters_token, &fbuffer, &buf_కొలత);
		parameter_flashfs_free();

		ఒకవేళ (పఠ != written) {
			స్థిర అస్థిర పూర్ణాంకం j;
			j++;
		}

		ఖండం_2 += 2048;
	}

	rv++;
	er++;
	విడుదల(బఫరు_2);
}
#అయితే_ముగింపు /* FLASH_UNIT_TEST */
#అయితే_ముగింపు /* CONFIG_ARCH_HAVE_PROGMEM */
