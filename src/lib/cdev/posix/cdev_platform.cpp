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

#شامل "cdev_platform.hpp"

#شامل "../CDev.hpp"

#شامل <px4_platform_common/log.h>
#شامل <px4_platform_common/posix.h>
#شامل <px4_platform_common/time.h>

#شامل <stdlib.h>

ایستا ثابت cdev::px4_file_operations_t g_fops = {};
ثابت cdev::px4_file_operations_t &cdev::CDev::fops_ref() { بازگشت g_fops; }

pthread_mutex_t devmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t filemutex = PTHREAD_MUTEX_INITIALIZER;

ساختار px4_dev_t {
	نویسه *name{nullptr};
	cdev::CDev *cdev{nullptr};

	px4_dev_t(ثابت نویسه *n, cdev::CDev *c) : cdev(c)
	{
		name = strdup(n);
	}

	~px4_dev_t()
	{
		آزاد(name);
	}
خصوصی:
	px4_dev_t() = پیشفرض;
};

#define PX4_MAX_FD 512
ایستا px4_dev_t *devmap[PX4_MAX_FD] {};
ایستا cdev::file_t filemap[PX4_MAX_FD] {};

رده VFile : عمومی cdev::CDev
{
عمومی:
	VFile(ثابت نویسه *fname, mode_t mode) : cdev::CDev(fname) {}
	~VFile() override = پیشفرض;

	ssize_t اكتب(cdev::file_t *handlep, ثابت نویسه *buffer, size_t buflen) override
	{
		// ignore what was written, but let pollers know something was written
		poll_notify(POLLIN);
		بازگشت buflen;
	}
};

ایستا cdev::CDev *getDev(ثابت نویسه *path)
{
	pthread_mutex_lock(&devmutex);

	برای (ثابت auto &dev : devmap) {
		اگر (dev && (strcmp(dev->name, path) == 0)) {
			pthread_mutex_unlock(&devmutex);
			بازگشت dev->cdev;
		}
	}

	pthread_mutex_unlock(&devmutex);

	بازگشت nullptr;
}

ایستا cdev::CDev *getFile(صحیح fd)
{
	pthread_mutex_lock(&filemutex);
	cdev::CDev *dev = nullptr;

	اگر (fd < PX4_MAX_FD && fd >= 0) {
		dev = filemap[fd].cdev;
	}

	pthread_mutex_unlock(&filemutex);
	بازگشت dev;
}

بیرونی "C" {

	صحیح register_driver(ثابت نویسه *name, ثابت cdev::px4_file_operations_t *fops, cdev::mode_t mode, تهی *data)
	{
		PX4_DEBUG("CDev::register_driver %s", name);
		صحیح ret = -ENOSPC;

		اگر (name == nullptr || data == nullptr) {
			بازگشت -EINVAL;
		}

		pthread_mutex_lock(&devmutex);

		// Make sure the device does not already exist
		برای (ثابت auto &dev : devmap) {
			اگر (dev && (strcmp(dev->name, name) == 0)) {
				pthread_mutex_unlock(&devmutex);
				بازگشت -EEXIST;
			}
		}

		برای (auto &dev : devmap) {
			اگر (dev == nullptr) {
				dev = جدید px4_dev_t(name, (cdev::CDev *)data);
				PX4_DEBUG("Registered DEV %s", name);
				ret = PX4_OK;
				بشکن;
			}
		}

		pthread_mutex_unlock(&devmutex);

		اگر (ret != PX4_OK) {
			PX4_ERR("No free devmap entries - increase devmap size");
		}

		بازگشت ret;
	}

	صحیح unregister_driver(ثابت نویسه *name)
	{
		PX4_DEBUG("CDev::unregister_driver %s", name);
		صحیح ret = -EINVAL;

		اگر (name == nullptr) {
			بازگشت -EINVAL;
		}

		pthread_mutex_lock(&devmutex);

		برای (auto &dev : devmap) {
			اگر (dev && (strcmp(name, dev->name) == 0)) {
				delete dev;
				dev = nullptr;
				PX4_DEBUG("Unregistered DEV %s", name);
				ret = PX4_OK;
				بشکن;
			}
		}

		pthread_mutex_unlock(&devmutex);

		بازگشت ret;
	}

	صحیح px4_open(ثابت نویسه *path, صحیح flags, ...)
	{
		PX4_DEBUG("px4_open");
		cdev::CDev *dev = getDev(path);
		صحیح ret = 0;
		صحیح i;
		mode_t mode;

		اگر (!dev && (flags & PX4_F_WRONLY) != 0 &&
		    strncmp(path, "/obj/", 5) != 0 &&
		    strncmp(path, "/dev/", 5) != 0) {
			va_list p;
			va_start(p, flags);
			mode = va_arg(p, صحیح);
			va_end(p);

			// Create the file
			PX4_DEBUG("Creating virtual file %s", path);
			dev = جدید VFile(path, mode);
			register_driver(path, nullptr, 0666, (تهی *)dev);
		}

		اگر (dev) {
			pthread_mutex_lock(&filemutex);

			برای (i = 0; i < PX4_MAX_FD; ++i) {
				اگر (filemap[i].cdev == nullptr) {
					filemap[i] = cdev::file_t(flags, dev);
					بشکن;
				}
			}

			pthread_mutex_unlock(&filemutex);

			اگر (i < PX4_MAX_FD) {
				ret = dev->open(&filemap[i]);

			} وگرنه {

				ثابت unsigned NAMELEN = 32;
				نویسه thread_name[NAMELEN] {};

				PX4_WARN("%s: exceeded maximum number of file descriptors, accessing %s", thread_name, path);
#ifndef __PX4_QURT
				صحیح nret = pthread_getname_np(pthread_self(), thread_name, NAMELEN);

				اگر (nret || thread_name[0] == 0) {
					PX4_WARN("failed getting thread name");
				}

#endif

				ret = -ENOENT;
			}

		} وگرنه {
			ret = -EINVAL;
		}

		اگر (ret < 0) {
			errno = -ret;
			بازگشت -1;
		}

		PX4_DEBUG("px4_open fd = %d", i);
		بازگشت i;
	}

	صحیح px4_close(صحیح fd)
	{
		صحیح ret;

		cdev::CDev *dev = getFile(fd);

		اگر (dev) {
			pthread_mutex_lock(&filemutex);
			ret = dev->close(&filemap[fd]);

			filemap[fd].cdev = nullptr;

			pthread_mutex_unlock(&filemutex);
			PX4_DEBUG("px4_close fd = %d", fd);

		} وگرنه {
			ret = -EINVAL;
		}

		اگر (ret < 0) {
			ret = PX4_ERROR;
		}

		بازگشت ret;
	}

	ssize_t px4_read(صحیح fd, تهی *buffer, size_t buflen)
	{
		صحیح ret;

		cdev::CDev *dev = getFile(fd);

		اگر (dev) {
			PX4_DEBUG("px4_read fd = %d", fd);
			ret = dev->اقرأ(&filemap[fd], (نویسه *)buffer, buflen);

		} وگرنه {
			ret = -EINVAL;
		}

		اگر (ret < 0) {
			ret = PX4_ERROR;
		}

		بازگشت ret;
	}

	ssize_t px4_write(صحیح fd, ثابت تهی *buffer, size_t buflen)
	{
		صحیح ret;

		cdev::CDev *dev = getFile(fd);

		اگر (dev) {
			PX4_DEBUG("px4_write fd = %d", fd);
			ret = dev->اكتب(&filemap[fd], (ثابت نویسه *)buffer, buflen);

		} وگرنه {
			ret = -EINVAL;
		}

		اگر (ret < 0) {
			ret = PX4_ERROR;
		}

		بازگشت ret;
	}

	صحیح px4_ioctl(صحیح fd, صحیح cmd, unsigned long arg)
	{
		PX4_DEBUG("px4_ioctl fd = %d", fd);
		صحیح ret = 0;

		cdev::CDev *dev = getFile(fd);

		اگر (dev) {
			ret = dev->ioctl(&filemap[fd], cmd, arg);

		} وگرنه {
			ret = -EINVAL;
		}

		بازگشت ret;
	}

	صحیح px4_poll(px4_pollfd_struct_t *fds, unsigned صحیح nfds, صحیح timeout)
	{
		اگر (nfds == 0) {
			PX4_WARN("px4_poll with no fds");
			بازگشت -1;
		}

		px4_sem_t sem;
		صحیح شمار = 0;
		صحیح ret = -1;

		ثابت unsigned NAMELEN = 32;
		نویسه thread_name[NAMELEN] {};

#ifndef __PX4_QURT
		صحیح nret = pthread_getname_np(pthread_self(), thread_name, NAMELEN);

		اگر (nret || thread_name[0] == 0) {
			PX4_WARN("failed getting thread name");
		}

#endif

		PX4_DEBUG("Called px4_poll timeout = %d", timeout);

		px4_sem_init(&sem, 0, 0);

		// sem use case is a signal
		px4_sem_setprotocol(&sem, SEM_PRIO_NONE);

		// Go through all fds and check them for a pollable state
		منطقی fd_pollable = نادرست;

		برای (unsigned صحیح i = 0; i < nfds; ++i) {
			fds[i].sem     = &sem;
			fds[i].revents = 0;
			fds[i].priv    = nullptr;

			cdev::CDev *dev = getFile(fds[i].fd);

			// If fd is valid
			اگر (dev) {
				PX4_DEBUG("%s: px4_poll: CDev->poll(setup) %d", thread_name, fds[i].fd);
				ret = dev->poll(&filemap[fds[i].fd], &fds[i], درست);

				اگر (ret < 0) {
					PX4_WARN("%s: px4_poll() error: %s", thread_name, strerror(errno));
					بشکن;
				}

				اگر (ret >= 0) {
					fd_pollable = درست;
				}
			}
		}

		// If any FD can be polled, lock the semaphore and
		// check for new data
		اگر (fd_pollable) {
			اگر (timeout > 0) {
				// Get the current time
				ساختار timespec ts;
#اگر defined(__PX4_DARWIN) && !defined(ENABLE_LOCKSTEP_SCHEDULER)
				// macOS has no pthread_condattr_setclock(), so px4_sem_t's condition
				// variable waits on CLOCK_REALTIME (px4_sem_init()). Use that clock here
				// too. CLOCK_MONOTONIC counts from boot, so the deadline would land
				// decades in the past and px4_sem_timedwait() would return immediately.
				px4_clock_gettime(CLOCK_REALTIME, &ts);
#وگرنه
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

				اگر (ret && errno != ETIMEDOUT) {
					PX4_WARN("%s: px4_poll() sem error: %s", thread_name, strerror(errno));
				}

			} وگرنه اگر (timeout < 0) {
				px4_sem_wait(&sem);
			}

			// We have waited now (or not, depending on timeout),
			// go through all fds and count how many have data
			برای (unsigned صحیح i = 0; i < nfds; ++i) {

				cdev::CDev *dev = getFile(fds[i].fd);

				// If fd is valid
				اگر (dev) {
					PX4_DEBUG("%s: px4_poll: CDev->poll(teardown) %d", thread_name, fds[i].fd);
					ret = dev->poll(&filemap[fds[i].fd], &fds[i], نادرست);

					اگر (ret < 0) {
						PX4_WARN("%s: px4_poll() 2nd poll fail", thread_name);
						بشکن;
					}

					اگر (fds[i].revents) {
						شمار += 1;
					}
				}
			}
		}

		px4_sem_destroy(&sem);

		// Return the positive count if present,
		// return the negative error number if failed
		بازگشت (شمار) ? شمار : ret;
	}

	صحیح px4_access(ثابت نویسه *pathname, صحیح mode)
	{
		اگر (mode != F_OK) {
			errno = EINVAL;
			بازگشت -1;
		}

		cdev::CDev *dev = getDev(pathname);
		بازگشت (dev != nullptr) ? 0 : -1;
	}

	تهی px4_show_files()
	{
		PX4_INFO("Files:");

		pthread_mutex_lock(&devmutex);

		برای (ثابت auto &dev : devmap) {
			اگر (dev) {
				PX4_INFO_RAW("   %s\n", dev->name);
			}
		}

		pthread_mutex_unlock(&devmutex);
	}

} // extern "C"
