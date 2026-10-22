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

#अन्तर्भाव "CDev.hpp"

#अन्तर्भाव <cstring>

#अन्तर्भाव <px4_platform_common/log.h>
#अन्तर्भाव <px4_platform_common/posix.h>

नामाकाश cdev
{

CDev::CDev(ध्रुव वर्ण *devname) :
	_devname(devname)
{
	PX4_DEBUG("CDev::CDev");

	पूर्णाङ्क ret = px4_sem_init(&_lock, 0, 1);

	यदि (ret != 0) {
		PX4_DEBUG("SEM INIT FAIL: ret %d", ret);
	}
}

CDev::~CDev()
{
	PX4_DEBUG("CDev::~CDev");

	यदि (_registered) {
		unregister_driver(_devname);
	}

	यदि (_pollset) {
		delete[](_pollset);
	}

	px4_sem_destroy(&_lock);
}

पूर्णाङ्क
CDev::register_class_devname(ध्रुव वर्ण *class_devname)
{
	PX4_DEBUG("CDev::register_class_devname %s", class_devname);

	यदि (class_devname == nullptr) {
		प्रत्यागम -EINVAL;
	}

	पूर्णाङ्क class_instance = 0;
	पूर्णाङ्क ret = -ENOSPC;

	यावत् (class_instance < 4) {
		वर्ण name[32];
		snprintf(name, परिमाण(name), "%s%d", class_devname, class_instance);
		ret = register_driver(name, &fops_ref(), 0666, (शून्य *)एतत्);

		यदि (ret == OK) {
			भङ्ग;
		}

		class_instance++;
	}

	यदि (class_instance == 4) {
		प्रत्यागम ret;
	}

	प्रत्यागम class_instance;
}

पूर्णाङ्क
CDev::unregister_class_devname(ध्रुव वर्ण *class_devname, unsigned class_instance)
{
	PX4_DEBUG("CDev::unregister_class_devname");

	वर्ण name[32];
	snprintf(name, परिमाण(name), "%s%u", class_devname, class_instance);
	प्रत्यागम unregister_driver(name);
}

पूर्णाङ्क
CDev::init()
{
	PX4_DEBUG("CDev::init");

	पूर्णाङ्क ret = PX4_OK;

	// now register the driver
	यदि (_devname != nullptr) {
		ret = register_driver(_devname, &fops_ref(), 0666, (शून्य *)एतत्);

		यदि (ret == PX4_OK) {
			_registered = सत्य;
		}

	} अन्यथा {
		ret = -ENODEV;
	}

	प्रत्यागम ret;
}

/*
 * Default implementations of the character device interface
 */
पूर्णाङ्क
CDev::open(file_t *filep)
{
	PX4_DEBUG("CDev::open");
	पूर्णाङ्क ret = PX4_OK;

	lock();
	/* increment the open count */
	open_गिनती++;

	यदि (open_गिनती == 1) {

		/* first-open callback may decline the open */
		ret = open_first(filep);

		यदि (ret != PX4_OK) {
			open_गिनती--;
		}
	}

	unlock();

	प्रत्यागम ret;
}

पूर्णाङ्क
CDev::close(file_t *filep)
{
	PX4_DEBUG("CDev::close");
	पूर्णाङ्क ret = PX4_OK;

	lock();

	यदि (open_गिनती > 0) {
		/* decrement the open count */
		open_गिनती--;

		/* callback cannot decline the close */
		यदि (open_गिनती == 0) {
			ret = close_last(filep);
		}

	} अन्यथा {
		ret = -EBADF;
	}

	unlock();

	प्रत्यागम ret;
}

पूर्णाङ्क
CDev::poll(file_t *filep, px4_pollfd_struct_t *fds, तार्किक setup)
{
	PX4_DEBUG("CDev::Poll %s", setup ? "setup" : "teardown");
	पूर्णाङ्क ret = PX4_OK;

	यदि (setup) {
		/*
		 * Save the file pointer in the pollfd for the subclass'
		 * benefit.
		 */
		fds->priv = (शून्य *)filep;
		PX4_DEBUG("CDev::poll: fds->priv = %p", filep);

		/*
		 * Lock against poll_notify() and possibly other callers (protect _pollset).
		 */
		ATOMIC_ENTER;

		/*
		 * Try to store the fds for later use and handle array resizing.
		 */
		यावत् ((ret = store_poll_waiter(fds)) == -ENFILE) {

			// No free slot found. Resize the pollset. This is expensive, but it's only needed initially.

			यदि (_max_pollwaiters >= 256 / 2) { //_max_pollwaiters is uint8_t
				ret = -ENOMEM;
				भङ्ग;
			}

			ध्रुव uint8_t new_गिनती = _max_pollwaiters > 0 ? _max_pollwaiters * 2 : 1;
			px4_pollfd_struct_t **prev_pollset = _pollset;

#ifdef __PX4_NUTTX
			// malloc uses a semaphore, we need to call it enabled IRQ's
			px4_leave_critical_section(flags);
#endif
			px4_pollfd_struct_t **new_pollset = नूतन px4_pollfd_struct_t *[new_गिनती];

#ifdef __PX4_NUTTX
			flags = px4_enter_critical_section();
#endif

			यदि (prev_pollset == _pollset) {
				// no one else updated the _pollset meanwhile, so we're good to go
				यदि (!new_pollset) {
					ret = -ENOMEM;
					भङ्ग;
				}

				यदि (_max_pollwaiters > 0) {
					memset(new_pollset + _max_pollwaiters, 0, परिमाण(px4_pollfd_struct_t *) * (new_गिनती - _max_pollwaiters));
					memcpy(new_pollset, _pollset, परिमाण(px4_pollfd_struct_t *) * _max_pollwaiters);
				}

				_pollset = new_pollset;
				_pollset[_max_pollwaiters] = fds;
				_max_pollwaiters = new_गिनती;

				// free the previous _pollset (we need to unlock here which is fine because we don't access _pollset anymore)
#ifdef __PX4_NUTTX
				px4_leave_critical_section(flags);
#endif

				यदि (prev_pollset) {
					delete[](prev_pollset);
				}

#ifdef __PX4_NUTTX
				flags = px4_enter_critical_section();
#endif

				// Success
				ret = PX4_OK;
				भङ्ग;
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

		यदि (ret == PX4_OK) {

			/*
			 * Check to see whether we should send a poll notification
			 * immediately.
			 */
			fds->revents |= fds->events & poll_state(filep);

			/* yes? invoke the callback */
#ifdef __PX4_NUTTX

			यदि (fds->revents != 0 && fds->cb != nullptr) {
				fds->cb(fds);
			}

#अन्यथा

			यदि (fds->revents != 0) {
				px4_sem_post(fds->sem);
			}

#endif

		}

		ATOMIC_LEAVE;

	} अन्यथा {
		ATOMIC_ENTER;
		/*
		 * Handle a teardown request.
		 */
		ret = remove_poll_waiter(fds);
		ATOMIC_LEAVE;
	}

	प्रत्यागम ret;
}

शून्य
CDev::poll_notify(px4_pollevent_t events)
{
	PX4_DEBUG("CDev::poll_notify events = %0x", events);

	/* lock against poll() as well as other wakeups */
	ATOMIC_ENTER;

	प्रति (unsigned i = 0; i < _max_pollwaiters; i++) {
		यदि (nullptr != _pollset[i]) {
			poll_notify_one(_pollset[i], events);
		}
	}

	ATOMIC_LEAVE;
}

शून्य
CDev::poll_notify_one(px4_pollfd_struct_t *fds, px4_pollevent_t events)
{
	PX4_DEBUG("CDev::poll_notify_one");

	/* update the reported event set */
	fds->revents |= fds->events & events;

	PX4_DEBUG(" Events fds=%p %0x %0x %0x", fds, fds->revents, fds->events, events);

#ifdef __PX4_NUTTX

	यदि (fds->revents != 0 && fds->cb != nullptr) {
		fds->cb(fds);
	}

#अन्यथा

	यदि (fds->revents != 0) {
		px4_sem_post(fds->sem);
	}

#endif
}

पूर्णाङ्क
CDev::store_poll_waiter(px4_pollfd_struct_t *fds)
{
	// Look for a free slot.
	PX4_DEBUG("CDev::store_poll_waiter");

	प्रति (unsigned i = 0; i < _max_pollwaiters; i++) {
		यदि (nullptr == _pollset[i]) {

			/* save the pollfd */
			_pollset[i] = fds;

			प्रत्यागम PX4_OK;
		}
	}

	प्रत्यागम -ENFILE;
}

पूर्णाङ्क
CDev::remove_poll_waiter(px4_pollfd_struct_t *fds)
{
	PX4_DEBUG("CDev::remove_poll_waiter");

	प्रति (unsigned i = 0; i < _max_pollwaiters; i++) {
		यदि (fds == _pollset[i]) {

			_pollset[i] = nullptr;
			प्रत्यागम PX4_OK;

		}
	}

	PX4_DEBUG("poll: bad fd state");
	प्रत्यागम -EINVAL;
}

} // namespace cdev
