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

#ضمن "cdev_platform.hpp"

#ضمن "../CDev.hpp"

#ضمن <px4_platform_common/log.h>
#ضمن <px4_platform_common/posix.h>
#ضمن <px4_platform_common/time.h>

#ضمن <stdlib.h>

ساكن ثابت cdev::px4_file_operations_t g_fops = {};
ثابت cdev::px4_file_operations_t &cdev::CDev::fops_ref() { החזר g_fops; }

pthread_mutex_t devmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t filemutex = PTHREAD_MUTEX_INITIALIZER;

بنية px4_dev_t {
	حرف *name{nullptr};
	cdev::CDev *cdev{nullptr};

	px4_dev_t(ثابت حرف *n, cdev::CDev *c) : cdev(c)
	{
		name = strdup(n);
	}

	~px4_dev_t()
	{
		حرر(name);
	}
خاص:
	px4_dev_t() = افتراضي;
};

#define PX4_MAX_FD 512
ساكن px4_dev_t *devmap[PX4_MAX_FD] {};
ساكن cdev::file_t filemap[PX4_MAX_FD] {};

מחלקה VFile : عام cdev::CDev
{
عام:
	VFile(ثابت حرف *fname, mode_t mode) : cdev::CDev(fname) {}
	~VFile() override = افتراضي;

	ssize_t اكتب(cdev::file_t *handlep, ثابت حرف *buffer, size_t buflen) override
	{
		// ignore what was written, but let pollers know something was written
		poll_notify(POLLIN);
		החזר buflen;
	}
};

ساكن cdev::CDev *getDev(ثابت حرف *path)
{
	pthread_mutex_lock(&devmutex);

	עבור (ثابت auto &dev : devmap) {
		אם (dev && (strcmp(dev->name, path) == 0)) {
			pthread_mutex_unlock(&devmutex);
			החזר dev->cdev;
		}
	}

	pthread_mutex_unlock(&devmutex);

	החזר nullptr;
}

ساكن cdev::CDev *getFile(صحيح fd)
{
	pthread_mutex_lock(&filemutex);
	cdev::CDev *dev = nullptr;

	אם (fd < PX4_MAX_FD && fd >= 0) {
		dev = filemap[fd].cdev;
	}

	pthread_mutex_unlock(&filemutex);
	החזר dev;
}

خارجي "C" {

	صحيح register_driver(ثابت حرف *name, ثابت cdev::px4_file_operations_t *fops, cdev::mode_t mode, فراغ *data)
	{
		PX4_DEBUG("CDev::register_driver %s", name);
		صحيح ret = -ENOSPC;

		אם (name == nullptr || data == nullptr) {
			החזר -EINVAL;
		}

		pthread_mutex_lock(&devmutex);

		// Make sure the device does not already exist
		עבור (ثابت auto &dev : devmap) {
			אם (dev && (strcmp(dev->name, name) == 0)) {
				pthread_mutex_unlock(&devmutex);
				החזר -EEXIST;
			}
		}

		עבור (auto &dev : devmap) {
			אם (dev == nullptr) {
				dev = جديد px4_dev_t(name, (cdev::CDev *)data);
				PX4_DEBUG("Registered DEV %s", name);
				ret = PX4_OK;
				اكسر;
			}
		}

		pthread_mutex_unlock(&devmutex);

		אם (ret != PX4_OK) {
			PX4_ERR("No free devmap entries - increase devmap size");
		}

		החזר ret;
	}

	صحيح unregister_driver(ثابت حرف *name)
	{
		PX4_DEBUG("CDev::unregister_driver %s", name);
		صحيح ret = -EINVAL;

		אם (name == nullptr) {
			החזר -EINVAL;
		}

		pthread_mutex_lock(&devmutex);

		עבור (auto &dev : devmap) {
			אם (dev && (strcmp(name, dev->name) == 0)) {
				delete dev;
				dev = nullptr;
				PX4_DEBUG("Unregistered DEV %s", name);
				ret = PX4_OK;
				اكسر;
			}
		}

		pthread_mutex_unlock(&devmutex);

		החזר ret;
	}

	صحيح px4_open(ثابت حرف *path, صحيح flags, ...)
	{
		PX4_DEBUG("px4_open");
		cdev::CDev *dev = getDev(path);
		صحيح ret = 0;
		صحيح i;
		mode_t mode;

		אם (!dev && (flags & PX4_F_WRONLY) != 0 &&
		    strncmp(path, "/obj/", 5) != 0 &&
		    strncmp(path, "/dev/", 5) != 0) {
			va_list p;
			va_start(p, flags);
			mode = va_arg(p, صحيح);
			va_end(p);

			// Create the file
			PX4_DEBUG("Creating virtual file %s", path);
			dev = جديد VFile(path, mode);
			register_driver(path, nullptr, 0666, (فراغ *)dev);
		}

		אם (dev) {
			pthread_mutex_lock(&filemutex);

			עבור (i = 0; i < PX4_MAX_FD; ++i) {
				אם (filemap[i].cdev == nullptr) {
					filemap[i] = cdev::file_t(flags, dev);
					اكسر;
				}
			}

			pthread_mutex_unlock(&filemutex);

			אם (i < PX4_MAX_FD) {
				ret = dev->open(&filemap[i]);

			} אחרת {

				ثابت unsigned NAMELEN = 32;
				حرف thread_name[NAMELEN] {};

				PX4_WARN("%s: exceeded maximum number of file descriptors, accessing %s", thread_name, path);
#ifndef __PX4_QURT
				صحيح nret = pthread_getname_np(pthread_self(), thread_name, NAMELEN);

				אם (nret || thread_name[0] == 0) {
					PX4_WARN("failed getting thread name");
				}

#endif

				ret = -ENOENT;
			}

		} אחרת {
			ret = -EINVAL;
		}

		אם (ret < 0) {
			errno = -ret;
			החזר -1;
		}

		PX4_DEBUG("px4_open fd = %d", i);
		החזר i;
	}

	صحيح px4_close(صحيح fd)
	{
		صحيح ret;

		cdev::CDev *dev = getFile(fd);

		אם (dev) {
			pthread_mutex_lock(&filemutex);
			ret = dev->close(&filemap[fd]);

			filemap[fd].cdev = nullptr;

			pthread_mutex_unlock(&filemutex);
			PX4_DEBUG("px4_close fd = %d", fd);

		} אחרת {
			ret = -EINVAL;
		}

		אם (ret < 0) {
			ret = PX4_ERROR;
		}

		החזר ret;
	}

	ssize_t px4_read(صحيح fd, فراغ *buffer, size_t buflen)
	{
		صحيح ret;

		cdev::CDev *dev = getFile(fd);

		אם (dev) {
			PX4_DEBUG("px4_read fd = %d", fd);
			ret = dev->اقرأ(&filemap[fd], (حرف *)buffer, buflen);

		} אחרת {
			ret = -EINVAL;
		}

		אם (ret < 0) {
			ret = PX4_ERROR;
		}

		החזר ret;
	}

	ssize_t px4_write(صحيح fd, ثابت فراغ *buffer, size_t buflen)
	{
		صحيح ret;

		cdev::CDev *dev = getFile(fd);

		אם (dev) {
			PX4_DEBUG("px4_write fd = %d", fd);
			ret = dev->اكتب(&filemap[fd], (ثابت حرف *)buffer, buflen);

		} אחרת {
			ret = -EINVAL;
		}

		אם (ret < 0) {
			ret = PX4_ERROR;
		}

		החזר ret;
	}

	صحيح px4_ioctl(صحيح fd, صحيح cmd, unsigned long arg)
	{
		PX4_DEBUG("px4_ioctl fd = %d", fd);
		صحيح ret = 0;

		cdev::CDev *dev = getFile(fd);

		אם (dev) {
			ret = dev->ioctl(&filemap[fd], cmd, arg);

		} אחרת {
			ret = -EINVAL;
		}

		החזר ret;
	}

	صحيح px4_poll(px4_pollfd_struct_t *fds, unsigned صحيح nfds, صحيح timeout)
	{
		אם (nfds == 0) {
			PX4_WARN("px4_poll with no fds");
			החזר -1;
		}

		px4_sem_t sem;
		صحيح count = 0;
		صحيح ret = -1;

		ثابت unsigned NAMELEN = 32;
		حرف thread_name[NAMELEN] {};

#ifndef __PX4_QURT
		صحيح nret = pthread_getname_np(pthread_self(), thread_name, NAMELEN);

		אם (nret || thread_name[0] == 0) {
			PX4_WARN("failed getting thread name");
		}

#endif

		PX4_DEBUG("Called px4_poll timeout = %d", timeout);

		px4_sem_init(&sem, 0, 0);

		// sem use case is a signal
		px4_sem_setprotocol(&sem, SEM_PRIO_NONE);

		// Go through all fds and check them for a pollable state
		منطقي fd_pollable = שקר;

		עבור (unsigned صحيح i = 0; i < nfds; ++i) {
			fds[i].sem     = &sem;
			fds[i].revents = 0;
			fds[i].priv    = nullptr;

			cdev::CDev *dev = getFile(fds[i].fd);

			// If fd is valid
			אם (dev) {
				PX4_DEBUG("%s: px4_poll: CDev->poll(setup) %d", thread_name, fds[i].fd);
				ret = dev->poll(&filemap[fds[i].fd], &fds[i], אמת);

				אם (ret < 0) {
					PX4_WARN("%s: px4_poll() error: %s", thread_name, strerror(errno));
					اكسر;
				}

				אם (ret >= 0) {
					fd_pollable = אמת;
				}
			}
		}

		// If any FD can be polled, lock the semaphore and
		// check for new data
		אם (fd_pollable) {
			אם (timeout > 0) {
				// Get the current time
				بنية timespec ts;
#אם defined(__PX4_DARWIN) && !defined(ENABLE_LOCKSTEP_SCHEDULER)
				// macOS has no pthread_condattr_setclock(), so px4_sem_t's condition
				// variable waits on CLOCK_REALTIME (px4_sem_init()). Use that clock here
				// too. CLOCK_MONOTONIC counts from boot, so the deadline would land
				// decades in the past and px4_sem_timedwait() would return immediately.
				px4_clock_gettime(CLOCK_REALTIME, &ts);
#אחרת
				// Note, we can't actually use CLOCK_MONOTONIC on macOS
				// but that's hidden and implemented in px4_clock_gettime.
				px4_clock_gettime(CLOCK_MONOTONIC, &ts);
#endif

				// Calculate an absolute time in the future
				ثابت unsigned billion = (1000 * 1000 * 1000);
				uint64_t nsecs = ts.tv_nsec + ((uint64_t)timeout * 1000 * 1000);
				ts.tv_sec += nsecs / billion;
				nsecs -= (nsecs / billion) * billion;
				ts.tv_nsec = nsecs;

				ret = px4_sem_timedwait(&sem, &ts);

				אם (ret && errno != ETIMEDOUT) {
					PX4_WARN("%s: px4_poll() sem error: %s", thread_name, strerror(errno));
				}

			} אחרת אם (timeout < 0) {
				px4_sem_wait(&sem);
			}

			// We have waited now (or not, depending on timeout),
			// go through all fds and count how many have data
			עבור (unsigned صحيح i = 0; i < nfds; ++i) {

				cdev::CDev *dev = getFile(fds[i].fd);

				// If fd is valid
				אם (dev) {
					PX4_DEBUG("%s: px4_poll: CDev->poll(teardown) %d", thread_name, fds[i].fd);
					ret = dev->poll(&filemap[fds[i].fd], &fds[i], שקר);

					אם (ret < 0) {
						PX4_WARN("%s: px4_poll() 2nd poll fail", thread_name);
						اكسر;
					}

					אם (fds[i].revents) {
						count += 1;
					}
				}
			}
		}

		px4_sem_destroy(&sem);

		// Return the positive count if present,
		// return the negative error number if failed
		החזר (count) ? count : ret;
	}

	صحيح px4_access(ثابت حرف *pathname, صحيح mode)
	{
		אם (mode != F_OK) {
			errno = EINVAL;
			החזר -1;
		}

		cdev::CDev *dev = getDev(pathname);
		החזר (dev != nullptr) ? 0 : -1;
	}

	فراغ px4_show_files()
	{
		PX4_INFO("Files:");

		pthread_mutex_lock(&devmutex);

		עבור (ثابت auto &dev : devmap) {
			אם (dev) {
				PX4_INFO_RAW("   %s\n", dev->name);
			}
		}

		pthread_mutex_unlock(&devmutex);
	}

} // extern "C"
