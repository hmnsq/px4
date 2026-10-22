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

#ਅਨ੍ਤਰ੍ਭਾਵ "cdev_platform.hpp"

#ਅਨ੍ਤਰ੍ਭਾਵ "../CDev.hpp"

#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/log.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/posix.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <px4_platform_common/time.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>

ਸਥਿਰ ਅਚਲ cdev::px4_file_operations_t g_fops = {};
ਅਚਲ cdev::px4_file_operations_t &cdev::CDev::fops_ref() { ਵਾਪਸ g_fops; }

pthread_mutex_t devmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t filemutex = PTHREAD_MUTEX_INITIALIZER;

ਬਣਤਰ px4_dev_t {
	ਅੱਖਰ *name{ਖਾਲੀ_ਸੂਚਕ};
	cdev::CDev *cdev{ਖਾਲੀ_ਸੂਚਕ};

	px4_dev_t(ਅਚਲ ਅੱਖਰ *n, cdev::CDev *c) : cdev(c)
	{
		name = strdup(n);
	}

	~px4_dev_t()
	{
		ਮੁਕਤ(name);
	}
ਨਿਜੀ:
	px4_dev_t() = ਮੂਲ;
};

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ PX4_MAX_FD 512
ਸਥਿਰ px4_dev_t *devmap[PX4_MAX_FD] {};
ਸਥਿਰ cdev::file_t filemap[PX4_MAX_FD] {};

ਵਰਗ VFile : ਜਨਤਕ cdev::CDev
{
ਜਨਤਕ:
	VFile(ਅਚਲ ਅੱਖਰ *fname, mode_t ਬਹੁਲਕ) : cdev::CDev(fname) {}
	~VFile() override = ਮੂਲ;

	ssize_t ਲਿਖ(cdev::file_t *handlep, ਅਚਲ ਅੱਖਰ *ਬਫਰ_2, size_t buflen) override
	{
		// ignore what was written, but let pollers know something was written
		poll_notify(POLLIN);
		ਵਾਪਸ buflen;
	}
};

ਸਥਿਰ cdev::CDev *getDev(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ)
{
	pthread_mutex_lock(&devmutex);

	ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &dev : devmap) {
		ਜੇ (dev && (strcmp(dev->name, ਰਸਤਾ) == 0)) {
			pthread_mutex_unlock(&devmutex);
			ਵਾਪਸ dev->cdev;
		}
	}

	pthread_mutex_unlock(&devmutex);

	ਵਾਪਸ ਖਾਲੀ_ਸੂਚਕ;
}

ਸਥਿਰ cdev::CDev *getFile(ਪੂਰਨ fd)
{
	pthread_mutex_lock(&filemutex);
	cdev::CDev *dev = ਖਾਲੀ_ਸੂਚਕ;

	ਜੇ (fd < PX4_MAX_FD && fd >= 0) {
		dev = filemap[fd].cdev;
	}

	pthread_mutex_unlock(&filemutex);
	ਵਾਪਸ dev;
}

ਬਾਹਰੀ "C" {

	ਪੂਰਨ ਰਜਿਸਟਰ_driver(ਅਚਲ ਅੱਖਰ *name, ਅਚਲ cdev::px4_file_operations_t *fops, cdev::mode_t ਬਹੁਲਕ, ਖਾਲੀ *ਅੰਕੜੇ)
	{
		PX4_DEBUG("CDev::register_driver %s", name);
		ਪੂਰਨ ret = -ENOSPC;

		ਜੇ (name == ਖਾਲੀ_ਸੂਚਕ || ਅੰਕੜੇ == ਖਾਲੀ_ਸੂਚਕ) {
			ਵਾਪਸ -EINVAL;
		}

		pthread_mutex_lock(&devmutex);

		// Make sure the device does not already exist
		ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &dev : devmap) {
			ਜੇ (dev && (strcmp(dev->name, name) == 0)) {
				pthread_mutex_unlock(&devmutex);
				ਵਾਪਸ -EEXIST;
			}
		}

		ਲਈ (ਸਵੈਚਾਲਿਤ &dev : devmap) {
			ਜੇ (dev == ਖਾਲੀ_ਸੂਚਕ) {
				dev = ਨਵਾਂ px4_dev_t(name, (cdev::CDev *)ਅੰਕੜੇ);
				PX4_DEBUG("Registered DEV %s", name);
				ret = PX4_OK;
				ਰੋਕੋ;
			}
		}

		pthread_mutex_unlock(&devmutex);

		ਜੇ (ret != PX4_OK) {
			PX4_ERR("No free devmap entries - increase devmap size");
		}

		ਵਾਪਸ ret;
	}

	ਪੂਰਨ unregister_driver(ਅਚਲ ਅੱਖਰ *name)
	{
		PX4_DEBUG("CDev::unregister_driver %s", name);
		ਪੂਰਨ ret = -EINVAL;

		ਜੇ (name == ਖਾਲੀ_ਸੂਚਕ) {
			ਵਾਪਸ -EINVAL;
		}

		pthread_mutex_lock(&devmutex);

		ਲਈ (ਸਵੈਚਾਲਿਤ &dev : devmap) {
			ਜੇ (dev && (strcmp(name, dev->name) == 0)) {
				ਮਿਟਾਓ dev;
				dev = ਖਾਲੀ_ਸੂਚਕ;
				PX4_DEBUG("Unregistered DEV %s", name);
				ret = PX4_OK;
				ਰੋਕੋ;
			}
		}

		pthread_mutex_unlock(&devmutex);

		ਵਾਪਸ ret;
	}

	ਪੂਰਨ px4_open(ਅਚਲ ਅੱਖਰ *ਰਸਤਾ, ਪੂਰਨ flags, ...)
	{
		PX4_DEBUG("px4_open");
		cdev::CDev *dev = getDev(ਰਸਤਾ);
		ਪੂਰਨ ret = 0;
		ਪੂਰਨ i;
		mode_t ਬਹੁਲਕ;

		ਜੇ (!dev && (flags & PX4_F_WRONLY) != 0 &&
		    ਲੜੀ_ਤੁਲਨਾ(ਰਸਤਾ, "/obj/", 5) != 0 &&
		    ਲੜੀ_ਤੁਲਨਾ(ਰਸਤਾ, "/dev/", 5) != 0) {
			va_list p;
			va_ਸ਼ੁਰੂ(p, flags);
			ਬਹੁਲਕ = va_arg(p, ਪੂਰਨ);
			va_ਸਿਰਾ(p);

			// Create the file
			PX4_DEBUG("Creating virtual file %s", ਰਸਤਾ);
			dev = ਨਵਾਂ VFile(ਰਸਤਾ, ਬਹੁਲਕ);
			ਰਜਿਸਟਰ_driver(ਰਸਤਾ, ਖਾਲੀ_ਸੂਚਕ, 0666, (ਖਾਲੀ *)dev);
		}

		ਜੇ (dev) {
			pthread_mutex_lock(&filemutex);

			ਲਈ (i = 0; i < PX4_MAX_FD; ++i) {
				ਜੇ (filemap[i].cdev == ਖਾਲੀ_ਸੂਚਕ) {
					filemap[i] = cdev::file_t(flags, dev);
					ਰੋਕੋ;
				}
			}

			pthread_mutex_unlock(&filemutex);

			ਜੇ (i < PX4_MAX_FD) {
				ret = dev->open(&filemap[i]);

			} ਨਹੀਂਤਾਂ {

				ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ NAMELEN = 32;
				ਅੱਖਰ thread_name[NAMELEN] {};

				PX4_WARN("%s: exceeded maximum number of file descriptors, accessing %s", thread_name, ਰਸਤਾ);
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __PX4_QURT
				ਪੂਰਨ nret = pthread_getname_np(pthread_self(), thread_name, NAMELEN);

				ਜੇ (nret || thread_name[0] == 0) {
					PX4_WARN("failed getting thread name");
				}

#ਜੇ_ਅੰਤ

				ret = -ENOENT;
			}

		} ਨਹੀਂਤਾਂ {
			ret = -EINVAL;
		}

		ਜੇ (ret < 0) {
			errno = -ret;
			ਵਾਪਸ -1;
		}

		PX4_DEBUG("px4_open fd = %d", i);
		ਵਾਪਸ i;
	}

	ਪੂਰਨ px4_close(ਪੂਰਨ fd)
	{
		ਪੂਰਨ ret;

		cdev::CDev *dev = getFile(fd);

		ਜੇ (dev) {
			pthread_mutex_lock(&filemutex);
			ret = dev->close(&filemap[fd]);

			filemap[fd].cdev = ਖਾਲੀ_ਸੂਚਕ;

			pthread_mutex_unlock(&filemutex);
			PX4_DEBUG("px4_close fd = %d", fd);

		} ਨਹੀਂਤਾਂ {
			ret = -EINVAL;
		}

		ਜੇ (ret < 0) {
			ret = PX4_ERROR;
		}

		ਵਾਪਸ ret;
	}

	ssize_t px4_read(ਪੂਰਨ fd, ਖਾਲੀ *ਬਫਰ_2, size_t buflen)
	{
		ਪੂਰਨ ret;

		cdev::CDev *dev = getFile(fd);

		ਜੇ (dev) {
			PX4_DEBUG("px4_read fd = %d", fd);
			ret = dev->ਪਠ(&filemap[fd], (ਅੱਖਰ *)ਬਫਰ_2, buflen);

		} ਨਹੀਂਤਾਂ {
			ret = -EINVAL;
		}

		ਜੇ (ret < 0) {
			ret = PX4_ERROR;
		}

		ਵਾਪਸ ret;
	}

	ssize_t px4_write(ਪੂਰਨ fd, ਅਚਲ ਖਾਲੀ *ਬਫਰ_2, size_t buflen)
	{
		ਪੂਰਨ ret;

		cdev::CDev *dev = getFile(fd);

		ਜੇ (dev) {
			PX4_DEBUG("px4_write fd = %d", fd);
			ret = dev->ਲਿਖ(&filemap[fd], (ਅਚਲ ਅੱਖਰ *)ਬਫਰ_2, buflen);

		} ਨਹੀਂਤਾਂ {
			ret = -EINVAL;
		}

		ਜੇ (ret < 0) {
			ret = PX4_ERROR;
		}

		ਵਾਪਸ ret;
	}

	ਪੂਰਨ px4_ioctl(ਪੂਰਨ fd, ਪੂਰਨ cmd, ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ arg)
	{
		PX4_DEBUG("px4_ioctl fd = %d", fd);
		ਪੂਰਨ ret = 0;

		cdev::CDev *dev = getFile(fd);

		ਜੇ (dev) {
			ret = dev->ioctl(&filemap[fd], cmd, arg);

		} ਨਹੀਂਤਾਂ {
			ret = -EINVAL;
		}

		ਵਾਪਸ ret;
	}

	ਪੂਰਨ px4_poll(px4_pollfd_struct_t *fds, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ nfds, ਪੂਰਨ timeout)
	{
		ਜੇ (nfds == 0) {
			PX4_WARN("px4_poll with no fds");
			ਵਾਪਸ -1;
		}

		px4_sem_t sem;
		ਪੂਰਨ ਗਿਣਤੀ_2 = 0;
		ਪੂਰਨ ret = -1;

		ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ NAMELEN = 32;
		ਅੱਖਰ thread_name[NAMELEN] {};

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __PX4_QURT
		ਪੂਰਨ nret = pthread_getname_np(pthread_self(), thread_name, NAMELEN);

		ਜੇ (nret || thread_name[0] == 0) {
			PX4_WARN("failed getting thread name");
		}

#ਜੇ_ਅੰਤ

		PX4_DEBUG("Called px4_poll timeout = %d", timeout);

		px4_sem_init(&sem, 0, 0);

		// sem use case is a signal
		px4_sem_setprotocol(&sem, SEM_PRIO_NONE);

		// Go through all fds and check them for a pollable state
		ਤਾਰਕਿਕ fd_pollable = ਝੂਠ;

		ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ i = 0; i < nfds; ++i) {
			fds[i].sem     = &sem;
			fds[i].revents = 0;
			fds[i].priv    = ਖਾਲੀ_ਸੂਚਕ;

			cdev::CDev *dev = getFile(fds[i].fd);

			// If fd is valid
			ਜੇ (dev) {
				PX4_DEBUG("%s: px4_poll: CDev->poll(setup) %d", thread_name, fds[i].fd);
				ret = dev->poll(&filemap[fds[i].fd], &fds[i], ਸੱਚ);

				ਜੇ (ret < 0) {
					PX4_WARN("%s: px4_poll() error: %s", thread_name, strerror(errno));
					ਰੋਕੋ;
				}

				ਜੇ (ret >= 0) {
					fd_pollable = ਸੱਚ;
				}
			}
		}

		// If any FD can be polled, lock the semaphore and
		// check for new data
		ਜੇ (fd_pollable) {
			ਜੇ (timeout > 0) {
				// Get the current time
				ਬਣਤਰ timespec ts;
#ਜੇ defined(__PX4_DARWIN) && !defined(ENABLE_LOCKSTEP_SCHEDULER)
				// macOS has no pthread_condattr_setclock(), so px4_sem_t's condition
				// variable waits on CLOCK_REALTIME (px4_sem_init()). Use that clock here
				// too. CLOCK_MONOTONIC counts from boot, so the deadline would land
				// decades in the past and px4_sem_timedwait() would return immediately.
				px4_clock_gettime(CLOCK_REALTIME, &ts);
#ਨਹੀਂਤਾਂ
				// Note, we can't actually use CLOCK_MONOTONIC on macOS
				// but that's hidden and implemented in px4_clock_gettime.
				px4_clock_gettime(CLOCK_MONOTONIC, &ts);
#ਜੇ_ਅੰਤ

				// Calculate an absolute time in the future
				ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ billion = (1000 * 1000 * 1000);
				uint64_t nsecs = ts.tv_nsec + ((uint64_t)timeout * 1000 * 1000);
				ts.tv_sec += nsecs / billion;
				nsecs -= (nsecs / billion) * billion;
				ts.tv_nsec = nsecs;

				ret = px4_sem_timedwait(&sem, &ts);

				ਜੇ (ret && errno != ETIMEDOUT) {
					PX4_WARN("%s: px4_poll() sem error: %s", thread_name, strerror(errno));
				}

			} ਨਹੀਂਤਾਂ ਜੇ (timeout < 0) {
				px4_sem_wait(&sem);
			}

			// We have waited now (or not, depending on timeout),
			// go through all fds and count how many have data
			ਲਈ (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ i = 0; i < nfds; ++i) {

				cdev::CDev *dev = getFile(fds[i].fd);

				// If fd is valid
				ਜੇ (dev) {
					PX4_DEBUG("%s: px4_poll: CDev->poll(teardown) %d", thread_name, fds[i].fd);
					ret = dev->poll(&filemap[fds[i].fd], &fds[i], ਝੂਠ);

					ਜੇ (ret < 0) {
						PX4_WARN("%s: px4_poll() 2nd poll fail", thread_name);
						ਰੋਕੋ;
					}

					ਜੇ (fds[i].revents) {
						ਗਿਣਤੀ_2 += 1;
					}
				}
			}
		}

		px4_sem_destroy(&sem);

		// Return the positive count if present,
		// return the negative error number if failed
		ਵਾਪਸ (ਗਿਣਤੀ_2) ? ਗਿਣਤੀ_2 : ret;
	}

	ਪੂਰਨ px4_access(ਅਚਲ ਅੱਖਰ *pathname, ਪੂਰਨ ਬਹੁਲਕ)
	{
		ਜੇ (ਬਹੁਲਕ != F_OK) {
			errno = EINVAL;
			ਵਾਪਸ -1;
		}

		cdev::CDev *dev = getDev(pathname);
		ਵਾਪਸ (dev != ਖਾਲੀ_ਸੂਚਕ) ? 0 : -1;
	}

	ਖਾਲੀ px4_show_files()
	{
		PX4_INFO("Files:");

		pthread_mutex_lock(&devmutex);

		ਲਈ (ਅਚਲ ਸਵੈਚਾਲਿਤ &dev : devmap) {
			ਜੇ (dev) {
				PX4_INFO_RAW("   %s\n", dev->name);
			}
		}

		pthread_mutex_unlock(&devmutex);
	}

} // extern "C"
