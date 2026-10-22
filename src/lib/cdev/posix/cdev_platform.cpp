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

#ಅನ್ತರ್ಭಾವ "cdev_platform.hpp"

#ಅನ್ತರ್ಭಾವ "../CDev.hpp"

#ಅನ್ತರ್ಭಾವ <px4_platform_common/log.h>
#ಅನ್ತರ್ಭಾವ <px4_platform_common/posix.h>
#ಅನ್ತರ್ಭಾವ <px4_platform_common/time.h>

#ಅನ್ತರ್ಭಾವ <stdlib.h>

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ cdev::px4_file_operations_t g_fops = {};
ಸ್ಥಿರಾಂಕ cdev::px4_file_operations_t &cdev::CDev::fops_ref() { ಹಿಂತಿರುಗಿ g_fops; }

pthread_mutex_t devmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t filemutex = PTHREAD_MUTEX_INITIALIZER;

ರಚನೆ px4_dev_t {
	ಅಕ್ಷರ *name{ಶೂನ್ಯಸೂಚಕ};
	cdev::CDev *cdev{ಶೂನ್ಯಸೂಚಕ};

	px4_dev_t(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *n, cdev::CDev *c) : cdev(c)
	{
		name = strdup(n);
	}

	~px4_dev_t()
	{
		ಬಿಡುಗಡೆ(name);
	}
ಖಾಸಗಿ:
	px4_dev_t() = ಪೂರ್ವನಿಯೋಜಿತ;
};

#ವ್ಯಾಖ್ಯಾನಿಸು PX4_MAX_FD 512
ಸ್ಥಿರ px4_dev_t *devmap[PX4_MAX_FD] {};
ಸ್ಥಿರ cdev::file_t filemap[PX4_MAX_FD] {};

ವರ್ಗ VFile : ಸಾರ್ವಜನಿಕ cdev::CDev
{
ಸಾರ್ವಜನಿಕ:
	VFile(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fname, mode_t ಬಹುಲಕ) : cdev::CDev(fname) {}
	~VFile() override = ಪೂರ್ವನಿಯೋಜಿತ;

	ssize_t ಲಿಖ(cdev::file_t *handlep, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಫರ್_2, size_t buflen) override
	{
		// ignore what was written, but let pollers know something was written
		poll_notify(POLLIN);
		ಹಿಂತಿರುಗಿ buflen;
	}
};

ಸ್ಥಿರ cdev::CDev *getDev(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ)
{
	pthread_mutex_lock(&devmutex);

	ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &dev : devmap) {
		ಆದರೆ (dev && (strcmp(dev->name, ಮಾರ್ಗ) == 0)) {
			pthread_mutex_unlock(&devmutex);
			ಹಿಂತಿರುಗಿ dev->cdev;
		}
	}

	pthread_mutex_unlock(&devmutex);

	ಹಿಂತಿರುಗಿ ಶೂನ್ಯಸೂಚಕ;
}

ಸ್ಥಿರ cdev::CDev *getFile(ಪೂರ್ಣಾಂಕ fd)
{
	pthread_mutex_lock(&filemutex);
	cdev::CDev *dev = ಶೂನ್ಯಸೂಚಕ;

	ಆದರೆ (fd < PX4_MAX_FD && fd >= 0) {
		dev = filemap[fd].cdev;
	}

	pthread_mutex_unlock(&filemutex);
	ಹಿಂತಿರುಗಿ dev;
}

ಬಾಹ್ಯ "C" {

	ಪೂರ್ಣಾಂಕ ರಿಜಿಸ್ಟರ್_driver(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name, ಸ್ಥಿರಾಂಕ cdev::px4_file_operations_t *fops, cdev::mode_t ಬಹುಲಕ, ಶೂನ್ಯ *ದತ್ತಾಂಶ)
	{
		PX4_DEBUG("CDev::register_driver %s", name);
		ಪೂರ್ಣಾಂಕ ret = -ENOSPC;

		ಆದರೆ (name == ಶೂನ್ಯಸೂಚಕ || ದತ್ತಾಂಶ == ಶೂನ್ಯಸೂಚಕ) {
			ಹಿಂತಿರುಗಿ -EINVAL;
		}

		pthread_mutex_lock(&devmutex);

		// Make sure the device does not already exist
		ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &dev : devmap) {
			ಆದರೆ (dev && (strcmp(dev->name, name) == 0)) {
				pthread_mutex_unlock(&devmutex);
				ಹಿಂತಿರುಗಿ -EEXIST;
			}
		}

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &dev : devmap) {
			ಆದರೆ (dev == ಶೂನ್ಯಸೂಚಕ) {
				dev = ಹೊಸ px4_dev_t(name, (cdev::CDev *)ದತ್ತಾಂಶ);
				PX4_DEBUG("Registered DEV %s", name);
				ret = PX4_OK;
				ನಿಲ್ಲಿಸು;
			}
		}

		pthread_mutex_unlock(&devmutex);

		ಆದರೆ (ret != PX4_OK) {
			PX4_ERR("No free devmap entries - increase devmap size");
		}

		ಹಿಂತಿರುಗಿ ret;
	}

	ಪೂರ್ಣಾಂಕ unregister_driver(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name)
	{
		PX4_DEBUG("CDev::unregister_driver %s", name);
		ಪೂರ್ಣಾಂಕ ret = -EINVAL;

		ಆದರೆ (name == ಶೂನ್ಯಸೂಚಕ) {
			ಹಿಂತಿರುಗಿ -EINVAL;
		}

		pthread_mutex_lock(&devmutex);

		ಗಾಗಿ (ಸ್ವಯಂಚಾಲಿತ &dev : devmap) {
			ಆದರೆ (dev && (strcmp(name, dev->name) == 0)) {
				ಅಳಿಸು dev;
				dev = ಶೂನ್ಯಸೂಚಕ;
				PX4_DEBUG("Unregistered DEV %s", name);
				ret = PX4_OK;
				ನಿಲ್ಲಿಸು;
			}
		}

		pthread_mutex_unlock(&devmutex);

		ಹಿಂತಿರುಗಿ ret;
	}

	ಪೂರ್ಣಾಂಕ px4_open(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ flags, ...)
	{
		PX4_DEBUG("px4_open");
		cdev::CDev *dev = getDev(ಮಾರ್ಗ);
		ಪೂರ್ಣಾಂಕ ret = 0;
		ಪೂರ್ಣಾಂಕ i;
		mode_t ಬಹುಲಕ;

		ಆದರೆ (!dev && (flags & PX4_F_WRONLY) != 0 &&
		    ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(ಮಾರ್ಗ, "/obj/", 5) != 0 &&
		    ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(ಮಾರ್ಗ, "/dev/", 5) != 0) {
			va_list p;
			va_ಆರಂಭ(p, flags);
			ಬಹುಲಕ = va_arg(p, ಪೂರ್ಣಾಂಕ);
			va_ಕೊನೆ(p);

			// Create the file
			PX4_DEBUG("Creating virtual file %s", ಮಾರ್ಗ);
			dev = ಹೊಸ VFile(ಮಾರ್ಗ, ಬಹುಲಕ);
			ರಿಜಿಸ್ಟರ್_driver(ಮಾರ್ಗ, ಶೂನ್ಯಸೂಚಕ, 0666, (ಶೂನ್ಯ *)dev);
		}

		ಆದರೆ (dev) {
			pthread_mutex_lock(&filemutex);

			ಗಾಗಿ (i = 0; i < PX4_MAX_FD; ++i) {
				ಆದರೆ (filemap[i].cdev == ಶೂನ್ಯಸೂಚಕ) {
					filemap[i] = cdev::file_t(flags, dev);
					ನಿಲ್ಲಿಸು;
				}
			}

			pthread_mutex_unlock(&filemutex);

			ಆದರೆ (i < PX4_MAX_FD) {
				ret = dev->open(&filemap[i]);

			} ಇಲ್ಲದಿದ್ದರೆ {

				ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ NAMELEN = 32;
				ಅಕ್ಷರ thread_name[NAMELEN] {};

				PX4_WARN("%s: exceeded maximum number of file descriptors, accessing %s", thread_name, ಮಾರ್ಗ);
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __PX4_QURT
				ಪೂರ್ಣಾಂಕ nret = pthread_getname_np(pthread_self(), thread_name, NAMELEN);

				ಆದರೆ (nret || thread_name[0] == 0) {
					PX4_WARN("failed getting thread name");
				}

#ಆದರೆ_ಅಂತ್ಯ

				ret = -ENOENT;
			}

		} ಇಲ್ಲದಿದ್ದರೆ {
			ret = -EINVAL;
		}

		ಆದರೆ (ret < 0) {
			errno = -ret;
			ಹಿಂತಿರುಗಿ -1;
		}

		PX4_DEBUG("px4_open fd = %d", i);
		ಹಿಂತಿರುಗಿ i;
	}

	ಪೂರ್ಣಾಂಕ px4_close(ಪೂರ್ಣಾಂಕ fd)
	{
		ಪೂರ್ಣಾಂಕ ret;

		cdev::CDev *dev = getFile(fd);

		ಆದರೆ (dev) {
			pthread_mutex_lock(&filemutex);
			ret = dev->close(&filemap[fd]);

			filemap[fd].cdev = ಶೂನ್ಯಸೂಚಕ;

			pthread_mutex_unlock(&filemutex);
			PX4_DEBUG("px4_close fd = %d", fd);

		} ಇಲ್ಲದಿದ್ದರೆ {
			ret = -EINVAL;
		}

		ಆದರೆ (ret < 0) {
			ret = PX4_ERROR;
		}

		ಹಿಂತಿರುಗಿ ret;
	}

	ssize_t px4_read(ಪೂರ್ಣಾಂಕ fd, ಶೂನ್ಯ *ಬಫರ್_2, size_t buflen)
	{
		ಪೂರ್ಣಾಂಕ ret;

		cdev::CDev *dev = getFile(fd);

		ಆದರೆ (dev) {
			PX4_DEBUG("px4_read fd = %d", fd);
			ret = dev->ಪಠ(&filemap[fd], (ಅಕ್ಷರ *)ಬಫರ್_2, buflen);

		} ಇಲ್ಲದಿದ್ದರೆ {
			ret = -EINVAL;
		}

		ಆದರೆ (ret < 0) {
			ret = PX4_ERROR;
		}

		ಹಿಂತಿರುಗಿ ret;
	}

	ssize_t px4_write(ಪೂರ್ಣಾಂಕ fd, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಬಫರ್_2, size_t buflen)
	{
		ಪೂರ್ಣಾಂಕ ret;

		cdev::CDev *dev = getFile(fd);

		ಆದರೆ (dev) {
			PX4_DEBUG("px4_write fd = %d", fd);
			ret = dev->ಲಿಖ(&filemap[fd], (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಬಫರ್_2, buflen);

		} ಇಲ್ಲದಿದ್ದರೆ {
			ret = -EINVAL;
		}

		ಆದರೆ (ret < 0) {
			ret = PX4_ERROR;
		}

		ಹಿಂತಿರುಗಿ ret;
	}

	ಪೂರ್ಣಾಂಕ px4_ioctl(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ cmd, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ arg)
	{
		PX4_DEBUG("px4_ioctl fd = %d", fd);
		ಪೂರ್ಣಾಂಕ ret = 0;

		cdev::CDev *dev = getFile(fd);

		ಆದರೆ (dev) {
			ret = dev->ioctl(&filemap[fd], cmd, arg);

		} ಇಲ್ಲದಿದ್ದರೆ {
			ret = -EINVAL;
		}

		ಹಿಂತಿರುಗಿ ret;
	}

	ಪೂರ್ಣಾಂಕ px4_poll(px4_pollfd_struct_t *fds, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ nfds, ಪೂರ್ಣಾಂಕ timeout)
	{
		ಆದರೆ (nfds == 0) {
			PX4_WARN("px4_poll with no fds");
			ಹಿಂತಿರುಗಿ -1;
		}

		px4_sem_t sem;
		ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2 = 0;
		ಪೂರ್ಣಾಂಕ ret = -1;

		ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ NAMELEN = 32;
		ಅಕ್ಷರ thread_name[NAMELEN] {};

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __PX4_QURT
		ಪೂರ್ಣಾಂಕ nret = pthread_getname_np(pthread_self(), thread_name, NAMELEN);

		ಆದರೆ (nret || thread_name[0] == 0) {
			PX4_WARN("failed getting thread name");
		}

#ಆದರೆ_ಅಂತ್ಯ

		PX4_DEBUG("Called px4_poll timeout = %d", timeout);

		px4_sem_init(&sem, 0, 0);

		// sem use case is a signal
		px4_sem_setprotocol(&sem, SEM_PRIO_NONE);

		// Go through all fds and check them for a pollable state
		ತಾರ್ಕಿಕ fd_pollable = ಸುಳ್ಳು;

		ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ i = 0; i < nfds; ++i) {
			fds[i].sem     = &sem;
			fds[i].revents = 0;
			fds[i].priv    = ಶೂನ್ಯಸೂಚಕ;

			cdev::CDev *dev = getFile(fds[i].fd);

			// If fd is valid
			ಆದರೆ (dev) {
				PX4_DEBUG("%s: px4_poll: CDev->poll(setup) %d", thread_name, fds[i].fd);
				ret = dev->poll(&filemap[fds[i].fd], &fds[i], ನಿಜ);

				ಆದರೆ (ret < 0) {
					PX4_WARN("%s: px4_poll() error: %s", thread_name, strerror(errno));
					ನಿಲ್ಲಿಸು;
				}

				ಆದರೆ (ret >= 0) {
					fd_pollable = ನಿಜ;
				}
			}
		}

		// If any FD can be polled, lock the semaphore and
		// check for new data
		ಆದರೆ (fd_pollable) {
			ಆದರೆ (timeout > 0) {
				// Get the current time
				ರಚನೆ timespec ts;
#ಆದರೆ defined(__PX4_DARWIN) && !defined(ENABLE_LOCKSTEP_SCHEDULER)
				// macOS has no pthread_condattr_setclock(), so px4_sem_t's condition
				// variable waits on CLOCK_REALTIME (px4_sem_init()). Use that clock here
				// too. CLOCK_MONOTONIC counts from boot, so the deadline would land
				// decades in the past and px4_sem_timedwait() would return immediately.
				px4_clock_gettime(CLOCK_REALTIME, &ts);
#ಇಲ್ಲದಿದ್ದರೆ
				// Note, we can't actually use CLOCK_MONOTONIC on macOS
				// but that's hidden and implemented in px4_clock_gettime.
				px4_clock_gettime(CLOCK_MONOTONIC, &ts);
#ಆದರೆ_ಅಂತ್ಯ

				// Calculate an absolute time in the future
				ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ billion = (1000 * 1000 * 1000);
				uint64_t nsecs = ts.tv_nsec + ((uint64_t)timeout * 1000 * 1000);
				ts.tv_sec += nsecs / billion;
				nsecs -= (nsecs / billion) * billion;
				ts.tv_nsec = nsecs;

				ret = px4_sem_timedwait(&sem, &ts);

				ಆದರೆ (ret && errno != ETIMEDOUT) {
					PX4_WARN("%s: px4_poll() sem error: %s", thread_name, strerror(errno));
				}

			} ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (timeout < 0) {
				px4_sem_wait(&sem);
			}

			// We have waited now (or not, depending on timeout),
			// go through all fds and count how many have data
			ಗಾಗಿ (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ i = 0; i < nfds; ++i) {

				cdev::CDev *dev = getFile(fds[i].fd);

				// If fd is valid
				ಆದರೆ (dev) {
					PX4_DEBUG("%s: px4_poll: CDev->poll(teardown) %d", thread_name, fds[i].fd);
					ret = dev->poll(&filemap[fds[i].fd], &fds[i], ಸುಳ್ಳು);

					ಆದರೆ (ret < 0) {
						PX4_WARN("%s: px4_poll() 2nd poll fail", thread_name);
						ನಿಲ್ಲಿಸು;
					}

					ಆದರೆ (fds[i].revents) {
						ಎಣಿಕೆ_2 += 1;
					}
				}
			}
		}

		px4_sem_destroy(&sem);

		// Return the positive count if present,
		// return the negative error number if failed
		ಹಿಂತಿರುಗಿ (ಎಣಿಕೆ_2) ? ಎಣಿಕೆ_2 : ret;
	}

	ಪೂರ್ಣಾಂಕ px4_access(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *pathname, ಪೂರ್ಣಾಂಕ ಬಹುಲಕ)
	{
		ಆದರೆ (ಬಹುಲಕ != F_OK) {
			errno = EINVAL;
			ಹಿಂತಿರುಗಿ -1;
		}

		cdev::CDev *dev = getDev(pathname);
		ಹಿಂತಿರುಗಿ (dev != ಶೂನ್ಯಸೂಚಕ) ? 0 : -1;
	}

	ಶೂನ್ಯ px4_show_files()
	{
		PX4_INFO("Files:");

		pthread_mutex_lock(&devmutex);

		ಗಾಗಿ (ಸ್ಥಿರಾಂಕ ಸ್ವಯಂಚಾಲಿತ &dev : devmap) {
			ಆದರೆ (dev) {
				PX4_INFO_RAW("   %s\n", dev->name);
			}
		}

		pthread_mutex_unlock(&devmutex);
	}

} // extern "C"
