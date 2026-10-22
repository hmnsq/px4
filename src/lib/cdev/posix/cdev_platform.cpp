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

#ଅନ୍ତର୍ଭାଵ "cdev_platform.hpp"

#ଅନ୍ତର୍ଭାଵ "../CDev.hpp"

#ଅନ୍ତର୍ଭାଵ <px4_platform_common/log.h>
#ଅନ୍ତର୍ଭାଵ <px4_platform_common/posix.h>
#ଅନ୍ତର୍ଭାଵ <px4_platform_common/time.h>

#ଅନ୍ତର୍ଭାଵ <stdlib.h>

ସ୍ଥିର ଅଚଳ cdev::px4_file_operations_t g_fops = {};
ଅଚଳ cdev::px4_file_operations_t &cdev::CDev::fops_ref() { ଫେରନ୍ତୁ g_fops; }

pthread_mutex_t devmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t filemutex = PTHREAD_MUTEX_INITIALIZER;

ସଂରଚନା px4_dev_t {
	ଅକ୍ଷର *name{nullptr};
	cdev::CDev *cdev{nullptr};

	px4_dev_t(ଅଚଳ ଅକ୍ଷର *n, cdev::CDev *c) : cdev(c)
	{
		name = strdup(n);
	}

	~px4_dev_t()
	{
		ମୁକ୍ତ(name);
	}
ବ୍ୟକ୍ତିଗତ:
	px4_dev_t() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;
};

#define PX4_MAX_FD 512
ସ୍ଥିର px4_dev_t *devmap[PX4_MAX_FD] {};
ସ୍ଥିର cdev::file_t filemap[PX4_MAX_FD] {};

ଶ୍ରେଣୀ VFile : ସର୍ବସାଧାରଣ cdev::CDev
{
ସର୍ବସାଧାରଣ:
	VFile(ଅଚଳ ଅକ୍ଷର *fname, mode_t mode) : cdev::CDev(fname) {}
	~VFile() override = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

	ssize_t ଲିଖ(cdev::file_t *handlep, ଅଚଳ ଅକ୍ଷର *buffer, size_t buflen) override
	{
		// ignore what was written, but let pollers know something was written
		poll_notify(POLLIN);
		ଫେରନ୍ତୁ buflen;
	}
};

ସ୍ଥିର cdev::CDev *getDev(ଅଚଳ ଅକ୍ଷର *path)
{
	pthread_mutex_lock(&devmutex);

	ପାଇଁ (ଅଚଳ auto &dev : devmap) {
		ଯଦି (dev && (strcmp(dev->name, path) == 0)) {
			pthread_mutex_unlock(&devmutex);
			ଫେରନ୍ତୁ dev->cdev;
		}
	}

	pthread_mutex_unlock(&devmutex);

	ଫେରନ୍ତୁ nullptr;
}

ସ୍ଥିର cdev::CDev *getFile(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd)
{
	pthread_mutex_lock(&filemutex);
	cdev::CDev *dev = nullptr;

	ଯଦି (fd < PX4_MAX_FD && fd >= 0) {
		dev = filemap[fd].cdev;
	}

	pthread_mutex_unlock(&filemutex);
	ଫେରନ୍ତୁ dev;
}

ବାହ୍ୟ "C" {

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା register_driver(ଅଚଳ ଅକ୍ଷର *name, ଅଚଳ cdev::px4_file_operations_t *fops, cdev::mode_t mode, ଶୂନ୍ୟ *data)
	{
		PX4_DEBUG("CDev::register_driver %s", name);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = -ENOSPC;

		ଯଦି (name == nullptr || data == nullptr) {
			ଫେରନ୍ତୁ -EINVAL;
		}

		pthread_mutex_lock(&devmutex);

		// Make sure the device does not already exist
		ପାଇଁ (ଅଚଳ auto &dev : devmap) {
			ଯଦି (dev && (strcmp(dev->name, name) == 0)) {
				pthread_mutex_unlock(&devmutex);
				ଫେରନ୍ତୁ -EEXIST;
			}
		}

		ପାଇଁ (auto &dev : devmap) {
			ଯଦି (dev == nullptr) {
				dev = ନୂତନ px4_dev_t(name, (cdev::CDev *)data);
				PX4_DEBUG("Registered DEV %s", name);
				ret = PX4_OK;
				ଭାଙ୍ଗ;
			}
		}

		pthread_mutex_unlock(&devmutex);

		ଯଦି (ret != PX4_OK) {
			PX4_ERR("No free devmap entries - increase devmap size");
		}

		ଫେରନ୍ତୁ ret;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା unregister_driver(ଅଚଳ ଅକ୍ଷର *name)
	{
		PX4_DEBUG("CDev::unregister_driver %s", name);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = -EINVAL;

		ଯଦି (name == nullptr) {
			ଫେରନ୍ତୁ -EINVAL;
		}

		pthread_mutex_lock(&devmutex);

		ପାଇଁ (auto &dev : devmap) {
			ଯଦି (dev && (strcmp(name, dev->name) == 0)) {
				delete dev;
				dev = nullptr;
				PX4_DEBUG("Unregistered DEV %s", name);
				ret = PX4_OK;
				ଭାଙ୍ଗ;
			}
		}

		pthread_mutex_unlock(&devmutex);

		ଫେରନ୍ତୁ ret;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା px4_open(ଅଚଳ ଅକ୍ଷର *path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags, ...)
	{
		PX4_DEBUG("px4_open");
		cdev::CDev *dev = getDev(path);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = 0;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
		mode_t mode;

		ଯଦି (!dev && (flags & PX4_F_WRONLY) != 0 &&
		    strncmp(path, "/obj/", 5) != 0 &&
		    strncmp(path, "/dev/", 5) != 0) {
			va_list p;
			va_start(p, flags);
			mode = va_arg(p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
			va_end(p);

			// Create the file
			PX4_DEBUG("Creating virtual file %s", path);
			dev = ନୂତନ VFile(path, mode);
			register_driver(path, nullptr, 0666, (ଶୂନ୍ୟ *)dev);
		}

		ଯଦି (dev) {
			pthread_mutex_lock(&filemutex);

			ପାଇଁ (i = 0; i < PX4_MAX_FD; ++i) {
				ଯଦି (filemap[i].cdev == nullptr) {
					filemap[i] = cdev::file_t(flags, dev);
					ଭାଙ୍ଗ;
				}
			}

			pthread_mutex_unlock(&filemutex);

			ଯଦି (i < PX4_MAX_FD) {
				ret = dev->open(&filemap[i]);

			} ଅନ୍ୟଥା {

				ଅଚଳ unsigned NAMELEN = 32;
				ଅକ୍ଷର thread_name[NAMELEN] {};

				PX4_WARN("%s: exceeded maximum number of file descriptors, accessing %s", thread_name, path);
#ifndef __PX4_QURT
				ପୂର୍ଣ୍ଣସଂଖ୍ୟା nret = pthread_getname_np(pthread_self(), thread_name, NAMELEN);

				ଯଦି (nret || thread_name[0] == 0) {
					PX4_WARN("failed getting thread name");
				}

#endif

				ret = -ENOENT;
			}

		} ଅନ୍ୟଥା {
			ret = -EINVAL;
		}

		ଯଦି (ret < 0) {
			errno = -ret;
			ଫେରନ୍ତୁ -1;
		}

		PX4_DEBUG("px4_open fd = %d", i);
		ଫେରନ୍ତୁ i;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା px4_close(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd)
	{
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;

		cdev::CDev *dev = getFile(fd);

		ଯଦି (dev) {
			pthread_mutex_lock(&filemutex);
			ret = dev->close(&filemap[fd]);

			filemap[fd].cdev = nullptr;

			pthread_mutex_unlock(&filemutex);
			PX4_DEBUG("px4_close fd = %d", fd);

		} ଅନ୍ୟଥା {
			ret = -EINVAL;
		}

		ଯଦି (ret < 0) {
			ret = PX4_ERROR;
		}

		ଫେରନ୍ତୁ ret;
	}

	ssize_t px4_read(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଶୂନ୍ୟ *buffer, size_t buflen)
	{
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;

		cdev::CDev *dev = getFile(fd);

		ଯଦି (dev) {
			PX4_DEBUG("px4_read fd = %d", fd);
			ret = dev->ପଠ(&filemap[fd], (ଅକ୍ଷର *)buffer, buflen);

		} ଅନ୍ୟଥା {
			ret = -EINVAL;
		}

		ଯଦି (ret < 0) {
			ret = PX4_ERROR;
		}

		ଫେରନ୍ତୁ ret;
	}

	ssize_t px4_write(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଅଚଳ ଶୂନ୍ୟ *buffer, size_t buflen)
	{
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;

		cdev::CDev *dev = getFile(fd);

		ଯଦି (dev) {
			PX4_DEBUG("px4_write fd = %d", fd);
			ret = dev->ଲିଖ(&filemap[fd], (ଅଚଳ ଅକ୍ଷର *)buffer, buflen);

		} ଅନ୍ୟଥା {
			ret = -EINVAL;
		}

		ଯଦି (ret < 0) {
			ret = PX4_ERROR;
		}

		ଫେରନ୍ତୁ ret;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା px4_ioctl(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cmd, unsigned long arg)
	{
		PX4_DEBUG("px4_ioctl fd = %d", fd);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = 0;

		cdev::CDev *dev = getFile(fd);

		ଯଦି (dev) {
			ret = dev->ioctl(&filemap[fd], cmd, arg);

		} ଅନ୍ୟଥା {
			ret = -EINVAL;
		}

		ଫେରନ୍ତୁ ret;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା px4_poll(px4_pollfd_struct_t *fds, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା nfds, ପୂର୍ଣ୍ଣସଂଖ୍ୟା timeout)
	{
		ଯଦି (nfds == 0) {
			PX4_WARN("px4_poll with no fds");
			ଫେରନ୍ତୁ -1;
		}

		px4_sem_t sem;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2 = 0;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = -1;

		ଅଚଳ unsigned NAMELEN = 32;
		ଅକ୍ଷର thread_name[NAMELEN] {};

#ifndef __PX4_QURT
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା nret = pthread_getname_np(pthread_self(), thread_name, NAMELEN);

		ଯଦି (nret || thread_name[0] == 0) {
			PX4_WARN("failed getting thread name");
		}

#endif

		PX4_DEBUG("Called px4_poll timeout = %d", timeout);

		px4_sem_init(&sem, 0, 0);

		// sem use case is a signal
		px4_sem_setprotocol(&sem, SEM_PRIO_NONE);

		// Go through all fds and check them for a pollable state
		ତାର୍କିକ fd_pollable = ମିଥ୍ୟା;

		ପାଇଁ (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < nfds; ++i) {
			fds[i].sem     = &sem;
			fds[i].revents = 0;
			fds[i].priv    = nullptr;

			cdev::CDev *dev = getFile(fds[i].fd);

			// If fd is valid
			ଯଦି (dev) {
				PX4_DEBUG("%s: px4_poll: CDev->poll(setup) %d", thread_name, fds[i].fd);
				ret = dev->poll(&filemap[fds[i].fd], &fds[i], ସତ୍ୟ);

				ଯଦି (ret < 0) {
					PX4_WARN("%s: px4_poll() error: %s", thread_name, strerror(errno));
					ଭାଙ୍ଗ;
				}

				ଯଦି (ret >= 0) {
					fd_pollable = ସତ୍ୟ;
				}
			}
		}

		// If any FD can be polled, lock the semaphore and
		// check for new data
		ଯଦି (fd_pollable) {
			ଯଦି (timeout > 0) {
				// Get the current time
				ସଂରଚନା timespec ts;
#ଯଦି defined(__PX4_DARWIN) && !defined(ENABLE_LOCKSTEP_SCHEDULER)
				// macOS has no pthread_condattr_setclock(), so px4_sem_t's condition
				// variable waits on CLOCK_REALTIME (px4_sem_init()). Use that clock here
				// too. CLOCK_MONOTONIC counts from boot, so the deadline would land
				// decades in the past and px4_sem_timedwait() would return immediately.
				px4_clock_gettime(CLOCK_REALTIME, &ts);
#ଅନ୍ୟଥା
				// Note, we can't actually use CLOCK_MONOTONIC on macOS
				// but that's hidden and implemented in px4_clock_gettime.
				px4_clock_gettime(CLOCK_MONOTONIC, &ts);
#endif

				// Calculate an absolute time in the future
				ଅଚଳ unsigned billion = (1000 * 1000 * 1000);
				uint64_t nsecs = ts.tv_nsec + ((uint64_t)timeout * 1000 * 1000);
				ts.tv_sec += nsecs / billion;
				nsecs -= (nsecs / billion) * billion;
				ts.tv_nsec = nsecs;

				ret = px4_sem_timedwait(&sem, &ts);

				ଯଦି (ret && errno != ETIMEDOUT) {
					PX4_WARN("%s: px4_poll() sem error: %s", thread_name, strerror(errno));
				}

			} ଅନ୍ୟଥା ଯଦି (timeout < 0) {
				px4_sem_wait(&sem);
			}

			// We have waited now (or not, depending on timeout),
			// go through all fds and count how many have data
			ପାଇଁ (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < nfds; ++i) {

				cdev::CDev *dev = getFile(fds[i].fd);

				// If fd is valid
				ଯଦି (dev) {
					PX4_DEBUG("%s: px4_poll: CDev->poll(teardown) %d", thread_name, fds[i].fd);
					ret = dev->poll(&filemap[fds[i].fd], &fds[i], ମିଥ୍ୟା);

					ଯଦି (ret < 0) {
						PX4_WARN("%s: px4_poll() 2nd poll fail", thread_name);
						ଭାଙ୍ଗ;
					}

					ଯଦି (fds[i].revents) {
						ଗଣନା_2 += 1;
					}
				}
			}
		}

		px4_sem_destroy(&sem);

		// Return the positive count if present,
		// return the negative error number if failed
		ଫେରନ୍ତୁ (ଗଣନା_2) ? ଗଣନା_2 : ret;
	}

	ପୂର୍ଣ୍ଣସଂଖ୍ୟା px4_access(ଅଚଳ ଅକ୍ଷର *pathname, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mode)
	{
		ଯଦି (mode != F_OK) {
			errno = EINVAL;
			ଫେରନ୍ତୁ -1;
		}

		cdev::CDev *dev = getDev(pathname);
		ଫେରନ୍ତୁ (dev != nullptr) ? 0 : -1;
	}

	ଶୂନ୍ୟ px4_show_files()
	{
		PX4_INFO("Files:");

		pthread_mutex_lock(&devmutex);

		ପାଇଁ (ଅଚଳ auto &dev : devmap) {
			ଯଦି (dev) {
				PX4_INFO_RAW("   %s\n", dev->name);
			}
		}

		pthread_mutex_unlock(&devmutex);
	}

} // extern "C"
