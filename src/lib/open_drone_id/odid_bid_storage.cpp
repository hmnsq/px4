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

#جيڪڏهن_وصف_ٿيل __PX4_NUTTX
#ضمن <nuttx/crc32.h>
#ضمن <px4_platform_common/px4_manifest.h>
#وإلا
#ضمن <crc32.h>
#جيڪڏهن_پورو

نطاق open_drone_id
{
نطاق
{

ساكن constexpr uint32_t ODID_UASID_MAGIC = 0x4449444f; // "ODID"
ساكن constexpr uint8_t ODID_UASID_رڪارڊ_نسخو = 1;

بنية __attribute__((packed)) Uas_Id_رڪارڊ {
	uint32_t magic;
	uint8_t نسخو;
	uint8_t id_type;
	uint8_t ua_type;
	uint8_t reserved;
	uint8_t uas_id[UAS_ID_سائيز];
	uint32_t چڪري_جاچ;
};

صنف BidStorageBackend
{
عام:
	Bid_تالو_Info get();
	Bid_Store_نتيجو checkOrStore(uint8_t ua_type, ثابت uint8_t uas_id[UAS_ID_سائيز]);

خاص:
	ساكن منطقي uas_Id_Is_خالي_جايون(ثابت uint8_t uas_id[UAS_ID_سائيز])
	{
		لكل (بنانشان i = 0; i < UAS_ID_سائيز; ++i) {
			إذا (uas_id[i] != 0) {
				أرجع خطأ;
			}
		}

		أرجع سچ;
	}

	ساكن uint32_t رڪارڊ_چڪري_جاچ(ثابت Uas_Id_رڪارڊ &رڪارڊ)
	{
		أرجع چڪري_جاچ_32(reinterpret_cast<ثابت uint8_t *>(&رڪارڊ.نسخو),
			     offsetof(Uas_Id_رڪارڊ, چڪري_جاچ) - offsetof(Uas_Id_رڪارڊ, نسخو));
	}

	ساكن منطقي رڪارڊ_Valid(ثابت Uas_Id_رڪارڊ &رڪارڊ)
	{
		أرجع رڪارڊ.magic == ODID_UASID_MAGIC
		       && رڪارڊ.نسخو == ODID_UASID_رڪارڊ_نسخو
		       && رڪارڊ.id_type == UAS_ID_TYPE_SERIAL_عدد
		       && !uas_Id_Is_خالي_جايون(رڪارڊ.uas_id)
		       && رڪارڊ.چڪري_جاچ == رڪارڊ_چڪري_جاچ(رڪارڊ);
	}

	// Check if the record is all 0xff, which is the erased state of flash.
	// This is needed to distinguish between an unwritten record and a record that was written with all bytes set to 0 (which would be invalid but not unwritten).
	ساكن منطقي رڪارڊ_Blank(ثابت Uas_Id_رڪارڊ &رڪارڊ)
	{
		ثابت uint8_t *بائيٽون = reinterpret_cast<ثابت uint8_t *>(&رڪارڊ);

		لكل (بنانشان i = 0; i < حجم(رڪارڊ); ++i) {
			إذا (بائيٽون[i] != 0xff) {
				أرجع خطأ;
			}
		}

		أرجع سچ;
	}

	ساكن منطقي رڪارڊ_Matches(ثابت Uas_Id_رڪارڊ &رڪارڊ, ثابت uint8_t uas_id[UAS_ID_سائيز])
	{
		أرجع memcmp(رڪارڊ.uas_id, uas_id, UAS_ID_سائيز) == 0;
	}

	// Write the entire buffer to the file descriptor, handling short writes.
	ساكن منطقي writeAll(صحيح fd, ثابت فراغ *بفر_2, size_t سائيز)
	{
		ثابت uint8_t *بائيٽون = static_cast<ثابت uint8_t *>(بفر_2);

		طالما (سائيز > 0) {
			ثابت ssize_t written = اكتب(fd, بائيٽون, سائيز);

			إذا (written <= 0) {
				أرجع خطأ;
			}

			بائيٽون += written;
			سائيز -= written;
		}

		أرجع سچ;
	}

};

static_assert(حجم(Uas_Id_رڪارڊ) <= 256, "OpenDroneID UAS ID record must fit in /fs/mtd_id");

// The UAS ID storage is only supported on PX4 targets with NuttX and an MTD partition named "ID".
// On other platforms, the get() method will return StorageUnavailable and the checkOrStore() method will return Unsupported.
Bid_تالو_Info BidStorageBackend::get()
{
#إذا defined(__PX4_NUTTX)
	ثابت حرف *رستو = خالي_اشارو;

	إذا (px4_mtd_query("MTD_ID", خالي_اشارو, &رستو) != 0 || رستو == خالي_اشارو) {
		أرجع {Bid_تالو_State::StorageUnavailable, {}};
	}

	صحيح fd = open(رستو, O_RDONLY);

	إذا (fd < 0) {
		أرجع {Bid_تالو_State::StorageUnavailable, {}};
	}

	Uas_Id_رڪارڊ رڪارڊ{};

	إذا (اقرأ(fd, &رڪارڊ, حجم(رڪارڊ)) != حجم(رڪارڊ)) {
		close(fd);
		أرجع {Bid_تالو_State::StorageUnavailable, {}};
	}

	close(fd);

	إذا (رڪارڊ_Valid(رڪارڊ)) {
		Bid_تالو_Info info{Bid_تالو_State::Locked, {}};
		info.id.ua_type = رڪارڊ.ua_type;
		يادگيري_نقل(info.id.uas_id, رڪارڊ.uas_id, UAS_ID_سائيز);
		أرجع info;
	}

	إذا (رڪارڊ_Blank(رڪارڊ)) {
		أرجع {Bid_تالو_State::خالي_جايون_2, {}};
	}

	أرجع {Bid_تالو_State::Invalid_رڪارڊ, {}};
#وإلا
	أرجع {Bid_تالو_State::Unsupported, {}};
#جيڪڏهن_پورو
}

// Check if the given UAS ID matches the stored one, or store it if the storage is empty.
// The storage is write-once, so once a UAS ID is stored it cannot be changed (except by erasing the flash).
Bid_Store_نتيجو BidStorageBackend::checkOrStore(uint8_t ua_type, ثابت uint8_t uas_id[UAS_ID_سائيز])
{
	إذا (uas_Id_Is_خالي_جايون(uas_id)) {
		أرجع Bid_Store_نتيجو::InvalidBasicId;
	}

#إذا defined(__PX4_NUTTX)
	ثابت حرف *رستو = خالي_اشارو;

	إذا (px4_mtd_query("MTD_ID", خالي_اشارو, &رستو) != 0 || رستو == خالي_اشارو) {
		أرجع Bid_Store_نتيجو::StorageUnavailable;
	}

	صحيح fd = open(رستو, O_RDWR);

	إذا (fd < 0) {
		أرجع Bid_Store_نتيجو::StorageUnavailable;
	}

	Uas_Id_رڪارڊ existing{};

	إذا (اقرأ(fd, &existing, حجم(existing)) != حجم(existing)) {
		close(fd);
		أرجع Bid_Store_نتيجو::StorageUnavailable;
	}

	إذا (رڪارڊ_Valid(existing)) {
		ثابت Bid_Store_نتيجو نتيجو = رڪارڊ_Matches(existing, uas_id)
					      ? Bid_Store_نتيجو::AlreadyLocked
					      : Bid_Store_نتيجو::AlreadyLockedDifferent;
		close(fd);
		أرجع نتيجو;
	}

	إذا (!رڪارڊ_Blank(existing)) {
		close(fd);
		أرجع Bid_Store_نتيجو::Invalid_رڪارڊ;
	}

	Uas_Id_رڪارڊ رڪارڊ{};
	رڪارڊ.magic = ODID_UASID_MAGIC;
	رڪارڊ.نسخو = ODID_UASID_رڪارڊ_نسخو;
	رڪارڊ.id_type = UAS_ID_TYPE_SERIAL_عدد;
	رڪارڊ.ua_type = ua_type;
	يادگيري_نقل(رڪارڊ.uas_id, uas_id, UAS_ID_سائيز);
	رڪارڊ.چڪري_جاچ = رڪارڊ_چڪري_جاچ(رڪارڊ);

	ثابت uint32_t magic = رڪارڊ.magic;
	رڪارڊ.magic = 0xffffffff;

	ثابت منطقي written = lseek(fd, حجم(رڪارڊ.magic), SEEK_SET) == static_cast<off_t>(حجم(رڪارڊ.magic))
			     && writeAll(fd, reinterpret_cast<ثابت uint8_t *>(&رڪارڊ) + حجم(رڪارڊ.magic),
					 حجم(رڪارڊ) - حجم(رڪارڊ.magic))
			     && fsync(fd) == 0
			     && lseek(fd, 0, SEEK_SET) == 0
			     && writeAll(fd, &magic, حجم(magic))
			     && fsync(fd) == 0;

	close(fd);

	أرجع written ? Bid_Store_نتيجو::Stored : Bid_Store_نتيجو::WriteError;
#وإلا
	أرجع Bid_Store_نتيجو::Unsupported;
#جيڪڏهن_پورو
}

} // namespace

// Public interface that uses the backend implementation.
Bid_تالو_Info BidStorage::get()
{
	BidStorageBackend storage;
	أرجع storage.get();
}

Bid_Store_نتيجو BidStorage::checkOrStore(uint8_t ua_type, ثابت uint8_t uas_id[UAS_ID_سائيز])
{
	BidStorageBackend storage;
	أرجع storage.checkOrStore(ua_type, uas_id);
}

//
Bid_Store_نتيجو Bid_تالو::resolve(uint8_t id_type, uint8_t ua_type, ثابت uint8_t uas_id[UAS_ID_سائيز],
				UasId &locked_id)
{
	إذا (id_type != UAS_ID_TYPE_SERIAL_عدد) {
		أرجع Bid_Store_نتيجو::InvalidBasicId;
	}

	إذا (!_loaded) {
		_info = BidStorage::get();
		_loaded = سچ;
	}

	إذا (_info.locked()) {
		locked_id = _info.id;
		أرجع Bid_Store_نتيجو::AlreadyLocked;
	}

	إذا (_info.state != Bid_تالو_State::خالي_جايون_2) {
		بدل (_info.state) {
		حالة Bid_تالو_State::Invalid_رڪارڊ:
			أرجع Bid_Store_نتيجو::Invalid_رڪارڊ;

		حالة Bid_تالو_State::StorageUnavailable:
			أرجع Bid_Store_نتيجو::StorageUnavailable;

		حالة Bid_تالو_State::WriteError:
			أرجع Bid_Store_نتيجو::WriteError;

		حالة Bid_تالو_State::Unsupported:
			أرجع Bid_Store_نتيجو::Unsupported;

		حالة Bid_تالو_State::خالي_جايون_2:
		حالة Bid_تالو_State::Locked:
			اكسر;
		}
	}

	ثابت Bid_Store_نتيجو نتيجو = BidStorage::checkOrStore(ua_type, uas_id);

	بدل (نتيجو) {
	حالة Bid_Store_نتيجو::Stored:
		_info.state = Bid_تالو_State::Locked;
		_info.id.ua_type = ua_type;
		يادگيري_نقل(_info.id.uas_id, uas_id, UAS_ID_سائيز);
		locked_id = _info.id;
		أرجع نتيجو;

	حالة Bid_Store_نتيجو::AlreadyLocked:
	حالة Bid_Store_نتيجو::AlreadyLockedDifferent: {
			_info = BidStorage::get();

			إذا (_info.locked()) {
				locked_id = _info.id;
				أرجع Bid_Store_نتيجو::AlreadyLocked;
			}

			أرجع Bid_Store_نتيجو::Invalid_رڪارڊ;
		}

	حالة Bid_Store_نتيجو::InvalidBasicId:
	حالة Bid_Store_نتيجو::StorageUnavailable:
	حالة Bid_Store_نتيجو::Invalid_رڪارڊ:
	حالة Bid_Store_نتيجو::WriteError:
	حالة Bid_Store_نتيجو::Unsupported:
		أرجع نتيجو;
	}

	أرجع Bid_Store_نتيجو::Invalid_رڪارڊ;
}

} // namespace open_drone_id
