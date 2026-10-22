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
#không_thì
#include <crc32.h>
#endif

namespace open_drone_id
{
namespace
{

static constexpr uint32_t ODID_UASID_MAGIC = 0x4449444f; // "ODID"
static constexpr uint8_t ODID_UASID_RECORD_VERSION = 1;

struct __attribute__((packed)) UasIdRecord {
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
		cho (unsigned i = 0; i < UAS_ID_SIZE; ++i) {
			nếu (uas_id[i] != 0) {
				trả_về false;
			}
		}

		trả_về true;
	}

	static uint32_t recordCrc(const UasIdRecord &record)
	{
		trả_về crc32(reinterpret_cast<const uint8_t *>(&record.version),
			     offsetof(UasIdRecord, crc) - offsetof(UasIdRecord, version));
	}

	static bool recordValid(const UasIdRecord &record)
	{
		trả_về record.magic == ODID_UASID_MAGIC
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

		cho (unsigned i = 0; i < sizeof(record); ++i) {
			nếu (bytes[i] != 0xff) {
				trả_về false;
			}
		}

		trả_về true;
	}

	static bool recordMatches(const UasIdRecord &record, const uint8_t uas_id[UAS_ID_SIZE])
	{
		trả_về memcmp(record.uas_id, uas_id, UAS_ID_SIZE) == 0;
	}

	// Write the entire buffer to the file descriptor, handling short writes.
	static bool writeAll(số_nguyên fd, const void *buffer, size_t size)
	{
		const uint8_t *bytes = static_cast<const uint8_t *>(buffer);

		trong_khi (size > 0) {
			const ssize_t written = write(fd, bytes, size);

			nếu (written <= 0) {
				trả_về false;
			}

			bytes += written;
			size -= written;
		}

		trả_về true;
	}

};

static_assert(sizeof(UasIdRecord) <= 256, "OpenDroneID UAS ID record must fit in /fs/mtd_id");

// The UAS ID storage is only supported on PX4 targets with NuttX and an MTD partition named "ID".
// On other platforms, the get() method will return StorageUnavailable and the checkOrStore() method will return Unsupported.
BidLockInfo BidStorageBackend::get()
{
#nếu defined(__PX4_NUTTX)
	const char *path = nullptr;

	nếu (px4_mtd_query("MTD_ID", nullptr, &path) != 0 || path == nullptr) {
		trả_về {BidLockState::StorageUnavailable, {}};
	}

	số_nguyên fd = open(path, O_RDONLY);

	nếu (fd < 0) {
		trả_về {BidLockState::StorageUnavailable, {}};
	}

	UasIdRecord record{};

	nếu (read(fd, &record, sizeof(record)) != sizeof(record)) {
		close(fd);
		trả_về {BidLockState::StorageUnavailable, {}};
	}

	close(fd);

	nếu (recordValid(record)) {
		BidLockInfo info{BidLockState::Locked, {}};
		info.id.ua_type = record.ua_type;
		memcpy(info.id.uas_id, record.uas_id, UAS_ID_SIZE);
		trả_về info;
	}

	nếu (recordBlank(record)) {
		trả_về {BidLockState::Empty, {}};
	}

	trả_về {BidLockState::InvalidRecord, {}};
#không_thì
	trả_về {BidLockState::Unsupported, {}};
#endif
}

// Check if the given UAS ID matches the stored one, or store it if the storage is empty.
// The storage is write-once, so once a UAS ID is stored it cannot be changed (except by erasing the flash).
BidStoreResult BidStorageBackend::checkOrStore(uint8_t ua_type, const uint8_t uas_id[UAS_ID_SIZE])
{
	nếu (uasIdIsEmpty(uas_id)) {
		trả_về BidStoreResult::InvalidBasicId;
	}

#nếu defined(__PX4_NUTTX)
	const char *path = nullptr;

	nếu (px4_mtd_query("MTD_ID", nullptr, &path) != 0 || path == nullptr) {
		trả_về BidStoreResult::StorageUnavailable;
	}

	số_nguyên fd = open(path, O_RDWR);

	nếu (fd < 0) {
		trả_về BidStoreResult::StorageUnavailable;
	}

	UasIdRecord existing{};

	nếu (read(fd, &existing, sizeof(existing)) != sizeof(existing)) {
		close(fd);
		trả_về BidStoreResult::StorageUnavailable;
	}

	nếu (recordValid(existing)) {
		const BidStoreResult result = recordMatches(existing, uas_id)
					      ? BidStoreResult::AlreadyLocked
					      : BidStoreResult::AlreadyLockedDifferent;
		close(fd);
		trả_về result;
	}

	nếu (!recordBlank(existing)) {
		close(fd);
		trả_về BidStoreResult::InvalidRecord;
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

	const bool written = lseek(fd, sizeof(record.magic), SEEK_SET) == static_cast<off_t>(sizeof(record.magic))
			     && writeAll(fd, reinterpret_cast<const uint8_t *>(&record) + sizeof(record.magic),
					 sizeof(record) - sizeof(record.magic))
			     && fsync(fd) == 0
			     && lseek(fd, 0, SEEK_SET) == 0
			     && writeAll(fd, &magic, sizeof(magic))
			     && fsync(fd) == 0;

	close(fd);

	trả_về written ? BidStoreResult::Stored : BidStoreResult::WriteError;
#không_thì
	trả_về BidStoreResult::Unsupported;
#endif
}

} // namespace

// Public interface that uses the backend implementation.
BidLockInfo BidStorage::get()
{
	BidStorageBackend storage;
	trả_về storage.get();
}

BidStoreResult BidStorage::checkOrStore(uint8_t ua_type, const uint8_t uas_id[UAS_ID_SIZE])
{
	BidStorageBackend storage;
	trả_về storage.checkOrStore(ua_type, uas_id);
}

//
BidStoreResult BidLock::resolve(uint8_t id_type, uint8_t ua_type, const uint8_t uas_id[UAS_ID_SIZE],
				UasId &locked_id)
{
	nếu (id_type != UAS_ID_TYPE_SERIAL_NUMBER) {
		trả_về BidStoreResult::InvalidBasicId;
	}

	nếu (!_loaded) {
		_info = BidStorage::get();
		_loaded = true;
	}

	nếu (_info.locked()) {
		locked_id = _info.id;
		trả_về BidStoreResult::AlreadyLocked;
	}

	nếu (_info.state != BidLockState::Empty) {
		switch (_info.state) {
		case BidLockState::InvalidRecord:
			trả_về BidStoreResult::InvalidRecord;

		case BidLockState::StorageUnavailable:
			trả_về BidStoreResult::StorageUnavailable;

		case BidLockState::WriteError:
			trả_về BidStoreResult::WriteError;

		case BidLockState::Unsupported:
			trả_về BidStoreResult::Unsupported;

		case BidLockState::Empty:
		case BidLockState::Locked:
			break;
		}
	}

	const BidStoreResult result = BidStorage::checkOrStore(ua_type, uas_id);

	switch (result) {
	case BidStoreResult::Stored:
		_info.state = BidLockState::Locked;
		_info.id.ua_type = ua_type;
		memcpy(_info.id.uas_id, uas_id, UAS_ID_SIZE);
		locked_id = _info.id;
		trả_về result;

	case BidStoreResult::AlreadyLocked:
	case BidStoreResult::AlreadyLockedDifferent: {
			_info = BidStorage::get();

			nếu (_info.locked()) {
				locked_id = _info.id;
				trả_về BidStoreResult::AlreadyLocked;
			}

			trả_về BidStoreResult::InvalidRecord;
		}

	case BidStoreResult::InvalidBasicId:
	case BidStoreResult::StorageUnavailable:
	case BidStoreResult::InvalidRecord:
	case BidStoreResult::WriteError:
	case BidStoreResult::Unsupported:
		trả_về result;
	}

	trả_về BidStoreResult::InvalidRecord;
}

} // namespace open_drone_id
