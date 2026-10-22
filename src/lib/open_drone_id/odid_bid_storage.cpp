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

#અન્તર્ભાવ "odid_bid_storage.hpp"

#અન્તર્ભાવ <fcntl.h>
#અન્તર્ભાવ <stddef.h>
#અન્તર્ભાવ <string.h>
#અન્તર્ભાવ <unistd.h>

#ifdef __PX4_NUTTX
#અન્તર્ભાવ <nuttx/crc32.h>
#અન્તર્ભાવ <px4_platform_common/px4_manifest.h>
#નહીંતર
#અન્તર્ભાવ <crc32.h>
#endif

નામાકાશ open_drone_id
{
નામાકાશ
{

સ્થિર constexpr uint32_t ODID_UASID_MAGIC = 0x4449444f; // "ODID"
સ્થિર constexpr uint8_t ODID_UASID_RECORD_VERSION = 1;

સંરચના __attribute__((packed)) UasIdRecord {
	uint32_t magic;
	uint8_t version;
	uint8_t id_type;
	uint8_t ua_type;
	uint8_t reserved;
	uint8_t uas_id[UAS_ID_SIZE];
	uint32_t crc;
};

વર્ગ BidStorageBackend
{
જાહેર:
	BidLockInfo get();
	BidStoreResult checkOrStore(uint8_t ua_type, અચલ uint8_t uas_id[UAS_ID_SIZE]);

ખાનગી:
	સ્થિર તાર્કિક uasIdIsEmpty(અચલ uint8_t uas_id[UAS_ID_SIZE])
	{
		માટે (unsigned i = 0; i < UAS_ID_SIZE; ++i) {
			જો (uas_id[i] != 0) {
				પાછા ખોટું;
			}
		}

		પાછા સાચું;
	}

	સ્થિર uint32_t recordCrc(અચલ UasIdRecord &record)
	{
		પાછા crc32(reinterpret_cast<અચલ uint8_t *>(&record.version),
			     offsetof(UasIdRecord, crc) - offsetof(UasIdRecord, version));
	}

	સ્થિર તાર્કિક recordValid(અચલ UasIdRecord &record)
	{
		પાછા record.magic == ODID_UASID_MAGIC
		       && record.version == ODID_UASID_RECORD_VERSION
		       && record.id_type == UAS_ID_TYPE_SERIAL_સંખ્યા
		       && !uasIdIsEmpty(record.uas_id)
		       && record.crc == recordCrc(record);
	}

	// Check if the record is all 0xff, which is the erased state of flash.
	// This is needed to distinguish between an unwritten record and a record that was written with all bytes set to 0 (which would be invalid but not unwritten).
	સ્થિર તાર્કિક recordBlank(અચલ UasIdRecord &record)
	{
		અચલ uint8_t *bytes = reinterpret_cast<અચલ uint8_t *>(&record);

		માટે (unsigned i = 0; i < કદ(record); ++i) {
			જો (bytes[i] != 0xff) {
				પાછા ખોટું;
			}
		}

		પાછા સાચું;
	}

	સ્થિર તાર્કિક recordMatches(અચલ UasIdRecord &record, અચલ uint8_t uas_id[UAS_ID_SIZE])
	{
		પાછા memcmp(record.uas_id, uas_id, UAS_ID_SIZE) == 0;
	}

	// Write the entire buffer to the file descriptor, handling short writes.
	સ્થિર તાર્કિક writeAll(પૂર્ણાંક fd, અચલ શૂન્ય *buffer, size_t size)
	{
		અચલ uint8_t *bytes = static_cast<અચલ uint8_t *>(buffer);

		જ્યારે (size > 0) {
			અચલ ssize_t written = લિખ(fd, bytes, size);

			જો (written <= 0) {
				પાછા ખોટું;
			}

			bytes += written;
			size -= written;
		}

		પાછા સાચું;
	}

};

static_assert(કદ(UasIdRecord) <= 256, "OpenDroneID UAS ID record must fit in /fs/mtd_id");

// The UAS ID storage is only supported on PX4 targets with NuttX and an MTD partition named "ID".
// On other platforms, the get() method will return StorageUnavailable and the checkOrStore() method will return Unsupported.
BidLockInfo BidStorageBackend::get()
{
#જો defined(__PX4_NUTTX)
	અચલ અક્ષર *path = nullptr;

	જો (px4_mtd_query("MTD_ID", nullptr, &path) != 0 || path == nullptr) {
		પાછા {BidLockState::StorageUnavailable, {}};
	}

	પૂર્ણાંક fd = open(path, O_RDONLY);

	જો (fd < 0) {
		પાછા {BidLockState::StorageUnavailable, {}};
	}

	UasIdRecord record{};

	જો (પઠ(fd, &record, કદ(record)) != કદ(record)) {
		close(fd);
		પાછા {BidLockState::StorageUnavailable, {}};
	}

	close(fd);

	જો (recordValid(record)) {
		BidLockInfo info{BidLockState::Locked, {}};
		info.id.ua_type = record.ua_type;
		memcpy(info.id.uas_id, record.uas_id, UAS_ID_SIZE);
		પાછા info;
	}

	જો (recordBlank(record)) {
		પાછા {BidLockState::Empty, {}};
	}

	પાછા {BidLockState::InvalidRecord, {}};
#નહીંતર
	પાછા {BidLockState::Unsupported, {}};
#endif
}

// Check if the given UAS ID matches the stored one, or store it if the storage is empty.
// The storage is write-once, so once a UAS ID is stored it cannot be changed (except by erasing the flash).
BidStoreResult BidStorageBackend::checkOrStore(uint8_t ua_type, અચલ uint8_t uas_id[UAS_ID_SIZE])
{
	જો (uasIdIsEmpty(uas_id)) {
		પાછા BidStoreResult::InvalidBasicId;
	}

#જો defined(__PX4_NUTTX)
	અચલ અક્ષર *path = nullptr;

	જો (px4_mtd_query("MTD_ID", nullptr, &path) != 0 || path == nullptr) {
		પાછા BidStoreResult::StorageUnavailable;
	}

	પૂર્ણાંક fd = open(path, O_RDWR);

	જો (fd < 0) {
		પાછા BidStoreResult::StorageUnavailable;
	}

	UasIdRecord existing{};

	જો (પઠ(fd, &existing, કદ(existing)) != કદ(existing)) {
		close(fd);
		પાછા BidStoreResult::StorageUnavailable;
	}

	જો (recordValid(existing)) {
		અચલ BidStoreResult result = recordMatches(existing, uas_id)
					      ? BidStoreResult::AlreadyLocked
					      : BidStoreResult::AlreadyLockedDifferent;
		close(fd);
		પાછા result;
	}

	જો (!recordBlank(existing)) {
		close(fd);
		પાછા BidStoreResult::InvalidRecord;
	}

	UasIdRecord record{};
	record.magic = ODID_UASID_MAGIC;
	record.version = ODID_UASID_RECORD_VERSION;
	record.id_type = UAS_ID_TYPE_SERIAL_સંખ્યા;
	record.ua_type = ua_type;
	memcpy(record.uas_id, uas_id, UAS_ID_SIZE);
	record.crc = recordCrc(record);

	અચલ uint32_t magic = record.magic;
	record.magic = 0xffffffff;

	અચલ તાર્કિક written = lseek(fd, કદ(record.magic), SEEK_SET) == static_cast<off_t>(કદ(record.magic))
			     && writeAll(fd, reinterpret_cast<અચલ uint8_t *>(&record) + કદ(record.magic),
					 કદ(record) - કદ(record.magic))
			     && fsync(fd) == 0
			     && lseek(fd, 0, SEEK_SET) == 0
			     && writeAll(fd, &magic, કદ(magic))
			     && fsync(fd) == 0;

	close(fd);

	પાછા written ? BidStoreResult::Stored : BidStoreResult::WriteError;
#નહીંતર
	પાછા BidStoreResult::Unsupported;
#endif
}

} // namespace

// Public interface that uses the backend implementation.
BidLockInfo BidStorage::get()
{
	BidStorageBackend storage;
	પાછા storage.get();
}

BidStoreResult BidStorage::checkOrStore(uint8_t ua_type, અચલ uint8_t uas_id[UAS_ID_SIZE])
{
	BidStorageBackend storage;
	પાછા storage.checkOrStore(ua_type, uas_id);
}

//
BidStoreResult BidLock::resolve(uint8_t id_type, uint8_t ua_type, અચલ uint8_t uas_id[UAS_ID_SIZE],
				UasId &locked_id)
{
	જો (id_type != UAS_ID_TYPE_SERIAL_સંખ્યા) {
		પાછા BidStoreResult::InvalidBasicId;
	}

	જો (!_loaded) {
		_info = BidStorage::get();
		_loaded = સાચું;
	}

	જો (_info.locked()) {
		locked_id = _info.id;
		પાછા BidStoreResult::AlreadyLocked;
	}

	જો (_info.state != BidLockState::Empty) {
		પસંદગી (_info.state) {
		સ્થિતિ BidLockState::InvalidRecord:
			પાછા BidStoreResult::InvalidRecord;

		સ્થિતિ BidLockState::StorageUnavailable:
			પાછા BidStoreResult::StorageUnavailable;

		સ્થિતિ BidLockState::WriteError:
			પાછા BidStoreResult::WriteError;

		સ્થિતિ BidLockState::Unsupported:
			પાછા BidStoreResult::Unsupported;

		સ્થિતિ BidLockState::Empty:
		સ્થિતિ BidLockState::Locked:
			તોડો;
		}
	}

	અચલ BidStoreResult result = BidStorage::checkOrStore(ua_type, uas_id);

	પસંદગી (result) {
	સ્થિતિ BidStoreResult::Stored:
		_info.state = BidLockState::Locked;
		_info.id.ua_type = ua_type;
		memcpy(_info.id.uas_id, uas_id, UAS_ID_SIZE);
		locked_id = _info.id;
		પાછા result;

	સ્થિતિ BidStoreResult::AlreadyLocked:
	સ્થિતિ BidStoreResult::AlreadyLockedDifferent: {
			_info = BidStorage::get();

			જો (_info.locked()) {
				locked_id = _info.id;
				પાછા BidStoreResult::AlreadyLocked;
			}

			પાછા BidStoreResult::InvalidRecord;
		}

	સ્થિતિ BidStoreResult::InvalidBasicId:
	સ્થિતિ BidStoreResult::StorageUnavailable:
	સ્થિતિ BidStoreResult::InvalidRecord:
	સ્થિતિ BidStoreResult::WriteError:
	સ્થિતિ BidStoreResult::Unsupported:
		પાછા result;
	}

	પાછા BidStoreResult::InvalidRecord;
}

} // namespace open_drone_id
