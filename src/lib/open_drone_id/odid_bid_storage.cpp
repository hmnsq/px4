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

#ضمن "odid_bid_storage.hpp"

#ضمن <fcntl.h>
#ضمن <stddef.h>
#ضمن <string.h>
#ضمن <unistd.h>

#ifdef __PX4_NUTTX
#ضمن <nuttx/crc32.h>
#ضمن <px4_platform_common/px4_manifest.h>
#وإلا
#ضمن <crc32.h>
#endif

نطاق open_drone_id
{
نطاق
{

ساكن constexpr uint32_t ODID_UASID_MAGIC = 0x4449444f; // "ODID"
ساكن constexpr uint8_t ODID_UASID_RECORD_VERSION = 1;

بنية __attribute__((packed)) UasIdRecord {
	uint32_t magic;
	uint8_t version;
	uint8_t id_type;
	uint8_t ua_type;
	uint8_t reserved;
	uint8_t uas_id[UAS_ID_SIZE];
	uint32_t crc;
};

صنف BidStorageBackend
{
عام:
	BidLockInfo get();
	BidStoreResult checkOrStore(uint8_t ua_type, ثابت uint8_t uas_id[UAS_ID_SIZE]);

خاص:
	ساكن منطقي uasIdIsEmpty(ثابت uint8_t uas_id[UAS_ID_SIZE])
	{
		لكل (unsigned i = 0; i < UAS_ID_SIZE; ++i) {
			𐤀𐤌 (uas_id[i] != 0) {
				أرجع خطأ;
			}
		}

		أرجع true;
	}

	ساكن uint32_t recordCrc(ثابت UasIdRecord &record)
	{
		أرجع crc32(reinterpret_cast<ثابت uint8_t *>(&record.version),
			     offsetof(UasIdRecord, crc) - offsetof(UasIdRecord, version));
	}

	ساكن منطقي recordValid(ثابت UasIdRecord &record)
	{
		أرجع record.magic == ODID_UASID_MAGIC
		       && record.version == ODID_UASID_RECORD_VERSION
		       && record.id_type == UAS_ID_TYPE_SERIAL_NUMBER
		       && !uasIdIsEmpty(record.uas_id)
		       && record.crc == recordCrc(record);
	}

	// Check if the record is all 0xff, which is the erased state of flash.
	// This is needed to distinguish between an unwritten record and a record that was written with all bytes set to 0 (which would be invalid but not unwritten).
	ساكن منطقي recordBlank(ثابت UasIdRecord &record)
	{
		ثابت uint8_t *bytes = reinterpret_cast<ثابت uint8_t *>(&record);

		لكل (unsigned i = 0; i < حجم(record); ++i) {
			𐤀𐤌 (bytes[i] != 0xff) {
				أرجع خطأ;
			}
		}

		أرجع true;
	}

	ساكن منطقي recordMatches(ثابت UasIdRecord &record, ثابت uint8_t uas_id[UAS_ID_SIZE])
	{
		أرجع memcmp(record.uas_id, uas_id, UAS_ID_SIZE) == 0;
	}

	// Write the entire buffer to the file descriptor, handling short writes.
	ساكن منطقي writeAll(صحيح fd, ثابت فراغ *buffer, size_t size)
	{
		ثابت uint8_t *bytes = static_cast<ثابت uint8_t *>(buffer);

		طالما (size > 0) {
			ثابت ssize_t written = اكتب(fd, bytes, size);

			𐤀𐤌 (written <= 0) {
				أرجع خطأ;
			}

			bytes += written;
			size -= written;
		}

		أرجع true;
	}

};

static_assert(حجم(UasIdRecord) <= 256, "OpenDroneID UAS ID record must fit in /fs/mtd_id");

// The UAS ID storage is only supported on PX4 targets with NuttX and an MTD partition named "ID".
// On other platforms, the get() method will return StorageUnavailable and the checkOrStore() method will return Unsupported.
BidLockInfo BidStorageBackend::get()
{
#𐤀𐤌 defined(__PX4_NUTTX)
	ثابت حرف *path = nullptr;

	𐤀𐤌 (px4_mtd_query("MTD_ID", nullptr, &path) != 0 || path == nullptr) {
		أرجع {BidLockState::StorageUnavailable, {}};
	}

	صحيح fd = open(path, O_RDONLY);

	𐤀𐤌 (fd < 0) {
		أرجع {BidLockState::StorageUnavailable, {}};
	}

	UasIdRecord record{};

	𐤀𐤌 (اقرأ(fd, &record, حجم(record)) != حجم(record)) {
		close(fd);
		أرجع {BidLockState::StorageUnavailable, {}};
	}

	close(fd);

	𐤀𐤌 (recordValid(record)) {
		BidLockInfo info{BidLockState::Locked, {}};
		info.id.ua_type = record.ua_type;
		memcpy(info.id.uas_id, record.uas_id, UAS_ID_SIZE);
		أرجع info;
	}

	𐤀𐤌 (recordBlank(record)) {
		أرجع {BidLockState::Empty, {}};
	}

	أرجع {BidLockState::InvalidRecord, {}};
#وإلا
	أرجع {BidLockState::Unsupported, {}};
#endif
}

// Check if the given UAS ID matches the stored one, or store it if the storage is empty.
// The storage is write-once, so once a UAS ID is stored it cannot be changed (except by erasing the flash).
BidStoreResult BidStorageBackend::checkOrStore(uint8_t ua_type, ثابت uint8_t uas_id[UAS_ID_SIZE])
{
	𐤀𐤌 (uasIdIsEmpty(uas_id)) {
		أرجع BidStoreResult::InvalidBasicId;
	}

#𐤀𐤌 defined(__PX4_NUTTX)
	ثابت حرف *path = nullptr;

	𐤀𐤌 (px4_mtd_query("MTD_ID", nullptr, &path) != 0 || path == nullptr) {
		أرجع BidStoreResult::StorageUnavailable;
	}

	صحيح fd = open(path, O_RDWR);

	𐤀𐤌 (fd < 0) {
		أرجع BidStoreResult::StorageUnavailable;
	}

	UasIdRecord existing{};

	𐤀𐤌 (اقرأ(fd, &existing, حجم(existing)) != حجم(existing)) {
		close(fd);
		أرجع BidStoreResult::StorageUnavailable;
	}

	𐤀𐤌 (recordValid(existing)) {
		ثابت BidStoreResult result = recordMatches(existing, uas_id)
					      ? BidStoreResult::AlreadyLocked
					      : BidStoreResult::AlreadyLockedDifferent;
		close(fd);
		أرجع result;
	}

	𐤀𐤌 (!recordBlank(existing)) {
		close(fd);
		أرجع BidStoreResult::InvalidRecord;
	}

	UasIdRecord record{};
	record.magic = ODID_UASID_MAGIC;
	record.version = ODID_UASID_RECORD_VERSION;
	record.id_type = UAS_ID_TYPE_SERIAL_NUMBER;
	record.ua_type = ua_type;
	memcpy(record.uas_id, uas_id, UAS_ID_SIZE);
	record.crc = recordCrc(record);

	ثابت uint32_t magic = record.magic;
	record.magic = 0xffffffff;

	ثابت منطقي written = lseek(fd, حجم(record.magic), SEEK_SET) == static_cast<off_t>(حجم(record.magic))
			     && writeAll(fd, reinterpret_cast<ثابت uint8_t *>(&record) + حجم(record.magic),
					 حجم(record) - حجم(record.magic))
			     && fsync(fd) == 0
			     && lseek(fd, 0, SEEK_SET) == 0
			     && writeAll(fd, &magic, حجم(magic))
			     && fsync(fd) == 0;

	close(fd);

	أرجع written ? BidStoreResult::Stored : BidStoreResult::WriteError;
#وإلا
	أرجع BidStoreResult::Unsupported;
#endif
}

} // namespace

// Public interface that uses the backend implementation.
BidLockInfo BidStorage::get()
{
	BidStorageBackend storage;
	أرجع storage.get();
}

BidStoreResult BidStorage::checkOrStore(uint8_t ua_type, ثابت uint8_t uas_id[UAS_ID_SIZE])
{
	BidStorageBackend storage;
	أرجع storage.checkOrStore(ua_type, uas_id);
}

//
BidStoreResult BidLock::resolve(uint8_t id_type, uint8_t ua_type, ثابت uint8_t uas_id[UAS_ID_SIZE],
				UasId &locked_id)
{
	𐤀𐤌 (id_type != UAS_ID_TYPE_SERIAL_NUMBER) {
		أرجع BidStoreResult::InvalidBasicId;
	}

	𐤀𐤌 (!_loaded) {
		_info = BidStorage::get();
		_loaded = true;
	}

	𐤀𐤌 (_info.locked()) {
		locked_id = _info.id;
		أرجع BidStoreResult::AlreadyLocked;
	}

	𐤀𐤌 (_info.state != BidLockState::Empty) {
		بدل (_info.state) {
		حالة BidLockState::InvalidRecord:
			أرجع BidStoreResult::InvalidRecord;

		حالة BidLockState::StorageUnavailable:
			أرجع BidStoreResult::StorageUnavailable;

		حالة BidLockState::WriteError:
			أرجع BidStoreResult::WriteError;

		حالة BidLockState::Unsupported:
			أرجع BidStoreResult::Unsupported;

		حالة BidLockState::Empty:
		حالة BidLockState::Locked:
			اكسر;
		}
	}

	ثابت BidStoreResult result = BidStorage::checkOrStore(ua_type, uas_id);

	بدل (result) {
	حالة BidStoreResult::Stored:
		_info.state = BidLockState::Locked;
		_info.id.ua_type = ua_type;
		memcpy(_info.id.uas_id, uas_id, UAS_ID_SIZE);
		locked_id = _info.id;
		أرجع result;

	حالة BidStoreResult::AlreadyLocked:
	حالة BidStoreResult::AlreadyLockedDifferent: {
			_info = BidStorage::get();

			𐤀𐤌 (_info.locked()) {
				locked_id = _info.id;
				أرجع BidStoreResult::AlreadyLocked;
			}

			أرجع BidStoreResult::InvalidRecord;
		}

	حالة BidStoreResult::InvalidBasicId:
	حالة BidStoreResult::StorageUnavailable:
	حالة BidStoreResult::InvalidRecord:
	حالة BidStoreResult::WriteError:
	حالة BidStoreResult::Unsupported:
		أرجع result;
	}

	أرجع BidStoreResult::InvalidRecord;
}

} // namespace open_drone_id
