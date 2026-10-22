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

CDev::CDev(const char *devname) :
	_devname(devname)
{
	PX4_DEBUG("CDev::CDev");

	整数 ret = px4_sem_init(&_lock, 0, 1);

	もし (ret != 0) {
		PX4_DEBUG("SEM INIT FAIL: ret %d", ret);
	}
}

CDev::~CDev()
{
	PX4_DEBUG("CDev::~CDev");

	もし (_registered) {
		unregister_driver(_devname);
	}

	もし (_pollset) {
		delete[](_pollset);
	}

	px4_sem_destroy(&_lock);
}

整数
CDev::register_class_devname(const char *class_devname)
{
	PX4_DEBUG("CDev::register_class_devname %s", class_devname);

	もし (class_devname == nullptr) {
		戻る -EINVAL;
	}

	整数 class_instance = 0;
	整数 ret = -ENOSPC;

	ながら (class_instance < 4) {
		char name[32];
		snprintf(name, sizeof(name), "%s%d", class_devname, class_instance);
		ret = register_driver(name, &fops_ref(), 0666, (void *)this);

		もし (ret == OK) {
			break;
		}

		class_instance++;
	}

	もし (class_instance == 4) {
		戻る ret;
	}

	戻る class_instance;
}

整数
CDev::unregister_class_devname(const char *class_devname, unsigned class_instance)
{
	PX4_DEBUG("CDev::unregister_class_devname");

	char name[32];
	snprintf(name, sizeof(name), "%s%u", class_devname, class_instance);
	戻る unregister_driver(name);
}

整数
CDev::init()
{
	PX4_DEBUG("CDev::init");

	整数 ret = PX4_OK;

	// now register the driver
	もし (_devname != nullptr) {
		ret = register_driver(_devname, &fops_ref(), 0666, (void *)this);

		もし (ret == PX4_OK) {
			_registered = true;
		}

	} 否则 {
		ret = -ENODEV;
	}

	戻る ret;
}

/*
 * Default implementations of the character device interface
 */
整数
CDev::open(file_t *filep)
{
	PX4_DEBUG("CDev::open");
	整数 ret = PX4_OK;

	lock();
	/* increment the open count */
	_open_count++;

	もし (_open_count == 1) {

		/* first-open callback may decline the open */
		ret = open_first(filep);

		もし (ret != PX4_OK) {
			_open_count--;
		}
	}

	unlock();

	戻る ret;
}

整数
CDev::close(file_t *filep)
{
	PX4_DEBUG("CDev::close");
	整数 ret = PX4_OK;

	lock();

	もし (_open_count > 0) {
		/* decrement the open count */
		_open_count--;

		/* callback cannot decline the close */
		もし (_open_count == 0) {
			ret = close_last(filep);
		}

	} 否则 {
		ret = -EBADF;
	}

	unlock();

	戻る ret;
}

整数
CDev::poll(file_t *filep, px4_pollfd_struct_t *fds, bool setup)
{
	PX4_DEBUG("CDev::Poll %s", setup ? "setup" : "teardown");
	整数 ret = PX4_OK;

	もし (setup) {
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
		ながら ((ret = store_poll_waiter(fds)) == -ENFILE) {

			// No free slot found. Resize the pollset. This is expensive, but it's only needed initially.

			もし (_max_pollwaiters >= 256 / 2) { //_max_pollwaiters is uint8_t
				ret = -ENOMEM;
				break;
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

			もし (prev_pollset == _pollset) {
				// no one else updated the _pollset meanwhile, so we're good to go
				もし (!new_pollset) {
					ret = -ENOMEM;
					break;
				}

				もし (_max_pollwaiters > 0) {
					memset(new_pollset + _max_pollwaiters, 0, sizeof(px4_pollfd_struct_t *) * (new_count - _max_pollwaiters));
					memcpy(new_pollset, _pollset, sizeof(px4_pollfd_struct_t *) * _max_pollwaiters);
				}

				_pollset = new_pollset;
				_pollset[_max_pollwaiters] = fds;
				_max_pollwaiters = new_count;

				// free the previous _pollset (we need to unlock here which is fine because we don't access _pollset anymore)
#ifdef __PX4_NUTTX
				px4_leave_critical_section(flags);
#endif

				もし (prev_pollset) {
					delete[](prev_pollset);
				}

#ifdef __PX4_NUTTX
				flags = px4_enter_critical_section();
#endif

				// Success
				ret = PX4_OK;
				break;
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

		もし (ret == PX4_OK) {

			/*
			 * Check to see whether we should send a poll notification
			 * immediately.
			 */
			fds->revents |= fds->events & poll_state(filep);

			/* yes? invoke the callback */
#ifdef __PX4_NUTTX

			もし (fds->revents != 0 && fds->cb != nullptr) {
				fds->cb(fds);
			}

#否则

			もし (fds->revents != 0) {
				px4_sem_post(fds->sem);
			}

#endif

		}

		ATOMIC_LEAVE;

	} 否则 {
		ATOMIC_ENTER;
		/*
		 * Handle a teardown request.
		 */
		ret = remove_poll_waiter(fds);
		ATOMIC_LEAVE;
	}

	戻る ret;
}

void
CDev::poll_notify(px4_pollevent_t events)
{
	PX4_DEBUG("CDev::poll_notify events = %0x", events);

	/* lock against poll() as well as other wakeups */
	ATOMIC_ENTER;

	ために (unsigned i = 0; i < _max_pollwaiters; i++) {
		もし (nullptr != _pollset[i]) {
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

	もし (fds->revents != 0 && fds->cb != nullptr) {
		fds->cb(fds);
	}

#否则

	もし (fds->revents != 0) {
		px4_sem_post(fds->sem);
	}

#endif
}

整数
CDev::store_poll_waiter(px4_pollfd_struct_t *fds)
{
	// Look for a free slot.
	PX4_DEBUG("CDev::store_poll_waiter");

	ために (unsigned i = 0; i < _max_pollwaiters; i++) {
		もし (nullptr == _pollset[i]) {

			/* save the pollfd */
			_pollset[i] = fds;

			戻る PX4_OK;
		}
	}

	戻る -ENFILE;
}

整数
CDev::remove_poll_waiter(px4_pollfd_struct_t *fds)
{
	PX4_DEBUG("CDev::remove_poll_waiter");

	ために (unsigned i = 0; i < _max_pollwaiters; i++) {
		もし (fds == _pollset[i]) {

			_pollset[i] = nullptr;
			戻る PX4_OK;

		}
	}

	PX4_DEBUG("poll: bad fd state");
	戻る -EINVAL;
}

} // namespace cdev
