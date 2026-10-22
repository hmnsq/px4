/****************************************************************************
 *
 * Copyright (c) 2015 Mark Charlebois. All rights reserved.
 * Copyright (c) 2018 PX4 Development Team. All rights reserved.
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

#అన్తర్భావ "cdev_platform.hpp"

#అన్తర్భావ "../CDev.hpp"

#అన్తర్భావ <px4_platform_common/log.h>
#అన్తర్భావ <px4_platform_common/posix.h>
#అన్తర్భావ <px4_platform_common/time.h>

#అన్తర్భావ <stdlib.h>

స్థిర స్థిరరాశి cdev::px4_file_operations_t g_fops = {};
స్థిరరాశి cdev::px4_file_operations_t &cdev::CDev::fops_ref() { తిరిగి g_fops; }

pthread_mutex_t devmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t filemutex = PTHREAD_MUTEX_INITIALIZER;

నిర్మాణం px4_dev_t {
	అక్షరం *name{శూన్యసూచిక};
	cdev::CDev *cdev{శూన్యసూచిక};

	px4_dev_t(స్థిరరాశి అక్షరం *n, cdev::CDev *c) : cdev(c)
	{
		name = strdup(n);
	}

	~px4_dev_t()
	{
		విడుదల(name);
	}
అంతరంగ:
	px4_dev_t() = అప్రమేయ;
};

#నిర్వచించు PX4_MAX_FD 512
స్థిర px4_dev_t *devmap[PX4_MAX_FD] {};
స్థిర cdev::file_t filemap[PX4_MAX_FD] {};

తరగతి VFile : బహిరంగ cdev::CDev
{
బహిరంగ:
	VFile(స్థిరరాశి అక్షరం *fname, mode_t బహుళకం) : cdev::CDev(fname) {}
	~VFile() override = అప్రమేయ;

	ssize_t లిఖ(cdev::file_t *handlep, స్థిరరాశి అక్షరం *బఫరు_2, size_t buflen) override
	{
		// ignore what was written, but let pollers know something was written
		poll_notify(POLLIN);
		తిరిగి buflen;
	}
};

స్థిర cdev::CDev *getDev(స్థిరరాశి అక్షరం *మార్గం)
{
	pthread_mutex_lock(&devmutex);

	కొరకు (స్థిరరాశి స్వయంచాలక &dev : devmap) {
		ఒకవేళ (dev && (strcmp(dev->name, మార్గం) == 0)) {
			pthread_mutex_unlock(&devmutex);
			తిరిగి dev->cdev;
		}
	}

	pthread_mutex_unlock(&devmutex);

	తిరిగి శూన్యసూచిక;
}

స్థిర cdev::CDev *getFile(పూర్ణాంకం fd)
{
	pthread_mutex_lock(&filemutex);
	cdev::CDev *dev = శూన్యసూచిక;

	ఒకవేళ (fd < PX4_MAX_FD && fd >= 0) {
		dev = filemap[fd].cdev;
	}

	pthread_mutex_unlock(&filemutex);
	తిరిగి dev;
}

బాహ్య "C" {

	పూర్ణాంకం రిజిస్టరు_driver(స్థిరరాశి అక్షరం *name, స్థిరరాశి cdev::px4_file_operations_t *fops, cdev::mode_t బహుళకం, శూన్య *దత్తాంశం)
	{
		PX4_DEBUG("CDev::register_driver %s", name);
		పూర్ణాంకం ret = -ENOSPC;

		ఒకవేళ (name == శూన్యసూచిక || దత్తాంశం == శూన్యసూచిక) {
			తిరిగి -EINVAL;
		}

		pthread_mutex_lock(&devmutex);

		// Make sure the device does not already exist
		కొరకు (స్థిరరాశి స్వయంచాలక &dev : devmap) {
			ఒకవేళ (dev && (strcmp(dev->name, name) == 0)) {
				pthread_mutex_unlock(&devmutex);
				తిరిగి -EEXIST;
			}
		}

		కొరకు (స్వయంచాలక &dev : devmap) {
			ఒకవేళ (dev == శూన్యసూచిక) {
				dev = కొత్త px4_dev_t(name, (cdev::CDev *)దత్తాంశం);
				PX4_DEBUG("Registered DEV %s", name);
				ret = PX4_OK;
				ఆపు;
			}
		}

		pthread_mutex_unlock(&devmutex);

		ఒకవేళ (ret != PX4_OK) {
			PX4_ERR("No free devmap entries - increase devmap size");
		}

		తిరిగి ret;
	}

	పూర్ణాంకం unregister_driver(స్థిరరాశి అక్షరం *name)
	{
		PX4_DEBUG("CDev::unregister_driver %s", name);
		పూర్ణాంకం ret = -EINVAL;

		ఒకవేళ (name == శూన్యసూచిక) {
			తిరిగి -EINVAL;
		}

		pthread_mutex_lock(&devmutex);

		కొరకు (స్వయంచాలక &dev : devmap) {
			ఒకవేళ (dev && (strcmp(name, dev->name) == 0)) {
				తొలగించు dev;
				dev = శూన్యసూచిక;
				PX4_DEBUG("Unregistered DEV %s", name);
				ret = PX4_OK;
				ఆపు;
			}
		}

		pthread_mutex_unlock(&devmutex);

		తిరిగి ret;
	}

	పూర్ణాంకం px4_open(స్థిరరాశి అక్షరం *మార్గం, పూర్ణాంకం flags, ...)
	{
		PX4_DEBUG("px4_open");
		cdev::CDev *dev = getDev(మార్గం);
		పూర్ణాంకం ret = 0;
		పూర్ణాంకం i;
		mode_t బహుళకం;

		ఒకవేళ (!dev && (flags & PX4_F_WRONLY) != 0 &&
		    అక్షరశ్రేణి_పోలిక(మార్గం, "/obj/", 5) != 0 &&
		    అక్షరశ్రేణి_పోలిక(మార్గం, "/dev/", 5) != 0) {
			va_list p;
			va_ప్రారంభం(p, flags);
			బహుళకం = va_arg(p, పూర్ణాంకం);
			va_చివర(p);

			// Create the file
			PX4_DEBUG("Creating virtual file %s", మార్గం);
			dev = కొత్త VFile(మార్గం, బహుళకం);
			రిజిస్టరు_driver(మార్గం, శూన్యసూచిక, 0666, (శూన్య *)dev);
		}

		ఒకవేళ (dev) {
			pthread_mutex_lock(&filemutex);

			కొరకు (i = 0; i < PX4_MAX_FD; ++i) {
				ఒకవేళ (filemap[i].cdev == శూన్యసూచిక) {
					filemap[i] = cdev::file_t(flags, dev);
					ఆపు;
				}
			}

			pthread_mutex_unlock(&filemutex);

			ఒకవేళ (i < PX4_MAX_FD) {
				ret = dev->open(&filemap[i]);

			} లేకపోతే {

				స్థిరరాశి గుర్తులేని NAMELEN = 32;
				అక్షరం thread_name[NAMELEN] {};

				PX4_WARN("%s: exceeded maximum number of file descriptors, accessing %s", thread_name, మార్గం);
#నిర్వచితం_కాకపోతే __PX4_QURT
				పూర్ణాంకం nret = pthread_getname_np(pthread_self(), thread_name, NAMELEN);

				ఒకవేళ (nret || thread_name[0] == 0) {
					PX4_WARN("failed getting thread name");
				}

#అయితే_ముగింపు

				ret = -ENOENT;
			}

		} లేకపోతే {
			ret = -EINVAL;
		}

		ఒకవేళ (ret < 0) {
			errno = -ret;
			తిరిగి -1;
		}

		PX4_DEBUG("px4_open fd = %d", i);
		తిరిగి i;
	}

	పూర్ణాంకం px4_close(పూర్ణాంకం fd)
	{
		పూర్ణాంకం ret;

		cdev::CDev *dev = getFile(fd);

		ఒకవేళ (dev) {
			pthread_mutex_lock(&filemutex);
			ret = dev->close(&filemap[fd]);

			filemap[fd].cdev = శూన్యసూచిక;

			pthread_mutex_unlock(&filemutex);
			PX4_DEBUG("px4_close fd = %d", fd);

		} లేకపోతే {
			ret = -EINVAL;
		}

		ఒకవేళ (ret < 0) {
			ret = PX4_ERROR;
		}

		తిరిగి ret;
	}

	ssize_t px4_read(పూర్ణాంకం fd, శూన్య *బఫరు_2, size_t buflen)
	{
		పూర్ణాంకం ret;

		cdev::CDev *dev = getFile(fd);

		ఒకవేళ (dev) {
			PX4_DEBUG("px4_read fd = %d", fd);
			ret = dev->పఠ(&filemap[fd], (అక్షరం *)బఫరు_2, buflen);

		} లేకపోతే {
			ret = -EINVAL;
		}

		ఒకవేళ (ret < 0) {
			ret = PX4_ERROR;
		}

		తిరిగి ret;
	}

	ssize_t px4_write(పూర్ణాంకం fd, స్థిరరాశి శూన్య *బఫరు_2, size_t buflen)
	{
		పూర్ణాంకం ret;

		cdev::CDev *dev = getFile(fd);

		ఒకవేళ (dev) {
			PX4_DEBUG("px4_write fd = %d", fd);
			ret = dev->లిఖ(&filemap[fd], (స్థిరరాశి అక్షరం *)బఫరు_2, buflen);

		} లేకపోతే {
			ret = -EINVAL;
		}

		ఒకవేళ (ret < 0) {
			ret = PX4_ERROR;
		}

		తిరిగి ret;
	}

	పూర్ణాంకం px4_ioctl(పూర్ణాంకం fd, పూర్ణాంకం cmd, గుర్తులేని దీర్ఘ arg)
	{
		PX4_DEBUG("px4_ioctl fd = %d", fd);
		పూర్ణాంకం ret = 0;

		cdev::CDev *dev = getFile(fd);

		ఒకవేళ (dev) {
			ret = dev->ioctl(&filemap[fd], cmd, arg);

		} లేకపోతే {
			ret = -EINVAL;
		}

		తిరిగి ret;
	}

	పూర్ణాంకం px4_poll(px4_pollfd_struct_t *fds, గుర్తులేని పూర్ణాంకం nfds, పూర్ణాంకం timeout)
	{
		ఒకవేళ (nfds == 0) {
			PX4_WARN("px4_poll with no fds");
			తిరిగి -1;
		}

		px4_sem_t sem;
		పూర్ణాంకం లెక్క = 0;
		పూర్ణాంకం ret = -1;

		స్థిరరాశి గుర్తులేని NAMELEN = 32;
		అక్షరం thread_name[NAMELEN] {};

#నిర్వచితం_కాకపోతే __PX4_QURT
		పూర్ణాంకం nret = pthread_getname_np(pthread_self(), thread_name, NAMELEN);

		ఒకవేళ (nret || thread_name[0] == 0) {
			PX4_WARN("failed getting thread name");
		}

#అయితే_ముగింపు

		PX4_DEBUG("Called px4_poll timeout = %d", timeout);

		px4_sem_init(&sem, 0, 0);

		// sem use case is a signal
		px4_sem_setprotocol(&sem, SEM_PRIO_NONE);

		// Go through all fds and check them for a pollable state
		తార్కిక fd_pollable = అబద్ధం;

		కొరకు (గుర్తులేని పూర్ణాంకం i = 0; i < nfds; ++i) {
			fds[i].sem     = &sem;
			fds[i].revents = 0;
			fds[i].priv    = శూన్యసూచిక;

			cdev::CDev *dev = getFile(fds[i].fd);

			// If fd is valid
			ఒకవేళ (dev) {
				PX4_DEBUG("%s: px4_poll: CDev->poll(setup) %d", thread_name, fds[i].fd);
				ret = dev->poll(&filemap[fds[i].fd], &fds[i], నిజం);

				ఒకవేళ (ret < 0) {
					PX4_WARN("%s: px4_poll() error: %s", thread_name, strerror(errno));
					ఆపు;
				}

				ఒకవేళ (ret >= 0) {
					fd_pollable = నిజం;
				}
			}
		}

		// If any FD can be polled, lock the semaphore and
		// check for new data
		ఒకవేళ (fd_pollable) {
			ఒకవేళ (timeout > 0) {
				// Get the current time
				నిర్మాణం timespec ts;
#ఒకవేళ defined(__PX4_DARWIN) && !defined(ENABLE_LOCKSTEP_SCHEDULER)
				// macOS has no pthread_condattr_setclock(), so px4_sem_t's condition
				// variable waits on CLOCK_REALTIME (px4_sem_init()). Use that clock here
				// too. CLOCK_MONOTONIC counts from boot, so the deadline would land
				// decades in the past and px4_sem_timedwait() would return immediately.
				px4_clock_gettime(CLOCK_REALTIME, &ts);
#లేకపోతే
				// Note, we can't actually use CLOCK_MONOTONIC on macOS
				// but that's hidden and implemented in px4_clock_gettime.
				px4_clock_gettime(CLOCK_MONOTONIC, &ts);
#అయితే_ముగింపు

				// Calculate an absolute time in the future
				స్థిరరాశి గుర్తులేని billion = (1000 * 1000 * 1000);
				uint64_t nsecs = ts.tv_nsec + ((uint64_t)timeout * 1000 * 1000);
				ts.tv_sec += nsecs / billion;
				nsecs -= (nsecs / billion) * billion;
				ts.tv_nsec = nsecs;

				ret = px4_sem_timedwait(&sem, &ts);

				ఒకవేళ (ret && errno != ETIMEDOUT) {
					PX4_WARN("%s: px4_poll() sem error: %s", thread_name, strerror(errno));
				}

			} లేకపోతే ఒకవేళ (timeout < 0) {
				px4_sem_wait(&sem);
			}

			// We have waited now (or not, depending on timeout),
			// go through all fds and count how many have data
			కొరకు (గుర్తులేని పూర్ణాంకం i = 0; i < nfds; ++i) {

				cdev::CDev *dev = getFile(fds[i].fd);

				// If fd is valid
				ఒకవేళ (dev) {
					PX4_DEBUG("%s: px4_poll: CDev->poll(teardown) %d", thread_name, fds[i].fd);
					ret = dev->poll(&filemap[fds[i].fd], &fds[i], అబద్ధం);

					ఒకవేళ (ret < 0) {
						PX4_WARN("%s: px4_poll() 2nd poll fail", thread_name);
						ఆపు;
					}

					ఒకవేళ (fds[i].revents) {
						లెక్క += 1;
					}
				}
			}
		}

		px4_sem_destroy(&sem);

		// Return the positive count if present,
		// return the negative error number if failed
		తిరిగి (లెక్క) ? లెక్క : ret;
	}

	పూర్ణాంకం px4_access(స్థిరరాశి అక్షరం *pathname, పూర్ణాంకం బహుళకం)
	{
		ఒకవేళ (బహుళకం != F_OK) {
			errno = EINVAL;
			తిరిగి -1;
		}

		cdev::CDev *dev = getDev(pathname);
		తిరిగి (dev != శూన్యసూచిక) ? 0 : -1;
	}

	శూన్య px4_show_files()
	{
		PX4_INFO("Files:");

		pthread_mutex_lock(&devmutex);

		కొరకు (స్థిరరాశి స్వయంచాలక &dev : devmap) {
			ఒకవేళ (dev) {
				PX4_INFO_RAW("   %s\n", dev->name);
			}
		}

		pthread_mutex_unlock(&devmutex);
	}

} // extern "C"
