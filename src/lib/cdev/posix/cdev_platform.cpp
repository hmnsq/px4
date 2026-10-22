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

#অন্তর্ভাব "cdev_platform.hpp"

#অন্তর্ভাব "../CDev.hpp"

#অন্তর্ভাব <px4_platform_common/log.h>
#অন্তর্ভাব <px4_platform_common/posix.h>
#অন্তর্ভাব <px4_platform_common/time.h>

#অন্তর্ভাব <stdlib.h>

স্থির ধ্রুবক cdev::px4_file_operations_t g_fops = {};
ধ্রুবক cdev::px4_file_operations_t &cdev::CDev::fops_ref() { ফেরত g_fops; }

pthread_mutex_t devmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t filemutex = PTHREAD_MUTEX_INITIALIZER;

কাঠামো px4_dev_t {
	অক্ষর *name{nullptr};
	cdev::CDev *cdev{nullptr};

	px4_dev_t(ধ্রুবক অক্ষর *n, cdev::CDev *c) : cdev(c)
	{
		name = strdup(n);
	}

	~px4_dev_t()
	{
		মুক্ত(name);
	}
ব্যক্তিগত:
	px4_dev_t() = পূর্বনির্ধারিত;
};

#define PX4_MAX_FD 512
স্থির px4_dev_t *devmap[PX4_MAX_FD] {};
স্থির cdev::file_t filemap[PX4_MAX_FD] {};

শ্রেণী VFile : সর্বজনীন cdev::CDev
{
সর্বজনীন:
	VFile(ধ্রুবক অক্ষর *fname, mode_t mode) : cdev::CDev(fname) {}
	~VFile() override = পূর্বনির্ধারিত;

	ssize_t লিখ(cdev::file_t *handlep, ধ্রুবক অক্ষর *বাফার_2, size_t buflen) override
	{
		// ignore what was written, but let pollers know something was written
		poll_notify(POLLIN);
		ফেরত buflen;
	}
};

স্থির cdev::CDev *getDev(ধ্রুবক অক্ষর *path)
{
	pthread_mutex_lock(&devmutex);

	জন্য (ধ্রুবক auto &dev : devmap) {
		যদি (dev && (strcmp(dev->name, path) == 0)) {
			pthread_mutex_unlock(&devmutex);
			ফেরত dev->cdev;
		}
	}

	pthread_mutex_unlock(&devmutex);

	ফেরত nullptr;
}

স্থির cdev::CDev *getFile(পূর্ণসংখ্যা fd)
{
	pthread_mutex_lock(&filemutex);
	cdev::CDev *dev = nullptr;

	যদি (fd < PX4_MAX_FD && fd >= 0) {
		dev = filemap[fd].cdev;
	}

	pthread_mutex_unlock(&filemutex);
	ফেরত dev;
}

বাহ্যিক "C" {

	পূর্ণসংখ্যা register_driver(ধ্রুবক অক্ষর *name, ধ্রুবক cdev::px4_file_operations_t *fops, cdev::mode_t mode, শূন্য *তথ্য)
	{
		PX4_DEBUG("CDev::register_driver %s", name);
		পূর্ণসংখ্যা ret = -ENOSPC;

		যদি (name == nullptr || তথ্য == nullptr) {
			ফেরত -EINVAL;
		}

		pthread_mutex_lock(&devmutex);

		// Make sure the device does not already exist
		জন্য (ধ্রুবক auto &dev : devmap) {
			যদি (dev && (strcmp(dev->name, name) == 0)) {
				pthread_mutex_unlock(&devmutex);
				ফেরত -EEXIST;
			}
		}

		জন্য (auto &dev : devmap) {
			যদি (dev == nullptr) {
				dev = নতুন px4_dev_t(name, (cdev::CDev *)তথ্য);
				PX4_DEBUG("Registered DEV %s", name);
				ret = PX4_OK;
				বিরতি;
			}
		}

		pthread_mutex_unlock(&devmutex);

		যদি (ret != PX4_OK) {
			PX4_ERR("No free devmap entries - increase devmap size");
		}

		ফেরত ret;
	}

	পূর্ণসংখ্যা unregister_driver(ধ্রুবক অক্ষর *name)
	{
		PX4_DEBUG("CDev::unregister_driver %s", name);
		পূর্ণসংখ্যা ret = -EINVAL;

		যদি (name == nullptr) {
			ফেরত -EINVAL;
		}

		pthread_mutex_lock(&devmutex);

		জন্য (auto &dev : devmap) {
			যদি (dev && (strcmp(name, dev->name) == 0)) {
				delete dev;
				dev = nullptr;
				PX4_DEBUG("Unregistered DEV %s", name);
				ret = PX4_OK;
				বিরতি;
			}
		}

		pthread_mutex_unlock(&devmutex);

		ফেরত ret;
	}

	পূর্ণসংখ্যা px4_open(ধ্রুবক অক্ষর *path, পূর্ণসংখ্যা flags, ...)
	{
		PX4_DEBUG("px4_open");
		cdev::CDev *dev = getDev(path);
		পূর্ণসংখ্যা ret = 0;
		পূর্ণসংখ্যা i;
		mode_t mode;

		যদি (!dev && (flags & PX4_F_WRONLY) != 0 &&
		    strncmp(path, "/obj/", 5) != 0 &&
		    strncmp(path, "/dev/", 5) != 0) {
			va_list p;
			va_start(p, flags);
			mode = va_arg(p, পূর্ণসংখ্যা);
			va_end(p);

			// Create the file
			PX4_DEBUG("Creating virtual file %s", path);
			dev = নতুন VFile(path, mode);
			register_driver(path, nullptr, 0666, (শূন্য *)dev);
		}

		যদি (dev) {
			pthread_mutex_lock(&filemutex);

			জন্য (i = 0; i < PX4_MAX_FD; ++i) {
				যদি (filemap[i].cdev == nullptr) {
					filemap[i] = cdev::file_t(flags, dev);
					বিরতি;
				}
			}

			pthread_mutex_unlock(&filemutex);

			যদি (i < PX4_MAX_FD) {
				ret = dev->open(&filemap[i]);

			} অন্যথা {

				ধ্রুবক unsigned NAMELEN = 32;
				অক্ষর thread_name[NAMELEN] {};

				PX4_WARN("%s: exceeded maximum number of file descriptors, accessing %s", thread_name, path);
#ifndef __PX4_QURT
				পূর্ণসংখ্যা nret = pthread_getname_np(pthread_self(), thread_name, NAMELEN);

				যদি (nret || thread_name[0] == 0) {
					PX4_WARN("failed getting thread name");
				}

#endif

				ret = -ENOENT;
			}

		} অন্যথা {
			ret = -EINVAL;
		}

		যদি (ret < 0) {
			errno = -ret;
			ফেরত -1;
		}

		PX4_DEBUG("px4_open fd = %d", i);
		ফেরত i;
	}

	পূর্ণসংখ্যা px4_close(পূর্ণসংখ্যা fd)
	{
		পূর্ণসংখ্যা ret;

		cdev::CDev *dev = getFile(fd);

		যদি (dev) {
			pthread_mutex_lock(&filemutex);
			ret = dev->close(&filemap[fd]);

			filemap[fd].cdev = nullptr;

			pthread_mutex_unlock(&filemutex);
			PX4_DEBUG("px4_close fd = %d", fd);

		} অন্যথা {
			ret = -EINVAL;
		}

		যদি (ret < 0) {
			ret = PX4_ERROR;
		}

		ফেরত ret;
	}

	ssize_t px4_read(পূর্ণসংখ্যা fd, শূন্য *বাফার_2, size_t buflen)
	{
		পূর্ণসংখ্যা ret;

		cdev::CDev *dev = getFile(fd);

		যদি (dev) {
			PX4_DEBUG("px4_read fd = %d", fd);
			ret = dev->পঠ(&filemap[fd], (অক্ষর *)বাফার_2, buflen);

		} অন্যথা {
			ret = -EINVAL;
		}

		যদি (ret < 0) {
			ret = PX4_ERROR;
		}

		ফেরত ret;
	}

	ssize_t px4_write(পূর্ণসংখ্যা fd, ধ্রুবক শূন্য *বাফার_2, size_t buflen)
	{
		পূর্ণসংখ্যা ret;

		cdev::CDev *dev = getFile(fd);

		যদি (dev) {
			PX4_DEBUG("px4_write fd = %d", fd);
			ret = dev->লিখ(&filemap[fd], (ধ্রুবক অক্ষর *)বাফার_2, buflen);

		} অন্যথা {
			ret = -EINVAL;
		}

		যদি (ret < 0) {
			ret = PX4_ERROR;
		}

		ফেরত ret;
	}

	পূর্ণসংখ্যা px4_ioctl(পূর্ণসংখ্যা fd, পূর্ণসংখ্যা cmd, unsigned long arg)
	{
		PX4_DEBUG("px4_ioctl fd = %d", fd);
		পূর্ণসংখ্যা ret = 0;

		cdev::CDev *dev = getFile(fd);

		যদি (dev) {
			ret = dev->ioctl(&filemap[fd], cmd, arg);

		} অন্যথা {
			ret = -EINVAL;
		}

		ফেরত ret;
	}

	পূর্ণসংখ্যা px4_poll(px4_pollfd_struct_t *fds, unsigned পূর্ণসংখ্যা nfds, পূর্ণসংখ্যা timeout)
	{
		যদি (nfds == 0) {
			PX4_WARN("px4_poll with no fds");
			ফেরত -1;
		}

		px4_sem_t sem;
		পূর্ণসংখ্যা গুনতি = 0;
		পূর্ণসংখ্যা ret = -1;

		ধ্রুবক unsigned NAMELEN = 32;
		অক্ষর thread_name[NAMELEN] {};

#ifndef __PX4_QURT
		পূর্ণসংখ্যা nret = pthread_getname_np(pthread_self(), thread_name, NAMELEN);

		যদি (nret || thread_name[0] == 0) {
			PX4_WARN("failed getting thread name");
		}

#endif

		PX4_DEBUG("Called px4_poll timeout = %d", timeout);

		px4_sem_init(&sem, 0, 0);

		// sem use case is a signal
		px4_sem_setprotocol(&sem, SEM_PRIO_NONE);

		// Go through all fds and check them for a pollable state
		যৌক্তিক fd_pollable = মিথ্যা;

		জন্য (unsigned পূর্ণসংখ্যা i = 0; i < nfds; ++i) {
			fds[i].sem     = &sem;
			fds[i].revents = 0;
			fds[i].priv    = nullptr;

			cdev::CDev *dev = getFile(fds[i].fd);

			// If fd is valid
			যদি (dev) {
				PX4_DEBUG("%s: px4_poll: CDev->poll(setup) %d", thread_name, fds[i].fd);
				ret = dev->poll(&filemap[fds[i].fd], &fds[i], সত্য);

				যদি (ret < 0) {
					PX4_WARN("%s: px4_poll() error: %s", thread_name, strerror(errno));
					বিরতি;
				}

				যদি (ret >= 0) {
					fd_pollable = সত্য;
				}
			}
		}

		// If any FD can be polled, lock the semaphore and
		// check for new data
		যদি (fd_pollable) {
			যদি (timeout > 0) {
				// Get the current time
				কাঠামো timespec ts;
#যদি defined(__PX4_DARWIN) && !defined(ENABLE_LOCKSTEP_SCHEDULER)
				// macOS has no pthread_condattr_setclock(), so px4_sem_t's condition
				// variable waits on CLOCK_REALTIME (px4_sem_init()). Use that clock here
				// too. CLOCK_MONOTONIC counts from boot, so the deadline would land
				// decades in the past and px4_sem_timedwait() would return immediately.
				px4_clock_gettime(CLOCK_REALTIME, &ts);
#অন্যথা
				// Note, we can't actually use CLOCK_MONOTONIC on macOS
				// but that's hidden and implemented in px4_clock_gettime.
				px4_clock_gettime(CLOCK_MONOTONIC, &ts);
#endif

				// Calculate an absolute time in the future
				ধ্রুবক unsigned billion = (1000 * 1000 * 1000);
				uint64_t nsecs = ts.tv_nsec + ((uint64_t)timeout * 1000 * 1000);
				ts.tv_sec += nsecs / billion;
				nsecs -= (nsecs / billion) * billion;
				ts.tv_nsec = nsecs;

				ret = px4_sem_timedwait(&sem, &ts);

				যদি (ret && errno != ETIMEDOUT) {
					PX4_WARN("%s: px4_poll() sem error: %s", thread_name, strerror(errno));
				}

			} অন্যথা যদি (timeout < 0) {
				px4_sem_wait(&sem);
			}

			// We have waited now (or not, depending on timeout),
			// go through all fds and count how many have data
			জন্য (unsigned পূর্ণসংখ্যা i = 0; i < nfds; ++i) {

				cdev::CDev *dev = getFile(fds[i].fd);

				// If fd is valid
				যদি (dev) {
					PX4_DEBUG("%s: px4_poll: CDev->poll(teardown) %d", thread_name, fds[i].fd);
					ret = dev->poll(&filemap[fds[i].fd], &fds[i], মিথ্যা);

					যদি (ret < 0) {
						PX4_WARN("%s: px4_poll() 2nd poll fail", thread_name);
						বিরতি;
					}

					যদি (fds[i].revents) {
						গুনতি += 1;
					}
				}
			}
		}

		px4_sem_destroy(&sem);

		// Return the positive count if present,
		// return the negative error number if failed
		ফেরত (গুনতি) ? গুনতি : ret;
	}

	পূর্ণসংখ্যা px4_access(ধ্রুবক অক্ষর *pathname, পূর্ণসংখ্যা mode)
	{
		যদি (mode != F_OK) {
			errno = EINVAL;
			ফেরত -1;
		}

		cdev::CDev *dev = getDev(pathname);
		ফেরত (dev != nullptr) ? 0 : -1;
	}

	শূন্য px4_show_files()
	{
		PX4_INFO("Files:");

		pthread_mutex_lock(&devmutex);

		জন্য (ধ্রুবক auto &dev : devmap) {
			যদি (dev) {
				PX4_INFO_RAW("   %s\n", dev->name);
			}
		}

		pthread_mutex_unlock(&devmutex);
	}

} // extern "C"
