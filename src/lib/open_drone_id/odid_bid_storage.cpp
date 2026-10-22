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

#यदि_परिभाषितम् __PX4_NUTTX
#अन्तर्भाव <nuttx/crc32.h>
#अन्तर्भाव <px4_platform_common/px4_manifest.h>
#अन्यथा
#अन्तर्भाव <crc32.h>
#यदि_अन्तः

नामाकाश open_drone_id
{
नामाकाश
{

स्थिर constexpr uint32_t ODID_UASID_MAGIC = 0x4449444f; // "ODID"
स्थिर constexpr uint8_t ODID_UASID_लेखः_संस्करणम् = 1;

संरचना __attribute__((packed)) Uas_Id_लेखः {
	uint32_t magic;
	uint8_t संस्करणम्;
	uint8_t id_type;
	uint8_t ua_type;
	uint8_t reserved;
	uint8_t uas_id[UAS_ID_आकार];
	uint32_t crc;
};

वर्ग BidStorageBackend
{
सार्वजनिक:
	Bid_तालकम्_Info get();
	Bid_Store_परिणाम checkOrStore(uint8_t ua_type, ध्रुव uint8_t uas_id[UAS_ID_आकार]);

गुप्त:
	स्थिर तार्किक uas_Id_Is_रिक्तानि(ध्रुव uint8_t uas_id[UAS_ID_आकार])
	{
		प्रति (अचिह्नित i = 0; i < UAS_ID_आकार; ++i) {
			यदि (uas_id[i] != 0) {
				प्रत्यागम असत्य;
			}
		}

		प्रत्यागम सत्य;
	}

	स्थिर uint32_t लेखः_Crc(ध्रुव Uas_Id_लेखः &लेखः)
	{
		प्रत्यागम चक्रीयपरीक्षा_32(reinterpret_cast<ध्रुव uint8_t *>(&लेखः.संस्करणम्),
			     offsetof(Uas_Id_लेखः, crc) - offsetof(Uas_Id_लेखः, संस्करणम्));
	}

	स्थिर तार्किक लेखः_Valid(ध्रुव Uas_Id_लेखः &लेखः)
	{
		प्रत्यागम लेखः.magic == ODID_UASID_MAGIC
		       && लेखः.संस्करणम् == ODID_UASID_लेखः_संस्करणम्
		       && लेखः.id_type == UAS_ID_TYPE_SERIAL_सङ्ख्या
		       && !uas_Id_Is_रिक्तानि(लेखः.uas_id)
		       && लेखः.crc == लेखः_Crc(लेखः);
	}

	// Check if the record is all 0xff, which is the erased state of flash.
	// This is needed to distinguish between an unwritten record and a record that was written with all bytes set to 0 (which would be invalid but not unwritten).
	स्थिर तार्किक लेखः_Blank(ध्रुव Uas_Id_लेखः &लेखः)
	{
		ध्रुव uint8_t *अष्टकानि = reinterpret_cast<ध्रुव uint8_t *>(&लेखः);

		प्रति (अचिह्नित i = 0; i < परिमाण(लेखः); ++i) {
			यदि (अष्टकानि[i] != 0xff) {
				प्रत्यागम असत्य;
			}
		}

		प्रत्यागम सत्य;
	}

	स्थिर तार्किक लेखः_Matches(ध्रुव Uas_Id_लेखः &लेखः, ध्रुव uint8_t uas_id[UAS_ID_आकार])
	{
		प्रत्यागम memcmp(लेखः.uas_id, uas_id, UAS_ID_आकार) == 0;
	}

	// Write the entire buffer to the file descriptor, handling short writes.
	स्थिर तार्किक writeAll(पूर्णाङ्क fd, ध्रुव शून्य *बफर_2, size_t आकार)
	{
		ध्रुव uint8_t *अष्टकानि = static_cast<ध्रुव uint8_t *>(बफर_2);

		यावत् (आकार > 0) {
			ध्रुव ssize_t written = लिख(fd, अष्टकानि, आकार);

			यदि (written <= 0) {
				प्रत्यागम असत्य;
			}

			अष्टकानि += written;
			आकार -= written;
		}

		प्रत्यागम सत्य;
	}

};

static_assert(परिमाण(Uas_Id_लेखः) <= 256, "OpenDroneID UAS ID record must fit in /fs/mtd_id");

// The UAS ID storage is only supported on PX4 targets with NuttX and an MTD partition named "ID".
// On other platforms, the get() method will return StorageUnavailable and the checkOrStore() method will return Unsupported.
Bid_तालकम्_Info BidStorageBackend::get()
{
#यदि defined(__PX4_NUTTX)
	ध्रुव वर्ण *मार्गः = शून्यसूचकः;

	यदि (px4_mtd_query("MTD_ID", शून्यसूचकः, &मार्गः) != 0 || मार्गः == शून्यसूचकः) {
		प्रत्यागम {Bid_तालकम्_State::StorageUnavailable, {}};
	}

	पूर्णाङ्क fd = उद्घाटय(मार्गः, O_RDONLY);

	यदि (fd < 0) {
		प्रत्यागम {Bid_तालकम्_State::StorageUnavailable, {}};
	}

	Uas_Id_लेखः लेखः{};

	यदि (पठ(fd, &लेखः, परिमाण(लेखः)) != परिमाण(लेखः)) {
		close(fd);
		प्रत्यागम {Bid_तालकम्_State::StorageUnavailable, {}};
	}

	close(fd);

	यदि (लेखः_Valid(लेखः)) {
		Bid_तालकम्_Info info{Bid_तालकम्_State::Locked, {}};
		info.id.ua_type = लेखः.ua_type;
		स्मृतिं_प्रतिलिख(info.id.uas_id, लेखः.uas_id, UAS_ID_आकार);
		प्रत्यागम info;
	}

	यदि (लेखः_Blank(लेखः)) {
		प्रत्यागम {Bid_तालकम्_State::रिक्तानि_2, {}};
	}

	प्रत्यागम {Bid_तालकम्_State::Invalid_लेखः, {}};
#अन्यथा
	प्रत्यागम {Bid_तालकम्_State::Unsupported, {}};
#यदि_अन्तः
}

// Check if the given UAS ID matches the stored one, or store it if the storage is empty.
// The storage is write-once, so once a UAS ID is stored it cannot be changed (except by erasing the flash).
Bid_Store_परिणाम BidStorageBackend::checkOrStore(uint8_t ua_type, ध्रुव uint8_t uas_id[UAS_ID_आकार])
{
	यदि (uas_Id_Is_रिक्तानि(uas_id)) {
		प्रत्यागम Bid_Store_परिणाम::InvalidBasicId;
	}

#यदि defined(__PX4_NUTTX)
	ध्रुव वर्ण *मार्गः = शून्यसूचकः;

	यदि (px4_mtd_query("MTD_ID", शून्यसूचकः, &मार्गः) != 0 || मार्गः == शून्यसूचकः) {
		प्रत्यागम Bid_Store_परिणाम::StorageUnavailable;
	}

	पूर्णाङ्क fd = उद्घाटय(मार्गः, O_RDWR);

	यदि (fd < 0) {
		प्रत्यागम Bid_Store_परिणाम::StorageUnavailable;
	}

	Uas_Id_लेखः existing{};

	यदि (पठ(fd, &existing, परिमाण(existing)) != परिमाण(existing)) {
		close(fd);
		प्रत्यागम Bid_Store_परिणाम::StorageUnavailable;
	}

	यदि (लेखः_Valid(existing)) {
		ध्रुव Bid_Store_परिणाम परिणाम = लेखः_Matches(existing, uas_id)
					      ? Bid_Store_परिणाम::AlreadyLocked
					      : Bid_Store_परिणाम::AlreadyLockedDifferent;
		close(fd);
		प्रत्यागम परिणाम;
	}

	यदि (!लेखः_Blank(existing)) {
		close(fd);
		प्रत्यागम Bid_Store_परिणाम::Invalid_लेखः;
	}

	Uas_Id_लेखः लेखः{};
	लेखः.magic = ODID_UASID_MAGIC;
	लेखः.संस्करणम् = ODID_UASID_लेखः_संस्करणम्;
	लेखः.id_type = UAS_ID_TYPE_SERIAL_सङ्ख्या;
	लेखः.ua_type = ua_type;
	स्मृतिं_प्रतिलिख(लेखः.uas_id, uas_id, UAS_ID_आकार);
	लेखः.crc = लेखः_Crc(लेखः);

	ध्रुव uint32_t magic = लेखः.magic;
	लेखः.magic = 0xffffffff;

	ध्रुव तार्किक written = lseek(fd, परिमाण(लेखः.magic), SEEK_SET) == static_cast<off_t>(परिमाण(लेखः.magic))
			     && writeAll(fd, reinterpret_cast<ध्रुव uint8_t *>(&लेखः) + परिमाण(लेखः.magic),
					 परिमाण(लेखः) - परिमाण(लेखः.magic))
			     && fsync(fd) == 0
			     && lseek(fd, 0, SEEK_SET) == 0
			     && writeAll(fd, &magic, परिमाण(magic))
			     && fsync(fd) == 0;

	close(fd);

	प्रत्यागम written ? Bid_Store_परिणाम::Stored : Bid_Store_परिणाम::WriteError;
#अन्यथा
	प्रत्यागम Bid_Store_परिणाम::Unsupported;
#यदि_अन्तः
}

} // namespace

// Public interface that uses the backend implementation.
Bid_तालकम्_Info BidStorage::get()
{
	BidStorageBackend storage;
	प्रत्यागम storage.get();
}

Bid_Store_परिणाम BidStorage::checkOrStore(uint8_t ua_type, ध्रुव uint8_t uas_id[UAS_ID_आकार])
{
	BidStorageBackend storage;
	प्रत्यागम storage.checkOrStore(ua_type, uas_id);
}

//
Bid_Store_परिणाम Bid_तालकम्::resolve(uint8_t id_type, uint8_t ua_type, ध्रुव uint8_t uas_id[UAS_ID_आकार],
				UasId &locked_id)
{
	यदि (id_type != UAS_ID_TYPE_SERIAL_सङ्ख्या) {
		प्रत्यागम Bid_Store_परिणाम::InvalidBasicId;
	}

	यदि (!_loaded) {
		_info = BidStorage::get();
		_loaded = सत्य;
	}

	यदि (_info.locked()) {
		locked_id = _info.id;
		प्रत्यागम Bid_Store_परिणाम::AlreadyLocked;
	}

	यदि (_info.state != Bid_तालकम्_State::रिक्तानि_2) {
		विकल्प (_info.state) {
		पक्ष Bid_तालकम्_State::Invalid_लेखः:
			प्रत्यागम Bid_Store_परिणाम::Invalid_लेखः;

		पक्ष Bid_तालकम्_State::StorageUnavailable:
			प्रत्यागम Bid_Store_परिणाम::StorageUnavailable;

		पक्ष Bid_तालकम्_State::WriteError:
			प्रत्यागम Bid_Store_परिणाम::WriteError;

		पक्ष Bid_तालकम्_State::Unsupported:
			प्रत्यागम Bid_Store_परिणाम::Unsupported;

		पक्ष Bid_तालकम्_State::रिक्तानि_2:
		पक्ष Bid_तालकम्_State::Locked:
			भङ्ग;
		}
	}

	ध्रुव Bid_Store_परिणाम परिणाम = BidStorage::checkOrStore(ua_type, uas_id);

	विकल्प (परिणाम) {
	पक्ष Bid_Store_परिणाम::Stored:
		_info.state = Bid_तालकम्_State::Locked;
		_info.id.ua_type = ua_type;
		स्मृतिं_प्रतिलिख(_info.id.uas_id, uas_id, UAS_ID_आकार);
		locked_id = _info.id;
		प्रत्यागम परिणाम;

	पक्ष Bid_Store_परिणाम::AlreadyLocked:
	पक्ष Bid_Store_परिणाम::AlreadyLockedDifferent: {
			_info = BidStorage::get();

			यदि (_info.locked()) {
				locked_id = _info.id;
				प्रत्यागम Bid_Store_परिणाम::AlreadyLocked;
			}

			प्रत्यागम Bid_Store_परिणाम::Invalid_लेखः;
		}

	पक्ष Bid_Store_परिणाम::InvalidBasicId:
	पक्ष Bid_Store_परिणाम::StorageUnavailable:
	पक्ष Bid_Store_परिणाम::Invalid_लेखः:
	पक्ष Bid_Store_परिणाम::WriteError:
	पक्ष Bid_Store_परिणाम::Unsupported:
		प्रत्यागम परिणाम;
	}

	प्रत्यागम Bid_Store_परिणाम::Invalid_लेखः;
}

} // namespace open_drone_id
