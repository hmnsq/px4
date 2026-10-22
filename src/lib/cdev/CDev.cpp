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

CDev::CDev(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *devname) :
	_devname(devname)
{
	PX4_DEBUG("CDev::CDev");

	ಪೂರ್ಣಾಂಕ ret = px4_sem_init(&_lock, 0, 1);

	ಆದರೆ (ret != 0) {
		PX4_DEBUG("SEM INIT FAIL: ret %d", ret);
	}
}

CDev::~CDev()
{
	PX4_DEBUG("CDev::~CDev");

	ಆದರೆ (_registered) {
		unregister_driver(_devname);
	}

	ಆದರೆ (_pollset) {
		ಅಳಿಸು[](_pollset);
	}

	px4_sem_destroy(&_lock);
}

ಪೂರ್ಣಾಂಕ
CDev::ರಿಜಿಸ್ಟರ್_class_devname(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *class_devname)
{
	PX4_DEBUG("CDev::register_class_devname %s", class_devname);

	ಆದರೆ (class_devname == ಶೂನ್ಯಸೂಚಕ) {
		ಹಿಂತಿರುಗಿ -EINVAL;
	}

	ಪೂರ್ಣಾಂಕ class_instance = 0;
	ಪೂರ್ಣಾಂಕ ret = -ENOSPC;

	ಆಗ (class_instance < 4) {
		ಅಕ್ಷರ name[32];
		snprintf(name, ಗಾತ್ರ(name), "%s%d", class_devname, class_instance);
		ret = ರಿಜಿಸ್ಟರ್_driver(name, &fops_ref(), 0666, (ಶೂನ್ಯ *)ಇದು);

		ಆದರೆ (ret == OK) {
			ನಿಲ್ಲಿಸು;
		}

		class_instance++;
	}

	ಆದರೆ (class_instance == 4) {
		ಹಿಂತಿರುಗಿ ret;
	}

	ಹಿಂತಿರುಗಿ class_instance;
}

ಪೂರ್ಣಾಂಕ
CDev::unregister_class_devname(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *class_devname, ಚಿಹ್ನೆರಹಿತ class_instance)
{
	PX4_DEBUG("CDev::unregister_class_devname");

	ಅಕ್ಷರ name[32];
	snprintf(name, ಗಾತ್ರ(name), "%s%u", class_devname, class_instance);
	ಹಿಂತಿರುಗಿ unregister_driver(name);
}

ಪೂರ್ಣಾಂಕ
CDev::init()
{
	PX4_DEBUG("CDev::init");

	ಪೂರ್ಣಾಂಕ ret = PX4_OK;

	// now register the driver
	ಆದರೆ (_devname != ಶೂನ್ಯಸೂಚಕ) {
		ret = ರಿಜಿಸ್ಟರ್_driver(_devname, &fops_ref(), 0666, (ಶೂನ್ಯ *)ಇದು);

		ಆದರೆ (ret == PX4_OK) {
			_registered = ನಿಜ;
		}

	} ಇಲ್ಲದಿದ್ದರೆ {
		ret = -ENODEV;
	}

	ಹಿಂತಿರುಗಿ ret;
}

/*
 * Default implementations of the character device interface
 */
ಪೂರ್ಣಾಂಕ
CDev::open(file_t *filep)
{
	PX4_DEBUG("CDev::open");
	ಪೂರ್ಣಾಂಕ ret = PX4_OK;

	ಬೀಗ();
	/* increment the open count */
	open_ಎಣಿಕೆ++;

	ಆದರೆ (open_ಎಣಿಕೆ == 1) {

		/* first-open callback may decline the open */
		ret = open_ಮೊದಲ(filep);

		ಆದರೆ (ret != PX4_OK) {
			open_ಎಣಿಕೆ--;
		}
	}

	unlock();

	ಹಿಂತಿರುಗಿ ret;
}

ಪೂರ್ಣಾಂಕ
CDev::close(file_t *filep)
{
	PX4_DEBUG("CDev::close");
	ಪೂರ್ಣಾಂಕ ret = PX4_OK;

	ಬೀಗ();

	ಆದರೆ (open_ಎಣಿಕೆ > 0) {
		/* decrement the open count */
		open_ಎಣಿಕೆ--;

		/* callback cannot decline the close */
		ಆದರೆ (open_ಎಣಿಕೆ == 0) {
			ret = close_ಕೊನೆಯ(filep);
		}

	} ಇಲ್ಲದಿದ್ದರೆ {
		ret = -EBADF;
	}

	unlock();

	ಹಿಂತಿರುಗಿ ret;
}

ಪೂರ್ಣಾಂಕ
CDev::poll(file_t *filep, px4_pollfd_struct_t *fds, ತಾರ್ಕಿಕ ಸಿದ್ಧತೆ)
{
	PX4_DEBUG("CDev::Poll %s", ಸಿದ್ಧತೆ ? "setup" : "teardown");
	ಪೂರ್ಣಾಂಕ ret = PX4_OK;

	ಆದರೆ (ಸಿದ್ಧತೆ) {
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
		ಆಗ ((ret = store_poll_waiter(fds)) == -ENFILE) {

			// No free slot found. Resize the pollset. This is expensive, but it's only needed initially.

			ಆದರೆ (_max_pollwaiters >= 256 / 2) { //_max_pollwaiters is uint8_t
				ret = -ENOMEM;
				ನಿಲ್ಲಿಸು;
			}

			ಸ್ಥಿರಾಂಕ uint8_t new_ಎಣಿಕೆ = _max_pollwaiters > 0 ? _max_pollwaiters * 2 : 1;
			px4_pollfd_struct_t **prev_pollset = _pollset;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __PX4_NUTTX
			// malloc uses a semaphore, we need to call it enabled IRQ's
			px4_leave_critical_section(flags);
#ಆದರೆ_ಅಂತ್ಯ
			px4_pollfd_struct_t **new_pollset = ಹೊಸ px4_pollfd_struct_t *[new_ಎಣಿಕೆ];

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __PX4_NUTTX
			flags = px4_enter_critical_section();
#ಆದರೆ_ಅಂತ್ಯ

			ಆದರೆ (prev_pollset == _pollset) {
				// no one else updated the _pollset meanwhile, so we're good to go
				ಆದರೆ (!new_pollset) {
					ret = -ENOMEM;
					ನಿಲ್ಲಿಸು;
				}

				ಆದರೆ (_max_pollwaiters > 0) {
					memset(new_pollset + _max_pollwaiters, 0, ಗಾತ್ರ(px4_pollfd_struct_t *) * (new_ಎಣಿಕೆ - _max_pollwaiters));
					ಸ್ಮರಣೆ_ನಕಲು(new_pollset, _pollset, ಗಾತ್ರ(px4_pollfd_struct_t *) * _max_pollwaiters);
				}

				_pollset = new_pollset;
				_pollset[_max_pollwaiters] = fds;
				_max_pollwaiters = new_ಎಣಿಕೆ;

				// free the previous _pollset (we need to unlock here which is fine because we don't access _pollset anymore)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __PX4_NUTTX
				px4_leave_critical_section(flags);
#ಆದರೆ_ಅಂತ್ಯ

				ಆದರೆ (prev_pollset) {
					ಅಳಿಸು[](prev_pollset);
				}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __PX4_NUTTX
				flags = px4_enter_critical_section();
#ಆದರೆ_ಅಂತ್ಯ

				// Success
				ret = PX4_OK;
				ನಿಲ್ಲಿಸು;
			}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __PX4_NUTTX
			px4_leave_critical_section(flags);
#ಆದರೆ_ಅಂತ್ಯ
			// We have to retry
			ಅಳಿಸು[] new_pollset;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __PX4_NUTTX
			flags = px4_enter_critical_section();
#ಆದರೆ_ಅಂತ್ಯ
		}

		ಆದರೆ (ret == PX4_OK) {

			/*
			 * Check to see whether we should send a poll notification
			 * immediately.
			 */
			fds->revents |= fds->events & poll_state(filep);

			/* yes? invoke the callback */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __PX4_NUTTX

			ಆದರೆ (fds->revents != 0 && fds->cb != ಶೂನ್ಯಸೂಚಕ) {
				fds->cb(fds);
			}

#ಇಲ್ಲದಿದ್ದರೆ

			ಆದರೆ (fds->revents != 0) {
				px4_sem_post(fds->sem);
			}

#ಆದರೆ_ಅಂತ್ಯ

		}

		ATOMIC_LEAVE;

	} ಇಲ್ಲದಿದ್ದರೆ {
		ATOMIC_ENTER;
		/*
		 * Handle a teardown request.
		 */
		ret = remove_poll_waiter(fds);
		ATOMIC_LEAVE;
	}

	ಹಿಂತಿರುಗಿ ret;
}

ಶೂನ್ಯ
CDev::poll_notify(px4_pollevent_t events)
{
	PX4_DEBUG("CDev::poll_notify events = %0x", events);

	/* lock against poll() as well as other wakeups */
	ATOMIC_ENTER;

	ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ i = 0; i < _max_pollwaiters; i++) {
		ಆದರೆ (ಶೂನ್ಯಸೂಚಕ != _pollset[i]) {
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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __PX4_NUTTX

	ಆದರೆ (fds->revents != 0 && fds->cb != ಶೂನ್ಯಸೂಚಕ) {
		fds->cb(fds);
	}

#ಇಲ್ಲದಿದ್ದರೆ

	ಆದರೆ (fds->revents != 0) {
		px4_sem_post(fds->sem);
	}

#ಆದರೆ_ಅಂತ್ಯ
}

ಪೂರ್ಣಾಂಕ
CDev::store_poll_waiter(px4_pollfd_struct_t *fds)
{
	// Look for a free slot.
	PX4_DEBUG("CDev::store_poll_waiter");

	ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ i = 0; i < _max_pollwaiters; i++) {
		ಆದರೆ (ಶೂನ್ಯಸೂಚಕ == _pollset[i]) {

			/* save the pollfd */
			_pollset[i] = fds;

			ಹಿಂತಿರುಗಿ PX4_OK;
		}
	}

	ಹಿಂತಿರುಗಿ -ENFILE;
}

ಪೂರ್ಣಾಂಕ
CDev::remove_poll_waiter(px4_pollfd_struct_t *fds)
{
	PX4_DEBUG("CDev::remove_poll_waiter");

	ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ i = 0; i < _max_pollwaiters; i++) {
		ಆದರೆ (fds == _pollset[i]) {

			_pollset[i] = ಶೂನ್ಯಸೂಚಕ;
			ಹಿಂತಿರುಗಿ PX4_OK;

		}
	}

	PX4_DEBUG("poll: bad fd state");
	ಹಿಂತಿರುಗಿ -EINVAL;
}

} // namespace cdev
