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

#شامل "CDev.hpp"

#شامل <cstring>

#شامل <px4_platform_common/log.h>
#شامل <px4_platform_common/posix.h>

فضاینام cdev
{

CDev::CDev(ثابت نویسه *devname) :
	_devname(devname)
{
	PX4_DEBUG("CDev::CDev");

	صحیح ret = px4_sem_init(&_lock, 0, 1);

	اگر (ret != 0) {
		PX4_DEBUG("SEM INIT FAIL: ret %d", ret);
	}
}

CDev::~CDev()
{
	PX4_DEBUG("CDev::~CDev");

	اگر (_registered) {
		unregister_driver(_devname);
	}

	اگر (_pollset) {
		delete[](_pollset);
	}

	px4_sem_destroy(&_lock);
}

صحیح
CDev::register_class_devname(ثابت نویسه *class_devname)
{
	PX4_DEBUG("CDev::register_class_devname %s", class_devname);

	اگر (class_devname == nullptr) {
		بازگشت -EINVAL;
	}

	صحیح class_instance = 0;
	صحیح ret = -ENOSPC;

	وقتی (class_instance < 4) {
		نویسه name[32];
		snprintf(name, اندازه(name), "%s%d", class_devname, class_instance);
		ret = register_driver(name, &fops_ref(), 0666, (تهی *)این);

		اگر (ret == OK) {
			بشکن;
		}

		class_instance++;
	}

	اگر (class_instance == 4) {
		بازگشت ret;
	}

	بازگشت class_instance;
}

صحیح
CDev::unregister_class_devname(ثابت نویسه *class_devname, unsigned class_instance)
{
	PX4_DEBUG("CDev::unregister_class_devname");

	نویسه name[32];
	snprintf(name, اندازه(name), "%s%u", class_devname, class_instance);
	بازگشت unregister_driver(name);
}

صحیح
CDev::init()
{
	PX4_DEBUG("CDev::init");

	صحیح ret = PX4_OK;

	// now register the driver
	اگر (_devname != nullptr) {
		ret = register_driver(_devname, &fops_ref(), 0666, (تهی *)این);

		اگر (ret == PX4_OK) {
			_registered = درست;
		}

	} وگرنه {
		ret = -ENODEV;
	}

	بازگشت ret;
}

/*
 * Default implementations of the character device interface
 */
صحیح
CDev::open(file_t *filep)
{
	PX4_DEBUG("CDev::open");
	صحیح ret = PX4_OK;

	lock();
	/* increment the open count */
	open_شمار++;

	اگر (open_شمار == 1) {

		/* first-open callback may decline the open */
		ret = open_first(filep);

		اگر (ret != PX4_OK) {
			open_شمار--;
		}
	}

	unlock();

	بازگشت ret;
}

صحیح
CDev::close(file_t *filep)
{
	PX4_DEBUG("CDev::close");
	صحیح ret = PX4_OK;

	lock();

	اگر (open_شمار > 0) {
		/* decrement the open count */
		open_شمار--;

		/* callback cannot decline the close */
		اگر (open_شمار == 0) {
			ret = close_last(filep);
		}

	} وگرنه {
		ret = -EBADF;
	}

	unlock();

	بازگشت ret;
}

صحیح
CDev::poll(file_t *filep, px4_pollfd_struct_t *fds, منطقی setup)
{
	PX4_DEBUG("CDev::Poll %s", setup ? "setup" : "teardown");
	صحیح ret = PX4_OK;

	اگر (setup) {
		/*
		 * Save the file pointer in the pollfd for the subclass'
		 * benefit.
		 */
		fds->priv = (تهی *)filep;
		PX4_DEBUG("CDev::poll: fds->priv = %p", filep);

		/*
		 * Lock against poll_notify() and possibly other callers (protect _pollset).
		 */
		ATOMIC_ENTER;

		/*
		 * Try to store the fds for later use and handle array resizing.
		 */
		وقتی ((ret = store_poll_waiter(fds)) == -ENFILE) {

			// No free slot found. Resize the pollset. This is expensive, but it's only needed initially.

			اگر (_max_pollwaiters >= 256 / 2) { //_max_pollwaiters is uint8_t
				ret = -ENOMEM;
				بشکن;
			}

			ثابت uint8_t new_شمار = _max_pollwaiters > 0 ? _max_pollwaiters * 2 : 1;
			px4_pollfd_struct_t **prev_pollset = _pollset;

#ifdef __PX4_NUTTX
			// malloc uses a semaphore, we need to call it enabled IRQ's
			px4_leave_critical_section(flags);
#endif
			px4_pollfd_struct_t **new_pollset = جدید px4_pollfd_struct_t *[new_شمار];

#ifdef __PX4_NUTTX
			flags = px4_enter_critical_section();
#endif

			اگر (prev_pollset == _pollset) {
				// no one else updated the _pollset meanwhile, so we're good to go
				اگر (!new_pollset) {
					ret = -ENOMEM;
					بشکن;
				}

				اگر (_max_pollwaiters > 0) {
					memset(new_pollset + _max_pollwaiters, 0, اندازه(px4_pollfd_struct_t *) * (new_شمار - _max_pollwaiters));
					memcpy(new_pollset, _pollset, اندازه(px4_pollfd_struct_t *) * _max_pollwaiters);
				}

				_pollset = new_pollset;
				_pollset[_max_pollwaiters] = fds;
				_max_pollwaiters = new_شمار;

				// free the previous _pollset (we need to unlock here which is fine because we don't access _pollset anymore)
#ifdef __PX4_NUTTX
				px4_leave_critical_section(flags);
#endif

				اگر (prev_pollset) {
					delete[](prev_pollset);
				}

#ifdef __PX4_NUTTX
				flags = px4_enter_critical_section();
#endif

				// Success
				ret = PX4_OK;
				بشکن;
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

		اگر (ret == PX4_OK) {

			/*
			 * Check to see whether we should send a poll notification
			 * immediately.
			 */
			fds->revents |= fds->events & poll_state(filep);

			/* yes? invoke the callback */
#ifdef __PX4_NUTTX

			اگر (fds->revents != 0 && fds->cb != nullptr) {
				fds->cb(fds);
			}

#وگرنه

			اگر (fds->revents != 0) {
				px4_sem_post(fds->sem);
			}

#endif

		}

		ATOMIC_LEAVE;

	} وگرنه {
		ATOMIC_ENTER;
		/*
		 * Handle a teardown request.
		 */
		ret = remove_poll_waiter(fds);
		ATOMIC_LEAVE;
	}

	بازگشت ret;
}

تهی
CDev::poll_notify(px4_pollevent_t events)
{
	PX4_DEBUG("CDev::poll_notify events = %0x", events);

	/* lock against poll() as well as other wakeups */
	ATOMIC_ENTER;

	برای (unsigned i = 0; i < _max_pollwaiters; i++) {
		اگر (nullptr != _pollset[i]) {
			poll_notify_one(_pollset[i], events);
		}
	}

	ATOMIC_LEAVE;
}

تهی
CDev::poll_notify_one(px4_pollfd_struct_t *fds, px4_pollevent_t events)
{
	PX4_DEBUG("CDev::poll_notify_one");

	/* update the reported event set */
	fds->revents |= fds->events & events;

	PX4_DEBUG(" Events fds=%p %0x %0x %0x", fds, fds->revents, fds->events, events);

#ifdef __PX4_NUTTX

	اگر (fds->revents != 0 && fds->cb != nullptr) {
		fds->cb(fds);
	}

#وگرنه

	اگر (fds->revents != 0) {
		px4_sem_post(fds->sem);
	}

#endif
}

صحیح
CDev::store_poll_waiter(px4_pollfd_struct_t *fds)
{
	// Look for a free slot.
	PX4_DEBUG("CDev::store_poll_waiter");

	برای (unsigned i = 0; i < _max_pollwaiters; i++) {
		اگر (nullptr == _pollset[i]) {

			/* save the pollfd */
			_pollset[i] = fds;

			بازگشت PX4_OK;
		}
	}

	بازگشت -ENFILE;
}

صحیح
CDev::remove_poll_waiter(px4_pollfd_struct_t *fds)
{
	PX4_DEBUG("CDev::remove_poll_waiter");

	برای (unsigned i = 0; i < _max_pollwaiters; i++) {
		اگر (fds == _pollset[i]) {

			_pollset[i] = nullptr;
			بازگشت PX4_OK;

		}
	}

	PX4_DEBUG("poll: bad fd state");
	بازگشت -EINVAL;
}

} // namespace cdev
