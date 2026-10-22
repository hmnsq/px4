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

#અન્તર્ભાવ "CDev.hpp"

#અન્તર્ભાવ <cstring>

#અન્તર્ભાવ <px4_platform_common/log.h>
#અન્તર્ભાવ <px4_platform_common/posix.h>

નામાકાશ cdev
{

CDev::CDev(અચલ અક્ષર *devname) :
	_devname(devname)
{
	PX4_DEBUG("CDev::CDev");

	પૂર્ણાંક ret = px4_sem_init(&_lock, 0, 1);

	જો (ret != 0) {
		PX4_DEBUG("SEM INIT FAIL: ret %d", ret);
	}
}

CDev::~CDev()
{
	PX4_DEBUG("CDev::~CDev");

	જો (_registered) {
		unregister_driver(_devname);
	}

	જો (_pollset) {
		delete[](_pollset);
	}

	px4_sem_destroy(&_lock);
}

પૂર્ણાંક
CDev::register_class_devname(અચલ અક્ષર *class_devname)
{
	PX4_DEBUG("CDev::register_class_devname %s", class_devname);

	જો (class_devname == nullptr) {
		પાછા -EINVAL;
	}

	પૂર્ણાંક class_instance = 0;
	પૂર્ણાંક ret = -ENOSPC;

	જ્યારે (class_instance < 4) {
		અક્ષર name[32];
		snprintf(name, કદ(name), "%s%d", class_devname, class_instance);
		ret = register_driver(name, &fops_ref(), 0666, (શૂન્ય *)આ);

		જો (ret == OK) {
			તોડો;
		}

		class_instance++;
	}

	જો (class_instance == 4) {
		પાછા ret;
	}

	પાછા class_instance;
}

પૂર્ણાંક
CDev::unregister_class_devname(અચલ અક્ષર *class_devname, unsigned class_instance)
{
	PX4_DEBUG("CDev::unregister_class_devname");

	અક્ષર name[32];
	snprintf(name, કદ(name), "%s%u", class_devname, class_instance);
	પાછા unregister_driver(name);
}

પૂર્ણાંક
CDev::init()
{
	PX4_DEBUG("CDev::init");

	પૂર્ણાંક ret = PX4_OK;

	// now register the driver
	જો (_devname != nullptr) {
		ret = register_driver(_devname, &fops_ref(), 0666, (શૂન્ય *)આ);

		જો (ret == PX4_OK) {
			_registered = સાચું;
		}

	} નહીંતર {
		ret = -ENODEV;
	}

	પાછા ret;
}

/*
 * Default implementations of the character device interface
 */
પૂર્ણાંક
CDev::open(file_t *filep)
{
	PX4_DEBUG("CDev::open");
	પૂર્ણાંક ret = PX4_OK;

	lock();
	/* increment the open count */
	open_ગણતરી++;

	જો (open_ગણતરી == 1) {

		/* first-open callback may decline the open */
		ret = open_first(filep);

		જો (ret != PX4_OK) {
			open_ગણતરી--;
		}
	}

	unlock();

	પાછા ret;
}

પૂર્ણાંક
CDev::close(file_t *filep)
{
	PX4_DEBUG("CDev::close");
	પૂર્ણાંક ret = PX4_OK;

	lock();

	જો (open_ગણતરી > 0) {
		/* decrement the open count */
		open_ગણતરી--;

		/* callback cannot decline the close */
		જો (open_ગણતરી == 0) {
			ret = close_last(filep);
		}

	} નહીંતર {
		ret = -EBADF;
	}

	unlock();

	પાછા ret;
}

પૂર્ણાંક
CDev::poll(file_t *filep, px4_pollfd_struct_t *fds, તાર્કિક setup)
{
	PX4_DEBUG("CDev::Poll %s", setup ? "setup" : "teardown");
	પૂર્ણાંક ret = PX4_OK;

	જો (setup) {
		/*
		 * Save the file pointer in the pollfd for the subclass'
		 * benefit.
		 */
		fds->priv = (શૂન્ય *)filep;
		PX4_DEBUG("CDev::poll: fds->priv = %p", filep);

		/*
		 * Lock against poll_notify() and possibly other callers (protect _pollset).
		 */
		ATOMIC_ENTER;

		/*
		 * Try to store the fds for later use and handle array resizing.
		 */
		જ્યારે ((ret = store_poll_waiter(fds)) == -ENFILE) {

			// No free slot found. Resize the pollset. This is expensive, but it's only needed initially.

			જો (_max_pollwaiters >= 256 / 2) { //_max_pollwaiters is uint8_t
				ret = -ENOMEM;
				તોડો;
			}

			અચલ uint8_t new_ગણતરી = _max_pollwaiters > 0 ? _max_pollwaiters * 2 : 1;
			px4_pollfd_struct_t **prev_pollset = _pollset;

#ifdef __PX4_NUTTX
			// malloc uses a semaphore, we need to call it enabled IRQ's
			px4_leave_critical_section(flags);
#endif
			px4_pollfd_struct_t **new_pollset = નવું px4_pollfd_struct_t *[new_ગણતરી];

#ifdef __PX4_NUTTX
			flags = px4_enter_critical_section();
#endif

			જો (prev_pollset == _pollset) {
				// no one else updated the _pollset meanwhile, so we're good to go
				જો (!new_pollset) {
					ret = -ENOMEM;
					તોડો;
				}

				જો (_max_pollwaiters > 0) {
					memset(new_pollset + _max_pollwaiters, 0, કદ(px4_pollfd_struct_t *) * (new_ગણતરી - _max_pollwaiters));
					memcpy(new_pollset, _pollset, કદ(px4_pollfd_struct_t *) * _max_pollwaiters);
				}

				_pollset = new_pollset;
				_pollset[_max_pollwaiters] = fds;
				_max_pollwaiters = new_ગણતરી;

				// free the previous _pollset (we need to unlock here which is fine because we don't access _pollset anymore)
#ifdef __PX4_NUTTX
				px4_leave_critical_section(flags);
#endif

				જો (prev_pollset) {
					delete[](prev_pollset);
				}

#ifdef __PX4_NUTTX
				flags = px4_enter_critical_section();
#endif

				// Success
				ret = PX4_OK;
				તોડો;
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

		જો (ret == PX4_OK) {

			/*
			 * Check to see whether we should send a poll notification
			 * immediately.
			 */
			fds->revents |= fds->events & poll_state(filep);

			/* yes? invoke the callback */
#ifdef __PX4_NUTTX

			જો (fds->revents != 0 && fds->cb != nullptr) {
				fds->cb(fds);
			}

#નહીંતર

			જો (fds->revents != 0) {
				px4_sem_post(fds->sem);
			}

#endif

		}

		ATOMIC_LEAVE;

	} નહીંતર {
		ATOMIC_ENTER;
		/*
		 * Handle a teardown request.
		 */
		ret = remove_poll_waiter(fds);
		ATOMIC_LEAVE;
	}

	પાછા ret;
}

શૂન્ય
CDev::poll_notify(px4_pollevent_t events)
{
	PX4_DEBUG("CDev::poll_notify events = %0x", events);

	/* lock against poll() as well as other wakeups */
	ATOMIC_ENTER;

	માટે (unsigned i = 0; i < _max_pollwaiters; i++) {
		જો (nullptr != _pollset[i]) {
			poll_notify_one(_pollset[i], events);
		}
	}

	ATOMIC_LEAVE;
}

શૂન્ય
CDev::poll_notify_one(px4_pollfd_struct_t *fds, px4_pollevent_t events)
{
	PX4_DEBUG("CDev::poll_notify_one");

	/* update the reported event set */
	fds->revents |= fds->events & events;

	PX4_DEBUG(" Events fds=%p %0x %0x %0x", fds, fds->revents, fds->events, events);

#ifdef __PX4_NUTTX

	જો (fds->revents != 0 && fds->cb != nullptr) {
		fds->cb(fds);
	}

#નહીંતર

	જો (fds->revents != 0) {
		px4_sem_post(fds->sem);
	}

#endif
}

પૂર્ણાંક
CDev::store_poll_waiter(px4_pollfd_struct_t *fds)
{
	// Look for a free slot.
	PX4_DEBUG("CDev::store_poll_waiter");

	માટે (unsigned i = 0; i < _max_pollwaiters; i++) {
		જો (nullptr == _pollset[i]) {

			/* save the pollfd */
			_pollset[i] = fds;

			પાછા PX4_OK;
		}
	}

	પાછા -ENFILE;
}

પૂર્ણાંક
CDev::remove_poll_waiter(px4_pollfd_struct_t *fds)
{
	PX4_DEBUG("CDev::remove_poll_waiter");

	માટે (unsigned i = 0; i < _max_pollwaiters; i++) {
		જો (fds == _pollset[i]) {

			_pollset[i] = nullptr;
			પાછા PX4_OK;

		}
	}

	PX4_DEBUG("poll: bad fd state");
	પાછા -EINVAL;
}

} // namespace cdev
