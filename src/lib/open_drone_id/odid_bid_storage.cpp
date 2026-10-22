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

#شامل "odid_bid_storage.hpp"

#شامل <fcntl.h>
#شامل <stddef.h>
#شامل <string.h>
#شامل <unistd.h>

#اگر_تعریف_شدہ __PX4_NUTTX
#شامل <nuttx/crc32.h>
#شامل <px4_platform_common/px4_manifest.h>
#ورنہ
#شامل <crc32.h>
#اگر_ختم

نامخلا open_drone_id
{
نامخلا
{

ساکن constexpr uint32_t ODID_UASID_MAGIC = 0x4449444f; // "ODID"
ساکن constexpr uint8_t ODID_UASID_ریکارڈ_نسخہ = 1;

ساخت __attribute__((packed)) Uas_Id_ریکارڈ {
	uint32_t magic;
	uint8_t نسخہ;
	uint8_t id_type;
	uint8_t ua_type;
	uint8_t reserved;
	uint8_t uas_id[UAS_ID_سائز];
	uint32_t چکری_جانچ;
};

جماعت BidStorageBackend
{
عوامی:
	Bid_تالا_Info get();
	Bid_Store_نتیجہ checkOrStore(uint8_t ua_type, ثابت uint8_t uas_id[UAS_ID_سائز]);

نجی:
	ساکن منطقی uas_Id_Is_خالی_خانے(ثابت uint8_t uas_id[UAS_ID_سائز])
	{
		برائے (غیرعلامتی i = 0; i < UAS_ID_سائز; ++i) {
			اگر (uas_id[i] != 0) {
				واپس جھوٹ;
			}
		}

		واپس سچ;
	}

	ساکن uint32_t ریکارڈ_چکری_جانچ(ثابت Uas_Id_ریکارڈ &ریکارڈ)
	{
		واپس چکری_جانچ_32(reinterpret_cast<ثابت uint8_t *>(&ریکارڈ.نسخہ),
			     offsetof(Uas_Id_ریکارڈ, چکری_جانچ) - offsetof(Uas_Id_ریکارڈ, نسخہ));
	}

	ساکن منطقی ریکارڈ_Valid(ثابت Uas_Id_ریکارڈ &ریکارڈ)
	{
		واپس ریکارڈ.magic == ODID_UASID_MAGIC
		       && ریکارڈ.نسخہ == ODID_UASID_ریکارڈ_نسخہ
		       && ریکارڈ.id_type == UAS_ID_TYPE_SERIAL_نمبر
		       && !uas_Id_Is_خالی_خانے(ریکارڈ.uas_id)
		       && ریکارڈ.چکری_جانچ == ریکارڈ_چکری_جانچ(ریکارڈ);
	}

	// Check if the record is all 0xff, which is the erased state of flash.
	// This is needed to distinguish between an unwritten record and a record that was written with all bytes set to 0 (which would be invalid but not unwritten).
	ساکن منطقی ریکارڈ_Blank(ثابت Uas_Id_ریکارڈ &ریکارڈ)
	{
		ثابت uint8_t *بائٹیں = reinterpret_cast<ثابت uint8_t *>(&ریکارڈ);

		برائے (غیرعلامتی i = 0; i < حجم(ریکارڈ); ++i) {
			اگر (بائٹیں[i] != 0xff) {
				واپس جھوٹ;
			}
		}

		واپس سچ;
	}

	ساکن منطقی ریکارڈ_Matches(ثابت Uas_Id_ریکارڈ &ریکارڈ, ثابت uint8_t uas_id[UAS_ID_سائز])
	{
		واپس memcmp(ریکارڈ.uas_id, uas_id, UAS_ID_سائز) == 0;
	}

	// Write the entire buffer to the file descriptor, handling short writes.
	ساکن منطقی writeAll(صحیح fd, ثابت خالی *بفر_2, size_t سائز)
	{
		ثابت uint8_t *بائٹیں = static_cast<ثابت uint8_t *>(بفر_2);

		جب (سائز > 0) {
			ثابت ssize_t written = اكتب(fd, بائٹیں, سائز);

			اگر (written <= 0) {
				واپس جھوٹ;
			}

			بائٹیں += written;
			سائز -= written;
		}

		واپس سچ;
	}

};

static_assert(حجم(Uas_Id_ریکارڈ) <= 256, "OpenDroneID UAS ID record must fit in /fs/mtd_id");

// The UAS ID storage is only supported on PX4 targets with NuttX and an MTD partition named "ID".
// On other platforms, the get() method will return StorageUnavailable and the checkOrStore() method will return Unsupported.
Bid_تالا_Info BidStorageBackend::get()
{
#اگر defined(__PX4_NUTTX)
	ثابت حرف *راستہ = خالی_اشاریہ;

	اگر (px4_mtd_query("MTD_ID", خالی_اشاریہ, &راستہ) != 0 || راستہ == خالی_اشاریہ) {
		واپس {Bid_تالا_State::StorageUnavailable, {}};
	}

	صحیح fd = open(راستہ, O_RDONLY);

	اگر (fd < 0) {
		واپس {Bid_تالا_State::StorageUnavailable, {}};
	}

	Uas_Id_ریکارڈ ریکارڈ{};

	اگر (اقرأ(fd, &ریکارڈ, حجم(ریکارڈ)) != حجم(ریکارڈ)) {
		close(fd);
		واپس {Bid_تالا_State::StorageUnavailable, {}};
	}

	close(fd);

	اگر (ریکارڈ_Valid(ریکارڈ)) {
		Bid_تالا_Info info{Bid_تالا_State::Locked, {}};
		info.id.ua_type = ریکارڈ.ua_type;
		حافظہ_نقل(info.id.uas_id, ریکارڈ.uas_id, UAS_ID_سائز);
		واپس info;
	}

	اگر (ریکارڈ_Blank(ریکارڈ)) {
		واپس {Bid_تالا_State::خالی_خانے_2, {}};
	}

	واپس {Bid_تالا_State::Invalid_ریکارڈ, {}};
#ورنہ
	واپس {Bid_تالا_State::Unsupported, {}};
#اگر_ختم
}

// Check if the given UAS ID matches the stored one, or store it if the storage is empty.
// The storage is write-once, so once a UAS ID is stored it cannot be changed (except by erasing the flash).
Bid_Store_نتیجہ BidStorageBackend::checkOrStore(uint8_t ua_type, ثابت uint8_t uas_id[UAS_ID_سائز])
{
	اگر (uas_Id_Is_خالی_خانے(uas_id)) {
		واپس Bid_Store_نتیجہ::InvalidBasicId;
	}

#اگر defined(__PX4_NUTTX)
	ثابت حرف *راستہ = خالی_اشاریہ;

	اگر (px4_mtd_query("MTD_ID", خالی_اشاریہ, &راستہ) != 0 || راستہ == خالی_اشاریہ) {
		واپس Bid_Store_نتیجہ::StorageUnavailable;
	}

	صحیح fd = open(راستہ, O_RDWR);

	اگر (fd < 0) {
		واپس Bid_Store_نتیجہ::StorageUnavailable;
	}

	Uas_Id_ریکارڈ existing{};

	اگر (اقرأ(fd, &existing, حجم(existing)) != حجم(existing)) {
		close(fd);
		واپس Bid_Store_نتیجہ::StorageUnavailable;
	}

	اگر (ریکارڈ_Valid(existing)) {
		ثابت Bid_Store_نتیجہ نتیجہ = ریکارڈ_Matches(existing, uas_id)
					      ? Bid_Store_نتیجہ::AlreadyLocked
					      : Bid_Store_نتیجہ::AlreadyLockedDifferent;
		close(fd);
		واپس نتیجہ;
	}

	اگر (!ریکارڈ_Blank(existing)) {
		close(fd);
		واپس Bid_Store_نتیجہ::Invalid_ریکارڈ;
	}

	Uas_Id_ریکارڈ ریکارڈ{};
	ریکارڈ.magic = ODID_UASID_MAGIC;
	ریکارڈ.نسخہ = ODID_UASID_ریکارڈ_نسخہ;
	ریکارڈ.id_type = UAS_ID_TYPE_SERIAL_نمبر;
	ریکارڈ.ua_type = ua_type;
	حافظہ_نقل(ریکارڈ.uas_id, uas_id, UAS_ID_سائز);
	ریکارڈ.چکری_جانچ = ریکارڈ_چکری_جانچ(ریکارڈ);

	ثابت uint32_t magic = ریکارڈ.magic;
	ریکارڈ.magic = 0xffffffff;

	ثابت منطقی written = lseek(fd, حجم(ریکارڈ.magic), SEEK_SET) == static_cast<off_t>(حجم(ریکارڈ.magic))
			     && writeAll(fd, reinterpret_cast<ثابت uint8_t *>(&ریکارڈ) + حجم(ریکارڈ.magic),
					 حجم(ریکارڈ) - حجم(ریکارڈ.magic))
			     && fsync(fd) == 0
			     && lseek(fd, 0, SEEK_SET) == 0
			     && writeAll(fd, &magic, حجم(magic))
			     && fsync(fd) == 0;

	close(fd);

	واپس written ? Bid_Store_نتیجہ::Stored : Bid_Store_نتیجہ::WriteError;
#ورنہ
	واپس Bid_Store_نتیجہ::Unsupported;
#اگر_ختم
}

} // namespace

// Public interface that uses the backend implementation.
Bid_تالا_Info BidStorage::get()
{
	BidStorageBackend storage;
	واپس storage.get();
}

Bid_Store_نتیجہ BidStorage::checkOrStore(uint8_t ua_type, ثابت uint8_t uas_id[UAS_ID_سائز])
{
	BidStorageBackend storage;
	واپس storage.checkOrStore(ua_type, uas_id);
}

//
Bid_Store_نتیجہ Bid_تالا::resolve(uint8_t id_type, uint8_t ua_type, ثابت uint8_t uas_id[UAS_ID_سائز],
				UasId &locked_id)
{
	اگر (id_type != UAS_ID_TYPE_SERIAL_نمبر) {
		واپس Bid_Store_نتیجہ::InvalidBasicId;
	}

	اگر (!_loaded) {
		_info = BidStorage::get();
		_loaded = سچ;
	}

	اگر (_info.locked()) {
		locked_id = _info.id;
		واپس Bid_Store_نتیجہ::AlreadyLocked;
	}

	اگر (_info.state != Bid_تالا_State::خالی_خانے_2) {
		انتخاب (_info.state) {
		حالت Bid_تالا_State::Invalid_ریکارڈ:
			واپس Bid_Store_نتیجہ::Invalid_ریکارڈ;

		حالت Bid_تالا_State::StorageUnavailable:
			واپس Bid_Store_نتیجہ::StorageUnavailable;

		حالت Bid_تالا_State::WriteError:
			واپس Bid_Store_نتیجہ::WriteError;

		حالت Bid_تالا_State::Unsupported:
			واپس Bid_Store_نتیجہ::Unsupported;

		حالت Bid_تالا_State::خالی_خانے_2:
		حالت Bid_تالا_State::Locked:
			توڑو;
		}
	}

	ثابت Bid_Store_نتیجہ نتیجہ = BidStorage::checkOrStore(ua_type, uas_id);

	انتخاب (نتیجہ) {
	حالت Bid_Store_نتیجہ::Stored:
		_info.state = Bid_تالا_State::Locked;
		_info.id.ua_type = ua_type;
		حافظہ_نقل(_info.id.uas_id, uas_id, UAS_ID_سائز);
		locked_id = _info.id;
		واپس نتیجہ;

	حالت Bid_Store_نتیجہ::AlreadyLocked:
	حالت Bid_Store_نتیجہ::AlreadyLockedDifferent: {
			_info = BidStorage::get();

			اگر (_info.locked()) {
				locked_id = _info.id;
				واپس Bid_Store_نتیجہ::AlreadyLocked;
			}

			واپس Bid_Store_نتیجہ::Invalid_ریکارڈ;
		}

	حالت Bid_Store_نتیجہ::InvalidBasicId:
	حالت Bid_Store_نتیجہ::StorageUnavailable:
	حالت Bid_Store_نتیجہ::Invalid_ریکارڈ:
	حالت Bid_Store_نتیجہ::WriteError:
	حالت Bid_Store_نتیجہ::Unsupported:
		واپس نتیجہ;
	}

	واپس Bid_Store_نتیجہ::Invalid_ریکارڈ;
}

} // namespace open_drone_id
