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

#অন্তর্ভাব "CDev.hpp"

#অন্তর্ভাব <cstring>

#অন্তর্ভাব <px4_platform_common/log.h>
#অন্তর্ভাব <px4_platform_common/posix.h>

নামাকাশ cdev
{

CDev::CDev(ধ্রুব বর্ণ *devname) :
	_devname(devname)
{
	PX4_DEBUG("CDev::CDev");

	পূর্ণাঙ্ক ret = px4_sem_init(&_lock, 0, 1);

	যদি (ret != 0) {
		PX4_DEBUG("SEM INIT FAIL: ret %d", ret);
	}
}

CDev::~CDev()
{
	PX4_DEBUG("CDev::~CDev");

	যদি (_registered) {
		unregister_driver(_devname);
	}

	যদি (_pollset) {
		delete[](_pollset);
	}

	px4_sem_destroy(&_lock);
}

পূর্ণাঙ্ক
CDev::register_class_devname(ধ্রুব বর্ণ *class_devname)
{
	PX4_DEBUG("CDev::register_class_devname %s", class_devname);

	যদি (class_devname == nullptr) {
		প্রত্যাগম -EINVAL;
	}

	পূর্ণাঙ্ক class_instance = 0;
	পূর্ণাঙ্ক ret = -ENOSPC;

	যাবত্ (class_instance < 4) {
		বর্ণ name[32];
		snprintf(name, পরিমাণ(name), "%s%d", class_devname, class_instance);
		ret = register_driver(name, &fops_ref(), 0666, (শূন্য *)এতত্);

		যদি (ret == OK) {
			ভঙ্গ;
		}

		class_instance++;
	}

	যদি (class_instance == 4) {
		প্রত্যাগম ret;
	}

	প্রত্যাগম class_instance;
}

পূর্ণাঙ্ক
CDev::unregister_class_devname(ধ্রুব বর্ণ *class_devname, unsigned class_instance)
{
	PX4_DEBUG("CDev::unregister_class_devname");

	বর্ণ name[32];
	snprintf(name, পরিমাণ(name), "%s%u", class_devname, class_instance);
	প্রত্যাগম unregister_driver(name);
}

পূর্ণাঙ্ক
CDev::init()
{
	PX4_DEBUG("CDev::init");

	পূর্ণাঙ্ক ret = PX4_OK;

	// now register the driver
	যদি (_devname != nullptr) {
		ret = register_driver(_devname, &fops_ref(), 0666, (শূন্য *)এতত্);

		যদি (ret == PX4_OK) {
			_registered = সত্য;
		}

	} অন্যথা {
		ret = -ENODEV;
	}

	প্রত্যাগম ret;
}

/*
 * Default implementations of the character device interface
 */
পূর্ণাঙ্ক
CDev::open(file_t *filep)
{
	PX4_DEBUG("CDev::open");
	পূর্ণাঙ্ক ret = PX4_OK;

	lock();
	/* increment the open count */
	open_গুনতি++;

	যদি (open_গুনতি == 1) {

		/* first-open callback may decline the open */
		ret = open_first(filep);

		যদি (ret != PX4_OK) {
			open_গুনতি--;
		}
	}

	unlock();

	প্রত্যাগম ret;
}

পূর্ণাঙ্ক
CDev::close(file_t *filep)
{
	PX4_DEBUG("CDev::close");
	পূর্ণাঙ্ক ret = PX4_OK;

	lock();

	যদি (open_গুনতি > 0) {
		/* decrement the open count */
		open_গুনতি--;

		/* callback cannot decline the close */
		যদি (open_গুনতি == 0) {
			ret = close_last(filep);
		}

	} অন্যথা {
		ret = -EBADF;
	}

	unlock();

	প্রত্যাগম ret;
}

পূর্ণাঙ্ক
CDev::poll(file_t *filep, px4_pollfd_struct_t *fds, তার্কিক setup)
{
	PX4_DEBUG("CDev::Poll %s", setup ? "setup" : "teardown");
	পূর্ণাঙ্ক ret = PX4_OK;

	যদি (setup) {
		/*
		 * Save the file pointer in the pollfd for the subclass'
		 * benefit.
		 */
		fds->priv = (শূন্য *)filep;
		PX4_DEBUG("CDev::poll: fds->priv = %p", filep);

		/*
		 * Lock against poll_notify() and possibly other callers (protect _pollset).
		 */
		ATOMIC_ENTER;

		/*
		 * Try to store the fds for later use and handle array resizing.
		 */
		যাবত্ ((ret = store_poll_waiter(fds)) == -ENFILE) {

			// No free slot found. Resize the pollset. This is expensive, but it's only needed initially.

			যদি (_max_pollwaiters >= 256 / 2) { //_max_pollwaiters is uint8_t
				ret = -ENOMEM;
				ভঙ্গ;
			}

			ধ্রুব uint8_t new_গুনতি = _max_pollwaiters > 0 ? _max_pollwaiters * 2 : 1;
			px4_pollfd_struct_t **prev_pollset = _pollset;

#ifdef __PX4_NUTTX
			// malloc uses a semaphore, we need to call it enabled IRQ's
			px4_leave_critical_section(flags);
#endif
			px4_pollfd_struct_t **new_pollset = নূতন px4_pollfd_struct_t *[new_গুনতি];

#ifdef __PX4_NUTTX
			flags = px4_enter_critical_section();
#endif

			যদি (prev_pollset == _pollset) {
				// no one else updated the _pollset meanwhile, so we're good to go
				যদি (!new_pollset) {
					ret = -ENOMEM;
					ভঙ্গ;
				}

				যদি (_max_pollwaiters > 0) {
					memset(new_pollset + _max_pollwaiters, 0, পরিমাণ(px4_pollfd_struct_t *) * (new_গুনতি - _max_pollwaiters));
					memcpy(new_pollset, _pollset, পরিমাণ(px4_pollfd_struct_t *) * _max_pollwaiters);
				}

				_pollset = new_pollset;
				_pollset[_max_pollwaiters] = fds;
				_max_pollwaiters = new_গুনতি;

				// free the previous _pollset (we need to unlock here which is fine because we don't access _pollset anymore)
#ifdef __PX4_NUTTX
				px4_leave_critical_section(flags);
#endif

				যদি (prev_pollset) {
					delete[](prev_pollset);
				}

#ifdef __PX4_NUTTX
				flags = px4_enter_critical_section();
#endif

				// Success
				ret = PX4_OK;
				ভঙ্গ;
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

		যদি (ret == PX4_OK) {

			/*
			 * Check to see whether we should send a poll notification
			 * immediately.
			 */
			fds->revents |= fds->events & poll_state(filep);

			/* yes? invoke the callback */
#ifdef __PX4_NUTTX

			যদি (fds->revents != 0 && fds->cb != nullptr) {
				fds->cb(fds);
			}

#অন্যথা

			যদি (fds->revents != 0) {
				px4_sem_post(fds->sem);
			}

#endif

		}

		ATOMIC_LEAVE;

	} অন্যথা {
		ATOMIC_ENTER;
		/*
		 * Handle a teardown request.
		 */
		ret = remove_poll_waiter(fds);
		ATOMIC_LEAVE;
	}

	প্রত্যাগম ret;
}

শূন্য
CDev::poll_notify(px4_pollevent_t events)
{
	PX4_DEBUG("CDev::poll_notify events = %0x", events);

	/* lock against poll() as well as other wakeups */
	ATOMIC_ENTER;

	প্রতি (unsigned i = 0; i < _max_pollwaiters; i++) {
		যদি (nullptr != _pollset[i]) {
			poll_notify_one(_pollset[i], events);
		}
	}

	ATOMIC_LEAVE;
}

শূন্য
CDev::poll_notify_one(px4_pollfd_struct_t *fds, px4_pollevent_t events)
{
	PX4_DEBUG("CDev::poll_notify_one");

	/* update the reported event set */
	fds->revents |= fds->events & events;

	PX4_DEBUG(" Events fds=%p %0x %0x %0x", fds, fds->revents, fds->events, events);

#ifdef __PX4_NUTTX

	যদি (fds->revents != 0 && fds->cb != nullptr) {
		fds->cb(fds);
	}

#অন্যথা

	যদি (fds->revents != 0) {
		px4_sem_post(fds->sem);
	}

#endif
}

পূর্ণাঙ্ক
CDev::store_poll_waiter(px4_pollfd_struct_t *fds)
{
	// Look for a free slot.
	PX4_DEBUG("CDev::store_poll_waiter");

	প্রতি (unsigned i = 0; i < _max_pollwaiters; i++) {
		যদি (nullptr == _pollset[i]) {

			/* save the pollfd */
			_pollset[i] = fds;

			প্রত্যাগম PX4_OK;
		}
	}

	প্রত্যাগম -ENFILE;
}

পূর্ণাঙ্ক
CDev::remove_poll_waiter(px4_pollfd_struct_t *fds)
{
	PX4_DEBUG("CDev::remove_poll_waiter");

	প্রতি (unsigned i = 0; i < _max_pollwaiters; i++) {
		যদি (fds == _pollset[i]) {

			_pollset[i] = nullptr;
			প্রত্যাগম PX4_OK;

		}
	}

	PX4_DEBUG("poll: bad fd state");
	প্রত্যাগম -EINVAL;
}

} // namespace cdev
