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
#ضمن <errno.h>

#ضمن "cdev_platform.hpp"
#ضمن "../CDev.hpp"

#ضمن <sys/ioctl.h>

#ifdef CONFIG_DISABLE_POLL
# error This driver is not compatible with CONFIG_DISABLE_POLL
#endif

نطاق cdev
{

/*
 * The standard NuttX operation dispatch table can't call C++ member functions
 * directly, so we have to bounce them through this dispatch table.
 */
ساكن صحيح	cdev_open(file_t *filp);
ساكن صحيح	cdev_close(file_t *filp);
ساكن ssize_t	cdev_read(file_t *filp, حرف *buffer, size_t buflen);
ساكن ssize_t	cdev_write(file_t *filp, ثابت حرف *buffer, size_t buflen);
ساكن off_t	cdev_seek(file_t *filp, off_t offset, صحيح whence);
ساكن صحيح	cdev_ioctl(file_t *filp, صحيح cmd, unsigned long arg);
ساكن صحيح	cdev_poll(file_t *filp, px4_pollfd_struct_t *fds, منطقي setup);

/**
 * Character device indirection table.
 *
 * Every cdev we register gets the same function table; we use the private data
 * field in the inode to store the instance pointer.
 *
 * Note that we use the GNU extension syntax here because we don't get designated
 * initialisers in gcc 4.6.
 */
ساكن ثابت بنية file_operations g_fops = {
open	: cdev_open,
close	: cdev_close,
اقرأ	: cdev_read,
اكتب	: cdev_write,
seek	: cdev_seek,
ioctl	: cdev_ioctl,
poll	: cdev_poll,
#ifndef CONFIG_DISABLE_PSEUDOFS_OPERATIONS
unlink	: nullptr
#endif
};

ثابت cdev::px4_file_operations_t &cdev::CDev::fops_ref() { أرجع g_fops; }

ساكن صحيح
cdev_open(file_t *filp)
{
	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_private);

	أرجع cdev->open(filp);
}

ساكن صحيح
cdev_close(file_t *filp)
{
	إذا ((filp->f_inode->i_crefs) == 0) {
		أرجع -ENODEV;
	}

	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_private);

	أرجع cdev->close(filp);
}

ساكن ssize_t
cdev_read(file_t *filp, حرف *buffer, size_t buflen)
{
	إذا ((filp->f_inode->i_crefs) == 0) {
		أرجع -ENODEV;
	}

	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_private);

	أرجع cdev->اقرأ(filp, buffer, buflen);
}

ساكن ssize_t
cdev_write(file_t *filp, ثابت حرف *buffer, size_t buflen)
{
	إذا ((filp->f_inode->i_crefs) == 0) {
		أرجع -ENODEV;
	}

	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_private);

	أرجع cdev->اكتب(filp, buffer, buflen);
}

ساكن off_t
cdev_seek(file_t *filp, off_t offset, صحيح whence)
{
	إذا ((filp->f_inode->i_crefs) == 0) {
		أرجع -ENODEV;
	}

	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_private);

	أرجع cdev->seek(filp, offset, whence);
}

ساكن صحيح
cdev_ioctl(file_t *filp, صحيح cmd, unsigned long arg)
{
	إذا ((filp->f_inode->i_crefs) == 0) {
		أرجع -ENODEV;
	}

	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_private);

	أرجع cdev->ioctl(filp, cmd, arg);
}

ساكن صحيح
cdev_poll(file_t *filp, px4_pollfd_struct_t *fds, منطقي setup)
{
	إذا ((filp->f_inode->i_crefs) == 0) {
		أرجع -ENODEV;
	}

	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_private);

	أرجع cdev->poll(filp, fds, setup);
}

} // namespace cdev
