/****************************************************************************
 *
 *	Copyright (c) 2012-2015 PX4 Development Team. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *	notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *	notice, this list of conditions and the following disclaimer in
 *	the documentation and/or other materials provided with the
 *	distribution.
 * 3. Neither the name PX4 nor the names of its contributors may be
 *	used to endorse or promote products derived from this software
 *	without specific prior written permission.
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

/**
 * @file spektrum_rssi.h
 *
 * RSSI dBm to percentage conversion for Spektrum telemetry receivers
 *
 * @author Kurt Kiefer <kekiefer@gmail.com>
 */

#لارښوونه once

#ضمن <climits>

/*
min_rssi_dbm = -92.0
max_rssi_dbm = -42.0

def dbm_to_percent(abs_dbm):
	if -abs_dbm < min_rssi_dbm:
		return 0.0
	elif -abs_dbm > max_rssi_dbm:
		return 100.0
	else:
		return 100.0 * math.log10(1.0 + (float)(-abs_dbm - min_rssi_dbm) * (9.0 / (max_rssi_dbm - min_rssi_dbm)))
*/
constexpr int8_t lu_dbm_percent[] = {
	100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
	100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100,
	100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 100, 99,  98,  98,  97,  96,
	95,  94,  93,  92,  91,  90,  89,  88,  87,  86,  85,  84,  83,  82,  81,  79,
	78,  77,  75,  74,  73,  71,  70,  68,  66,  65,  63,  61,  59,  57,  55,  52,
	50,  47,  45,  42,  39,  35,  32,  28,  24,  19,  13,  7,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
	0,
};

#تعریف_کړه ماسک_2(n) (n >> (حجم(n) * CHAR_بټ - 1))

/* constexpr-compatible version of abs() */
constexpr بینښه c_abs(صحيح n)
{
	أرجع (n + ماسک_2(n)) ^ ماسک_2(n);
}

/* convert signed spektrum dbm (-92 to -42) to percentage */
constexpr int8_t spek_dbm_منزل_percent(int8_t dbm)
{
	أرجع lu_dbm_percent[c_abs(dbm)];
}

static_assert(spek_dbm_منزل_percent(0) == 100, "0 dbm should be 100%");
static_assert(spek_dbm_منزل_percent(-42) == 100, "-42 dbm should be 100%");
static_assert(spek_dbm_منزل_percent(-80) == 50, "-80 dbm should be 50%");
static_assert(spek_dbm_منزل_percent(-92) == 0, "-92 dbm should be 0%");
static_assert(spek_dbm_منزل_percent(-128) == 0, "-128 dbm should be 0%");
