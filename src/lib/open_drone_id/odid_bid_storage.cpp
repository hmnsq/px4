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

#అన్తర్భావ "odid_bid_storage.hpp"

#అన్తర్భావ <fcntl.h>
#అన్తర్భావ <stddef.h>
#అన్తర్భావ <string.h>
#అన్తర్భావ <unistd.h>

#నిర్వచితమైతే __PX4_NUTTX
#అన్తర్భావ <nuttx/crc32.h>
#అన్తర్భావ <px4_platform_common/px4_manifest.h>
#లేకపోతే
#అన్తర్భావ <crc32.h>
#అయితే_ముగింపు

నామాకాశ open_drone_id
{
నామాకాశ
{

స్థిర constexpr uint32_t ODID_UASID_MAGIC = 0x4449444f; // "ODID"
స్థిర constexpr uint8_t ODID_UASID_రికార్డు_సంస్కరణ = 1;

నిర్మాణం __attribute__((packed)) Uas_Id_రికార్డు {
	uint32_t magic;
	uint8_t సంస్కరణ;
	uint8_t id_type;
	uint8_t ua_type;
	uint8_t reserved;
	uint8_t uas_id[UAS_ID_కొలత];
	uint32_t చక్రీయ_తనిఖీ;
};

తరగతి BidStorageBackend
{
బహిరంగ:
	Bid_తాళం_Info get();
	Bid_Store_ఫలితం checkOrStore(uint8_t ua_type, స్థిరరాశి uint8_t uas_id[UAS_ID_కొలత]);

అంతరంగ:
	స్థిర తార్కిక uas_Id_Is_ఖాళీలు(స్థిరరాశి uint8_t uas_id[UAS_ID_కొలత])
	{
		కొరకు (గుర్తులేని i = 0; i < UAS_ID_కొలత; ++i) {
			ఒకవేళ (uas_id[i] != 0) {
				తిరిగి అబద్ధం;
			}
		}

		తిరిగి నిజం;
	}

	స్థిర uint32_t రికార్డు_చక్రీయ_తనిఖీ(స్థిరరాశి Uas_Id_రికార్డు &రికార్డు)
	{
		తిరిగి చక్రీయ_తనిఖీ_32(reinterpret_cast<స్థిరరాశి uint8_t *>(&రికార్డు.సంస్కరణ),
			     offsetof(Uas_Id_రికార్డు, చక్రీయ_తనిఖీ) - offsetof(Uas_Id_రికార్డు, సంస్కరణ));
	}

	స్థిర తార్కిక రికార్డు_Valid(స్థిరరాశి Uas_Id_రికార్డు &రికార్డు)
	{
		తిరిగి రికార్డు.magic == ODID_UASID_MAGIC
		       && రికార్డు.సంస్కరణ == ODID_UASID_రికార్డు_సంస్కరణ
		       && రికార్డు.id_type == UAS_ID_TYPE_SERIAL_సంఖ్య
		       && !uas_Id_Is_ఖాళీలు(రికార్డు.uas_id)
		       && రికార్డు.చక్రీయ_తనిఖీ == రికార్డు_చక్రీయ_తనిఖీ(రికార్డు);
	}

	// Check if the record is all 0xff, which is the erased state of flash.
	// This is needed to distinguish between an unwritten record and a record that was written with all bytes set to 0 (which would be invalid but not unwritten).
	స్థిర తార్కిక రికార్డు_Blank(స్థిరరాశి Uas_Id_రికార్డు &రికార్డు)
	{
		స్థిరరాశి uint8_t *బైట్లు = reinterpret_cast<స్థిరరాశి uint8_t *>(&రికార్డు);

		కొరకు (గుర్తులేని i = 0; i < పరిమాణం(రికార్డు); ++i) {
			ఒకవేళ (బైట్లు[i] != 0xff) {
				తిరిగి అబద్ధం;
			}
		}

		తిరిగి నిజం;
	}

	స్థిర తార్కిక రికార్డు_Matches(స్థిరరాశి Uas_Id_రికార్డు &రికార్డు, స్థిరరాశి uint8_t uas_id[UAS_ID_కొలత])
	{
		తిరిగి memcmp(రికార్డు.uas_id, uas_id, UAS_ID_కొలత) == 0;
	}

	// Write the entire buffer to the file descriptor, handling short writes.
	స్థిర తార్కిక writeAll(పూర్ణాంకం fd, స్థిరరాశి శూన్య *బఫరు_2, size_t కొలత)
	{
		స్థిరరాశి uint8_t *బైట్లు = static_cast<స్థిరరాశి uint8_t *>(బఫరు_2);

		అయితే (కొలత > 0) {
			స్థిరరాశి ssize_t written = లిఖ(fd, బైట్లు, కొలత);

			ఒకవేళ (written <= 0) {
				తిరిగి అబద్ధం;
			}

			బైట్లు += written;
			కొలత -= written;
		}

		తిరిగి నిజం;
	}

};

static_assert(పరిమాణం(Uas_Id_రికార్డు) <= 256, "OpenDroneID UAS ID record must fit in /fs/mtd_id");

// The UAS ID storage is only supported on PX4 targets with NuttX and an MTD partition named "ID".
// On other platforms, the get() method will return StorageUnavailable and the checkOrStore() method will return Unsupported.
Bid_తాళం_Info BidStorageBackend::get()
{
#ఒకవేళ defined(__PX4_NUTTX)
	స్థిరరాశి అక్షరం *మార్గం = శూన్యసూచిక;

	ఒకవేళ (px4_mtd_query("MTD_ID", శూన్యసూచిక, &మార్గం) != 0 || మార్గం == శూన్యసూచిక) {
		తిరిగి {Bid_తాళం_State::StorageUnavailable, {}};
	}

	పూర్ణాంకం fd = open(మార్గం, O_RDONLY);

	ఒకవేళ (fd < 0) {
		తిరిగి {Bid_తాళం_State::StorageUnavailable, {}};
	}

	Uas_Id_రికార్డు రికార్డు{};

	ఒకవేళ (పఠ(fd, &రికార్డు, పరిమాణం(రికార్డు)) != పరిమాణం(రికార్డు)) {
		close(fd);
		తిరిగి {Bid_తాళం_State::StorageUnavailable, {}};
	}

	close(fd);

	ఒకవేళ (రికార్డు_Valid(రికార్డు)) {
		Bid_తాళం_Info info{Bid_తాళం_State::Locked, {}};
		info.id.ua_type = రికార్డు.ua_type;
		జ్ఞాపకం_నకలు(info.id.uas_id, రికార్డు.uas_id, UAS_ID_కొలత);
		తిరిగి info;
	}

	ఒకవేళ (రికార్డు_Blank(రికార్డు)) {
		తిరిగి {Bid_తాళం_State::ఖాళీలు_2, {}};
	}

	తిరిగి {Bid_తాళం_State::Invalid_రికార్డు, {}};
#లేకపోతే
	తిరిగి {Bid_తాళం_State::Unsupported, {}};
#అయితే_ముగింపు
}

// Check if the given UAS ID matches the stored one, or store it if the storage is empty.
// The storage is write-once, so once a UAS ID is stored it cannot be changed (except by erasing the flash).
Bid_Store_ఫలితం BidStorageBackend::checkOrStore(uint8_t ua_type, స్థిరరాశి uint8_t uas_id[UAS_ID_కొలత])
{
	ఒకవేళ (uas_Id_Is_ఖాళీలు(uas_id)) {
		తిరిగి Bid_Store_ఫలితం::InvalidBasicId;
	}

#ఒకవేళ defined(__PX4_NUTTX)
	స్థిరరాశి అక్షరం *మార్గం = శూన్యసూచిక;

	ఒకవేళ (px4_mtd_query("MTD_ID", శూన్యసూచిక, &మార్గం) != 0 || మార్గం == శూన్యసూచిక) {
		తిరిగి Bid_Store_ఫలితం::StorageUnavailable;
	}

	పూర్ణాంకం fd = open(మార్గం, O_RDWR);

	ఒకవేళ (fd < 0) {
		తిరిగి Bid_Store_ఫలితం::StorageUnavailable;
	}

	Uas_Id_రికార్డు existing{};

	ఒకవేళ (పఠ(fd, &existing, పరిమాణం(existing)) != పరిమాణం(existing)) {
		close(fd);
		తిరిగి Bid_Store_ఫలితం::StorageUnavailable;
	}

	ఒకవేళ (రికార్డు_Valid(existing)) {
		స్థిరరాశి Bid_Store_ఫలితం ఫలితం = రికార్డు_Matches(existing, uas_id)
					      ? Bid_Store_ఫలితం::AlreadyLocked
					      : Bid_Store_ఫలితం::AlreadyLockedDifferent;
		close(fd);
		తిరిగి ఫలితం;
	}

	ఒకవేళ (!రికార్డు_Blank(existing)) {
		close(fd);
		తిరిగి Bid_Store_ఫలితం::Invalid_రికార్డు;
	}

	Uas_Id_రికార్డు రికార్డు{};
	రికార్డు.magic = ODID_UASID_MAGIC;
	రికార్డు.సంస్కరణ = ODID_UASID_రికార్డు_సంస్కరణ;
	రికార్డు.id_type = UAS_ID_TYPE_SERIAL_సంఖ్య;
	రికార్డు.ua_type = ua_type;
	జ్ఞాపకం_నకలు(రికార్డు.uas_id, uas_id, UAS_ID_కొలత);
	రికార్డు.చక్రీయ_తనిఖీ = రికార్డు_చక్రీయ_తనిఖీ(రికార్డు);

	స్థిరరాశి uint32_t magic = రికార్డు.magic;
	రికార్డు.magic = 0xffffffff;

	స్థిరరాశి తార్కిక written = lseek(fd, పరిమాణం(రికార్డు.magic), SEEK_SET) == static_cast<off_t>(పరిమాణం(రికార్డు.magic))
			     && writeAll(fd, reinterpret_cast<స్థిరరాశి uint8_t *>(&రికార్డు) + పరిమాణం(రికార్డు.magic),
					 పరిమాణం(రికార్డు) - పరిమాణం(రికార్డు.magic))
			     && fsync(fd) == 0
			     && lseek(fd, 0, SEEK_SET) == 0
			     && writeAll(fd, &magic, పరిమాణం(magic))
			     && fsync(fd) == 0;

	close(fd);

	తిరిగి written ? Bid_Store_ఫలితం::Stored : Bid_Store_ఫలితం::WriteError;
#లేకపోతే
	తిరిగి Bid_Store_ఫలితం::Unsupported;
#అయితే_ముగింపు
}

} // namespace

// Public interface that uses the backend implementation.
Bid_తాళం_Info BidStorage::get()
{
	BidStorageBackend storage;
	తిరిగి storage.get();
}

Bid_Store_ఫలితం BidStorage::checkOrStore(uint8_t ua_type, స్థిరరాశి uint8_t uas_id[UAS_ID_కొలత])
{
	BidStorageBackend storage;
	తిరిగి storage.checkOrStore(ua_type, uas_id);
}

//
Bid_Store_ఫలితం Bid_తాళం::resolve(uint8_t id_type, uint8_t ua_type, స్థిరరాశి uint8_t uas_id[UAS_ID_కొలత],
				UasId &locked_id)
{
	ఒకవేళ (id_type != UAS_ID_TYPE_SERIAL_సంఖ్య) {
		తిరిగి Bid_Store_ఫలితం::InvalidBasicId;
	}

	ఒకవేళ (!_loaded) {
		_info = BidStorage::get();
		_loaded = నిజం;
	}

	ఒకవేళ (_info.locked()) {
		locked_id = _info.id;
		తిరిగి Bid_Store_ఫలితం::AlreadyLocked;
	}

	ఒకవేళ (_info.state != Bid_తాళం_State::ఖాళీలు_2) {
		ఎంపిక (_info.state) {
		స్థితి Bid_తాళం_State::Invalid_రికార్డు:
			తిరిగి Bid_Store_ఫలితం::Invalid_రికార్డు;

		స్థితి Bid_తాళం_State::StorageUnavailable:
			తిరిగి Bid_Store_ఫలితం::StorageUnavailable;

		స్థితి Bid_తాళం_State::WriteError:
			తిరిగి Bid_Store_ఫలితం::WriteError;

		స్థితి Bid_తాళం_State::Unsupported:
			తిరిగి Bid_Store_ఫలితం::Unsupported;

		స్థితి Bid_తాళం_State::ఖాళీలు_2:
		స్థితి Bid_తాళం_State::Locked:
			ఆపు;
		}
	}

	స్థిరరాశి Bid_Store_ఫలితం ఫలితం = BidStorage::checkOrStore(ua_type, uas_id);

	ఎంపిక (ఫలితం) {
	స్థితి Bid_Store_ఫలితం::Stored:
		_info.state = Bid_తాళం_State::Locked;
		_info.id.ua_type = ua_type;
		జ్ఞాపకం_నకలు(_info.id.uas_id, uas_id, UAS_ID_కొలత);
		locked_id = _info.id;
		తిరిగి ఫలితం;

	స్థితి Bid_Store_ఫలితం::AlreadyLocked:
	స్థితి Bid_Store_ఫలితం::AlreadyLockedDifferent: {
			_info = BidStorage::get();

			ఒకవేళ (_info.locked()) {
				locked_id = _info.id;
				తిరిగి Bid_Store_ఫలితం::AlreadyLocked;
			}

			తిరిగి Bid_Store_ఫలితం::Invalid_రికార్డు;
		}

	స్థితి Bid_Store_ఫలితం::InvalidBasicId:
	స్థితి Bid_Store_ఫలితం::StorageUnavailable:
	స్థితి Bid_Store_ఫలితం::Invalid_రికార్డు:
	స్థితి Bid_Store_ఫలితం::WriteError:
	స్థితి Bid_Store_ఫలితం::Unsupported:
		తిరిగి ఫలితం;
	}

	తిరిగి Bid_Store_ఫలితం::Invalid_రికార్డు;
}

} // namespace open_drone_id
