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
#아니면
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
		위해 (unsigned i = 0; i < UAS_ID_SIZE; ++i) {
			만약 (uas_id[i] != 0) {
				반환 false;
			}
		}

		반환 true;
	}

	static uint32_t recordCrc(const UasIdRecord &record)
	{
		반환 crc32(reinterpret_cast<const uint8_t *>(&record.version),
			     offsetof(UasIdRecord, crc) - offsetof(UasIdRecord, version));
	}

	static bool recordValid(const UasIdRecord &record)
	{
		반환 record.magic == ODID_UASID_MAGIC
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

		위해 (unsigned i = 0; i < sizeof(record); ++i) {
			만약 (bytes[i] != 0xff) {
				반환 false;
			}
		}

		반환 true;
	}

	static bool recordMatches(const UasIdRecord &record, const uint8_t uas_id[UAS_ID_SIZE])
	{
		반환 memcmp(record.uas_id, uas_id, UAS_ID_SIZE) == 0;
	}

	// Write the entire buffer to the file descriptor, handling short writes.
	static bool writeAll(정수 fd, const void *buffer, size_t size)
	{
		const uint8_t *bytes = static_cast<const uint8_t *>(buffer);

		동안 (size > 0) {
			const ssize_t written = write(fd, bytes, size);

			만약 (written <= 0) {
				반환 false;
			}

			bytes += written;
			size -= written;
		}

		반환 true;
	}

};

static_assert(sizeof(UasIdRecord) <= 256, "OpenDroneID UAS ID record must fit in /fs/mtd_id");

// The UAS ID storage is only supported on PX4 targets with NuttX and an MTD partition named "ID".
// On other platforms, the get() method will return StorageUnavailable and the checkOrStore() method will return Unsupported.
BidLockInfo BidStorageBackend::get()
{
#만약 defined(__PX4_NUTTX)
	const char *path = nullptr;

	만약 (px4_mtd_query("MTD_ID", nullptr, &path) != 0 || path == nullptr) {
		반환 {BidLockState::StorageUnavailable, {}};
	}

	정수 fd = open(path, O_RDONLY);

	만약 (fd < 0) {
		반환 {BidLockState::StorageUnavailable, {}};
	}

	UasIdRecord record{};

	만약 (read(fd, &record, sizeof(record)) != sizeof(record)) {
		close(fd);
		반환 {BidLockState::StorageUnavailable, {}};
	}

	close(fd);

	만약 (recordValid(record)) {
		BidLockInfo info{BidLockState::Locked, {}};
		info.id.ua_type = record.ua_type;
		memcpy(info.id.uas_id, record.uas_id, UAS_ID_SIZE);
		반환 info;
	}

	만약 (recordBlank(record)) {
		반환 {BidLockState::Empty, {}};
	}

	반환 {BidLockState::InvalidRecord, {}};
#아니면
	반환 {BidLockState::Unsupported, {}};
#endif
}

// Check if the given UAS ID matches the stored one, or store it if the storage is empty.
// The storage is write-once, so once a UAS ID is stored it cannot be changed (except by erasing the flash).
BidStoreResult BidStorageBackend::checkOrStore(uint8_t ua_type, const uint8_t uas_id[UAS_ID_SIZE])
{
	만약 (uasIdIsEmpty(uas_id)) {
		반환 BidStoreResult::InvalidBasicId;
	}

#만약 defined(__PX4_NUTTX)
	const char *path = nullptr;

	만약 (px4_mtd_query("MTD_ID", nullptr, &path) != 0 || path == nullptr) {
		반환 BidStoreResult::StorageUnavailable;
	}

	정수 fd = open(path, O_RDWR);

	만약 (fd < 0) {
		반환 BidStoreResult::StorageUnavailable;
	}

	UasIdRecord existing{};

	만약 (read(fd, &existing, sizeof(existing)) != sizeof(existing)) {
		close(fd);
		반환 BidStoreResult::StorageUnavailable;
	}

	만약 (recordValid(existing)) {
		const BidStoreResult result = recordMatches(existing, uas_id)
					      ? BidStoreResult::AlreadyLocked
					      : BidStoreResult::AlreadyLockedDifferent;
		close(fd);
		반환 result;
	}

	만약 (!recordBlank(existing)) {
		close(fd);
		반환 BidStoreResult::InvalidRecord;
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

	반환 written ? BidStoreResult::Stored : BidStoreResult::WriteError;
#아니면
	반환 BidStoreResult::Unsupported;
#endif
}

} // namespace

// Public interface that uses the backend implementation.
BidLockInfo BidStorage::get()
{
	BidStorageBackend storage;
	반환 storage.get();
}

BidStoreResult BidStorage::checkOrStore(uint8_t ua_type, const uint8_t uas_id[UAS_ID_SIZE])
{
	BidStorageBackend storage;
	반환 storage.checkOrStore(ua_type, uas_id);
}

//
BidStoreResult BidLock::resolve(uint8_t id_type, uint8_t ua_type, const uint8_t uas_id[UAS_ID_SIZE],
				UasId &locked_id)
{
	만약 (id_type != UAS_ID_TYPE_SERIAL_NUMBER) {
		반환 BidStoreResult::InvalidBasicId;
	}

	만약 (!_loaded) {
		_info = BidStorage::get();
		_loaded = true;
	}

	만약 (_info.locked()) {
		locked_id = _info.id;
		반환 BidStoreResult::AlreadyLocked;
	}

	만약 (_info.state != BidLockState::Empty) {
		switch (_info.state) {
		case BidLockState::InvalidRecord:
			반환 BidStoreResult::InvalidRecord;

		case BidLockState::StorageUnavailable:
			반환 BidStoreResult::StorageUnavailable;

		case BidLockState::WriteError:
			반환 BidStoreResult::WriteError;

		case BidLockState::Unsupported:
			반환 BidStoreResult::Unsupported;

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
		반환 result;

	case BidStoreResult::AlreadyLocked:
	case BidStoreResult::AlreadyLockedDifferent: {
			_info = BidStorage::get();

			만약 (_info.locked()) {
				locked_id = _info.id;
				반환 BidStoreResult::AlreadyLocked;
			}

			반환 BidStoreResult::InvalidRecord;
		}

	case BidStoreResult::InvalidBasicId:
	case BidStoreResult::StorageUnavailable:
	case BidStoreResult::InvalidRecord:
	case BidStoreResult::WriteError:
	case BidStoreResult::Unsupported:
		반환 result;
	}

	반환 BidStoreResult::InvalidRecord;
}

} // namespace open_drone_id
