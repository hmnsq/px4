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
const cdev::px4_file_operations_t &cdev::CDev::fops_ref() { 返回 g_fops; }

pthread_mutex_t devmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t filemutex = PTHREAD_MUTEX_INITIALIZER;

struct px4_dev_t {
	char *name{nullptr};
	cdev::CDev *cdev{nullptr};

	px4_dev_t(const char *n, cdev::CDev *c) : cdev(c)
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
	VFile(const char *fname, mode_t mode) : cdev::CDev(fname) {}
	~VFile() override = default;

	ssize_t write(cdev::file_t *handlep, const char *buffer, size_t buflen) override
	{
		// ignore what was written, but let pollers know something was written
		poll_notify(POLLIN);
		返回 buflen;
	}
};

static cdev::CDev *getDev(const char *path)
{
	pthread_mutex_lock(&devmutex);

	为 (const auto &dev : devmap) {
		如果 (dev && (strcmp(dev->name, path) == 0)) {
			pthread_mutex_unlock(&devmutex);
			返回 dev->cdev;
		}
	}

	pthread_mutex_unlock(&devmutex);

	返回 nullptr;
}

static cdev::CDev *getFile(整数 fd)
{
	pthread_mutex_lock(&filemutex);
	cdev::CDev *dev = nullptr;

	如果 (fd < PX4_MAX_FD && fd >= 0) {
		dev = filemap[fd].cdev;
	}

	pthread_mutex_unlock(&filemutex);
	返回 dev;
}

extern "C" {

	整数 register_driver(const char *name, const cdev::px4_file_operations_t *fops, cdev::mode_t mode, void *data)
	{
		PX4_DEBUG("CDev::register_driver %s", name);
		整数 ret = -ENOSPC;

		如果 (name == nullptr || data == nullptr) {
			返回 -EINVAL;
		}

		pthread_mutex_lock(&devmutex);

		// Make sure the device does not already exist
		为 (const auto &dev : devmap) {
			如果 (dev && (strcmp(dev->name, name) == 0)) {
				pthread_mutex_unlock(&devmutex);
				返回 -EEXIST;
			}
		}

		为 (auto &dev : devmap) {
			如果 (dev == nullptr) {
				dev = new px4_dev_t(name, (cdev::CDev *)data);
				PX4_DEBUG("Registered DEV %s", name);
				ret = PX4_OK;
				break;
			}
		}

		pthread_mutex_unlock(&devmutex);

		如果 (ret != PX4_OK) {
			PX4_ERR("No free devmap entries - increase devmap size");
		}

		返回 ret;
	}

	整数 unregister_driver(const char *name)
	{
		PX4_DEBUG("CDev::unregister_driver %s", name);
		整数 ret = -EINVAL;

		如果 (name == nullptr) {
			返回 -EINVAL;
		}

		pthread_mutex_lock(&devmutex);

		为 (auto &dev : devmap) {
			如果 (dev && (strcmp(name, dev->name) == 0)) {
				delete dev;
				dev = nullptr;
				PX4_DEBUG("Unregistered DEV %s", name);
				ret = PX4_OK;
				break;
			}
		}

		pthread_mutex_unlock(&devmutex);

		返回 ret;
	}

	整数 px4_open(const char *path, 整数 flags, ...)
	{
		PX4_DEBUG("px4_open");
		cdev::CDev *dev = getDev(path);
		整数 ret = 0;
		整数 i;
		mode_t mode;

		如果 (!dev && (flags & PX4_F_WRONLY) != 0 &&
		    strncmp(path, "/obj/", 5) != 0 &&
		    strncmp(path, "/dev/", 5) != 0) {
			va_list p;
			va_start(p, flags);
			mode = va_arg(p, 整数);
			va_end(p);

			// Create the file
			PX4_DEBUG("Creating virtual file %s", path);
			dev = new VFile(path, mode);
			register_driver(path, nullptr, 0666, (void *)dev);
		}

		如果 (dev) {
			pthread_mutex_lock(&filemutex);

			为 (i = 0; i < PX4_MAX_FD; ++i) {
				如果 (filemap[i].cdev == nullptr) {
					filemap[i] = cdev::file_t(flags, dev);
					break;
				}
			}

			pthread_mutex_unlock(&filemutex);

			如果 (i < PX4_MAX_FD) {
				ret = dev->open(&filemap[i]);

			} 否则 {

				const unsigned NAMELEN = 32;
				char thread_name[NAMELEN] {};

				PX4_WARN("%s: exceeded maximum number of file descriptors, accessing %s", thread_name, path);
#ifndef __PX4_QURT
				整数 nret = pthread_getname_np(pthread_self(), thread_name, NAMELEN);

				如果 (nret || thread_name[0] == 0) {
					PX4_WARN("failed getting thread name");
				}

#endif

				ret = -ENOENT;
			}

		} 否则 {
			ret = -EINVAL;
		}

		如果 (ret < 0) {
			errno = -ret;
			返回 -1;
		}

		PX4_DEBUG("px4_open fd = %d", i);
		返回 i;
	}

	整数 px4_close(整数 fd)
	{
		整数 ret;

		cdev::CDev *dev = getFile(fd);

		如果 (dev) {
			pthread_mutex_lock(&filemutex);
			ret = dev->close(&filemap[fd]);

			filemap[fd].cdev = nullptr;

			pthread_mutex_unlock(&filemutex);
			PX4_DEBUG("px4_close fd = %d", fd);

		} 否则 {
			ret = -EINVAL;
		}

		如果 (ret < 0) {
			ret = PX4_ERROR;
		}

		返回 ret;
	}

	ssize_t px4_read(整数 fd, void *buffer, size_t buflen)
	{
		整数 ret;

		cdev::CDev *dev = getFile(fd);

		如果 (dev) {
			PX4_DEBUG("px4_read fd = %d", fd);
			ret = dev->read(&filemap[fd], (char *)buffer, buflen);

		} 否则 {
			ret = -EINVAL;
		}

		如果 (ret < 0) {
			ret = PX4_ERROR;
		}

		返回 ret;
	}

	ssize_t px4_write(整数 fd, const void *buffer, size_t buflen)
	{
		整数 ret;

		cdev::CDev *dev = getFile(fd);

		如果 (dev) {
			PX4_DEBUG("px4_write fd = %d", fd);
			ret = dev->write(&filemap[fd], (const char *)buffer, buflen);

		} 否则 {
			ret = -EINVAL;
		}

		如果 (ret < 0) {
			ret = PX4_ERROR;
		}

		返回 ret;
	}

	整数 px4_ioctl(整数 fd, 整数 cmd, unsigned long arg)
	{
		PX4_DEBUG("px4_ioctl fd = %d", fd);
		整数 ret = 0;

		cdev::CDev *dev = getFile(fd);

		如果 (dev) {
			ret = dev->ioctl(&filemap[fd], cmd, arg);

		} 否则 {
			ret = -EINVAL;
		}

		返回 ret;
	}

	整数 px4_poll(px4_pollfd_struct_t *fds, unsigned 整数 nfds, 整数 timeout)
	{
		如果 (nfds == 0) {
			PX4_WARN("px4_poll with no fds");
			返回 -1;
		}

		px4_sem_t sem;
		整数 count = 0;
		整数 ret = -1;

		const unsigned NAMELEN = 32;
		char thread_name[NAMELEN] {};

#ifndef __PX4_QURT
		整数 nret = pthread_getname_np(pthread_self(), thread_name, NAMELEN);

		如果 (nret || thread_name[0] == 0) {
			PX4_WARN("failed getting thread name");
		}

#endif

		PX4_DEBUG("Called px4_poll timeout = %d", timeout);

		px4_sem_init(&sem, 0, 0);

		// sem use case is a signal
		px4_sem_setprotocol(&sem, SEM_PRIO_NONE);

		// Go through all fds and check them for a pollable state
		bool fd_pollable = false;

		为 (unsigned 整数 i = 0; i < nfds; ++i) {
			fds[i].sem     = &sem;
			fds[i].revents = 0;
			fds[i].priv    = nullptr;

			cdev::CDev *dev = getFile(fds[i].fd);

			// If fd is valid
			如果 (dev) {
				PX4_DEBUG("%s: px4_poll: CDev->poll(setup) %d", thread_name, fds[i].fd);
				ret = dev->poll(&filemap[fds[i].fd], &fds[i], true);

				如果 (ret < 0) {
					PX4_WARN("%s: px4_poll() error: %s", thread_name, strerror(errno));
					break;
				}

				如果 (ret >= 0) {
					fd_pollable = true;
				}
			}
		}

		// If any FD can be polled, lock the semaphore and
		// check for new data
		如果 (fd_pollable) {
			如果 (timeout > 0) {
				// Get the current time
				struct timespec ts;
#如果 defined(__PX4_DARWIN) && !defined(ENABLE_LOCKSTEP_SCHEDULER)
				// macOS has no pthread_condattr_setclock(), so px4_sem_t's condition
				// variable waits on CLOCK_REALTIME (px4_sem_init()). Use that clock here
				// too. CLOCK_MONOTONIC counts from boot, so the deadline would land
				// decades in the past and px4_sem_timedwait() would return immediately.
				px4_clock_gettime(CLOCK_REALTIME, &ts);
#否则
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

				如果 (ret && errno != ETIMEDOUT) {
					PX4_WARN("%s: px4_poll() sem error: %s", thread_name, strerror(errno));
				}

			} 否则 如果 (timeout < 0) {
				px4_sem_wait(&sem);
			}

			// We have waited now (or not, depending on timeout),
			// go through all fds and count how many have data
			为 (unsigned 整数 i = 0; i < nfds; ++i) {

				cdev::CDev *dev = getFile(fds[i].fd);

				// If fd is valid
				如果 (dev) {
					PX4_DEBUG("%s: px4_poll: CDev->poll(teardown) %d", thread_name, fds[i].fd);
					ret = dev->poll(&filemap[fds[i].fd], &fds[i], false);

					如果 (ret < 0) {
						PX4_WARN("%s: px4_poll() 2nd poll fail", thread_name);
						break;
					}

					如果 (fds[i].revents) {
						count += 1;
					}
				}
			}
		}

		px4_sem_destroy(&sem);

		// Return the positive count if present,
		// return the negative error number if failed
		返回 (count) ? count : ret;
	}

	整数 px4_access(const char *pathname, 整数 mode)
	{
		如果 (mode != F_OK) {
			errno = EINVAL;
			返回 -1;
		}

		cdev::CDev *dev = getDev(pathname);
		返回 (dev != nullptr) ? 0 : -1;
	}

	void px4_show_files()
	{
		PX4_INFO("Files:");

		pthread_mutex_lock(&devmutex);

		为 (const auto &dev : devmap) {
			如果 (dev) {
				PX4_INFO_RAW("   %s\n", dev->name);
			}
		}

		pthread_mutex_unlock(&devmutex);
	}

} // extern "C"
