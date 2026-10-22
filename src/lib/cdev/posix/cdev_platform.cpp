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

#அந்தர்பாவ "cdev_platform.hpp"

#அந்தர்பாவ "../CDev.hpp"

#அந்தர்பாவ <px4_platform_common/log.h>
#அந்தர்பாவ <px4_platform_common/posix.h>
#அந்தர்பாவ <px4_platform_common/time.h>

#அந்தர்பாவ <stdlib.h>

நிலையான மாறிலி cdev::px4_file_operations_t g_fops = {};
மாறிலி cdev::px4_file_operations_t &cdev::CDev::fops_ref() { திரும்பு g_fops; }

pthread_mutex_t devmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t filemutex = PTHREAD_MUTEX_INITIALIZER;

கட்டமைப்பு px4_dev_t {
	எழுத்து *name{nullptr};
	cdev::CDev *cdev{nullptr};

	px4_dev_t(மாறிலி எழுத்து *n, cdev::CDev *c) : cdev(c)
	{
		name = strdup(n);
	}

	~px4_dev_t()
	{
		விடுவி(name);
	}
தனிப்பட்ட:
	px4_dev_t() = இயல்பு;
};

#define PX4_MAX_FD 512
நிலையான px4_dev_t *devmap[PX4_MAX_FD] {};
நிலையான cdev::file_t filemap[PX4_MAX_FD] {};

வகுப்பு VFile : பொது cdev::CDev
{
பொது:
	VFile(மாறிலி எழுத்து *fname, mode_t mode) : cdev::CDev(fname) {}
	~VFile() override = இயல்பு;

	ssize_t லிக(cdev::file_t *handlep, மாறிலி எழுத்து *buffer, size_t buflen) override
	{
		// ignore what was written, but let pollers know something was written
		poll_notify(POLLIN);
		திரும்பு buflen;
	}
};

நிலையான cdev::CDev *getDev(மாறிலி எழுத்து *path)
{
	pthread_mutex_lock(&devmutex);

	க்கு (மாறிலி auto &dev : devmap) {
		எனில் (dev && (strcmp(dev->name, path) == 0)) {
			pthread_mutex_unlock(&devmutex);
			திரும்பு dev->cdev;
		}
	}

	pthread_mutex_unlock(&devmutex);

	திரும்பு nullptr;
}

நிலையான cdev::CDev *getFile(முழு fd)
{
	pthread_mutex_lock(&filemutex);
	cdev::CDev *dev = nullptr;

	எனில் (fd < PX4_MAX_FD && fd >= 0) {
		dev = filemap[fd].cdev;
	}

	pthread_mutex_unlock(&filemutex);
	திரும்பு dev;
}

வெளி "C" {

	முழு register_driver(மாறிலி எழுத்து *name, மாறிலி cdev::px4_file_operations_t *fops, cdev::mode_t mode, வெறுமை *data)
	{
		PX4_DEBUG("CDev::register_driver %s", name);
		முழு ret = -ENOSPC;

		எனில் (name == nullptr || data == nullptr) {
			திரும்பு -EINVAL;
		}

		pthread_mutex_lock(&devmutex);

		// Make sure the device does not already exist
		க்கு (மாறிலி auto &dev : devmap) {
			எனில் (dev && (strcmp(dev->name, name) == 0)) {
				pthread_mutex_unlock(&devmutex);
				திரும்பு -EEXIST;
			}
		}

		க்கு (auto &dev : devmap) {
			எனில் (dev == nullptr) {
				dev = புதிய px4_dev_t(name, (cdev::CDev *)data);
				PX4_DEBUG("Registered DEV %s", name);
				ret = PX4_OK;
				நிறுத்து;
			}
		}

		pthread_mutex_unlock(&devmutex);

		எனில் (ret != PX4_OK) {
			PX4_ERR("No free devmap entries - increase devmap size");
		}

		திரும்பு ret;
	}

	முழு unregister_driver(மாறிலி எழுத்து *name)
	{
		PX4_DEBUG("CDev::unregister_driver %s", name);
		முழு ret = -EINVAL;

		எனில் (name == nullptr) {
			திரும்பு -EINVAL;
		}

		pthread_mutex_lock(&devmutex);

		க்கு (auto &dev : devmap) {
			எனில் (dev && (strcmp(name, dev->name) == 0)) {
				delete dev;
				dev = nullptr;
				PX4_DEBUG("Unregistered DEV %s", name);
				ret = PX4_OK;
				நிறுத்து;
			}
		}

		pthread_mutex_unlock(&devmutex);

		திரும்பு ret;
	}

	முழு px4_open(மாறிலி எழுத்து *path, முழு flags, ...)
	{
		PX4_DEBUG("px4_open");
		cdev::CDev *dev = getDev(path);
		முழு ret = 0;
		முழு i;
		mode_t mode;

		எனில் (!dev && (flags & PX4_F_WRONLY) != 0 &&
		    strncmp(path, "/obj/", 5) != 0 &&
		    strncmp(path, "/dev/", 5) != 0) {
			va_list p;
			va_start(p, flags);
			mode = va_arg(p, முழு);
			va_end(p);

			// Create the file
			PX4_DEBUG("Creating virtual file %s", path);
			dev = புதிய VFile(path, mode);
			register_driver(path, nullptr, 0666, (வெறுமை *)dev);
		}

		எனில் (dev) {
			pthread_mutex_lock(&filemutex);

			க்கு (i = 0; i < PX4_MAX_FD; ++i) {
				எனில் (filemap[i].cdev == nullptr) {
					filemap[i] = cdev::file_t(flags, dev);
					நிறுத்து;
				}
			}

			pthread_mutex_unlock(&filemutex);

			எனில் (i < PX4_MAX_FD) {
				ret = dev->open(&filemap[i]);

			} இல்லையெனில் {

				மாறிலி unsigned NAMELEN = 32;
				எழுத்து thread_name[NAMELEN] {};

				PX4_WARN("%s: exceeded maximum number of file descriptors, accessing %s", thread_name, path);
#ifndef __PX4_QURT
				முழு nret = pthread_getname_np(pthread_self(), thread_name, NAMELEN);

				எனில் (nret || thread_name[0] == 0) {
					PX4_WARN("failed getting thread name");
				}

#endif

				ret = -ENOENT;
			}

		} இல்லையெனில் {
			ret = -EINVAL;
		}

		எனில் (ret < 0) {
			errno = -ret;
			திரும்பு -1;
		}

		PX4_DEBUG("px4_open fd = %d", i);
		திரும்பு i;
	}

	முழு px4_close(முழு fd)
	{
		முழு ret;

		cdev::CDev *dev = getFile(fd);

		எனில் (dev) {
			pthread_mutex_lock(&filemutex);
			ret = dev->close(&filemap[fd]);

			filemap[fd].cdev = nullptr;

			pthread_mutex_unlock(&filemutex);
			PX4_DEBUG("px4_close fd = %d", fd);

		} இல்லையெனில் {
			ret = -EINVAL;
		}

		எனில் (ret < 0) {
			ret = PX4_ERROR;
		}

		திரும்பு ret;
	}

	ssize_t px4_read(முழு fd, வெறுமை *buffer, size_t buflen)
	{
		முழு ret;

		cdev::CDev *dev = getFile(fd);

		எனில் (dev) {
			PX4_DEBUG("px4_read fd = %d", fd);
			ret = dev->பட(&filemap[fd], (எழுத்து *)buffer, buflen);

		} இல்லையெனில் {
			ret = -EINVAL;
		}

		எனில் (ret < 0) {
			ret = PX4_ERROR;
		}

		திரும்பு ret;
	}

	ssize_t px4_write(முழு fd, மாறிலி வெறுமை *buffer, size_t buflen)
	{
		முழு ret;

		cdev::CDev *dev = getFile(fd);

		எனில் (dev) {
			PX4_DEBUG("px4_write fd = %d", fd);
			ret = dev->லிக(&filemap[fd], (மாறிலி எழுத்து *)buffer, buflen);

		} இல்லையெனில் {
			ret = -EINVAL;
		}

		எனில் (ret < 0) {
			ret = PX4_ERROR;
		}

		திரும்பு ret;
	}

	முழு px4_ioctl(முழு fd, முழு cmd, unsigned long arg)
	{
		PX4_DEBUG("px4_ioctl fd = %d", fd);
		முழு ret = 0;

		cdev::CDev *dev = getFile(fd);

		எனில் (dev) {
			ret = dev->ioctl(&filemap[fd], cmd, arg);

		} இல்லையெனில் {
			ret = -EINVAL;
		}

		திரும்பு ret;
	}

	முழு px4_poll(px4_pollfd_struct_t *fds, unsigned முழு nfds, முழு timeout)
	{
		எனில் (nfds == 0) {
			PX4_WARN("px4_poll with no fds");
			திரும்பு -1;
		}

		px4_sem_t sem;
		முழு மொத்தம் = 0;
		முழு ret = -1;

		மாறிலி unsigned NAMELEN = 32;
		எழுத்து thread_name[NAMELEN] {};

#ifndef __PX4_QURT
		முழு nret = pthread_getname_np(pthread_self(), thread_name, NAMELEN);

		எனில் (nret || thread_name[0] == 0) {
			PX4_WARN("failed getting thread name");
		}

#endif

		PX4_DEBUG("Called px4_poll timeout = %d", timeout);

		px4_sem_init(&sem, 0, 0);

		// sem use case is a signal
		px4_sem_setprotocol(&sem, SEM_PRIO_NONE);

		// Go through all fds and check them for a pollable state
		தருக்க fd_pollable = பொய்;

		க்கு (unsigned முழு i = 0; i < nfds; ++i) {
			fds[i].sem     = &sem;
			fds[i].revents = 0;
			fds[i].priv    = nullptr;

			cdev::CDev *dev = getFile(fds[i].fd);

			// If fd is valid
			எனில் (dev) {
				PX4_DEBUG("%s: px4_poll: CDev->poll(setup) %d", thread_name, fds[i].fd);
				ret = dev->poll(&filemap[fds[i].fd], &fds[i], உண்மை);

				எனில் (ret < 0) {
					PX4_WARN("%s: px4_poll() error: %s", thread_name, strerror(errno));
					நிறுத்து;
				}

				எனில் (ret >= 0) {
					fd_pollable = உண்மை;
				}
			}
		}

		// If any FD can be polled, lock the semaphore and
		// check for new data
		எனில் (fd_pollable) {
			எனில் (timeout > 0) {
				// Get the current time
				கட்டமைப்பு timespec ts;
#எனில் defined(__PX4_DARWIN) && !defined(ENABLE_LOCKSTEP_SCHEDULER)
				// macOS has no pthread_condattr_setclock(), so px4_sem_t's condition
				// variable waits on CLOCK_REALTIME (px4_sem_init()). Use that clock here
				// too. CLOCK_MONOTONIC counts from boot, so the deadline would land
				// decades in the past and px4_sem_timedwait() would return immediately.
				px4_clock_gettime(CLOCK_REALTIME, &ts);
#இல்லையெனில்
				// Note, we can't actually use CLOCK_MONOTONIC on macOS
				// but that's hidden and implemented in px4_clock_gettime.
				px4_clock_gettime(CLOCK_MONOTONIC, &ts);
#endif

				// Calculate an absolute time in the future
				மாறிலி unsigned billion = (1000 * 1000 * 1000);
				uint64_t nsecs = ts.tv_nsec + ((uint64_t)timeout * 1000 * 1000);
				ts.tv_sec += nsecs / billion;
				nsecs -= (nsecs / billion) * billion;
				ts.tv_nsec = nsecs;

				ret = px4_sem_timedwait(&sem, &ts);

				எனில் (ret && errno != ETIMEDOUT) {
					PX4_WARN("%s: px4_poll() sem error: %s", thread_name, strerror(errno));
				}

			} இல்லையெனில் எனில் (timeout < 0) {
				px4_sem_wait(&sem);
			}

			// We have waited now (or not, depending on timeout),
			// go through all fds and count how many have data
			க்கு (unsigned முழு i = 0; i < nfds; ++i) {

				cdev::CDev *dev = getFile(fds[i].fd);

				// If fd is valid
				எனில் (dev) {
					PX4_DEBUG("%s: px4_poll: CDev->poll(teardown) %d", thread_name, fds[i].fd);
					ret = dev->poll(&filemap[fds[i].fd], &fds[i], பொய்);

					எனில் (ret < 0) {
						PX4_WARN("%s: px4_poll() 2nd poll fail", thread_name);
						நிறுத்து;
					}

					எனில் (fds[i].revents) {
						மொத்தம் += 1;
					}
				}
			}
		}

		px4_sem_destroy(&sem);

		// Return the positive count if present,
		// return the negative error number if failed
		திரும்பு (மொத்தம்) ? மொத்தம் : ret;
	}

	முழு px4_access(மாறிலி எழுத்து *pathname, முழு mode)
	{
		எனில் (mode != F_OK) {
			errno = EINVAL;
			திரும்பு -1;
		}

		cdev::CDev *dev = getDev(pathname);
		திரும்பு (dev != nullptr) ? 0 : -1;
	}

	வெறுமை px4_show_files()
	{
		PX4_INFO("Files:");

		pthread_mutex_lock(&devmutex);

		க்கு (மாறிலி auto &dev : devmap) {
			எனில் (dev) {
				PX4_INFO_RAW("   %s\n", dev->name);
			}
		}

		pthread_mutex_unlock(&devmutex);
	}

} // extern "C"
