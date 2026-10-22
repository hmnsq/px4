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
#अन्तर्भाव <errno.h>

#अन्तर्भाव "cdev_platform.hpp"
#अन्तर्भाव "../CDev.hpp"

#अन्तर्भाव <sys/ioctl.h>

#ifdef CONFIG_DISABLE_POLL
# error This driver is not compatible with CONFIG_DISABLE_POLL
#endif

नामाकाश cdev
{

/*
 * The standard NuttX operation dispatch table can't call C++ member functions
 * directly, so we have to bounce them through this dispatch table.
 */
स्थिर पूर्णांक	cdev_open(file_t *filp);
स्थिर पूर्णांक	cdev_close(file_t *filp);
स्थिर ssize_t	cdev_read(file_t *filp, अक्षर *buffer, माप_प्रकार buflen);
स्थिर ssize_t	cdev_write(file_t *filp, स्थिरांक अक्षर *buffer, माप_प्रकार buflen);
स्थिर off_t	cdev_seek(file_t *filp, off_t offset, पूर्णांक whence);
स्थिर पूर्णांक	cdev_ioctl(file_t *filp, पूर्णांक cmd, अचिन्हित दीर्घ arg);
स्थिर पूर्णांक	cdev_poll(file_t *filp, px4_pollfd_struct_t *fds, तार्किक setup);

/**
 * Character device indirection table.
 *
 * Every cdev we register gets the same function table; we use the private data
 * field in the inode to store the instance pointer.
 *
 * Note that we use the GNU extension syntax here because we don't get designated
 * initialisers in gcc 4.6.
 */
स्थिर स्थिरांक संरचना file_operations g_fops = {
खोलो	: cdev_open,
बंद	: cdev_close,
पठ	: cdev_read,
लिख	: cdev_write,
seek	: cdev_seek,
ioctl	: cdev_ioctl,
poll	: cdev_poll,
#ifndef CONFIG_DISABLE_PSEUDOFS_OPERATIONS
unlink	: nullptr
#endif
};

स्थिरांक cdev::px4_file_operations_t &cdev::CDev::fops_ref() { परत g_fops; }

स्थिर पूर्णांक
cdev_open(file_t *filp)
{
	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_private);

	परत cdev->खोलो(filp);
}

स्थिर पूर्णांक
cdev_close(file_t *filp)
{
	जर ((filp->f_inode->i_crefs) == 0) {
		परत -ENODEV;
	}

	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_private);

	परत cdev->बंद(filp);
}

स्थिर ssize_t
cdev_read(file_t *filp, अक्षर *buffer, माप_प्रकार buflen)
{
	जर ((filp->f_inode->i_crefs) == 0) {
		परत -ENODEV;
	}

	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_private);

	परत cdev->पठ(filp, buffer, buflen);
}

स्थिर ssize_t
cdev_write(file_t *filp, स्थिरांक अक्षर *buffer, माप_प्रकार buflen)
{
	जर ((filp->f_inode->i_crefs) == 0) {
		परत -ENODEV;
	}

	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_private);

	परत cdev->लिख(filp, buffer, buflen);
}

स्थिर off_t
cdev_seek(file_t *filp, off_t offset, पूर्णांक whence)
{
	जर ((filp->f_inode->i_crefs) == 0) {
		परत -ENODEV;
	}

	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_private);

	परत cdev->seek(filp, offset, whence);
}

स्थिर पूर्णांक
cdev_ioctl(file_t *filp, पूर्णांक cmd, अचिन्हित दीर्घ arg)
{
	जर ((filp->f_inode->i_crefs) == 0) {
		परत -ENODEV;
	}

	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_private);

	परत cdev->ioctl(filp, cmd, arg);
}

स्थिर पूर्णांक
cdev_poll(file_t *filp, px4_pollfd_struct_t *fds, तार्किक setup)
{
	जर ((filp->f_inode->i_crefs) == 0) {
		परत -ENODEV;
	}

	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_private);

	परत cdev->poll(filp, fds, setup);
}

} // namespace cdev
