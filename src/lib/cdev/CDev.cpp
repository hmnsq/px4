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

#ضمن "CDev.hpp"

#ضمن <cstring>

#ضمن <px4_platform_common/log.h>
#ضمن <px4_platform_common/posix.h>

نطاق cdev
{

CDev::CDev(ثابت حرف *devname) :
	_devname(devname)
{
	PX4_DEBUG("CDev::CDev");

	صحيح ret = px4_sem_init(&_lock, 0, 1);

	إذا (ret != 0) {
		PX4_DEBUG("SEM INIT FAIL: ret %d", ret);
	}
}

CDev::~CDev()
{
	PX4_DEBUG("CDev::~CDev");

	إذا (_registered) {
		unregister_driver(_devname);
	}

	إذا (_pollset) {
		حذف[](_pollset);
	}

	px4_sem_destroy(&_lock);
}

صحيح
CDev::ثبات_class_devname(ثابت حرف *class_devname)
{
	PX4_DEBUG("CDev::register_class_devname %s", class_devname);

	إذا (class_devname == اشاره_تهی) {
		أرجع -EINVAL;
	}

	صحيح class_instance = 0;
	صحيح ret = -ENOSPC;

	طالما (class_instance < 4) {
		حرف name[32];
		snprintf(name, حجم(name), "%s%d", class_devname, class_instance);
		ret = ثبات_driver(name, &fops_ref(), 0666, (فراغ *)هذا);

		إذا (ret == OK) {
			اكسر;
		}

		class_instance++;
	}

	إذا (class_instance == 4) {
		أرجع ret;
	}

	أرجع class_instance;
}

صحيح
CDev::unregister_class_devname(ثابت حرف *class_devname, بیعلامت class_instance)
{
	PX4_DEBUG("CDev::unregister_class_devname");

	حرف name[32];
	snprintf(name, حجم(name), "%s%u", class_devname, class_instance);
	أرجع unregister_driver(name);
}

صحيح
CDev::init()
{
	PX4_DEBUG("CDev::init");

	صحيح ret = PX4_OK;

	// now register the driver
	إذا (_devname != اشاره_تهی) {
		ret = ثبات_driver(_devname, &fops_ref(), 0666, (فراغ *)هذا);

		إذا (ret == PX4_OK) {
			_registered = درست;
		}

	} وإلا {
		ret = -ENODEV;
	}

	أرجع ret;
}

/*
 * Default implementations of the character device interface
 */
صحيح
CDev::open(file_t *filep)
{
	PX4_DEBUG("CDev::open");
	صحيح ret = PX4_OK;

	قفل();
	/* increment the open count */
	open_شمار++;

	إذا (open_شمار == 1) {

		/* first-open callback may decline the open */
		ret = open_اولی(filep);

		إذا (ret != PX4_OK) {
			open_شمار--;
		}
	}

	unlock();

	أرجع ret;
}

صحيح
CDev::close(file_t *filep)
{
	PX4_DEBUG("CDev::close");
	صحيح ret = PX4_OK;

	قفل();

	إذا (open_شمار > 0) {
		/* decrement the open count */
		open_شمار--;

		/* callback cannot decline the close */
		إذا (open_شمار == 0) {
			ret = close_آخرین(filep);
		}

	} وإلا {
		ret = -EBADF;
	}

	unlock();

	أرجع ret;
}

صحيح
CDev::poll(file_t *filep, px4_pollfd_struct_t *fds, منطقي راهاندازی)
{
	PX4_DEBUG("CDev::Poll %s", راهاندازی ? "setup" : "teardown");
	صحيح ret = PX4_OK;

	إذا (راهاندازی) {
		/*
		 * Save the file pointer in the pollfd for the subclass'
		 * benefit.
		 */
		fds->priv = (فراغ *)filep;
		PX4_DEBUG("CDev::poll: fds->priv = %p", filep);

		/*
		 * Lock against poll_notify() and possibly other callers (protect _pollset).
		 */
		ATOMIC_ENTER;

		/*
		 * Try to store the fds for later use and handle array resizing.
		 */
		طالما ((ret = store_poll_waiter(fds)) == -ENFILE) {

			// No free slot found. Resize the pollset. This is expensive, but it's only needed initially.

			إذا (_max_pollwaiters >= 256 / 2) { //_max_pollwaiters is uint8_t
				ret = -ENOMEM;
				اكسر;
			}

			ثابت uint8_t new_شمار = _max_pollwaiters > 0 ? _max_pollwaiters * 2 : 1;
			px4_pollfd_struct_t **prev_pollset = _pollset;

#اگر_تعریف_شده __PX4_NUTTX
			// malloc uses a semaphore, we need to call it enabled IRQ's
			px4_leave_critical_section(flags);
#پایان_اگر
			px4_pollfd_struct_t **new_pollset = جديد px4_pollfd_struct_t *[new_شمار];

#اگر_تعریف_شده __PX4_NUTTX
			flags = px4_enter_critical_section();
#پایان_اگر

			إذا (prev_pollset == _pollset) {
				// no one else updated the _pollset meanwhile, so we're good to go
				إذا (!new_pollset) {
					ret = -ENOMEM;
					اكسر;
				}

				إذا (_max_pollwaiters > 0) {
					memset(new_pollset + _max_pollwaiters, 0, حجم(px4_pollfd_struct_t *) * (new_شمار - _max_pollwaiters));
					کپی_حافظه(new_pollset, _pollset, حجم(px4_pollfd_struct_t *) * _max_pollwaiters);
				}

				_pollset = new_pollset;
				_pollset[_max_pollwaiters] = fds;
				_max_pollwaiters = new_شمار;

				// free the previous _pollset (we need to unlock here which is fine because we don't access _pollset anymore)
#اگر_تعریف_شده __PX4_NUTTX
				px4_leave_critical_section(flags);
#پایان_اگر

				إذا (prev_pollset) {
					حذف[](prev_pollset);
				}

#اگر_تعریف_شده __PX4_NUTTX
				flags = px4_enter_critical_section();
#پایان_اگر

				// Success
				ret = PX4_OK;
				اكسر;
			}

#اگر_تعریف_شده __PX4_NUTTX
			px4_leave_critical_section(flags);
#پایان_اگر
			// We have to retry
			حذف[] new_pollset;
#اگر_تعریف_شده __PX4_NUTTX
			flags = px4_enter_critical_section();
#پایان_اگر
		}

		إذا (ret == PX4_OK) {

			/*
			 * Check to see whether we should send a poll notification
			 * immediately.
			 */
			fds->revents |= fds->events & poll_state(filep);

			/* yes? invoke the callback */
#اگر_تعریف_شده __PX4_NUTTX

			إذا (fds->revents != 0 && fds->cb != اشاره_تهی) {
				fds->cb(fds);
			}

#وإلا

			إذا (fds->revents != 0) {
				px4_sem_post(fds->sem);
			}

#پایان_اگر

		}

		ATOMIC_LEAVE;

	} وإلا {
		ATOMIC_ENTER;
		/*
		 * Handle a teardown request.
		 */
		ret = remove_poll_waiter(fds);
		ATOMIC_LEAVE;
	}

	أرجع ret;
}

فراغ
CDev::poll_notify(px4_pollevent_t events)
{
	PX4_DEBUG("CDev::poll_notify events = %0x", events);

	/* lock against poll() as well as other wakeups */
	ATOMIC_ENTER;

	لكل (بیعلامت i = 0; i < _max_pollwaiters; i++) {
		إذا (اشاره_تهی != _pollset[i]) {
			poll_notify_one(_pollset[i], events);
		}
	}

	ATOMIC_LEAVE;
}

فراغ
CDev::poll_notify_one(px4_pollfd_struct_t *fds, px4_pollevent_t events)
{
	PX4_DEBUG("CDev::poll_notify_one");

	/* update the reported event set */
	fds->revents |= fds->events & events;

	PX4_DEBUG(" Events fds=%p %0x %0x %0x", fds, fds->revents, fds->events, events);

#اگر_تعریف_شده __PX4_NUTTX

	إذا (fds->revents != 0 && fds->cb != اشاره_تهی) {
		fds->cb(fds);
	}

#وإلا

	إذا (fds->revents != 0) {
		px4_sem_post(fds->sem);
	}

#پایان_اگر
}

صحيح
CDev::store_poll_waiter(px4_pollfd_struct_t *fds)
{
	// Look for a free slot.
	PX4_DEBUG("CDev::store_poll_waiter");

	لكل (بیعلامت i = 0; i < _max_pollwaiters; i++) {
		إذا (اشاره_تهی == _pollset[i]) {

			/* save the pollfd */
			_pollset[i] = fds;

			أرجع PX4_OK;
		}
	}

	أرجع -ENFILE;
}

صحيح
CDev::remove_poll_waiter(px4_pollfd_struct_t *fds)
{
	PX4_DEBUG("CDev::remove_poll_waiter");

	لكل (بیعلامت i = 0; i < _max_pollwaiters; i++) {
		إذا (fds == _pollset[i]) {

			_pollset[i] = اشاره_تهی;
			أرجع PX4_OK;

		}
	}

	PX4_DEBUG("poll: bad fd state");
	أرجع -EINVAL;
}

} // namespace cdev
