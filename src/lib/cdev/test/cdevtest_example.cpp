
/****************************************************************************
 *
 *   Copyright (C) 2015 Mark Charlebois. All rights reserved.
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
 * @file cdevtest_example.cpp
 * Example for Linux
 *
 * @author Mark Charlebois <charlebm@gmail.com>
 */

#ضمن "cdevtest_example.h"

#ضمن <px4_platform_common/log.h>
#ضمن <px4_platform_common/tasks.h>
#ضمن <px4_platform_common/time.h>
#ضمن <lib/cdev/CDev.hpp>
#ضمن <unistd.h>
#ضمن <stdio.h>

px4::AppState CDevExample::appState;

#define TESTDEV "/dev/cdevtest"

ساكن منطقي g_exit = שקר;

ساكن صحيح writer_main(صحيح argc, حرف *argv[])
{
	حرف buf[1];

	صحيح fd = px4_open(TESTDEV, PX4_F_WRONLY);

	אם (fd < 0) {
		PX4_INFO("Writer: Open failed %d", fd);
		החזר PX4_ERROR;
	}

	صحيح ret = 0;
	صحيح i = 0;

	כלעוד (!g_exit) {
		// Wait for 2 seconds
		PX4_INFO("Writer: Sleeping for 2 sec");
		ret = px4_sleep(2);

		אם (ret < 0) {
			PX4_INFO("Writer: sleep failed %d %d", ret, errno);
			החזר ret;
		}

		buf[0] = 'A' + (حرف)(i % 26);
		PX4_INFO("Writer: writing char '%c'", buf[0]);
		ret = px4_write(fd, buf, 1);
		++i;
	}

	px4_close(fd);
	PX4_INFO("Writer: stopped");
	החזר ret;
}

מחלקה PrivData
{
عام:
	PrivData() : _read_offset(0) {}
	~PrivData() = افتراضي;

	size_t _read_offset;
};

מחלקה CDevNode : عام cdev::CDev
{
عام:
	CDevNode() :
		CDev(TESTDEV),
		_is_open_for_write(שקר),
		_write_offset(0) {}

	~CDevNode() override = افتراضي;

	صحيح open(cdev::file_t *handlep) override;
	صحيح close(cdev::file_t *handlep) override;
	ssize_t اكتب(cdev::file_t *handlep, ثابت حرف *buffer, size_t buflen) override;
	ssize_t اقرأ(cdev::file_t *handlep, حرف *buffer, size_t buflen) override;
خاص:
	منطقي _is_open_for_write;
	size_t _write_offset;
	حرف     _buf[1000];
};

صحيح CDevNode::open(cdev::file_t *handlep)
{
	// Only allow one writer
	אם (_is_open_for_write && (handlep->f_oflags & PX4_F_WRONLY)) {
		errno = EBUSY;
		החזר -1;
	}

	صحيح ret = CDev::open(handlep);

	אם (ret != 0) {
		החזר ret;
	}

	handlep->f_priv = جديد PrivData;

	אם (_is_open_for_write && (handlep->f_oflags & PX4_F_WRONLY)) {
		_is_open_for_write = אמת;
	}

	החזר 0;
}

صحيح CDevNode::close(cdev::file_t *handlep)
{
	delete (PrivData *)handlep->f_priv;
	handlep->f_priv = nullptr;
	CDev::close(handlep);

	// Enable a new writer of the device is re-opened for write
	אם ((handlep->f_oflags & PX4_F_WRONLY) && _is_open_for_write) {
		_is_open_for_write = שקר;
	}

	החזר 0;
}

ssize_t CDevNode::اكتب(cdev::file_t *handlep, ثابت حرف *buffer, size_t buflen)
{
	עבור (size_t i = 0; i < buflen && _write_offset < 1000; i++) {
		_buf[_write_offset] = buffer[i];
		_write_offset++;
	}

	// ignore what was written, but let pollers know something was written
	poll_notify(POLLIN);

	החזר buflen;
}

ssize_t CDevNode::اقرأ(cdev::file_t *handlep, حرف *buffer, size_t buflen)
{
	PrivData *p = (PrivData *)handlep->f_priv;
	ssize_t chars_read = 0;
	PX4_INFO("read %zu write %zu", p->_read_offset, _write_offset);

	עבור (size_t i = 0; i < buflen && (p->_read_offset < _write_offset); i++) {
		buffer[i] = _buf[p->_read_offset];
		p->_read_offset++;
		chars_read++;
	}

	החזר chars_read;
}

CDevExample::~CDevExample()
{
	אם (_node) {
		delete _node;
		_node = nullptr;
	}
}

صحيح CDevExample::do_poll(صحيح fd, صحيح timeout, صحيح iterations, صحيح delayms_after_poll)
{
	صحيح pollret, readret;
	صحيح loop_count = 0;
	حرف readbuf[10];
	px4_pollfd_struct_t fds[1];

	fds[0].fd = fd;
	fds[0].events = POLLIN;
	fds[0].revents = 0;

	منطقي mustblock = (timeout < 0);

	// Test indefinte blocking poll
	כלעוד ((!appState.exitRequested()) && (loop_count < iterations)) {
		pollret = px4_poll(fds, 1, timeout);

		אם (pollret < 0) {
			PX4_ERR("Reader: px4_poll failed %d FAIL", pollret);
			اذهب fail;
		}

		PX4_INFO("Reader: px4_poll returned %d", pollret);

		אם (pollret) {
			readret = px4_read(fd, readbuf, 10);

			אם (readret != 1) {
				אם (mustblock) {
					PX4_ERR("Reader:     read failed %d FAIL", readret);
					اذهب fail;

				} אחרת {
					PX4_INFO("Reader:     read failed %d FAIL", readret);
				}

			} אחרת {
				readbuf[readret] = '\0';
				PX4_INFO("Reader: px4_poll     returned %d, read '%s' PASS", pollret, readbuf);
			}
		}

		אם (delayms_after_poll) {
			px4_usleep(delayms_after_poll * 1000);
		}

		loop_count++;
	}

	החזר 0;
fail:
	החזר 1;
}
صحيح CDevExample::main()
{
	appState.setRunning(אמת);

	_node = جديد CDevNode();

	אם (_node == nullptr) {
		PX4_INFO("Failed to allocate CDevNode");
		החזר -ENOMEM;
	}

	אם (_node->init() != PX4_OK) {
		PX4_INFO("Failed to init CDevNode");
		החזר 1;
	}

	صحيح fd = px4_open(TESTDEV, PX4_F_RDONLY);

	אם (fd < 0) {
		PX4_INFO("Open failed %d", fd);
		החזר PX4_ERROR;
	}

	// Start a task that will write something in 4 seconds
	(فراغ)px4_task_spawn_cmd("writer",
				 SCHED_DEFAULT,
				 SCHED_PRIORITY_MAX - 6,
				 2000,
				 writer_main,
				 (حرف *ثابت *)nullptr);

	صحيح ret = 0;

	PX4_INFO("TEST: BLOCKING POLL ---------------");

	אם (do_poll(fd, -1, 3, 0)) {
		ret = 1;
		اذهب fail2;
	}

	PX4_INFO("TEST: ZERO TIMEOUT POLL -----------");

	אם (do_poll(fd, 0, 3, 0)) {
		ret = 1;
		اذهب fail2;
		اذهب fail2;
	}

	PX4_INFO("TEST: ZERO TIMEOUT POLL -----------");

	אם (do_poll(fd, 0, 3, 0)) {
		ret = 1;
		اذهب fail2;
		اذهب fail2;
	}

	PX4_INFO("TEST: ZERO TIMEOUT POLL -----------");

	אם (do_poll(fd, 0, 3, 0)) {
		ret = 1;
		اذهب fail2;
	}

	PX4_INFO("TEST: 100ms TIMEOUT POLL -----------");

	אם (do_poll(fd, 0, 30, 100)) {
		ret = 1;
		اذهب fail2;
	}

	PX4_INFO("TEST: 1 SEC TIMOUT POLL ------------");

	אם (do_poll(fd, 1000, 3, 0)) {
		ret = 1;
		اذهب fail2;
	}

	PX4_INFO("TEST: waiting for writer to stop");
fail2:
	g_exit = אמת;
	px4_close(fd);
	PX4_INFO("TEST: waiting for writer to stop");
	px4_sleep(3);
	החזר ret;
}
