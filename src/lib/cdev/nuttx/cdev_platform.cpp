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
#അന്തര്ഭാവ <errno.h>

#അന്തര്ഭാവ "cdev_platform.hpp"
#അന്തര്ഭാവ "../CDev.hpp"

#അന്തര്ഭാവ <sys/ioctl.h>

#ifdef CONFIG_DISABLE_POLL
# error This driver is not compatible with CONFIG_DISABLE_POLL
#endif

നാമാകാശ cdev
{

/*
 * The standard NuttX operation dispatch table can't call C++ member functions
 * directly, so we have to bounce them through this dispatch table.
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ	cdev_open(file_t *filp);
സ്ഥിതം പൂർണ്ണസംഖ്യ	cdev_close(file_t *filp);
സ്ഥിതം ssize_t	cdev_read(file_t *filp, അക്ഷരം *buffer, size_t buflen);
സ്ഥിതം ssize_t	cdev_write(file_t *filp, സ്ഥിരം അക്ഷരം *buffer, size_t buflen);
സ്ഥിതം off_t	cdev_seek(file_t *filp, off_t offset, പൂർണ്ണസംഖ്യ whence);
സ്ഥിതം പൂർണ്ണസംഖ്യ	cdev_ioctl(file_t *filp, പൂർണ്ണസംഖ്യ cmd, unsigned long arg);
സ്ഥിതം പൂർണ്ണസംഖ്യ	cdev_poll(file_t *filp, px4_pollfd_struct_t *fds, തർക്കം setup);

/**
 * Character device indirection table.
 *
 * Every cdev we register gets the same function table; we use the private data
 * field in the inode to store the instance pointer.
 *
 * Note that we use the GNU extension syntax here because we don't get designated
 * initialisers in gcc 4.6.
 */
സ്ഥിതം സ്ഥിരം ഘടന file_operations g_fops = {
open	: cdev_open,
close	: cdev_close,
പഠ	: cdev_read,
ലിഖ	: cdev_write,
seek	: cdev_seek,
ioctl	: cdev_ioctl,
poll	: cdev_poll,
#ifndef CONFIG_DISABLE_PSEUDOFS_OPERATIONS
unlink	: nullptr
#endif
};

സ്ഥിരം cdev::px4_file_operations_t &cdev::CDev::fops_ref() { തിരികെ g_fops; }

സ്ഥിതം പൂർണ്ണസംഖ്യ
cdev_open(file_t *filp)
{
	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_private);

	തിരികെ cdev->open(filp);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
cdev_close(file_t *filp)
{
	എങ്കിൽ ((filp->f_inode->i_crefs) == 0) {
		തിരികെ -ENODEV;
	}

	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_private);

	തിരികെ cdev->close(filp);
}

സ്ഥിതം ssize_t
cdev_read(file_t *filp, അക്ഷരം *buffer, size_t buflen)
{
	എങ്കിൽ ((filp->f_inode->i_crefs) == 0) {
		തിരികെ -ENODEV;
	}

	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_private);

	തിരികെ cdev->പഠ(filp, buffer, buflen);
}

സ്ഥിതം ssize_t
cdev_write(file_t *filp, സ്ഥിരം അക്ഷരം *buffer, size_t buflen)
{
	എങ്കിൽ ((filp->f_inode->i_crefs) == 0) {
		തിരികെ -ENODEV;
	}

	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_private);

	തിരികെ cdev->ലിഖ(filp, buffer, buflen);
}

സ്ഥിതം off_t
cdev_seek(file_t *filp, off_t offset, പൂർണ്ണസംഖ്യ whence)
{
	എങ്കിൽ ((filp->f_inode->i_crefs) == 0) {
		തിരികെ -ENODEV;
	}

	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_private);

	തിരികെ cdev->seek(filp, offset, whence);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
cdev_ioctl(file_t *filp, പൂർണ്ണസംഖ്യ cmd, unsigned long arg)
{
	എങ്കിൽ ((filp->f_inode->i_crefs) == 0) {
		തിരികെ -ENODEV;
	}

	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_private);

	തിരികെ cdev->ioctl(filp, cmd, arg);
}

സ്ഥിതം പൂർണ്ണസംഖ്യ
cdev_poll(file_t *filp, px4_pollfd_struct_t *fds, തർക്കം setup)
{
	എങ്കിൽ ((filp->f_inode->i_crefs) == 0) {
		തിരികെ -ENODEV;
	}

	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_private);

	തിരികെ cdev->poll(filp, fds, setup);
}

} // namespace cdev
