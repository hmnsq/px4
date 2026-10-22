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

#அந்தர்பாவ "odid_bid_storage.hpp"

#அந்தர்பாவ <fcntl.h>
#அந்தர்பாவ <stddef.h>
#அந்தர்பாவ <string.h>
#அந்தர்பாவ <unistd.h>

#ifdef __PX4_NUTTX
#அந்தர்பாவ <nuttx/crc32.h>
#அந்தர்பாவ <px4_platform_common/px4_manifest.h>
#இல்லையெனில்
#அந்தர்பாவ <crc32.h>
#endif

நாமாகாஶ open_drone_id
{
நாமாகாஶ
{

நிலையான constexpr uint32_t ODID_UASID_MAGIC = 0x4449444f; // "ODID"
நிலையான constexpr uint8_t ODID_UASID_RECORD_VERSION = 1;

கட்டமைப்பு __attribute__((packed)) UasIdRecord {
	uint32_t magic;
	uint8_t version;
	uint8_t id_type;
	uint8_t ua_type;
	uint8_t reserved;
	uint8_t uas_id[UAS_ID_SIZE];
	uint32_t crc;
};

வகுப்பு BidStorageBackend
{
பொது:
	BidLockInfo get();
	BidStoreResult checkOrStore(uint8_t ua_type, மாறிலி uint8_t uas_id[UAS_ID_SIZE]);

தனிப்பட்ட:
	நிலையான தருக்க uasIdIsEmpty(மாறிலி uint8_t uas_id[UAS_ID_SIZE])
	{
		க்கு (unsigned i = 0; i < UAS_ID_SIZE; ++i) {
			எனில் (uas_id[i] != 0) {
				திரும்பு பொய்;
			}
		}

		திரும்பு உண்மை;
	}

	நிலையான uint32_t recordCrc(மாறிலி UasIdRecord &record)
	{
		திரும்பு crc32(reinterpret_cast<மாறிலி uint8_t *>(&record.version),
			     offsetof(UasIdRecord, crc) - offsetof(UasIdRecord, version));
	}

	நிலையான தருக்க recordValid(மாறிலி UasIdRecord &record)
	{
		திரும்பு record.magic == ODID_UASID_MAGIC
		       && record.version == ODID_UASID_RECORD_VERSION
		       && record.id_type == UAS_ID_TYPE_SERIAL_எண்
		       && !uasIdIsEmpty(record.uas_id)
		       && record.crc == recordCrc(record);
	}

	// Check if the record is all 0xff, which is the erased state of flash.
	// This is needed to distinguish between an unwritten record and a record that was written with all bytes set to 0 (which would be invalid but not unwritten).
	நிலையான தருக்க recordBlank(மாறிலி UasIdRecord &record)
	{
		மாறிலி uint8_t *bytes = reinterpret_cast<மாறிலி uint8_t *>(&record);

		க்கு (unsigned i = 0; i < அளவு(record); ++i) {
			எனில் (bytes[i] != 0xff) {
				திரும்பு பொய்;
			}
		}

		திரும்பு உண்மை;
	}

	நிலையான தருக்க recordMatches(மாறிலி UasIdRecord &record, மாறிலி uint8_t uas_id[UAS_ID_SIZE])
	{
		திரும்பு memcmp(record.uas_id, uas_id, UAS_ID_SIZE) == 0;
	}

	// Write the entire buffer to the file descriptor, handling short writes.
	நிலையான தருக்க writeAll(முழு fd, மாறிலி வெறுமை *buffer, size_t size)
	{
		மாறிலி uint8_t *bytes = static_cast<மாறிலி uint8_t *>(buffer);

		போது (size > 0) {
			மாறிலி ssize_t written = லிக(fd, bytes, size);

			எனில் (written <= 0) {
				திரும்பு பொய்;
			}

			bytes += written;
			size -= written;
		}

		திரும்பு உண்மை;
	}

};

static_assert(அளவு(UasIdRecord) <= 256, "OpenDroneID UAS ID record must fit in /fs/mtd_id");

// The UAS ID storage is only supported on PX4 targets with NuttX and an MTD partition named "ID".
// On other platforms, the get() method will return StorageUnavailable and the checkOrStore() method will return Unsupported.
BidLockInfo BidStorageBackend::get()
{
#எனில் defined(__PX4_NUTTX)
	மாறிலி எழுத்து *path = nullptr;

	எனில் (px4_mtd_query("MTD_ID", nullptr, &path) != 0 || path == nullptr) {
		திரும்பு {BidLockState::StorageUnavailable, {}};
	}

	முழு fd = open(path, O_RDONLY);

	எனில் (fd < 0) {
		திரும்பு {BidLockState::StorageUnavailable, {}};
	}

	UasIdRecord record{};

	எனில் (பட(fd, &record, அளவு(record)) != அளவு(record)) {
		close(fd);
		திரும்பு {BidLockState::StorageUnavailable, {}};
	}

	close(fd);

	எனில் (recordValid(record)) {
		BidLockInfo info{BidLockState::Locked, {}};
		info.id.ua_type = record.ua_type;
		memcpy(info.id.uas_id, record.uas_id, UAS_ID_SIZE);
		திரும்பு info;
	}

	எனில் (recordBlank(record)) {
		திரும்பு {BidLockState::Empty, {}};
	}

	திரும்பு {BidLockState::InvalidRecord, {}};
#இல்லையெனில்
	திரும்பு {BidLockState::Unsupported, {}};
#endif
}

// Check if the given UAS ID matches the stored one, or store it if the storage is empty.
// The storage is write-once, so once a UAS ID is stored it cannot be changed (except by erasing the flash).
BidStoreResult BidStorageBackend::checkOrStore(uint8_t ua_type, மாறிலி uint8_t uas_id[UAS_ID_SIZE])
{
	எனில் (uasIdIsEmpty(uas_id)) {
		திரும்பு BidStoreResult::InvalidBasicId;
	}

#எனில் defined(__PX4_NUTTX)
	மாறிலி எழுத்து *path = nullptr;

	எனில் (px4_mtd_query("MTD_ID", nullptr, &path) != 0 || path == nullptr) {
		திரும்பு BidStoreResult::StorageUnavailable;
	}

	முழு fd = open(path, O_RDWR);

	எனில் (fd < 0) {
		திரும்பு BidStoreResult::StorageUnavailable;
	}

	UasIdRecord existing{};

	எனில் (பட(fd, &existing, அளவு(existing)) != அளவு(existing)) {
		close(fd);
		திரும்பு BidStoreResult::StorageUnavailable;
	}

	எனில் (recordValid(existing)) {
		மாறிலி BidStoreResult result = recordMatches(existing, uas_id)
					      ? BidStoreResult::AlreadyLocked
					      : BidStoreResult::AlreadyLockedDifferent;
		close(fd);
		திரும்பு result;
	}

	எனில் (!recordBlank(existing)) {
		close(fd);
		திரும்பு BidStoreResult::InvalidRecord;
	}

	UasIdRecord record{};
	record.magic = ODID_UASID_MAGIC;
	record.version = ODID_UASID_RECORD_VERSION;
	record.id_type = UAS_ID_TYPE_SERIAL_எண்;
	record.ua_type = ua_type;
	memcpy(record.uas_id, uas_id, UAS_ID_SIZE);
	record.crc = recordCrc(record);

	மாறிலி uint32_t magic = record.magic;
	record.magic = 0xffffffff;

	மாறிலி தருக்க written = lseek(fd, அளவு(record.magic), SEEK_SET) == static_cast<off_t>(அளவு(record.magic))
			     && writeAll(fd, reinterpret_cast<மாறிலி uint8_t *>(&record) + அளவு(record.magic),
					 அளவு(record) - அளவு(record.magic))
			     && fsync(fd) == 0
			     && lseek(fd, 0, SEEK_SET) == 0
			     && writeAll(fd, &magic, அளவு(magic))
			     && fsync(fd) == 0;

	close(fd);

	திரும்பு written ? BidStoreResult::Stored : BidStoreResult::WriteError;
#இல்லையெனில்
	திரும்பு BidStoreResult::Unsupported;
#endif
}

} // namespace

// Public interface that uses the backend implementation.
BidLockInfo BidStorage::get()
{
	BidStorageBackend storage;
	திரும்பு storage.get();
}

BidStoreResult BidStorage::checkOrStore(uint8_t ua_type, மாறிலி uint8_t uas_id[UAS_ID_SIZE])
{
	BidStorageBackend storage;
	திரும்பு storage.checkOrStore(ua_type, uas_id);
}

//
BidStoreResult BidLock::resolve(uint8_t id_type, uint8_t ua_type, மாறிலி uint8_t uas_id[UAS_ID_SIZE],
				UasId &locked_id)
{
	எனில் (id_type != UAS_ID_TYPE_SERIAL_எண்) {
		திரும்பு BidStoreResult::InvalidBasicId;
	}

	எனில் (!_loaded) {
		_info = BidStorage::get();
		_loaded = உண்மை;
	}

	எனில் (_info.locked()) {
		locked_id = _info.id;
		திரும்பு BidStoreResult::AlreadyLocked;
	}

	எனில் (_info.state != BidLockState::Empty) {
		தேர்வு (_info.state) {
		நிலை BidLockState::InvalidRecord:
			திரும்பு BidStoreResult::InvalidRecord;

		நிலை BidLockState::StorageUnavailable:
			திரும்பு BidStoreResult::StorageUnavailable;

		நிலை BidLockState::WriteError:
			திரும்பு BidStoreResult::WriteError;

		நிலை BidLockState::Unsupported:
			திரும்பு BidStoreResult::Unsupported;

		நிலை BidLockState::Empty:
		நிலை BidLockState::Locked:
			நிறுத்து;
		}
	}

	மாறிலி BidStoreResult result = BidStorage::checkOrStore(ua_type, uas_id);

	தேர்வு (result) {
	நிலை BidStoreResult::Stored:
		_info.state = BidLockState::Locked;
		_info.id.ua_type = ua_type;
		memcpy(_info.id.uas_id, uas_id, UAS_ID_SIZE);
		locked_id = _info.id;
		திரும்பு result;

	நிலை BidStoreResult::AlreadyLocked:
	நிலை BidStoreResult::AlreadyLockedDifferent: {
			_info = BidStorage::get();

			எனில் (_info.locked()) {
				locked_id = _info.id;
				திரும்பு BidStoreResult::AlreadyLocked;
			}

			திரும்பு BidStoreResult::InvalidRecord;
		}

	நிலை BidStoreResult::InvalidBasicId:
	நிலை BidStoreResult::StorageUnavailable:
	நிலை BidStoreResult::InvalidRecord:
	நிலை BidStoreResult::WriteError:
	நிலை BidStoreResult::Unsupported:
		திரும்பு result;
	}

	திரும்பு BidStoreResult::InvalidRecord;
}

} // namespace open_drone_id
