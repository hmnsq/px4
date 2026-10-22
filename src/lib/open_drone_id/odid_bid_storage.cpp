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

#ਅਨ੍ਤਰ੍ਭਾਵ "odid_bid_storage.hpp"

#ਅਨ੍ਤਰ੍ਭਾਵ <fcntl.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stddef.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __PX4_NUTTX
#ਅਨ੍ਤਰ੍ਭਾਵ <nuttx/crc32.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/px4_manifest.h>
#ਨਹੀਂਤਾਂ
#ਅਨ੍ਤਰ੍ਭਾਵ <crc32.h>
#ਜੇ_ਅੰਤ

ਨਾਮਾਕਾਸ਼ open_drone_id
{
ਨਾਮਾਕਾਸ਼
{

ਸਥਿਰ constexpr uint32_t ODID_UASID_MAGIC = 0x4449444f; // "ODID"
ਸਥਿਰ constexpr uint8_t ODID_UASID_ਰਿਕਾਰਡ_ਸੰਸਕਰਣ = 1;

ਬਣਤਰ __attribute__((packed)) Uas_Id_ਰਿਕਾਰਡ {
	uint32_t magic;
	uint8_t ਸੰਸਕਰਣ;
	uint8_t id_type;
	uint8_t ua_type;
	uint8_t reserved;
	uint8_t uas_id[UAS_ID_ਮਾਪ];
	uint32_t ਚੱਕਰੀ_ਜਾਂਚ;
};

ਵਰਗ BidStorageBackend
{
ਜਨਤਕ:
	Bid_ਤਾਲਾ_Info get();
	Bid_Store_ਨਤੀਜਾ checkOrStore(uint8_t ua_type, ਅਚਲ uint8_t uas_id[UAS_ID_ਮਾਪ]);

ਨਿਜੀ:
	ਸਥਿਰ ਤਾਰਕਿਕ uas_Id_Is_ਖਾਲੀ_ਥਾਵਾਂ(ਅਚਲ uint8_t uas_id[UAS_ID_ਮਾਪ])
	{
		ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < UAS_ID_ਮਾਪ; ++i) {
			ਜੇ (uas_id[i] != 0) {
				ਵਾਪਸ ਝੂਠ;
			}
		}

		ਵਾਪਸ ਸੱਚ;
	}

	ਸਥਿਰ uint32_t ਰਿਕਾਰਡ_ਚੱਕਰੀ_ਜਾਂਚ(ਅਚਲ Uas_Id_ਰਿਕਾਰਡ &ਰਿਕਾਰਡ)
	{
		ਵਾਪਸ ਚੱਕਰੀ_ਜਾਂਚ_32(reinterpret_cast<ਅਚਲ uint8_t *>(&ਰਿਕਾਰਡ.ਸੰਸਕਰਣ),
			     offsetof(Uas_Id_ਰਿਕਾਰਡ, ਚੱਕਰੀ_ਜਾਂਚ) - offsetof(Uas_Id_ਰਿਕਾਰਡ, ਸੰਸਕਰਣ));
	}

	ਸਥਿਰ ਤਾਰਕਿਕ ਰਿਕਾਰਡ_Valid(ਅਚਲ Uas_Id_ਰਿਕਾਰਡ &ਰਿਕਾਰਡ)
	{
		ਵਾਪਸ ਰਿਕਾਰਡ.magic == ODID_UASID_MAGIC
		       && ਰਿਕਾਰਡ.ਸੰਸਕਰਣ == ODID_UASID_ਰਿਕਾਰਡ_ਸੰਸਕਰਣ
		       && ਰਿਕਾਰਡ.id_type == UAS_ID_TYPE_SERIAL_ਸੰਖਿਆ
		       && !uas_Id_Is_ਖਾਲੀ_ਥਾਵਾਂ(ਰਿਕਾਰਡ.uas_id)
		       && ਰਿਕਾਰਡ.ਚੱਕਰੀ_ਜਾਂਚ == ਰਿਕਾਰਡ_ਚੱਕਰੀ_ਜਾਂਚ(ਰਿਕਾਰਡ);
	}

	// Check if the record is all 0xff, which is the erased state of flash.
	// This is needed to distinguish between an unwritten record and a record that was written with all bytes set to 0 (which would be invalid but not unwritten).
	ਸਥਿਰ ਤਾਰਕਿਕ ਰਿਕਾਰਡ_Blank(ਅਚਲ Uas_Id_ਰਿਕਾਰਡ &ਰਿਕਾਰਡ)
	{
		ਅਚਲ uint8_t *ਬਾਈਟਾਂ = reinterpret_cast<ਅਚਲ uint8_t *>(&ਰਿਕਾਰਡ);

		ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < ਆਕਾਰ(ਰਿਕਾਰਡ); ++i) {
			ਜੇ (ਬਾਈਟਾਂ[i] != 0xff) {
				ਵਾਪਸ ਝੂਠ;
			}
		}

		ਵਾਪਸ ਸੱਚ;
	}

	ਸਥਿਰ ਤਾਰਕਿਕ ਰਿਕਾਰਡ_Matches(ਅਚਲ Uas_Id_ਰਿਕਾਰਡ &ਰਿਕਾਰਡ, ਅਚਲ uint8_t uas_id[UAS_ID_ਮਾਪ])
	{
		ਵਾਪਸ memcmp(ਰਿਕਾਰਡ.uas_id, uas_id, UAS_ID_ਮਾਪ) == 0;
	}

	// Write the entire buffer to the file descriptor, handling short writes.
	ਸਥਿਰ ਤਾਰਕਿਕ writeAll(ਪੂਰਨ fd, ਅਚਲ ਖਾਲੀ *ਬਫਰ_2, size_t ਮਾਪ)
	{
		ਅਚਲ uint8_t *ਬਾਈਟਾਂ = static_cast<ਅਚਲ uint8_t *>(ਬਫਰ_2);

		ਜਦੋਂ (ਮਾਪ > 0) {
			ਅਚਲ ssize_t written = ਲਿਖ(fd, ਬਾਈਟਾਂ, ਮਾਪ);

			ਜੇ (written <= 0) {
				ਵਾਪਸ ਝੂਠ;
			}

			ਬਾਈਟਾਂ += written;
			ਮਾਪ -= written;
		}

		ਵਾਪਸ ਸੱਚ;
	}

};

static_assert(ਆਕਾਰ(Uas_Id_ਰਿਕਾਰਡ) <= 256, "OpenDroneID UAS ID record must fit in /fs/mtd_id");

// The UAS ID storage is only supported on PX4 targets with NuttX and an MTD partition named "ID".
// On other platforms, the get() method will return StorageUnavailable and the checkOrStore() method will return Unsupported.
Bid_ਤਾਲਾ_Info BidStorageBackend::get()
{
#ਜੇ defined(__PX4_NUTTX)
	ਅਚਲ ਅੱਖਰ *ਰਸਤਾ = ਖਾਲੀ_ਸੂਚਕ;

	ਜੇ (px4_mtd_query("MTD_ID", ਖਾਲੀ_ਸੂਚਕ, &ਰਸਤਾ) != 0 || ਰਸਤਾ == ਖਾਲੀ_ਸੂਚਕ) {
		ਵਾਪਸ {Bid_ਤਾਲਾ_State::StorageUnavailable, {}};
	}

	ਪੂਰਨ fd = open(ਰਸਤਾ, O_RDONLY);

	ਜੇ (fd < 0) {
		ਵਾਪਸ {Bid_ਤਾਲਾ_State::StorageUnavailable, {}};
	}

	Uas_Id_ਰਿਕਾਰਡ ਰਿਕਾਰਡ{};

	ਜੇ (ਪਠ(fd, &ਰਿਕਾਰਡ, ਆਕਾਰ(ਰਿਕਾਰਡ)) != ਆਕਾਰ(ਰਿਕਾਰਡ)) {
		close(fd);
		ਵਾਪਸ {Bid_ਤਾਲਾ_State::StorageUnavailable, {}};
	}

	close(fd);

	ਜੇ (ਰਿਕਾਰਡ_Valid(ਰਿਕਾਰਡ)) {
		Bid_ਤਾਲਾ_Info info{Bid_ਤਾਲਾ_State::Locked, {}};
		info.id.ua_type = ਰਿਕਾਰਡ.ua_type;
		ਸਮ੍ਰਿਤੀ_ਨਕਲ(info.id.uas_id, ਰਿਕਾਰਡ.uas_id, UAS_ID_ਮਾਪ);
		ਵਾਪਸ info;
	}

	ਜੇ (ਰਿਕਾਰਡ_Blank(ਰਿਕਾਰਡ)) {
		ਵਾਪਸ {Bid_ਤਾਲਾ_State::ਖਾਲੀ_ਥਾਵਾਂ_2, {}};
	}

	ਵਾਪਸ {Bid_ਤਾਲਾ_State::Invalid_ਰਿਕਾਰਡ, {}};
#ਨਹੀਂਤਾਂ
	ਵਾਪਸ {Bid_ਤਾਲਾ_State::Unsupported, {}};
#ਜੇ_ਅੰਤ
}

// Check if the given UAS ID matches the stored one, or store it if the storage is empty.
// The storage is write-once, so once a UAS ID is stored it cannot be changed (except by erasing the flash).
Bid_Store_ਨਤੀਜਾ BidStorageBackend::checkOrStore(uint8_t ua_type, ਅਚਲ uint8_t uas_id[UAS_ID_ਮਾਪ])
{
	ਜੇ (uas_Id_Is_ਖਾਲੀ_ਥਾਵਾਂ(uas_id)) {
		ਵਾਪਸ Bid_Store_ਨਤੀਜਾ::InvalidBasicId;
	}

#ਜੇ defined(__PX4_NUTTX)
	ਅਚਲ ਅੱਖਰ *ਰਸਤਾ = ਖਾਲੀ_ਸੂਚਕ;

	ਜੇ (px4_mtd_query("MTD_ID", ਖਾਲੀ_ਸੂਚਕ, &ਰਸਤਾ) != 0 || ਰਸਤਾ == ਖਾਲੀ_ਸੂਚਕ) {
		ਵਾਪਸ Bid_Store_ਨਤੀਜਾ::StorageUnavailable;
	}

	ਪੂਰਨ fd = open(ਰਸਤਾ, O_RDWR);

	ਜੇ (fd < 0) {
		ਵਾਪਸ Bid_Store_ਨਤੀਜਾ::StorageUnavailable;
	}

	Uas_Id_ਰਿਕਾਰਡ existing{};

	ਜੇ (ਪਠ(fd, &existing, ਆਕਾਰ(existing)) != ਆਕਾਰ(existing)) {
		close(fd);
		ਵਾਪਸ Bid_Store_ਨਤੀਜਾ::StorageUnavailable;
	}

	ਜੇ (ਰਿਕਾਰਡ_Valid(existing)) {
		ਅਚਲ Bid_Store_ਨਤੀਜਾ ਨਤੀਜਾ = ਰਿਕਾਰਡ_Matches(existing, uas_id)
					      ? Bid_Store_ਨਤੀਜਾ::AlreadyLocked
					      : Bid_Store_ਨਤੀਜਾ::AlreadyLockedDifferent;
		close(fd);
		ਵਾਪਸ ਨਤੀਜਾ;
	}

	ਜੇ (!ਰਿਕਾਰਡ_Blank(existing)) {
		close(fd);
		ਵਾਪਸ Bid_Store_ਨਤੀਜਾ::Invalid_ਰਿਕਾਰਡ;
	}

	Uas_Id_ਰਿਕਾਰਡ ਰਿਕਾਰਡ{};
	ਰਿਕਾਰਡ.magic = ODID_UASID_MAGIC;
	ਰਿਕਾਰਡ.ਸੰਸਕਰਣ = ODID_UASID_ਰਿਕਾਰਡ_ਸੰਸਕਰਣ;
	ਰਿਕਾਰਡ.id_type = UAS_ID_TYPE_SERIAL_ਸੰਖਿਆ;
	ਰਿਕਾਰਡ.ua_type = ua_type;
	ਸਮ੍ਰਿਤੀ_ਨਕਲ(ਰਿਕਾਰਡ.uas_id, uas_id, UAS_ID_ਮਾਪ);
	ਰਿਕਾਰਡ.ਚੱਕਰੀ_ਜਾਂਚ = ਰਿਕਾਰਡ_ਚੱਕਰੀ_ਜਾਂਚ(ਰਿਕਾਰਡ);

	ਅਚਲ uint32_t magic = ਰਿਕਾਰਡ.magic;
	ਰਿਕਾਰਡ.magic = 0xffffffff;

	ਅਚਲ ਤਾਰਕਿਕ written = lseek(fd, ਆਕਾਰ(ਰਿਕਾਰਡ.magic), SEEK_SET) == static_cast<off_t>(ਆਕਾਰ(ਰਿਕਾਰਡ.magic))
			     && writeAll(fd, reinterpret_cast<ਅਚਲ uint8_t *>(&ਰਿਕਾਰਡ) + ਆਕਾਰ(ਰਿਕਾਰਡ.magic),
					 ਆਕਾਰ(ਰਿਕਾਰਡ) - ਆਕਾਰ(ਰਿਕਾਰਡ.magic))
			     && fsync(fd) == 0
			     && lseek(fd, 0, SEEK_SET) == 0
			     && writeAll(fd, &magic, ਆਕਾਰ(magic))
			     && fsync(fd) == 0;

	close(fd);

	ਵਾਪਸ written ? Bid_Store_ਨਤੀਜਾ::Stored : Bid_Store_ਨਤੀਜਾ::WriteError;
#ਨਹੀਂਤਾਂ
	ਵਾਪਸ Bid_Store_ਨਤੀਜਾ::Unsupported;
#ਜੇ_ਅੰਤ
}

} // namespace

// Public interface that uses the backend implementation.
Bid_ਤਾਲਾ_Info BidStorage::get()
{
	BidStorageBackend storage;
	ਵਾਪਸ storage.get();
}

Bid_Store_ਨਤੀਜਾ BidStorage::checkOrStore(uint8_t ua_type, ਅਚਲ uint8_t uas_id[UAS_ID_ਮਾਪ])
{
	BidStorageBackend storage;
	ਵਾਪਸ storage.checkOrStore(ua_type, uas_id);
}

//
Bid_Store_ਨਤੀਜਾ Bid_ਤਾਲਾ::resolve(uint8_t id_type, uint8_t ua_type, ਅਚਲ uint8_t uas_id[UAS_ID_ਮਾਪ],
				UasId &locked_id)
{
	ਜੇ (id_type != UAS_ID_TYPE_SERIAL_ਸੰਖਿਆ) {
		ਵਾਪਸ Bid_Store_ਨਤੀਜਾ::InvalidBasicId;
	}

	ਜੇ (!_loaded) {
		_info = BidStorage::get();
		_loaded = ਸੱਚ;
	}

	ਜੇ (_info.locked()) {
		locked_id = _info.id;
		ਵਾਪਸ Bid_Store_ਨਤੀਜਾ::AlreadyLocked;
	}

	ਜੇ (_info.state != Bid_ਤਾਲਾ_State::ਖਾਲੀ_ਥਾਵਾਂ_2) {
		ਚੋਣ (_info.state) {
		ਹਾਲਤ Bid_ਤਾਲਾ_State::Invalid_ਰਿਕਾਰਡ:
			ਵਾਪਸ Bid_Store_ਨਤੀਜਾ::Invalid_ਰਿਕਾਰਡ;

		ਹਾਲਤ Bid_ਤਾਲਾ_State::StorageUnavailable:
			ਵਾਪਸ Bid_Store_ਨਤੀਜਾ::StorageUnavailable;

		ਹਾਲਤ Bid_ਤਾਲਾ_State::WriteError:
			ਵਾਪਸ Bid_Store_ਨਤੀਜਾ::WriteError;

		ਹਾਲਤ Bid_ਤਾਲਾ_State::Unsupported:
			ਵਾਪਸ Bid_Store_ਨਤੀਜਾ::Unsupported;

		ਹਾਲਤ Bid_ਤਾਲਾ_State::ਖਾਲੀ_ਥਾਵਾਂ_2:
		ਹਾਲਤ Bid_ਤਾਲਾ_State::Locked:
			ਰੋਕੋ;
		}
	}

	ਅਚਲ Bid_Store_ਨਤੀਜਾ ਨਤੀਜਾ = BidStorage::checkOrStore(ua_type, uas_id);

	ਚੋਣ (ਨਤੀਜਾ) {
	ਹਾਲਤ Bid_Store_ਨਤੀਜਾ::Stored:
		_info.state = Bid_ਤਾਲਾ_State::Locked;
		_info.id.ua_type = ua_type;
		ਸਮ੍ਰਿਤੀ_ਨਕਲ(_info.id.uas_id, uas_id, UAS_ID_ਮਾਪ);
		locked_id = _info.id;
		ਵਾਪਸ ਨਤੀਜਾ;

	ਹਾਲਤ Bid_Store_ਨਤੀਜਾ::AlreadyLocked:
	ਹਾਲਤ Bid_Store_ਨਤੀਜਾ::AlreadyLockedDifferent: {
			_info = BidStorage::get();

			ਜੇ (_info.locked()) {
				locked_id = _info.id;
				ਵਾਪਸ Bid_Store_ਨਤੀਜਾ::AlreadyLocked;
			}

			ਵਾਪਸ Bid_Store_ਨਤੀਜਾ::Invalid_ਰਿਕਾਰਡ;
		}

	ਹਾਲਤ Bid_Store_ਨਤੀਜਾ::InvalidBasicId:
	ਹਾਲਤ Bid_Store_ਨਤੀਜਾ::StorageUnavailable:
	ਹਾਲਤ Bid_Store_ਨਤੀਜਾ::Invalid_ਰਿਕਾਰਡ:
	ਹਾਲਤ Bid_Store_ਨਤੀਜਾ::WriteError:
	ਹਾਲਤ Bid_Store_ਨਤੀਜਾ::Unsupported:
		ਵਾਪਸ ਨਤੀਜਾ;
	}

	ਵਾਪਸ Bid_Store_ਨਤੀਜਾ::Invalid_ਰਿਕਾਰਡ;
}

} // namespace open_drone_id
