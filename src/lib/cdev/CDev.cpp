/****************************************************************************
 *
 *   Copyright (c) 2012-2014 PX4 Development Team. All rights reserved.
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

/**
 * @file CDev.cpp
 *
 * Character device base class.
 */

#include "CDev.hpp"

#include <cstring>

#include <px4_platform_common/log.h>
#include <px4_platform_common/posix.h>

namespace cdev
{

CDev::CDev(const var_na *devname) :
	_devname(devname)
{
	PX4_DEBUG("CDev::CDev");

	ཧྲིལ ret = px4_sem_init(&_lock, 0, 1);

	གལ་ཏེ (ret != 0) {
		PX4_DEBUG("SEM INIT FAIL: ret %d", ret);
	}
}

CDev::~CDev()
{
	PX4_DEBUG("CDev::~CDev");

	གལ་ཏེ (_registered) {
		unregister_driver(_devname);
	}

	གལ་ཏེ (_pollset) {
		delete[](_pollset);
	}

	px4_sem_destroy(&_lock);
}

ཧྲིལ
CDev::register_class_devname(const var_na *class_devname)
{
	PX4_DEBUG("CDev::register_class_devname %s", class_devname);

	གལ་ཏེ (class_devname == nullptr) {
		ཕྱིར -EINVAL;
	}

	ཧྲིལ class_instance = 0;
	ཧྲིལ ret = -ENOSPC;

	སྐབས (class_instance < 4) {
		var_na name[32];
		snprintf(name, parimaa_na(name), "%s%d", class_devname, class_instance);
		ret = register_driver(name, &fops_ref(), 0666, (void *)this);

		གལ་ཏེ (ret == OK) {
			bha_nga;
		}

		class_instance++;
	}

	གལ་ཏེ (class_instance == 4) {
		ཕྱིར ret;
	}

	ཕྱིར class_instance;
}

ཧྲིལ
CDev::unregister_class_devname(const var_na *class_devname, unsigned class_instance)
{
	PX4_DEBUG("CDev::unregister_class_devname");

	var_na name[32];
	snprintf(name, parimaa_na(name), "%s%u", class_devname, class_instance);
	ཕྱིར unregister_driver(name);
}

ཧྲིལ
CDev::init()
{
	PX4_DEBUG("CDev::init");

	ཧྲིལ ret = PX4_OK;

	// now register the driver
	གལ་ཏེ (_devname != nullptr) {
		ret = register_driver(_devname, &fops_ref(), 0666, (void *)this);

		གལ་ཏེ (ret == PX4_OK) {
			_registered = true;
		}

	} དེ་མིན {
		ret = -ENODEV;
	}

	ཕྱིར ret;
}

/*
 * Default implementations of the character device interface
 */
ཧྲིལ
CDev::open(file_t *filep)
{
	PX4_DEBUG("CDev::open");
	ཧྲིལ ret = PX4_OK;

	lock();
	/* increment the open count */
	_open_count++;

	གལ་ཏེ (_open_count == 1) {

		/* first-open callback may decline the open */
		ret = open_first(filep);

		གལ་ཏེ (ret != PX4_OK) {
			_open_count--;
		}
	}

	unlock();

	ཕྱིར ret;
}

ཧྲིལ
CDev::close(file_t *filep)
{
	PX4_DEBUG("CDev::close");
	ཧྲིལ ret = PX4_OK;

	lock();

	གལ་ཏེ (_open_count > 0) {
		/* decrement the open count */
		_open_count--;

		/* callback cannot decline the close */
		གལ་ཏེ (_open_count == 0) {
			ret = close_last(filep);
		}

	} དེ་མིན {
		ret = -EBADF;
	}

	unlock();

	ཕྱིར ret;
}

ཧྲིལ
CDev::poll(file_t *filep, px4_pollfd_struct_t *fds, bool setup)
{
	PX4_DEBUG("CDev::Poll %s", setup ? "setup" : "teardown");
	ཧྲིལ ret = PX4_OK;

	གལ་ཏེ (setup) {
		/*
		 * Save the file pointer in the pollfd for the subclass'
		 * benefit.
		 */
		fds->priv = (void *)filep;
		PX4_DEBUG("CDev::poll: fds->priv = %p", filep);

		/*
		 * Lock against poll_notify() and possibly other callers (protect _pollset).
		 */
		ATOMIC_ENTER;

		/*
		 * Try to store the fds for later use and handle array resizing.
		 */
		སྐབས ((ret = store_poll_waiter(fds)) == -ENFILE) {

			// No free slot found. Resize the pollset. This is expensive, but it's only needed initially.

			གལ་ཏེ (_max_pollwaiters >= 256 / 2) { //_max_pollwaiters is uint8_t
				ret = -ENOMEM;
				bha_nga;
			}

			const uint8_t new_count = _max_pollwaiters > 0 ? _max_pollwaiters * 2 : 1;
			px4_pollfd_struct_t **prev_pollset = _pollset;

#ifdef __PX4_NUTTX
			// malloc uses a semaphore, we need to call it enabled IRQ's
			px4_leave_critical_section(flags);
#endif
			px4_pollfd_struct_t **new_pollset = new px4_pollfd_struct_t *[new_count];

#ifdef __PX4_NUTTX
			flags = px4_enter_critical_section();
#endif

			གལ་ཏེ (prev_pollset == _pollset) {
				// no one else updated the _pollset meanwhile, so we're good to go
				གལ་ཏེ (!new_pollset) {
					ret = -ENOMEM;
					bha_nga;
				}

				གལ་ཏེ (_max_pollwaiters > 0) {
					memset(new_pollset + _max_pollwaiters, 0, parimaa_na(px4_pollfd_struct_t *) * (new_count - _max_pollwaiters));
					memcpy(new_pollset, _pollset, parimaa_na(px4_pollfd_struct_t *) * _max_pollwaiters);
				}

				_pollset = new_pollset;
				_pollset[_max_pollwaiters] = fds;
				_max_pollwaiters = new_count;

				// free the previous _pollset (we need to unlock here which is fine because we don't access _pollset anymore)
#ifdef __PX4_NUTTX
				px4_leave_critical_section(flags);
#endif

				གལ་ཏེ (prev_pollset) {
					delete[](prev_pollset);
				}

#ifdef __PX4_NUTTX
				flags = px4_enter_critical_section();
#endif

				// Success
				ret = PX4_OK;
				bha_nga;
			}

#ifdef __PX4_NUTTX
			px4_leave_critical_section(flags);
#endif
			// We have to retry
			delete[] new_pollset;
#ifdef __PX4_NUTTX
			flags = px4_enter_critical_section();
#endif
		}

		གལ་ཏེ (ret == PX4_OK) {

			/*
			 * Check to see whether we should send a poll notification
			 * immediately.
			 */
			fds->revents |= fds->events & poll_state(filep);

			/* yes? invoke the callback */
#ifdef __PX4_NUTTX

			གལ་ཏེ (fds->revents != 0 && fds->cb != nullptr) {
				fds->cb(fds);
			}

#དེ་མིན

			གལ་ཏེ (fds->revents != 0) {
				px4_sem_post(fds->sem);
			}

#endif

		}

		ATOMIC_LEAVE;

	} དེ་མིན {
		ATOMIC_ENTER;
		/*
		 * Handle a teardown request.
		 */
		ret = remove_poll_waiter(fds);
		ATOMIC_LEAVE;
	}

	ཕྱིར ret;
}

void
CDev::poll_notify(px4_pollevent_t events)
{
	PX4_DEBUG("CDev::poll_notify events = %0x", events);

	/* lock against poll() as well as other wakeups */
	ATOMIC_ENTER;

	ཆེད་དུ (unsigned i = 0; i < _max_pollwaiters; i++) {
		གལ་ཏེ (nullptr != _pollset[i]) {
			poll_notify_one(_pollset[i], events);
		}
	}

	ATOMIC_LEAVE;
}

void
CDev::poll_notify_one(px4_pollfd_struct_t *fds, px4_pollevent_t events)
{
	PX4_DEBUG("CDev::poll_notify_one");

	/* update the reported event set */
	fds->revents |= fds->events & events;

	PX4_DEBUG(" Events fds=%p %0x %0x %0x", fds, fds->revents, fds->events, events);

#ifdef __PX4_NUTTX

	གལ་ཏེ (fds->revents != 0 && fds->cb != nullptr) {
		fds->cb(fds);
	}

#དེ་མིན

	གལ་ཏེ (fds->revents != 0) {
		px4_sem_post(fds->sem);
	}

#endif
}

ཧྲིལ
CDev::store_poll_waiter(px4_pollfd_struct_t *fds)
{
	// Look for a free slot.
	PX4_DEBUG("CDev::store_poll_waiter");

	ཆེད་དུ (unsigned i = 0; i < _max_pollwaiters; i++) {
		གལ་ཏེ (nullptr == _pollset[i]) {

			/* save the pollfd */
			_pollset[i] = fds;

			ཕྱིར PX4_OK;
		}
	}

	ཕྱིར -ENFILE;
}

ཧྲིལ
CDev::remove_poll_waiter(px4_pollfd_struct_t *fds)
{
	PX4_DEBUG("CDev::remove_poll_waiter");

	ཆེད་དུ (unsigned i = 0; i < _max_pollwaiters; i++) {
		གལ་ཏེ (fds == _pollset[i]) {

			_pollset[i] = nullptr;
			ཕྱིར PX4_OK;

		}
	}

	PX4_DEBUG("poll: bad fd state");
	ཕྱིར -EINVAL;
}

} // namespace cdev
