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

#অন্তর্ভাব "odid_bid_storage.hpp"

#অন্তর্ভাব <fcntl.h>
#অন্তর্ভাব <stddef.h>
#অন্তর্ভাব <string.h>
#অন্তর্ভাব <unistd.h>

#ifdef __PX4_NUTTX
#অন্তর্ভাব <nuttx/crc32.h>
#অন্তর্ভাব <px4_platform_common/px4_manifest.h>
#অন্যথা
#অন্তর্ভাব <crc32.h>
#endif

নামাকাশ open_drone_id
{
নামাকাশ
{

স্থির constexpr uint32_t ODID_UASID_MAGIC = 0x4449444f; // "ODID"
স্থির constexpr uint8_t ODID_UASID_RECORD_VERSION = 1;

কাঠামো __attribute__((packed)) UasIdRecord {
	uint32_t magic;
	uint8_t version;
	uint8_t id_type;
	uint8_t ua_type;
	uint8_t reserved;
	uint8_t uas_id[UAS_ID_আকার];
	uint32_t crc;
};

শ্রেণী BidStorageBackend
{
সর্বজনীন:
	BidLockInfo get();
	Bid_Store_ফলাফল checkOrStore(uint8_t ua_type, ধ্রুবক uint8_t uas_id[UAS_ID_আকার]);

ব্যক্তিগত:
	স্থির যৌক্তিক uasIdIsEmpty(ধ্রুবক uint8_t uas_id[UAS_ID_আকার])
	{
		জন্য (unsigned i = 0; i < UAS_ID_আকার; ++i) {
			যদি (uas_id[i] != 0) {
				ফেরত মিথ্যা;
			}
		}

		ফেরত সত্য;
	}

	স্থির uint32_t recordCrc(ধ্রুবক UasIdRecord &record)
	{
		ফেরত crc32(reinterpret_cast<ধ্রুবক uint8_t *>(&record.version),
			     offsetof(UasIdRecord, crc) - offsetof(UasIdRecord, version));
	}

	স্থির যৌক্তিক recordValid(ধ্রুবক UasIdRecord &record)
	{
		ফেরত record.magic == ODID_UASID_MAGIC
		       && record.version == ODID_UASID_RECORD_VERSION
		       && record.id_type == UAS_ID_TYPE_SERIAL_সংখ্যা
		       && !uasIdIsEmpty(record.uas_id)
		       && record.crc == recordCrc(record);
	}

	// Check if the record is all 0xff, which is the erased state of flash.
	// This is needed to distinguish between an unwritten record and a record that was written with all bytes set to 0 (which would be invalid but not unwritten).
	স্থির যৌক্তিক recordBlank(ধ্রুবক UasIdRecord &record)
	{
		ধ্রুবক uint8_t *bytes = reinterpret_cast<ধ্রুবক uint8_t *>(&record);

		জন্য (unsigned i = 0; i < আকার(record); ++i) {
			যদি (bytes[i] != 0xff) {
				ফেরত মিথ্যা;
			}
		}

		ফেরত সত্য;
	}

	স্থির যৌক্তিক recordMatches(ধ্রুবক UasIdRecord &record, ধ্রুবক uint8_t uas_id[UAS_ID_আকার])
	{
		ফেরত memcmp(record.uas_id, uas_id, UAS_ID_আকার) == 0;
	}

	// Write the entire buffer to the file descriptor, handling short writes.
	স্থির যৌক্তিক writeAll(পূর্ণসংখ্যা fd, ধ্রুবক শূন্য *বাফার_2, size_t আকার_2)
	{
		ধ্রুবক uint8_t *bytes = static_cast<ধ্রুবক uint8_t *>(বাফার_2);

		যখন (আকার_2 > 0) {
			ধ্রুবক ssize_t written = লিখ(fd, bytes, আকার_2);

			যদি (written <= 0) {
				ফেরত মিথ্যা;
			}

			bytes += written;
			আকার_2 -= written;
		}

		ফেরত সত্য;
	}

};

static_assert(আকার(UasIdRecord) <= 256, "OpenDroneID UAS ID record must fit in /fs/mtd_id");

// The UAS ID storage is only supported on PX4 targets with NuttX and an MTD partition named "ID".
// On other platforms, the get() method will return StorageUnavailable and the checkOrStore() method will return Unsupported.
BidLockInfo BidStorageBackend::get()
{
#যদি defined(__PX4_NUTTX)
	ধ্রুবক অক্ষর *path = nullptr;

	যদি (px4_mtd_query("MTD_ID", nullptr, &path) != 0 || path == nullptr) {
		ফেরত {BidLockState::StorageUnavailable, {}};
	}

	পূর্ণসংখ্যা fd = open(path, O_RDONLY);

	যদি (fd < 0) {
		ফেরত {BidLockState::StorageUnavailable, {}};
	}

	UasIdRecord record{};

	যদি (পঠ(fd, &record, আকার(record)) != আকার(record)) {
		close(fd);
		ফেরত {BidLockState::StorageUnavailable, {}};
	}

	close(fd);

	যদি (recordValid(record)) {
		BidLockInfo info{BidLockState::Locked, {}};
		info.id.ua_type = record.ua_type;
		memcpy(info.id.uas_id, record.uas_id, UAS_ID_আকার);
		ফেরত info;
	}

	যদি (recordBlank(record)) {
		ফেরত {BidLockState::Empty, {}};
	}

	ফেরত {BidLockState::InvalidRecord, {}};
#অন্যথা
	ফেরত {BidLockState::Unsupported, {}};
#endif
}

// Check if the given UAS ID matches the stored one, or store it if the storage is empty.
// The storage is write-once, so once a UAS ID is stored it cannot be changed (except by erasing the flash).
Bid_Store_ফলাফল BidStorageBackend::checkOrStore(uint8_t ua_type, ধ্রুবক uint8_t uas_id[UAS_ID_আকার])
{
	যদি (uasIdIsEmpty(uas_id)) {
		ফেরত Bid_Store_ফলাফল::InvalidBasicId;
	}

#যদি defined(__PX4_NUTTX)
	ধ্রুবক অক্ষর *path = nullptr;

	যদি (px4_mtd_query("MTD_ID", nullptr, &path) != 0 || path == nullptr) {
		ফেরত Bid_Store_ফলাফল::StorageUnavailable;
	}

	পূর্ণসংখ্যা fd = open(path, O_RDWR);

	যদি (fd < 0) {
		ফেরত Bid_Store_ফলাফল::StorageUnavailable;
	}

	UasIdRecord existing{};

	যদি (পঠ(fd, &existing, আকার(existing)) != আকার(existing)) {
		close(fd);
		ফেরত Bid_Store_ফলাফল::StorageUnavailable;
	}

	যদি (recordValid(existing)) {
		ধ্রুবক Bid_Store_ফলাফল ফলাফল = recordMatches(existing, uas_id)
					      ? Bid_Store_ফলাফল::AlreadyLocked
					      : Bid_Store_ফলাফল::AlreadyLockedDifferent;
		close(fd);
		ফেরত ফলাফল;
	}

	যদি (!recordBlank(existing)) {
		close(fd);
		ফেরত Bid_Store_ফলাফল::InvalidRecord;
	}

	UasIdRecord record{};
	record.magic = ODID_UASID_MAGIC;
	record.version = ODID_UASID_RECORD_VERSION;
	record.id_type = UAS_ID_TYPE_SERIAL_সংখ্যা;
	record.ua_type = ua_type;
	memcpy(record.uas_id, uas_id, UAS_ID_আকার);
	record.crc = recordCrc(record);

	ধ্রুবক uint32_t magic = record.magic;
	record.magic = 0xffffffff;

	ধ্রুবক যৌক্তিক written = lseek(fd, আকার(record.magic), SEEK_SET) == static_cast<off_t>(আকার(record.magic))
			     && writeAll(fd, reinterpret_cast<ধ্রুবক uint8_t *>(&record) + আকার(record.magic),
					 আকার(record) - আকার(record.magic))
			     && fsync(fd) == 0
			     && lseek(fd, 0, SEEK_SET) == 0
			     && writeAll(fd, &magic, আকার(magic))
			     && fsync(fd) == 0;

	close(fd);

	ফেরত written ? Bid_Store_ফলাফল::Stored : Bid_Store_ফলাফল::WriteError;
#অন্যথা
	ফেরত Bid_Store_ফলাফল::Unsupported;
#endif
}

} // namespace

// Public interface that uses the backend implementation.
BidLockInfo BidStorage::get()
{
	BidStorageBackend storage;
	ফেরত storage.get();
}

Bid_Store_ফলাফল BidStorage::checkOrStore(uint8_t ua_type, ধ্রুবক uint8_t uas_id[UAS_ID_আকার])
{
	BidStorageBackend storage;
	ফেরত storage.checkOrStore(ua_type, uas_id);
}

//
Bid_Store_ফলাফল BidLock::resolve(uint8_t id_type, uint8_t ua_type, ধ্রুবক uint8_t uas_id[UAS_ID_আকার],
				UasId &locked_id)
{
	যদি (id_type != UAS_ID_TYPE_SERIAL_সংখ্যা) {
		ফেরত Bid_Store_ফলাফল::InvalidBasicId;
	}

	যদি (!_loaded) {
		_info = BidStorage::get();
		_loaded = সত্য;
	}

	যদি (_info.locked()) {
		locked_id = _info.id;
		ফেরত Bid_Store_ফলাফল::AlreadyLocked;
	}

	যদি (_info.state != BidLockState::Empty) {
		নির্বাচন (_info.state) {
		ক্ষেত্র BidLockState::InvalidRecord:
			ফেরত Bid_Store_ফলাফল::InvalidRecord;

		ক্ষেত্র BidLockState::StorageUnavailable:
			ফেরত Bid_Store_ফলাফল::StorageUnavailable;

		ক্ষেত্র BidLockState::WriteError:
			ফেরত Bid_Store_ফলাফল::WriteError;

		ক্ষেত্র BidLockState::Unsupported:
			ফেরত Bid_Store_ফলাফল::Unsupported;

		ক্ষেত্র BidLockState::Empty:
		ক্ষেত্র BidLockState::Locked:
			বিরতি;
		}
	}

	ধ্রুবক Bid_Store_ফলাফল ফলাফল = BidStorage::checkOrStore(ua_type, uas_id);

	নির্বাচন (ফলাফল) {
	ক্ষেত্র Bid_Store_ফলাফল::Stored:
		_info.state = BidLockState::Locked;
		_info.id.ua_type = ua_type;
		memcpy(_info.id.uas_id, uas_id, UAS_ID_আকার);
		locked_id = _info.id;
		ফেরত ফলাফল;

	ক্ষেত্র Bid_Store_ফলাফল::AlreadyLocked:
	ক্ষেত্র Bid_Store_ফলাফল::AlreadyLockedDifferent: {
			_info = BidStorage::get();

			যদি (_info.locked()) {
				locked_id = _info.id;
				ফেরত Bid_Store_ফলাফল::AlreadyLocked;
			}

			ফেরত Bid_Store_ফলাফল::InvalidRecord;
		}

	ক্ষেত্র Bid_Store_ফলাফল::InvalidBasicId:
	ক্ষেত্র Bid_Store_ফলাফল::StorageUnavailable:
	ক্ষেত্র Bid_Store_ফলাফল::InvalidRecord:
	ক্ষেত্র Bid_Store_ফলাফল::WriteError:
	ক্ষেত্র Bid_Store_ফলাফল::Unsupported:
		ফেরত ফলাফল;
	}

	ফেরত Bid_Store_ফলাফল::InvalidRecord;
}

} // namespace open_drone_id
