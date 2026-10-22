#شامل <unit_test.h>

#شامل <systemlib/err.h>

#شامل <stdio.h>
#شامل <string.h>
#شامل <unistd.h>

#شامل <drivers/drv_hrt.h>

#define DSM_DEBUG
#شامل <lib/rc/sbus.h>
#شامل <lib/rc/dsm.h>
#شامل <lib/rc/st24.h>
#شامل <lib/rc/sumd.h>
#شامل <lib/rc/crsf.h>
#شامل <lib/rc/ghst.hpp>

#اگر defined(CONFIG_ARCH_BOARD_PX4_SITL)
#define TEST_DATA_PATH "./test_data/"
#وگرنه
#define TEST_DATA_PATH CONFIG_BOARD_ریشه_PATH
#endif

بیرونی "C" __EXPORT صحیح rc_tests_main(صحیح argc, نویسه *argv[]);

رده RCTest : عمومی UnitTest
{
عمومی:
	منطقی run_tests() override;

خصوصی:
	منطقی crsfTest();
	منطقی ghstTest();
	منطقی dsmTest(ثابت نویسه *filepath, unsigned expected_chancount, unsigned expected_dropcount, unsigned chan0);
	منطقی dsmTest10Ch();
	منطقی dsmTest16Ch();
	منطقی dsmTest22msDSMX16Ch();
	منطقی dsmTestOrangeDsmx();
	منطقی sbus2Test();
	منطقی st24MalformedLengthTest();
	منطقی st24Test();
	منطقی sumdTest();
};

منطقی RCTest::run_tests()
{
	ut_run_test(crsfTest);
	ut_run_test(ghstTest);
	ut_run_test(dsmTest10Ch);
	ut_run_test(dsmTest16Ch);
	ut_run_test(dsmTest22msDSMX16Ch);
	ut_run_test(dsmTestOrangeDsmx);
	ut_run_test(sbus2Test);
	ut_run_test(st24MalformedLengthTest);
	ut_run_test(st24Test);
	ut_run_test(sumdTest);

	بازگشت (_tests_failed == 0);
}

منطقی RCTest::crsfTest()
{
	ثابت نویسه *filepath = TEST_DATA_PATH "crsf_rc_channels.txt";

	FILE *fp = fopen(filepath, "rt");

	ut_test(fp);
	//PX4_INFO("loading data from: %s", filepath);

	ثابت صحیح سطر_size = 500;
	نویسه سطر[سطر_size];
	منطقی has_decoded_مقادیر = نادرست;
	ثابت صحیح max_channels = 16;
	uint16_t rc_مقادیر[max_channels];
	uint16_t رقم_مقادیر = 0;
	صحیح سطر_counter = 1;

	وقتی (fgets(سطر, سطر_size, fp) != nullptr)  {

		اگر (strncmp(سطر, "INPUT ", 6) == 0) {

			اگر (has_decoded_مقادیر) {
				PX4_ERR("Parser decoded values that are not in the test file (line=%i)", سطر_counter);
				بازگشت نادرست;
			}

			// read the values
			ثابت نویسه *file_buffer = سطر + 6;
			صحیح frame_len = 0;
			uint8_t frame[300];
			صحیح offset;
			صحیح عدد;

			وقتی (sscanf(file_buffer, "%x, %n", &عدد, &offset) > 0) {
				frame[frame_len++] = عدد;
				file_buffer += offset;
			}

			// Pipe the data into the parser
			hrt_abstime now = hrt_absolute_time();

			منطقی result = crsf_parse(now, frame, frame_len, rc_مقادیر, &رقم_مقادیر, max_channels);

			اگر (result) {
				has_decoded_مقادیر = درست;
			}

		} وگرنه اگر (strncmp(سطر, "DECODED ", 8) == 0) {

			اگر (!has_decoded_مقادیر) {
				PX4_ERR("Test file contains decoded values but the parser did not decode anything (line=%i)", سطر_counter);
				بازگشت نادرست;
			}

			// read the values
			ثابت نویسه *file_buffer = سطر + 8;
			صحیح offset;
			صحیح expected_rc_مقدار;
			صحیح expected_رقم_channels = 0;

			وقتی (sscanf(file_buffer, "%x, %n", &expected_rc_مقدار, &offset) > 0) {

				// allow a small difference
				اگر (abs(expected_rc_مقدار - (صحیح)rc_مقادیر[expected_رقم_channels]) > 10) {
					PX4_ERR("File line: %i, channel: %i", سطر_counter, expected_رقم_channels);
					ut_compare("Wrong decoded channel", expected_rc_مقدار, rc_مقادیر[expected_رقم_channels]);
				}

				file_buffer += offset;
				++expected_رقم_channels;
			}

			اگر (expected_رقم_channels != رقم_مقادیر) {
				PX4_ERR("File line: %d", سطر_counter);
				ut_compare("Unexpected number of decoded channels", expected_رقم_channels, رقم_مقادیر);
			}

			has_decoded_مقادیر = نادرست;
		}

		++سطر_counter;
	}

	بازگشت درست;
}

منطقی RCTest::ghstTest()
{
	ثابت نویسه *filepath = TEST_DATA_PATH "ghst_rc_channels.txt";

	FILE *fp = fopen(filepath, "rt");

	ut_test(fp);

	صحیح uart_fd = -1;
	ثابت صحیح سطر_size = 500;
	نویسه سطر[سطر_size];
	منطقی has_decoded_مقادیر = نادرست;
	ثابت صحیح max_channels = 16;
	uint16_t rc_مقادیر[max_channels];
	uint16_t رقم_مقادیر = 0;
	صحیح سطر_counter = 1;
	ghstLinkStatistics_t link_stats;
	ghst_config(uart_fd);

	وقتی (fgets(سطر, سطر_size, fp) != nullptr)  {

		اگر (strncmp(سطر, "INPUT ", 6) == 0) {

			اگر (has_decoded_مقادیر) {
				PX4_ERR("Parser decoded values that are not in the test file (line=%i)", سطر_counter);
				بازگشت نادرست;
			}

			// read the values
			ثابت نویسه *file_buffer = سطر + 6;
			صحیح frame_len = 0;
			uint8_t frame[300];
			صحیح offset;
			صحیح عدد;

			وقتی (sscanf(file_buffer, "%x, %n", &عدد, &offset) > 0) {
				frame[frame_len++] = عدد;
				file_buffer += offset;
			}

			// Pipe the data into the parser
			hrt_abstime now = hrt_absolute_time();

			منطقی result = ghst_parse(now, frame, frame_len, rc_مقادیر, &link_stats, &رقم_مقادیر, max_channels);

			اگر (result) {
				has_decoded_مقادیر = درست;
			}

		} وگرنه اگر (strncmp(سطر, "DECODED ", 8) == 0) {

			اگر (!has_decoded_مقادیر) {
				PX4_ERR("Test file contains decoded values but the parser did not decode anything (line=%i)", سطر_counter);
				بازگشت نادرست;
			}

			// read the values
			ثابت نویسه *file_buffer = سطر + 8;
			صحیح offset;
			صحیح expected_rc_مقدار;
			صحیح expected_رقم_channels = 0;

			وقتی (sscanf(file_buffer, "%x, %n", &expected_rc_مقدار, &offset) > 0) {

				// allow a small difference
				اگر (abs(expected_rc_مقدار - (صحیح)rc_مقادیر[expected_رقم_channels]) > 10) {
					PX4_ERR("File line: %i, channel: %i", سطر_counter, expected_رقم_channels);
					ut_compare("Wrong decoded channel", expected_rc_مقدار, rc_مقادیر[expected_رقم_channels]);
				}

				file_buffer += offset;
				++expected_رقم_channels;
			}

			اگر (expected_رقم_channels != رقم_مقادیر) {
				PX4_ERR("File line: %d", سطر_counter);
				ut_compare("Unexpected number of decoded channels", expected_رقم_channels, رقم_مقادیر);
			}

			has_decoded_مقادیر = نادرست;
		}

		++سطر_counter;
	}

	بازگشت درست;
}

منطقی RCTest::dsmTest10Ch()
{
	بازگشت dsmTest(TEST_DATA_PATH "dsm_x_data.txt", 10, 2, 1500);
}

منطقی RCTest::dsmTest16Ch()
{
	بازگشت dsmTest(TEST_DATA_PATH "dsm_x_dx9_data.txt", 16, 1, 1500);
}

منطقی RCTest::dsmTest22msDSMX16Ch()
{
	بازگشت dsmTest(TEST_DATA_PATH "dsm_x_dx9_px4_binding_data.txt", 16, 1, 1499);
}

منطقی RCTest::dsmTestOrangeDsmx()
{
	بازگشت dsmTest(TEST_DATA_PATH "orangerx_dsmx_12.txt", 12, 1, 1499);
}

منطقی RCTest::dsmTest(ثابت نویسه *filepath, unsigned expected_chancount, unsigned expected_dropcount, unsigned chan0)
{
	FILE *fp;
	fp = fopen(filepath, "rt");

	ut_test(fp);
	//PX4_INFO("loading data from: %s", filepath);

	اعشاری f;
	unsigned x;
	صحیح ret;

	// Trash the first 20 lines
	برای (unsigned i = 0; i < 20; i++) {
		نویسه buf[200];
		(تهی)fgets(buf, اندازه(buf), fp);
	}

	// Init the parser
	uint8_t frame[30];
	uint16_t rc_مقادیر[18];
	uint16_t رقم_مقادیر;
	منطقی dsm_11_bit;
	unsigned dsm_frame_drops = 0;
	uint16_t max_channels = اندازه(rc_مقادیر) / اندازه(rc_مقادیر[0]);

	صحیح شمار = 0;
	unsigned last_drop = 0;

	dsm_proto_init();

	وقتی (EOF != (ret = fscanf(fp, "%f,%x,,", &f, &x))) {

		اگر (ret <= 0) {
			fclose(fp);
			ut_test(ret > 0);
		}

		frame[0] = x;
		unsigned len = 1;

		// Pipe the data into the parser
		منطقی result = dsm_parse(f * 1e6f, &frame[0], len, rc_مقادیر, &رقم_مقادیر,
					&dsm_11_bit, &dsm_frame_drops, nullptr, max_channels);

		اگر (result) {
			اگر (شمار > (16 * 20)) { // need to process enough data to have full channel count
				ut_compare("num_values == expected_chancount", رقم_مقادیر, expected_chancount);
			}

			ut_test(abs((صحیح)chan0 - (صحیح)rc_مقادیر[0]) < 30);

			//PX4_INFO("decoded packet with %d channels and %s encoding:", num_values, (dsm_11_bit) ? "11 bit" : "10 bit");

			برای (unsigned i = 0; i < رقم_مقادیر; i++) {
				//PX4_INFO("chan #%u:\t%d", i, (int)rc_values[i]);
			}
		}

		اگر (last_drop != (dsm_frame_drops)) {
			//PX4_INFO("frame dropped, now #%d", (dsm_frame_drops));
			last_drop = dsm_frame_drops;
		}

		شمار++;
	}

	fclose(fp);

	ut_compare("num_values == expected_chancount", رقم_مقادیر, expected_chancount);

	ut_test(ret == EOF);
	//PX4_INFO("drop: %d", (int)last_drop);
	ut_compare("last_drop == expected_dropcount", last_drop, expected_dropcount);

	بازگشت درست;
}

منطقی RCTest::sbus2Test()
{
	ثابت نویسه *filepath = TEST_DATA_PATH "sbus2_r7008SB.txt";

	FILE *fp;
	fp = fopen(filepath, "rt");

	ut_test(fp);
	//PX4_INFO("loading data from: %s", filepath);

	اعشاری f;
	unsigned x;
	صحیح ret;

	// Trash the first 20 lines
	برای (unsigned i = 0; i < 20; i++) {
		نویسه buf[200];
		(تهی)fgets(buf, اندازه(buf), fp);
	}

	// Init the parser
	uint8_t frame[SBUS_BUFFER_SIZE];
	uint16_t rc_مقادیر[18];
	uint16_t رقم_مقادیر;
	unsigned sbus_frame_drops = 0;
	unsigned sbus_frame_resets = 0;
	منطقی sbus_failsafe;
	منطقی sbus_frame_drop;
	uint16_t max_channels = اندازه(rc_مقادیر) / اندازه(rc_مقادیر[0]);

	unsigned last_drop = 0;

	وقتی (EOF != (ret = fscanf(fp, "%f,%x,,", &f, &x))) {

		اگر (ret <= 0) {
			fclose(fp);
			ut_test(ret > 0);
		}

		frame[0] = x;
		unsigned len = 1;

		// Pipe the data into the parser
		hrt_abstime now = hrt_absolute_time();

		// if (rate_limiter % byte_offset == 0) {
		منطقی result = sbus_parse(now, &frame[0], len, rc_مقادیر, &رقم_مقادیر,
					 &sbus_failsafe, &sbus_frame_drop, &sbus_frame_drops, max_channels);

		اگر (result) {
			//PX4_INFO("decoded packet");
		}

		// }

		اگر (last_drop != (sbus_frame_drops + sbus_frame_resets)) {
			PX4_WARN("frame dropped, now #%d", (sbus_frame_drops + sbus_frame_resets));
			last_drop = sbus_frame_drops + sbus_frame_resets;
		}

	}

	ut_test(ret == EOF);

	بازگشت درست;
}

منطقی RCTest::st24MalformedLengthTest()
{
	uint8_t rssi{};
	uint8_t lost_شمار{};
	uint16_t channel_شمار{};
	uint16_t channels[24] {};
	ثابت uint8_t invalid_lengths[] {0, 1};

	برای (ثابت uint8_t invalid_length : invalid_lengths) {
		ثابت uint8_t prefix[] {ST24_STX1, ST24_STX2, invalid_length, 0};

		برای (ثابت uint8_t byte : prefix) {
			(تهی)st24_decode(byte, &rssi, &lost_شمار, &channel_شمار, channels, 24);
		}

		برای (unsigned i = 0; i <= ST24_DATA_LEN_MAX; ++i) {
			(تهی)st24_decode(0xaa, &rssi, &lost_شمار, &channel_شمار, channels, 24);
		}
	}

	uint8_t minimal_packet[] {ST24_STX1, ST24_STX2, 2, 0xff, 0};
	minimal_packet[4] = st24_common_crc8(&minimal_packet[2], 2);
	صحیح result = 1;

	برای (ثابت uint8_t byte : minimal_packet) {
		result = st24_decode(byte, &rssi, &lost_شمار, &channel_شمار, channels, 24);
	}

	ut_test(result == 2);
	بازگشت درست;
}

منطقی RCTest::st24Test()
{
	ثابت نویسه *filepath = TEST_DATA_PATH "st24_data.txt";

	//PX4_INFO("loading data from: %s", filepath);

	FILE *fp;

	fp = fopen(filepath, "rt");
	ut_test(fp);

	اعشاری f;
	unsigned x;
	صحیح ret;

	// Trash the first 20 lines
	برای (unsigned i = 0; i < 20; i++) {
		نویسه buf[200];
		(تهی)fgets(buf, اندازه(buf), fp);
	}

	اعشاری last_time = 0;

	وقتی (EOF != (ret = fscanf(fp, "%f,%x,,", &f, &x))) {

		اگر (ret <= 0) {
			fclose(fp);
			ut_test(ret > 0);
		}

		اگر (((f - last_time) * 1000 * 1000) > 3000) {
			// PX4_INFO("FRAME RESET\n\n");
		}

		uint8_t b = static_cast<uint8_t>(x);

		last_time = f;

		// Pipe the data into the parser
		//hrt_abstime now = hrt_absolute_time();

		uint8_t rssi;
		uint8_t rx_شمار;
		uint16_t channel_شمار;
		uint16_t channels[20];

		اگر (!st24_decode(b, &rssi, &rx_شمار, &channel_شمار, channels, اندازه(channels) / اندازه(channels[0]))) {
			//PX4_INFO("decoded: %u channels (converted to PPM range)", (unsigned)channel_count);

			برای (unsigned i = 0; i < channel_شمار; i++) {
				//int16_t val = channels[i];
				//PX4_INFO("channel %u: %d 0x%03X", i, static_cast<int>(val), static_cast<int>(val));
			}
		}
	}

	ut_test(ret == EOF);

	بازگشت درست;
}

منطقی RCTest::sumdTest()
{
	ثابت نویسه *filepath = TEST_DATA_PATH "sumd_data.txt";

	//PX4_INFO("loading data from: %s", filepath);

	FILE *fp;

	fp = fopen(filepath, "rt");
	ut_test(fp);

	اعشاری f;
	unsigned x;
	صحیح ret;

	// Trash the first 20 lines
	برای (unsigned i = 0; i < 20; i++) {
		نویسه buf[200];
		(تهی)fgets(buf, اندازه(buf), fp);
	}

	اعشاری last_time = 0;

	وقتی (EOF != (ret = fscanf(fp, "%f,%x,,", &f, &x))) {

		اگر (ret <= 0) {
			fclose(fp);
			ut_test(ret > 0);
		}

		اگر (((f - last_time) * 1000 * 1000) > 3000) {
			// PX4_INFO("FRAME RESET\n\n");
		}

		uint8_t b = static_cast<uint8_t>(x);

		last_time = f;

		// Pipe the data into the parser
		//hrt_abstime now = hrt_absolute_time();

		uint8_t rssi;
		uint8_t rx_شمار;
		uint16_t channel_شمار;
		uint16_t channels[32];
		منطقی sumd_failsafe;


		اگر (!sumd_decode(b, &rssi, &rx_شمار, &channel_شمار, channels, 32, &sumd_failsafe)) {
			//PX4_INFO("decoded: %u channels (converted to PPM range)", (unsigned)channel_count);

			برای (unsigned i = 0; i < channel_شمار; i++) {
				//int16_t val = channels[i];
				//PX4_INFO("channel %u: %d 0x%03X", i, static_cast<int>(val), static_cast<int>(val));
			}
		}
	}

	ut_test(ret == EOF);

	بازگشت درست;
}

ut_declare_test_c(rc_tests_main, RCTest)
