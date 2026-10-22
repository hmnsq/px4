/****************************************************************************
 *
 *   Copyright (c) 2023 PX4 Development Team. All rights reserved.
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

#ಸೂಚನೆ once

#ಅನ್ತರ್ಭಾವ <px4_platform_common/atomic_bitset.h>
#ಅನ್ತರ್ಭಾವ "atomic_transaction.h"
#ಅನ್ತರ್ಭಾವ "param.h"
#ಅನ್ತರ್ಭಾವ <stdlib.h>

class ParamLayer
{
ಸಾರ್ವಜನಿಕ:
	ಸ್ಥಿರ constexpr param_t PARAM_ಎಣಿಕೆ = ಗಾತ್ರ(px4::parameters) / ಗಾತ್ರ(param_info_s);

	ParamLayer(ParamLayer *ಪೋಷಕ = nullptr) : ಪೋಷಕ_2(ಪೋಷಕ) {}

	ParamLayer(ಸ್ಥಿರಾಂಕ ParamLayer &) = delete;
	ParamLayer &ಕ್ರಿಯಾಕಾರಕ=(ಸ್ಥಿರಾಂಕ ParamLayer &) = delete;
	ParamLayer(ParamLayer &&) = delete;
	ParamLayer &ಕ್ರಿಯಾಕಾರಕ=(ParamLayer &&) = delete;


	ವಾಸ್ತವಿಕ bool store(param_t param, param_ಮೌಲ್ಯ_u ಮೌಲ್ಯ) = 0;

	ವಾಸ್ತವಿಕ bool ಒಳಗೊಂಡಿದೆ(param_t param) ಸ್ಥಿರಾಂಕ = 0;

	ವಾಸ್ತವಿಕ px4::AtomicBitset<PARAM_ಎಣಿಕೆ> containedAsBitset() ಸ್ಥಿರಾಂಕ = 0;

	ವಾಸ್ತವಿಕ param_ಮೌಲ್ಯ_u get(param_t param) ಸ್ಥಿರಾಂಕ = 0;

	ವಾಸ್ತವಿಕ ಶೂನ್ಯ reset(param_t param) = 0;

	ವಾಸ್ತವಿಕ ಶೂನ್ಯ refresh(param_t param) = 0;

	ವಾಸ್ತವಿಕ ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2() ಸ್ಥಿರಾಂಕ = 0;

	ವಾಸ್ತವಿಕ ಪೂರ್ಣಾಂಕ byte_ಗಾತ್ರ() ಸ್ಥಿರಾಂಕ = 0;

ಸಂರಕ್ಷಿತ:
	ParamLayer *ಸ್ಥಿರಾಂಕ ಪೋಷಕ_2;
};
