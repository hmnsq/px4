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

#اگر_تعریف_شده __PX4_NUTTX
#ضمن <nuttx/crc32.h>
#ضمن <px4_platform_common/px4_manifest.h>
#وإلا
#ضمن <crc32.h>
#پایان_اگر

نطاق open_drone_id
{
نطاق
{

ساكن constexpr uint32_t ODID_UASID_MAGIC = 0x4449444f; // "ODID"
ساكن constexpr uint8_t ODID_UASID_رکورد_نسخه = 1;

بنية __attribute__((packed)) Uas_Id_رکورد {
	uint32_t magic;
	uint8_t نسخه;
	uint8_t id_type;
	uint8_t ua_type;
	uint8_t reserved;
	uint8_t uas_id[UAS_ID_اندازه];
	uint32_t بررسی_چرخهای;
};

صنف BidStorageBackend
{
عام:
	Bid_قفل_Info get();
	Bid_Store_نتیجه checkOrStore(uint8_t ua_type, ثابت uint8_t uas_id[UAS_ID_اندازه]);

خاص:
	ساكن منطقي uas_Id_Is_خالی(ثابت uint8_t uas_id[UAS_ID_اندازه])
	{
		لكل (بیعلامت i = 0; i < UAS_ID_اندازه; ++i) {
			إذا (uas_id[i] != 0) {
				أرجع خطأ;
			}
		}

		أرجع درست;
	}

	ساكن uint32_t رکورد_بررسی_چرخهای(ثابت Uas_Id_رکورد &رکورد)
	{
		أرجع بررسی_چرخهای_32(reinterpret_cast<ثابت uint8_t *>(&رکورد.نسخه),
			     offsetof(Uas_Id_رکورد, بررسی_چرخهای) - offsetof(Uas_Id_رکورد, نسخه));
	}

	ساكن منطقي رکورد_Valid(ثابت Uas_Id_رکورد &رکورد)
	{
		أرجع رکورد.magic == ODID_UASID_MAGIC
		       && رکورد.نسخه == ODID_UASID_رکورد_نسخه
		       && رکورد.id_type == UAS_ID_TYPE_SERIAL_عدد
		       && !uas_Id_Is_خالی(رکورد.uas_id)
		       && رکورد.بررسی_چرخهای == رکورد_بررسی_چرخهای(رکورد);
	}

	// Check if the record is all 0xff, which is the erased state of flash.
	// This is needed to distinguish between an unwritten record and a record that was written with all bytes set to 0 (which would be invalid but not unwritten).
	ساكن منطقي رکورد_Blank(ثابت Uas_Id_رکورد &رکورد)
	{
		ثابت uint8_t *بایتها = reinterpret_cast<ثابت uint8_t *>(&رکورد);

		لكل (بیعلامت i = 0; i < حجم(رکورد); ++i) {
			إذا (بایتها[i] != 0xff) {
				أرجع خطأ;
			}
		}

		أرجع درست;
	}

	ساكن منطقي رکورد_Matches(ثابت Uas_Id_رکورد &رکورد, ثابت uint8_t uas_id[UAS_ID_اندازه])
	{
		أرجع memcmp(رکورد.uas_id, uas_id, UAS_ID_اندازه) == 0;
	}

	// Write the entire buffer to the file descriptor, handling short writes.
	ساكن منطقي writeAll(صحيح fd, ثابت فراغ *میانگیر_2, size_t اندازه)
	{
		ثابت uint8_t *بایتها = static_cast<ثابت uint8_t *>(میانگیر_2);

		طالما (اندازه > 0) {
			ثابت ssize_t written = اكتب(fd, بایتها, اندازه);

			إذا (written <= 0) {
				أرجع خطأ;
			}

			بایتها += written;
			اندازه -= written;
		}

		أرجع درست;
	}

};

static_assert(حجم(Uas_Id_رکورد) <= 256, "OpenDroneID UAS ID record must fit in /fs/mtd_id");

// The UAS ID storage is only supported on PX4 targets with NuttX and an MTD partition named "ID".
// On other platforms, the get() method will return StorageUnavailable and the checkOrStore() method will return Unsupported.
Bid_قفل_Info BidStorageBackend::get()
{
#إذا defined(__PX4_NUTTX)
	ثابت حرف *مسیر = اشاره_تهی;

	إذا (px4_mtd_query("MTD_ID", اشاره_تهی, &مسیر) != 0 || مسیر == اشاره_تهی) {
		أرجع {Bid_قفل_State::StorageUnavailable, {}};
	}

	صحيح fd = open(مسیر, O_RDONLY);

	إذا (fd < 0) {
		أرجع {Bid_قفل_State::StorageUnavailable, {}};
	}

	Uas_Id_رکورد رکورد{};

	إذا (اقرأ(fd, &رکورد, حجم(رکورد)) != حجم(رکورد)) {
		close(fd);
		أرجع {Bid_قفل_State::StorageUnavailable, {}};
	}

	close(fd);

	إذا (رکورد_Valid(رکورد)) {
		Bid_قفل_Info info{Bid_قفل_State::Locked, {}};
		info.id.ua_type = رکورد.ua_type;
		کپی_حافظه(info.id.uas_id, رکورد.uas_id, UAS_ID_اندازه);
		أرجع info;
	}

	إذا (رکورد_Blank(رکورد)) {
		أرجع {Bid_قفل_State::خالی_2, {}};
	}

	أرجع {Bid_قفل_State::Invalid_رکورد, {}};
#وإلا
	أرجع {Bid_قفل_State::Unsupported, {}};
#پایان_اگر
}

// Check if the given UAS ID matches the stored one, or store it if the storage is empty.
// The storage is write-once, so once a UAS ID is stored it cannot be changed (except by erasing the flash).
Bid_Store_نتیجه BidStorageBackend::checkOrStore(uint8_t ua_type, ثابت uint8_t uas_id[UAS_ID_اندازه])
{
	إذا (uas_Id_Is_خالی(uas_id)) {
		أرجع Bid_Store_نتیجه::InvalidBasicId;
	}

#إذا defined(__PX4_NUTTX)
	ثابت حرف *مسیر = اشاره_تهی;

	إذا (px4_mtd_query("MTD_ID", اشاره_تهی, &مسیر) != 0 || مسیر == اشاره_تهی) {
		أرجع Bid_Store_نتیجه::StorageUnavailable;
	}

	صحيح fd = open(مسیر, O_RDWR);

	إذا (fd < 0) {
		أرجع Bid_Store_نتیجه::StorageUnavailable;
	}

	Uas_Id_رکورد existing{};

	إذا (اقرأ(fd, &existing, حجم(existing)) != حجم(existing)) {
		close(fd);
		أرجع Bid_Store_نتیجه::StorageUnavailable;
	}

	إذا (رکورد_Valid(existing)) {
		ثابت Bid_Store_نتیجه نتیجه = رکورد_Matches(existing, uas_id)
					      ? Bid_Store_نتیجه::AlreadyLocked
					      : Bid_Store_نتیجه::AlreadyLockedDifferent;
		close(fd);
		أرجع نتیجه;
	}

	إذا (!رکورد_Blank(existing)) {
		close(fd);
		أرجع Bid_Store_نتیجه::Invalid_رکورد;
	}

	Uas_Id_رکورد رکورد{};
	رکورد.magic = ODID_UASID_MAGIC;
	رکورد.نسخه = ODID_UASID_رکورد_نسخه;
	رکورد.id_type = UAS_ID_TYPE_SERIAL_عدد;
	رکورد.ua_type = ua_type;
	کپی_حافظه(رکورد.uas_id, uas_id, UAS_ID_اندازه);
	رکورد.بررسی_چرخهای = رکورد_بررسی_چرخهای(رکورد);

	ثابت uint32_t magic = رکورد.magic;
	رکورد.magic = 0xffffffff;

	ثابت منطقي written = lseek(fd, حجم(رکورد.magic), SEEK_SET) == static_cast<off_t>(حجم(رکورد.magic))
			     && writeAll(fd, reinterpret_cast<ثابت uint8_t *>(&رکورد) + حجم(رکورد.magic),
					 حجم(رکورد) - حجم(رکورد.magic))
			     && fsync(fd) == 0
			     && lseek(fd, 0, SEEK_SET) == 0
			     && writeAll(fd, &magic, حجم(magic))
			     && fsync(fd) == 0;

	close(fd);

	أرجع written ? Bid_Store_نتیجه::Stored : Bid_Store_نتیجه::WriteError;
#وإلا
	أرجع Bid_Store_نتیجه::Unsupported;
#پایان_اگر
}

} // namespace

// Public interface that uses the backend implementation.
Bid_قفل_Info BidStorage::get()
{
	BidStorageBackend storage;
	أرجع storage.get();
}

Bid_Store_نتیجه BidStorage::checkOrStore(uint8_t ua_type, ثابت uint8_t uas_id[UAS_ID_اندازه])
{
	BidStorageBackend storage;
	أرجع storage.checkOrStore(ua_type, uas_id);
}

//
Bid_Store_نتیجه Bid_قفل::resolve(uint8_t id_type, uint8_t ua_type, ثابت uint8_t uas_id[UAS_ID_اندازه],
				UasId &locked_id)
{
	إذا (id_type != UAS_ID_TYPE_SERIAL_عدد) {
		أرجع Bid_Store_نتیجه::InvalidBasicId;
	}

	إذا (!_loaded) {
		_info = BidStorage::get();
		_loaded = درست;
	}

	إذا (_info.locked()) {
		locked_id = _info.id;
		أرجع Bid_Store_نتیجه::AlreadyLocked;
	}

	إذا (_info.state != Bid_قفل_State::خالی_2) {
		بدل (_info.state) {
		حالة Bid_قفل_State::Invalid_رکورد:
			أرجع Bid_Store_نتیجه::Invalid_رکورد;

		حالة Bid_قفل_State::StorageUnavailable:
			أرجع Bid_Store_نتیجه::StorageUnavailable;

		حالة Bid_قفل_State::WriteError:
			أرجع Bid_Store_نتیجه::WriteError;

		حالة Bid_قفل_State::Unsupported:
			أرجع Bid_Store_نتیجه::Unsupported;

		حالة Bid_قفل_State::خالی_2:
		حالة Bid_قفل_State::Locked:
			اكسر;
		}
	}

	ثابت Bid_Store_نتیجه نتیجه = BidStorage::checkOrStore(ua_type, uas_id);

	بدل (نتیجه) {
	حالة Bid_Store_نتیجه::Stored:
		_info.state = Bid_قفل_State::Locked;
		_info.id.ua_type = ua_type;
		کپی_حافظه(_info.id.uas_id, uas_id, UAS_ID_اندازه);
		locked_id = _info.id;
		أرجع نتیجه;

	حالة Bid_Store_نتیجه::AlreadyLocked:
	حالة Bid_Store_نتیجه::AlreadyLockedDifferent: {
			_info = BidStorage::get();

			إذا (_info.locked()) {
				locked_id = _info.id;
				أرجع Bid_Store_نتیجه::AlreadyLocked;
			}

			أرجع Bid_Store_نتیجه::Invalid_رکورد;
		}

	حالة Bid_Store_نتیجه::InvalidBasicId:
	حالة Bid_Store_نتیجه::StorageUnavailable:
	حالة Bid_Store_نتیجه::Invalid_رکورد:
	حالة Bid_Store_نتیجه::WriteError:
	حالة Bid_Store_نتیجه::Unsupported:
		أرجع نتیجه;
	}

	أرجع Bid_Store_نتیجه::Invalid_رکورد;
}

} // namespace open_drone_id
