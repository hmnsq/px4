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

#ଅନ୍ତର୍ଭାଵ "odid_bid_storage.hpp"

#ଅନ୍ତର୍ଭାଵ <fcntl.h>
#ଅନ୍ତର୍ଭାଵ <stddef.h>
#ଅନ୍ତର୍ଭାଵ <string.h>
#ଅନ୍ତର୍ଭାଵ <unistd.h>

#ifdef __PX4_NUTTX
#ଅନ୍ତର୍ଭାଵ <nuttx/crc32.h>
#ଅନ୍ତର୍ଭାଵ <px4_platform_common/px4_manifest.h>
#ଅନ୍ୟଥା
#ଅନ୍ତର୍ଭାଵ <crc32.h>
#endif

ନାମାକାଶ open_drone_id
{
ନାମାକାଶ
{

ସ୍ଥିର constexpr uint32_t ODID_UASID_MAGIC = 0x4449444f; // "ODID"
ସ୍ଥିର constexpr uint8_t ODID_UASID_RECORD_VERSION = 1;

ସଂରଚନା __attribute__((packed)) UasIdRecord {
	uint32_t magic;
	uint8_t version;
	uint8_t id_type;
	uint8_t ua_type;
	uint8_t reserved;
	uint8_t uas_id[UAS_ID_SIZE];
	uint32_t crc;
};

ଶ୍ରେଣୀ BidStorageBackend
{
ସର୍ବସାଧାରଣ:
	BidLockInfo get();
	BidStoreResult checkOrStore(uint8_t ua_type, ଅଚଳ uint8_t uas_id[UAS_ID_SIZE]);

ବ୍ୟକ୍ତିଗତ:
	ସ୍ଥିର ତାର୍କିକ uasIdIsEmpty(ଅଚଳ uint8_t uas_id[UAS_ID_SIZE])
	{
		ପାଇଁ (unsigned i = 0; i < UAS_ID_SIZE; ++i) {
			ଯଦି (uas_id[i] != 0) {
				ଫେରନ୍ତୁ ମିଥ୍ୟା;
			}
		}

		ଫେରନ୍ତୁ ସତ୍ୟ;
	}

	ସ୍ଥିର uint32_t recordCrc(ଅଚଳ UasIdRecord &record)
	{
		ଫେରନ୍ତୁ crc32(reinterpret_cast<ଅଚଳ uint8_t *>(&record.version),
			     offsetof(UasIdRecord, crc) - offsetof(UasIdRecord, version));
	}

	ସ୍ଥିର ତାର୍କିକ recordValid(ଅଚଳ UasIdRecord &record)
	{
		ଫେରନ୍ତୁ record.magic == ODID_UASID_MAGIC
		       && record.version == ODID_UASID_RECORD_VERSION
		       && record.id_type == UAS_ID_TYPE_SERIAL_ସଂଖ୍ୟା
		       && !uasIdIsEmpty(record.uas_id)
		       && record.crc == recordCrc(record);
	}

	// Check if the record is all 0xff, which is the erased state of flash.
	// This is needed to distinguish between an unwritten record and a record that was written with all bytes set to 0 (which would be invalid but not unwritten).
	ସ୍ଥିର ତାର୍କିକ recordBlank(ଅଚଳ UasIdRecord &record)
	{
		ଅଚଳ uint8_t *bytes = reinterpret_cast<ଅଚଳ uint8_t *>(&record);

		ପାଇଁ (unsigned i = 0; i < ଆକାର(record); ++i) {
			ଯଦି (bytes[i] != 0xff) {
				ଫେରନ୍ତୁ ମିଥ୍ୟା;
			}
		}

		ଫେରନ୍ତୁ ସତ୍ୟ;
	}

	ସ୍ଥିର ତାର୍କିକ recordMatches(ଅଚଳ UasIdRecord &record, ଅଚଳ uint8_t uas_id[UAS_ID_SIZE])
	{
		ଫେରନ୍ତୁ memcmp(record.uas_id, uas_id, UAS_ID_SIZE) == 0;
	}

	// Write the entire buffer to the file descriptor, handling short writes.
	ସ୍ଥିର ତାର୍କିକ writeAll(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଶୂନ୍ୟ *buffer, size_t size)
	{
		ଅଚଳ uint8_t *bytes = static_cast<ଅଚଳ uint8_t *>(buffer);

		ଯେତେବେଳେ (size > 0) {
			ଅଚଳ ssize_t written = ଲିଖ(fd, bytes, size);

			ଯଦି (written <= 0) {
				ଫେରନ୍ତୁ ମିଥ୍ୟା;
			}

			bytes += written;
			size -= written;
		}

		ଫେରନ୍ତୁ ସତ୍ୟ;
	}

};

static_assert(ଆକାର(UasIdRecord) <= 256, "OpenDroneID UAS ID record must fit in /fs/mtd_id");

// The UAS ID storage is only supported on PX4 targets with NuttX and an MTD partition named "ID".
// On other platforms, the get() method will return StorageUnavailable and the checkOrStore() method will return Unsupported.
BidLockInfo BidStorageBackend::get()
{
#ଯଦି defined(__PX4_NUTTX)
	ଅଚଳ ଅକ୍ଷର *path = nullptr;

	ଯଦି (px4_mtd_query("MTD_ID", nullptr, &path) != 0 || path == nullptr) {
		ଫେରନ୍ତୁ {BidLockState::StorageUnavailable, {}};
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = open(path, O_RDONLY);

	ଯଦି (fd < 0) {
		ଫେରନ୍ତୁ {BidLockState::StorageUnavailable, {}};
	}

	UasIdRecord record{};

	ଯଦି (ପଠ(fd, &record, ଆକାର(record)) != ଆକାର(record)) {
		close(fd);
		ଫେରନ୍ତୁ {BidLockState::StorageUnavailable, {}};
	}

	close(fd);

	ଯଦି (recordValid(record)) {
		BidLockInfo info{BidLockState::Locked, {}};
		info.id.ua_type = record.ua_type;
		memcpy(info.id.uas_id, record.uas_id, UAS_ID_SIZE);
		ଫେରନ୍ତୁ info;
	}

	ଯଦି (recordBlank(record)) {
		ଫେରନ୍ତୁ {BidLockState::Empty, {}};
	}

	ଫେରନ୍ତୁ {BidLockState::InvalidRecord, {}};
#ଅନ୍ୟଥା
	ଫେରନ୍ତୁ {BidLockState::Unsupported, {}};
#endif
}

// Check if the given UAS ID matches the stored one, or store it if the storage is empty.
// The storage is write-once, so once a UAS ID is stored it cannot be changed (except by erasing the flash).
BidStoreResult BidStorageBackend::checkOrStore(uint8_t ua_type, ଅଚଳ uint8_t uas_id[UAS_ID_SIZE])
{
	ଯଦି (uasIdIsEmpty(uas_id)) {
		ଫେରନ୍ତୁ BidStoreResult::InvalidBasicId;
	}

#ଯଦି defined(__PX4_NUTTX)
	ଅଚଳ ଅକ୍ଷର *path = nullptr;

	ଯଦି (px4_mtd_query("MTD_ID", nullptr, &path) != 0 || path == nullptr) {
		ଫେରନ୍ତୁ BidStoreResult::StorageUnavailable;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = open(path, O_RDWR);

	ଯଦି (fd < 0) {
		ଫେରନ୍ତୁ BidStoreResult::StorageUnavailable;
	}

	UasIdRecord existing{};

	ଯଦି (ପଠ(fd, &existing, ଆକାର(existing)) != ଆକାର(existing)) {
		close(fd);
		ଫେରନ୍ତୁ BidStoreResult::StorageUnavailable;
	}

	ଯଦି (recordValid(existing)) {
		ଅଚଳ BidStoreResult result = recordMatches(existing, uas_id)
					      ? BidStoreResult::AlreadyLocked
					      : BidStoreResult::AlreadyLockedDifferent;
		close(fd);
		ଫେରନ୍ତୁ result;
	}

	ଯଦି (!recordBlank(existing)) {
		close(fd);
		ଫେରନ୍ତୁ BidStoreResult::InvalidRecord;
	}

	UasIdRecord record{};
	record.magic = ODID_UASID_MAGIC;
	record.version = ODID_UASID_RECORD_VERSION;
	record.id_type = UAS_ID_TYPE_SERIAL_ସଂଖ୍ୟା;
	record.ua_type = ua_type;
	memcpy(record.uas_id, uas_id, UAS_ID_SIZE);
	record.crc = recordCrc(record);

	ଅଚଳ uint32_t magic = record.magic;
	record.magic = 0xffffffff;

	ଅଚଳ ତାର୍କିକ written = lseek(fd, ଆକାର(record.magic), SEEK_SET) == static_cast<off_t>(ଆକାର(record.magic))
			     && writeAll(fd, reinterpret_cast<ଅଚଳ uint8_t *>(&record) + ଆକାର(record.magic),
					 ଆକାର(record) - ଆକାର(record.magic))
			     && fsync(fd) == 0
			     && lseek(fd, 0, SEEK_SET) == 0
			     && writeAll(fd, &magic, ଆକାର(magic))
			     && fsync(fd) == 0;

	close(fd);

	ଫେରନ୍ତୁ written ? BidStoreResult::Stored : BidStoreResult::WriteError;
#ଅନ୍ୟଥା
	ଫେରନ୍ତୁ BidStoreResult::Unsupported;
#endif
}

} // namespace

// Public interface that uses the backend implementation.
BidLockInfo BidStorage::get()
{
	BidStorageBackend storage;
	ଫେରନ୍ତୁ storage.get();
}

BidStoreResult BidStorage::checkOrStore(uint8_t ua_type, ଅଚଳ uint8_t uas_id[UAS_ID_SIZE])
{
	BidStorageBackend storage;
	ଫେରନ୍ତୁ storage.checkOrStore(ua_type, uas_id);
}

//
BidStoreResult BidLock::resolve(uint8_t id_type, uint8_t ua_type, ଅଚଳ uint8_t uas_id[UAS_ID_SIZE],
				UasId &locked_id)
{
	ଯଦି (id_type != UAS_ID_TYPE_SERIAL_ସଂଖ୍ୟା) {
		ଫେରନ୍ତୁ BidStoreResult::InvalidBasicId;
	}

	ଯଦି (!_loaded) {
		_info = BidStorage::get();
		_loaded = ସତ୍ୟ;
	}

	ଯଦି (_info.locked()) {
		locked_id = _info.id;
		ଫେରନ୍ତୁ BidStoreResult::AlreadyLocked;
	}

	ଯଦି (_info.state != BidLockState::Empty) {
		ବାଛ (_info.state) {
		ସ୍ଥିତି BidLockState::InvalidRecord:
			ଫେରନ୍ତୁ BidStoreResult::InvalidRecord;

		ସ୍ଥିତି BidLockState::StorageUnavailable:
			ଫେରନ୍ତୁ BidStoreResult::StorageUnavailable;

		ସ୍ଥିତି BidLockState::WriteError:
			ଫେରନ୍ତୁ BidStoreResult::WriteError;

		ସ୍ଥିତି BidLockState::Unsupported:
			ଫେରନ୍ତୁ BidStoreResult::Unsupported;

		ସ୍ଥିତି BidLockState::Empty:
		ସ୍ଥିତି BidLockState::Locked:
			ଭାଙ୍ଗ;
		}
	}

	ଅଚଳ BidStoreResult result = BidStorage::checkOrStore(ua_type, uas_id);

	ବାଛ (result) {
	ସ୍ଥିତି BidStoreResult::Stored:
		_info.state = BidLockState::Locked;
		_info.id.ua_type = ua_type;
		memcpy(_info.id.uas_id, uas_id, UAS_ID_SIZE);
		locked_id = _info.id;
		ଫେରନ୍ତୁ result;

	ସ୍ଥିତି BidStoreResult::AlreadyLocked:
	ସ୍ଥିତି BidStoreResult::AlreadyLockedDifferent: {
			_info = BidStorage::get();

			ଯଦି (_info.locked()) {
				locked_id = _info.id;
				ଫେରନ୍ତୁ BidStoreResult::AlreadyLocked;
			}

			ଫେରନ୍ତୁ BidStoreResult::InvalidRecord;
		}

	ସ୍ଥିତି BidStoreResult::InvalidBasicId:
	ସ୍ଥିତି BidStoreResult::StorageUnavailable:
	ସ୍ଥିତି BidStoreResult::InvalidRecord:
	ସ୍ଥିତି BidStoreResult::WriteError:
	ସ୍ଥିତି BidStoreResult::Unsupported:
		ଫେରନ୍ତୁ result;
	}

	ଫେରନ୍ତୁ BidStoreResult::InvalidRecord;
}

} // namespace open_drone_id
