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

#که_تعریف_شوی __PX4_NUTTX
#ضمن <nuttx/crc32.h>
#ضمن <px4_platform_common/px4_manifest.h>
#وإلا
#ضمن <crc32.h>
#د_که_پای

نطاق open_drone_id
{
نطاق
{

ساكن constexpr uint32_t ODID_UASID_MAGIC = 0x4449444f; // "ODID"
ساكن constexpr uint8_t ODID_UASID_ریکارډ_نسخه = 1;

بنية __attribute__((packed)) Uas_Id_ریکارډ {
	uint32_t magic;
	uint8_t نسخه;
	uint8_t id_type;
	uint8_t ua_type;
	uint8_t reserved;
	uint8_t uas_id[UAS_ID_اندازه];
	uint32_t دوراني_ازموینه;
};

صنف BidStorageBackend
{
عام:
	Bid_قلف_Info get();
	Bid_Store_پایله checkOrStore(uint8_t ua_type, ثابت uint8_t uas_id[UAS_ID_اندازه]);

خاص:
	ساكن منطقي uas_Id_Is_تشه(ثابت uint8_t uas_id[UAS_ID_اندازه])
	{
		لكل (بینښه i = 0; i < UAS_ID_اندازه; ++i) {
			إذا (uas_id[i] != 0) {
				أرجع خطأ;
			}
		}

		أرجع ریښتیا;
	}

	ساكن uint32_t ریکارډ_دوراني_ازموینه(ثابت Uas_Id_ریکارډ &ریکارډ)
	{
		أرجع دوراني_ازموینه_32(reinterpret_cast<ثابت uint8_t *>(&ریکارډ.نسخه),
			     offsetof(Uas_Id_ریکارډ, دوراني_ازموینه) - offsetof(Uas_Id_ریکارډ, نسخه));
	}

	ساكن منطقي ریکارډ_Valid(ثابت Uas_Id_ریکارډ &ریکارډ)
	{
		أرجع ریکارډ.magic == ODID_UASID_MAGIC
		       && ریکارډ.نسخه == ODID_UASID_ریکارډ_نسخه
		       && ریکارډ.id_type == UAS_ID_TYPE_SERIAL_شمېره
		       && !uas_Id_Is_تشه(ریکارډ.uas_id)
		       && ریکارډ.دوراني_ازموینه == ریکارډ_دوراني_ازموینه(ریکارډ);
	}

	// Check if the record is all 0xff, which is the erased state of flash.
	// This is needed to distinguish between an unwritten record and a record that was written with all bytes set to 0 (which would be invalid but not unwritten).
	ساكن منطقي ریکارډ_Blank(ثابت Uas_Id_ریکارډ &ریکارډ)
	{
		ثابت uint8_t *بایټونه = reinterpret_cast<ثابت uint8_t *>(&ریکارډ);

		لكل (بینښه i = 0; i < حجم(ریکارډ); ++i) {
			إذا (بایټونه[i] != 0xff) {
				أرجع خطأ;
			}
		}

		أرجع ریښتیا;
	}

	ساكن منطقي ریکارډ_Matches(ثابت Uas_Id_ریکارډ &ریکارډ, ثابت uint8_t uas_id[UAS_ID_اندازه])
	{
		أرجع memcmp(ریکارډ.uas_id, uas_id, UAS_ID_اندازه) == 0;
	}

	// Write the entire buffer to the file descriptor, handling short writes.
	ساكن منطقي writeAll(صحيح fd, ثابت فراغ *بفر_2, size_t اندازه)
	{
		ثابت uint8_t *بایټونه = static_cast<ثابت uint8_t *>(بفر_2);

		طالما (اندازه > 0) {
			ثابت ssize_t written = اكتب(fd, بایټونه, اندازه);

			إذا (written <= 0) {
				أرجع خطأ;
			}

			بایټونه += written;
			اندازه -= written;
		}

		أرجع ریښتیا;
	}

};

static_assert(حجم(Uas_Id_ریکارډ) <= 256, "OpenDroneID UAS ID record must fit in /fs/mtd_id");

// The UAS ID storage is only supported on PX4 targets with NuttX and an MTD partition named "ID".
// On other platforms, the get() method will return StorageUnavailable and the checkOrStore() method will return Unsupported.
Bid_قلف_Info BidStorageBackend::get()
{
#إذا defined(__PX4_NUTTX)
	ثابت حرف *لار = تش_اشاره;

	إذا (px4_mtd_query("MTD_ID", تش_اشاره, &لار) != 0 || لار == تش_اشاره) {
		أرجع {Bid_قلف_State::StorageUnavailable, {}};
	}

	صحيح fd = open(لار, O_RDONLY);

	إذا (fd < 0) {
		أرجع {Bid_قلف_State::StorageUnavailable, {}};
	}

	Uas_Id_ریکارډ ریکارډ{};

	إذا (اقرأ(fd, &ریکارډ, حجم(ریکارډ)) != حجم(ریکارډ)) {
		close(fd);
		أرجع {Bid_قلف_State::StorageUnavailable, {}};
	}

	close(fd);

	إذا (ریکارډ_Valid(ریکارډ)) {
		Bid_قلف_Info info{Bid_قلف_State::Locked, {}};
		info.id.ua_type = ریکارډ.ua_type;
		حافظه_نقل(info.id.uas_id, ریکارډ.uas_id, UAS_ID_اندازه);
		أرجع info;
	}

	إذا (ریکارډ_Blank(ریکارډ)) {
		أرجع {Bid_قلف_State::تشه_2, {}};
	}

	أرجع {Bid_قلف_State::Invalid_ریکارډ, {}};
#وإلا
	أرجع {Bid_قلف_State::Unsupported, {}};
#د_که_پای
}

// Check if the given UAS ID matches the stored one, or store it if the storage is empty.
// The storage is write-once, so once a UAS ID is stored it cannot be changed (except by erasing the flash).
Bid_Store_پایله BidStorageBackend::checkOrStore(uint8_t ua_type, ثابت uint8_t uas_id[UAS_ID_اندازه])
{
	إذا (uas_Id_Is_تشه(uas_id)) {
		أرجع Bid_Store_پایله::InvalidBasicId;
	}

#إذا defined(__PX4_NUTTX)
	ثابت حرف *لار = تش_اشاره;

	إذا (px4_mtd_query("MTD_ID", تش_اشاره, &لار) != 0 || لار == تش_اشاره) {
		أرجع Bid_Store_پایله::StorageUnavailable;
	}

	صحيح fd = open(لار, O_RDWR);

	إذا (fd < 0) {
		أرجع Bid_Store_پایله::StorageUnavailable;
	}

	Uas_Id_ریکارډ existing{};

	إذا (اقرأ(fd, &existing, حجم(existing)) != حجم(existing)) {
		close(fd);
		أرجع Bid_Store_پایله::StorageUnavailable;
	}

	إذا (ریکارډ_Valid(existing)) {
		ثابت Bid_Store_پایله پایله = ریکارډ_Matches(existing, uas_id)
					      ? Bid_Store_پایله::AlreadyLocked
					      : Bid_Store_پایله::AlreadyLockedDifferent;
		close(fd);
		أرجع پایله;
	}

	إذا (!ریکارډ_Blank(existing)) {
		close(fd);
		أرجع Bid_Store_پایله::Invalid_ریکارډ;
	}

	Uas_Id_ریکارډ ریکارډ{};
	ریکارډ.magic = ODID_UASID_MAGIC;
	ریکارډ.نسخه = ODID_UASID_ریکارډ_نسخه;
	ریکارډ.id_type = UAS_ID_TYPE_SERIAL_شمېره;
	ریکارډ.ua_type = ua_type;
	حافظه_نقل(ریکارډ.uas_id, uas_id, UAS_ID_اندازه);
	ریکارډ.دوراني_ازموینه = ریکارډ_دوراني_ازموینه(ریکارډ);

	ثابت uint32_t magic = ریکارډ.magic;
	ریکارډ.magic = 0xffffffff;

	ثابت منطقي written = lseek(fd, حجم(ریکارډ.magic), SEEK_SET) == static_cast<off_t>(حجم(ریکارډ.magic))
			     && writeAll(fd, reinterpret_cast<ثابت uint8_t *>(&ریکارډ) + حجم(ریکارډ.magic),
					 حجم(ریکارډ) - حجم(ریکارډ.magic))
			     && fsync(fd) == 0
			     && lseek(fd, 0, SEEK_SET) == 0
			     && writeAll(fd, &magic, حجم(magic))
			     && fsync(fd) == 0;

	close(fd);

	أرجع written ? Bid_Store_پایله::Stored : Bid_Store_پایله::WriteError;
#وإلا
	أرجع Bid_Store_پایله::Unsupported;
#د_که_پای
}

} // namespace

// Public interface that uses the backend implementation.
Bid_قلف_Info BidStorage::get()
{
	BidStorageBackend storage;
	أرجع storage.get();
}

Bid_Store_پایله BidStorage::checkOrStore(uint8_t ua_type, ثابت uint8_t uas_id[UAS_ID_اندازه])
{
	BidStorageBackend storage;
	أرجع storage.checkOrStore(ua_type, uas_id);
}

//
Bid_Store_پایله Bid_قلف::resolve(uint8_t id_type, uint8_t ua_type, ثابت uint8_t uas_id[UAS_ID_اندازه],
				UasId &locked_id)
{
	إذا (id_type != UAS_ID_TYPE_SERIAL_شمېره) {
		أرجع Bid_Store_پایله::InvalidBasicId;
	}

	إذا (!_loaded) {
		_info = BidStorage::get();
		_loaded = ریښتیا;
	}

	إذا (_info.locked()) {
		locked_id = _info.id;
		أرجع Bid_Store_پایله::AlreadyLocked;
	}

	إذا (_info.state != Bid_قلف_State::تشه_2) {
		بدل (_info.state) {
		حالة Bid_قلف_State::Invalid_ریکارډ:
			أرجع Bid_Store_پایله::Invalid_ریکارډ;

		حالة Bid_قلف_State::StorageUnavailable:
			أرجع Bid_Store_پایله::StorageUnavailable;

		حالة Bid_قلف_State::WriteError:
			أرجع Bid_Store_پایله::WriteError;

		حالة Bid_قلف_State::Unsupported:
			أرجع Bid_Store_پایله::Unsupported;

		حالة Bid_قلف_State::تشه_2:
		حالة Bid_قلف_State::Locked:
			اكسر;
		}
	}

	ثابت Bid_Store_پایله پایله = BidStorage::checkOrStore(ua_type, uas_id);

	بدل (پایله) {
	حالة Bid_Store_پایله::Stored:
		_info.state = Bid_قلف_State::Locked;
		_info.id.ua_type = ua_type;
		حافظه_نقل(_info.id.uas_id, uas_id, UAS_ID_اندازه);
		locked_id = _info.id;
		أرجع پایله;

	حالة Bid_Store_پایله::AlreadyLocked:
	حالة Bid_Store_پایله::AlreadyLockedDifferent: {
			_info = BidStorage::get();

			إذا (_info.locked()) {
				locked_id = _info.id;
				أرجع Bid_Store_پایله::AlreadyLocked;
			}

			أرجع Bid_Store_پایله::Invalid_ریکارډ;
		}

	حالة Bid_Store_پایله::InvalidBasicId:
	حالة Bid_Store_پایله::StorageUnavailable:
	حالة Bid_Store_پایله::Invalid_ریکارډ:
	حالة Bid_Store_پایله::WriteError:
	حالة Bid_Store_پایله::Unsupported:
		أرجع پایله;
	}

	أرجع Bid_Store_پایله::Invalid_ریکارډ;
}

} // namespace open_drone_id
