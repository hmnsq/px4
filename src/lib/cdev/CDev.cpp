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

#அந்தர்பாவ "CDev.hpp"

#அந்தர்பாவ <cstring>

#அந்தர்பாவ <px4_platform_common/log.h>
#அந்தர்பாவ <px4_platform_common/posix.h>

நாமாகாஶ cdev
{

CDev::CDev(மாறிலி எழுத்து *devname) :
	_devname(devname)
{
	PX4_DEBUG("CDev::CDev");

	முழு ret = px4_sem_init(&_lock, 0, 1);

	எனில் (ret != 0) {
		PX4_DEBUG("SEM INIT FAIL: ret %d", ret);
	}
}

CDev::~CDev()
{
	PX4_DEBUG("CDev::~CDev");

	எனில் (_registered) {
		unregister_driver(_devname);
	}

	எனில் (_pollset) {
		delete[](_pollset);
	}

	px4_sem_destroy(&_lock);
}

முழு
CDev::register_class_devname(மாறிலி எழுத்து *class_devname)
{
	PX4_DEBUG("CDev::register_class_devname %s", class_devname);

	எனில் (class_devname == nullptr) {
		திரும்பு -EINVAL;
	}

	முழு class_instance = 0;
	முழு ret = -ENOSPC;

	போது (class_instance < 4) {
		எழுத்து name[32];
		snprintf(name, அளவு(name), "%s%d", class_devname, class_instance);
		ret = register_driver(name, &fops_ref(), 0666, (வெறுமை *)இது);

		எனில் (ret == OK) {
			நிறுத்து;
		}

		class_instance++;
	}

	எனில் (class_instance == 4) {
		திரும்பு ret;
	}

	திரும்பு class_instance;
}

முழு
CDev::unregister_class_devname(மாறிலி எழுத்து *class_devname, unsigned class_instance)
{
	PX4_DEBUG("CDev::unregister_class_devname");

	எழுத்து name[32];
	snprintf(name, அளவு(name), "%s%u", class_devname, class_instance);
	திரும்பு unregister_driver(name);
}

முழு
CDev::init()
{
	PX4_DEBUG("CDev::init");

	முழு ret = PX4_OK;

	// now register the driver
	எனில் (_devname != nullptr) {
		ret = register_driver(_devname, &fops_ref(), 0666, (வெறுமை *)இது);

		எனில் (ret == PX4_OK) {
			_registered = உண்மை;
		}

	} இல்லையெனில் {
		ret = -ENODEV;
	}

	திரும்பு ret;
}

/*
 * Default implementations of the character device interface
 */
முழு
CDev::open(file_t *filep)
{
	PX4_DEBUG("CDev::open");
	முழு ret = PX4_OK;

	lock();
	/* increment the open count */
	open_மொத்தம்++;

	எனில் (open_மொத்தம் == 1) {

		/* first-open callback may decline the open */
		ret = open_first(filep);

		எனில் (ret != PX4_OK) {
			open_மொத்தம்--;
		}
	}

	unlock();

	திரும்பு ret;
}

முழு
CDev::close(file_t *filep)
{
	PX4_DEBUG("CDev::close");
	முழு ret = PX4_OK;

	lock();

	எனில் (open_மொத்தம் > 0) {
		/* decrement the open count */
		open_மொத்தம்--;

		/* callback cannot decline the close */
		எனில் (open_மொத்தம் == 0) {
			ret = close_last(filep);
		}

	} இல்லையெனில் {
		ret = -EBADF;
	}

	unlock();

	திரும்பு ret;
}

முழு
CDev::poll(file_t *filep, px4_pollfd_struct_t *fds, தருக்க setup)
{
	PX4_DEBUG("CDev::Poll %s", setup ? "setup" : "teardown");
	முழு ret = PX4_OK;

	எனில் (setup) {
		/*
		 * Save the file pointer in the pollfd for the subclass'
		 * benefit.
		 */
		fds->priv = (வெறுமை *)filep;
		PX4_DEBUG("CDev::poll: fds->priv = %p", filep);

		/*
		 * Lock against poll_notify() and possibly other callers (protect _pollset).
		 */
		ATOMIC_ENTER;

		/*
		 * Try to store the fds for later use and handle array resizing.
		 */
		போது ((ret = store_poll_waiter(fds)) == -ENFILE) {

			// No free slot found. Resize the pollset. This is expensive, but it's only needed initially.

			எனில் (_max_pollwaiters >= 256 / 2) { //_max_pollwaiters is uint8_t
				ret = -ENOMEM;
				நிறுத்து;
			}

			மாறிலி uint8_t new_மொத்தம் = _max_pollwaiters > 0 ? _max_pollwaiters * 2 : 1;
			px4_pollfd_struct_t **prev_pollset = _pollset;

#ifdef __PX4_NUTTX
			// malloc uses a semaphore, we need to call it enabled IRQ's
			px4_leave_critical_section(flags);
#endif
			px4_pollfd_struct_t **new_pollset = புதிய px4_pollfd_struct_t *[new_மொத்தம்];

#ifdef __PX4_NUTTX
			flags = px4_enter_critical_section();
#endif

			எனில் (prev_pollset == _pollset) {
				// no one else updated the _pollset meanwhile, so we're good to go
				எனில் (!new_pollset) {
					ret = -ENOMEM;
					நிறுத்து;
				}

				எனில் (_max_pollwaiters > 0) {
					memset(new_pollset + _max_pollwaiters, 0, அளவு(px4_pollfd_struct_t *) * (new_மொத்தம் - _max_pollwaiters));
					memcpy(new_pollset, _pollset, அளவு(px4_pollfd_struct_t *) * _max_pollwaiters);
				}

				_pollset = new_pollset;
				_pollset[_max_pollwaiters] = fds;
				_max_pollwaiters = new_மொத்தம்;

				// free the previous _pollset (we need to unlock here which is fine because we don't access _pollset anymore)
#ifdef __PX4_NUTTX
				px4_leave_critical_section(flags);
#endif

				எனில் (prev_pollset) {
					delete[](prev_pollset);
				}

#ifdef __PX4_NUTTX
				flags = px4_enter_critical_section();
#endif

				// Success
				ret = PX4_OK;
				நிறுத்து;
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

		எனில் (ret == PX4_OK) {

			/*
			 * Check to see whether we should send a poll notification
			 * immediately.
			 */
			fds->revents |= fds->events & poll_state(filep);

			/* yes? invoke the callback */
#ifdef __PX4_NUTTX

			எனில் (fds->revents != 0 && fds->cb != nullptr) {
				fds->cb(fds);
			}

#இல்லையெனில்

			எனில் (fds->revents != 0) {
				px4_sem_post(fds->sem);
			}

#endif

		}

		ATOMIC_LEAVE;

	} இல்லையெனில் {
		ATOMIC_ENTER;
		/*
		 * Handle a teardown request.
		 */
		ret = remove_poll_waiter(fds);
		ATOMIC_LEAVE;
	}

	திரும்பு ret;
}

வெறுமை
CDev::poll_notify(px4_pollevent_t events)
{
	PX4_DEBUG("CDev::poll_notify events = %0x", events);

	/* lock against poll() as well as other wakeups */
	ATOMIC_ENTER;

	க்கு (unsigned i = 0; i < _max_pollwaiters; i++) {
		எனில் (nullptr != _pollset[i]) {
			poll_notify_one(_pollset[i], events);
		}
	}

	ATOMIC_LEAVE;
}

வெறுமை
CDev::poll_notify_one(px4_pollfd_struct_t *fds, px4_pollevent_t events)
{
	PX4_DEBUG("CDev::poll_notify_one");

	/* update the reported event set */
	fds->revents |= fds->events & events;

	PX4_DEBUG(" Events fds=%p %0x %0x %0x", fds, fds->revents, fds->events, events);

#ifdef __PX4_NUTTX

	எனில் (fds->revents != 0 && fds->cb != nullptr) {
		fds->cb(fds);
	}

#இல்லையெனில்

	எனில் (fds->revents != 0) {
		px4_sem_post(fds->sem);
	}

#endif
}

முழு
CDev::store_poll_waiter(px4_pollfd_struct_t *fds)
{
	// Look for a free slot.
	PX4_DEBUG("CDev::store_poll_waiter");

	க்கு (unsigned i = 0; i < _max_pollwaiters; i++) {
		எனில் (nullptr == _pollset[i]) {

			/* save the pollfd */
			_pollset[i] = fds;

			திரும்பு PX4_OK;
		}
	}

	திரும்பு -ENFILE;
}

முழு
CDev::remove_poll_waiter(px4_pollfd_struct_t *fds)
{
	PX4_DEBUG("CDev::remove_poll_waiter");

	க்கு (unsigned i = 0; i < _max_pollwaiters; i++) {
		எனில் (fds == _pollset[i]) {

			_pollset[i] = nullptr;
			திரும்பு PX4_OK;

		}
	}

	PX4_DEBUG("poll: bad fd state");
	திரும்பு -EINVAL;
}

} // namespace cdev
