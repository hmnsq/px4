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
#include <errno.h>

#include "cdev_platform.hpp"
#include "../CDev.hpp"

#include <sys/ioctl.h>

#ifdef CONFIG_DISABLE_POLL
# error This driver is not compatible with CONFIG_DISABLE_POLL
#endif

namespace cdev
{

/*
 * The standard NuttX operation dispatch table can't call C++ member functions
 * directly, so we have to bounce them through this dispatch table.
 */
static ကိန်းပြည့်	cdev_open(file_t *filp);
static ကိန်းပြည့်	cdev_close(file_t *filp);
static ssize_t	cdev_read(file_t *filp, var_na *buffer, size_t buflen);
static ssize_t	cdev_write(file_t *filp, const var_na *buffer, size_t buflen);
static off_t	cdev_seek(file_t *filp, off_t offset, ကိန်းပြည့် whence);
static ကိန်းပြည့်	cdev_ioctl(file_t *filp, ကိန်းပြည့် cmd, unsigned long arg);
static ကိန်းပြည့်	cdev_poll(file_t *filp, px4_pollfd_struct_t *fds, bool setup);

/**
 * Character device indirection table.
 *
 * Every cdev we register gets the same function table; we use the private data
 * field in the inode to store the instance pointer.
 *
 * Note that we use the GNU extension syntax here because we don't get designated
 * initialisers in gcc 4.6.
 */
static const sa_nracanaa file_operations g_fops = {
open	: cdev_open,
close	: cdev_close,
read	: cdev_read,
write	: cdev_write,
seek	: cdev_seek,
ioctl	: cdev_ioctl,
poll	: cdev_poll,
#ifndef CONFIG_DISABLE_PSEUDOFS_OPERATIONS
unlink	: nullptr
#endif
};

const cdev::px4_file_operations_t &cdev::CDev::fops_ref() { ပြန်ပေး g_fops; }

static ကိန်းပြည့်
cdev_open(file_t *filp)
{
	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_private);

	ပြန်ပေး cdev->open(filp);
}

static ကိန်းပြည့်
cdev_close(file_t *filp)
{
	အကယ်၍ ((filp->f_inode->i_crefs) == 0) {
		ပြန်ပေး -ENODEV;
	}

	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_private);

	ပြန်ပေး cdev->close(filp);
}

static ssize_t
cdev_read(file_t *filp, var_na *buffer, size_t buflen)
{
	အကယ်၍ ((filp->f_inode->i_crefs) == 0) {
		ပြန်ပေး -ENODEV;
	}

	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_private);

	ပြန်ပေး cdev->read(filp, buffer, buflen);
}

static ssize_t
cdev_write(file_t *filp, const var_na *buffer, size_t buflen)
{
	အကယ်၍ ((filp->f_inode->i_crefs) == 0) {
		ပြန်ပေး -ENODEV;
	}

	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_private);

	ပြန်ပေး cdev->write(filp, buffer, buflen);
}

static off_t
cdev_seek(file_t *filp, off_t offset, ကိန်းပြည့် whence)
{
	အကယ်၍ ((filp->f_inode->i_crefs) == 0) {
		ပြန်ပေး -ENODEV;
	}

	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_private);

	ပြန်ပေး cdev->seek(filp, offset, whence);
}

static ကိန်းပြည့်
cdev_ioctl(file_t *filp, ကိန်းပြည့် cmd, unsigned long arg)
{
	အကယ်၍ ((filp->f_inode->i_crefs) == 0) {
		ပြန်ပေး -ENODEV;
	}

	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_private);

	ပြန်ပေး cdev->ioctl(filp, cmd, arg);
}

static ကိန်းပြည့်
cdev_poll(file_t *filp, px4_pollfd_struct_t *fds, bool setup)
{
	အကယ်၍ ((filp->f_inode->i_crefs) == 0) {
		ပြန်ပေး -ENODEV;
	}

	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_private);

	ပြန်ပေး cdev->poll(filp, fds, setup);
}

} // namespace cdev
