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

#ಅನ್ತರ್ಭಾವ "dds_serializer.h"


ಶೂನ್ಯ *dds_malloc(size_t ಗಾತ್ರ_2)
{
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
ಶೂನ್ಯ *dds_realloc(ಶೂನ್ಯ *ptr, size_t new_ಗಾತ್ರ)
{
	ಬರೆ("Error CDR buffer is too small\n");
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
ಶೂನ್ಯ dds_free(ಶೂನ್ಯ *pt)
{

}
ಸ್ಥಿರಾಂಕ ರಚನೆ dds_cdrstream_allocator dds_allocator = { dds_malloc, dds_realloc, dds_free };

// CDR Xtypes header {0x00, 0x01} indicates it's Little Endian (CDR_LE representation)
ಸ್ಥಿರಾಂಕ uint8_t ros2_ಶೀರ್ಷಿಕೆ[4] = {0x00, 0x01, 0x00, 0x00};
