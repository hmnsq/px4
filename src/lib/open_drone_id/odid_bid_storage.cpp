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

#അന്തര്ഭാവ "odid_bid_storage.hpp"

#അന്തര്ഭാവ <fcntl.h>
#അന്തര്ഭാവ <stddef.h>
#അന്തര്ഭാവ <string.h>
#അന്തര്ഭാവ <unistd.h>

#ifdef __PX4_NUTTX
#അന്തര്ഭാവ <nuttx/crc32.h>
#അന്തര്ഭാവ <px4_platform_common/px4_manifest.h>
#അല്ലെങ്കിൽ
#അന്തര്ഭാവ <crc32.h>
#endif

നാമാകാശ open_drone_id
{
നാമാകാശ
{

സ്ഥിതം constexpr uint32_t ODID_UASID_MAGIC = 0x4449444f; // "ODID"
സ്ഥിതം constexpr uint8_t ODID_UASID_RECORD_VERSION = 1;

ഘടന __attribute__((packed)) UasIdRecord {
	uint32_t magic;
	uint8_t version;
	uint8_t id_type;
	uint8_t ua_type;
	uint8_t reserved;
	uint8_t uas_id[UAS_ID_SIZE];
	uint32_t crc;
};

വർഗ്ഗം BidStorageBackend
{
പൊതു:
	BidLockInfo get();
	BidStoreResult checkOrStore(uint8_t ua_type, സ്ഥിരം uint8_t uas_id[UAS_ID_SIZE]);

സ്വകാര്യ:
	സ്ഥിതം തർക്കം uasIdIsEmpty(സ്ഥിരം uint8_t uas_id[UAS_ID_SIZE])
	{
		വേണ്ടി (unsigned i = 0; i < UAS_ID_SIZE; ++i) {
			എങ്കിൽ (uas_id[i] != 0) {
				തിരികെ അസത്യം;
			}
		}

		തിരികെ സത്യം;
	}

	സ്ഥിതം uint32_t recordCrc(സ്ഥിരം UasIdRecord &record)
	{
		തിരികെ crc32(reinterpret_cast<സ്ഥിരം uint8_t *>(&record.version),
			     offsetof(UasIdRecord, crc) - offsetof(UasIdRecord, version));
	}

	സ്ഥിതം തർക്കം recordValid(സ്ഥിരം UasIdRecord &record)
	{
		തിരികെ record.magic == ODID_UASID_MAGIC
		       && record.version == ODID_UASID_RECORD_VERSION
		       && record.id_type == UAS_ID_TYPE_SERIAL_സംഖ്യ
		       && !uasIdIsEmpty(record.uas_id)
		       && record.crc == recordCrc(record);
	}

	// Check if the record is all 0xff, which is the erased state of flash.
	// This is needed to distinguish between an unwritten record and a record that was written with all bytes set to 0 (which would be invalid but not unwritten).
	സ്ഥിതം തർക്കം recordBlank(സ്ഥിരം UasIdRecord &record)
	{
		സ്ഥിരം uint8_t *bytes = reinterpret_cast<സ്ഥിരം uint8_t *>(&record);

		വേണ്ടി (unsigned i = 0; i < വലുപ്പം(record); ++i) {
			എങ്കിൽ (bytes[i] != 0xff) {
				തിരികെ അസത്യം;
			}
		}

		തിരികെ സത്യം;
	}

	സ്ഥിതം തർക്കം recordMatches(സ്ഥിരം UasIdRecord &record, സ്ഥിരം uint8_t uas_id[UAS_ID_SIZE])
	{
		തിരികെ memcmp(record.uas_id, uas_id, UAS_ID_SIZE) == 0;
	}

	// Write the entire buffer to the file descriptor, handling short writes.
	സ്ഥിതം തർക്കം writeAll(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം ശൂന്യം *buffer, size_t size)
	{
		സ്ഥിരം uint8_t *bytes = static_cast<സ്ഥിരം uint8_t *>(buffer);

		ആകുമ്പോൾ (size > 0) {
			സ്ഥിരം ssize_t written = ലിഖ(fd, bytes, size);

			എങ്കിൽ (written <= 0) {
				തിരികെ അസത്യം;
			}

			bytes += written;
			size -= written;
		}

		തിരികെ സത്യം;
	}

};

static_assert(വലുപ്പം(UasIdRecord) <= 256, "OpenDroneID UAS ID record must fit in /fs/mtd_id");

// The UAS ID storage is only supported on PX4 targets with NuttX and an MTD partition named "ID".
// On other platforms, the get() method will return StorageUnavailable and the checkOrStore() method will return Unsupported.
BidLockInfo BidStorageBackend::get()
{
#എങ്കിൽ defined(__PX4_NUTTX)
	സ്ഥിരം അക്ഷരം *path = nullptr;

	എങ്കിൽ (px4_mtd_query("MTD_ID", nullptr, &path) != 0 || path == nullptr) {
		തിരികെ {BidLockState::StorageUnavailable, {}};
	}

	പൂർണ്ണസംഖ്യ fd = open(path, O_RDONLY);

	എങ്കിൽ (fd < 0) {
		തിരികെ {BidLockState::StorageUnavailable, {}};
	}

	UasIdRecord record{};

	എങ്കിൽ (പഠ(fd, &record, വലുപ്പം(record)) != വലുപ്പം(record)) {
		close(fd);
		തിരികെ {BidLockState::StorageUnavailable, {}};
	}

	close(fd);

	എങ്കിൽ (recordValid(record)) {
		BidLockInfo info{BidLockState::Locked, {}};
		info.id.ua_type = record.ua_type;
		memcpy(info.id.uas_id, record.uas_id, UAS_ID_SIZE);
		തിരികെ info;
	}

	എങ്കിൽ (recordBlank(record)) {
		തിരികെ {BidLockState::Empty, {}};
	}

	തിരികെ {BidLockState::InvalidRecord, {}};
#അല്ലെങ്കിൽ
	തിരികെ {BidLockState::Unsupported, {}};
#endif
}

// Check if the given UAS ID matches the stored one, or store it if the storage is empty.
// The storage is write-once, so once a UAS ID is stored it cannot be changed (except by erasing the flash).
BidStoreResult BidStorageBackend::checkOrStore(uint8_t ua_type, സ്ഥിരം uint8_t uas_id[UAS_ID_SIZE])
{
	എങ്കിൽ (uasIdIsEmpty(uas_id)) {
		തിരികെ BidStoreResult::InvalidBasicId;
	}

#എങ്കിൽ defined(__PX4_NUTTX)
	സ്ഥിരം അക്ഷരം *path = nullptr;

	എങ്കിൽ (px4_mtd_query("MTD_ID", nullptr, &path) != 0 || path == nullptr) {
		തിരികെ BidStoreResult::StorageUnavailable;
	}

	പൂർണ്ണസംഖ്യ fd = open(path, O_RDWR);

	എങ്കിൽ (fd < 0) {
		തിരികെ BidStoreResult::StorageUnavailable;
	}

	UasIdRecord existing{};

	എങ്കിൽ (പഠ(fd, &existing, വലുപ്പം(existing)) != വലുപ്പം(existing)) {
		close(fd);
		തിരികെ BidStoreResult::StorageUnavailable;
	}

	എങ്കിൽ (recordValid(existing)) {
		സ്ഥിരം BidStoreResult result = recordMatches(existing, uas_id)
					      ? BidStoreResult::AlreadyLocked
					      : BidStoreResult::AlreadyLockedDifferent;
		close(fd);
		തിരികെ result;
	}

	എങ്കിൽ (!recordBlank(existing)) {
		close(fd);
		തിരികെ BidStoreResult::InvalidRecord;
	}

	UasIdRecord record{};
	record.magic = ODID_UASID_MAGIC;
	record.version = ODID_UASID_RECORD_VERSION;
	record.id_type = UAS_ID_TYPE_SERIAL_സംഖ്യ;
	record.ua_type = ua_type;
	memcpy(record.uas_id, uas_id, UAS_ID_SIZE);
	record.crc = recordCrc(record);

	സ്ഥിരം uint32_t magic = record.magic;
	record.magic = 0xffffffff;

	സ്ഥിരം തർക്കം written = lseek(fd, വലുപ്പം(record.magic), SEEK_SET) == static_cast<off_t>(വലുപ്പം(record.magic))
			     && writeAll(fd, reinterpret_cast<സ്ഥിരം uint8_t *>(&record) + വലുപ്പം(record.magic),
					 വലുപ്പം(record) - വലുപ്പം(record.magic))
			     && fsync(fd) == 0
			     && lseek(fd, 0, SEEK_SET) == 0
			     && writeAll(fd, &magic, വലുപ്പം(magic))
			     && fsync(fd) == 0;

	close(fd);

	തിരികെ written ? BidStoreResult::Stored : BidStoreResult::WriteError;
#അല്ലെങ്കിൽ
	തിരികെ BidStoreResult::Unsupported;
#endif
}

} // namespace

// Public interface that uses the backend implementation.
BidLockInfo BidStorage::get()
{
	BidStorageBackend storage;
	തിരികെ storage.get();
}

BidStoreResult BidStorage::checkOrStore(uint8_t ua_type, സ്ഥിരം uint8_t uas_id[UAS_ID_SIZE])
{
	BidStorageBackend storage;
	തിരികെ storage.checkOrStore(ua_type, uas_id);
}

//
BidStoreResult BidLock::resolve(uint8_t id_type, uint8_t ua_type, സ്ഥിരം uint8_t uas_id[UAS_ID_SIZE],
				UasId &locked_id)
{
	എങ്കിൽ (id_type != UAS_ID_TYPE_SERIAL_സംഖ്യ) {
		തിരികെ BidStoreResult::InvalidBasicId;
	}

	എങ്കിൽ (!_loaded) {
		_info = BidStorage::get();
		_loaded = സത്യം;
	}

	എങ്കിൽ (_info.locked()) {
		locked_id = _info.id;
		തിരികെ BidStoreResult::AlreadyLocked;
	}

	എങ്കിൽ (_info.state != BidLockState::Empty) {
		തിരഞ്ഞെടുപ്പ് (_info.state) {
		അവസ്ഥ BidLockState::InvalidRecord:
			തിരികെ BidStoreResult::InvalidRecord;

		അവസ്ഥ BidLockState::StorageUnavailable:
			തിരികെ BidStoreResult::StorageUnavailable;

		അവസ്ഥ BidLockState::WriteError:
			തിരികെ BidStoreResult::WriteError;

		അവസ്ഥ BidLockState::Unsupported:
			തിരികെ BidStoreResult::Unsupported;

		അവസ്ഥ BidLockState::Empty:
		അവസ്ഥ BidLockState::Locked:
			നിർത്തുക;
		}
	}

	സ്ഥിരം BidStoreResult result = BidStorage::checkOrStore(ua_type, uas_id);

	തിരഞ്ഞെടുപ്പ് (result) {
	അവസ്ഥ BidStoreResult::Stored:
		_info.state = BidLockState::Locked;
		_info.id.ua_type = ua_type;
		memcpy(_info.id.uas_id, uas_id, UAS_ID_SIZE);
		locked_id = _info.id;
		തിരികെ result;

	അവസ്ഥ BidStoreResult::AlreadyLocked:
	അവസ്ഥ BidStoreResult::AlreadyLockedDifferent: {
			_info = BidStorage::get();

			എങ്കിൽ (_info.locked()) {
				locked_id = _info.id;
				തിരികെ BidStoreResult::AlreadyLocked;
			}

			തിരികെ BidStoreResult::InvalidRecord;
		}

	അവസ്ഥ BidStoreResult::InvalidBasicId:
	അവസ്ഥ BidStoreResult::StorageUnavailable:
	അവസ്ഥ BidStoreResult::InvalidRecord:
	അവസ്ഥ BidStoreResult::WriteError:
	അവസ്ഥ BidStoreResult::Unsupported:
		തിരികെ result;
	}

	തിരികെ BidStoreResult::InvalidRecord;
}

} // namespace open_drone_id
