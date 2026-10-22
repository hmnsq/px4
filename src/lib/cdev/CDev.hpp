/****************************************************************************
 *
 *   Copyright (c) 2012-2018 PX4 Development Team. All rights reserved.
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

/**
 * @file CDev.hpp
 *
 * Definitions for the generic base classes in the device framework.
 */

#ifndef _CDEV_HPP
#define _CDEV_HPP

#ಅನ್ತರ್ಭಾವ <px4_platform_common/px4_config.h>
#ಅನ್ತರ್ಭಾವ <px4_platform_common/posix.h>

#ifdef __PX4_NUTTX
#ಅನ್ತರ್ಭಾವ "nuttx/cdev_platform.hpp"
#ಅನ್ಯಥಾ
#ಅನ್ತರ್ಭಾವ "posix/cdev_platform.hpp"
#endif

ನಾಮಾಕಾಶ cdev
{

/**
 * Abstract class for any character device
 */
ವರ್ಗ __EXPORT CDev
{
ಸಾರ್ವಜನಿಕ:
	/**
	 * Constructor
	 *
	 * @param name		Driver name
	 * @param devname	Device node name
	 */
	explicit CDev(ಧ್ರುವ ವರ್ಣ *devname);

	// no copy, assignment, move, move assignment
	CDev(ಧ್ರುವ CDev &) = delete;
	CDev &operator=(ಧ್ರುವ CDev &) = delete;
	CDev(CDev &&) = delete;
	CDev &operator=(CDev &&) = delete;

	ಆಭಾಸ ~CDev();

	ಆಭಾಸ ಪೂರ್ಣಾಙ್ಕ	init();

	/**
	 * Handle an open of the device.
	 *
	 * This function is called for every open of the device. The default
	 * implementation maintains _open_count and always returns OK.
	 *
	 * @param filep		Pointer to the NuttX file structure.
	 * @return		OK if the open is allowed, -errno otherwise.
	 */
	ಆಭಾಸ ಪೂರ್ಣಾಙ್ಕ	open(file_t *filep);

	/**
	 * Handle a close of the device.
	 *
	 * This function is called for every close of the device. The default
	 * implementation maintains _open_count and returns OK as long as it is not zero.
	 *
	 * @param filep		Pointer to the NuttX file structure.
	 * @return		OK if the close was successful, -errno otherwise.
	 */
	ಆಭಾಸ ಪೂರ್ಣಾಙ್ಕ	close(file_t *filep);

	/**
	 * Perform a read from the device.
	 *
	 * The default implementation returns -ENOSYS.
	 *
	 * @param filep		Pointer to the NuttX file structure.
	 * @param buffer	Pointer to the buffer into which data should be placed.
	 * @param buflen	The number of bytes to be read.
	 * @return		The number of bytes read or -errno otherwise.
	 */
	ಆಭಾಸ ssize_t	ಪಠ(file_t *filep, ವರ್ಣ *buffer, size_t buflen) { ಪ್ರತ್ಯಾಗಮ -ENOSYS; }

	/**
	 * Perform a write to the device.
	 *
	 * The default implementation returns -ENOSYS.
	 *
	 * @param filep		Pointer to the NuttX file structure.
	 * @param buffer	Pointer to the buffer from which data should be read.
	 * @param buflen	The number of bytes to be written.
	 * @return		The number of bytes written or -errno otherwise.
	 */
	ಆಭಾಸ ssize_t	ಲಿಖ(file_t *filep, ಧ್ರುವ ವರ್ಣ *buffer, size_t buflen) { ಪ್ರತ್ಯಾಗಮ -ENOSYS; }

	/**
	 * Perform a logical seek operation on the device.
	 *
	 * The default implementation returns -ENOSYS.
	 *
	 * @param filep		Pointer to the NuttX file structure.
	 * @param offset	The new file position relative to whence.
	 * @param whence	SEEK_OFS, SEEK_CUR or SEEK_END.
	 * @return		The previous offset, or -errno otherwise.
	 */
	ಆಭಾಸ off_t	seek(file_t *filep, off_t offset, ಪೂರ್ಣಾಙ್ಕ whence) { ಪ್ರತ್ಯಾಗಮ -ENOSYS; }

	/**
	 * Perform an ioctl operation on the device.
	 *
	 * The default implementation returns -ENOTTY.
	 * Subclasses should call the default implementation
	 * for any command they do not handle themselves.
	 *
	 * @param filep		Pointer to the NuttX file structure.
	 * @param cmd		The ioctl command value.
	 * @param arg		The ioctl argument value.
	 * @return		OK on success, or -errno otherwise.
	 */
	ಆಭಾಸ ಪೂರ್ಣಾಙ್ಕ	ioctl(file_t *filep, ಪೂರ್ಣಾಙ್ಕ cmd, unsigned long arg) { ಪ್ರತ್ಯಾಗಮ -ENOTTY; };

	/**
	 * Perform a poll setup/teardown operation.
	 *
	 * This is handled internally and should not normally be overridden.
	 *
	 * @param filep		Pointer to the internal file structure.
	 * @param fds		Poll descriptor being waited on.
	 * @param setup		True if this is establishing a request, false if
	 *			it is being torn down.
	 * @return		OK on success, or -errno otherwise.
	 */
	ಪೂರ್ಣಾಙ್ಕ	poll(file_t *filep, px4_pollfd_struct_t *fds, ತಾರ್ಕಿಕ setup);

	/**
	 * Get the device name.
	 *
	 * @return the file system string of the device handle
	 */
	ಧ್ರುವ ವರ್ಣ	*get_devname() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _devname; }

ರಕ್ಷಿತ:
	/**
	 * Accessor for the default cdev file operations table; useful for
	 * registering clone devices etc. Defined in the platform-specific
	 * cdev_platform.cpp.
	 */
	ಸ್ಥಿರ ಧ್ರುವ px4_file_operations_t	&fops_ref();

	/**
	 * Check the current state of the device for poll events from the
	 * perspective of the file.
	 *
	 * This function is called by the default poll() implementation when
	 * a poll is set up to determine whether the poll should return immediately.
	 *
	 * The default implementation returns no events.
	 *
	 * @param filep		The file that's interested.
	 * @return		The current set of poll events.
	 */
	ಆಭಾಸ px4_pollevent_t poll_state(file_t *filep) { ಪ್ರತ್ಯಾಗಮ 0; }

	/**
	 * Report new poll events.
	 *
	 * This function should be called anytime the state of the device changes
	 * in a fashion that might be interesting to a poll waiter.
	 *
	 * @param events	The new event(s) being announced.
	 */
	ಶೂನ್ಯ	poll_notify(px4_pollevent_t events);

	/**
	 * Internal implementation of poll_notify.
	 *
	 * @param fds		A poll waiter to notify.
	 * @param events	The event(s) to send to the waiter.
	 */
	ಆಭಾಸ ಶೂನ್ಯ	poll_notify_one(px4_pollfd_struct_t *fds, px4_pollevent_t events);

	/**
	 * Notification of the first open.
	 *
	 * This function is called when the device open count transitions from zero
	 * to one.  The driver lock is held for the duration of the call.
	 *
	 * The default implementation returns OK.
	 *
	 * @param filep		Pointer to the NuttX file structure.
	 * @return		OK if the open should proceed, -errno otherwise.
	 */
	ಆಭಾಸ ಪೂರ್ಣಾಙ್ಕ	open_first(file_t *filep) { ಪ್ರತ್ಯಾಗಮ PX4_OK; }

	/**
	 * Notification of the last close.
	 *
	 * This function is called when the device open count transitions from
	 * one to zero.  The driver lock is held for the duration of the call.
	 *
	 * The default implementation returns OK.
	 *
	 * @param filep		Pointer to the NuttX file structure.
	 * @return		OK if the open should return OK, -errno otherwise.
	 */
	ಆಭಾಸ ಪೂರ್ಣಾಙ್ಕ	close_last(file_t *filep) { ಪ್ರತ್ಯಾಗಮ PX4_OK; }

	/**
	 * Register a class device name, automatically adding device
	 * class instance suffix if need be.
	 *
	 * @param class_devname   Device class name
	 * @return class_instamce Class instance created, or -errno on failure
	 */
	ಪೂರ್ಣಾಙ್ಕ register_class_devname(ಧ್ರುವ ವರ್ಣ *class_devname);

	/**
	 * Register a class device name, automatically adding device
	 * class instance suffix if need be.
	 *
	 * @param class_devname   Device class name
	 * @param class_instance  Device class instance from register_class_devname()
	 * @return		  OK on success, -errno otherwise
	 */
	ಪೂರ್ಣಾಙ್ಕ unregister_class_devname(ಧ್ರುವ ವರ್ಣ *class_devname, unsigned class_instance);

	/**
	 * Take the driver lock.
	 *
	 * Each driver instance has its own lock/semaphore.
	 *
	 * Note that we must loop as the wait may be interrupted by a signal.
	 *
	 * Careful: lock() calls cannot be nested!
	 */
	ಶೂನ್ಯ		lock() { ಕುರು {} ಯಾವತ್ (px4_sem_wait(&_lock) != 0); }

	/**
	 * Release the driver lock.
	 */
	ಶೂನ್ಯ		unlock() { px4_sem_post(&_lock); }

	/**
	 * True once any poll waiter has been registered. Read it under
	 * ATOMIC_ENTER: poll() setup stores the waiter and then checks
	 * poll_state() under the same section, so a waiter that registers
	 * after a false reading sees the state change itself.
	 */
	ತಾರ್ಕಿಕ		has_poll_waiters() ಧ್ರುವ { ಪ್ರತ್ಯಾಗಮ _max_pollwaiters > 0; }

	px4_sem_t	_lock; /**< lock to protect access to all class members (also for derived classes) */

ಗುಪ್ತ:
	ಧ್ರುವ ವರ್ಣ	*_devname{nullptr};		/**< device node name */

	px4_pollfd_struct_t	**_pollset{nullptr};

	ತಾರ್ಕಿಕ		_registered{ಅಸತ್ಯ};		/**< true if device name was registered */

	uint8_t		_max_pollwaiters{0};		/**< size of the _pollset array */
	uint16_t	open_ಎಣಿಕೆ{0};			/**< number of successful opens */

	/**
	 * Store a pollwaiter in a slot where we can find it later.
	 *
	 * Expands the pollset as required.  Must be called with the driver locked.
	 *
	 * @return		OK, or -errno on error.
	 */
	ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ	store_poll_waiter(px4_pollfd_struct_t *fds);

	/**
	 * Remove a poll waiter.
	 *
	 * @return		OK, or -errno on error.
	 */
	ಅನ್ತಃಸ್ಥ ಪೂರ್ಣಾಙ್ಕ	remove_poll_waiter(px4_pollfd_struct_t *fds);

};

} // namespace cdev

#endif /* _CDEV_HPP */
