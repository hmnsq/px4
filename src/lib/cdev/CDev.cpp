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

#ಅನ್ತರ್ಭಾವ "CDev.hpp"

#ಅನ್ತರ್ಭಾವ <cstring>

#ಅನ್ತರ್ಭಾವ <px4_platform_common/log.h>
#ಅನ್ತರ್ಭಾವ <px4_platform_common/posix.h>

ನಾಮಾಕಾಶ cdev
{

CDev::CDev(ಧ್ರುವ ವರ್ಣ *devname) :
	_devname(devname)
{
	PX4_DEBUG("CDev::CDev");

	ಪೂರ್ಣಾಙ್ಕ ret = px4_sem_init(&_lock, 0, 1);

	ಯದಿ (ret != 0) {
		PX4_DEBUG("SEM INIT FAIL: ret %d", ret);
	}
}

CDev::~CDev()
{
	PX4_DEBUG("CDev::~CDev");

	ಯದಿ (_registered) {
		unregister_driver(_devname);
	}

	ಯದಿ (_pollset) {
		delete[](_pollset);
	}

	px4_sem_destroy(&_lock);
}

ಪೂರ್ಣಾಙ್ಕ
CDev::register_class_devname(ಧ್ರುವ ವರ್ಣ *class_devname)
{
	PX4_DEBUG("CDev::register_class_devname %s", class_devname);

	ಯದಿ (class_devname == nullptr) {
		ಪ್ರತ್ಯಾಗಮ -EINVAL;
	}

	ಪೂರ್ಣಾಙ್ಕ class_instance = 0;
	ಪೂರ್ಣಾಙ್ಕ ret = -ENOSPC;

	ಯಾವತ್ (class_instance < 4) {
		ವರ್ಣ name[32];
		snprintf(name, ಪರಿಮಾಣ(name), "%s%d", class_devname, class_instance);
		ret = register_driver(name, &fops_ref(), 0666, (ಶೂನ್ಯ *)ಏತತ್);

		ಯದಿ (ret == OK) {
			ಭಙ್ಗ;
		}

		class_instance++;
	}

	ಯದಿ (class_instance == 4) {
		ಪ್ರತ್ಯಾಗಮ ret;
	}

	ಪ್ರತ್ಯಾಗಮ class_instance;
}

ಪೂರ್ಣಾಙ್ಕ
CDev::unregister_class_devname(ಧ್ರುವ ವರ್ಣ *class_devname, unsigned class_instance)
{
	PX4_DEBUG("CDev::unregister_class_devname");

	ವರ್ಣ name[32];
	snprintf(name, ಪರಿಮಾಣ(name), "%s%u", class_devname, class_instance);
	ಪ್ರತ್ಯಾಗಮ unregister_driver(name);
}

ಪೂರ್ಣಾಙ್ಕ
CDev::init()
{
	PX4_DEBUG("CDev::init");

	ಪೂರ್ಣಾಙ್ಕ ret = PX4_OK;

	// now register the driver
	ಯದಿ (_devname != nullptr) {
		ret = register_driver(_devname, &fops_ref(), 0666, (ಶೂನ್ಯ *)ಏತತ್);

		ಯದಿ (ret == PX4_OK) {
			_registered = ಸತ್ಯ;
		}

	} ಅನ್ಯಥಾ {
		ret = -ENODEV;
	}

	ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Default implementations of the character device interface
 */
ಪೂರ್ಣಾಙ್ಕ
CDev::open(file_t *filep)
{
	PX4_DEBUG("CDev::open");
	ಪೂರ್ಣಾಙ್ಕ ret = PX4_OK;

	lock();
	/* increment the open count */
	open_ಎಣಿಕೆ++;

	ಯದಿ (open_ಎಣಿಕೆ == 1) {

		/* first-open callback may decline the open */
		ret = open_first(filep);

		ಯದಿ (ret != PX4_OK) {
			open_ಎಣಿಕೆ--;
		}
	}

	unlock();

	ಪ್ರತ್ಯಾಗಮ ret;
}

ಪೂರ್ಣಾಙ್ಕ
CDev::close(file_t *filep)
{
	PX4_DEBUG("CDev::close");
	ಪೂರ್ಣಾಙ್ಕ ret = PX4_OK;

	lock();

	ಯದಿ (open_ಎಣಿಕೆ > 0) {
		/* decrement the open count */
		open_ಎಣಿಕೆ--;

		/* callback cannot decline the close */
		ಯದಿ (open_ಎಣಿಕೆ == 0) {
			ret = close_last(filep);
		}

	} ಅನ್ಯಥಾ {
		ret = -EBADF;
	}

	unlock();

	ಪ್ರತ್ಯಾಗಮ ret;
}

ಪೂರ್ಣಾಙ್ಕ
CDev::poll(file_t *filep, px4_pollfd_struct_t *fds, ತಾರ್ಕಿಕ setup)
{
	PX4_DEBUG("CDev::Poll %s", setup ? "setup" : "teardown");
	ಪೂರ್ಣಾಙ್ಕ ret = PX4_OK;

	ಯದಿ (setup) {
		/*
		 * Save the file pointer in the pollfd for the subclass'
		 * benefit.
		 */
		fds->priv = (ಶೂನ್ಯ *)filep;
		PX4_DEBUG("CDev::poll: fds->priv = %p", filep);

		/*
		 * Lock against poll_notify() and possibly other callers (protect _pollset).
		 */
		ATOMIC_ENTER;

		/*
		 * Try to store the fds for later use and handle array resizing.
		 */
		ಯಾವತ್ ((ret = store_poll_waiter(fds)) == -ENFILE) {

			// No free slot found. Resize the pollset. This is expensive, but it's only needed initially.

			ಯದಿ (_max_pollwaiters >= 256 / 2) { //_max_pollwaiters is uint8_t
				ret = -ENOMEM;
				ಭಙ್ಗ;
			}

			ಧ್ರುವ uint8_t new_ಎಣಿಕೆ = _max_pollwaiters > 0 ? _max_pollwaiters * 2 : 1;
			px4_pollfd_struct_t **prev_pollset = _pollset;

#ifdef __PX4_NUTTX
			// malloc uses a semaphore, we need to call it enabled IRQ's
			px4_leave_critical_section(flags);
#endif
			px4_pollfd_struct_t **new_pollset = ನೂತನ px4_pollfd_struct_t *[new_ಎಣಿಕೆ];

#ifdef __PX4_NUTTX
			flags = px4_enter_critical_section();
#endif

			ಯದಿ (prev_pollset == _pollset) {
				// no one else updated the _pollset meanwhile, so we're good to go
				ಯದಿ (!new_pollset) {
					ret = -ENOMEM;
					ಭಙ್ಗ;
				}

				ಯದಿ (_max_pollwaiters > 0) {
					memset(new_pollset + _max_pollwaiters, 0, ಪರಿಮಾಣ(px4_pollfd_struct_t *) * (new_ಎಣಿಕೆ - _max_pollwaiters));
					memcpy(new_pollset, _pollset, ಪರಿಮಾಣ(px4_pollfd_struct_t *) * _max_pollwaiters);
				}

				_pollset = new_pollset;
				_pollset[_max_pollwaiters] = fds;
				_max_pollwaiters = new_ಎಣಿಕೆ;

				// free the previous _pollset (we need to unlock here which is fine because we don't access _pollset anymore)
#ifdef __PX4_NUTTX
				px4_leave_critical_section(flags);
#endif

				ಯದಿ (prev_pollset) {
					delete[](prev_pollset);
				}

#ifdef __PX4_NUTTX
				flags = px4_enter_critical_section();
#endif

				// Success
				ret = PX4_OK;
				ಭಙ್ಗ;
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

		ಯದಿ (ret == PX4_OK) {

			/*
			 * Check to see whether we should send a poll notification
			 * immediately.
			 */
			fds->revents |= fds->events & poll_state(filep);

			/* yes? invoke the callback */
#ifdef __PX4_NUTTX

			ಯದಿ (fds->revents != 0 && fds->cb != nullptr) {
				fds->cb(fds);
			}

#ಅನ್ಯಥಾ

			ಯದಿ (fds->revents != 0) {
				px4_sem_post(fds->sem);
			}

#endif

		}

		ATOMIC_LEAVE;

	} ಅನ್ಯಥಾ {
		ATOMIC_ENTER;
		/*
		 * Handle a teardown request.
		 */
		ret = remove_poll_waiter(fds);
		ATOMIC_LEAVE;
	}

	ಪ್ರತ್ಯಾಗಮ ret;
}

ಶೂನ್ಯ
CDev::poll_notify(px4_pollevent_t events)
{
	PX4_DEBUG("CDev::poll_notify events = %0x", events);

	/* lock against poll() as well as other wakeups */
	ATOMIC_ENTER;

	ಪ್ರತಿ (unsigned i = 0; i < _max_pollwaiters; i++) {
		ಯದಿ (nullptr != _pollset[i]) {
			poll_notify_one(_pollset[i], events);
		}
	}

	ATOMIC_LEAVE;
}

ಶೂನ್ಯ
CDev::poll_notify_one(px4_pollfd_struct_t *fds, px4_pollevent_t events)
{
	PX4_DEBUG("CDev::poll_notify_one");

	/* update the reported event set */
	fds->revents |= fds->events & events;

	PX4_DEBUG(" Events fds=%p %0x %0x %0x", fds, fds->revents, fds->events, events);

#ifdef __PX4_NUTTX

	ಯದಿ (fds->revents != 0 && fds->cb != nullptr) {
		fds->cb(fds);
	}

#ಅನ್ಯಥಾ

	ಯದಿ (fds->revents != 0) {
		px4_sem_post(fds->sem);
	}

#endif
}

ಪೂರ್ಣಾಙ್ಕ
CDev::store_poll_waiter(px4_pollfd_struct_t *fds)
{
	// Look for a free slot.
	PX4_DEBUG("CDev::store_poll_waiter");

	ಪ್ರತಿ (unsigned i = 0; i < _max_pollwaiters; i++) {
		ಯದಿ (nullptr == _pollset[i]) {

			/* save the pollfd */
			_pollset[i] = fds;

			ಪ್ರತ್ಯಾಗಮ PX4_OK;
		}
	}

	ಪ್ರತ್ಯಾಗಮ -ENFILE;
}

ಪೂರ್ಣಾಙ್ಕ
CDev::remove_poll_waiter(px4_pollfd_struct_t *fds)
{
	PX4_DEBUG("CDev::remove_poll_waiter");

	ಪ್ರತಿ (unsigned i = 0; i < _max_pollwaiters; i++) {
		ಯದಿ (fds == _pollset[i]) {

			_pollset[i] = nullptr;
			ಪ್ರತ್ಯಾಗಮ PX4_OK;

		}
	}

	PX4_DEBUG("poll: bad fd state");
	ಪ್ರತ್ಯಾಗಮ -EINVAL;
}

} // namespace cdev
