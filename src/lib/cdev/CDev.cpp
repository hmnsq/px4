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

	ចំនួនគត់ ret = px4_sem_init(&_lock, 0, 1);

	ប្រសិនបើ (ret != 0) {
		PX4_DEBUG("SEM INIT FAIL: ret %d", ret);
	}
}

CDev::~CDev()
{
	PX4_DEBUG("CDev::~CDev");

	ប្រសិនបើ (_registered) {
		unregister_driver(_devname);
	}

	ប្រសិនបើ (_pollset) {
		delete[](_pollset);
	}

	px4_sem_destroy(&_lock);
}

ចំនួនគត់
CDev::register_class_devname(const var_na *class_devname)
{
	PX4_DEBUG("CDev::register_class_devname %s", class_devname);

	ប្រសិនបើ (class_devname == nullptr) {
		ត្រឡប់ -EINVAL;
	}

	ចំនួនគត់ class_instance = 0;
	ចំនួនគត់ ret = -ENOSPC;

	ខណៈពេល (class_instance < 4) {
		var_na name[32];
		snprintf(name, parimaa_na(name), "%s%d", class_devname, class_instance);
		ret = register_driver(name, &fops_ref(), 0666, (void *)this);

		ប្រសិនបើ (ret == OK) {
			bha_nga;
		}

		class_instance++;
	}

	ប្រសិនបើ (class_instance == 4) {
		ត្រឡប់ ret;
	}

	ត្រឡប់ class_instance;
}

ចំនួនគត់
CDev::unregister_class_devname(const var_na *class_devname, unsigned class_instance)
{
	PX4_DEBUG("CDev::unregister_class_devname");

	var_na name[32];
	snprintf(name, parimaa_na(name), "%s%u", class_devname, class_instance);
	ត្រឡប់ unregister_driver(name);
}

ចំនួនគត់
CDev::init()
{
	PX4_DEBUG("CDev::init");

	ចំនួនគត់ ret = PX4_OK;

	// now register the driver
	ប្រសិនបើ (_devname != nullptr) {
		ret = register_driver(_devname, &fops_ref(), 0666, (void *)this);

		ប្រសិនបើ (ret == PX4_OK) {
			_registered = true;
		}

	} បើមិនដូច្នេះ {
		ret = -ENODEV;
	}

	ត្រឡប់ ret;
}

/*
 * Default implementations of the character device interface
 */
ចំនួនគត់
CDev::open(file_t *filep)
{
	PX4_DEBUG("CDev::open");
	ចំនួនគត់ ret = PX4_OK;

	lock();
	/* increment the open count */
	_open_count++;

	ប្រសិនបើ (_open_count == 1) {

		/* first-open callback may decline the open */
		ret = open_first(filep);

		ប្រសិនបើ (ret != PX4_OK) {
			_open_count--;
		}
	}

	unlock();

	ត្រឡប់ ret;
}

ចំនួនគត់
CDev::close(file_t *filep)
{
	PX4_DEBUG("CDev::close");
	ចំនួនគត់ ret = PX4_OK;

	lock();

	ប្រសិនបើ (_open_count > 0) {
		/* decrement the open count */
		_open_count--;

		/* callback cannot decline the close */
		ប្រសិនបើ (_open_count == 0) {
			ret = close_last(filep);
		}

	} បើមិនដូច្នេះ {
		ret = -EBADF;
	}

	unlock();

	ត្រឡប់ ret;
}

ចំនួនគត់
CDev::poll(file_t *filep, px4_pollfd_struct_t *fds, bool setup)
{
	PX4_DEBUG("CDev::Poll %s", setup ? "setup" : "teardown");
	ចំនួនគត់ ret = PX4_OK;

	ប្រសិនបើ (setup) {
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
		ខណៈពេល ((ret = store_poll_waiter(fds)) == -ENFILE) {

			// No free slot found. Resize the pollset. This is expensive, but it's only needed initially.

			ប្រសិនបើ (_max_pollwaiters >= 256 / 2) { //_max_pollwaiters is uint8_t
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

			ប្រសិនបើ (prev_pollset == _pollset) {
				// no one else updated the _pollset meanwhile, so we're good to go
				ប្រសិនបើ (!new_pollset) {
					ret = -ENOMEM;
					bha_nga;
				}

				ប្រសិនបើ (_max_pollwaiters > 0) {
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

				ប្រសិនបើ (prev_pollset) {
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

		ប្រសិនបើ (ret == PX4_OK) {

			/*
			 * Check to see whether we should send a poll notification
			 * immediately.
			 */
			fds->revents |= fds->events & poll_state(filep);

			/* yes? invoke the callback */
#ifdef __PX4_NUTTX

			ប្រសិនបើ (fds->revents != 0 && fds->cb != nullptr) {
				fds->cb(fds);
			}

#បើមិនដូច្នេះ

			ប្រសិនបើ (fds->revents != 0) {
				px4_sem_post(fds->sem);
			}

#endif

		}

		ATOMIC_LEAVE;

	} បើមិនដូច្នេះ {
		ATOMIC_ENTER;
		/*
		 * Handle a teardown request.
		 */
		ret = remove_poll_waiter(fds);
		ATOMIC_LEAVE;
	}

	ត្រឡប់ ret;
}

void
CDev::poll_notify(px4_pollevent_t events)
{
	PX4_DEBUG("CDev::poll_notify events = %0x", events);

	/* lock against poll() as well as other wakeups */
	ATOMIC_ENTER;

	សម្រាប់ (unsigned i = 0; i < _max_pollwaiters; i++) {
		ប្រសិនបើ (nullptr != _pollset[i]) {
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

	ប្រសិនបើ (fds->revents != 0 && fds->cb != nullptr) {
		fds->cb(fds);
	}

#បើមិនដូច្នេះ

	ប្រសិនបើ (fds->revents != 0) {
		px4_sem_post(fds->sem);
	}

#endif
}

ចំនួនគត់
CDev::store_poll_waiter(px4_pollfd_struct_t *fds)
{
	// Look for a free slot.
	PX4_DEBUG("CDev::store_poll_waiter");

	សម្រាប់ (unsigned i = 0; i < _max_pollwaiters; i++) {
		ប្រសិនបើ (nullptr == _pollset[i]) {

			/* save the pollfd */
			_pollset[i] = fds;

			ត្រឡប់ PX4_OK;
		}
	}

	ត្រឡប់ -ENFILE;
}

ចំនួនគត់
CDev::remove_poll_waiter(px4_pollfd_struct_t *fds)
{
	PX4_DEBUG("CDev::remove_poll_waiter");

	សម្រាប់ (unsigned i = 0; i < _max_pollwaiters; i++) {
		ប្រសិនបើ (fds == _pollset[i]) {

			_pollset[i] = nullptr;
			ត្រឡប់ PX4_OK;

		}
	}

	PX4_DEBUG("poll: bad fd state");
	ត្រឡប់ -EINVAL;
}

} // namespace cdev
