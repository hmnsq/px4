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

#అన్తర్భావ "CDev.hpp"

#అన్తర్భావ <cstring>

#అన్తర్భావ <px4_platform_common/log.h>
#అన్తర్భావ <px4_platform_common/posix.h>

నామాకాశ cdev
{

CDev::CDev(స్థిరరాశి అక్షరం *devname) :
	_devname(devname)
{
	PX4_DEBUG("CDev::CDev");

	పూర్ణాంకం ret = px4_sem_init(&_lock, 0, 1);

	ఒకవేళ (ret != 0) {
		PX4_DEBUG("SEM INIT FAIL: ret %d", ret);
	}
}

CDev::~CDev()
{
	PX4_DEBUG("CDev::~CDev");

	ఒకవేళ (_registered) {
		unregister_driver(_devname);
	}

	ఒకవేళ (_pollset) {
		తొలగించు[](_pollset);
	}

	px4_sem_destroy(&_lock);
}

పూర్ణాంకం
CDev::రిజిస్టరు_class_devname(స్థిరరాశి అక్షరం *class_devname)
{
	PX4_DEBUG("CDev::register_class_devname %s", class_devname);

	ఒకవేళ (class_devname == శూన్యసూచిక) {
		తిరిగి -EINVAL;
	}

	పూర్ణాంకం class_instance = 0;
	పూర్ణాంకం ret = -ENOSPC;

	అయితే (class_instance < 4) {
		అక్షరం name[32];
		snprintf(name, పరిమాణం(name), "%s%d", class_devname, class_instance);
		ret = రిజిస్టరు_driver(name, &fops_ref(), 0666, (శూన్య *)ఇది);

		ఒకవేళ (ret == OK) {
			ఆపు;
		}

		class_instance++;
	}

	ఒకవేళ (class_instance == 4) {
		తిరిగి ret;
	}

	తిరిగి class_instance;
}

పూర్ణాంకం
CDev::unregister_class_devname(స్థిరరాశి అక్షరం *class_devname, గుర్తులేని class_instance)
{
	PX4_DEBUG("CDev::unregister_class_devname");

	అక్షరం name[32];
	snprintf(name, పరిమాణం(name), "%s%u", class_devname, class_instance);
	తిరిగి unregister_driver(name);
}

పూర్ణాంకం
CDev::init()
{
	PX4_DEBUG("CDev::init");

	పూర్ణాంకం ret = PX4_OK;

	// now register the driver
	ఒకవేళ (_devname != శూన్యసూచిక) {
		ret = రిజిస్టరు_driver(_devname, &fops_ref(), 0666, (శూన్య *)ఇది);

		ఒకవేళ (ret == PX4_OK) {
			_registered = నిజం;
		}

	} లేకపోతే {
		ret = -ENODEV;
	}

	తిరిగి ret;
}

/*
 * Default implementations of the character device interface
 */
పూర్ణాంకం
CDev::open(file_t *filep)
{
	PX4_DEBUG("CDev::open");
	పూర్ణాంకం ret = PX4_OK;

	తాళం();
	/* increment the open count */
	open_లెక్క++;

	ఒకవేళ (open_లెక్క == 1) {

		/* first-open callback may decline the open */
		ret = open_మొదటి(filep);

		ఒకవేళ (ret != PX4_OK) {
			open_లెక్క--;
		}
	}

	unlock();

	తిరిగి ret;
}

పూర్ణాంకం
CDev::close(file_t *filep)
{
	PX4_DEBUG("CDev::close");
	పూర్ణాంకం ret = PX4_OK;

	తాళం();

	ఒకవేళ (open_లెక్క > 0) {
		/* decrement the open count */
		open_లెక్క--;

		/* callback cannot decline the close */
		ఒకవేళ (open_లెక్క == 0) {
			ret = close_చివరి(filep);
		}

	} లేకపోతే {
		ret = -EBADF;
	}

	unlock();

	తిరిగి ret;
}

పూర్ణాంకం
CDev::poll(file_t *filep, px4_pollfd_struct_t *fds, తార్కిక అమరిక)
{
	PX4_DEBUG("CDev::Poll %s", అమరిక ? "setup" : "teardown");
	పూర్ణాంకం ret = PX4_OK;

	ఒకవేళ (అమరిక) {
		/*
		 * Save the file pointer in the pollfd for the subclass'
		 * benefit.
		 */
		fds->priv = (శూన్య *)filep;
		PX4_DEBUG("CDev::poll: fds->priv = %p", filep);

		/*
		 * Lock against poll_notify() and possibly other callers (protect _pollset).
		 */
		ATOMIC_ENTER;

		/*
		 * Try to store the fds for later use and handle array resizing.
		 */
		అయితే ((ret = store_poll_waiter(fds)) == -ENFILE) {

			// No free slot found. Resize the pollset. This is expensive, but it's only needed initially.

			ఒకవేళ (_max_pollwaiters >= 256 / 2) { //_max_pollwaiters is uint8_t
				ret = -ENOMEM;
				ఆపు;
			}

			స్థిరరాశి uint8_t new_లెక్క = _max_pollwaiters > 0 ? _max_pollwaiters * 2 : 1;
			px4_pollfd_struct_t **prev_pollset = _pollset;

#నిర్వచితమైతే __PX4_NUTTX
			// malloc uses a semaphore, we need to call it enabled IRQ's
			px4_leave_critical_section(flags);
#అయితే_ముగింపు
			px4_pollfd_struct_t **new_pollset = కొత్త px4_pollfd_struct_t *[new_లెక్క];

#నిర్వచితమైతే __PX4_NUTTX
			flags = px4_enter_critical_section();
#అయితే_ముగింపు

			ఒకవేళ (prev_pollset == _pollset) {
				// no one else updated the _pollset meanwhile, so we're good to go
				ఒకవేళ (!new_pollset) {
					ret = -ENOMEM;
					ఆపు;
				}

				ఒకవేళ (_max_pollwaiters > 0) {
					memset(new_pollset + _max_pollwaiters, 0, పరిమాణం(px4_pollfd_struct_t *) * (new_లెక్క - _max_pollwaiters));
					జ్ఞాపకం_నకలు(new_pollset, _pollset, పరిమాణం(px4_pollfd_struct_t *) * _max_pollwaiters);
				}

				_pollset = new_pollset;
				_pollset[_max_pollwaiters] = fds;
				_max_pollwaiters = new_లెక్క;

				// free the previous _pollset (we need to unlock here which is fine because we don't access _pollset anymore)
#నిర్వచితమైతే __PX4_NUTTX
				px4_leave_critical_section(flags);
#అయితే_ముగింపు

				ఒకవేళ (prev_pollset) {
					తొలగించు[](prev_pollset);
				}

#నిర్వచితమైతే __PX4_NUTTX
				flags = px4_enter_critical_section();
#అయితే_ముగింపు

				// Success
				ret = PX4_OK;
				ఆపు;
			}

#నిర్వచితమైతే __PX4_NUTTX
			px4_leave_critical_section(flags);
#అయితే_ముగింపు
			// We have to retry
			తొలగించు[] new_pollset;
#నిర్వచితమైతే __PX4_NUTTX
			flags = px4_enter_critical_section();
#అయితే_ముగింపు
		}

		ఒకవేళ (ret == PX4_OK) {

			/*
			 * Check to see whether we should send a poll notification
			 * immediately.
			 */
			fds->revents |= fds->events & poll_state(filep);

			/* yes? invoke the callback */
#నిర్వచితమైతే __PX4_NUTTX

			ఒకవేళ (fds->revents != 0 && fds->cb != శూన్యసూచిక) {
				fds->cb(fds);
			}

#లేకపోతే

			ఒకవేళ (fds->revents != 0) {
				px4_sem_post(fds->sem);
			}

#అయితే_ముగింపు

		}

		ATOMIC_LEAVE;

	} లేకపోతే {
		ATOMIC_ENTER;
		/*
		 * Handle a teardown request.
		 */
		ret = remove_poll_waiter(fds);
		ATOMIC_LEAVE;
	}

	తిరిగి ret;
}

శూన్య
CDev::poll_notify(px4_pollevent_t events)
{
	PX4_DEBUG("CDev::poll_notify events = %0x", events);

	/* lock against poll() as well as other wakeups */
	ATOMIC_ENTER;

	కొరకు (గుర్తులేని i = 0; i < _max_pollwaiters; i++) {
		ఒకవేళ (శూన్యసూచిక != _pollset[i]) {
			poll_notify_one(_pollset[i], events);
		}
	}

	ATOMIC_LEAVE;
}

శూన్య
CDev::poll_notify_one(px4_pollfd_struct_t *fds, px4_pollevent_t events)
{
	PX4_DEBUG("CDev::poll_notify_one");

	/* update the reported event set */
	fds->revents |= fds->events & events;

	PX4_DEBUG(" Events fds=%p %0x %0x %0x", fds, fds->revents, fds->events, events);

#నిర్వచితమైతే __PX4_NUTTX

	ఒకవేళ (fds->revents != 0 && fds->cb != శూన్యసూచిక) {
		fds->cb(fds);
	}

#లేకపోతే

	ఒకవేళ (fds->revents != 0) {
		px4_sem_post(fds->sem);
	}

#అయితే_ముగింపు
}

పూర్ణాంకం
CDev::store_poll_waiter(px4_pollfd_struct_t *fds)
{
	// Look for a free slot.
	PX4_DEBUG("CDev::store_poll_waiter");

	కొరకు (గుర్తులేని i = 0; i < _max_pollwaiters; i++) {
		ఒకవేళ (శూన్యసూచిక == _pollset[i]) {

			/* save the pollfd */
			_pollset[i] = fds;

			తిరిగి PX4_OK;
		}
	}

	తిరిగి -ENFILE;
}

పూర్ణాంకం
CDev::remove_poll_waiter(px4_pollfd_struct_t *fds)
{
	PX4_DEBUG("CDev::remove_poll_waiter");

	కొరకు (గుర్తులేని i = 0; i < _max_pollwaiters; i++) {
		ఒకవేళ (fds == _pollset[i]) {

			_pollset[i] = శూన్యసూచిక;
			తిరిగి PX4_OK;

		}
	}

	PX4_DEBUG("poll: bad fd state");
	తిరిగి -EINVAL;
}

} // namespace cdev
