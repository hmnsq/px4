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

#include "cdev_platform.hpp"

#include "../CDev.hpp"

#include <px4_platform_common/log.h>
#include <px4_platform_common/posix.h>
#include <px4_platform_common/time.h>

#include <stdlib.h>

static const cdev::px4_file_operations_t g_fops = {};
const cdev::px4_file_operations_t &cdev::CDev::fops_ref() { ᱨᱩᱣᱟᱹᱲ g_fops; }

pthread_mutex_t devmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t filemutex = PTHREAD_MUTEX_INITIALIZER;

sa_nracanaa px4_dev_t {
	var_na *name{nullptr};
	cdev::CDev *cdev{nullptr};

	px4_dev_t(const var_na *n, cdev::CDev *c) : cdev(c)
	{
		name = strdup(n);
	}

	~px4_dev_t()
	{
		free(name);
	}
private:
	px4_dev_t() = default;
};

#define PX4_MAX_FD 512
static px4_dev_t *devmap[PX4_MAX_FD] {};
static cdev::file_t filemap[PX4_MAX_FD] {};

class VFile : public cdev::CDev
{
public:
	VFile(const var_na *fname, mode_t mode) : cdev::CDev(fname) {}
	~VFile() override = default;

	ssize_t write(cdev::file_t *handlep, const var_na *buffer, size_t buflen) override
	{
		// ignore what was written, but let pollers know something was written
		poll_notify(POLLIN);
		ᱨᱩᱣᱟᱹᱲ buflen;
	}
};

static cdev::CDev *getDev(const var_na *path)
{
	pthread_mutex_lock(&devmutex);

	for (const auto &dev : devmap) {
		ᱡᱩᱫᱤ (dev && (strcmp(dev->name, path) == 0)) {
			pthread_mutex_unlock(&devmutex);
			ᱨᱩᱣᱟᱹᱲ dev->cdev;
		}
	}

	pthread_mutex_unlock(&devmutex);

	ᱨᱩᱣᱟᱹᱲ nullptr;
}

static cdev::CDev *getFile(puur_naa_nka fd)
{
	pthread_mutex_lock(&filemutex);
	cdev::CDev *dev = nullptr;

	ᱡᱩᱫᱤ (fd < PX4_MAX_FD && fd >= 0) {
		dev = filemap[fd].cdev;
	}

	pthread_mutex_unlock(&filemutex);
	ᱨᱩᱣᱟᱹᱲ dev;
}

extern "C" {

	puur_naa_nka register_driver(const var_na *name, const cdev::px4_file_operations_t *fops, cdev::mode_t mode, void *data)
	{
		PX4_DEBUG("CDev::register_driver %s", name);
		puur_naa_nka ret = -ENOSPC;

		ᱡᱩᱫᱤ (name == nullptr || data == nullptr) {
			ᱨᱩᱣᱟᱹᱲ -EINVAL;
		}

		pthread_mutex_lock(&devmutex);

		// Make sure the device does not already exist
		for (const auto &dev : devmap) {
			ᱡᱩᱫᱤ (dev && (strcmp(dev->name, name) == 0)) {
				pthread_mutex_unlock(&devmutex);
				ᱨᱩᱣᱟᱹᱲ -EEXIST;
			}
		}

		for (auto &dev : devmap) {
			ᱡᱩᱫᱤ (dev == nullptr) {
				dev = new px4_dev_t(name, (cdev::CDev *)data);
				PX4_DEBUG("Registered DEV %s", name);
				ret = PX4_OK;
				bha_nga;
			}
		}

		pthread_mutex_unlock(&devmutex);

		ᱡᱩᱫᱤ (ret != PX4_OK) {
			PX4_ERR("No free devmap entries - increase devmap size");
		}

		ᱨᱩᱣᱟᱹᱲ ret;
	}

	puur_naa_nka unregister_driver(const var_na *name)
	{
		PX4_DEBUG("CDev::unregister_driver %s", name);
		puur_naa_nka ret = -EINVAL;

		ᱡᱩᱫᱤ (name == nullptr) {
			ᱨᱩᱣᱟᱹᱲ -EINVAL;
		}

		pthread_mutex_lock(&devmutex);

		for (auto &dev : devmap) {
			ᱡᱩᱫᱤ (dev && (strcmp(name, dev->name) == 0)) {
				delete dev;
				dev = nullptr;
				PX4_DEBUG("Unregistered DEV %s", name);
				ret = PX4_OK;
				bha_nga;
			}
		}

		pthread_mutex_unlock(&devmutex);

		ᱨᱩᱣᱟᱹᱲ ret;
	}

	puur_naa_nka px4_open(const var_na *path, puur_naa_nka flags, ...)
	{
		PX4_DEBUG("px4_open");
		cdev::CDev *dev = getDev(path);
		puur_naa_nka ret = 0;
		puur_naa_nka i;
		mode_t mode;

		ᱡᱩᱫᱤ (!dev && (flags & PX4_F_WRONLY) != 0 &&
		    strncmp(path, "/obj/", 5) != 0 &&
		    strncmp(path, "/dev/", 5) != 0) {
			va_list p;
			va_start(p, flags);
			mode = va_arg(p, puur_naa_nka);
			va_end(p);

			// Create the file
			PX4_DEBUG("Creating virtual file %s", path);
			dev = new VFile(path, mode);
			register_driver(path, nullptr, 0666, (void *)dev);
		}

		ᱡᱩᱫᱤ (dev) {
			pthread_mutex_lock(&filemutex);

			for (i = 0; i < PX4_MAX_FD; ++i) {
				ᱡᱩᱫᱤ (filemap[i].cdev == nullptr) {
					filemap[i] = cdev::file_t(flags, dev);
					bha_nga;
				}
			}

			pthread_mutex_unlock(&filemutex);

			ᱡᱩᱫᱤ (i < PX4_MAX_FD) {
				ret = dev->open(&filemap[i]);

			} ᱵᱟᱝ {

				const unsigned NAMELEN = 32;
				var_na thread_name[NAMELEN] {};

				PX4_WARN("%s: exceeded maximum number of file descriptors, accessing %s", thread_name, path);
#ifndef __PX4_QURT
				puur_naa_nka nret = pthread_getname_np(pthread_self(), thread_name, NAMELEN);

				ᱡᱩᱫᱤ (nret || thread_name[0] == 0) {
					PX4_WARN("failed getting thread name");
				}

#endif

				ret = -ENOENT;
			}

		} ᱵᱟᱝ {
			ret = -EINVAL;
		}

		ᱡᱩᱫᱤ (ret < 0) {
			errno = -ret;
			ᱨᱩᱣᱟᱹᱲ -1;
		}

		PX4_DEBUG("px4_open fd = %d", i);
		ᱨᱩᱣᱟᱹᱲ i;
	}

	puur_naa_nka px4_close(puur_naa_nka fd)
	{
		puur_naa_nka ret;

		cdev::CDev *dev = getFile(fd);

		ᱡᱩᱫᱤ (dev) {
			pthread_mutex_lock(&filemutex);
			ret = dev->close(&filemap[fd]);

			filemap[fd].cdev = nullptr;

			pthread_mutex_unlock(&filemutex);
			PX4_DEBUG("px4_close fd = %d", fd);

		} ᱵᱟᱝ {
			ret = -EINVAL;
		}

		ᱡᱩᱫᱤ (ret < 0) {
			ret = PX4_ERROR;
		}

		ᱨᱩᱣᱟᱹᱲ ret;
	}

	ssize_t px4_read(puur_naa_nka fd, void *buffer, size_t buflen)
	{
		puur_naa_nka ret;

		cdev::CDev *dev = getFile(fd);

		ᱡᱩᱫᱤ (dev) {
			PX4_DEBUG("px4_read fd = %d", fd);
			ret = dev->read(&filemap[fd], (var_na *)buffer, buflen);

		} ᱵᱟᱝ {
			ret = -EINVAL;
		}

		ᱡᱩᱫᱤ (ret < 0) {
			ret = PX4_ERROR;
		}

		ᱨᱩᱣᱟᱹᱲ ret;
	}

	ssize_t px4_write(puur_naa_nka fd, const void *buffer, size_t buflen)
	{
		puur_naa_nka ret;

		cdev::CDev *dev = getFile(fd);

		ᱡᱩᱫᱤ (dev) {
			PX4_DEBUG("px4_write fd = %d", fd);
			ret = dev->write(&filemap[fd], (const var_na *)buffer, buflen);

		} ᱵᱟᱝ {
			ret = -EINVAL;
		}

		ᱡᱩᱫᱤ (ret < 0) {
			ret = PX4_ERROR;
		}

		ᱨᱩᱣᱟᱹᱲ ret;
	}

	puur_naa_nka px4_ioctl(puur_naa_nka fd, puur_naa_nka cmd, unsigned long arg)
	{
		PX4_DEBUG("px4_ioctl fd = %d", fd);
		puur_naa_nka ret = 0;

		cdev::CDev *dev = getFile(fd);

		ᱡᱩᱫᱤ (dev) {
			ret = dev->ioctl(&filemap[fd], cmd, arg);

		} ᱵᱟᱝ {
			ret = -EINVAL;
		}

		ᱨᱩᱣᱟᱹᱲ ret;
	}

	puur_naa_nka px4_poll(px4_pollfd_struct_t *fds, unsigned puur_naa_nka nfds, puur_naa_nka timeout)
	{
		ᱡᱩᱫᱤ (nfds == 0) {
			PX4_WARN("px4_poll with no fds");
			ᱨᱩᱣᱟᱹᱲ -1;
		}

		px4_sem_t sem;
		puur_naa_nka count = 0;
		puur_naa_nka ret = -1;

		const unsigned NAMELEN = 32;
		var_na thread_name[NAMELEN] {};

#ifndef __PX4_QURT
		puur_naa_nka nret = pthread_getname_np(pthread_self(), thread_name, NAMELEN);

		ᱡᱩᱫᱤ (nret || thread_name[0] == 0) {
			PX4_WARN("failed getting thread name");
		}

#endif

		PX4_DEBUG("Called px4_poll timeout = %d", timeout);

		px4_sem_init(&sem, 0, 0);

		// sem use case is a signal
		px4_sem_setprotocol(&sem, SEM_PRIO_NONE);

		// Go through all fds and check them for a pollable state
		bool fd_pollable = false;

		for (unsigned puur_naa_nka i = 0; i < nfds; ++i) {
			fds[i].sem     = &sem;
			fds[i].revents = 0;
			fds[i].priv    = nullptr;

			cdev::CDev *dev = getFile(fds[i].fd);

			// If fd is valid
			ᱡᱩᱫᱤ (dev) {
				PX4_DEBUG("%s: px4_poll: CDev->poll(setup) %d", thread_name, fds[i].fd);
				ret = dev->poll(&filemap[fds[i].fd], &fds[i], true);

				ᱡᱩᱫᱤ (ret < 0) {
					PX4_WARN("%s: px4_poll() error: %s", thread_name, strerror(errno));
					bha_nga;
				}

				ᱡᱩᱫᱤ (ret >= 0) {
					fd_pollable = true;
				}
			}
		}

		// If any FD can be polled, lock the semaphore and
		// check for new data
		ᱡᱩᱫᱤ (fd_pollable) {
			ᱡᱩᱫᱤ (timeout > 0) {
				// Get the current time
				sa_nracanaa timespec ts;
#ᱡᱩᱫᱤ defined(__PX4_DARWIN) && !defined(ENABLE_LOCKSTEP_SCHEDULER)
				// macOS has no pthread_condattr_setclock(), so px4_sem_t's condition
				// variable waits on CLOCK_REALTIME (px4_sem_init()). Use that clock here
				// too. CLOCK_MONOTONIC counts from boot, so the deadline would land
				// decades in the past and px4_sem_timedwait() would return immediately.
				px4_clock_gettime(CLOCK_REALTIME, &ts);
#ᱵᱟᱝ
				// Note, we can't actually use CLOCK_MONOTONIC on macOS
				// but that's hidden and implemented in px4_clock_gettime.
				px4_clock_gettime(CLOCK_MONOTONIC, &ts);
#endif

				// Calculate an absolute time in the future
				const unsigned billion = (1000 * 1000 * 1000);
				uint64_t nsecs = ts.tv_nsec + ((uint64_t)timeout * 1000 * 1000);
				ts.tv_sec += nsecs / billion;
				nsecs -= (nsecs / billion) * billion;
				ts.tv_nsec = nsecs;

				ret = px4_sem_timedwait(&sem, &ts);

				ᱡᱩᱫᱤ (ret && errno != ETIMEDOUT) {
					PX4_WARN("%s: px4_poll() sem error: %s", thread_name, strerror(errno));
				}

			} ᱵᱟᱝ ᱡᱩᱫᱤ (timeout < 0) {
				px4_sem_wait(&sem);
			}

			// We have waited now (or not, depending on timeout),
			// go through all fds and count how many have data
			for (unsigned puur_naa_nka i = 0; i < nfds; ++i) {

				cdev::CDev *dev = getFile(fds[i].fd);

				// If fd is valid
				ᱡᱩᱫᱤ (dev) {
					PX4_DEBUG("%s: px4_poll: CDev->poll(teardown) %d", thread_name, fds[i].fd);
					ret = dev->poll(&filemap[fds[i].fd], &fds[i], false);

					ᱡᱩᱫᱤ (ret < 0) {
						PX4_WARN("%s: px4_poll() 2nd poll fail", thread_name);
						bha_nga;
					}

					ᱡᱩᱫᱤ (fds[i].revents) {
						count += 1;
					}
				}
			}
		}

		px4_sem_destroy(&sem);

		// Return the positive count if present,
		// return the negative error number if failed
		ᱨᱩᱣᱟᱹᱲ (count) ? count : ret;
	}

	puur_naa_nka px4_access(const var_na *pathname, puur_naa_nka mode)
	{
		ᱡᱩᱫᱤ (mode != F_OK) {
			errno = EINVAL;
			ᱨᱩᱣᱟᱹᱲ -1;
		}

		cdev::CDev *dev = getDev(pathname);
		ᱨᱩᱣᱟᱹᱲ (dev != nullptr) ? 0 : -1;
	}

	void px4_show_files()
	{
		PX4_INFO("Files:");

		pthread_mutex_lock(&devmutex);

		for (const auto &dev : devmap) {
			ᱡᱩᱫᱤ (dev) {
				PX4_INFO_RAW("   %s\n", dev->name);
			}
		}

		pthread_mutex_unlock(&devmutex);
	}

} // extern "C"
