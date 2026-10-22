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

#अन्तर्भाव "odid_bid_storage.hpp"

#अन्तर्भाव <fcntl.h>
#अन्तर्भाव <stddef.h>
#अन्तर्भाव <string.h>
#अन्तर्भाव <unistd.h>

#ifdef __PX4_NUTTX
#अन्तर्भाव <nuttx/crc32.h>
#अन्तर्भाव <px4_platform_common/px4_manifest.h>
#अञ्ञथा
#अन्तर्भाव <crc32.h>
#endif

नामाकाश open_drone_id
{
नामाकाश
{

स्थिर constexpr uint32_t ODID_UASID_MAGIC = 0x4449444f; // "ODID"
स्थिर constexpr uint8_t ODID_UASID_RECORD_VERSION = 1;

संरचना __attribute__((packed)) UasIdRecord {
	uint32_t magic;
	uint8_t version;
	uint8_t id_type;
	uint8_t ua_type;
	uint8_t reserved;
	uint8_t uas_id[UAS_ID_SIZE];
	uint32_t crc;
};

वग्ग BidStorageBackend
{
सार्वजनिक:
	BidLockInfo get();
	BidStoreResult checkOrStore(uint8_t ua_type, ध्रुव uint8_t uas_id[UAS_ID_SIZE]);

गुप्त:
	स्थिर तार्किक uasIdIsEmpty(ध्रुव uint8_t uas_id[UAS_ID_SIZE])
	{
		अत्थाय (unsigned i = 0; i < UAS_ID_SIZE; ++i) {
			सचे (uas_id[i] != 0) {
				पटिनिवत्त मिच्छा;
			}
		}

		पटिनिवत्त सच्च;
	}

	स्थिर uint32_t recordCrc(ध्रुव UasIdRecord &record)
	{
		पटिनिवत्त crc32(reinterpret_cast<ध्रुव uint8_t *>(&record.version),
			     offsetof(UasIdRecord, crc) - offsetof(UasIdRecord, version));
	}

	स्थिर तार्किक recordValid(ध्रुव UasIdRecord &record)
	{
		पटिनिवत्त record.magic == ODID_UASID_MAGIC
		       && record.version == ODID_UASID_RECORD_VERSION
		       && record.id_type == UAS_ID_TYPE_SERIAL_सङ्ख्या
		       && !uasIdIsEmpty(record.uas_id)
		       && record.crc == recordCrc(record);
	}

	// Check if the record is all 0xff, which is the erased state of flash.
	// This is needed to distinguish between an unwritten record and a record that was written with all bytes set to 0 (which would be invalid but not unwritten).
	स्थिर तार्किक recordBlank(ध्रुव UasIdRecord &record)
	{
		ध्रुव uint8_t *bytes = reinterpret_cast<ध्रुव uint8_t *>(&record);

		अत्थाय (unsigned i = 0; i < परिमाण(record); ++i) {
			सचे (bytes[i] != 0xff) {
				पटिनिवत्त मिच्छा;
			}
		}

		पटिनिवत्त सच्च;
	}

	स्थिर तार्किक recordMatches(ध्रुव UasIdRecord &record, ध्रुव uint8_t uas_id[UAS_ID_SIZE])
	{
		पटिनिवत्त memcmp(record.uas_id, uas_id, UAS_ID_SIZE) == 0;
	}

	// Write the entire buffer to the file descriptor, handling short writes.
	स्थिर तार्किक writeAll(पूर्णाङ्क fd, ध्रुव शून्य *buffer, size_t size)
	{
		ध्रुव uint8_t *bytes = static_cast<ध्रुव uint8_t *>(buffer);

		याव (size > 0) {
			ध्रुव ssize_t written = लिख(fd, bytes, size);

			सचे (written <= 0) {
				पटिनिवत्त मिच्छा;
			}

			bytes += written;
			size -= written;
		}

		पटिनिवत्त सच्च;
	}

};

static_assert(परिमाण(UasIdRecord) <= 256, "OpenDroneID UAS ID record must fit in /fs/mtd_id");

// The UAS ID storage is only supported on PX4 targets with NuttX and an MTD partition named "ID".
// On other platforms, the get() method will return StorageUnavailable and the checkOrStore() method will return Unsupported.
BidLockInfo BidStorageBackend::get()
{
#सचे defined(__PX4_NUTTX)
	ध्रुव वर्ण *path = nullptr;

	सचे (px4_mtd_query("MTD_ID", nullptr, &path) != 0 || path == nullptr) {
		पटिनिवत्त {BidLockState::StorageUnavailable, {}};
	}

	पूर्णाङ्क fd = open(path, O_RDONLY);

	सचे (fd < 0) {
		पटिनिवत्त {BidLockState::StorageUnavailable, {}};
	}

	UasIdRecord record{};

	सचे (पठ(fd, &record, परिमाण(record)) != परिमाण(record)) {
		close(fd);
		पटिनिवत्त {BidLockState::StorageUnavailable, {}};
	}

	close(fd);

	सचे (recordValid(record)) {
		BidLockInfo info{BidLockState::Locked, {}};
		info.id.ua_type = record.ua_type;
		memcpy(info.id.uas_id, record.uas_id, UAS_ID_SIZE);
		पटिनिवत्त info;
	}

	सचे (recordBlank(record)) {
		पटिनिवत्त {BidLockState::Empty, {}};
	}

	पटिनिवत्त {BidLockState::InvalidRecord, {}};
#अञ्ञथा
	पटिनिवत्त {BidLockState::Unsupported, {}};
#endif
}

// Check if the given UAS ID matches the stored one, or store it if the storage is empty.
// The storage is write-once, so once a UAS ID is stored it cannot be changed (except by erasing the flash).
BidStoreResult BidStorageBackend::checkOrStore(uint8_t ua_type, ध्रुव uint8_t uas_id[UAS_ID_SIZE])
{
	सचे (uasIdIsEmpty(uas_id)) {
		पटिनिवत्त BidStoreResult::InvalidBasicId;
	}

#सचे defined(__PX4_NUTTX)
	ध्रुव वर्ण *path = nullptr;

	सचे (px4_mtd_query("MTD_ID", nullptr, &path) != 0 || path == nullptr) {
		पटिनिवत्त BidStoreResult::StorageUnavailable;
	}

	पूर्णाङ्क fd = open(path, O_RDWR);

	सचे (fd < 0) {
		पटिनिवत्त BidStoreResult::StorageUnavailable;
	}

	UasIdRecord existing{};

	सचे (पठ(fd, &existing, परिमाण(existing)) != परिमाण(existing)) {
		close(fd);
		पटिनिवत्त BidStoreResult::StorageUnavailable;
	}

	सचे (recordValid(existing)) {
		ध्रुव BidStoreResult result = recordMatches(existing, uas_id)
					      ? BidStoreResult::AlreadyLocked
					      : BidStoreResult::AlreadyLockedDifferent;
		close(fd);
		पटिनिवत्त result;
	}

	सचे (!recordBlank(existing)) {
		close(fd);
		पटिनिवत्त BidStoreResult::InvalidRecord;
	}

	UasIdRecord record{};
	record.magic = ODID_UASID_MAGIC;
	record.version = ODID_UASID_RECORD_VERSION;
	record.id_type = UAS_ID_TYPE_SERIAL_सङ्ख्या;
	record.ua_type = ua_type;
	memcpy(record.uas_id, uas_id, UAS_ID_SIZE);
	record.crc = recordCrc(record);

	ध्रुव uint32_t magic = record.magic;
	record.magic = 0xffffffff;

	ध्रुव तार्किक written = lseek(fd, परिमाण(record.magic), SEEK_SET) == static_cast<off_t>(परिमाण(record.magic))
			     && writeAll(fd, reinterpret_cast<ध्रुव uint8_t *>(&record) + परिमाण(record.magic),
					 परिमाण(record) - परिमाण(record.magic))
			     && fsync(fd) == 0
			     && lseek(fd, 0, SEEK_SET) == 0
			     && writeAll(fd, &magic, परिमाण(magic))
			     && fsync(fd) == 0;

	close(fd);

	पटिनिवत्त written ? BidStoreResult::Stored : BidStoreResult::WriteError;
#अञ्ञथा
	पटिनिवत्त BidStoreResult::Unsupported;
#endif
}

} // namespace

// Public interface that uses the backend implementation.
BidLockInfo BidStorage::get()
{
	BidStorageBackend storage;
	पटिनिवत्त storage.get();
}

BidStoreResult BidStorage::checkOrStore(uint8_t ua_type, ध्रुव uint8_t uas_id[UAS_ID_SIZE])
{
	BidStorageBackend storage;
	पटिनिवत्त storage.checkOrStore(ua_type, uas_id);
}

//
BidStoreResult BidLock::resolve(uint8_t id_type, uint8_t ua_type, ध्रुव uint8_t uas_id[UAS_ID_SIZE],
				UasId &locked_id)
{
	सचे (id_type != UAS_ID_TYPE_SERIAL_सङ्ख्या) {
		पटिनिवत्त BidStoreResult::InvalidBasicId;
	}

	सचे (!_loaded) {
		_info = BidStorage::get();
		_loaded = सच्च;
	}

	सचे (_info.locked()) {
		locked_id = _info.id;
		पटिनिवत्त BidStoreResult::AlreadyLocked;
	}

	सचे (_info.state != BidLockState::Empty) {
		विकल्प (_info.state) {
		पक्ष BidLockState::InvalidRecord:
			पटिनिवत्त BidStoreResult::InvalidRecord;

		पक्ष BidLockState::StorageUnavailable:
			पटिनिवत्त BidStoreResult::StorageUnavailable;

		पक्ष BidLockState::WriteError:
			पटिनिवत्त BidStoreResult::WriteError;

		पक्ष BidLockState::Unsupported:
			पटिनिवत्त BidStoreResult::Unsupported;

		पक्ष BidLockState::Empty:
		पक्ष BidLockState::Locked:
			भङ्ग;
		}
	}

	ध्रुव BidStoreResult result = BidStorage::checkOrStore(ua_type, uas_id);

	विकल्प (result) {
	पक्ष BidStoreResult::Stored:
		_info.state = BidLockState::Locked;
		_info.id.ua_type = ua_type;
		memcpy(_info.id.uas_id, uas_id, UAS_ID_SIZE);
		locked_id = _info.id;
		पटिनिवत्त result;

	पक्ष BidStoreResult::AlreadyLocked:
	पक्ष BidStoreResult::AlreadyLockedDifferent: {
			_info = BidStorage::get();

			सचे (_info.locked()) {
				locked_id = _info.id;
				पटिनिवत्त BidStoreResult::AlreadyLocked;
			}

			पटिनिवत्त BidStoreResult::InvalidRecord;
		}

	पक्ष BidStoreResult::InvalidBasicId:
	पक्ष BidStoreResult::StorageUnavailable:
	पक्ष BidStoreResult::InvalidRecord:
	पक्ष BidStoreResult::WriteError:
	पक्ष BidStoreResult::Unsupported:
		पटिनिवत्त result;
	}

	पटिनिवत्त BidStoreResult::InvalidRecord;
}

} // namespace open_drone_id
