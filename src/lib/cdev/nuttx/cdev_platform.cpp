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

#यदि_परिभाषितम् CONFIG_DISABLE_POLL
# error This driver is not compatible with CONFIG_DISABLE_POLL
#यदि_अन्तः

नामाकाश cdev
{

/*
 * The standard NuttX operation dispatch table can't call C++ member functions
 * directly, so we have to bounce them through this dispatch table.
 */
स्थिर पूर्णाङ्क	cdev_open(file_t *filp);
स्थिर पूर्णाङ्क	cdev_close(file_t *filp);
स्थिर ssize_t	cdev_read(file_t *filp, वर्ण *बफर_2, size_t buflen);
स्थिर ssize_t	cdev_write(file_t *filp, ध्रुव वर्ण *बफर_2, size_t buflen);
स्थिर off_t	cdev_seek(file_t *filp, off_t विस्थापनम्, पूर्णाङ्क whence);
स्थिर पूर्णाङ्क	cdev_ioctl(file_t *filp, पूर्णाङ्क cmd, अचिह्नित दीर्घ arg);
स्थिर पूर्णाङ्क	cdev_poll(file_t *filp, px4_pollfd_struct_t *fds, तार्किक सज्जता);

/**
 * Character device indirection table.
 *
 * Every cdev we register gets the same function table; we use the private data
 * field in the inode to store the instance pointer.
 *
 * Note that we use the GNU extension syntax here because we don't get designated
 * initialisers in gcc 4.6.
 */
स्थिर ध्रुव संरचना file_operations g_fops = {
उद्घाटय	: cdev_open,
close	: cdev_close,
पठ	: cdev_read,
लिख	: cdev_write,
seek	: cdev_seek,
ioctl	: cdev_ioctl,
poll	: cdev_poll,
#यदि_अपरिभाषितम् CONFIG_DISABLE_PSEUDOFS_OPERATIONS
unlink	: शून्यसूचकः
#यदि_अन्तः
};

ध्रुव cdev::px4_file_operations_t &cdev::CDev::fops_ref() { प्रत्यागम g_fops; }

स्थिर पूर्णाङ्क
cdev_open(file_t *filp)
{
	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_वैयक्तिकम्);

	प्रत्यागम cdev->उद्घाटय(filp);
}

स्थिर पूर्णाङ्क
cdev_close(file_t *filp)
{
	यदि ((filp->f_inode->i_crefs) == 0) {
		प्रत्यागम -ENODEV;
	}

	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_वैयक्तिकम्);

	प्रत्यागम cdev->close(filp);
}

स्थिर ssize_t
cdev_read(file_t *filp, वर्ण *बफर_2, size_t buflen)
{
	यदि ((filp->f_inode->i_crefs) == 0) {
		प्रत्यागम -ENODEV;
	}

	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_वैयक्तिकम्);

	प्रत्यागम cdev->पठ(filp, बफर_2, buflen);
}

स्थिर ssize_t
cdev_write(file_t *filp, ध्रुव वर्ण *बफर_2, size_t buflen)
{
	यदि ((filp->f_inode->i_crefs) == 0) {
		प्रत्यागम -ENODEV;
	}

	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_वैयक्तिकम्);

	प्रत्यागम cdev->लिख(filp, बफर_2, buflen);
}

स्थिर off_t
cdev_seek(file_t *filp, off_t विस्थापनम्, पूर्णाङ्क whence)
{
	यदि ((filp->f_inode->i_crefs) == 0) {
		प्रत्यागम -ENODEV;
	}

	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_वैयक्तिकम्);

	प्रत्यागम cdev->seek(filp, विस्थापनम्, whence);
}

स्थिर पूर्णाङ्क
cdev_ioctl(file_t *filp, पूर्णाङ्क cmd, अचिह्नित दीर्घ arg)
{
	यदि ((filp->f_inode->i_crefs) == 0) {
		प्रत्यागम -ENODEV;
	}

	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_वैयक्तिकम्);

	प्रत्यागम cdev->ioctl(filp, cmd, arg);
}

स्थिर पूर्णाङ्क
cdev_poll(file_t *filp, px4_pollfd_struct_t *fds, तार्किक सज्जता)
{
	यदि ((filp->f_inode->i_crefs) == 0) {
		प्रत्यागम -ENODEV;
	}

	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_वैयक्तिकम्);

	प्रत्यागम cdev->poll(filp, fds, सज्जता);
}

} // namespace cdev
