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
#अन्यथा
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

वर्ग BidStorageBackend
{
सार्वजनिक:
	BidLockInfo get();
	BidStoreResult checkOrStore(uint8_t ua_type, अचर uint8_t uas_id[UAS_ID_SIZE]);

गुप्त:
	स्थिर तार्किक uasIdIsEmpty(अचर uint8_t uas_id[UAS_ID_SIZE])
	{
		लागि (अचिन्हित i = 0; i < UAS_ID_SIZE; ++i) {
			यदि (uas_id[i] != 0) {
				फर्काऊ असत्य;
			}
		}

		फर्काऊ सत्य;
	}

	स्थिर uint32_t recordCrc(अचर UasIdRecord &record)
	{
		फर्काऊ crc32(reinterpret_cast<अचर uint8_t *>(&record.version),
			     दुरत्व(UasIdRecord, crc) - दुरत्व(UasIdRecord, version));
	}

	स्थिर तार्किक recordValid(अचर UasIdRecord &record)
	{
		फर्काऊ record.magic == ODID_UASID_MAGIC
		       && record.version == ODID_UASID_RECORD_VERSION
		       && record.id_type == UAS_ID_TYPE_SERIAL_सङ्ख्या
		       && !uasIdIsEmpty(record.uas_id)
		       && record.crc == recordCrc(record);
	}

	// Check if the record is all 0xff, which is the erased state of flash.
	// This is needed to distinguish between an unwritten record and a record that was written with all bytes set to 0 (which would be invalid but not unwritten).
	स्थिर तार्किक recordBlank(अचर UasIdRecord &record)
	{
		अचर uint8_t *bytes = reinterpret_cast<अचर uint8_t *>(&record);

		लागि (अचिन्हित i = 0; i < परिमाण(record); ++i) {
			यदि (bytes[i] != 0xff) {
				फर्काऊ असत्य;
			}
		}

		फर्काऊ सत्य;
	}

	स्थिर तार्किक recordMatches(अचर UasIdRecord &record, अचर uint8_t uas_id[UAS_ID_SIZE])
	{
		फर्काऊ स_भेद(record.uas_id, uas_id, UAS_ID_SIZE) == 0;
	}

	// Write the entire buffer to the file descriptor, handling short writes.
	स्थिर तार्किक writeAll(पूर्णाङ्क fd, अचर शून्य *buffer, माप_प्रकार size)
	{
		अचर uint8_t *bytes = static_cast<अचर uint8_t *>(buffer);

		जबसम्म (size > 0) {
			अचर ssize_t written = लिख(fd, bytes, size);

			यदि (written <= 0) {
				फर्काऊ असत्य;
			}

			bytes += written;
			size -= written;
		}

		फर्काऊ सत्य;
	}

};

static_assert(परिमाण(UasIdRecord) <= 256, "OpenDroneID UAS ID record must fit in /fs/mtd_id");

// The UAS ID storage is only supported on PX4 targets with NuttX and an MTD partition named "ID".
// On other platforms, the get() method will return StorageUnavailable and the checkOrStore() method will return Unsupported.
BidLockInfo BidStorageBackend::get()
{
#यदि defined(__PX4_NUTTX)
	अचर वर्ण *path = nullptr;

	यदि (px4_mtd_query("MTD_ID", nullptr, &path) != 0 || path == nullptr) {
		फर्काऊ {BidLockState::StorageUnavailable, {}};
	}

	पूर्णाङ्क fd = खोलो(path, O_RDONLY);

	यदि (fd < 0) {
		फर्काऊ {BidLockState::StorageUnavailable, {}};
	}

	UasIdRecord record{};

	यदि (पठ(fd, &record, परिमाण(record)) != परिमाण(record)) {
		बंद(fd);
		फर्काऊ {BidLockState::StorageUnavailable, {}};
	}

	बंद(fd);

	यदि (recordValid(record)) {
		BidLockInfo info{BidLockState::Locked, {}};
		info.id.ua_type = record.ua_type;
		स_नकल(info.id.uas_id, record.uas_id, UAS_ID_SIZE);
		फर्काऊ info;
	}

	यदि (recordBlank(record)) {
		फर्काऊ {BidLockState::Empty, {}};
	}

	फर्काऊ {BidLockState::InvalidRecord, {}};
#अन्यथा
	फर्काऊ {BidLockState::Unsupported, {}};
#endif
}

// Check if the given UAS ID matches the stored one, or store it if the storage is empty.
// The storage is write-once, so once a UAS ID is stored it cannot be changed (except by erasing the flash).
BidStoreResult BidStorageBackend::checkOrStore(uint8_t ua_type, अचर uint8_t uas_id[UAS_ID_SIZE])
{
	यदि (uasIdIsEmpty(uas_id)) {
		फर्काऊ BidStoreResult::InvalidBasicId;
	}

#यदि defined(__PX4_NUTTX)
	अचर वर्ण *path = nullptr;

	यदि (px4_mtd_query("MTD_ID", nullptr, &path) != 0 || path == nullptr) {
		फर्काऊ BidStoreResult::StorageUnavailable;
	}

	पूर्णाङ्क fd = खोलो(path, O_RDWR);

	यदि (fd < 0) {
		फर्काऊ BidStoreResult::StorageUnavailable;
	}

	UasIdRecord existing{};

	यदि (पठ(fd, &existing, परिमाण(existing)) != परिमाण(existing)) {
		बंद(fd);
		फर्काऊ BidStoreResult::StorageUnavailable;
	}

	यदि (recordValid(existing)) {
		अचर BidStoreResult result = recordMatches(existing, uas_id)
					      ? BidStoreResult::AlreadyLocked
					      : BidStoreResult::AlreadyLockedDifferent;
		बंद(fd);
		फर्काऊ result;
	}

	यदि (!recordBlank(existing)) {
		बंद(fd);
		फर्काऊ BidStoreResult::InvalidRecord;
	}

	UasIdRecord record{};
	record.magic = ODID_UASID_MAGIC;
	record.version = ODID_UASID_RECORD_VERSION;
	record.id_type = UAS_ID_TYPE_SERIAL_सङ्ख्या;
	record.ua_type = ua_type;
	स_नकल(record.uas_id, uas_id, UAS_ID_SIZE);
	record.crc = recordCrc(record);

	अचर uint32_t magic = record.magic;
	record.magic = 0xffffffff;

	अचर तार्किक written = lseek(fd, परिमाण(record.magic), शुरू_से) == static_cast<off_t>(परिमाण(record.magic))
			     && writeAll(fd, reinterpret_cast<अचर uint8_t *>(&record) + परिमाण(record.magic),
					 परिमाण(record) - परिमाण(record.magic))
			     && fsync(fd) == 0
			     && lseek(fd, 0, शुरू_से) == 0
			     && writeAll(fd, &magic, परिमाण(magic))
			     && fsync(fd) == 0;

	बंद(fd);

	फर्काऊ written ? BidStoreResult::Stored : BidStoreResult::WriteError;
#अन्यथा
	फर्काऊ BidStoreResult::Unsupported;
#endif
}

} // namespace

// Public interface that uses the backend implementation.
BidLockInfo BidStorage::get()
{
	BidStorageBackend storage;
	फर्काऊ storage.get();
}

BidStoreResult BidStorage::checkOrStore(uint8_t ua_type, अचर uint8_t uas_id[UAS_ID_SIZE])
{
	BidStorageBackend storage;
	फर्काऊ storage.checkOrStore(ua_type, uas_id);
}

//
BidStoreResult BidLock::resolve(uint8_t id_type, uint8_t ua_type, अचर uint8_t uas_id[UAS_ID_SIZE],
				UasId &locked_id)
{
	यदि (id_type != UAS_ID_TYPE_SERIAL_सङ्ख्या) {
		फर्काऊ BidStoreResult::InvalidBasicId;
	}

	यदि (!_loaded) {
		_info = BidStorage::get();
		_loaded = सत्य;
	}

	यदि (_info.locked()) {
		locked_id = _info.id;
		फर्काऊ BidStoreResult::AlreadyLocked;
	}

	यदि (_info.state != BidLockState::Empty) {
		विकल्प (_info.state) {
		पक्ष BidLockState::InvalidRecord:
			फर्काऊ BidStoreResult::InvalidRecord;

		पक्ष BidLockState::StorageUnavailable:
			फर्काऊ BidStoreResult::StorageUnavailable;

		पक्ष BidLockState::WriteError:
			फर्काऊ BidStoreResult::WriteError;

		पक्ष BidLockState::Unsupported:
			फर्काऊ BidStoreResult::Unsupported;

		पक्ष BidLockState::Empty:
		पक्ष BidLockState::Locked:
			रोक;
		}
	}

	अचर BidStoreResult result = BidStorage::checkOrStore(ua_type, uas_id);

	विकल्प (result) {
	पक्ष BidStoreResult::Stored:
		_info.state = BidLockState::Locked;
		_info.id.ua_type = ua_type;
		स_नकल(_info.id.uas_id, uas_id, UAS_ID_SIZE);
		locked_id = _info.id;
		फर्काऊ result;

	पक्ष BidStoreResult::AlreadyLocked:
	पक्ष BidStoreResult::AlreadyLockedDifferent: {
			_info = BidStorage::get();

			यदि (_info.locked()) {
				locked_id = _info.id;
				फर्काऊ BidStoreResult::AlreadyLocked;
			}

			फर्काऊ BidStoreResult::InvalidRecord;
		}

	पक्ष BidStoreResult::InvalidBasicId:
	पक्ष BidStoreResult::StorageUnavailable:
	पक्ष BidStoreResult::InvalidRecord:
	पक्ष BidStoreResult::WriteError:
	पक्ष BidStoreResult::Unsupported:
		फर्काऊ result;
	}

	फर्काऊ BidStoreResult::InvalidRecord;
}

} // namespace open_drone_id
