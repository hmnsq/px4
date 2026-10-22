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

#شامل "odid_bid_storage.hpp"

#شامل <fcntl.h>
#شامل <stddef.h>
#شامل <string.h>
#شامل <unistd.h>

#ifdef __PX4_NUTTX
#شامل <nuttx/crc32.h>
#شامل <px4_platform_common/px4_manifest.h>
#وگرنه
#شامل <crc32.h>
#endif

فضاینام open_drone_id
{
فضاینام
{

ایستا constexpr uint32_t ODID_UASID_MAGIC = 0x4449444f; // "ODID"
ایستا constexpr uint8_t ODID_UASID_RECORD_VERSION = 1;

ساختار __attribute__((packed)) UasIdRecord {
	uint32_t magic;
	uint8_t version;
	uint8_t id_type;
	uint8_t ua_type;
	uint8_t reserved;
	uint8_t uas_id[UAS_ID_SIZE];
	uint32_t crc;
};

رده BidStorageBackend
{
عمومی:
	BidLockInfo get();
	BidStoreResult checkOrStore(uint8_t ua_type, ثابت uint8_t uas_id[UAS_ID_SIZE]);

خصوصی:
	ایستا منطقی uasIdIsEmpty(ثابت uint8_t uas_id[UAS_ID_SIZE])
	{
		برای (unsigned i = 0; i < UAS_ID_SIZE; ++i) {
			اگر (uas_id[i] != 0) {
				بازگشت نادرست;
			}
		}

		بازگشت درست;
	}

	ایستا uint32_t recordCrc(ثابت UasIdRecord &record)
	{
		بازگشت crc32(reinterpret_cast<ثابت uint8_t *>(&record.version),
			     offsetof(UasIdRecord, crc) - offsetof(UasIdRecord, version));
	}

	ایستا منطقی recordValid(ثابت UasIdRecord &record)
	{
		بازگشت record.magic == ODID_UASID_MAGIC
		       && record.version == ODID_UASID_RECORD_VERSION
		       && record.id_type == UAS_ID_TYPE_SERIAL_عدد
		       && !uasIdIsEmpty(record.uas_id)
		       && record.crc == recordCrc(record);
	}

	// Check if the record is all 0xff, which is the erased state of flash.
	// This is needed to distinguish between an unwritten record and a record that was written with all bytes set to 0 (which would be invalid but not unwritten).
	ایستا منطقی recordBlank(ثابت UasIdRecord &record)
	{
		ثابت uint8_t *bytes = reinterpret_cast<ثابت uint8_t *>(&record);

		برای (unsigned i = 0; i < اندازه(record); ++i) {
			اگر (bytes[i] != 0xff) {
				بازگشت نادرست;
			}
		}

		بازگشت درست;
	}

	ایستا منطقی recordMatches(ثابت UasIdRecord &record, ثابت uint8_t uas_id[UAS_ID_SIZE])
	{
		بازگشت memcmp(record.uas_id, uas_id, UAS_ID_SIZE) == 0;
	}

	// Write the entire buffer to the file descriptor, handling short writes.
	ایستا منطقی writeAll(صحیح fd, ثابت تهی *buffer, size_t size)
	{
		ثابت uint8_t *bytes = static_cast<ثابت uint8_t *>(buffer);

		وقتی (size > 0) {
			ثابت ssize_t written = اكتب(fd, bytes, size);

			اگر (written <= 0) {
				بازگشت نادرست;
			}

			bytes += written;
			size -= written;
		}

		بازگشت درست;
	}

};

static_assert(اندازه(UasIdRecord) <= 256, "OpenDroneID UAS ID record must fit in /fs/mtd_id");

// The UAS ID storage is only supported on PX4 targets with NuttX and an MTD partition named "ID".
// On other platforms, the get() method will return StorageUnavailable and the checkOrStore() method will return Unsupported.
BidLockInfo BidStorageBackend::get()
{
#اگر defined(__PX4_NUTTX)
	ثابت نویسه *path = nullptr;

	اگر (px4_mtd_query("MTD_ID", nullptr, &path) != 0 || path == nullptr) {
		بازگشت {BidLockState::StorageUnavailable, {}};
	}

	صحیح fd = open(path, O_RDONLY);

	اگر (fd < 0) {
		بازگشت {BidLockState::StorageUnavailable, {}};
	}

	UasIdRecord record{};

	اگر (اقرأ(fd, &record, اندازه(record)) != اندازه(record)) {
		close(fd);
		بازگشت {BidLockState::StorageUnavailable, {}};
	}

	close(fd);

	اگر (recordValid(record)) {
		BidLockInfo info{BidLockState::Locked, {}};
		info.id.ua_type = record.ua_type;
		memcpy(info.id.uas_id, record.uas_id, UAS_ID_SIZE);
		بازگشت info;
	}

	اگر (recordBlank(record)) {
		بازگشت {BidLockState::Empty, {}};
	}

	بازگشت {BidLockState::InvalidRecord, {}};
#وگرنه
	بازگشت {BidLockState::Unsupported, {}};
#endif
}

// Check if the given UAS ID matches the stored one, or store it if the storage is empty.
// The storage is write-once, so once a UAS ID is stored it cannot be changed (except by erasing the flash).
BidStoreResult BidStorageBackend::checkOrStore(uint8_t ua_type, ثابت uint8_t uas_id[UAS_ID_SIZE])
{
	اگر (uasIdIsEmpty(uas_id)) {
		بازگشت BidStoreResult::InvalidBasicId;
	}

#اگر defined(__PX4_NUTTX)
	ثابت نویسه *path = nullptr;

	اگر (px4_mtd_query("MTD_ID", nullptr, &path) != 0 || path == nullptr) {
		بازگشت BidStoreResult::StorageUnavailable;
	}

	صحیح fd = open(path, O_RDWR);

	اگر (fd < 0) {
		بازگشت BidStoreResult::StorageUnavailable;
	}

	UasIdRecord existing{};

	اگر (اقرأ(fd, &existing, اندازه(existing)) != اندازه(existing)) {
		close(fd);
		بازگشت BidStoreResult::StorageUnavailable;
	}

	اگر (recordValid(existing)) {
		ثابت BidStoreResult result = recordMatches(existing, uas_id)
					      ? BidStoreResult::AlreadyLocked
					      : BidStoreResult::AlreadyLockedDifferent;
		close(fd);
		بازگشت result;
	}

	اگر (!recordBlank(existing)) {
		close(fd);
		بازگشت BidStoreResult::InvalidRecord;
	}

	UasIdRecord record{};
	record.magic = ODID_UASID_MAGIC;
	record.version = ODID_UASID_RECORD_VERSION;
	record.id_type = UAS_ID_TYPE_SERIAL_عدد;
	record.ua_type = ua_type;
	memcpy(record.uas_id, uas_id, UAS_ID_SIZE);
	record.crc = recordCrc(record);

	ثابت uint32_t magic = record.magic;
	record.magic = 0xffffffff;

	ثابت منطقی written = lseek(fd, اندازه(record.magic), SEEK_SET) == static_cast<off_t>(اندازه(record.magic))
			     && writeAll(fd, reinterpret_cast<ثابت uint8_t *>(&record) + اندازه(record.magic),
					 اندازه(record) - اندازه(record.magic))
			     && fsync(fd) == 0
			     && lseek(fd, 0, SEEK_SET) == 0
			     && writeAll(fd, &magic, اندازه(magic))
			     && fsync(fd) == 0;

	close(fd);

	بازگشت written ? BidStoreResult::Stored : BidStoreResult::WriteError;
#وگرنه
	بازگشت BidStoreResult::Unsupported;
#endif
}

} // namespace

// Public interface that uses the backend implementation.
BidLockInfo BidStorage::get()
{
	BidStorageBackend storage;
	بازگشت storage.get();
}

BidStoreResult BidStorage::checkOrStore(uint8_t ua_type, ثابت uint8_t uas_id[UAS_ID_SIZE])
{
	BidStorageBackend storage;
	بازگشت storage.checkOrStore(ua_type, uas_id);
}

//
BidStoreResult BidLock::resolve(uint8_t id_type, uint8_t ua_type, ثابت uint8_t uas_id[UAS_ID_SIZE],
				UasId &locked_id)
{
	اگر (id_type != UAS_ID_TYPE_SERIAL_عدد) {
		بازگشت BidStoreResult::InvalidBasicId;
	}

	اگر (!_loaded) {
		_info = BidStorage::get();
		_loaded = درست;
	}

	اگر (_info.locked()) {
		locked_id = _info.id;
		بازگشت BidStoreResult::AlreadyLocked;
	}

	اگر (_info.state != BidLockState::Empty) {
		انتخاب (_info.state) {
		حالت BidLockState::InvalidRecord:
			بازگشت BidStoreResult::InvalidRecord;

		حالت BidLockState::StorageUnavailable:
			بازگشت BidStoreResult::StorageUnavailable;

		حالت BidLockState::WriteError:
			بازگشت BidStoreResult::WriteError;

		حالت BidLockState::Unsupported:
			بازگشت BidStoreResult::Unsupported;

		حالت BidLockState::Empty:
		حالت BidLockState::Locked:
			بشکن;
		}
	}

	ثابت BidStoreResult result = BidStorage::checkOrStore(ua_type, uas_id);

	انتخاب (result) {
	حالت BidStoreResult::Stored:
		_info.state = BidLockState::Locked;
		_info.id.ua_type = ua_type;
		memcpy(_info.id.uas_id, uas_id, UAS_ID_SIZE);
		locked_id = _info.id;
		بازگشت result;

	حالت BidStoreResult::AlreadyLocked:
	حالت BidStoreResult::AlreadyLockedDifferent: {
			_info = BidStorage::get();

			اگر (_info.locked()) {
				locked_id = _info.id;
				بازگشت BidStoreResult::AlreadyLocked;
			}

			بازگشت BidStoreResult::InvalidRecord;
		}

	حالت BidStoreResult::InvalidBasicId:
	حالت BidStoreResult::StorageUnavailable:
	حالت BidStoreResult::InvalidRecord:
	حالت BidStoreResult::WriteError:
	حالت BidStoreResult::Unsupported:
		بازگشت result;
	}

	بازگشت BidStoreResult::InvalidRecord;
}

} // namespace open_drone_id
