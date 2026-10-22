/****************************************************************************
 *
 *   Copyright (c) 2012-2021 PX4 Development Team. All rights reserved.
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
 * @file cdev_platform.cpp
 *
 * NuttX Character device functions
 */
#ಅನ್ತರ್ಭಾವ <errno.h>

#ಅನ್ತರ್ಭಾವ "cdev_platform.hpp"
#ಅನ್ತರ್ಭಾವ "../CDev.hpp"

#ಅನ್ತರ್ಭಾವ <sys/ioctl.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CONFIG_DISABLE_POLL
# error This driver is not compatible with CONFIG_DISABLE_POLL
#ಆದರೆ_ಅಂತ್ಯ

ನಾಮಾಕಾಶ cdev
{

/*
 * The standard NuttX operation dispatch table can't call C++ member functions
 * directly, so we have to bounce them through this dispatch table.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	cdev_open(file_t *filp);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	cdev_close(file_t *filp);
ಸ್ಥಿರ ssize_t	cdev_read(file_t *filp, ಅಕ್ಷರ *ಬಫರ್_2, size_t buflen);
ಸ್ಥಿರ ssize_t	cdev_write(file_t *filp, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಫರ್_2, size_t buflen);
ಸ್ಥಿರ off_t	cdev_seek(file_t *filp, off_t ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ whence);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	cdev_ioctl(file_t *filp, ಪೂರ್ಣಾಂಕ cmd, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ arg);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	cdev_poll(file_t *filp, px4_pollfd_struct_t *fds, ತಾರ್ಕಿಕ ಸಿದ್ಧತೆ);

/**
 * Character device indirection table.
 *
 * Every cdev we register gets the same function table; we use the private data
 * field in the inode to store the instance pointer.
 *
 * Note that we use the GNU extension syntax here because we don't get designated
 * initialisers in gcc 4.6.
 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ file_operations g_fops = {
open	: cdev_open,
close	: cdev_close,
ಪಠ	: cdev_read,
ಲಿಖ	: cdev_write,
seek	: cdev_seek,
ioctl	: cdev_ioctl,
poll	: cdev_poll,
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CONFIG_DISABLE_PSEUDOFS_OPERATIONS
unlink	: ಶೂನ್ಯಸೂಚಕ
#ಆದರೆ_ಅಂತ್ಯ
};

ಸ್ಥಿರಾಂಕ cdev::px4_file_operations_t &cdev::CDev::fops_ref() { ಹಿಂತಿರುಗಿ g_fops; }

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cdev_open(file_t *filp)
{
	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_ಖಾಸಗಿ);

	ಹಿಂತಿರುಗಿ cdev->open(filp);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cdev_close(file_t *filp)
{
	ಆದರೆ ((filp->f_inode->i_crefs) == 0) {
		ಹಿಂತಿರುಗಿ -ENODEV;
	}

	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_ಖಾಸಗಿ);

	ಹಿಂತಿರುಗಿ cdev->close(filp);
}

ಸ್ಥಿರ ssize_t
cdev_read(file_t *filp, ಅಕ್ಷರ *ಬಫರ್_2, size_t buflen)
{
	ಆದರೆ ((filp->f_inode->i_crefs) == 0) {
		ಹಿಂತಿರುಗಿ -ENODEV;
	}

	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_ಖಾಸಗಿ);

	ಹಿಂತಿರುಗಿ cdev->ಪಠ(filp, ಬಫರ್_2, buflen);
}

ಸ್ಥಿರ ssize_t
cdev_write(file_t *filp, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಫರ್_2, size_t buflen)
{
	ಆದರೆ ((filp->f_inode->i_crefs) == 0) {
		ಹಿಂತಿರುಗಿ -ENODEV;
	}

	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_ಖಾಸಗಿ);

	ಹಿಂತಿರುಗಿ cdev->ಲಿಖ(filp, ಬಫರ್_2, buflen);
}

ಸ್ಥಿರ off_t
cdev_seek(file_t *filp, off_t ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ whence)
{
	ಆದರೆ ((filp->f_inode->i_crefs) == 0) {
		ಹಿಂತಿರುಗಿ -ENODEV;
	}

	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_ಖಾಸಗಿ);

	ಹಿಂತಿರುಗಿ cdev->seek(filp, ಸ್ಥಾನಾಂತರ, whence);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cdev_ioctl(file_t *filp, ಪೂರ್ಣಾಂಕ cmd, ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ arg)
{
	ಆದರೆ ((filp->f_inode->i_crefs) == 0) {
		ಹಿಂತಿರುಗಿ -ENODEV;
	}

	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_ಖಾಸಗಿ);

	ಹಿಂತಿರುಗಿ cdev->ioctl(filp, cmd, arg);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cdev_poll(file_t *filp, px4_pollfd_struct_t *fds, ತಾರ್ಕಿಕ ಸಿದ್ಧತೆ)
{
	ಆದರೆ ((filp->f_inode->i_crefs) == 0) {
		ಹಿಂತಿರುಗಿ -ENODEV;
	}

	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_ಖಾಸಗಿ);

	ಹಿಂತಿರುಗಿ cdev->poll(filp, fds, ಸಿದ್ಧತೆ);
}

} // namespace cdev
