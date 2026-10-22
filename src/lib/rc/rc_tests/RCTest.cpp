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

#གལ་སྲིད defined(CONFIG_ARCH_BOARD_PX4_SITL)
#define TEST_DATA_PATH "./test_data/"
#དེ་མིན
#define TEST_DATA_PATH CONFIG_BOARD_ROOT_PATH
#endif

extern "C" __EXPORT ཧྲིལ rc_tests_main(ཧྲིལ argc, var_na *argv[]);

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

	ལོག (_tests_failed == 0);
}

bool RCTest::crsfTest()
{
	const var_na *filepath = TEST_DATA_PATH "crsf_rc_channels.txt";

	FILE *fp = fopen(filepath, "rt");

	ut_test(fp);
	//PX4_INFO("loading data from: %s", filepath);

	const ཧྲིལ line_size = 500;
	var_na line[line_size];
	bool has_decoded_values = false;
	const ཧྲིལ max_channels = 16;
	uint16_t rc_values[max_channels];
	uint16_t num_values = 0;
	ཧྲིལ line_counter = 1;

	སྐབས (fgets(line, line_size, fp) != nullptr)  {

		གལ་སྲིད (strncmp(line, "INPUT ", 6) == 0) {

			གལ་སྲིད (has_decoded_values) {
				PX4_ERR("Parser decoded values that are not in the test file (line=%i)", line_counter);
				ལོག false;
			}

			// read the values
			const var_na *file_buffer = line + 6;
			ཧྲིལ frame_len = 0;
			uint8_t frame[300];
			ཧྲིལ offset;
			ཧྲིལ number;

			སྐབས (sscanf(file_buffer, "%x, %n", &number, &offset) > 0) {
				frame[frame_len++] = number;
				file_buffer += offset;
			}

			// Pipe the data into the parser
			hrt_abstime now = hrt_absolute_time();

			bool result = crsf_parse(now, frame, frame_len, rc_values, &num_values, max_channels);

			གལ་སྲིད (result) {
				has_decoded_values = true;
			}

		} དེ་མིན གལ་སྲིད (strncmp(line, "DECODED ", 8) == 0) {

			གལ་སྲིད (!has_decoded_values) {
				PX4_ERR("Test file contains decoded values but the parser did not decode anything (line=%i)", line_counter);
				ལོག false;
			}

			// read the values
			const var_na *file_buffer = line + 8;
			ཧྲིལ offset;
			ཧྲིལ expected_rc_value;
			ཧྲིལ expected_num_channels = 0;

			སྐབས (sscanf(file_buffer, "%x, %n", &expected_rc_value, &offset) > 0) {

				// allow a small difference
				གལ་སྲིད (abs(expected_rc_value - (ཧྲིལ)rc_values[expected_num_channels]) > 10) {
					PX4_ERR("File line: %i, channel: %i", line_counter, expected_num_channels);
					ut_compare("Wrong decoded channel", expected_rc_value, rc_values[expected_num_channels]);
				}

				file_buffer += offset;
				++expected_num_channels;
			}

			གལ་སྲིད (expected_num_channels != num_values) {
				PX4_ERR("File line: %d", line_counter);
				ut_compare("Unexpected number of decoded channels", expected_num_channels, num_values);
			}

			has_decoded_values = false;
		}

		++line_counter;
	}

	ལོག true;
}

bool RCTest::ghstTest()
{
	const var_na *filepath = TEST_DATA_PATH "ghst_rc_channels.txt";

	FILE *fp = fopen(filepath, "rt");

	ut_test(fp);

	ཧྲིལ uart_fd = -1;
	const ཧྲིལ line_size = 500;
	var_na line[line_size];
	bool has_decoded_values = false;
	const ཧྲིལ max_channels = 16;
	uint16_t rc_values[max_channels];
	uint16_t num_values = 0;
	ཧྲིལ line_counter = 1;
	ghstLinkStatistics_t link_stats;
	ghst_config(uart_fd);

	སྐབས (fgets(line, line_size, fp) != nullptr)  {

		གལ་སྲིད (strncmp(line, "INPUT ", 6) == 0) {

			གལ་སྲིད (has_decoded_values) {
				PX4_ERR("Parser decoded values that are not in the test file (line=%i)", line_counter);
				ལོག false;
			}

			// read the values
			const var_na *file_buffer = line + 6;
			ཧྲིལ frame_len = 0;
			uint8_t frame[300];
			ཧྲིལ offset;
			ཧྲིལ number;

			སྐབས (sscanf(file_buffer, "%x, %n", &number, &offset) > 0) {
				frame[frame_len++] = number;
				file_buffer += offset;
			}

			// Pipe the data into the parser
			hrt_abstime now = hrt_absolute_time();

			bool result = ghst_parse(now, frame, frame_len, rc_values, &link_stats, &num_values, max_channels);

			གལ་སྲིད (result) {
				has_decoded_values = true;
			}

		} དེ་མིན གལ་སྲིད (strncmp(line, "DECODED ", 8) == 0) {

			གལ་སྲིད (!has_decoded_values) {
				PX4_ERR("Test file contains decoded values but the parser did not decode anything (line=%i)", line_counter);
				ལོག false;
			}

			// read the values
			const var_na *file_buffer = line + 8;
			ཧྲིལ offset;
			ཧྲིལ expected_rc_value;
			ཧྲིལ expected_num_channels = 0;

			སྐབས (sscanf(file_buffer, "%x, %n", &expected_rc_value, &offset) > 0) {

				// allow a small difference
				གལ་སྲིད (abs(expected_rc_value - (ཧྲིལ)rc_values[expected_num_channels]) > 10) {
					PX4_ERR("File line: %i, channel: %i", line_counter, expected_num_channels);
					ut_compare("Wrong decoded channel", expected_rc_value, rc_values[expected_num_channels]);
				}

				file_buffer += offset;
				++expected_num_channels;
			}

			གལ་སྲིད (expected_num_channels != num_values) {
				PX4_ERR("File line: %d", line_counter);
				ut_compare("Unexpected number of decoded channels", expected_num_channels, num_values);
			}

			has_decoded_values = false;
		}

		++line_counter;
	}

	ལོག true;
}

bool RCTest::dsmTest10Ch()
{
	ལོག dsmTest(TEST_DATA_PATH "dsm_x_data.txt", 10, 2, 1500);
}

bool RCTest::dsmTest16Ch()
{
	ལོག dsmTest(TEST_DATA_PATH "dsm_x_dx9_data.txt", 16, 1, 1500);
}

bool RCTest::dsmTest22msDSMX16Ch()
{
	ལོག dsmTest(TEST_DATA_PATH "dsm_x_dx9_px4_binding_data.txt", 16, 1, 1499);
}

bool RCTest::dsmTestOrangeDsmx()
{
	ལོག dsmTest(TEST_DATA_PATH "orangerx_dsmx_12.txt", 12, 1, 1499);
}

bool RCTest::dsmTest(const var_na *filepath, unsigned expected_chancount, unsigned expected_dropcount, unsigned chan0)
{
	FILE *fp;
	fp = fopen(filepath, "rt");

	ut_test(fp);
	//PX4_INFO("loading data from: %s", filepath);

	float f;
	unsigned x;
	ཧྲིལ ret;

	// Trash the first 20 lines
	དོན་ལུ (unsigned i = 0; i < 20; i++) {
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

	ཧྲིལ count = 0;
	unsigned last_drop = 0;

	dsm_proto_init();

	སྐབས (EOF != (ret = fscanf(fp, "%f,%x,,", &f, &x))) {

		གལ་སྲིད (ret <= 0) {
			fclose(fp);
			ut_test(ret > 0);
		}

		frame[0] = x;
		unsigned len = 1;

		// Pipe the data into the parser
		bool result = dsm_parse(f * 1e6f, &frame[0], len, rc_values, &num_values,
					&dsm_11_bit, &dsm_frame_drops, nullptr, max_channels);

		གལ་སྲིད (result) {
			གལ་སྲིད (count > (16 * 20)) { // need to process enough data to have full channel count
				ut_compare("num_values == expected_chancount", num_values, expected_chancount);
			}

			ut_test(abs((ཧྲིལ)chan0 - (ཧྲིལ)rc_values[0]) < 30);

			//PX4_INFO("decoded packet with %d channels and %s encoding:", num_values, (dsm_11_bit) ? "11 bit" : "10 bit");

			དོན་ལུ (unsigned i = 0; i < num_values; i++) {
				//PX4_INFO("chan #%u:\t%d", i, (int)rc_values[i]);
			}
		}

		གལ་སྲིད (last_drop != (dsm_frame_drops)) {
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

	ལོག true;
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
	ཧྲིལ ret;

	// Trash the first 20 lines
	དོན་ལུ (unsigned i = 0; i < 20; i++) {
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

	སྐབས (EOF != (ret = fscanf(fp, "%f,%x,,", &f, &x))) {

		གལ་སྲིད (ret <= 0) {
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

		གལ་སྲིད (result) {
			//PX4_INFO("decoded packet");
		}

		// }

		གལ་སྲིད (last_drop != (sbus_frame_drops + sbus_frame_resets)) {
			PX4_WARN("frame dropped, now #%d", (sbus_frame_drops + sbus_frame_resets));
			last_drop = sbus_frame_drops + sbus_frame_resets;
		}

	}

	ut_test(ret == EOF);

	ལོག true;
}

bool RCTest::st24MalformedLengthTest()
{
	uint8_t rssi{};
	uint8_t lost_count{};
	uint16_t channel_count{};
	uint16_t channels[24] {};
	const uint8_t invalid_lengths[] {0, 1};

	དོན་ལུ (const uint8_t invalid_length : invalid_lengths) {
		const uint8_t prefix[] {ST24_STX1, ST24_STX2, invalid_length, 0};

		དོན་ལུ (const uint8_t byte : prefix) {
			(void)st24_decode(byte, &rssi, &lost_count, &channel_count, channels, 24);
		}

		དོན་ལུ (unsigned i = 0; i <= ST24_DATA_LEN_MAX; ++i) {
			(void)st24_decode(0xaa, &rssi, &lost_count, &channel_count, channels, 24);
		}
	}

	uint8_t minimal_packet[] {ST24_STX1, ST24_STX2, 2, 0xff, 0};
	minimal_packet[4] = st24_common_crc8(&minimal_packet[2], 2);
	ཧྲིལ result = 1;

	དོན་ལུ (const uint8_t byte : minimal_packet) {
		result = st24_decode(byte, &rssi, &lost_count, &channel_count, channels, 24);
	}

	ut_test(result == 2);
	ལོག true;
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
	ཧྲིལ ret;

	// Trash the first 20 lines
	དོན་ལུ (unsigned i = 0; i < 20; i++) {
		var_na buf[200];
		(void)fgets(buf, parimaa_na(buf), fp);
	}

	float last_time = 0;

	སྐབས (EOF != (ret = fscanf(fp, "%f,%x,,", &f, &x))) {

		གལ་སྲིད (ret <= 0) {
			fclose(fp);
			ut_test(ret > 0);
		}

		གལ་སྲིད (((f - last_time) * 1000 * 1000) > 3000) {
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

		གལ་སྲིད (!st24_decode(b, &rssi, &rx_count, &channel_count, channels, parimaa_na(channels) / parimaa_na(channels[0]))) {
			//PX4_INFO("decoded: %u channels (converted to PPM range)", (unsigned)channel_count);

			དོན་ལུ (unsigned i = 0; i < channel_count; i++) {
				//int16_t val = channels[i];
				//PX4_INFO("channel %u: %d 0x%03X", i, static_cast<int>(val), static_cast<int>(val));
			}
		}
	}

	ut_test(ret == EOF);

	ལོག true;
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
	ཧྲིལ ret;

	// Trash the first 20 lines
	དོན་ལུ (unsigned i = 0; i < 20; i++) {
		var_na buf[200];
		(void)fgets(buf, parimaa_na(buf), fp);
	}

	float last_time = 0;

	སྐབས (EOF != (ret = fscanf(fp, "%f,%x,,", &f, &x))) {

		གལ་སྲིད (ret <= 0) {
			fclose(fp);
			ut_test(ret > 0);
		}

		གལ་སྲིད (((f - last_time) * 1000 * 1000) > 3000) {
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


		གལ་སྲིད (!sumd_decode(b, &rssi, &rx_count, &channel_count, channels, 32, &sumd_failsafe)) {
			//PX4_INFO("decoded: %u channels (converted to PPM range)", (unsigned)channel_count);

			དོན་ལུ (unsigned i = 0; i < channel_count; i++) {
				//int16_t val = channels[i];
				//PX4_INFO("channel %u: %d 0x%03X", i, static_cast<int>(val), static_cast<int>(val));
			}
		}
	}

	ut_test(ret == EOF);

	ལོག true;
}

ut_declare_test_c(rc_tests_main, RCTest)
