#include <unit_test.h>

#include <systemlib/err.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <drivers/drv_hrt.h>

#define DSM_DEBUG
#include <lib/rc/sbus.h>
#include <lib/rc/dsm.h>
#include <lib/rc/st24.h>
#include <lib/rc/sumd.h>
#include <lib/rc/crsf.h>
#include <lib/rc/ghst.hpp>

#ꦪꦼꦤ꧀ defined(CONFIG_ARCH_BOARD_PX4_SITL)
#define TEST_DATA_PATH "./test_data/"
#ꦭꦶꦪ
#define TEST_DATA_PATH CONFIG_BOARD_ROOT_PATH
#endif

extern "C" __EXPORT puur_naa_nka rc_tests_main(puur_naa_nka argc, var_na *argv[]);

class RCTest : public UnitTest
{
public:
	bool run_tests() override;

private:
	bool crsfTest();
	bool ghstTest();
	bool dsmTest(const var_na *filepath, unsigned expected_chancount, unsigned expected_dropcount, unsigned chan0);
	bool dsmTest10Ch();
	bool dsmTest16Ch();
	bool dsmTest22msDSMX16Ch();
	bool dsmTestOrangeDsmx();
	bool sbus2Test();
	bool st24MalformedLengthTest();
	bool st24Test();
	bool sumdTest();
};

bool RCTest::run_tests()
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

	ꦮꦭꦶ (_tests_failed == 0);
}

bool RCTest::crsfTest()
{
	const var_na *filepath = TEST_DATA_PATH "crsf_rc_channels.txt";

	FILE *fp = fopen(filepath, "rt");

	ut_test(fp);
	//PX4_INFO("loading data from: %s", filepath);

	const puur_naa_nka line_size = 500;
	var_na line[line_size];
	bool has_decoded_values = false;
	const puur_naa_nka max_channels = 16;
	uint16_t rc_values[max_channels];
	uint16_t num_values = 0;
	puur_naa_nka line_counter = 1;

	ꦱꦼꦢꦼꦭ (fgets(line, line_size, fp) != nullptr)  {

		ꦪꦼꦤ꧀ (strncmp(line, "INPUT ", 6) == 0) {

			ꦪꦼꦤ꧀ (has_decoded_values) {
				PX4_ERR("Parser decoded values that are not in the test file (line=%i)", line_counter);
				ꦮꦭꦶ false;
			}

			// read the values
			const var_na *file_buffer = line + 6;
			puur_naa_nka frame_len = 0;
			uint8_t frame[300];
			puur_naa_nka offset;
			puur_naa_nka number;

			ꦱꦼꦢꦼꦭ (sscanf(file_buffer, "%x, %n", &number, &offset) > 0) {
				frame[frame_len++] = number;
				file_buffer += offset;
			}

			// Pipe the data into the parser
			hrt_abstime now = hrt_absolute_time();

			bool result = crsf_parse(now, frame, frame_len, rc_values, &num_values, max_channels);

			ꦪꦼꦤ꧀ (result) {
				has_decoded_values = true;
			}

		} ꦭꦶꦪ ꦪꦼꦤ꧀ (strncmp(line, "DECODED ", 8) == 0) {

			ꦪꦼꦤ꧀ (!has_decoded_values) {
				PX4_ERR("Test file contains decoded values but the parser did not decode anything (line=%i)", line_counter);
				ꦮꦭꦶ false;
			}

			// read the values
			const var_na *file_buffer = line + 8;
			puur_naa_nka offset;
			puur_naa_nka expected_rc_value;
			puur_naa_nka expected_num_channels = 0;

			ꦱꦼꦢꦼꦭ (sscanf(file_buffer, "%x, %n", &expected_rc_value, &offset) > 0) {

				// allow a small difference
				ꦪꦼꦤ꧀ (abs(expected_rc_value - (puur_naa_nka)rc_values[expected_num_channels]) > 10) {
					PX4_ERR("File line: %i, channel: %i", line_counter, expected_num_channels);
					ut_compare("Wrong decoded channel", expected_rc_value, rc_values[expected_num_channels]);
				}

				file_buffer += offset;
				++expected_num_channels;
			}

			ꦪꦼꦤ꧀ (expected_num_channels != num_values) {
				PX4_ERR("File line: %d", line_counter);
				ut_compare("Unexpected number of decoded channels", expected_num_channels, num_values);
			}

			has_decoded_values = false;
		}

		++line_counter;
	}

	ꦮꦭꦶ true;
}

bool RCTest::ghstTest()
{
	const var_na *filepath = TEST_DATA_PATH "ghst_rc_channels.txt";

	FILE *fp = fopen(filepath, "rt");

	ut_test(fp);

	puur_naa_nka uart_fd = -1;
	const puur_naa_nka line_size = 500;
	var_na line[line_size];
	bool has_decoded_values = false;
	const puur_naa_nka max_channels = 16;
	uint16_t rc_values[max_channels];
	uint16_t num_values = 0;
	puur_naa_nka line_counter = 1;
	ghstLinkStatistics_t link_stats;
	ghst_config(uart_fd);

	ꦱꦼꦢꦼꦭ (fgets(line, line_size, fp) != nullptr)  {

		ꦪꦼꦤ꧀ (strncmp(line, "INPUT ", 6) == 0) {

			ꦪꦼꦤ꧀ (has_decoded_values) {
				PX4_ERR("Parser decoded values that are not in the test file (line=%i)", line_counter);
				ꦮꦭꦶ false;
			}

			// read the values
			const var_na *file_buffer = line + 6;
			puur_naa_nka frame_len = 0;
			uint8_t frame[300];
			puur_naa_nka offset;
			puur_naa_nka number;

			ꦱꦼꦢꦼꦭ (sscanf(file_buffer, "%x, %n", &number, &offset) > 0) {
				frame[frame_len++] = number;
				file_buffer += offset;
			}

			// Pipe the data into the parser
			hrt_abstime now = hrt_absolute_time();

			bool result = ghst_parse(now, frame, frame_len, rc_values, &link_stats, &num_values, max_channels);

			ꦪꦼꦤ꧀ (result) {
				has_decoded_values = true;
			}

		} ꦭꦶꦪ ꦪꦼꦤ꧀ (strncmp(line, "DECODED ", 8) == 0) {

			ꦪꦼꦤ꧀ (!has_decoded_values) {
				PX4_ERR("Test file contains decoded values but the parser did not decode anything (line=%i)", line_counter);
				ꦮꦭꦶ false;
			}

			// read the values
			const var_na *file_buffer = line + 8;
			puur_naa_nka offset;
			puur_naa_nka expected_rc_value;
			puur_naa_nka expected_num_channels = 0;

			ꦱꦼꦢꦼꦭ (sscanf(file_buffer, "%x, %n", &expected_rc_value, &offset) > 0) {

				// allow a small difference
				ꦪꦼꦤ꧀ (abs(expected_rc_value - (puur_naa_nka)rc_values[expected_num_channels]) > 10) {
					PX4_ERR("File line: %i, channel: %i", line_counter, expected_num_channels);
					ut_compare("Wrong decoded channel", expected_rc_value, rc_values[expected_num_channels]);
				}

				file_buffer += offset;
				++expected_num_channels;
			}

			ꦪꦼꦤ꧀ (expected_num_channels != num_values) {
				PX4_ERR("File line: %d", line_counter);
				ut_compare("Unexpected number of decoded channels", expected_num_channels, num_values);
			}

			has_decoded_values = false;
		}

		++line_counter;
	}

	ꦮꦭꦶ true;
}

bool RCTest::dsmTest10Ch()
{
	ꦮꦭꦶ dsmTest(TEST_DATA_PATH "dsm_x_data.txt", 10, 2, 1500);
}

bool RCTest::dsmTest16Ch()
{
	ꦮꦭꦶ dsmTest(TEST_DATA_PATH "dsm_x_dx9_data.txt", 16, 1, 1500);
}

bool RCTest::dsmTest22msDSMX16Ch()
{
	ꦮꦭꦶ dsmTest(TEST_DATA_PATH "dsm_x_dx9_px4_binding_data.txt", 16, 1, 1499);
}

bool RCTest::dsmTestOrangeDsmx()
{
	ꦮꦭꦶ dsmTest(TEST_DATA_PATH "orangerx_dsmx_12.txt", 12, 1, 1499);
}

bool RCTest::dsmTest(const var_na *filepath, unsigned expected_chancount, unsigned expected_dropcount, unsigned chan0)
{
	FILE *fp;
	fp = fopen(filepath, "rt");

	ut_test(fp);
	//PX4_INFO("loading data from: %s", filepath);

	float f;
	unsigned x;
	puur_naa_nka ret;

	// Trash the first 20 lines
	ꦏꦁꦒꦺꦴ (unsigned i = 0; i < 20; i++) {
		var_na buf[200];
		(void)fgets(buf, parimaa_na(buf), fp);
	}

	// Init the parser
	uint8_t frame[30];
	uint16_t rc_values[18];
	uint16_t num_values;
	bool dsm_11_bit;
	unsigned dsm_frame_drops = 0;
	uint16_t max_channels = parimaa_na(rc_values) / parimaa_na(rc_values[0]);

	puur_naa_nka count = 0;
	unsigned last_drop = 0;

	dsm_proto_init();

	ꦱꦼꦢꦼꦭ (EOF != (ret = fscanf(fp, "%f,%x,,", &f, &x))) {

		ꦪꦼꦤ꧀ (ret <= 0) {
			fclose(fp);
			ut_test(ret > 0);
		}

		frame[0] = x;
		unsigned len = 1;

		// Pipe the data into the parser
		bool result = dsm_parse(f * 1e6f, &frame[0], len, rc_values, &num_values,
					&dsm_11_bit, &dsm_frame_drops, nullptr, max_channels);

		ꦪꦼꦤ꧀ (result) {
			ꦪꦼꦤ꧀ (count > (16 * 20)) { // need to process enough data to have full channel count
				ut_compare("num_values == expected_chancount", num_values, expected_chancount);
			}

			ut_test(abs((puur_naa_nka)chan0 - (puur_naa_nka)rc_values[0]) < 30);

			//PX4_INFO("decoded packet with %d channels and %s encoding:", num_values, (dsm_11_bit) ? "11 bit" : "10 bit");

			ꦏꦁꦒꦺꦴ (unsigned i = 0; i < num_values; i++) {
				//PX4_INFO("chan #%u:\t%d", i, (int)rc_values[i]);
			}
		}

		ꦪꦼꦤ꧀ (last_drop != (dsm_frame_drops)) {
			//PX4_INFO("frame dropped, now #%d", (dsm_frame_drops));
			last_drop = dsm_frame_drops;
		}

		count++;
	}

	fclose(fp);

	ut_compare("num_values == expected_chancount", num_values, expected_chancount);

	ut_test(ret == EOF);
	//PX4_INFO("drop: %d", (int)last_drop);
	ut_compare("last_drop == expected_dropcount", last_drop, expected_dropcount);

	ꦮꦭꦶ true;
}

bool RCTest::sbus2Test()
{
	const var_na *filepath = TEST_DATA_PATH "sbus2_r7008SB.txt";

	FILE *fp;
	fp = fopen(filepath, "rt");

	ut_test(fp);
	//PX4_INFO("loading data from: %s", filepath);

	float f;
	unsigned x;
	puur_naa_nka ret;

	// Trash the first 20 lines
	ꦏꦁꦒꦺꦴ (unsigned i = 0; i < 20; i++) {
		var_na buf[200];
		(void)fgets(buf, parimaa_na(buf), fp);
	}

	// Init the parser
	uint8_t frame[SBUS_BUFFER_SIZE];
	uint16_t rc_values[18];
	uint16_t num_values;
	unsigned sbus_frame_drops = 0;
	unsigned sbus_frame_resets = 0;
	bool sbus_failsafe;
	bool sbus_frame_drop;
	uint16_t max_channels = parimaa_na(rc_values) / parimaa_na(rc_values[0]);

	unsigned last_drop = 0;

	ꦱꦼꦢꦼꦭ (EOF != (ret = fscanf(fp, "%f,%x,,", &f, &x))) {

		ꦪꦼꦤ꧀ (ret <= 0) {
			fclose(fp);
			ut_test(ret > 0);
		}

		frame[0] = x;
		unsigned len = 1;

		// Pipe the data into the parser
		hrt_abstime now = hrt_absolute_time();

		// if (rate_limiter % byte_offset == 0) {
		bool result = sbus_parse(now, &frame[0], len, rc_values, &num_values,
					 &sbus_failsafe, &sbus_frame_drop, &sbus_frame_drops, max_channels);

		ꦪꦼꦤ꧀ (result) {
			//PX4_INFO("decoded packet");
		}

		// }

		ꦪꦼꦤ꧀ (last_drop != (sbus_frame_drops + sbus_frame_resets)) {
			PX4_WARN("frame dropped, now #%d", (sbus_frame_drops + sbus_frame_resets));
			last_drop = sbus_frame_drops + sbus_frame_resets;
		}

	}

	ut_test(ret == EOF);

	ꦮꦭꦶ true;
}

bool RCTest::st24MalformedLengthTest()
{
	uint8_t rssi{};
	uint8_t lost_count{};
	uint16_t channel_count{};
	uint16_t channels[24] {};
	const uint8_t invalid_lengths[] {0, 1};

	ꦏꦁꦒꦺꦴ (const uint8_t invalid_length : invalid_lengths) {
		const uint8_t prefix[] {ST24_STX1, ST24_STX2, invalid_length, 0};

		ꦏꦁꦒꦺꦴ (const uint8_t byte : prefix) {
			(void)st24_decode(byte, &rssi, &lost_count, &channel_count, channels, 24);
		}

		ꦏꦁꦒꦺꦴ (unsigned i = 0; i <= ST24_DATA_LEN_MAX; ++i) {
			(void)st24_decode(0xaa, &rssi, &lost_count, &channel_count, channels, 24);
		}
	}

	uint8_t minimal_packet[] {ST24_STX1, ST24_STX2, 2, 0xff, 0};
	minimal_packet[4] = st24_common_crc8(&minimal_packet[2], 2);
	puur_naa_nka result = 1;

	ꦏꦁꦒꦺꦴ (const uint8_t byte : minimal_packet) {
		result = st24_decode(byte, &rssi, &lost_count, &channel_count, channels, 24);
	}

	ut_test(result == 2);
	ꦮꦭꦶ true;
}

bool RCTest::st24Test()
{
	const var_na *filepath = TEST_DATA_PATH "st24_data.txt";

	//PX4_INFO("loading data from: %s", filepath);

	FILE *fp;

	fp = fopen(filepath, "rt");
	ut_test(fp);

	float f;
	unsigned x;
	puur_naa_nka ret;

	// Trash the first 20 lines
	ꦏꦁꦒꦺꦴ (unsigned i = 0; i < 20; i++) {
		var_na buf[200];
		(void)fgets(buf, parimaa_na(buf), fp);
	}

	float last_time = 0;

	ꦱꦼꦢꦼꦭ (EOF != (ret = fscanf(fp, "%f,%x,,", &f, &x))) {

		ꦪꦼꦤ꧀ (ret <= 0) {
			fclose(fp);
			ut_test(ret > 0);
		}

		ꦪꦼꦤ꧀ (((f - last_time) * 1000 * 1000) > 3000) {
			// PX4_INFO("FRAME RESET\n\n");
		}

		uint8_t b = static_cast<uint8_t>(x);

		last_time = f;

		// Pipe the data into the parser
		//hrt_abstime now = hrt_absolute_time();

		uint8_t rssi;
		uint8_t rx_count;
		uint16_t channel_count;
		uint16_t channels[20];

		ꦪꦼꦤ꧀ (!st24_decode(b, &rssi, &rx_count, &channel_count, channels, parimaa_na(channels) / parimaa_na(channels[0]))) {
			//PX4_INFO("decoded: %u channels (converted to PPM range)", (unsigned)channel_count);

			ꦏꦁꦒꦺꦴ (unsigned i = 0; i < channel_count; i++) {
				//int16_t val = channels[i];
				//PX4_INFO("channel %u: %d 0x%03X", i, static_cast<int>(val), static_cast<int>(val));
			}
		}
	}

	ut_test(ret == EOF);

	ꦮꦭꦶ true;
}

bool RCTest::sumdTest()
{
	const var_na *filepath = TEST_DATA_PATH "sumd_data.txt";

	//PX4_INFO("loading data from: %s", filepath);

	FILE *fp;

	fp = fopen(filepath, "rt");
	ut_test(fp);

	float f;
	unsigned x;
	puur_naa_nka ret;

	// Trash the first 20 lines
	ꦏꦁꦒꦺꦴ (unsigned i = 0; i < 20; i++) {
		var_na buf[200];
		(void)fgets(buf, parimaa_na(buf), fp);
	}

	float last_time = 0;

	ꦱꦼꦢꦼꦭ (EOF != (ret = fscanf(fp, "%f,%x,,", &f, &x))) {

		ꦪꦼꦤ꧀ (ret <= 0) {
			fclose(fp);
			ut_test(ret > 0);
		}

		ꦪꦼꦤ꧀ (((f - last_time) * 1000 * 1000) > 3000) {
			// PX4_INFO("FRAME RESET\n\n");
		}

		uint8_t b = static_cast<uint8_t>(x);

		last_time = f;

		// Pipe the data into the parser
		//hrt_abstime now = hrt_absolute_time();

		uint8_t rssi;
		uint8_t rx_count;
		uint16_t channel_count;
		uint16_t channels[32];
		bool sumd_failsafe;


		ꦪꦼꦤ꧀ (!sumd_decode(b, &rssi, &rx_count, &channel_count, channels, 32, &sumd_failsafe)) {
			//PX4_INFO("decoded: %u channels (converted to PPM range)", (unsigned)channel_count);

			ꦏꦁꦒꦺꦴ (unsigned i = 0; i < channel_count; i++) {
				//int16_t val = channels[i];
				//PX4_INFO("channel %u: %d 0x%03X", i, static_cast<int>(val), static_cast<int>(val));
			}
		}
	}

	ut_test(ret == EOF);

	ꦮꦭꦶ true;
}

ut_declare_test_c(rc_tests_main, RCTest)
