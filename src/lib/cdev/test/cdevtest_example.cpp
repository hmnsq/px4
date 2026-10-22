
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

#include "cdevtest_example.h"

#include <px4_platform_common/log.h>
#include <px4_platform_common/tasks.h>
#include <px4_platform_common/time.h>
#include <lib/cdev/CDev.hpp>
#include <unistd.h>
#include <stdio.h>

px4::AppState CDevExample::appState;

#define TESTDEV "/dev/cdevtest"

static bool g_exit = false;

static số_nguyên writer_main(số_nguyên argc, char *argv[])
{
	char buf[1];

	số_nguyên fd = px4_open(TESTDEV, PX4_F_WRONLY);

	nếu (fd < 0) {
		PX4_INFO("Writer: Open failed %d", fd);
		trả_về PX4_ERROR;
	}

	số_nguyên ret = 0;
	số_nguyên i = 0;

	trong_khi (!g_exit) {
		// Wait for 2 seconds
		PX4_INFO("Writer: Sleeping for 2 sec");
		ret = px4_sleep(2);

		nếu (ret < 0) {
			PX4_INFO("Writer: sleep failed %d %d", ret, errno);
			trả_về ret;
		}

		buf[0] = 'A' + (char)(i % 26);
		PX4_INFO("Writer: writing char '%c'", buf[0]);
		ret = px4_write(fd, buf, 1);
		++i;
	}

	px4_close(fd);
	PX4_INFO("Writer: stopped");
	trả_về ret;
}

class PrivData
{
public:
	PrivData() : _read_offset(0) {}
	~PrivData() = default;

	size_t _read_offset;
};

class CDevNode : public cdev::CDev
{
public:
	CDevNode() :
		CDev(TESTDEV),
		_is_open_for_write(false),
		_write_offset(0) {}

	~CDevNode() override = default;

	số_nguyên open(cdev::file_t *handlep) override;
	số_nguyên close(cdev::file_t *handlep) override;
	ssize_t write(cdev::file_t *handlep, const char *buffer, size_t buflen) override;
	ssize_t read(cdev::file_t *handlep, char *buffer, size_t buflen) override;
private:
	bool _is_open_for_write;
	size_t _write_offset;
	char     _buf[1000];
};

số_nguyên CDevNode::open(cdev::file_t *handlep)
{
	// Only allow one writer
	nếu (_is_open_for_write && (handlep->f_oflags & PX4_F_WRONLY)) {
		errno = EBUSY;
		trả_về -1;
	}

	số_nguyên ret = CDev::open(handlep);

	nếu (ret != 0) {
		trả_về ret;
	}

	handlep->f_priv = new PrivData;

	nếu (_is_open_for_write && (handlep->f_oflags & PX4_F_WRONLY)) {
		_is_open_for_write = true;
	}

	trả_về 0;
}

số_nguyên CDevNode::close(cdev::file_t *handlep)
{
	delete (PrivData *)handlep->f_priv;
	handlep->f_priv = nullptr;
	CDev::close(handlep);

	// Enable a new writer of the device is re-opened for write
	nếu ((handlep->f_oflags & PX4_F_WRONLY) && _is_open_for_write) {
		_is_open_for_write = false;
	}

	trả_về 0;
}

ssize_t CDevNode::write(cdev::file_t *handlep, const char *buffer, size_t buflen)
{
	cho (size_t i = 0; i < buflen && _write_offset < 1000; i++) {
		_buf[_write_offset] = buffer[i];
		_write_offset++;
	}

	// ignore what was written, but let pollers know something was written
	poll_notify(POLLIN);

	trả_về buflen;
}

ssize_t CDevNode::read(cdev::file_t *handlep, char *buffer, size_t buflen)
{
	PrivData *p = (PrivData *)handlep->f_priv;
	ssize_t chars_read = 0;
	PX4_INFO("read %zu write %zu", p->_read_offset, _write_offset);

	cho (size_t i = 0; i < buflen && (p->_read_offset < _write_offset); i++) {
		buffer[i] = _buf[p->_read_offset];
		p->_read_offset++;
		chars_read++;
	}

	trả_về chars_read;
}

CDevExample::~CDevExample()
{
	nếu (_node) {
		delete _node;
		_node = nullptr;
	}
}

số_nguyên CDevExample::do_poll(số_nguyên fd, số_nguyên timeout, số_nguyên iterations, số_nguyên delayms_after_poll)
{
	số_nguyên pollret, readret;
	số_nguyên loop_count = 0;
	char readbuf[10];
	px4_pollfd_struct_t fds[1];

	fds[0].fd = fd;
	fds[0].events = POLLIN;
	fds[0].revents = 0;

	bool mustblock = (timeout < 0);

	// Test indefinte blocking poll
	trong_khi ((!appState.exitRequested()) && (loop_count < iterations)) {
		pollret = px4_poll(fds, 1, timeout);

		nếu (pollret < 0) {
			PX4_ERR("Reader: px4_poll failed %d FAIL", pollret);
			goto fail;
		}

		PX4_INFO("Reader: px4_poll returned %d", pollret);

		nếu (pollret) {
			readret = px4_read(fd, readbuf, 10);

			nếu (readret != 1) {
				nếu (mustblock) {
					PX4_ERR("Reader:     read failed %d FAIL", readret);
					goto fail;

				} không_thì {
					PX4_INFO("Reader:     read failed %d FAIL", readret);
				}

			} không_thì {
				readbuf[readret] = '\0';
				PX4_INFO("Reader: px4_poll     returned %d, read '%s' PASS", pollret, readbuf);
			}
		}

		nếu (delayms_after_poll) {
			px4_usleep(delayms_after_poll * 1000);
		}

		loop_count++;
	}

	trả_về 0;
fail:
	trả_về 1;
}
số_nguyên CDevExample::main()
{
	appState.setRunning(true);

	_node = new CDevNode();

	nếu (_node == nullptr) {
		PX4_INFO("Failed to allocate CDevNode");
		trả_về -ENOMEM;
	}

	nếu (_node->init() != PX4_OK) {
		PX4_INFO("Failed to init CDevNode");
		trả_về 1;
	}

	số_nguyên fd = px4_open(TESTDEV, PX4_F_RDONLY);

	nếu (fd < 0) {
		PX4_INFO("Open failed %d", fd);
		trả_về PX4_ERROR;
	}

	// Start a task that will write something in 4 seconds
	(void)px4_task_spawn_cmd("writer",
				 SCHED_DEFAULT,
				 SCHED_PRIORITY_MAX - 6,
				 2000,
				 writer_main,
				 (char *const *)nullptr);

	số_nguyên ret = 0;

	PX4_INFO("TEST: BLOCKING POLL ---------------");

	nếu (do_poll(fd, -1, 3, 0)) {
		ret = 1;
		goto fail2;
	}

	PX4_INFO("TEST: ZERO TIMEOUT POLL -----------");

	nếu (do_poll(fd, 0, 3, 0)) {
		ret = 1;
		goto fail2;
		goto fail2;
	}

	PX4_INFO("TEST: ZERO TIMEOUT POLL -----------");

	nếu (do_poll(fd, 0, 3, 0)) {
		ret = 1;
		goto fail2;
		goto fail2;
	}

	PX4_INFO("TEST: ZERO TIMEOUT POLL -----------");

	nếu (do_poll(fd, 0, 3, 0)) {
		ret = 1;
		goto fail2;
	}

	PX4_INFO("TEST: 100ms TIMEOUT POLL -----------");

	nếu (do_poll(fd, 0, 30, 100)) {
		ret = 1;
		goto fail2;
	}

	PX4_INFO("TEST: 1 SEC TIMOUT POLL ------------");

	nếu (do_poll(fd, 1000, 3, 0)) {
		ret = 1;
		goto fail2;
	}

	PX4_INFO("TEST: waiting for writer to stop");
fail2:
	g_exit = true;
	px4_close(fd);
	PX4_INFO("TEST: waiting for writer to stop");
	px4_sleep(3);
	trả_về ret;
}
