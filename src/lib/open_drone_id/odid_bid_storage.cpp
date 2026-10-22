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
	BidStoreResult checkOrStore(uint8_t ua_type, स्थिरांक uint8_t uas_id[UAS_ID_SIZE]);

खाजगी:
	स्थिर तार्किक uasIdIsEmpty(स्थिरांक uint8_t uas_id[UAS_ID_SIZE])
	{
		साठी (अचिन्हित i = 0; i < UAS_ID_SIZE; ++i) {
			जर (uas_id[i] != 0) {
				परत असत्य;
			}
		}

		परत सत्य;
	}

	स्थिर uint32_t recordCrc(स्थिरांक UasIdRecord &record)
	{
		परत crc32(reinterpret_cast<स्थिरांक uint8_t *>(&record.version),
			     दुरत्व(UasIdRecord, crc) - दुरत्व(UasIdRecord, version));
	}

	स्थिर तार्किक recordValid(स्थिरांक UasIdRecord &record)
	{
		परत record.magic == ODID_UASID_MAGIC
		       && record.version == ODID_UASID_RECORD_VERSION
		       && record.id_type == UAS_ID_TYPE_SERIAL_संख्या
		       && !uasIdIsEmpty(record.uas_id)
		       && record.crc == recordCrc(record);
	}

	// Check if the record is all 0xff, which is the erased state of flash.
	// This is needed to distinguish between an unwritten record and a record that was written with all bytes set to 0 (which would be invalid but not unwritten).
	स्थिर तार्किक recordBlank(स्थिरांक UasIdRecord &record)
	{
		स्थिरांक uint8_t *bytes = reinterpret_cast<स्थिरांक uint8_t *>(&record);

		साठी (अचिन्हित i = 0; i < आकार(record); ++i) {
			जर (bytes[i] != 0xff) {
				परत असत्य;
			}
		}

		परत सत्य;
	}

	स्थिर तार्किक recordMatches(स्थिरांक UasIdRecord &record, स्थिरांक uint8_t uas_id[UAS_ID_SIZE])
	{
		परत स_भेद(record.uas_id, uas_id, UAS_ID_SIZE) == 0;
	}

	// Write the entire buffer to the file descriptor, handling short writes.
	स्थिर तार्किक writeAll(पूर्णांक fd, स्थिरांक शून्य *buffer, माप_प्रकार size)
	{
		स्थिरांक uint8_t *bytes = static_cast<स्थिरांक uint8_t *>(buffer);

		पर्यंत (size > 0) {
			स्थिरांक ssize_t written = लिख(fd, bytes, size);

			जर (written <= 0) {
				परत असत्य;
			}

			bytes += written;
			size -= written;
		}

		परत सत्य;
	}

};

static_assert(आकार(UasIdRecord) <= 256, "OpenDroneID UAS ID record must fit in /fs/mtd_id");

// The UAS ID storage is only supported on PX4 targets with NuttX and an MTD partition named "ID".
// On other platforms, the get() method will return StorageUnavailable and the checkOrStore() method will return Unsupported.
BidLockInfo BidStorageBackend::get()
{
#जर defined(__PX4_NUTTX)
	स्थिरांक अक्षर *path = nullptr;

	जर (px4_mtd_query("MTD_ID", nullptr, &path) != 0 || path == nullptr) {
		परत {BidLockState::StorageUnavailable, {}};
	}

	पूर्णांक fd = खोलो(path, O_RDONLY);

	जर (fd < 0) {
		परत {BidLockState::StorageUnavailable, {}};
	}

	UasIdRecord record{};

	जर (पठ(fd, &record, आकार(record)) != आकार(record)) {
		बंद(fd);
		परत {BidLockState::StorageUnavailable, {}};
	}

	बंद(fd);

	जर (recordValid(record)) {
		BidLockInfo info{BidLockState::Locked, {}};
		info.id.ua_type = record.ua_type;
		स_नकल(info.id.uas_id, record.uas_id, UAS_ID_SIZE);
		परत info;
	}

	जर (recordBlank(record)) {
		परत {BidLockState::Empty, {}};
	}

	परत {BidLockState::InvalidRecord, {}};
#अन्यथा
	परत {BidLockState::Unsupported, {}};
#endif
}

// Check if the given UAS ID matches the stored one, or store it if the storage is empty.
// The storage is write-once, so once a UAS ID is stored it cannot be changed (except by erasing the flash).
BidStoreResult BidStorageBackend::checkOrStore(uint8_t ua_type, स्थिरांक uint8_t uas_id[UAS_ID_SIZE])
{
	जर (uasIdIsEmpty(uas_id)) {
		परत BidStoreResult::InvalidBasicId;
	}

#जर defined(__PX4_NUTTX)
	स्थिरांक अक्षर *path = nullptr;

	जर (px4_mtd_query("MTD_ID", nullptr, &path) != 0 || path == nullptr) {
		परत BidStoreResult::StorageUnavailable;
	}

	पूर्णांक fd = खोलो(path, O_RDWR);

	जर (fd < 0) {
		परत BidStoreResult::StorageUnavailable;
	}

	UasIdRecord existing{};

	जर (पठ(fd, &existing, आकार(existing)) != आकार(existing)) {
		बंद(fd);
		परत BidStoreResult::StorageUnavailable;
	}

	जर (recordValid(existing)) {
		स्थिरांक BidStoreResult result = recordMatches(existing, uas_id)
					      ? BidStoreResult::AlreadyLocked
					      : BidStoreResult::AlreadyLockedDifferent;
		बंद(fd);
		परत result;
	}

	जर (!recordBlank(existing)) {
		बंद(fd);
		परत BidStoreResult::InvalidRecord;
	}

	UasIdRecord record{};
	record.magic = ODID_UASID_MAGIC;
	record.version = ODID_UASID_RECORD_VERSION;
	record.id_type = UAS_ID_TYPE_SERIAL_संख्या;
	record.ua_type = ua_type;
	स_नकल(record.uas_id, uas_id, UAS_ID_SIZE);
	record.crc = recordCrc(record);

	स्थिरांक uint32_t magic = record.magic;
	record.magic = 0xffffffff;

	स्थिरांक तार्किक written = lseek(fd, आकार(record.magic), शुरू_से) == static_cast<off_t>(आकार(record.magic))
			     && writeAll(fd, reinterpret_cast<स्थिरांक uint8_t *>(&record) + आकार(record.magic),
					 आकार(record) - आकार(record.magic))
			     && fsync(fd) == 0
			     && lseek(fd, 0, शुरू_से) == 0
			     && writeAll(fd, &magic, आकार(magic))
			     && fsync(fd) == 0;

	बंद(fd);

	परत written ? BidStoreResult::Stored : BidStoreResult::WriteError;
#अन्यथा
	परत BidStoreResult::Unsupported;
#endif
}

} // namespace

// Public interface that uses the backend implementation.
BidLockInfo BidStorage::get()
{
	BidStorageBackend storage;
	परत storage.get();
}

BidStoreResult BidStorage::checkOrStore(uint8_t ua_type, स्थिरांक uint8_t uas_id[UAS_ID_SIZE])
{
	BidStorageBackend storage;
	परत storage.checkOrStore(ua_type, uas_id);
}

//
BidStoreResult BidLock::resolve(uint8_t id_type, uint8_t ua_type, स्थिरांक uint8_t uas_id[UAS_ID_SIZE],
				UasId &locked_id)
{
	जर (id_type != UAS_ID_TYPE_SERIAL_संख्या) {
		परत BidStoreResult::InvalidBasicId;
	}

	जर (!_loaded) {
		_info = BidStorage::get();
		_loaded = सत्य;
	}

	जर (_info.locked()) {
		locked_id = _info.id;
		परत BidStoreResult::AlreadyLocked;
	}

	जर (_info.state != BidLockState::Empty) {
		निवड (_info.state) {
		स्थिती BidLockState::InvalidRecord:
			परत BidStoreResult::InvalidRecord;

		स्थिती BidLockState::StorageUnavailable:
			परत BidStoreResult::StorageUnavailable;

		स्थिती BidLockState::WriteError:
			परत BidStoreResult::WriteError;

		स्थिती BidLockState::Unsupported:
			परत BidStoreResult::Unsupported;

		स्थिती BidLockState::Empty:
		स्थिती BidLockState::Locked:
			थांबा;
		}
	}

	स्थिरांक BidStoreResult result = BidStorage::checkOrStore(ua_type, uas_id);

	निवड (result) {
	स्थिती BidStoreResult::Stored:
		_info.state = BidLockState::Locked;
		_info.id.ua_type = ua_type;
		स_नकल(_info.id.uas_id, uas_id, UAS_ID_SIZE);
		locked_id = _info.id;
		परत result;

	स्थिती BidStoreResult::AlreadyLocked:
	स्थिती BidStoreResult::AlreadyLockedDifferent: {
			_info = BidStorage::get();

			जर (_info.locked()) {
				locked_id = _info.id;
				परत BidStoreResult::AlreadyLocked;
			}

			परत BidStoreResult::InvalidRecord;
		}

	स्थिती BidStoreResult::InvalidBasicId:
	स्थिती BidStoreResult::StorageUnavailable:
	स्थिती BidStoreResult::InvalidRecord:
	स्थिती BidStoreResult::WriteError:
	स्थिती BidStoreResult::Unsupported:
		परत result;
	}

	परत BidStoreResult::InvalidRecord;
}

} // namespace open_drone_id
