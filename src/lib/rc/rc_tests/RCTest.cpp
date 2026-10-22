#ਅਨ੍ਤਰ੍ਭਾਵ <unit_test.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <systemlib/err.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <string.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <drivers/drv_hrt.h>

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DSM_DEBUG
#ਅਨ੍ਤਰ੍ਭਾਵ <lib/rc/sbus.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <lib/rc/dsm.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <lib/rc/st24.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <lib/rc/sumd.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <lib/rc/crsf.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <lib/rc/ghst.hpp>

#ਜੇ defined(CONFIG_ARCH_BOARD_PX4_SITL)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TEST_ਅੰਕੜੇ_ਰਸਤਾ "./test_data/"
#ਨਹੀਂਤਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TEST_ਅੰਕੜੇ_ਰਸਤਾ CONFIG_BOARD_ਜੜ੍ਹ_ਰਸਤਾ
#ਜੇ_ਅੰਤ

ਬਾਹਰੀ "C" __EXPORT ਪੂਰਨ rc_tests_main(ਪੂਰਨ argc, ਅੱਖਰ *argv[]);

ਵਰਗ RCTest : ਜਨਤਕ UnitTest
{
ਜਨਤਕ:
	ਤਾਰਕਿਕ ਲੜੀਵਾਰ_tests() override;

ਨਿਜੀ:
	ਤਾਰਕਿਕ crsfTest();
	ਤਾਰਕਿਕ ghstTest();
	ਤਾਰਕਿਕ dsmTest(ਅਚਲ ਅੱਖਰ *filepath, ਬਿਨਾਨਿਸ਼ਾਨੀ expected_chancount, ਬਿਨਾਨਿਸ਼ਾਨੀ expected_dropcount, ਬਿਨਾਨਿਸ਼ਾਨੀ chan0);
	ਤਾਰਕਿਕ dsmTest10Ch();
	ਤਾਰਕਿਕ dsmTest16Ch();
	ਤਾਰਕਿਕ dsmTest22msDSMX16Ch();
	ਤਾਰਕਿਕ dsmTestOrangeDsmx();
	ਤਾਰਕਿਕ sbus2Test();
	ਤਾਰਕਿਕ st24_Malformed_ਲੰਮਾਈ_Test();
	ਤਾਰਕਿਕ st24Test();
	ਤਾਰਕਿਕ sumdTest();
};

ਤਾਰਕਿਕ RCTest::ਲੜੀਵਾਰ_tests()
{
	ut_ਲੜੀਵਾਰ_test(crsfTest);
	ut_ਲੜੀਵਾਰ_test(ghstTest);
	ut_ਲੜੀਵਾਰ_test(dsmTest10Ch);
	ut_ਲੜੀਵਾਰ_test(dsmTest16Ch);
	ut_ਲੜੀਵਾਰ_test(dsmTest22msDSMX16Ch);
	ut_ਲੜੀਵਾਰ_test(dsmTestOrangeDsmx);
	ut_ਲੜੀਵਾਰ_test(sbus2Test);
	ut_ਲੜੀਵਾਰ_test(st24_Malformed_ਲੰਮਾਈ_Test);
	ut_ਲੜੀਵਾਰ_test(st24Test);
	ut_ਲੜੀਵਾਰ_test(sumdTest);

	ਵਾਪਸ (_tests_failed == 0);
}

ਤਾਰਕਿਕ RCTest::crsfTest()
{
	ਅਚਲ ਅੱਖਰ *filepath = TEST_ਅੰਕੜੇ_ਰਸਤਾ "crsf_rc_channels.txt";

	FILE *fp = fopen(filepath, "rt");

	ut_test(fp);
	//PX4_INFO("loading data from: %s", filepath);

	ਅਚਲ ਪੂਰਨ ਸਤਰ_ਮਾਪ = 500;
	ਅੱਖਰ ਸਤਰ[ਸਤਰ_ਮਾਪ];
	ਤਾਰਕਿਕ has_decoded_ਮੁੱਲਾਂ = ਝੂਠ;
	ਅਚਲ ਪੂਰਨ max_channels = 16;
	uint16_t rc_ਮੁੱਲਾਂ[max_channels];
	uint16_t ਅੰਕ_ਮੁੱਲਾਂ = 0;
	ਪੂਰਨ ਸਤਰ_counter = 1;

	ਜਦੋਂ (fgets(ਸਤਰ, ਸਤਰ_ਮਾਪ, fp) != ਖਾਲੀ_ਸੂਚਕ)  {

		ਜੇ (ਲੜੀ_ਤੁਲਨਾ(ਸਤਰ, "INPUT ", 6) == 0) {

			ਜੇ (has_decoded_ਮੁੱਲਾਂ) {
				PX4_ERR("Parser decoded values that are not in the test file (line=%i)", ਸਤਰ_counter);
				ਵਾਪਸ ਝੂਠ;
			}

			// read the values
			ਅਚਲ ਅੱਖਰ *file_ਬਫਰ = ਸਤਰ + 6;
			ਪੂਰਨ ਚੌਖਟਾ_len = 0;
			uint8_t ਚੌਖਟਾ[300];
			ਪੂਰਨ ਵਿੱਥ;
			ਪੂਰਨ ਸੰਖਿਆ;

			ਜਦੋਂ (ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ(file_ਬਫਰ, "%x, %n", &ਸੰਖਿਆ, &ਵਿੱਥ) > 0) {
				ਚੌਖਟਾ[ਚੌਖਟਾ_len++] = ਸੰਖਿਆ;
				file_ਬਫਰ += ਵਿੱਥ;
			}

			// Pipe the data into the parser
			hrt_abstime now = hrt_absolute_ਸਮਾਂ();

			ਤਾਰਕਿਕ ਨਤੀਜਾ = crsf_parse(now, ਚੌਖਟਾ, ਚੌਖਟਾ_len, rc_ਮੁੱਲਾਂ, &ਅੰਕ_ਮੁੱਲਾਂ, max_channels);

			ਜੇ (ਨਤੀਜਾ) {
				has_decoded_ਮੁੱਲਾਂ = ਸੱਚ;
			}

		} ਨਹੀਂਤਾਂ ਜੇ (ਲੜੀ_ਤੁਲਨਾ(ਸਤਰ, "DECODED ", 8) == 0) {

			ਜੇ (!has_decoded_ਮੁੱਲਾਂ) {
				PX4_ERR("Test file contains decoded values but the parser did not decode anything (line=%i)", ਸਤਰ_counter);
				ਵਾਪਸ ਝੂਠ;
			}

			// read the values
			ਅਚਲ ਅੱਖਰ *file_ਬਫਰ = ਸਤਰ + 8;
			ਪੂਰਨ ਵਿੱਥ;
			ਪੂਰਨ expected_rc_ਮੁੱਲ;
			ਪੂਰਨ expected_ਅੰਕ_channels = 0;

			ਜਦੋਂ (ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ(file_ਬਫਰ, "%x, %n", &expected_rc_ਮੁੱਲ, &ਵਿੱਥ) > 0) {

				// allow a small difference
				ਜੇ (ਨਿਰਪੇਖ(expected_rc_ਮੁੱਲ - (ਪੂਰਨ)rc_ਮੁੱਲਾਂ[expected_ਅੰਕ_channels]) > 10) {
					PX4_ERR("File line: %i, channel: %i", ਸਤਰ_counter, expected_ਅੰਕ_channels);
					ut_compare("Wrong decoded channel", expected_rc_ਮੁੱਲ, rc_ਮੁੱਲਾਂ[expected_ਅੰਕ_channels]);
				}

				file_ਬਫਰ += ਵਿੱਥ;
				++expected_ਅੰਕ_channels;
			}

			ਜੇ (expected_ਅੰਕ_channels != ਅੰਕ_ਮੁੱਲਾਂ) {
				PX4_ERR("File line: %d", ਸਤਰ_counter);
				ut_compare("Unexpected number of decoded channels", expected_ਅੰਕ_channels, ਅੰਕ_ਮੁੱਲਾਂ);
			}

			has_decoded_ਮੁੱਲਾਂ = ਝੂਠ;
		}

		++ਸਤਰ_counter;
	}

	ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ RCTest::ghstTest()
{
	ਅਚਲ ਅੱਖਰ *filepath = TEST_ਅੰਕੜੇ_ਰਸਤਾ "ghst_rc_channels.txt";

	FILE *fp = fopen(filepath, "rt");

	ut_test(fp);

	ਪੂਰਨ uart_fd = -1;
	ਅਚਲ ਪੂਰਨ ਸਤਰ_ਮਾਪ = 500;
	ਅੱਖਰ ਸਤਰ[ਸਤਰ_ਮਾਪ];
	ਤਾਰਕਿਕ has_decoded_ਮੁੱਲਾਂ = ਝੂਠ;
	ਅਚਲ ਪੂਰਨ max_channels = 16;
	uint16_t rc_ਮੁੱਲਾਂ[max_channels];
	uint16_t ਅੰਕ_ਮੁੱਲਾਂ = 0;
	ਪੂਰਨ ਸਤਰ_counter = 1;
	ghstLinkStatistics_t link_stats;
	ghst_config(uart_fd);

	ਜਦੋਂ (fgets(ਸਤਰ, ਸਤਰ_ਮਾਪ, fp) != ਖਾਲੀ_ਸੂਚਕ)  {

		ਜੇ (ਲੜੀ_ਤੁਲਨਾ(ਸਤਰ, "INPUT ", 6) == 0) {

			ਜੇ (has_decoded_ਮੁੱਲਾਂ) {
				PX4_ERR("Parser decoded values that are not in the test file (line=%i)", ਸਤਰ_counter);
				ਵਾਪਸ ਝੂਠ;
			}

			// read the values
			ਅਚਲ ਅੱਖਰ *file_ਬਫਰ = ਸਤਰ + 6;
			ਪੂਰਨ ਚੌਖਟਾ_len = 0;
			uint8_t ਚੌਖਟਾ[300];
			ਪੂਰਨ ਵਿੱਥ;
			ਪੂਰਨ ਸੰਖਿਆ;

			ਜਦੋਂ (ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ(file_ਬਫਰ, "%x, %n", &ਸੰਖਿਆ, &ਵਿੱਥ) > 0) {
				ਚੌਖਟਾ[ਚੌਖਟਾ_len++] = ਸੰਖਿਆ;
				file_ਬਫਰ += ਵਿੱਥ;
			}

			// Pipe the data into the parser
			hrt_abstime now = hrt_absolute_ਸਮਾਂ();

			ਤਾਰਕਿਕ ਨਤੀਜਾ = ghst_parse(now, ਚੌਖਟਾ, ਚੌਖਟਾ_len, rc_ਮੁੱਲਾਂ, &link_stats, &ਅੰਕ_ਮੁੱਲਾਂ, max_channels);

			ਜੇ (ਨਤੀਜਾ) {
				has_decoded_ਮੁੱਲਾਂ = ਸੱਚ;
			}

		} ਨਹੀਂਤਾਂ ਜੇ (ਲੜੀ_ਤੁਲਨਾ(ਸਤਰ, "DECODED ", 8) == 0) {

			ਜੇ (!has_decoded_ਮੁੱਲਾਂ) {
				PX4_ERR("Test file contains decoded values but the parser did not decode anything (line=%i)", ਸਤਰ_counter);
				ਵਾਪਸ ਝੂਠ;
			}

			// read the values
			ਅਚਲ ਅੱਖਰ *file_ਬਫਰ = ਸਤਰ + 8;
			ਪੂਰਨ ਵਿੱਥ;
			ਪੂਰਨ expected_rc_ਮੁੱਲ;
			ਪੂਰਨ expected_ਅੰਕ_channels = 0;

			ਜਦੋਂ (ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ(file_ਬਫਰ, "%x, %n", &expected_rc_ਮੁੱਲ, &ਵਿੱਥ) > 0) {

				// allow a small difference
				ਜੇ (ਨਿਰਪੇਖ(expected_rc_ਮੁੱਲ - (ਪੂਰਨ)rc_ਮੁੱਲਾਂ[expected_ਅੰਕ_channels]) > 10) {
					PX4_ERR("File line: %i, channel: %i", ਸਤਰ_counter, expected_ਅੰਕ_channels);
					ut_compare("Wrong decoded channel", expected_rc_ਮੁੱਲ, rc_ਮੁੱਲਾਂ[expected_ਅੰਕ_channels]);
				}

				file_ਬਫਰ += ਵਿੱਥ;
				++expected_ਅੰਕ_channels;
			}

			ਜੇ (expected_ਅੰਕ_channels != ਅੰਕ_ਮੁੱਲਾਂ) {
				PX4_ERR("File line: %d", ਸਤਰ_counter);
				ut_compare("Unexpected number of decoded channels", expected_ਅੰਕ_channels, ਅੰਕ_ਮੁੱਲਾਂ);
			}

			has_decoded_ਮੁੱਲਾਂ = ਝੂਠ;
		}

		++ਸਤਰ_counter;
	}

	ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ RCTest::dsmTest10Ch()
{
	ਵਾਪਸ dsmTest(TEST_ਅੰਕੜੇ_ਰਸਤਾ "dsm_x_data.txt", 10, 2, 1500);
}

ਤਾਰਕਿਕ RCTest::dsmTest16Ch()
{
	ਵਾਪਸ dsmTest(TEST_ਅੰਕੜੇ_ਰਸਤਾ "dsm_x_dx9_data.txt", 16, 1, 1500);
}

ਤਾਰਕਿਕ RCTest::dsmTest22msDSMX16Ch()
{
	ਵਾਪਸ dsmTest(TEST_ਅੰਕੜੇ_ਰਸਤਾ "dsm_x_dx9_px4_binding_data.txt", 16, 1, 1499);
}

ਤਾਰਕਿਕ RCTest::dsmTestOrangeDsmx()
{
	ਵਾਪਸ dsmTest(TEST_ਅੰਕੜੇ_ਰਸਤਾ "orangerx_dsmx_12.txt", 12, 1, 1499);
}

ਤਾਰਕਿਕ RCTest::dsmTest(ਅਚਲ ਅੱਖਰ *filepath, ਬਿਨਾਨਿਸ਼ਾਨੀ expected_chancount, ਬਿਨਾਨਿਸ਼ਾਨੀ expected_dropcount, ਬਿਨਾਨਿਸ਼ਾਨੀ chan0)
{
	FILE *fp;
	fp = fopen(filepath, "rt");

	ut_test(fp);
	//PX4_INFO("loading data from: %s", filepath);

	ਦਸ਼ਮਲਵ f;
	ਬਿਨਾਨਿਸ਼ਾਨੀ x;
	ਪੂਰਨ ret;

	// Trash the first 20 lines
	ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < 20; i++) {
		ਅੱਖਰ buf[200];
		(ਖਾਲੀ)fgets(buf, ਆਕਾਰ(buf), fp);
	}

	// Init the parser
	uint8_t ਚੌਖਟਾ[30];
	uint16_t rc_ਮੁੱਲਾਂ[18];
	uint16_t ਅੰਕ_ਮੁੱਲਾਂ;
	ਤਾਰਕਿਕ dsm_11_ਦੋਆਧਾਰੀ_ਅੰਕ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ dsm_ਚੌਖਟਾ_drops = 0;
	uint16_t max_channels = ਆਕਾਰ(rc_ਮੁੱਲਾਂ) / ਆਕਾਰ(rc_ਮੁੱਲਾਂ[0]);

	ਪੂਰਨ ਗਿਣਤੀ_2 = 0;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਆਖਰੀ_drop = 0;

	dsm_proto_init();

	ਜਦੋਂ (EOF != (ret = fscanf(fp, "%f,%x,,", &f, &x))) {

		ਜੇ (ret <= 0) {
			fclose(fp);
			ut_test(ret > 0);
		}

		ਚੌਖਟਾ[0] = x;
		ਬਿਨਾਨਿਸ਼ਾਨੀ len = 1;

		// Pipe the data into the parser
		ਤਾਰਕਿਕ ਨਤੀਜਾ = dsm_parse(f * 1e6f, &ਚੌਖਟਾ[0], len, rc_ਮੁੱਲਾਂ, &ਅੰਕ_ਮੁੱਲਾਂ,
					&dsm_11_ਦੋਆਧਾਰੀ_ਅੰਕ, &dsm_ਚੌਖਟਾ_drops, ਖਾਲੀ_ਸੂਚਕ, max_channels);

		ਜੇ (ਨਤੀਜਾ) {
			ਜੇ (ਗਿਣਤੀ_2 > (16 * 20)) { // need to process enough data to have full channel count
				ut_compare("num_values == expected_chancount", ਅੰਕ_ਮੁੱਲਾਂ, expected_chancount);
			}

			ut_test(ਨਿਰਪੇਖ((ਪੂਰਨ)chan0 - (ਪੂਰਨ)rc_ਮੁੱਲਾਂ[0]) < 30);

			//PX4_INFO("decoded packet with %d channels and %s encoding:", num_values, (dsm_11_bit) ? "11 bit" : "10 bit");

			ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < ਅੰਕ_ਮੁੱਲਾਂ; i++) {
				//PX4_INFO("chan #%u:\t%d", i, (int)rc_values[i]);
			}
		}

		ਜੇ (ਆਖਰੀ_drop != (dsm_ਚੌਖਟਾ_drops)) {
			//PX4_INFO("frame dropped, now #%d", (dsm_frame_drops));
			ਆਖਰੀ_drop = dsm_ਚੌਖਟਾ_drops;
		}

		ਗਿਣਤੀ_2++;
	}

	fclose(fp);

	ut_compare("num_values == expected_chancount", ਅੰਕ_ਮੁੱਲਾਂ, expected_chancount);

	ut_test(ret == EOF);
	//PX4_INFO("drop: %d", (int)last_drop);
	ut_compare("last_drop == expected_dropcount", ਆਖਰੀ_drop, expected_dropcount);

	ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ RCTest::sbus2Test()
{
	ਅਚਲ ਅੱਖਰ *filepath = TEST_ਅੰਕੜੇ_ਰਸਤਾ "sbus2_r7008SB.txt";

	FILE *fp;
	fp = fopen(filepath, "rt");

	ut_test(fp);
	//PX4_INFO("loading data from: %s", filepath);

	ਦਸ਼ਮਲਵ f;
	ਬਿਨਾਨਿਸ਼ਾਨੀ x;
	ਪੂਰਨ ret;

	// Trash the first 20 lines
	ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < 20; i++) {
		ਅੱਖਰ buf[200];
		(ਖਾਲੀ)fgets(buf, ਆਕਾਰ(buf), fp);
	}

	// Init the parser
	uint8_t ਚੌਖਟਾ[SBUS_ਬਫਰ_ਮਾਪ];
	uint16_t rc_ਮੁੱਲਾਂ[18];
	uint16_t ਅੰਕ_ਮੁੱਲਾਂ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ sbus_ਚੌਖਟਾ_drops = 0;
	ਬਿਨਾਨਿਸ਼ਾਨੀ sbus_ਚੌਖਟਾ_resets = 0;
	ਤਾਰਕਿਕ sbus_failsafe;
	ਤਾਰਕਿਕ sbus_ਚੌਖਟਾ_drop;
	uint16_t max_channels = ਆਕਾਰ(rc_ਮੁੱਲਾਂ) / ਆਕਾਰ(rc_ਮੁੱਲਾਂ[0]);

	ਬਿਨਾਨਿਸ਼ਾਨੀ ਆਖਰੀ_drop = 0;

	ਜਦੋਂ (EOF != (ret = fscanf(fp, "%f,%x,,", &f, &x))) {

		ਜੇ (ret <= 0) {
			fclose(fp);
			ut_test(ret > 0);
		}

		ਚੌਖਟਾ[0] = x;
		ਬਿਨਾਨਿਸ਼ਾਨੀ len = 1;

		// Pipe the data into the parser
		hrt_abstime now = hrt_absolute_ਸਮਾਂ();

		// if (rate_limiter % byte_offset == 0) {
		ਤਾਰਕਿਕ ਨਤੀਜਾ = sbus_parse(now, &ਚੌਖਟਾ[0], len, rc_ਮੁੱਲਾਂ, &ਅੰਕ_ਮੁੱਲਾਂ,
					 &sbus_failsafe, &sbus_ਚੌਖਟਾ_drop, &sbus_ਚੌਖਟਾ_drops, max_channels);

		ਜੇ (ਨਤੀਜਾ) {
			//PX4_INFO("decoded packet");
		}

		// }

		ਜੇ (ਆਖਰੀ_drop != (sbus_ਚੌਖਟਾ_drops + sbus_ਚੌਖਟਾ_resets)) {
			PX4_WARN("frame dropped, now #%d", (sbus_ਚੌਖਟਾ_drops + sbus_ਚੌਖਟਾ_resets));
			ਆਖਰੀ_drop = sbus_ਚੌਖਟਾ_drops + sbus_ਚੌਖਟਾ_resets;
		}

	}

	ut_test(ret == EOF);

	ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ RCTest::st24_Malformed_ਲੰਮਾਈ_Test()
{
	uint8_t rssi{};
	uint8_t ਗੁਆਚਾ_ਗਿਣਤੀ{};
	uint16_t channel_ਗਿਣਤੀ{};
	uint16_t channels[24] {};
	ਅਚਲ uint8_t invalid_lengths[] {0, 1};

	ਲਈ (ਅਚਲ uint8_t invalid_ਲੰਮਾਈ : invalid_lengths) {
		ਅਚਲ uint8_t ਅਗੇਤਰ[] {ST24_STX1, ST24_STX2, invalid_ਲੰਮਾਈ, 0};

		ਲਈ (ਅਚਲ uint8_t byte : ਅਗੇਤਰ) {
			(ਖਾਲੀ)st24_decode(byte, &rssi, &ਗੁਆਚਾ_ਗਿਣਤੀ, &channel_ਗਿਣਤੀ, channels, 24);
		}

		ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i <= ST24_ਅੰਕੜੇ_LEN_MAX; ++i) {
			(ਖਾਲੀ)st24_decode(0xaa, &rssi, &ਗੁਆਚਾ_ਗਿਣਤੀ, &channel_ਗਿਣਤੀ, channels, 24);
		}
	}

	uint8_t ਨਿਊਨਤਮ_packet[] {ST24_STX1, ST24_STX2, 2, 0xff, 0};
	ਨਿਊਨਤਮ_packet[4] = st24_common_crc8(&ਨਿਊਨਤਮ_packet[2], 2);
	ਪੂਰਨ ਨਤੀਜਾ = 1;

	ਲਈ (ਅਚਲ uint8_t byte : ਨਿਊਨਤਮ_packet) {
		ਨਤੀਜਾ = st24_decode(byte, &rssi, &ਗੁਆਚਾ_ਗਿਣਤੀ, &channel_ਗਿਣਤੀ, channels, 24);
	}

	ut_test(ਨਤੀਜਾ == 2);
	ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ RCTest::st24Test()
{
	ਅਚਲ ਅੱਖਰ *filepath = TEST_ਅੰਕੜੇ_ਰਸਤਾ "st24_data.txt";

	//PX4_INFO("loading data from: %s", filepath);

	FILE *fp;

	fp = fopen(filepath, "rt");
	ut_test(fp);

	ਦਸ਼ਮਲਵ f;
	ਬਿਨਾਨਿਸ਼ਾਨੀ x;
	ਪੂਰਨ ret;

	// Trash the first 20 lines
	ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < 20; i++) {
		ਅੱਖਰ buf[200];
		(ਖਾਲੀ)fgets(buf, ਆਕਾਰ(buf), fp);
	}

	ਦਸ਼ਮਲਵ ਆਖਰੀ_ਸਮਾਂ = 0;

	ਜਦੋਂ (EOF != (ret = fscanf(fp, "%f,%x,,", &f, &x))) {

		ਜੇ (ret <= 0) {
			fclose(fp);
			ut_test(ret > 0);
		}

		ਜੇ (((f - ਆਖਰੀ_ਸਮਾਂ) * 1000 * 1000) > 3000) {
			// PX4_INFO("FRAME RESET\n\n");
		}

		uint8_t b = static_cast<uint8_t>(x);

		ਆਖਰੀ_ਸਮਾਂ = f;

		// Pipe the data into the parser
		//hrt_abstime now = hrt_absolute_time();

		uint8_t rssi;
		uint8_t rx_ਗਿਣਤੀ;
		uint16_t channel_ਗਿਣਤੀ;
		uint16_t channels[20];

		ਜੇ (!st24_decode(b, &rssi, &rx_ਗਿਣਤੀ, &channel_ਗਿਣਤੀ, channels, ਆਕਾਰ(channels) / ਆਕਾਰ(channels[0]))) {
			//PX4_INFO("decoded: %u channels (converted to PPM range)", (unsigned)channel_count);

			ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < channel_ਗਿਣਤੀ; i++) {
				//int16_t val = channels[i];
				//PX4_INFO("channel %u: %d 0x%03X", i, static_cast<int>(val), static_cast<int>(val));
			}
		}
	}

	ut_test(ret == EOF);

	ਵਾਪਸ ਸੱਚ;
}

ਤਾਰਕਿਕ RCTest::sumdTest()
{
	ਅਚਲ ਅੱਖਰ *filepath = TEST_ਅੰਕੜੇ_ਰਸਤਾ "sumd_data.txt";

	//PX4_INFO("loading data from: %s", filepath);

	FILE *fp;

	fp = fopen(filepath, "rt");
	ut_test(fp);

	ਦਸ਼ਮਲਵ f;
	ਬਿਨਾਨਿਸ਼ਾਨੀ x;
	ਪੂਰਨ ret;

	// Trash the first 20 lines
	ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < 20; i++) {
		ਅੱਖਰ buf[200];
		(ਖਾਲੀ)fgets(buf, ਆਕਾਰ(buf), fp);
	}

	ਦਸ਼ਮਲਵ ਆਖਰੀ_ਸਮਾਂ = 0;

	ਜਦੋਂ (EOF != (ret = fscanf(fp, "%f,%x,,", &f, &x))) {

		ਜੇ (ret <= 0) {
			fclose(fp);
			ut_test(ret > 0);
		}

		ਜੇ (((f - ਆਖਰੀ_ਸਮਾਂ) * 1000 * 1000) > 3000) {
			// PX4_INFO("FRAME RESET\n\n");
		}

		uint8_t b = static_cast<uint8_t>(x);

		ਆਖਰੀ_ਸਮਾਂ = f;

		// Pipe the data into the parser
		//hrt_abstime now = hrt_absolute_time();

		uint8_t rssi;
		uint8_t rx_ਗਿਣਤੀ;
		uint16_t channel_ਗਿਣਤੀ;
		uint16_t channels[32];
		ਤਾਰਕਿਕ sumd_failsafe;


		ਜੇ (!sumd_decode(b, &rssi, &rx_ਗਿਣਤੀ, &channel_ਗਿਣਤੀ, channels, 32, &sumd_failsafe)) {
			//PX4_INFO("decoded: %u channels (converted to PPM range)", (unsigned)channel_count);

			ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < channel_ਗਿਣਤੀ; i++) {
				//int16_t val = channels[i];
				//PX4_INFO("channel %u: %d 0x%03X", i, static_cast<int>(val), static_cast<int>(val));
			}
		}
	}

	ut_test(ret == EOF);

	ਵਾਪਸ ਸੱਚ;
}

ut_declare_test_c(rc_tests_main, RCTest)
