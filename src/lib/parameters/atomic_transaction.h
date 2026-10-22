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

#निर्देशः once

#यदि_परिभाषितम् __PX4_NUTTX
#अन्तर्भाव "px4_platform_common/micro_hal.h"
#यदि_अन्तः

#यदि_परिभाषितम् __PX4_POSIX
#अन्तर्भाव <pthread.h>
#यदि_अन्तः


class AtomicTransaction
{
गुप्त:
#यदि_परिभाषितम् __PX4_NUTTX
	irqstate_t _irq_state;
#यदि_अन्तः

#यदि_परिभाषितम् __PX4_POSIX
	स्थिर pthread_mutex_t *_get_mutex();
#यदि_अन्तः

सार्वजनिक:
#यदि_परिभाषितम् __PX4_POSIX
	// Initialize the recursive param mutex. Must be called once before any thread
	// accesses parameters (done from param_init()).
	स्थिर शून्य initialize();
#यदि_अन्तः

	AtomicTransaction()
	{
		तालकम्();
	}

	~AtomicTransaction()
	{
		unlock();
	}

	शून्य तालकम्()
	{
#यदि_परिभाषितम् __PX4_NUTTX
		_irq_state = px4_enter_critical_section();
#यदि_अन्तः
#यदि_परिभाषितम् __PX4_POSIX
		pthread_mutex_lock(_get_mutex());
#यदि_अन्तः
	}

	शून्य unlock()
	{
#यदि_परिभाषितम् __PX4_NUTTX
		px4_leave_critical_section(_irq_state);
#यदि_अन्तः
#यदि_परिभाषितम् __PX4_POSIX
		pthread_mutex_unlock(_get_mutex());
#यदि_अन्तः
	}
};
