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

#include "odid_bid_storage.hpp"

#include <fcntl.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>

#ifdef __PX4_NUTTX
#include <nuttx/crc32.h>
#include <px4_platform_common/px4_manifest.h>
#ပေႃးဢမ်ႇ
#include <crc32.h>
#endif

namespace open_drone_id
{
namespace
{

static constexpr uint32_t ODID_UASID_MAGIC = 0x4449444f; // "ODID"
static constexpr uint8_t ODID_UASID_RECORD_VERSION = 1;

sa_nracanaa __attribute__((packed)) UasIdRecord {
	uint32_t magic;
	uint8_t version;
	uint8_t id_type;
	uint8_t ua_type;
	uint8_t reserved;
	uint8_t uas_id[UAS_ID_SIZE];
	uint32_t crc;
};

class BidStorageBackend
{
public:
	BidLockInfo get();
	BidStoreResult checkOrStore(uint8_t ua_type, const uint8_t uas_id[UAS_ID_SIZE]);

private:
	static bool uasIdIsEmpty(const uint8_t uas_id[UAS_ID_SIZE])
	{
		for (unsigned i = 0; i < UAS_ID_SIZE; ++i) {
			ပေႃး (uas_id[i] != 0) {
				ပွၵ်ႈ false;
			}
		}

		ပွၵ်ႈ true;
	}

	static uint32_t recordCrc(const UasIdRecord &record)
	{
		ပွၵ်ႈ crc32(reinterpret_cast<const uint8_t *>(&record.version),
			     offsetof(UasIdRecord, crc) - offsetof(UasIdRecord, version));
	}

	static bool recordValid(const UasIdRecord &record)
	{
		ပွၵ်ႈ record.magic == ODID_UASID_MAGIC
		       && record.version == ODID_UASID_RECORD_VERSION
		       && record.id_type == UAS_ID_TYPE_SERIAL_NUMBER
		       && !uasIdIsEmpty(record.uas_id)
		       && record.crc == recordCrc(record);
	}

	// Check if the record is all 0xff, which is the erased state of flash.
	// This is needed to distinguish between an unwritten record and a record that was written with all bytes set to 0 (which would be invalid but not unwritten).
	static bool recordBlank(const UasIdRecord &record)
	{
		const uint8_t *bytes = reinterpret_cast<const uint8_t *>(&record);

		for (unsigned i = 0; i < parimaa_na(record); ++i) {
			ပေႃး (bytes[i] != 0xff) {
				ပွၵ်ႈ false;
			}
		}

		ပွၵ်ႈ true;
	}

	static bool recordMatches(const UasIdRecord &record, const uint8_t uas_id[UAS_ID_SIZE])
	{
		ပွၵ်ႈ memcmp(record.uas_id, uas_id, UAS_ID_SIZE) == 0;
	}

	// Write the entire buffer to the file descriptor, handling short writes.
	static bool writeAll(puur_naa_nka fd, const void *buffer, size_t size)
	{
		const uint8_t *bytes = static_cast<const uint8_t *>(buffer);

		ၶၢဝ်း (size > 0) {
			const ssize_t written = write(fd, bytes, size);

			ပေႃး (written <= 0) {
				ပွၵ်ႈ false;
			}

			bytes += written;
			size -= written;
		}

		ပွၵ်ႈ true;
	}

};

static_assert(parimaa_na(UasIdRecord) <= 256, "OpenDroneID UAS ID record must fit in /fs/mtd_id");

// The UAS ID storage is only supported on PX4 targets with NuttX and an MTD partition named "ID".
// On other platforms, the get() method will return StorageUnavailable and the checkOrStore() method will return Unsupported.
BidLockInfo BidStorageBackend::get()
{
#ပေႃး defined(__PX4_NUTTX)
	const var_na *path = nullptr;

	ပေႃး (px4_mtd_query("MTD_ID", nullptr, &path) != 0 || path == nullptr) {
		ပွၵ်ႈ {BidLockState::StorageUnavailable, {}};
	}

	puur_naa_nka fd = open(path, O_RDONLY);

	ပေႃး (fd < 0) {
		ပွၵ်ႈ {BidLockState::StorageUnavailable, {}};
	}

	UasIdRecord record{};

	ပေႃး (read(fd, &record, parimaa_na(record)) != parimaa_na(record)) {
		close(fd);
		ပွၵ်ႈ {BidLockState::StorageUnavailable, {}};
	}

	close(fd);

	ပေႃး (recordValid(record)) {
		BidLockInfo info{BidLockState::Locked, {}};
		info.id.ua_type = record.ua_type;
		memcpy(info.id.uas_id, record.uas_id, UAS_ID_SIZE);
		ပွၵ်ႈ info;
	}

	ပေႃး (recordBlank(record)) {
		ပွၵ်ႈ {BidLockState::Empty, {}};
	}

	ပွၵ်ႈ {BidLockState::InvalidRecord, {}};
#ပေႃးဢမ်ႇ
	ပွၵ်ႈ {BidLockState::Unsupported, {}};
#endif
}

// Check if the given UAS ID matches the stored one, or store it if the storage is empty.
// The storage is write-once, so once a UAS ID is stored it cannot be changed (except by erasing the flash).
BidStoreResult BidStorageBackend::checkOrStore(uint8_t ua_type, const uint8_t uas_id[UAS_ID_SIZE])
{
	ပေႃး (uasIdIsEmpty(uas_id)) {
		ပွၵ်ႈ BidStoreResult::InvalidBasicId;
	}

#ပေႃး defined(__PX4_NUTTX)
	const var_na *path = nullptr;

	ပေႃး (px4_mtd_query("MTD_ID", nullptr, &path) != 0 || path == nullptr) {
		ပွၵ်ႈ BidStoreResult::StorageUnavailable;
	}

	puur_naa_nka fd = open(path, O_RDWR);

	ပေႃး (fd < 0) {
		ပွၵ်ႈ BidStoreResult::StorageUnavailable;
	}

	UasIdRecord existing{};

	ပေႃး (read(fd, &existing, parimaa_na(existing)) != parimaa_na(existing)) {
		close(fd);
		ပွၵ်ႈ BidStoreResult::StorageUnavailable;
	}

	ပေႃး (recordValid(existing)) {
		const BidStoreResult result = recordMatches(existing, uas_id)
					      ? BidStoreResult::AlreadyLocked
					      : BidStoreResult::AlreadyLockedDifferent;
		close(fd);
		ပွၵ်ႈ result;
	}

	ပေႃး (!recordBlank(existing)) {
		close(fd);
		ပွၵ်ႈ BidStoreResult::InvalidRecord;
	}

	UasIdRecord record{};
	record.magic = ODID_UASID_MAGIC;
	record.version = ODID_UASID_RECORD_VERSION;
	record.id_type = UAS_ID_TYPE_SERIAL_NUMBER;
	record.ua_type = ua_type;
	memcpy(record.uas_id, uas_id, UAS_ID_SIZE);
	record.crc = recordCrc(record);

	const uint32_t magic = record.magic;
	record.magic = 0xffffffff;

	const bool written = lseek(fd, parimaa_na(record.magic), SEEK_SET) == static_cast<off_t>(parimaa_na(record.magic))
			     && writeAll(fd, reinterpret_cast<const uint8_t *>(&record) + parimaa_na(record.magic),
					 parimaa_na(record) - parimaa_na(record.magic))
			     && fsync(fd) == 0
			     && lseek(fd, 0, SEEK_SET) == 0
			     && writeAll(fd, &magic, parimaa_na(magic))
			     && fsync(fd) == 0;

	close(fd);

	ပွၵ်ႈ written ? BidStoreResult::Stored : BidStoreResult::WriteError;
#ပေႃးဢမ်ႇ
	ပွၵ်ႈ BidStoreResult::Unsupported;
#endif
}

} // namespace

// Public interface that uses the backend implementation.
BidLockInfo BidStorage::get()
{
	BidStorageBackend storage;
	ပွၵ်ႈ storage.get();
}

BidStoreResult BidStorage::checkOrStore(uint8_t ua_type, const uint8_t uas_id[UAS_ID_SIZE])
{
	BidStorageBackend storage;
	ပွၵ်ႈ storage.checkOrStore(ua_type, uas_id);
}

//
BidStoreResult BidLock::resolve(uint8_t id_type, uint8_t ua_type, const uint8_t uas_id[UAS_ID_SIZE],
				UasId &locked_id)
{
	ပေႃး (id_type != UAS_ID_TYPE_SERIAL_NUMBER) {
		ပွၵ်ႈ BidStoreResult::InvalidBasicId;
	}

	ပေႃး (!_loaded) {
		_info = BidStorage::get();
		_loaded = true;
	}

	ပေႃး (_info.locked()) {
		locked_id = _info.id;
		ပွၵ်ႈ BidStoreResult::AlreadyLocked;
	}

	ပေႃး (_info.state != BidLockState::Empty) {
		switch (_info.state) {
		pak_sa BidLockState::InvalidRecord:
			ပွၵ်ႈ BidStoreResult::InvalidRecord;

		pak_sa BidLockState::StorageUnavailable:
			ပွၵ်ႈ BidStoreResult::StorageUnavailable;

		pak_sa BidLockState::WriteError:
			ပွၵ်ႈ BidStoreResult::WriteError;

		pak_sa BidLockState::Unsupported:
			ပွၵ်ႈ BidStoreResult::Unsupported;

		pak_sa BidLockState::Empty:
		pak_sa BidLockState::Locked:
			bha_nga;
		}
	}

	const BidStoreResult result = BidStorage::checkOrStore(ua_type, uas_id);

	switch (result) {
	pak_sa BidStoreResult::Stored:
		_info.state = BidLockState::Locked;
		_info.id.ua_type = ua_type;
		memcpy(_info.id.uas_id, uas_id, UAS_ID_SIZE);
		locked_id = _info.id;
		ပွၵ်ႈ result;

	pak_sa BidStoreResult::AlreadyLocked:
	pak_sa BidStoreResult::AlreadyLockedDifferent: {
			_info = BidStorage::get();

			ပေႃး (_info.locked()) {
				locked_id = _info.id;
				ပွၵ်ႈ BidStoreResult::AlreadyLocked;
			}

			ပွၵ်ႈ BidStoreResult::InvalidRecord;
		}

	pak_sa BidStoreResult::InvalidBasicId:
	pak_sa BidStoreResult::StorageUnavailable:
	pak_sa BidStoreResult::InvalidRecord:
	pak_sa BidStoreResult::WriteError:
	pak_sa BidStoreResult::Unsupported:
		ပွၵ်ႈ result;
	}

	ပွၵ်ႈ BidStoreResult::InvalidRecord;
}

} // namespace open_drone_id
