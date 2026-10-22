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

نامخلا cdev
{

CDev::CDev(ثابت حرف *devname) :
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
		حذف[](_pollset);
	}

	px4_sem_destroy(&_lock);
}

صحیح
CDev::رجسٹر_class_devname(ثابت حرف *class_devname)
{
	PX4_DEBUG("CDev::register_class_devname %s", class_devname);

	اگر (class_devname == خالی_اشاریہ) {
		واپس -EINVAL;
	}

	صحیح class_instance = 0;
	صحیح ret = -ENOSPC;

	جب (class_instance < 4) {
		حرف name[32];
		snprintf(name, حجم(name), "%s%d", class_devname, class_instance);
		ret = رجسٹر_driver(name, &fops_ref(), 0666, (خالی *)یہ);

		اگر (ret == OK) {
			توڑو;
		}

		class_instance++;
	}

	اگر (class_instance == 4) {
		واپس ret;
	}

	واپس class_instance;
}

صحیح
CDev::unregister_class_devname(ثابت حرف *class_devname, غیرعلامتی class_instance)
{
	PX4_DEBUG("CDev::unregister_class_devname");

	حرف name[32];
	snprintf(name, حجم(name), "%s%u", class_devname, class_instance);
	واپس unregister_driver(name);
}

صحیح
CDev::init()
{
	PX4_DEBUG("CDev::init");

	صحیح ret = PX4_OK;

	// now register the driver
	اگر (_devname != خالی_اشاریہ) {
		ret = رجسٹر_driver(_devname, &fops_ref(), 0666, (خالی *)یہ);

		اگر (ret == PX4_OK) {
			_registered = سچ;
		}

	} ورنہ {
		ret = -ENODEV;
	}

	واپس ret;
}

/*
 * Default implementations of the character device interface
 */
صحیح
CDev::open(file_t *filep)
{
	PX4_DEBUG("CDev::open");
	صحیح ret = PX4_OK;

	تالا();
	/* increment the open count */
	open_گنتی++;

	اگر (open_گنتی == 1) {

		/* first-open callback may decline the open */
		ret = open_پہلا(filep);

		اگر (ret != PX4_OK) {
			open_گنتی--;
		}
	}

	unlock();

	واپس ret;
}

صحیح
CDev::close(file_t *filep)
{
	PX4_DEBUG("CDev::close");
	صحیح ret = PX4_OK;

	تالا();

	اگر (open_گنتی > 0) {
		/* decrement the open count */
		open_گنتی--;

		/* callback cannot decline the close */
		اگر (open_گنتی == 0) {
			ret = close_آخری(filep);
		}

	} ورنہ {
		ret = -EBADF;
	}

	unlock();

	واپس ret;
}

صحیح
CDev::poll(file_t *filep, px4_pollfd_struct_t *fds, منطقی تیاری)
{
	PX4_DEBUG("CDev::Poll %s", تیاری ? "setup" : "teardown");
	صحیح ret = PX4_OK;

	اگر (تیاری) {
		/*
		 * Save the file pointer in the pollfd for the subclass'
		 * benefit.
		 */
		fds->priv = (خالی *)filep;
		PX4_DEBUG("CDev::poll: fds->priv = %p", filep);

		/*
		 * Lock against poll_notify() and possibly other callers (protect _pollset).
		 */
		ATOMIC_ENTER;

		/*
		 * Try to store the fds for later use and handle array resizing.
		 */
		جب ((ret = store_poll_waiter(fds)) == -ENFILE) {

			// No free slot found. Resize the pollset. This is expensive, but it's only needed initially.

			اگر (_max_pollwaiters >= 256 / 2) { //_max_pollwaiters is uint8_t
				ret = -ENOMEM;
				توڑو;
			}

			ثابت uint8_t new_گنتی = _max_pollwaiters > 0 ? _max_pollwaiters * 2 : 1;
			px4_pollfd_struct_t **پچھلا_pollset = _pollset;

#اگر_تعریف_شدہ __PX4_NUTTX
			// malloc uses a semaphore, we need to call it enabled IRQ's
			px4_leave_critical_section(flags);
#اگر_ختم
			px4_pollfd_struct_t **new_pollset = نیا px4_pollfd_struct_t *[new_گنتی];

#اگر_تعریف_شدہ __PX4_NUTTX
			flags = px4_enter_critical_section();
#اگر_ختم

			اگر (پچھلا_pollset == _pollset) {
				// no one else updated the _pollset meanwhile, so we're good to go
				اگر (!new_pollset) {
					ret = -ENOMEM;
					توڑو;
				}

				اگر (_max_pollwaiters > 0) {
					memset(new_pollset + _max_pollwaiters, 0, حجم(px4_pollfd_struct_t *) * (new_گنتی - _max_pollwaiters));
					حافظہ_نقل(new_pollset, _pollset, حجم(px4_pollfd_struct_t *) * _max_pollwaiters);
				}

				_pollset = new_pollset;
				_pollset[_max_pollwaiters] = fds;
				_max_pollwaiters = new_گنتی;

				// free the previous _pollset (we need to unlock here which is fine because we don't access _pollset anymore)
#اگر_تعریف_شدہ __PX4_NUTTX
				px4_leave_critical_section(flags);
#اگر_ختم

				اگر (پچھلا_pollset) {
					حذف[](پچھلا_pollset);
				}

#اگر_تعریف_شدہ __PX4_NUTTX
				flags = px4_enter_critical_section();
#اگر_ختم

				// Success
				ret = PX4_OK;
				توڑو;
			}

#اگر_تعریف_شدہ __PX4_NUTTX
			px4_leave_critical_section(flags);
#اگر_ختم
			// We have to retry
			حذف[] new_pollset;
#اگر_تعریف_شدہ __PX4_NUTTX
			flags = px4_enter_critical_section();
#اگر_ختم
		}

		اگر (ret == PX4_OK) {

			/*
			 * Check to see whether we should send a poll notification
			 * immediately.
			 */
			fds->revents |= fds->events & poll_state(filep);

			/* yes? invoke the callback */
#اگر_تعریف_شدہ __PX4_NUTTX

			اگر (fds->revents != 0 && fds->cb != خالی_اشاریہ) {
				fds->cb(fds);
			}

#ورنہ

			اگر (fds->revents != 0) {
				px4_sem_post(fds->sem);
			}

#اگر_ختم

		}

		ATOMIC_LEAVE;

	} ورنہ {
		ATOMIC_ENTER;
		/*
		 * Handle a teardown request.
		 */
		ret = remove_poll_waiter(fds);
		ATOMIC_LEAVE;
	}

	واپس ret;
}

خالی
CDev::poll_notify(px4_pollevent_t events)
{
	PX4_DEBUG("CDev::poll_notify events = %0x", events);

	/* lock against poll() as well as other wakeups */
	ATOMIC_ENTER;

	برائے (غیرعلامتی i = 0; i < _max_pollwaiters; i++) {
		اگر (خالی_اشاریہ != _pollset[i]) {
			poll_notify_one(_pollset[i], events);
		}
	}

	ATOMIC_LEAVE;
}

خالی
CDev::poll_notify_one(px4_pollfd_struct_t *fds, px4_pollevent_t events)
{
	PX4_DEBUG("CDev::poll_notify_one");

	/* update the reported event set */
	fds->revents |= fds->events & events;

	PX4_DEBUG(" Events fds=%p %0x %0x %0x", fds, fds->revents, fds->events, events);

#اگر_تعریف_شدہ __PX4_NUTTX

	اگر (fds->revents != 0 && fds->cb != خالی_اشاریہ) {
		fds->cb(fds);
	}

#ورنہ

	اگر (fds->revents != 0) {
		px4_sem_post(fds->sem);
	}

#اگر_ختم
}

صحیح
CDev::store_poll_waiter(px4_pollfd_struct_t *fds)
{
	// Look for a free slot.
	PX4_DEBUG("CDev::store_poll_waiter");

	برائے (غیرعلامتی i = 0; i < _max_pollwaiters; i++) {
		اگر (خالی_اشاریہ == _pollset[i]) {

			/* save the pollfd */
			_pollset[i] = fds;

			واپس PX4_OK;
		}
	}

	واپس -ENFILE;
}

صحیح
CDev::remove_poll_waiter(px4_pollfd_struct_t *fds)
{
	PX4_DEBUG("CDev::remove_poll_waiter");

	برائے (غیرعلامتی i = 0; i < _max_pollwaiters; i++) {
		اگر (fds == _pollset[i]) {

			_pollset[i] = خالی_اشاریہ;
			واپس PX4_OK;

		}
	}

	PX4_DEBUG("poll: bad fd state");
	واپس -EINVAL;
}

} // namespace cdev
