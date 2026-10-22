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

ساکن ثابت cdev::px4_file_operations_t g_fops = {};
ثابت cdev::px4_file_operations_t &cdev::CDev::fops_ref() { واپس g_fops; }

pthread_mutex_t devmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t filemutex = PTHREAD_MUTEX_INITIALIZER;

ساخت px4_dev_t {
	حرف *name{خالی_اشاریہ};
	cdev::CDev *cdev{خالی_اشاریہ};

	px4_dev_t(ثابت حرف *n, cdev::CDev *c) : cdev(c)
	{
		name = strdup(n);
	}

	~px4_dev_t()
	{
		آزاد(name);
	}
نجی:
	px4_dev_t() = طےشدہ;
};

#تعریف_کرو PX4_MAX_FD 512
ساکن px4_dev_t *devmap[PX4_MAX_FD] {};
ساکن cdev::file_t filemap[PX4_MAX_FD] {};

جماعت VFile : عوامی cdev::CDev
{
عوامی:
	VFile(ثابت حرف *fname, mode_t منوال) : cdev::CDev(fname) {}
	~VFile() override = طےشدہ;

	ssize_t اكتب(cdev::file_t *handlep, ثابت حرف *بفر_2, size_t buflen) override
	{
		// ignore what was written, but let pollers know something was written
		poll_notify(POLLIN);
		واپس buflen;
	}
};

ساکن cdev::CDev *getDev(ثابت حرف *راستہ)
{
	pthread_mutex_lock(&devmutex);

	برائے (ثابت خودکار &dev : devmap) {
		اگر (dev && (strcmp(dev->name, راستہ) == 0)) {
			pthread_mutex_unlock(&devmutex);
			واپس dev->cdev;
		}
	}

	pthread_mutex_unlock(&devmutex);

	واپس خالی_اشاریہ;
}

ساکن cdev::CDev *getFile(صحیح fd)
{
	pthread_mutex_lock(&filemutex);
	cdev::CDev *dev = خالی_اشاریہ;

	اگر (fd < PX4_MAX_FD && fd >= 0) {
		dev = filemap[fd].cdev;
	}

	pthread_mutex_unlock(&filemutex);
	واپس dev;
}

خارجی "C" {

	صحیح رجسٹر_driver(ثابت حرف *name, ثابت cdev::px4_file_operations_t *fops, cdev::mode_t منوال, خالی *ڈیٹا)
	{
		PX4_DEBUG("CDev::register_driver %s", name);
		صحیح ret = -ENOSPC;

		اگر (name == خالی_اشاریہ || ڈیٹا == خالی_اشاریہ) {
			واپس -EINVAL;
		}

		pthread_mutex_lock(&devmutex);

		// Make sure the device does not already exist
		برائے (ثابت خودکار &dev : devmap) {
			اگر (dev && (strcmp(dev->name, name) == 0)) {
				pthread_mutex_unlock(&devmutex);
				واپس -EEXIST;
			}
		}

		برائے (خودکار &dev : devmap) {
			اگر (dev == خالی_اشاریہ) {
				dev = نیا px4_dev_t(name, (cdev::CDev *)ڈیٹا);
				PX4_DEBUG("Registered DEV %s", name);
				ret = PX4_OK;
				توڑو;
			}
		}

		pthread_mutex_unlock(&devmutex);

		اگر (ret != PX4_OK) {
			PX4_ERR("No free devmap entries - increase devmap size");
		}

		واپس ret;
	}

	صحیح unregister_driver(ثابت حرف *name)
	{
		PX4_DEBUG("CDev::unregister_driver %s", name);
		صحیح ret = -EINVAL;

		اگر (name == خالی_اشاریہ) {
			واپس -EINVAL;
		}

		pthread_mutex_lock(&devmutex);

		برائے (خودکار &dev : devmap) {
			اگر (dev && (strcmp(name, dev->name) == 0)) {
				حذف dev;
				dev = خالی_اشاریہ;
				PX4_DEBUG("Unregistered DEV %s", name);
				ret = PX4_OK;
				توڑو;
			}
		}

		pthread_mutex_unlock(&devmutex);

		واپس ret;
	}

	صحیح px4_open(ثابت حرف *راستہ, صحیح flags, ...)
	{
		PX4_DEBUG("px4_open");
		cdev::CDev *dev = getDev(راستہ);
		صحیح ret = 0;
		صحیح i;
		mode_t منوال;

		اگر (!dev && (flags & PX4_F_WRONLY) != 0 &&
		    لڑی_موازنہ(راستہ, "/obj/", 5) != 0 &&
		    لڑی_موازنہ(راستہ, "/dev/", 5) != 0) {
			va_list p;
			va_آغاز(p, flags);
			منوال = va_arg(p, صحیح);
			va_انتہا(p);

			// Create the file
			PX4_DEBUG("Creating virtual file %s", راستہ);
			dev = نیا VFile(راستہ, منوال);
			رجسٹر_driver(راستہ, خالی_اشاریہ, 0666, (خالی *)dev);
		}

		اگر (dev) {
			pthread_mutex_lock(&filemutex);

			برائے (i = 0; i < PX4_MAX_FD; ++i) {
				اگر (filemap[i].cdev == خالی_اشاریہ) {
					filemap[i] = cdev::file_t(flags, dev);
					توڑو;
				}
			}

			pthread_mutex_unlock(&filemutex);

			اگر (i < PX4_MAX_FD) {
				ret = dev->open(&filemap[i]);

			} ورنہ {

				ثابت غیرعلامتی NAMELEN = 32;
				حرف thread_name[NAMELEN] {};

				PX4_WARN("%s: exceeded maximum number of file descriptors, accessing %s", thread_name, راستہ);
#اگر_غیر_تعریف_شدہ __PX4_QURT
				صحیح nret = pthread_getname_np(pthread_self(), thread_name, NAMELEN);

				اگر (nret || thread_name[0] == 0) {
					PX4_WARN("failed getting thread name");
				}

#اگر_ختم

				ret = -ENOENT;
			}

		} ورنہ {
			ret = -EINVAL;
		}

		اگر (ret < 0) {
			errno = -ret;
			واپس -1;
		}

		PX4_DEBUG("px4_open fd = %d", i);
		واپس i;
	}

	صحیح px4_close(صحیح fd)
	{
		صحیح ret;

		cdev::CDev *dev = getFile(fd);

		اگر (dev) {
			pthread_mutex_lock(&filemutex);
			ret = dev->close(&filemap[fd]);

			filemap[fd].cdev = خالی_اشاریہ;

			pthread_mutex_unlock(&filemutex);
			PX4_DEBUG("px4_close fd = %d", fd);

		} ورنہ {
			ret = -EINVAL;
		}

		اگر (ret < 0) {
			ret = PX4_ERROR;
		}

		واپس ret;
	}

	ssize_t px4_read(صحیح fd, خالی *بفر_2, size_t buflen)
	{
		صحیح ret;

		cdev::CDev *dev = getFile(fd);

		اگر (dev) {
			PX4_DEBUG("px4_read fd = %d", fd);
			ret = dev->اقرأ(&filemap[fd], (حرف *)بفر_2, buflen);

		} ورنہ {
			ret = -EINVAL;
		}

		اگر (ret < 0) {
			ret = PX4_ERROR;
		}

		واپس ret;
	}

	ssize_t px4_write(صحیح fd, ثابت خالی *بفر_2, size_t buflen)
	{
		صحیح ret;

		cdev::CDev *dev = getFile(fd);

		اگر (dev) {
			PX4_DEBUG("px4_write fd = %d", fd);
			ret = dev->اكتب(&filemap[fd], (ثابت حرف *)بفر_2, buflen);

		} ورنہ {
			ret = -EINVAL;
		}

		اگر (ret < 0) {
			ret = PX4_ERROR;
		}

		واپس ret;
	}

	صحیح px4_ioctl(صحیح fd, صحیح cmd, غیرعلامتی طویل arg)
	{
		PX4_DEBUG("px4_ioctl fd = %d", fd);
		صحیح ret = 0;

		cdev::CDev *dev = getFile(fd);

		اگر (dev) {
			ret = dev->ioctl(&filemap[fd], cmd, arg);

		} ورنہ {
			ret = -EINVAL;
		}

		واپس ret;
	}

	صحیح px4_poll(px4_pollfd_struct_t *fds, غیرعلامتی صحیح nfds, صحیح timeout)
	{
		اگر (nfds == 0) {
			PX4_WARN("px4_poll with no fds");
			واپس -1;
		}

		px4_sem_t sem;
		صحیح گنتی = 0;
		صحیح ret = -1;

		ثابت غیرعلامتی NAMELEN = 32;
		حرف thread_name[NAMELEN] {};

#اگر_غیر_تعریف_شدہ __PX4_QURT
		صحیح nret = pthread_getname_np(pthread_self(), thread_name, NAMELEN);

		اگر (nret || thread_name[0] == 0) {
			PX4_WARN("failed getting thread name");
		}

#اگر_ختم

		PX4_DEBUG("Called px4_poll timeout = %d", timeout);

		px4_sem_init(&sem, 0, 0);

		// sem use case is a signal
		px4_sem_setprotocol(&sem, SEM_PRIO_NONE);

		// Go through all fds and check them for a pollable state
		منطقی fd_pollable = جھوٹ;

		برائے (غیرعلامتی صحیح i = 0; i < nfds; ++i) {
			fds[i].sem     = &sem;
			fds[i].revents = 0;
			fds[i].priv    = خالی_اشاریہ;

			cdev::CDev *dev = getFile(fds[i].fd);

			// If fd is valid
			اگر (dev) {
				PX4_DEBUG("%s: px4_poll: CDev->poll(setup) %d", thread_name, fds[i].fd);
				ret = dev->poll(&filemap[fds[i].fd], &fds[i], سچ);

				اگر (ret < 0) {
					PX4_WARN("%s: px4_poll() error: %s", thread_name, strerror(errno));
					توڑو;
				}

				اگر (ret >= 0) {
					fd_pollable = سچ;
				}
			}
		}

		// If any FD can be polled, lock the semaphore and
		// check for new data
		اگر (fd_pollable) {
			اگر (timeout > 0) {
				// Get the current time
				ساخت timespec ts;
#اگر defined(__PX4_DARWIN) && !defined(ENABLE_LOCKSTEP_SCHEDULER)
				// macOS has no pthread_condattr_setclock(), so px4_sem_t's condition
				// variable waits on CLOCK_REALTIME (px4_sem_init()). Use that clock here
				// too. CLOCK_MONOTONIC counts from boot, so the deadline would land
				// decades in the past and px4_sem_timedwait() would return immediately.
				px4_clock_gettime(CLOCK_REALTIME, &ts);
#ورنہ
				// Note, we can't actually use CLOCK_MONOTONIC on macOS
				// but that's hidden and implemented in px4_clock_gettime.
				px4_clock_gettime(CLOCK_MONOTONIC, &ts);
#اگر_ختم

				// Calculate an absolute time in the future
				ثابت غیرعلامتی billion = (1000 * 1000 * 1000);
				uint64_t nsecs = ts.tv_nsec + ((uint64_t)timeout * 1000 * 1000);
				ts.tv_sec += nsecs / billion;
				nsecs -= (nsecs / billion) * billion;
				ts.tv_nsec = nsecs;

				ret = px4_sem_timedwait(&sem, &ts);

				اگر (ret && errno != ETIMEDOUT) {
					PX4_WARN("%s: px4_poll() sem error: %s", thread_name, strerror(errno));
				}

			} ورنہ اگر (timeout < 0) {
				px4_sem_wait(&sem);
			}

			// We have waited now (or not, depending on timeout),
			// go through all fds and count how many have data
			برائے (غیرعلامتی صحیح i = 0; i < nfds; ++i) {

				cdev::CDev *dev = getFile(fds[i].fd);

				// If fd is valid
				اگر (dev) {
					PX4_DEBUG("%s: px4_poll: CDev->poll(teardown) %d", thread_name, fds[i].fd);
					ret = dev->poll(&filemap[fds[i].fd], &fds[i], جھوٹ);

					اگر (ret < 0) {
						PX4_WARN("%s: px4_poll() 2nd poll fail", thread_name);
						توڑو;
					}

					اگر (fds[i].revents) {
						گنتی += 1;
					}
				}
			}
		}

		px4_sem_destroy(&sem);

		// Return the positive count if present,
		// return the negative error number if failed
		واپس (گنتی) ? گنتی : ret;
	}

	صحیح px4_access(ثابت حرف *pathname, صحیح منوال)
	{
		اگر (منوال != F_OK) {
			errno = EINVAL;
			واپس -1;
		}

		cdev::CDev *dev = getDev(pathname);
		واپس (dev != خالی_اشاریہ) ? 0 : -1;
	}

	خالی px4_show_files()
	{
		PX4_INFO("Files:");

		pthread_mutex_lock(&devmutex);

		برائے (ثابت خودکار &dev : devmap) {
			اگر (dev) {
				PX4_INFO_RAW("   %s\n", dev->name);
			}
		}

		pthread_mutex_unlock(&devmutex);
	}

} // extern "C"
