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

#अन्तर्भाव "cdev_platform.hpp"

#अन्तर्भाव "../CDev.hpp"

#अन्तर्भाव <px4_platform_common/log.h>
#अन्तर्भाव <px4_platform_common/posix.h>
#अन्तर्भाव <px4_platform_common/time.h>

#अन्तर्भाव <stdlib.h>

स्थिर ध्रुव cdev::px4_file_operations_t g_fops = {};
ध्रुव cdev::px4_file_operations_t &cdev::CDev::fops_ref() { प्रत्यागम g_fops; }

pthread_mutex_t devmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t filemutex = PTHREAD_MUTEX_INITIALIZER;

संरचना px4_dev_t {
	वर्ण *name{nullptr};
	cdev::CDev *cdev{nullptr};

	px4_dev_t(ध्रुव वर्ण *n, cdev::CDev *c) : cdev(c)
	{
		name = strdup(n);
	}

	~px4_dev_t()
	{
		मुक्त(name);
	}
गुप्त:
	px4_dev_t() = स्वभाव;
};

#define PX4_MAX_FD 512
स्थिर px4_dev_t *devmap[PX4_MAX_FD] {};
स्थिर cdev::file_t filemap[PX4_MAX_FD] {};

वर्ग VFile : सार्वजनिक cdev::CDev
{
सार्वजनिक:
	VFile(ध्रुव वर्ण *fname, mode_t mode) : cdev::CDev(fname) {}
	~VFile() override = स्वभाव;

	ssize_t लिख(cdev::file_t *handlep, ध्रुव वर्ण *buffer, size_t buflen) override
	{
		// ignore what was written, but let pollers know something was written
		poll_notify(POLLIN);
		प्रत्यागम buflen;
	}
};

स्थिर cdev::CDev *getDev(ध्रुव वर्ण *path)
{
	pthread_mutex_lock(&devmutex);

	प्रति (ध्रुव auto &dev : devmap) {
		जइ (dev && (strcmp(dev->name, path) == 0)) {
			pthread_mutex_unlock(&devmutex);
			प्रत्यागम dev->cdev;
		}
	}

	pthread_mutex_unlock(&devmutex);

	प्रत्यागम nullptr;
}

स्थिर cdev::CDev *getFile(पूर्णाङ्क fd)
{
	pthread_mutex_lock(&filemutex);
	cdev::CDev *dev = nullptr;

	जइ (fd < PX4_MAX_FD && fd >= 0) {
		dev = filemap[fd].cdev;
	}

	pthread_mutex_unlock(&filemutex);
	प्रत्यागम dev;
}

बाह्य "C" {

	पूर्णाङ्क register_driver(ध्रुव वर्ण *name, ध्रुव cdev::px4_file_operations_t *fops, cdev::mode_t mode, शून्य *data)
	{
		PX4_DEBUG("CDev::register_driver %s", name);
		पूर्णाङ्क ret = -ENOSPC;

		जइ (name == nullptr || data == nullptr) {
			प्रत्यागम -EINVAL;
		}

		pthread_mutex_lock(&devmutex);

		// Make sure the device does not already exist
		प्रति (ध्रुव auto &dev : devmap) {
			जइ (dev && (strcmp(dev->name, name) == 0)) {
				pthread_mutex_unlock(&devmutex);
				प्रत्यागम -EEXIST;
			}
		}

		प्रति (auto &dev : devmap) {
			जइ (dev == nullptr) {
				dev = नूतन px4_dev_t(name, (cdev::CDev *)data);
				PX4_DEBUG("Registered DEV %s", name);
				ret = PX4_OK;
				भङ्ग;
			}
		}

		pthread_mutex_unlock(&devmutex);

		जइ (ret != PX4_OK) {
			PX4_ERR("No free devmap entries - increase devmap size");
		}

		प्रत्यागम ret;
	}

	पूर्णाङ्क unregister_driver(ध्रुव वर्ण *name)
	{
		PX4_DEBUG("CDev::unregister_driver %s", name);
		पूर्णाङ्क ret = -EINVAL;

		जइ (name == nullptr) {
			प्रत्यागम -EINVAL;
		}

		pthread_mutex_lock(&devmutex);

		प्रति (auto &dev : devmap) {
			जइ (dev && (strcmp(name, dev->name) == 0)) {
				delete dev;
				dev = nullptr;
				PX4_DEBUG("Unregistered DEV %s", name);
				ret = PX4_OK;
				भङ्ग;
			}
		}

		pthread_mutex_unlock(&devmutex);

		प्रत्यागम ret;
	}

	पूर्णाङ्क px4_open(ध्रुव वर्ण *path, पूर्णाङ्क flags, ...)
	{
		PX4_DEBUG("px4_open");
		cdev::CDev *dev = getDev(path);
		पूर्णाङ्क ret = 0;
		पूर्णाङ्क i;
		mode_t mode;

		जइ (!dev && (flags & PX4_F_WRONLY) != 0 &&
		    strncmp(path, "/obj/", 5) != 0 &&
		    strncmp(path, "/dev/", 5) != 0) {
			va_list p;
			va_start(p, flags);
			mode = va_arg(p, पूर्णाङ्क);
			va_end(p);

			// Create the file
			PX4_DEBUG("Creating virtual file %s", path);
			dev = नूतन VFile(path, mode);
			register_driver(path, nullptr, 0666, (शून्य *)dev);
		}

		जइ (dev) {
			pthread_mutex_lock(&filemutex);

			प्रति (i = 0; i < PX4_MAX_FD; ++i) {
				जइ (filemap[i].cdev == nullptr) {
					filemap[i] = cdev::file_t(flags, dev);
					भङ्ग;
				}
			}

			pthread_mutex_unlock(&filemutex);

			जइ (i < PX4_MAX_FD) {
				ret = dev->open(&filemap[i]);

			} अण्णहा {

				ध्रुव unsigned NAMELEN = 32;
				वर्ण thread_name[NAMELEN] {};

				PX4_WARN("%s: exceeded maximum number of file descriptors, accessing %s", thread_name, path);
#ifndef __PX4_QURT
				पूर्णाङ्क nret = pthread_getname_np(pthread_self(), thread_name, NAMELEN);

				जइ (nret || thread_name[0] == 0) {
					PX4_WARN("failed getting thread name");
				}

#endif

				ret = -ENOENT;
			}

		} अण्णहा {
			ret = -EINVAL;
		}

		जइ (ret < 0) {
			errno = -ret;
			प्रत्यागम -1;
		}

		PX4_DEBUG("px4_open fd = %d", i);
		प्रत्यागम i;
	}

	पूर्णाङ्क px4_close(पूर्णाङ्क fd)
	{
		पूर्णाङ्क ret;

		cdev::CDev *dev = getFile(fd);

		जइ (dev) {
			pthread_mutex_lock(&filemutex);
			ret = dev->close(&filemap[fd]);

			filemap[fd].cdev = nullptr;

			pthread_mutex_unlock(&filemutex);
			PX4_DEBUG("px4_close fd = %d", fd);

		} अण्णहा {
			ret = -EINVAL;
		}

		जइ (ret < 0) {
			ret = PX4_ERROR;
		}

		प्रत्यागम ret;
	}

	ssize_t px4_read(पूर्णाङ्क fd, शून्य *buffer, size_t buflen)
	{
		पूर्णाङ्क ret;

		cdev::CDev *dev = getFile(fd);

		जइ (dev) {
			PX4_DEBUG("px4_read fd = %d", fd);
			ret = dev->पठ(&filemap[fd], (वर्ण *)buffer, buflen);

		} अण्णहा {
			ret = -EINVAL;
		}

		जइ (ret < 0) {
			ret = PX4_ERROR;
		}

		प्रत्यागम ret;
	}

	ssize_t px4_write(पूर्णाङ्क fd, ध्रुव शून्य *buffer, size_t buflen)
	{
		पूर्णाङ्क ret;

		cdev::CDev *dev = getFile(fd);

		जइ (dev) {
			PX4_DEBUG("px4_write fd = %d", fd);
			ret = dev->लिख(&filemap[fd], (ध्रुव वर्ण *)buffer, buflen);

		} अण्णहा {
			ret = -EINVAL;
		}

		जइ (ret < 0) {
			ret = PX4_ERROR;
		}

		प्रत्यागम ret;
	}

	पूर्णाङ्क px4_ioctl(पूर्णाङ्क fd, पूर्णाङ्क cmd, unsigned long arg)
	{
		PX4_DEBUG("px4_ioctl fd = %d", fd);
		पूर्णाङ्क ret = 0;

		cdev::CDev *dev = getFile(fd);

		जइ (dev) {
			ret = dev->ioctl(&filemap[fd], cmd, arg);

		} अण्णहा {
			ret = -EINVAL;
		}

		प्रत्यागम ret;
	}

	पूर्णाङ्क px4_poll(px4_pollfd_struct_t *fds, unsigned पूर्णाङ्क nfds, पूर्णाङ्क timeout)
	{
		जइ (nfds == 0) {
			PX4_WARN("px4_poll with no fds");
			प्रत्यागम -1;
		}

		px4_sem_t sem;
		पूर्णाङ्क गणनम् = 0;
		पूर्णाङ्क ret = -1;

		ध्रुव unsigned NAMELEN = 32;
		वर्ण thread_name[NAMELEN] {};

#ifndef __PX4_QURT
		पूर्णाङ्क nret = pthread_getname_np(pthread_self(), thread_name, NAMELEN);

		जइ (nret || thread_name[0] == 0) {
			PX4_WARN("failed getting thread name");
		}

#endif

		PX4_DEBUG("Called px4_poll timeout = %d", timeout);

		px4_sem_init(&sem, 0, 0);

		// sem use case is a signal
		px4_sem_setprotocol(&sem, SEM_PRIO_NONE);

		// Go through all fds and check them for a pollable state
		तार्किक fd_pollable = मिच्छ;

		प्रति (unsigned पूर्णाङ्क i = 0; i < nfds; ++i) {
			fds[i].sem     = &sem;
			fds[i].revents = 0;
			fds[i].priv    = nullptr;

			cdev::CDev *dev = getFile(fds[i].fd);

			// If fd is valid
			जइ (dev) {
				PX4_DEBUG("%s: px4_poll: CDev->poll(setup) %d", thread_name, fds[i].fd);
				ret = dev->poll(&filemap[fds[i].fd], &fds[i], सच्च);

				जइ (ret < 0) {
					PX4_WARN("%s: px4_poll() error: %s", thread_name, strerror(errno));
					भङ्ग;
				}

				जइ (ret >= 0) {
					fd_pollable = सच्च;
				}
			}
		}

		// If any FD can be polled, lock the semaphore and
		// check for new data
		जइ (fd_pollable) {
			जइ (timeout > 0) {
				// Get the current time
				संरचना timespec ts;
#जइ defined(__PX4_DARWIN) && !defined(ENABLE_LOCKSTEP_SCHEDULER)
				// macOS has no pthread_condattr_setclock(), so px4_sem_t's condition
				// variable waits on CLOCK_REALTIME (px4_sem_init()). Use that clock here
				// too. CLOCK_MONOTONIC counts from boot, so the deadline would land
				// decades in the past and px4_sem_timedwait() would return immediately.
				px4_clock_gettime(CLOCK_REALTIME, &ts);
#अण्णहा
				// Note, we can't actually use CLOCK_MONOTONIC on macOS
				// but that's hidden and implemented in px4_clock_gettime.
				px4_clock_gettime(CLOCK_MONOTONIC, &ts);
#endif

				// Calculate an absolute time in the future
				ध्रुव unsigned billion = (1000 * 1000 * 1000);
				uint64_t nsecs = ts.tv_nsec + ((uint64_t)timeout * 1000 * 1000);
				ts.tv_sec += nsecs / billion;
				nsecs -= (nsecs / billion) * billion;
				ts.tv_nsec = nsecs;

				ret = px4_sem_timedwait(&sem, &ts);

				जइ (ret && errno != ETIMEDOUT) {
					PX4_WARN("%s: px4_poll() sem error: %s", thread_name, strerror(errno));
				}

			} अण्णहा जइ (timeout < 0) {
				px4_sem_wait(&sem);
			}

			// We have waited now (or not, depending on timeout),
			// go through all fds and count how many have data
			प्रति (unsigned पूर्णाङ्क i = 0; i < nfds; ++i) {

				cdev::CDev *dev = getFile(fds[i].fd);

				// If fd is valid
				जइ (dev) {
					PX4_DEBUG("%s: px4_poll: CDev->poll(teardown) %d", thread_name, fds[i].fd);
					ret = dev->poll(&filemap[fds[i].fd], &fds[i], मिच्छ);

					जइ (ret < 0) {
						PX4_WARN("%s: px4_poll() 2nd poll fail", thread_name);
						भङ्ग;
					}

					जइ (fds[i].revents) {
						गणनम् += 1;
					}
				}
			}
		}

		px4_sem_destroy(&sem);

		// Return the positive count if present,
		// return the negative error number if failed
		प्रत्यागम (गणनम्) ? गणनम् : ret;
	}

	पूर्णाङ्क px4_access(ध्रुव वर्ण *pathname, पूर्णाङ्क mode)
	{
		जइ (mode != F_OK) {
			errno = EINVAL;
			प्रत्यागम -1;
		}

		cdev::CDev *dev = getDev(pathname);
		प्रत्यागम (dev != nullptr) ? 0 : -1;
	}

	शून्य px4_show_files()
	{
		PX4_INFO("Files:");

		pthread_mutex_lock(&devmutex);

		प्रति (ध्रुव auto &dev : devmap) {
			जइ (dev) {
				PX4_INFO_RAW("   %s\n", dev->name);
			}
		}

		pthread_mutex_unlock(&devmutex);
	}

} // extern "C"
