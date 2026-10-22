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

#ಅನ್ತರ್ಭಾವ "odid_bid_storage.hpp"

#ಅನ್ತರ್ಭಾವ <fcntl.h>
#ಅನ್ತರ್ಭಾವ <stddef.h>
#ಅನ್ತರ್ಭಾವ <string.h>
#ಅನ್ತರ್ಭಾವ <unistd.h>

#ifdef __PX4_NUTTX
#ಅನ್ತರ್ಭಾವ <nuttx/crc32.h>
#ಅನ್ತರ್ಭಾವ <px4_platform_common/px4_manifest.h>
#ಅನ್ಯಥಾ
#ಅನ್ತರ್ಭಾವ <crc32.h>
#endif

ನಾಮಾಕಾಶ open_drone_id
{
ನಾಮಾಕಾಶ
{

ಸ್ಥಿರ constexpr uint32_t ODID_UASID_MAGIC = 0x4449444f; // "ODID"
ಸ್ಥಿರ constexpr uint8_t ODID_UASID_RECORD_VERSION = 1;

ಸಂರಚನಾ __attribute__((packed)) UasIdRecord {
	uint32_t magic;
	uint8_t version;
	uint8_t id_type;
	uint8_t ua_type;
	uint8_t reserved;
	uint8_t uas_id[UAS_ID_SIZE];
	uint32_t crc;
};

ವರ್ಗ BidStorageBackend
{
ಸಾರ್ವಜನಿಕ:
	BidLockInfo get();
	BidStoreResult checkOrStore(uint8_t ua_type, ಧ್ರುವ uint8_t uas_id[UAS_ID_SIZE]);

ಗುಪ್ತ:
	ಸ್ಥಿರ ತಾರ್ಕಿಕ uasIdIsEmpty(ಧ್ರುವ uint8_t uas_id[UAS_ID_SIZE])
	{
		ಪ್ರತಿ (unsigned i = 0; i < UAS_ID_SIZE; ++i) {
			ಯದಿ (uas_id[i] != 0) {
				ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
			}
		}

		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	}

	ಸ್ಥಿರ uint32_t recordCrc(ಧ್ರುವ UasIdRecord &record)
	{
		ಪ್ರತ್ಯಾಗಮ crc32(reinterpret_cast<ಧ್ರುವ uint8_t *>(&record.version),
			     offsetof(UasIdRecord, crc) - offsetof(UasIdRecord, version));
	}

	ಸ್ಥಿರ ತಾರ್ಕಿಕ recordValid(ಧ್ರುವ UasIdRecord &record)
	{
		ಪ್ರತ್ಯಾಗಮ record.magic == ODID_UASID_MAGIC
		       && record.version == ODID_UASID_RECORD_VERSION
		       && record.id_type == UAS_ID_TYPE_SERIAL_ಸಂಖ್ಯೆ
		       && !uasIdIsEmpty(record.uas_id)
		       && record.crc == recordCrc(record);
	}

	// Check if the record is all 0xff, which is the erased state of flash.
	// This is needed to distinguish between an unwritten record and a record that was written with all bytes set to 0 (which would be invalid but not unwritten).
	ಸ್ಥಿರ ತಾರ್ಕಿಕ recordBlank(ಧ್ರುವ UasIdRecord &record)
	{
		ಧ್ರುವ uint8_t *bytes = reinterpret_cast<ಧ್ರುವ uint8_t *>(&record);

		ಪ್ರತಿ (unsigned i = 0; i < ಪರಿಮಾಣ(record); ++i) {
			ಯದಿ (bytes[i] != 0xff) {
				ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
			}
		}

		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	}

	ಸ್ಥಿರ ತಾರ್ಕಿಕ recordMatches(ಧ್ರುವ UasIdRecord &record, ಧ್ರುವ uint8_t uas_id[UAS_ID_SIZE])
	{
		ಪ್ರತ್ಯಾಗಮ memcmp(record.uas_id, uas_id, UAS_ID_SIZE) == 0;
	}

	// Write the entire buffer to the file descriptor, handling short writes.
	ಸ್ಥಿರ ತಾರ್ಕಿಕ writeAll(ಪೂರ್ಣಾಙ್ಕ fd, ಧ್ರುವ ಶೂನ್ಯ *buffer, size_t size)
	{
		ಧ್ರುವ uint8_t *bytes = static_cast<ಧ್ರುವ uint8_t *>(buffer);

		ಯಾವತ್ (size > 0) {
			ಧ್ರುವ ssize_t written = ಲಿಖ(fd, bytes, size);

			ಯದಿ (written <= 0) {
				ಪ್ರತ್ಯಾಗಮ ಅಸತ್ಯ;
			}

			bytes += written;
			size -= written;
		}

		ಪ್ರತ್ಯಾಗಮ ಸತ್ಯ;
	}

};

static_assert(ಪರಿಮಾಣ(UasIdRecord) <= 256, "OpenDroneID UAS ID record must fit in /fs/mtd_id");

// The UAS ID storage is only supported on PX4 targets with NuttX and an MTD partition named "ID".
// On other platforms, the get() method will return StorageUnavailable and the checkOrStore() method will return Unsupported.
BidLockInfo BidStorageBackend::get()
{
#ಯದಿ defined(__PX4_NUTTX)
	ಧ್ರುವ ವರ್ಣ *path = nullptr;

	ಯದಿ (px4_mtd_query("MTD_ID", nullptr, &path) != 0 || path == nullptr) {
		ಪ್ರತ್ಯಾಗಮ {BidLockState::StorageUnavailable, {}};
	}

	ಪೂರ್ಣಾಙ್ಕ fd = open(path, O_RDONLY);

	ಯದಿ (fd < 0) {
		ಪ್ರತ್ಯಾಗಮ {BidLockState::StorageUnavailable, {}};
	}

	UasIdRecord record{};

	ಯದಿ (ಪಠ(fd, &record, ಪರಿಮಾಣ(record)) != ಪರಿಮಾಣ(record)) {
		close(fd);
		ಪ್ರತ್ಯಾಗಮ {BidLockState::StorageUnavailable, {}};
	}

	close(fd);

	ಯದಿ (recordValid(record)) {
		BidLockInfo info{BidLockState::Locked, {}};
		info.id.ua_type = record.ua_type;
		memcpy(info.id.uas_id, record.uas_id, UAS_ID_SIZE);
		ಪ್ರತ್ಯಾಗಮ info;
	}

	ಯದಿ (recordBlank(record)) {
		ಪ್ರತ್ಯಾಗಮ {BidLockState::Empty, {}};
	}

	ಪ್ರತ್ಯಾಗಮ {BidLockState::InvalidRecord, {}};
#ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ {BidLockState::Unsupported, {}};
#endif
}

// Check if the given UAS ID matches the stored one, or store it if the storage is empty.
// The storage is write-once, so once a UAS ID is stored it cannot be changed (except by erasing the flash).
BidStoreResult BidStorageBackend::checkOrStore(uint8_t ua_type, ಧ್ರುವ uint8_t uas_id[UAS_ID_SIZE])
{
	ಯದಿ (uasIdIsEmpty(uas_id)) {
		ಪ್ರತ್ಯಾಗಮ BidStoreResult::InvalidBasicId;
	}

#ಯದಿ defined(__PX4_NUTTX)
	ಧ್ರುವ ವರ್ಣ *path = nullptr;

	ಯದಿ (px4_mtd_query("MTD_ID", nullptr, &path) != 0 || path == nullptr) {
		ಪ್ರತ್ಯಾಗಮ BidStoreResult::StorageUnavailable;
	}

	ಪೂರ್ಣಾಙ್ಕ fd = open(path, O_RDWR);

	ಯದಿ (fd < 0) {
		ಪ್ರತ್ಯಾಗಮ BidStoreResult::StorageUnavailable;
	}

	UasIdRecord existing{};

	ಯದಿ (ಪಠ(fd, &existing, ಪರಿಮಾಣ(existing)) != ಪರಿಮಾಣ(existing)) {
		close(fd);
		ಪ್ರತ್ಯಾಗಮ BidStoreResult::StorageUnavailable;
	}

	ಯದಿ (recordValid(existing)) {
		ಧ್ರುವ BidStoreResult result = recordMatches(existing, uas_id)
					      ? BidStoreResult::AlreadyLocked
					      : BidStoreResult::AlreadyLockedDifferent;
		close(fd);
		ಪ್ರತ್ಯಾಗಮ result;
	}

	ಯದಿ (!recordBlank(existing)) {
		close(fd);
		ಪ್ರತ್ಯಾಗಮ BidStoreResult::InvalidRecord;
	}

	UasIdRecord record{};
	record.magic = ODID_UASID_MAGIC;
	record.version = ODID_UASID_RECORD_VERSION;
	record.id_type = UAS_ID_TYPE_SERIAL_ಸಂಖ್ಯೆ;
	record.ua_type = ua_type;
	memcpy(record.uas_id, uas_id, UAS_ID_SIZE);
	record.crc = recordCrc(record);

	ಧ್ರುವ uint32_t magic = record.magic;
	record.magic = 0xffffffff;

	ಧ್ರುವ ತಾರ್ಕಿಕ written = lseek(fd, ಪರಿಮಾಣ(record.magic), SEEK_SET) == static_cast<off_t>(ಪರಿಮಾಣ(record.magic))
			     && writeAll(fd, reinterpret_cast<ಧ್ರುವ uint8_t *>(&record) + ಪರಿಮಾಣ(record.magic),
					 ಪರಿಮಾಣ(record) - ಪರಿಮಾಣ(record.magic))
			     && fsync(fd) == 0
			     && lseek(fd, 0, SEEK_SET) == 0
			     && writeAll(fd, &magic, ಪರಿಮಾಣ(magic))
			     && fsync(fd) == 0;

	close(fd);

	ಪ್ರತ್ಯಾಗಮ written ? BidStoreResult::Stored : BidStoreResult::WriteError;
#ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ BidStoreResult::Unsupported;
#endif
}

} // namespace

// Public interface that uses the backend implementation.
BidLockInfo BidStorage::get()
{
	BidStorageBackend storage;
	ಪ್ರತ್ಯಾಗಮ storage.get();
}

BidStoreResult BidStorage::checkOrStore(uint8_t ua_type, ಧ್ರುವ uint8_t uas_id[UAS_ID_SIZE])
{
	BidStorageBackend storage;
	ಪ್ರತ್ಯಾಗಮ storage.checkOrStore(ua_type, uas_id);
}

//
BidStoreResult BidLock::resolve(uint8_t id_type, uint8_t ua_type, ಧ್ರುವ uint8_t uas_id[UAS_ID_SIZE],
				UasId &locked_id)
{
	ಯದಿ (id_type != UAS_ID_TYPE_SERIAL_ಸಂಖ್ಯೆ) {
		ಪ್ರತ್ಯಾಗಮ BidStoreResult::InvalidBasicId;
	}

	ಯದಿ (!_loaded) {
		_info = BidStorage::get();
		_loaded = ಸತ್ಯ;
	}

	ಯದಿ (_info.locked()) {
		locked_id = _info.id;
		ಪ್ರತ್ಯಾಗಮ BidStoreResult::AlreadyLocked;
	}

	ಯದಿ (_info.state != BidLockState::Empty) {
		ವಿಕಲ್ಪ (_info.state) {
		ಪಕ್ಷ BidLockState::InvalidRecord:
			ಪ್ರತ್ಯಾಗಮ BidStoreResult::InvalidRecord;

		ಪಕ್ಷ BidLockState::StorageUnavailable:
			ಪ್ರತ್ಯಾಗಮ BidStoreResult::StorageUnavailable;

		ಪಕ್ಷ BidLockState::WriteError:
			ಪ್ರತ್ಯಾಗಮ BidStoreResult::WriteError;

		ಪಕ್ಷ BidLockState::Unsupported:
			ಪ್ರತ್ಯಾಗಮ BidStoreResult::Unsupported;

		ಪಕ್ಷ BidLockState::Empty:
		ಪಕ್ಷ BidLockState::Locked:
			ಭಙ್ಗ;
		}
	}

	ಧ್ರುವ BidStoreResult result = BidStorage::checkOrStore(ua_type, uas_id);

	ವಿಕಲ್ಪ (result) {
	ಪಕ್ಷ BidStoreResult::Stored:
		_info.state = BidLockState::Locked;
		_info.id.ua_type = ua_type;
		memcpy(_info.id.uas_id, uas_id, UAS_ID_SIZE);
		locked_id = _info.id;
		ಪ್ರತ್ಯಾಗಮ result;

	ಪಕ್ಷ BidStoreResult::AlreadyLocked:
	ಪಕ್ಷ BidStoreResult::AlreadyLockedDifferent: {
			_info = BidStorage::get();

			ಯದಿ (_info.locked()) {
				locked_id = _info.id;
				ಪ್ರತ್ಯಾಗಮ BidStoreResult::AlreadyLocked;
			}

			ಪ್ರತ್ಯಾಗಮ BidStoreResult::InvalidRecord;
		}

	ಪಕ್ಷ BidStoreResult::InvalidBasicId:
	ಪಕ್ಷ BidStoreResult::StorageUnavailable:
	ಪಕ್ಷ BidStoreResult::InvalidRecord:
	ಪಕ್ಷ BidStoreResult::WriteError:
	ಪಕ್ಷ BidStoreResult::Unsupported:
		ಪ್ರತ್ಯಾಗಮ result;
	}

	ಪ್ರತ್ಯಾಗಮ BidStoreResult::InvalidRecord;
}

} // namespace open_drone_id
