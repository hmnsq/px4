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

#અન્તર્ભાવ "cdev_platform.hpp"

#અન્તર્ભાવ "../CDev.hpp"

#અન્તર્ભાવ <px4_platform_common/log.h>
#અન્તર્ભાવ <px4_platform_common/posix.h>
#અન્તર્ભાવ <px4_platform_common/time.h>

#અન્તર્ભાવ <stdlib.h>

સ્થિર અચલ cdev::px4_file_operations_t g_fops = {};
અચલ cdev::px4_file_operations_t &cdev::CDev::fops_ref() { પાછા g_fops; }

pthread_mutex_t devmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t filemutex = PTHREAD_MUTEX_INITIALIZER;

સંરચના px4_dev_t {
	અક્ષર *name{nullptr};
	cdev::CDev *cdev{nullptr};

	px4_dev_t(અચલ અક્ષર *n, cdev::CDev *c) : cdev(c)
	{
		name = strdup(n);
	}

	~px4_dev_t()
	{
		મુક્ત(name);
	}
ખાનગી:
	px4_dev_t() = મૂળભૂત;
};

#define PX4_MAX_FD 512
સ્થિર px4_dev_t *devmap[PX4_MAX_FD] {};
સ્થિર cdev::file_t filemap[PX4_MAX_FD] {};

વર્ગ VFile : જાહેર cdev::CDev
{
જાહેર:
	VFile(અચલ અક્ષર *fname, mode_t mode) : cdev::CDev(fname) {}
	~VFile() override = મૂળભૂત;

	ssize_t લિખ(cdev::file_t *handlep, અચલ અક્ષર *buffer, size_t buflen) override
	{
		// ignore what was written, but let pollers know something was written
		poll_notify(POLLIN);
		પાછા buflen;
	}
};

સ્થિર cdev::CDev *getDev(અચલ અક્ષર *path)
{
	pthread_mutex_lock(&devmutex);

	માટે (અચલ auto &dev : devmap) {
		જો (dev && (strcmp(dev->name, path) == 0)) {
			pthread_mutex_unlock(&devmutex);
			પાછા dev->cdev;
		}
	}

	pthread_mutex_unlock(&devmutex);

	પાછા nullptr;
}

સ્થિર cdev::CDev *getFile(પૂર્ણાંક fd)
{
	pthread_mutex_lock(&filemutex);
	cdev::CDev *dev = nullptr;

	જો (fd < PX4_MAX_FD && fd >= 0) {
		dev = filemap[fd].cdev;
	}

	pthread_mutex_unlock(&filemutex);
	પાછા dev;
}

બાહ્ય "C" {

	પૂર્ણાંક register_driver(અચલ અક્ષર *name, અચલ cdev::px4_file_operations_t *fops, cdev::mode_t mode, શૂન્ય *data)
	{
		PX4_DEBUG("CDev::register_driver %s", name);
		પૂર્ણાંક ret = -ENOSPC;

		જો (name == nullptr || data == nullptr) {
			પાછા -EINVAL;
		}

		pthread_mutex_lock(&devmutex);

		// Make sure the device does not already exist
		માટે (અચલ auto &dev : devmap) {
			જો (dev && (strcmp(dev->name, name) == 0)) {
				pthread_mutex_unlock(&devmutex);
				પાછા -EEXIST;
			}
		}

		માટે (auto &dev : devmap) {
			જો (dev == nullptr) {
				dev = નવું px4_dev_t(name, (cdev::CDev *)data);
				PX4_DEBUG("Registered DEV %s", name);
				ret = PX4_OK;
				તોડો;
			}
		}

		pthread_mutex_unlock(&devmutex);

		જો (ret != PX4_OK) {
			PX4_ERR("No free devmap entries - increase devmap size");
		}

		પાછા ret;
	}

	પૂર્ણાંક unregister_driver(અચલ અક્ષર *name)
	{
		PX4_DEBUG("CDev::unregister_driver %s", name);
		પૂર્ણાંક ret = -EINVAL;

		જો (name == nullptr) {
			પાછા -EINVAL;
		}

		pthread_mutex_lock(&devmutex);

		માટે (auto &dev : devmap) {
			જો (dev && (strcmp(name, dev->name) == 0)) {
				delete dev;
				dev = nullptr;
				PX4_DEBUG("Unregistered DEV %s", name);
				ret = PX4_OK;
				તોડો;
			}
		}

		pthread_mutex_unlock(&devmutex);

		પાછા ret;
	}

	પૂર્ણાંક px4_open(અચલ અક્ષર *path, પૂર્ણાંક flags, ...)
	{
		PX4_DEBUG("px4_open");
		cdev::CDev *dev = getDev(path);
		પૂર્ણાંક ret = 0;
		પૂર્ણાંક i;
		mode_t mode;

		જો (!dev && (flags & PX4_F_WRONLY) != 0 &&
		    strncmp(path, "/obj/", 5) != 0 &&
		    strncmp(path, "/dev/", 5) != 0) {
			va_list p;
			va_start(p, flags);
			mode = va_arg(p, પૂર્ણાંક);
			va_end(p);

			// Create the file
			PX4_DEBUG("Creating virtual file %s", path);
			dev = નવું VFile(path, mode);
			register_driver(path, nullptr, 0666, (શૂન્ય *)dev);
		}

		જો (dev) {
			pthread_mutex_lock(&filemutex);

			માટે (i = 0; i < PX4_MAX_FD; ++i) {
				જો (filemap[i].cdev == nullptr) {
					filemap[i] = cdev::file_t(flags, dev);
					તોડો;
				}
			}

			pthread_mutex_unlock(&filemutex);

			જો (i < PX4_MAX_FD) {
				ret = dev->open(&filemap[i]);

			} નહીંતર {

				અચલ unsigned NAMELEN = 32;
				અક્ષર thread_name[NAMELEN] {};

				PX4_WARN("%s: exceeded maximum number of file descriptors, accessing %s", thread_name, path);
#ifndef __PX4_QURT
				પૂર્ણાંક nret = pthread_getname_np(pthread_self(), thread_name, NAMELEN);

				જો (nret || thread_name[0] == 0) {
					PX4_WARN("failed getting thread name");
				}

#endif

				ret = -ENOENT;
			}

		} નહીંતર {
			ret = -EINVAL;
		}

		જો (ret < 0) {
			errno = -ret;
			પાછા -1;
		}

		PX4_DEBUG("px4_open fd = %d", i);
		પાછા i;
	}

	પૂર્ણાંક px4_close(પૂર્ણાંક fd)
	{
		પૂર્ણાંક ret;

		cdev::CDev *dev = getFile(fd);

		જો (dev) {
			pthread_mutex_lock(&filemutex);
			ret = dev->close(&filemap[fd]);

			filemap[fd].cdev = nullptr;

			pthread_mutex_unlock(&filemutex);
			PX4_DEBUG("px4_close fd = %d", fd);

		} નહીંતર {
			ret = -EINVAL;
		}

		જો (ret < 0) {
			ret = PX4_ERROR;
		}

		પાછા ret;
	}

	ssize_t px4_read(પૂર્ણાંક fd, શૂન્ય *buffer, size_t buflen)
	{
		પૂર્ણાંક ret;

		cdev::CDev *dev = getFile(fd);

		જો (dev) {
			PX4_DEBUG("px4_read fd = %d", fd);
			ret = dev->પઠ(&filemap[fd], (અક્ષર *)buffer, buflen);

		} નહીંતર {
			ret = -EINVAL;
		}

		જો (ret < 0) {
			ret = PX4_ERROR;
		}

		પાછા ret;
	}

	ssize_t px4_write(પૂર્ણાંક fd, અચલ શૂન્ય *buffer, size_t buflen)
	{
		પૂર્ણાંક ret;

		cdev::CDev *dev = getFile(fd);

		જો (dev) {
			PX4_DEBUG("px4_write fd = %d", fd);
			ret = dev->લિખ(&filemap[fd], (અચલ અક્ષર *)buffer, buflen);

		} નહીંતર {
			ret = -EINVAL;
		}

		જો (ret < 0) {
			ret = PX4_ERROR;
		}

		પાછા ret;
	}

	પૂર્ણાંક px4_ioctl(પૂર્ણાંક fd, પૂર્ણાંક cmd, unsigned long arg)
	{
		PX4_DEBUG("px4_ioctl fd = %d", fd);
		પૂર્ણાંક ret = 0;

		cdev::CDev *dev = getFile(fd);

		જો (dev) {
			ret = dev->ioctl(&filemap[fd], cmd, arg);

		} નહીંતર {
			ret = -EINVAL;
		}

		પાછા ret;
	}

	પૂર્ણાંક px4_poll(px4_pollfd_struct_t *fds, unsigned પૂર્ણાંક nfds, પૂર્ણાંક timeout)
	{
		જો (nfds == 0) {
			PX4_WARN("px4_poll with no fds");
			પાછા -1;
		}

		px4_sem_t sem;
		પૂર્ણાંક ગણતરી = 0;
		પૂર્ણાંક ret = -1;

		અચલ unsigned NAMELEN = 32;
		અક્ષર thread_name[NAMELEN] {};

#ifndef __PX4_QURT
		પૂર્ણાંક nret = pthread_getname_np(pthread_self(), thread_name, NAMELEN);

		જો (nret || thread_name[0] == 0) {
			PX4_WARN("failed getting thread name");
		}

#endif

		PX4_DEBUG("Called px4_poll timeout = %d", timeout);

		px4_sem_init(&sem, 0, 0);

		// sem use case is a signal
		px4_sem_setprotocol(&sem, SEM_PRIO_NONE);

		// Go through all fds and check them for a pollable state
		તાર્કિક fd_pollable = ખોટું;

		માટે (unsigned પૂર્ણાંક i = 0; i < nfds; ++i) {
			fds[i].sem     = &sem;
			fds[i].revents = 0;
			fds[i].priv    = nullptr;

			cdev::CDev *dev = getFile(fds[i].fd);

			// If fd is valid
			જો (dev) {
				PX4_DEBUG("%s: px4_poll: CDev->poll(setup) %d", thread_name, fds[i].fd);
				ret = dev->poll(&filemap[fds[i].fd], &fds[i], સાચું);

				જો (ret < 0) {
					PX4_WARN("%s: px4_poll() error: %s", thread_name, strerror(errno));
					તોડો;
				}

				જો (ret >= 0) {
					fd_pollable = સાચું;
				}
			}
		}

		// If any FD can be polled, lock the semaphore and
		// check for new data
		જો (fd_pollable) {
			જો (timeout > 0) {
				// Get the current time
				સંરચના timespec ts;
#જો defined(__PX4_DARWIN) && !defined(ENABLE_LOCKSTEP_SCHEDULER)
				// macOS has no pthread_condattr_setclock(), so px4_sem_t's condition
				// variable waits on CLOCK_REALTIME (px4_sem_init()). Use that clock here
				// too. CLOCK_MONOTONIC counts from boot, so the deadline would land
				// decades in the past and px4_sem_timedwait() would return immediately.
				px4_clock_gettime(CLOCK_REALTIME, &ts);
#નહીંતર
				// Note, we can't actually use CLOCK_MONOTONIC on macOS
				// but that's hidden and implemented in px4_clock_gettime.
				px4_clock_gettime(CLOCK_MONOTONIC, &ts);
#endif

				// Calculate an absolute time in the future
				અચલ unsigned billion = (1000 * 1000 * 1000);
				uint64_t nsecs = ts.tv_nsec + ((uint64_t)timeout * 1000 * 1000);
				ts.tv_sec += nsecs / billion;
				nsecs -= (nsecs / billion) * billion;
				ts.tv_nsec = nsecs;

				ret = px4_sem_timedwait(&sem, &ts);

				જો (ret && errno != ETIMEDOUT) {
					PX4_WARN("%s: px4_poll() sem error: %s", thread_name, strerror(errno));
				}

			} નહીંતર જો (timeout < 0) {
				px4_sem_wait(&sem);
			}

			// We have waited now (or not, depending on timeout),
			// go through all fds and count how many have data
			માટે (unsigned પૂર્ણાંક i = 0; i < nfds; ++i) {

				cdev::CDev *dev = getFile(fds[i].fd);

				// If fd is valid
				જો (dev) {
					PX4_DEBUG("%s: px4_poll: CDev->poll(teardown) %d", thread_name, fds[i].fd);
					ret = dev->poll(&filemap[fds[i].fd], &fds[i], ખોટું);

					જો (ret < 0) {
						PX4_WARN("%s: px4_poll() 2nd poll fail", thread_name);
						તોડો;
					}

					જો (fds[i].revents) {
						ગણતરી += 1;
					}
				}
			}
		}

		px4_sem_destroy(&sem);

		// Return the positive count if present,
		// return the negative error number if failed
		પાછા (ગણતરી) ? ગણતરી : ret;
	}

	પૂર્ણાંક px4_access(અચલ અક્ષર *pathname, પૂર્ણાંક mode)
	{
		જો (mode != F_OK) {
			errno = EINVAL;
			પાછા -1;
		}

		cdev::CDev *dev = getDev(pathname);
		પાછા (dev != nullptr) ? 0 : -1;
	}

	શૂન્ય px4_show_files()
	{
		PX4_INFO("Files:");

		pthread_mutex_lock(&devmutex);

		માટે (અચલ auto &dev : devmap) {
			જો (dev) {
				PX4_INFO_RAW("   %s\n", dev->name);
			}
		}

		pthread_mutex_unlock(&devmutex);
	}

} // extern "C"
