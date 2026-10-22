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

#അന്തര്ഭാവ "cdev_platform.hpp"

#അന്തര്ഭാവ "../CDev.hpp"

#അന്തര്ഭാവ <px4_platform_common/log.h>
#അന്തര്ഭാവ <px4_platform_common/posix.h>
#അന്തര്ഭാവ <px4_platform_common/time.h>

#അന്തര്ഭാവ <stdlib.h>

സ്ഥിതം സ്ഥിരം cdev::px4_file_operations_t g_fops = {};
സ്ഥിരം cdev::px4_file_operations_t &cdev::CDev::fops_ref() { തിരികെ g_fops; }

pthread_mutex_t devmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t filemutex = PTHREAD_MUTEX_INITIALIZER;

ഘടന px4_dev_t {
	അക്ഷരം *name{nullptr};
	cdev::CDev *cdev{nullptr};

	px4_dev_t(സ്ഥിരം അക്ഷരം *n, cdev::CDev *c) : cdev(c)
	{
		name = strdup(n);
	}

	~px4_dev_t()
	{
		സ്വതന്ത്രം(name);
	}
സ്വകാര്യ:
	px4_dev_t() = സ്ഥിരസ്ഥിതി;
};

#define PX4_MAX_FD 512
സ്ഥിതം px4_dev_t *devmap[PX4_MAX_FD] {};
സ്ഥിതം cdev::file_t filemap[PX4_MAX_FD] {};

വർഗ്ഗം VFile : പൊതു cdev::CDev
{
പൊതു:
	VFile(സ്ഥിരം അക്ഷരം *fname, mode_t mode) : cdev::CDev(fname) {}
	~VFile() override = സ്ഥിരസ്ഥിതി;

	ssize_t ലിഖ(cdev::file_t *handlep, സ്ഥിരം അക്ഷരം *buffer, size_t buflen) override
	{
		// ignore what was written, but let pollers know something was written
		poll_notify(POLLIN);
		തിരികെ buflen;
	}
};

സ്ഥിതം cdev::CDev *getDev(സ്ഥിരം അക്ഷരം *path)
{
	pthread_mutex_lock(&devmutex);

	വേണ്ടി (സ്ഥിരം auto &dev : devmap) {
		എങ്കിൽ (dev && (strcmp(dev->name, path) == 0)) {
			pthread_mutex_unlock(&devmutex);
			തിരികെ dev->cdev;
		}
	}

	pthread_mutex_unlock(&devmutex);

	തിരികെ nullptr;
}

സ്ഥിതം cdev::CDev *getFile(പൂർണ്ണസംഖ്യ fd)
{
	pthread_mutex_lock(&filemutex);
	cdev::CDev *dev = nullptr;

	എങ്കിൽ (fd < PX4_MAX_FD && fd >= 0) {
		dev = filemap[fd].cdev;
	}

	pthread_mutex_unlock(&filemutex);
	തിരികെ dev;
}

ബാഹ്യ "C" {

	പൂർണ്ണസംഖ്യ register_driver(സ്ഥിരം അക്ഷരം *name, സ്ഥിരം cdev::px4_file_operations_t *fops, cdev::mode_t mode, ശൂന്യം *data)
	{
		PX4_DEBUG("CDev::register_driver %s", name);
		പൂർണ്ണസംഖ്യ ret = -ENOSPC;

		എങ്കിൽ (name == nullptr || data == nullptr) {
			തിരികെ -EINVAL;
		}

		pthread_mutex_lock(&devmutex);

		// Make sure the device does not already exist
		വേണ്ടി (സ്ഥിരം auto &dev : devmap) {
			എങ്കിൽ (dev && (strcmp(dev->name, name) == 0)) {
				pthread_mutex_unlock(&devmutex);
				തിരികെ -EEXIST;
			}
		}

		വേണ്ടി (auto &dev : devmap) {
			എങ്കിൽ (dev == nullptr) {
				dev = പുതിയ px4_dev_t(name, (cdev::CDev *)data);
				PX4_DEBUG("Registered DEV %s", name);
				ret = PX4_OK;
				നിർത്തുക;
			}
		}

		pthread_mutex_unlock(&devmutex);

		എങ്കിൽ (ret != PX4_OK) {
			PX4_ERR("No free devmap entries - increase devmap size");
		}

		തിരികെ ret;
	}

	പൂർണ്ണസംഖ്യ unregister_driver(സ്ഥിരം അക്ഷരം *name)
	{
		PX4_DEBUG("CDev::unregister_driver %s", name);
		പൂർണ്ണസംഖ്യ ret = -EINVAL;

		എങ്കിൽ (name == nullptr) {
			തിരികെ -EINVAL;
		}

		pthread_mutex_lock(&devmutex);

		വേണ്ടി (auto &dev : devmap) {
			എങ്കിൽ (dev && (strcmp(name, dev->name) == 0)) {
				delete dev;
				dev = nullptr;
				PX4_DEBUG("Unregistered DEV %s", name);
				ret = PX4_OK;
				നിർത്തുക;
			}
		}

		pthread_mutex_unlock(&devmutex);

		തിരികെ ret;
	}

	പൂർണ്ണസംഖ്യ px4_open(സ്ഥിരം അക്ഷരം *path, പൂർണ്ണസംഖ്യ flags, ...)
	{
		PX4_DEBUG("px4_open");
		cdev::CDev *dev = getDev(path);
		പൂർണ്ണസംഖ്യ ret = 0;
		പൂർണ്ണസംഖ്യ i;
		mode_t mode;

		എങ്കിൽ (!dev && (flags & PX4_F_WRONLY) != 0 &&
		    strncmp(path, "/obj/", 5) != 0 &&
		    strncmp(path, "/dev/", 5) != 0) {
			va_list p;
			va_start(p, flags);
			mode = va_arg(p, പൂർണ്ണസംഖ്യ);
			va_end(p);

			// Create the file
			PX4_DEBUG("Creating virtual file %s", path);
			dev = പുതിയ VFile(path, mode);
			register_driver(path, nullptr, 0666, (ശൂന്യം *)dev);
		}

		എങ്കിൽ (dev) {
			pthread_mutex_lock(&filemutex);

			വേണ്ടി (i = 0; i < PX4_MAX_FD; ++i) {
				എങ്കിൽ (filemap[i].cdev == nullptr) {
					filemap[i] = cdev::file_t(flags, dev);
					നിർത്തുക;
				}
			}

			pthread_mutex_unlock(&filemutex);

			എങ്കിൽ (i < PX4_MAX_FD) {
				ret = dev->open(&filemap[i]);

			} അല്ലെങ്കിൽ {

				സ്ഥിരം unsigned NAMELEN = 32;
				അക്ഷരം thread_name[NAMELEN] {};

				PX4_WARN("%s: exceeded maximum number of file descriptors, accessing %s", thread_name, path);
#ifndef __PX4_QURT
				പൂർണ്ണസംഖ്യ nret = pthread_getname_np(pthread_self(), thread_name, NAMELEN);

				എങ്കിൽ (nret || thread_name[0] == 0) {
					PX4_WARN("failed getting thread name");
				}

#endif

				ret = -ENOENT;
			}

		} അല്ലെങ്കിൽ {
			ret = -EINVAL;
		}

		എങ്കിൽ (ret < 0) {
			errno = -ret;
			തിരികെ -1;
		}

		PX4_DEBUG("px4_open fd = %d", i);
		തിരികെ i;
	}

	പൂർണ്ണസംഖ്യ px4_close(പൂർണ്ണസംഖ്യ fd)
	{
		പൂർണ്ണസംഖ്യ ret;

		cdev::CDev *dev = getFile(fd);

		എങ്കിൽ (dev) {
			pthread_mutex_lock(&filemutex);
			ret = dev->close(&filemap[fd]);

			filemap[fd].cdev = nullptr;

			pthread_mutex_unlock(&filemutex);
			PX4_DEBUG("px4_close fd = %d", fd);

		} അല്ലെങ്കിൽ {
			ret = -EINVAL;
		}

		എങ്കിൽ (ret < 0) {
			ret = PX4_ERROR;
		}

		തിരികെ ret;
	}

	ssize_t px4_read(പൂർണ്ണസംഖ്യ fd, ശൂന്യം *buffer, size_t buflen)
	{
		പൂർണ്ണസംഖ്യ ret;

		cdev::CDev *dev = getFile(fd);

		എങ്കിൽ (dev) {
			PX4_DEBUG("px4_read fd = %d", fd);
			ret = dev->പഠ(&filemap[fd], (അക്ഷരം *)buffer, buflen);

		} അല്ലെങ്കിൽ {
			ret = -EINVAL;
		}

		എങ്കിൽ (ret < 0) {
			ret = PX4_ERROR;
		}

		തിരികെ ret;
	}

	ssize_t px4_write(പൂർണ്ണസംഖ്യ fd, സ്ഥിരം ശൂന്യം *buffer, size_t buflen)
	{
		പൂർണ്ണസംഖ്യ ret;

		cdev::CDev *dev = getFile(fd);

		എങ്കിൽ (dev) {
			PX4_DEBUG("px4_write fd = %d", fd);
			ret = dev->ലിഖ(&filemap[fd], (സ്ഥിരം അക്ഷരം *)buffer, buflen);

		} അല്ലെങ്കിൽ {
			ret = -EINVAL;
		}

		എങ്കിൽ (ret < 0) {
			ret = PX4_ERROR;
		}

		തിരികെ ret;
	}

	പൂർണ്ണസംഖ്യ px4_ioctl(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ cmd, unsigned long arg)
	{
		PX4_DEBUG("px4_ioctl fd = %d", fd);
		പൂർണ്ണസംഖ്യ ret = 0;

		cdev::CDev *dev = getFile(fd);

		എങ്കിൽ (dev) {
			ret = dev->ioctl(&filemap[fd], cmd, arg);

		} അല്ലെങ്കിൽ {
			ret = -EINVAL;
		}

		തിരികെ ret;
	}

	പൂർണ്ണസംഖ്യ px4_poll(px4_pollfd_struct_t *fds, unsigned പൂർണ്ണസംഖ്യ nfds, പൂർണ്ണസംഖ്യ timeout)
	{
		എങ്കിൽ (nfds == 0) {
			PX4_WARN("px4_poll with no fds");
			തിരികെ -1;
		}

		px4_sem_t sem;
		പൂർണ്ണസംഖ്യ എണ്ണം = 0;
		പൂർണ്ണസംഖ്യ ret = -1;

		സ്ഥിരം unsigned NAMELEN = 32;
		അക്ഷരം thread_name[NAMELEN] {};

#ifndef __PX4_QURT
		പൂർണ്ണസംഖ്യ nret = pthread_getname_np(pthread_self(), thread_name, NAMELEN);

		എങ്കിൽ (nret || thread_name[0] == 0) {
			PX4_WARN("failed getting thread name");
		}

#endif

		PX4_DEBUG("Called px4_poll timeout = %d", timeout);

		px4_sem_init(&sem, 0, 0);

		// sem use case is a signal
		px4_sem_setprotocol(&sem, SEM_PRIO_NONE);

		// Go through all fds and check them for a pollable state
		തർക്കം fd_pollable = അസത്യം;

		വേണ്ടി (unsigned പൂർണ്ണസംഖ്യ i = 0; i < nfds; ++i) {
			fds[i].sem     = &sem;
			fds[i].revents = 0;
			fds[i].priv    = nullptr;

			cdev::CDev *dev = getFile(fds[i].fd);

			// If fd is valid
			എങ്കിൽ (dev) {
				PX4_DEBUG("%s: px4_poll: CDev->poll(setup) %d", thread_name, fds[i].fd);
				ret = dev->poll(&filemap[fds[i].fd], &fds[i], സത്യം);

				എങ്കിൽ (ret < 0) {
					PX4_WARN("%s: px4_poll() error: %s", thread_name, strerror(errno));
					നിർത്തുക;
				}

				എങ്കിൽ (ret >= 0) {
					fd_pollable = സത്യം;
				}
			}
		}

		// If any FD can be polled, lock the semaphore and
		// check for new data
		എങ്കിൽ (fd_pollable) {
			എങ്കിൽ (timeout > 0) {
				// Get the current time
				ഘടന timespec ts;
#എങ്കിൽ defined(__PX4_DARWIN) && !defined(ENABLE_LOCKSTEP_SCHEDULER)
				// macOS has no pthread_condattr_setclock(), so px4_sem_t's condition
				// variable waits on CLOCK_REALTIME (px4_sem_init()). Use that clock here
				// too. CLOCK_MONOTONIC counts from boot, so the deadline would land
				// decades in the past and px4_sem_timedwait() would return immediately.
				px4_clock_gettime(CLOCK_REALTIME, &ts);
#അല്ലെങ്കിൽ
				// Note, we can't actually use CLOCK_MONOTONIC on macOS
				// but that's hidden and implemented in px4_clock_gettime.
				px4_clock_gettime(CLOCK_MONOTONIC, &ts);
#endif

				// Calculate an absolute time in the future
				സ്ഥിരം unsigned billion = (1000 * 1000 * 1000);
				uint64_t nsecs = ts.tv_nsec + ((uint64_t)timeout * 1000 * 1000);
				ts.tv_sec += nsecs / billion;
				nsecs -= (nsecs / billion) * billion;
				ts.tv_nsec = nsecs;

				ret = px4_sem_timedwait(&sem, &ts);

				എങ്കിൽ (ret && errno != ETIMEDOUT) {
					PX4_WARN("%s: px4_poll() sem error: %s", thread_name, strerror(errno));
				}

			} അല്ലെങ്കിൽ എങ്കിൽ (timeout < 0) {
				px4_sem_wait(&sem);
			}

			// We have waited now (or not, depending on timeout),
			// go through all fds and count how many have data
			വേണ്ടി (unsigned പൂർണ്ണസംഖ്യ i = 0; i < nfds; ++i) {

				cdev::CDev *dev = getFile(fds[i].fd);

				// If fd is valid
				എങ്കിൽ (dev) {
					PX4_DEBUG("%s: px4_poll: CDev->poll(teardown) %d", thread_name, fds[i].fd);
					ret = dev->poll(&filemap[fds[i].fd], &fds[i], അസത്യം);

					എങ്കിൽ (ret < 0) {
						PX4_WARN("%s: px4_poll() 2nd poll fail", thread_name);
						നിർത്തുക;
					}

					എങ്കിൽ (fds[i].revents) {
						എണ്ണം += 1;
					}
				}
			}
		}

		px4_sem_destroy(&sem);

		// Return the positive count if present,
		// return the negative error number if failed
		തിരികെ (എണ്ണം) ? എണ്ണം : ret;
	}

	പൂർണ്ണസംഖ്യ px4_access(സ്ഥിരം അക്ഷരം *pathname, പൂർണ്ണസംഖ്യ mode)
	{
		എങ്കിൽ (mode != F_OK) {
			errno = EINVAL;
			തിരികെ -1;
		}

		cdev::CDev *dev = getDev(pathname);
		തിരികെ (dev != nullptr) ? 0 : -1;
	}

	ശൂന്യം px4_show_files()
	{
		PX4_INFO("Files:");

		pthread_mutex_lock(&devmutex);

		വേണ്ടി (സ്ഥിരം auto &dev : devmap) {
			എങ്കിൽ (dev) {
				PX4_INFO_RAW("   %s\n", dev->name);
			}
		}

		pthread_mutex_unlock(&devmutex);
	}

} // extern "C"
