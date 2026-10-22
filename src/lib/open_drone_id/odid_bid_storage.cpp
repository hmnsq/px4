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

#অন্তর্ভাৱ "odid_bid_storage.hpp"

#অন্তর্ভাৱ <fcntl.h>
#অন্তর্ভাৱ <stddef.h>
#অন্তর্ভাৱ <string.h>
#অন্তর্ভাৱ <unistd.h>

#ifdef __PX4_NUTTX
#অন্তর্ভাৱ <nuttx/crc32.h>
#অন্তর্ভাৱ <px4_platform_common/px4_manifest.h>
#অন্যথা
#অন্তর্ভাৱ <crc32.h>
#endif

নামাকাশ open_drone_id
{
নামাকাশ
{

স্থির constexpr uint32_t ODID_UASID_MAGIC = 0x4449444f; // "ODID"
স্থির constexpr uint8_t ODID_UASID_RECORD_VERSION = 1;

সংরচনা __attribute__((packed)) UasIdRecord {
	uint32_t magic;
	uint8_t version;
	uint8_t id_type;
	uint8_t ua_type;
	uint8_t reserved;
	uint8_t uas_id[UAS_ID_SIZE];
	uint32_t crc;
};

শ্ৰেণী BidStorageBackend
{
সার্ৱজনিক:
	BidLockInfo get();
	BidStoreResult checkOrStore(uint8_t ua_type, ধ্রুৱ uint8_t uas_id[UAS_ID_SIZE]);

গুপ্ত:
	স্থির তার্কিক uasIdIsEmpty(ধ্রুৱ uint8_t uas_id[UAS_ID_SIZE])
	{
		বাবে (unsigned i = 0; i < UAS_ID_SIZE; ++i) {
			যদি (uas_id[i] != 0) {
				ঘূৰাই মিছা;
			}
		}

		ঘূৰাই সঁচা;
	}

	স্থির uint32_t recordCrc(ধ্রুৱ UasIdRecord &record)
	{
		ঘূৰাই crc32(reinterpret_cast<ধ্রুৱ uint8_t *>(&record.version),
			     offsetof(UasIdRecord, crc) - offsetof(UasIdRecord, version));
	}

	স্থির তার্কিক recordValid(ধ্রুৱ UasIdRecord &record)
	{
		ঘূৰাই record.magic == ODID_UASID_MAGIC
		       && record.version == ODID_UASID_RECORD_VERSION
		       && record.id_type == UAS_ID_TYPE_SERIAL_সংখ্যা
		       && !uasIdIsEmpty(record.uas_id)
		       && record.crc == recordCrc(record);
	}

	// Check if the record is all 0xff, which is the erased state of flash.
	// This is needed to distinguish between an unwritten record and a record that was written with all bytes set to 0 (which would be invalid but not unwritten).
	স্থির তার্কিক recordBlank(ধ্রুৱ UasIdRecord &record)
	{
		ধ্রুৱ uint8_t *bytes = reinterpret_cast<ধ্রুৱ uint8_t *>(&record);

		বাবে (unsigned i = 0; i < পরিমাণ(record); ++i) {
			যদি (bytes[i] != 0xff) {
				ঘূৰাই মিছা;
			}
		}

		ঘূৰাই সঁচা;
	}

	স্থির তার্কিক recordMatches(ধ্রুৱ UasIdRecord &record, ধ্রুৱ uint8_t uas_id[UAS_ID_SIZE])
	{
		ঘূৰাই memcmp(record.uas_id, uas_id, UAS_ID_SIZE) == 0;
	}

	// Write the entire buffer to the file descriptor, handling short writes.
	স্থির তার্কিক writeAll(পূর্ণাঙ্ক fd, ধ্রুৱ শূন্য *buffer, size_t size)
	{
		ধ্রুৱ uint8_t *bytes = static_cast<ধ্রুৱ uint8_t *>(buffer);

		যেতিয়ালৈ (size > 0) {
			ধ্রুৱ ssize_t written = লিখ(fd, bytes, size);

			যদি (written <= 0) {
				ঘূৰাই মিছা;
			}

			bytes += written;
			size -= written;
		}

		ঘূৰাই সঁচা;
	}

};

static_assert(পরিমাণ(UasIdRecord) <= 256, "OpenDroneID UAS ID record must fit in /fs/mtd_id");

// The UAS ID storage is only supported on PX4 targets with NuttX and an MTD partition named "ID".
// On other platforms, the get() method will return StorageUnavailable and the checkOrStore() method will return Unsupported.
BidLockInfo BidStorageBackend::get()
{
#যদি defined(__PX4_NUTTX)
	ধ্রুৱ ৱর্ণ *path = nullptr;

	যদি (px4_mtd_query("MTD_ID", nullptr, &path) != 0 || path == nullptr) {
		ঘূৰাই {BidLockState::StorageUnavailable, {}};
	}

	পূর্ণাঙ্ক fd = open(path, O_RDONLY);

	যদি (fd < 0) {
		ঘূৰাই {BidLockState::StorageUnavailable, {}};
	}

	UasIdRecord record{};

	যদি (পঠ(fd, &record, পরিমাণ(record)) != পরিমাণ(record)) {
		close(fd);
		ঘূৰাই {BidLockState::StorageUnavailable, {}};
	}

	close(fd);

	যদি (recordValid(record)) {
		BidLockInfo info{BidLockState::Locked, {}};
		info.id.ua_type = record.ua_type;
		memcpy(info.id.uas_id, record.uas_id, UAS_ID_SIZE);
		ঘূৰাই info;
	}

	যদি (recordBlank(record)) {
		ঘূৰাই {BidLockState::Empty, {}};
	}

	ঘূৰাই {BidLockState::InvalidRecord, {}};
#অন্যথা
	ঘূৰাই {BidLockState::Unsupported, {}};
#endif
}

// Check if the given UAS ID matches the stored one, or store it if the storage is empty.
// The storage is write-once, so once a UAS ID is stored it cannot be changed (except by erasing the flash).
BidStoreResult BidStorageBackend::checkOrStore(uint8_t ua_type, ধ্রুৱ uint8_t uas_id[UAS_ID_SIZE])
{
	যদি (uasIdIsEmpty(uas_id)) {
		ঘূৰাই BidStoreResult::InvalidBasicId;
	}

#যদি defined(__PX4_NUTTX)
	ধ্রুৱ ৱর্ণ *path = nullptr;

	যদি (px4_mtd_query("MTD_ID", nullptr, &path) != 0 || path == nullptr) {
		ঘূৰাই BidStoreResult::StorageUnavailable;
	}

	পূর্ণাঙ্ক fd = open(path, O_RDWR);

	যদি (fd < 0) {
		ঘূৰাই BidStoreResult::StorageUnavailable;
	}

	UasIdRecord existing{};

	যদি (পঠ(fd, &existing, পরিমাণ(existing)) != পরিমাণ(existing)) {
		close(fd);
		ঘূৰাই BidStoreResult::StorageUnavailable;
	}

	যদি (recordValid(existing)) {
		ধ্রুৱ BidStoreResult result = recordMatches(existing, uas_id)
					      ? BidStoreResult::AlreadyLocked
					      : BidStoreResult::AlreadyLockedDifferent;
		close(fd);
		ঘূৰাই result;
	}

	যদি (!recordBlank(existing)) {
		close(fd);
		ঘূৰাই BidStoreResult::InvalidRecord;
	}

	UasIdRecord record{};
	record.magic = ODID_UASID_MAGIC;
	record.version = ODID_UASID_RECORD_VERSION;
	record.id_type = UAS_ID_TYPE_SERIAL_সংখ্যা;
	record.ua_type = ua_type;
	memcpy(record.uas_id, uas_id, UAS_ID_SIZE);
	record.crc = recordCrc(record);

	ধ্রুৱ uint32_t magic = record.magic;
	record.magic = 0xffffffff;

	ধ্রুৱ তার্কিক written = lseek(fd, পরিমাণ(record.magic), SEEK_SET) == static_cast<off_t>(পরিমাণ(record.magic))
			     && writeAll(fd, reinterpret_cast<ধ্রুৱ uint8_t *>(&record) + পরিমাণ(record.magic),
					 পরিমাণ(record) - পরিমাণ(record.magic))
			     && fsync(fd) == 0
			     && lseek(fd, 0, SEEK_SET) == 0
			     && writeAll(fd, &magic, পরিমাণ(magic))
			     && fsync(fd) == 0;

	close(fd);

	ঘূৰাই written ? BidStoreResult::Stored : BidStoreResult::WriteError;
#অন্যথা
	ঘূৰাই BidStoreResult::Unsupported;
#endif
}

} // namespace

// Public interface that uses the backend implementation.
BidLockInfo BidStorage::get()
{
	BidStorageBackend storage;
	ঘূৰাই storage.get();
}

BidStoreResult BidStorage::checkOrStore(uint8_t ua_type, ধ্রুৱ uint8_t uas_id[UAS_ID_SIZE])
{
	BidStorageBackend storage;
	ঘূৰাই storage.checkOrStore(ua_type, uas_id);
}

//
BidStoreResult BidLock::resolve(uint8_t id_type, uint8_t ua_type, ধ্রুৱ uint8_t uas_id[UAS_ID_SIZE],
				UasId &locked_id)
{
	যদি (id_type != UAS_ID_TYPE_SERIAL_সংখ্যা) {
		ঘূৰাই BidStoreResult::InvalidBasicId;
	}

	যদি (!_loaded) {
		_info = BidStorage::get();
		_loaded = সঁচা;
	}

	যদি (_info.locked()) {
		locked_id = _info.id;
		ঘূৰাই BidStoreResult::AlreadyLocked;
	}

	যদি (_info.state != BidLockState::Empty) {
		ৱিকল্প (_info.state) {
		পক্ষ BidLockState::InvalidRecord:
			ঘূৰাই BidStoreResult::InvalidRecord;

		পক্ষ BidLockState::StorageUnavailable:
			ঘূৰাই BidStoreResult::StorageUnavailable;

		পক্ষ BidLockState::WriteError:
			ঘূৰাই BidStoreResult::WriteError;

		পক্ষ BidLockState::Unsupported:
			ঘূৰাই BidStoreResult::Unsupported;

		পক্ষ BidLockState::Empty:
		পক্ষ BidLockState::Locked:
			ভঙ্গ;
		}
	}

	ধ্রুৱ BidStoreResult result = BidStorage::checkOrStore(ua_type, uas_id);

	ৱিকল্প (result) {
	পক্ষ BidStoreResult::Stored:
		_info.state = BidLockState::Locked;
		_info.id.ua_type = ua_type;
		memcpy(_info.id.uas_id, uas_id, UAS_ID_SIZE);
		locked_id = _info.id;
		ঘূৰাই result;

	পক্ষ BidStoreResult::AlreadyLocked:
	পক্ষ BidStoreResult::AlreadyLockedDifferent: {
			_info = BidStorage::get();

			যদি (_info.locked()) {
				locked_id = _info.id;
				ঘূৰাই BidStoreResult::AlreadyLocked;
			}

			ঘূৰাই BidStoreResult::InvalidRecord;
		}

	পক্ষ BidStoreResult::InvalidBasicId:
	পক্ষ BidStoreResult::StorageUnavailable:
	পক্ষ BidStoreResult::InvalidRecord:
	পক্ষ BidStoreResult::WriteError:
	পক্ষ BidStoreResult::Unsupported:
		ঘূৰাই result;
	}

	ঘূৰাই BidStoreResult::InvalidRecord;
}

} // namespace open_drone_id
