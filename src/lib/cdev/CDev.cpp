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

#ਅਨ੍ਤਰ੍ਭਾਵ "CDev.hpp"

#ਅਨ੍ਤਰ੍ਭਾਵ <cstring>

#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/log.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/posix.h>

ਨਾਮਾਕਾਸ਼ cdev
{

CDev::CDev(ਅਚਲ ਅੱਖਰ *devname) :
	_devname(devname)
{
	PX4_DEBUG("CDev::CDev");

	ਪੂਰਨ ret = px4_sem_init(&_lock, 0, 1);

	ਜੇ (ret != 0) {
		PX4_DEBUG("SEM INIT FAIL: ret %d", ret);
	}
}

CDev::~CDev()
{
	PX4_DEBUG("CDev::~CDev");

	ਜੇ (_registered) {
		unregister_driver(_devname);
	}

	ਜੇ (_pollset) {
		ਮਿਟਾਓ[](_pollset);
	}

	px4_sem_destroy(&_lock);
}

ਪੂਰਨ
CDev::ਰਜਿਸਟਰ_class_devname(ਅਚਲ ਅੱਖਰ *class_devname)
{
	PX4_DEBUG("CDev::register_class_devname %s", class_devname);

	ਜੇ (class_devname == ਖਾਲੀ_ਸੂਚਕ) {
		ਵਾਪਸ -EINVAL;
	}

	ਪੂਰਨ class_instance = 0;
	ਪੂਰਨ ret = -ENOSPC;

	ਜਦੋਂ (class_instance < 4) {
		ਅੱਖਰ name[32];
		snprintf(name, ਆਕਾਰ(name), "%s%d", class_devname, class_instance);
		ret = ਰਜਿਸਟਰ_driver(name, &fops_ref(), 0666, (ਖਾਲੀ *)ਇਹ);

		ਜੇ (ret == OK) {
			ਰੋਕੋ;
		}

		class_instance++;
	}

	ਜੇ (class_instance == 4) {
		ਵਾਪਸ ret;
	}

	ਵਾਪਸ class_instance;
}

ਪੂਰਨ
CDev::unregister_class_devname(ਅਚਲ ਅੱਖਰ *class_devname, ਬਿਨਾਨਿਸ਼ਾਨੀ class_instance)
{
	PX4_DEBUG("CDev::unregister_class_devname");

	ਅੱਖਰ name[32];
	snprintf(name, ਆਕਾਰ(name), "%s%u", class_devname, class_instance);
	ਵਾਪਸ unregister_driver(name);
}

ਪੂਰਨ
CDev::init()
{
	PX4_DEBUG("CDev::init");

	ਪੂਰਨ ret = PX4_OK;

	// now register the driver
	ਜੇ (_devname != ਖਾਲੀ_ਸੂਚਕ) {
		ret = ਰਜਿਸਟਰ_driver(_devname, &fops_ref(), 0666, (ਖਾਲੀ *)ਇਹ);

		ਜੇ (ret == PX4_OK) {
			_registered = ਸੱਚ;
		}

	} ਨਹੀਂਤਾਂ {
		ret = -ENODEV;
	}

	ਵਾਪਸ ret;
}

/*
 * Default implementations of the character device interface
 */
ਪੂਰਨ
CDev::open(file_t *filep)
{
	PX4_DEBUG("CDev::open");
	ਪੂਰਨ ret = PX4_OK;

	ਤਾਲਾ();
	/* increment the open count */
	open_ਗਿਣਤੀ++;

	ਜੇ (open_ਗਿਣਤੀ == 1) {

		/* first-open callback may decline the open */
		ret = open_ਪਹਿਲਾ(filep);

		ਜੇ (ret != PX4_OK) {
			open_ਗਿਣਤੀ--;
		}
	}

	unlock();

	ਵਾਪਸ ret;
}

ਪੂਰਨ
CDev::close(file_t *filep)
{
	PX4_DEBUG("CDev::close");
	ਪੂਰਨ ret = PX4_OK;

	ਤਾਲਾ();

	ਜੇ (open_ਗਿਣਤੀ > 0) {
		/* decrement the open count */
		open_ਗਿਣਤੀ--;

		/* callback cannot decline the close */
		ਜੇ (open_ਗਿਣਤੀ == 0) {
			ret = close_ਆਖਰੀ(filep);
		}

	} ਨਹੀਂਤਾਂ {
		ret = -EBADF;
	}

	unlock();

	ਵਾਪਸ ret;
}

ਪੂਰਨ
CDev::poll(file_t *filep, px4_pollfd_struct_t *fds, ਤਾਰਕਿਕ ਤਿਆਰੀ)
{
	PX4_DEBUG("CDev::Poll %s", ਤਿਆਰੀ ? "setup" : "teardown");
	ਪੂਰਨ ret = PX4_OK;

	ਜੇ (ਤਿਆਰੀ) {
		/*
		 * Save the file pointer in the pollfd for the subclass'
		 * benefit.
		 */
		fds->priv = (ਖਾਲੀ *)filep;
		PX4_DEBUG("CDev::poll: fds->priv = %p", filep);

		/*
		 * Lock against poll_notify() and possibly other callers (protect _pollset).
		 */
		ATOMIC_ENTER;

		/*
		 * Try to store the fds for later use and handle array resizing.
		 */
		ਜਦੋਂ ((ret = store_poll_waiter(fds)) == -ENFILE) {

			// No free slot found. Resize the pollset. This is expensive, but it's only needed initially.

			ਜੇ (_max_pollwaiters >= 256 / 2) { //_max_pollwaiters is uint8_t
				ret = -ENOMEM;
				ਰੋਕੋ;
			}

			ਅਚਲ uint8_t new_ਗਿਣਤੀ = _max_pollwaiters > 0 ? _max_pollwaiters * 2 : 1;
			px4_pollfd_struct_t **prev_pollset = _pollset;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __PX4_NUTTX
			// malloc uses a semaphore, we need to call it enabled IRQ's
			px4_leave_critical_section(flags);
#ਜੇ_ਅੰਤ
			px4_pollfd_struct_t **new_pollset = ਨਵਾਂ px4_pollfd_struct_t *[new_ਗਿਣਤੀ];

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __PX4_NUTTX
			flags = px4_enter_critical_section();
#ਜੇ_ਅੰਤ

			ਜੇ (prev_pollset == _pollset) {
				// no one else updated the _pollset meanwhile, so we're good to go
				ਜੇ (!new_pollset) {
					ret = -ENOMEM;
					ਰੋਕੋ;
				}

				ਜੇ (_max_pollwaiters > 0) {
					memset(new_pollset + _max_pollwaiters, 0, ਆਕਾਰ(px4_pollfd_struct_t *) * (new_ਗਿਣਤੀ - _max_pollwaiters));
					ਸਮ੍ਰਿਤੀ_ਨਕਲ(new_pollset, _pollset, ਆਕਾਰ(px4_pollfd_struct_t *) * _max_pollwaiters);
				}

				_pollset = new_pollset;
				_pollset[_max_pollwaiters] = fds;
				_max_pollwaiters = new_ਗਿਣਤੀ;

				// free the previous _pollset (we need to unlock here which is fine because we don't access _pollset anymore)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __PX4_NUTTX
				px4_leave_critical_section(flags);
#ਜੇ_ਅੰਤ

				ਜੇ (prev_pollset) {
					ਮਿਟਾਓ[](prev_pollset);
				}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __PX4_NUTTX
				flags = px4_enter_critical_section();
#ਜੇ_ਅੰਤ

				// Success
				ret = PX4_OK;
				ਰੋਕੋ;
			}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __PX4_NUTTX
			px4_leave_critical_section(flags);
#ਜੇ_ਅੰਤ
			// We have to retry
			ਮਿਟਾਓ[] new_pollset;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __PX4_NUTTX
			flags = px4_enter_critical_section();
#ਜੇ_ਅੰਤ
		}

		ਜੇ (ret == PX4_OK) {

			/*
			 * Check to see whether we should send a poll notification
			 * immediately.
			 */
			fds->revents |= fds->events & poll_state(filep);

			/* yes? invoke the callback */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __PX4_NUTTX

			ਜੇ (fds->revents != 0 && fds->cb != ਖਾਲੀ_ਸੂਚਕ) {
				fds->cb(fds);
			}

#ਨਹੀਂਤਾਂ

			ਜੇ (fds->revents != 0) {
				px4_sem_post(fds->sem);
			}

#ਜੇ_ਅੰਤ

		}

		ATOMIC_LEAVE;

	} ਨਹੀਂਤਾਂ {
		ATOMIC_ENTER;
		/*
		 * Handle a teardown request.
		 */
		ret = remove_poll_waiter(fds);
		ATOMIC_LEAVE;
	}

	ਵਾਪਸ ret;
}

ਖਾਲੀ
CDev::poll_notify(px4_pollevent_t events)
{
	PX4_DEBUG("CDev::poll_notify events = %0x", events);

	/* lock against poll() as well as other wakeups */
	ATOMIC_ENTER;

	ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < _max_pollwaiters; i++) {
		ਜੇ (ਖਾਲੀ_ਸੂਚਕ != _pollset[i]) {
			poll_notify_one(_pollset[i], events);
		}
	}

	ATOMIC_LEAVE;
}

ਖਾਲੀ
CDev::poll_notify_one(px4_pollfd_struct_t *fds, px4_pollevent_t events)
{
	PX4_DEBUG("CDev::poll_notify_one");

	/* update the reported event set */
	fds->revents |= fds->events & events;

	PX4_DEBUG(" Events fds=%p %0x %0x %0x", fds, fds->revents, fds->events, events);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __PX4_NUTTX

	ਜੇ (fds->revents != 0 && fds->cb != ਖਾਲੀ_ਸੂਚਕ) {
		fds->cb(fds);
	}

#ਨਹੀਂਤਾਂ

	ਜੇ (fds->revents != 0) {
		px4_sem_post(fds->sem);
	}

#ਜੇ_ਅੰਤ
}

ਪੂਰਨ
CDev::store_poll_waiter(px4_pollfd_struct_t *fds)
{
	// Look for a free slot.
	PX4_DEBUG("CDev::store_poll_waiter");

	ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < _max_pollwaiters; i++) {
		ਜੇ (ਖਾਲੀ_ਸੂਚਕ == _pollset[i]) {

			/* save the pollfd */
			_pollset[i] = fds;

			ਵਾਪਸ PX4_OK;
		}
	}

	ਵਾਪਸ -ENFILE;
}

ਪੂਰਨ
CDev::remove_poll_waiter(px4_pollfd_struct_t *fds)
{
	PX4_DEBUG("CDev::remove_poll_waiter");

	ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ i = 0; i < _max_pollwaiters; i++) {
		ਜੇ (fds == _pollset[i]) {

			_pollset[i] = ਖਾਲੀ_ਸੂਚਕ;
			ਵਾਪਸ PX4_OK;

		}
	}

	PX4_DEBUG("poll: bad fd state");
	ਵਾਪਸ -EINVAL;
}

} // namespace cdev
