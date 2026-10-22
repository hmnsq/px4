/****************************************************************************
 *
 *   Copyright (c) 2026 PX4 Development Team. All rights reserved.
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

#ಅನ್ತರ್ಭಾವ "odid_bid_storage.hpp"

#ಅನ್ತರ್ಭಾವ <fcntl.h>
#ಅನ್ತರ್ಭಾವ <stddef.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __PX4_NUTTX
#ಅನ್ತರ್ಭಾವ <nuttx/crc32.h>
#ಅನ್ತರ್ಭಾವ <px4_platform_common/px4_manifest.h>
#ಇಲ್ಲದಿದ್ದರೆ
#ಅನ್ತರ್ಭಾವ <crc32.h>
#ಆದರೆ_ಅಂತ್ಯ

ನಾಮಾಕಾಶ open_drone_id
{
ನಾಮಾಕಾಶ
{

ಸ್ಥಿರ constexpr uint32_t ODID_UASID_MAGIC = 0x4449444f; // "ODID"
ಸ್ಥಿರ constexpr uint8_t ODID_UASID_ದಾಖಲೆ_ಆವೃತ್ತಿ = 1;

ರಚನೆ __attribute__((packed)) Uas_Id_ದಾಖಲೆ {
	uint32_t magic;
	uint8_t ಆವೃತ್ತಿ;
	uint8_t id_type;
	uint8_t ua_type;
	uint8_t reserved;
	uint8_t uas_id[UAS_ID_ಗಾತ್ರ];
	uint32_t ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ;
};

ವರ್ಗ BidStorageBackend
{
ಸಾರ್ವಜನಿಕ:
	Bid_ಬೀಗ_Info get();
	Bid_Store_ಫಲಿತಾಂಶ checkOrStore(uint8_t ua_type, ಸ್ಥಿರಾಂಕ uint8_t uas_id[UAS_ID_ಗಾತ್ರ]);

ಖಾಸಗಿ:
	ಸ್ಥಿರ ತಾರ್ಕಿಕ uas_Id_Is_ಖಾಲಿಗಳು(ಸ್ಥಿರಾಂಕ uint8_t uas_id[UAS_ID_ಗಾತ್ರ])
	{
		ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ i = 0; i < UAS_ID_ಗಾತ್ರ; ++i) {
			ಆದರೆ (uas_id[i] != 0) {
				ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
			}
		}

		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಸ್ಥಿರ uint32_t ದಾಖಲೆ_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ(ಸ್ಥಿರಾಂಕ Uas_Id_ದಾಖಲೆ &ದಾಖಲೆ)
	{
		ಹಿಂತಿರುಗಿ ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ_32(reinterpret_cast<ಸ್ಥಿರಾಂಕ uint8_t *>(&ದಾಖಲೆ.ಆವೃತ್ತಿ),
			     offsetof(Uas_Id_ದಾಖಲೆ, ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ) - offsetof(Uas_Id_ದಾಖಲೆ, ಆವೃತ್ತಿ));
	}

	ಸ್ಥಿರ ತಾರ್ಕಿಕ ದಾಖಲೆ_Valid(ಸ್ಥಿರಾಂಕ Uas_Id_ದಾಖಲೆ &ದಾಖಲೆ)
	{
		ಹಿಂತಿರುಗಿ ದಾಖಲೆ.magic == ODID_UASID_MAGIC
		       && ದಾಖಲೆ.ಆವೃತ್ತಿ == ODID_UASID_ದಾಖಲೆ_ಆವೃತ್ತಿ
		       && ದಾಖಲೆ.id_type == UAS_ID_TYPE_SERIAL_ಸಂಖ್ಯೆ
		       && !uas_Id_Is_ಖಾಲಿಗಳು(ದಾಖಲೆ.uas_id)
		       && ದಾಖಲೆ.ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ == ದಾಖಲೆ_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ(ದಾಖಲೆ);
	}

	// Check if the record is all 0xff, which is the erased state of flash.
	// This is needed to distinguish between an unwritten record and a record that was written with all bytes set to 0 (which would be invalid but not unwritten).
	ಸ್ಥಿರ ತಾರ್ಕಿಕ ದಾಖಲೆ_Blank(ಸ್ಥಿರಾಂಕ Uas_Id_ದಾಖಲೆ &ದಾಖಲೆ)
	{
		ಸ್ಥಿರಾಂಕ uint8_t *ಬೈಟುಗಳು = reinterpret_cast<ಸ್ಥಿರಾಂಕ uint8_t *>(&ದಾಖಲೆ);

		ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ i = 0; i < ಗಾತ್ರ(ದಾಖಲೆ); ++i) {
			ಆದರೆ (ಬೈಟುಗಳು[i] != 0xff) {
				ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
			}
		}

		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

	ಸ್ಥಿರ ತಾರ್ಕಿಕ ದಾಖಲೆ_Matches(ಸ್ಥಿರಾಂಕ Uas_Id_ದಾಖಲೆ &ದಾಖಲೆ, ಸ್ಥಿರಾಂಕ uint8_t uas_id[UAS_ID_ಗಾತ್ರ])
	{
		ಹಿಂತಿರುಗಿ memcmp(ದಾಖಲೆ.uas_id, uas_id, UAS_ID_ಗಾತ್ರ) == 0;
	}

	// Write the entire buffer to the file descriptor, handling short writes.
	ಸ್ಥಿರ ತಾರ್ಕಿಕ writeAll(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಬಫರ್_2, size_t ಗಾತ್ರ_2)
	{
		ಸ್ಥಿರಾಂಕ uint8_t *ಬೈಟುಗಳು = static_cast<ಸ್ಥಿರಾಂಕ uint8_t *>(ಬಫರ್_2);

		ಆಗ (ಗಾತ್ರ_2 > 0) {
			ಸ್ಥಿರಾಂಕ ssize_t written = ಲಿಖ(fd, ಬೈಟುಗಳು, ಗಾತ್ರ_2);

			ಆದರೆ (written <= 0) {
				ಹಿಂತಿರುಗಿ ಸುಳ್ಳು;
			}

			ಬೈಟುಗಳು += written;
			ಗಾತ್ರ_2 -= written;
		}

		ಹಿಂತಿರುಗಿ ನಿಜ;
	}

};

static_assert(ಗಾತ್ರ(Uas_Id_ದಾಖಲೆ) <= 256, "OpenDroneID UAS ID record must fit in /fs/mtd_id");

// The UAS ID storage is only supported on PX4 targets with NuttX and an MTD partition named "ID".
// On other platforms, the get() method will return StorageUnavailable and the checkOrStore() method will return Unsupported.
Bid_ಬೀಗ_Info BidStorageBackend::get()
{
#ಆದರೆ defined(__PX4_NUTTX)
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ = ಶೂನ್ಯಸೂಚಕ;

	ಆದರೆ (px4_mtd_query("MTD_ID", ಶೂನ್ಯಸೂಚಕ, &ಮಾರ್ಗ) != 0 || ಮಾರ್ಗ == ಶೂನ್ಯಸೂಚಕ) {
		ಹಿಂತಿರುಗಿ {Bid_ಬೀಗ_State::StorageUnavailable, {}};
	}

	ಪೂರ್ಣಾಂಕ fd = open(ಮಾರ್ಗ, O_RDONLY);

	ಆದರೆ (fd < 0) {
		ಹಿಂತಿರುಗಿ {Bid_ಬೀಗ_State::StorageUnavailable, {}};
	}

	Uas_Id_ದಾಖಲೆ ದಾಖಲೆ{};

	ಆದರೆ (ಪಠ(fd, &ದಾಖಲೆ, ಗಾತ್ರ(ದಾಖಲೆ)) != ಗಾತ್ರ(ದಾಖಲೆ)) {
		close(fd);
		ಹಿಂತಿರುಗಿ {Bid_ಬೀಗ_State::StorageUnavailable, {}};
	}

	close(fd);

	ಆದರೆ (ದಾಖಲೆ_Valid(ದಾಖಲೆ)) {
		Bid_ಬೀಗ_Info info{Bid_ಬೀಗ_State::Locked, {}};
		info.id.ua_type = ದಾಖಲೆ.ua_type;
		ಸ್ಮರಣೆ_ನಕಲು(info.id.uas_id, ದಾಖಲೆ.uas_id, UAS_ID_ಗಾತ್ರ);
		ಹಿಂತಿರುಗಿ info;
	}

	ಆದರೆ (ದಾಖಲೆ_Blank(ದಾಖಲೆ)) {
		ಹಿಂತಿರುಗಿ {Bid_ಬೀಗ_State::ಖಾಲಿಗಳು_2, {}};
	}

	ಹಿಂತಿರುಗಿ {Bid_ಬೀಗ_State::Invalid_ದಾಖಲೆ, {}};
#ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ {Bid_ಬೀಗ_State::Unsupported, {}};
#ಆದರೆ_ಅಂತ್ಯ
}

// Check if the given UAS ID matches the stored one, or store it if the storage is empty.
// The storage is write-once, so once a UAS ID is stored it cannot be changed (except by erasing the flash).
Bid_Store_ಫಲಿತಾಂಶ BidStorageBackend::checkOrStore(uint8_t ua_type, ಸ್ಥಿರಾಂಕ uint8_t uas_id[UAS_ID_ಗಾತ್ರ])
{
	ಆದರೆ (uas_Id_Is_ಖಾಲಿಗಳು(uas_id)) {
		ಹಿಂತಿರುಗಿ Bid_Store_ಫಲಿತಾಂಶ::InvalidBasicId;
	}

#ಆದರೆ defined(__PX4_NUTTX)
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ = ಶೂನ್ಯಸೂಚಕ;

	ಆದರೆ (px4_mtd_query("MTD_ID", ಶೂನ್ಯಸೂಚಕ, &ಮಾರ್ಗ) != 0 || ಮಾರ್ಗ == ಶೂನ್ಯಸೂಚಕ) {
		ಹಿಂತಿರುಗಿ Bid_Store_ಫಲಿತಾಂಶ::StorageUnavailable;
	}

	ಪೂರ್ಣಾಂಕ fd = open(ಮಾರ್ಗ, O_RDWR);

	ಆದರೆ (fd < 0) {
		ಹಿಂತಿರುಗಿ Bid_Store_ಫಲಿತಾಂಶ::StorageUnavailable;
	}

	Uas_Id_ದಾಖಲೆ existing{};

	ಆದರೆ (ಪಠ(fd, &existing, ಗಾತ್ರ(existing)) != ಗಾತ್ರ(existing)) {
		close(fd);
		ಹಿಂತಿರುಗಿ Bid_Store_ಫಲಿತಾಂಶ::StorageUnavailable;
	}

	ಆದರೆ (ದಾಖಲೆ_Valid(existing)) {
		ಸ್ಥಿರಾಂಕ Bid_Store_ಫಲಿತಾಂಶ ಫಲಿತಾಂಶ = ದಾಖಲೆ_Matches(existing, uas_id)
					      ? Bid_Store_ಫಲಿತಾಂಶ::AlreadyLocked
					      : Bid_Store_ಫಲಿತಾಂಶ::AlreadyLockedDifferent;
		close(fd);
		ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
	}

	ಆದರೆ (!ದಾಖಲೆ_Blank(existing)) {
		close(fd);
		ಹಿಂತಿರುಗಿ Bid_Store_ಫಲಿತಾಂಶ::Invalid_ದಾಖಲೆ;
	}

	Uas_Id_ದಾಖಲೆ ದಾಖಲೆ{};
	ದಾಖಲೆ.magic = ODID_UASID_MAGIC;
	ದಾಖಲೆ.ಆವೃತ್ತಿ = ODID_UASID_ದಾಖಲೆ_ಆವೃತ್ತಿ;
	ದಾಖಲೆ.id_type = UAS_ID_TYPE_SERIAL_ಸಂಖ್ಯೆ;
	ದಾಖಲೆ.ua_type = ua_type;
	ಸ್ಮರಣೆ_ನಕಲು(ದಾಖಲೆ.uas_id, uas_id, UAS_ID_ಗಾತ್ರ);
	ದಾಖಲೆ.ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ = ದಾಖಲೆ_ಚಕ್ರೀಯ_ಪರೀಕ್ಷೆ(ದಾಖಲೆ);

	ಸ್ಥಿರಾಂಕ uint32_t magic = ದಾಖಲೆ.magic;
	ದಾಖಲೆ.magic = 0xffffffff;

	ಸ್ಥಿರಾಂಕ ತಾರ್ಕಿಕ written = lseek(fd, ಗಾತ್ರ(ದಾಖಲೆ.magic), SEEK_SET) == static_cast<off_t>(ಗಾತ್ರ(ದಾಖಲೆ.magic))
			     && writeAll(fd, reinterpret_cast<ಸ್ಥಿರಾಂಕ uint8_t *>(&ದಾಖಲೆ) + ಗಾತ್ರ(ದಾಖಲೆ.magic),
					 ಗಾತ್ರ(ದಾಖಲೆ) - ಗಾತ್ರ(ದಾಖಲೆ.magic))
			     && fsync(fd) == 0
			     && lseek(fd, 0, SEEK_SET) == 0
			     && writeAll(fd, &magic, ಗಾತ್ರ(magic))
			     && fsync(fd) == 0;

	close(fd);

	ಹಿಂತಿರುಗಿ written ? Bid_Store_ಫಲಿತಾಂಶ::Stored : Bid_Store_ಫಲಿತಾಂಶ::WriteError;
#ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ Bid_Store_ಫಲಿತಾಂಶ::Unsupported;
#ಆದರೆ_ಅಂತ್ಯ
}

} // namespace

// Public interface that uses the backend implementation.
Bid_ಬೀಗ_Info BidStorage::get()
{
	BidStorageBackend storage;
	ಹಿಂತಿರುಗಿ storage.get();
}

Bid_Store_ಫಲಿತಾಂಶ BidStorage::checkOrStore(uint8_t ua_type, ಸ್ಥಿರಾಂಕ uint8_t uas_id[UAS_ID_ಗಾತ್ರ])
{
	BidStorageBackend storage;
	ಹಿಂತಿರುಗಿ storage.checkOrStore(ua_type, uas_id);
}

//
Bid_Store_ಫಲಿತಾಂಶ Bid_ಬೀಗ::resolve(uint8_t id_type, uint8_t ua_type, ಸ್ಥಿರಾಂಕ uint8_t uas_id[UAS_ID_ಗಾತ್ರ],
				UasId &locked_id)
{
	ಆದರೆ (id_type != UAS_ID_TYPE_SERIAL_ಸಂಖ್ಯೆ) {
		ಹಿಂತಿರುಗಿ Bid_Store_ಫಲಿತಾಂಶ::InvalidBasicId;
	}

	ಆದರೆ (!_loaded) {
		_info = BidStorage::get();
		_loaded = ನಿಜ;
	}

	ಆದರೆ (_info.locked()) {
		locked_id = _info.id;
		ಹಿಂತಿರುಗಿ Bid_Store_ಫಲಿತಾಂಶ::AlreadyLocked;
	}

	ಆದರೆ (_info.state != Bid_ಬೀಗ_State::ಖಾಲಿಗಳು_2) {
		ಆಯ್ಕೆ (_info.state) {
		ಸ್ಥಿತಿ Bid_ಬೀಗ_State::Invalid_ದಾಖಲೆ:
			ಹಿಂತಿರುಗಿ Bid_Store_ಫಲಿತಾಂಶ::Invalid_ದಾಖಲೆ;

		ಸ್ಥಿತಿ Bid_ಬೀಗ_State::StorageUnavailable:
			ಹಿಂತಿರುಗಿ Bid_Store_ಫಲಿತಾಂಶ::StorageUnavailable;

		ಸ್ಥಿತಿ Bid_ಬೀಗ_State::WriteError:
			ಹಿಂತಿರುಗಿ Bid_Store_ಫಲಿತಾಂಶ::WriteError;

		ಸ್ಥಿತಿ Bid_ಬೀಗ_State::Unsupported:
			ಹಿಂತಿರುಗಿ Bid_Store_ಫಲಿತಾಂಶ::Unsupported;

		ಸ್ಥಿತಿ Bid_ಬೀಗ_State::ಖಾಲಿಗಳು_2:
		ಸ್ಥಿತಿ Bid_ಬೀಗ_State::Locked:
			ನಿಲ್ಲಿಸು;
		}
	}

	ಸ್ಥಿರಾಂಕ Bid_Store_ಫಲಿತಾಂಶ ಫಲಿತಾಂಶ = BidStorage::checkOrStore(ua_type, uas_id);

	ಆಯ್ಕೆ (ಫಲಿತಾಂಶ) {
	ಸ್ಥಿತಿ Bid_Store_ಫಲಿತಾಂಶ::Stored:
		_info.state = Bid_ಬೀಗ_State::Locked;
		_info.id.ua_type = ua_type;
		ಸ್ಮರಣೆ_ನಕಲು(_info.id.uas_id, uas_id, UAS_ID_ಗಾತ್ರ);
		locked_id = _info.id;
		ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

	ಸ್ಥಿತಿ Bid_Store_ಫಲಿತಾಂಶ::AlreadyLocked:
	ಸ್ಥಿತಿ Bid_Store_ಫಲಿತಾಂಶ::AlreadyLockedDifferent: {
			_info = BidStorage::get();

			ಆದರೆ (_info.locked()) {
				locked_id = _info.id;
				ಹಿಂತಿರುಗಿ Bid_Store_ಫಲಿತಾಂಶ::AlreadyLocked;
			}

			ಹಿಂತಿರುಗಿ Bid_Store_ಫಲಿತಾಂಶ::Invalid_ದಾಖಲೆ;
		}

	ಸ್ಥಿತಿ Bid_Store_ಫಲಿತಾಂಶ::InvalidBasicId:
	ಸ್ಥಿತಿ Bid_Store_ಫಲಿತಾಂಶ::StorageUnavailable:
	ಸ್ಥಿತಿ Bid_Store_ಫಲಿತಾಂಶ::Invalid_ದಾಖಲೆ:
	ಸ್ಥಿತಿ Bid_Store_ಫಲಿತಾಂಶ::WriteError:
	ಸ್ಥಿತಿ Bid_Store_ಫಲಿತಾಂಶ::Unsupported:
		ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
	}

	ಹಿಂತಿರುಗಿ Bid_Store_ಫಲಿತಾಂಶ::Invalid_ದಾಖಲೆ;
}

} // namespace open_drone_id
