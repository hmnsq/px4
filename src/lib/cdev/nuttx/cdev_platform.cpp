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
#అన్తర్భావ <errno.h>

#అన్తర్భావ "cdev_platform.hpp"
#అన్తర్భావ "../CDev.hpp"

#అన్తర్భావ <sys/ioctl.h>

#నిర్వచితమైతే CONFIG_DISABLE_POLL
# error This driver is not compatible with CONFIG_DISABLE_POLL
#అయితే_ముగింపు

నామాకాశ cdev
{

/*
 * The standard NuttX operation dispatch table can't call C++ member functions
 * directly, so we have to bounce them through this dispatch table.
 */
స్థిర పూర్ణాంకం	cdev_open(file_t *filp);
స్థిర పూర్ణాంకం	cdev_close(file_t *filp);
స్థిర ssize_t	cdev_read(file_t *filp, అక్షరం *బఫరు_2, size_t buflen);
స్థిర ssize_t	cdev_write(file_t *filp, స్థిరరాశి అక్షరం *బఫరు_2, size_t buflen);
స్థిర off_t	cdev_seek(file_t *filp, off_t స్థానభ్రంశం, పూర్ణాంకం whence);
స్థిర పూర్ణాంకం	cdev_ioctl(file_t *filp, పూర్ణాంకం cmd, గుర్తులేని దీర్ఘ arg);
స్థిర పూర్ణాంకం	cdev_poll(file_t *filp, px4_pollfd_struct_t *fds, తార్కిక అమరిక);

/**
 * Character device indirection table.
 *
 * Every cdev we register gets the same function table; we use the private data
 * field in the inode to store the instance pointer.
 *
 * Note that we use the GNU extension syntax here because we don't get designated
 * initialisers in gcc 4.6.
 */
స్థిర స్థిరరాశి నిర్మాణం file_operations g_fops = {
open	: cdev_open,
close	: cdev_close,
పఠ	: cdev_read,
లిఖ	: cdev_write,
seek	: cdev_seek,
ioctl	: cdev_ioctl,
poll	: cdev_poll,
#నిర్వచితం_కాకపోతే CONFIG_DISABLE_PSEUDOFS_OPERATIONS
unlink	: శూన్యసూచిక
#అయితే_ముగింపు
};

స్థిరరాశి cdev::px4_file_operations_t &cdev::CDev::fops_ref() { తిరిగి g_fops; }

స్థిర పూర్ణాంకం
cdev_open(file_t *filp)
{
	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_వ్యక్తిగత);

	తిరిగి cdev->open(filp);
}

స్థిర పూర్ణాంకం
cdev_close(file_t *filp)
{
	ఒకవేళ ((filp->f_inode->i_crefs) == 0) {
		తిరిగి -ENODEV;
	}

	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_వ్యక్తిగత);

	తిరిగి cdev->close(filp);
}

స్థిర ssize_t
cdev_read(file_t *filp, అక్షరం *బఫరు_2, size_t buflen)
{
	ఒకవేళ ((filp->f_inode->i_crefs) == 0) {
		తిరిగి -ENODEV;
	}

	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_వ్యక్తిగత);

	తిరిగి cdev->పఠ(filp, బఫరు_2, buflen);
}

స్థిర ssize_t
cdev_write(file_t *filp, స్థిరరాశి అక్షరం *బఫరు_2, size_t buflen)
{
	ఒకవేళ ((filp->f_inode->i_crefs) == 0) {
		తిరిగి -ENODEV;
	}

	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_వ్యక్తిగత);

	తిరిగి cdev->లిఖ(filp, బఫరు_2, buflen);
}

స్థిర off_t
cdev_seek(file_t *filp, off_t స్థానభ్రంశం, పూర్ణాంకం whence)
{
	ఒకవేళ ((filp->f_inode->i_crefs) == 0) {
		తిరిగి -ENODEV;
	}

	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_వ్యక్తిగత);

	తిరిగి cdev->seek(filp, స్థానభ్రంశం, whence);
}

స్థిర పూర్ణాంకం
cdev_ioctl(file_t *filp, పూర్ణాంకం cmd, గుర్తులేని దీర్ఘ arg)
{
	ఒకవేళ ((filp->f_inode->i_crefs) == 0) {
		తిరిగి -ENODEV;
	}

	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_వ్యక్తిగత);

	తిరిగి cdev->ioctl(filp, cmd, arg);
}

స్థిర పూర్ణాంకం
cdev_poll(file_t *filp, px4_pollfd_struct_t *fds, తార్కిక అమరిక)
{
	ఒకవేళ ((filp->f_inode->i_crefs) == 0) {
		తిరిగి -ENODEV;
	}

	cdev::CDev *cdev = (cdev::CDev *)(filp->f_inode->i_వ్యక్తిగత);

	తిరిగి cdev->poll(filp, fds, అమరిక);
}

} // namespace cdev
