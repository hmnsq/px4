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

#جے_تعریف_شدہ __PX4_NUTTX
#ضمن <nuttx/crc32.h>
#ضمن <px4_platform_common/px4_manifest.h>
#وإلا
#ضمن <crc32.h>
#جے_انت

نطاق open_drone_id
{
نطاق
{

ساكن constexpr uint32_t ODID_UASID_MAGIC = 0x4449444f; // "ODID"
ساكن constexpr uint8_t ODID_UASID_ریکارڈ_نسخہ = 1;

بنية __attribute__((packed)) Uas_Id_ریکارڈ {
	uint32_t magic;
	uint8_t نسخہ;
	uint8_t id_type;
	uint8_t ua_type;
	uint8_t reserved;
	uint8_t uas_id[UAS_ID_ناپ];
	uint32_t چکری_جانچ;
};

صنف BidStorageBackend
{
عام:
	Bid_جندرا_Info get();
	Bid_Store_نتیجہ checkOrStore(uint8_t ua_type, ثابت uint8_t uas_id[UAS_ID_ناپ]);

خاص:
	ساكن منطقي uas_Id_Is_خالی_تھاواں(ثابت uint8_t uas_id[UAS_ID_ناپ])
	{
		لكل (بنانشانی i = 0; i < UAS_ID_ناپ; ++i) {
			إذا (uas_id[i] != 0) {
				أرجع خطأ;
			}
		}

		أرجع سچ;
	}

	ساكن uint32_t ریکارڈ_چکری_جانچ(ثابت Uas_Id_ریکارڈ &ریکارڈ)
	{
		أرجع چکری_جانچ_32(reinterpret_cast<ثابت uint8_t *>(&ریکارڈ.نسخہ),
			     offsetof(Uas_Id_ریکارڈ, چکری_جانچ) - offsetof(Uas_Id_ریکارڈ, نسخہ));
	}

	ساكن منطقي ریکارڈ_Valid(ثابت Uas_Id_ریکارڈ &ریکارڈ)
	{
		أرجع ریکارڈ.magic == ODID_UASID_MAGIC
		       && ریکارڈ.نسخہ == ODID_UASID_ریکارڈ_نسخہ
		       && ریکارڈ.id_type == UAS_ID_TYPE_SERIAL_نمبر
		       && !uas_Id_Is_خالی_تھاواں(ریکارڈ.uas_id)
		       && ریکارڈ.چکری_جانچ == ریکارڈ_چکری_جانچ(ریکارڈ);
	}

	// Check if the record is all 0xff, which is the erased state of flash.
	// This is needed to distinguish between an unwritten record and a record that was written with all bytes set to 0 (which would be invalid but not unwritten).
	ساكن منطقي ریکارڈ_Blank(ثابت Uas_Id_ریکارڈ &ریکارڈ)
	{
		ثابت uint8_t *بائٹاں = reinterpret_cast<ثابت uint8_t *>(&ریکارڈ);

		لكل (بنانشانی i = 0; i < حجم(ریکارڈ); ++i) {
			إذا (بائٹاں[i] != 0xff) {
				أرجع خطأ;
			}
		}

		أرجع سچ;
	}

	ساكن منطقي ریکارڈ_Matches(ثابت Uas_Id_ریکارڈ &ریکارڈ, ثابت uint8_t uas_id[UAS_ID_ناپ])
	{
		أرجع memcmp(ریکارڈ.uas_id, uas_id, UAS_ID_ناپ) == 0;
	}

	// Write the entire buffer to the file descriptor, handling short writes.
	ساكن منطقي writeAll(صحيح fd, ثابت فراغ *بفر_2, size_t ناپ)
	{
		ثابت uint8_t *بائٹاں = static_cast<ثابت uint8_t *>(بفر_2);

		طالما (ناپ > 0) {
			ثابت ssize_t written = اكتب(fd, بائٹاں, ناپ);

			إذا (written <= 0) {
				أرجع خطأ;
			}

			بائٹاں += written;
			ناپ -= written;
		}

		أرجع سچ;
	}

};

static_assert(حجم(Uas_Id_ریکارڈ) <= 256, "OpenDroneID UAS ID record must fit in /fs/mtd_id");

// The UAS ID storage is only supported on PX4 targets with NuttX and an MTD partition named "ID".
// On other platforms, the get() method will return StorageUnavailable and the checkOrStore() method will return Unsupported.
Bid_جندرا_Info BidStorageBackend::get()
{
#إذا defined(__PX4_NUTTX)
	ثابت حرف *رستہ = خالی_سوچک;

	إذا (px4_mtd_query("MTD_ID", خالی_سوچک, &رستہ) != 0 || رستہ == خالی_سوچک) {
		أرجع {Bid_جندرا_State::StorageUnavailable, {}};
	}

	صحيح fd = open(رستہ, O_RDONLY);

	إذا (fd < 0) {
		أرجع {Bid_جندرا_State::StorageUnavailable, {}};
	}

	Uas_Id_ریکارڈ ریکارڈ{};

	إذا (اقرأ(fd, &ریکارڈ, حجم(ریکارڈ)) != حجم(ریکارڈ)) {
		close(fd);
		أرجع {Bid_جندرا_State::StorageUnavailable, {}};
	}

	close(fd);

	إذا (ریکارڈ_Valid(ریکارڈ)) {
		Bid_جندرا_Info info{Bid_جندرا_State::Locked, {}};
		info.id.ua_type = ریکارڈ.ua_type;
		حافظہ_نقل(info.id.uas_id, ریکارڈ.uas_id, UAS_ID_ناپ);
		أرجع info;
	}

	إذا (ریکارڈ_Blank(ریکارڈ)) {
		أرجع {Bid_جندرا_State::خالی_تھاواں_2, {}};
	}

	أرجع {Bid_جندرا_State::Invalid_ریکارڈ, {}};
#وإلا
	أرجع {Bid_جندرا_State::Unsupported, {}};
#جے_انت
}

// Check if the given UAS ID matches the stored one, or store it if the storage is empty.
// The storage is write-once, so once a UAS ID is stored it cannot be changed (except by erasing the flash).
Bid_Store_نتیجہ BidStorageBackend::checkOrStore(uint8_t ua_type, ثابت uint8_t uas_id[UAS_ID_ناپ])
{
	إذا (uas_Id_Is_خالی_تھاواں(uas_id)) {
		أرجع Bid_Store_نتیجہ::InvalidBasicId;
	}

#إذا defined(__PX4_NUTTX)
	ثابت حرف *رستہ = خالی_سوچک;

	إذا (px4_mtd_query("MTD_ID", خالی_سوچک, &رستہ) != 0 || رستہ == خالی_سوچک) {
		أرجع Bid_Store_نتیجہ::StorageUnavailable;
	}

	صحيح fd = open(رستہ, O_RDWR);

	إذا (fd < 0) {
		أرجع Bid_Store_نتیجہ::StorageUnavailable;
	}

	Uas_Id_ریکارڈ existing{};

	إذا (اقرأ(fd, &existing, حجم(existing)) != حجم(existing)) {
		close(fd);
		أرجع Bid_Store_نتیجہ::StorageUnavailable;
	}

	إذا (ریکارڈ_Valid(existing)) {
		ثابت Bid_Store_نتیجہ نتیجہ = ریکارڈ_Matches(existing, uas_id)
					      ? Bid_Store_نتیجہ::AlreadyLocked
					      : Bid_Store_نتیجہ::AlreadyLockedDifferent;
		close(fd);
		أرجع نتیجہ;
	}

	إذا (!ریکارڈ_Blank(existing)) {
		close(fd);
		أرجع Bid_Store_نتیجہ::Invalid_ریکارڈ;
	}

	Uas_Id_ریکارڈ ریکارڈ{};
	ریکارڈ.magic = ODID_UASID_MAGIC;
	ریکارڈ.نسخہ = ODID_UASID_ریکارڈ_نسخہ;
	ریکارڈ.id_type = UAS_ID_TYPE_SERIAL_نمبر;
	ریکارڈ.ua_type = ua_type;
	حافظہ_نقل(ریکارڈ.uas_id, uas_id, UAS_ID_ناپ);
	ریکارڈ.چکری_جانچ = ریکارڈ_چکری_جانچ(ریکارڈ);

	ثابت uint32_t magic = ریکارڈ.magic;
	ریکارڈ.magic = 0xffffffff;

	ثابت منطقي written = lseek(fd, حجم(ریکارڈ.magic), SEEK_SET) == static_cast<off_t>(حجم(ریکارڈ.magic))
			     && writeAll(fd, reinterpret_cast<ثابت uint8_t *>(&ریکارڈ) + حجم(ریکارڈ.magic),
					 حجم(ریکارڈ) - حجم(ریکارڈ.magic))
			     && fsync(fd) == 0
			     && lseek(fd, 0, SEEK_SET) == 0
			     && writeAll(fd, &magic, حجم(magic))
			     && fsync(fd) == 0;

	close(fd);

	أرجع written ? Bid_Store_نتیجہ::Stored : Bid_Store_نتیجہ::WriteError;
#وإلا
	أرجع Bid_Store_نتیجہ::Unsupported;
#جے_انت
}

} // namespace

// Public interface that uses the backend implementation.
Bid_جندرا_Info BidStorage::get()
{
	BidStorageBackend storage;
	أرجع storage.get();
}

Bid_Store_نتیجہ BidStorage::checkOrStore(uint8_t ua_type, ثابت uint8_t uas_id[UAS_ID_ناپ])
{
	BidStorageBackend storage;
	أرجع storage.checkOrStore(ua_type, uas_id);
}

//
Bid_Store_نتیجہ Bid_جندرا::resolve(uint8_t id_type, uint8_t ua_type, ثابت uint8_t uas_id[UAS_ID_ناپ],
				UasId &locked_id)
{
	إذا (id_type != UAS_ID_TYPE_SERIAL_نمبر) {
		أرجع Bid_Store_نتیجہ::InvalidBasicId;
	}

	إذا (!_loaded) {
		_info = BidStorage::get();
		_loaded = سچ;
	}

	إذا (_info.locked()) {
		locked_id = _info.id;
		أرجع Bid_Store_نتیجہ::AlreadyLocked;
	}

	إذا (_info.state != Bid_جندرا_State::خالی_تھاواں_2) {
		بدل (_info.state) {
		حالة Bid_جندرا_State::Invalid_ریکارڈ:
			أرجع Bid_Store_نتیجہ::Invalid_ریکارڈ;

		حالة Bid_جندرا_State::StorageUnavailable:
			أرجع Bid_Store_نتیجہ::StorageUnavailable;

		حالة Bid_جندرا_State::WriteError:
			أرجع Bid_Store_نتیجہ::WriteError;

		حالة Bid_جندرا_State::Unsupported:
			أرجع Bid_Store_نتیجہ::Unsupported;

		حالة Bid_جندرا_State::خالی_تھاواں_2:
		حالة Bid_جندرا_State::Locked:
			اكسر;
		}
	}

	ثابت Bid_Store_نتیجہ نتیجہ = BidStorage::checkOrStore(ua_type, uas_id);

	بدل (نتیجہ) {
	حالة Bid_Store_نتیجہ::Stored:
		_info.state = Bid_جندرا_State::Locked;
		_info.id.ua_type = ua_type;
		حافظہ_نقل(_info.id.uas_id, uas_id, UAS_ID_ناپ);
		locked_id = _info.id;
		أرجع نتیجہ;

	حالة Bid_Store_نتیجہ::AlreadyLocked:
	حالة Bid_Store_نتیجہ::AlreadyLockedDifferent: {
			_info = BidStorage::get();

			إذا (_info.locked()) {
				locked_id = _info.id;
				أرجع Bid_Store_نتیجہ::AlreadyLocked;
			}

			أرجع Bid_Store_نتیجہ::Invalid_ریکارڈ;
		}

	حالة Bid_Store_نتیجہ::InvalidBasicId:
	حالة Bid_Store_نتیجہ::StorageUnavailable:
	حالة Bid_Store_نتیجہ::Invalid_ریکارڈ:
	حالة Bid_Store_نتیجہ::WriteError:
	حالة Bid_Store_نتیجہ::Unsupported:
		أرجع نتیجہ;
	}

	أرجع Bid_Store_نتیجہ::Invalid_ریکارڈ;
}

} // namespace open_drone_id
