
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

static ຈຳນວນເຕັມ writer_main(ຈຳນວນເຕັມ argc, var_na *argv[])
{
	var_na buf[1];

	ຈຳນວນເຕັມ fd = px4_open(TESTDEV, PX4_F_WRONLY);

	ຖ້າ (fd < 0) {
		PX4_INFO("Writer: Open failed %d", fd);
		ສົ່ງຄືນ PX4_ERROR;
	}

	ຈຳນວນເຕັມ ret = 0;
	ຈຳນວນເຕັມ i = 0;

	ໃນຂະນະທີ່ (!g_exit) {
		// Wait for 2 seconds
		PX4_INFO("Writer: Sleeping for 2 sec");
		ret = px4_sleep(2);

		ຖ້າ (ret < 0) {
			PX4_INFO("Writer: sleep failed %d %d", ret, errno);
			ສົ່ງຄືນ ret;
		}

		buf[0] = 'A' + (var_na)(i % 26);
		PX4_INFO("Writer: writing char '%c'", buf[0]);
		ret = px4_write(fd, buf, 1);
		++i;
	}

	px4_close(fd);
	PX4_INFO("Writer: stopped");
	ສົ່ງຄືນ ret;
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

	ຈຳນວນເຕັມ open(cdev::file_t *handlep) override;
	ຈຳນວນເຕັມ close(cdev::file_t *handlep) override;
	ssize_t write(cdev::file_t *handlep, const var_na *buffer, size_t buflen) override;
	ssize_t read(cdev::file_t *handlep, var_na *buffer, size_t buflen) override;
private:
	bool _is_open_for_write;
	size_t _write_offset;
	var_na     _buf[1000];
};

ຈຳນວນເຕັມ CDevNode::open(cdev::file_t *handlep)
{
	// Only allow one writer
	ຖ້າ (_is_open_for_write && (handlep->f_oflags & PX4_F_WRONLY)) {
		errno = EBUSY;
		ສົ່ງຄືນ -1;
	}

	ຈຳນວນເຕັມ ret = CDev::open(handlep);

	ຖ້າ (ret != 0) {
		ສົ່ງຄືນ ret;
	}

	handlep->f_priv = new PrivData;

	ຖ້າ (_is_open_for_write && (handlep->f_oflags & PX4_F_WRONLY)) {
		_is_open_for_write = true;
	}

	ສົ່ງຄືນ 0;
}

ຈຳນວນເຕັມ CDevNode::close(cdev::file_t *handlep)
{
	delete (PrivData *)handlep->f_priv;
	handlep->f_priv = nullptr;
	CDev::close(handlep);

	// Enable a new writer of the device is re-opened for write
	ຖ້າ ((handlep->f_oflags & PX4_F_WRONLY) && _is_open_for_write) {
		_is_open_for_write = false;
	}

	ສົ່ງຄືນ 0;
}

ssize_t CDevNode::write(cdev::file_t *handlep, const var_na *buffer, size_t buflen)
{
	ສຳລັບ (size_t i = 0; i < buflen && _write_offset < 1000; i++) {
		_buf[_write_offset] = buffer[i];
		_write_offset++;
	}

	// ignore what was written, but let pollers know something was written
	poll_notify(POLLIN);

	ສົ່ງຄືນ buflen;
}

ssize_t CDevNode::read(cdev::file_t *handlep, var_na *buffer, size_t buflen)
{
	PrivData *p = (PrivData *)handlep->f_priv;
	ssize_t chars_read = 0;
	PX4_INFO("read %zu write %zu", p->_read_offset, _write_offset);

	ສຳລັບ (size_t i = 0; i < buflen && (p->_read_offset < _write_offset); i++) {
		buffer[i] = _buf[p->_read_offset];
		p->_read_offset++;
		chars_read++;
	}

	ສົ່ງຄືນ chars_read;
}

CDevExample::~CDevExample()
{
	ຖ້າ (_node) {
		delete _node;
		_node = nullptr;
	}
}

ຈຳນວນເຕັມ CDevExample::do_poll(ຈຳນວນເຕັມ fd, ຈຳນວນເຕັມ timeout, ຈຳນວນເຕັມ iterations, ຈຳນວນເຕັມ delayms_after_poll)
{
	ຈຳນວນເຕັມ pollret, readret;
	ຈຳນວນເຕັມ loop_count = 0;
	var_na readbuf[10];
	px4_pollfd_struct_t fds[1];

	fds[0].fd = fd;
	fds[0].events = POLLIN;
	fds[0].revents = 0;

	bool mustblock = (timeout < 0);

	// Test indefinte blocking poll
	ໃນຂະນະທີ່ ((!appState.exitRequested()) && (loop_count < iterations)) {
		pollret = px4_poll(fds, 1, timeout);

		ຖ້າ (pollret < 0) {
			PX4_ERR("Reader: px4_poll failed %d FAIL", pollret);
			goto fail;
		}

		PX4_INFO("Reader: px4_poll returned %d", pollret);

		ຖ້າ (pollret) {
			readret = px4_read(fd, readbuf, 10);

			ຖ້າ (readret != 1) {
				ຖ້າ (mustblock) {
					PX4_ERR("Reader:     read failed %d FAIL", readret);
					goto fail;

				} ບໍ່ດັ່ງນັ້ນ {
					PX4_INFO("Reader:     read failed %d FAIL", readret);
				}

			} ບໍ່ດັ່ງນັ້ນ {
				readbuf[readret] = '\0';
				PX4_INFO("Reader: px4_poll     returned %d, read '%s' PASS", pollret, readbuf);
			}
		}

		ຖ້າ (delayms_after_poll) {
			px4_usleep(delayms_after_poll * 1000);
		}

		loop_count++;
	}

	ສົ່ງຄືນ 0;
fail:
	ສົ່ງຄືນ 1;
}
ຈຳນວນເຕັມ CDevExample::main()
{
	appState.setRunning(true);

	_node = new CDevNode();

	ຖ້າ (_node == nullptr) {
		PX4_INFO("Failed to allocate CDevNode");
		ສົ່ງຄືນ -ENOMEM;
	}

	ຖ້າ (_node->init() != PX4_OK) {
		PX4_INFO("Failed to init CDevNode");
		ສົ່ງຄືນ 1;
	}

	ຈຳນວນເຕັມ fd = px4_open(TESTDEV, PX4_F_RDONLY);

	ຖ້າ (fd < 0) {
		PX4_INFO("Open failed %d", fd);
		ສົ່ງຄືນ PX4_ERROR;
	}

	// Start a task that will write something in 4 seconds
	(void)px4_task_spawn_cmd("writer",
				 SCHED_DEFAULT,
				 SCHED_PRIORITY_MAX - 6,
				 2000,
				 writer_main,
				 (var_na *const *)nullptr);

	ຈຳນວນເຕັມ ret = 0;

	PX4_INFO("TEST: BLOCKING POLL ---------------");

	ຖ້າ (do_poll(fd, -1, 3, 0)) {
		ret = 1;
		goto fail2;
	}

	PX4_INFO("TEST: ZERO TIMEOUT POLL -----------");

	ຖ້າ (do_poll(fd, 0, 3, 0)) {
		ret = 1;
		goto fail2;
		goto fail2;
	}

	PX4_INFO("TEST: ZERO TIMEOUT POLL -----------");

	ຖ້າ (do_poll(fd, 0, 3, 0)) {
		ret = 1;
		goto fail2;
		goto fail2;
	}

	PX4_INFO("TEST: ZERO TIMEOUT POLL -----------");

	ຖ້າ (do_poll(fd, 0, 3, 0)) {
		ret = 1;
		goto fail2;
	}

	PX4_INFO("TEST: 100ms TIMEOUT POLL -----------");

	ຖ້າ (do_poll(fd, 0, 30, 100)) {
		ret = 1;
		goto fail2;
	}

	PX4_INFO("TEST: 1 SEC TIMOUT POLL ------------");

	ຖ້າ (do_poll(fd, 1000, 3, 0)) {
		ret = 1;
		goto fail2;
	}

	PX4_INFO("TEST: waiting for writer to stop");
fail2:
	g_exit = true;
	px4_close(fd);
	PX4_INFO("TEST: waiting for writer to stop");
	px4_sleep(3);
	ສົ່ງຄືນ ret;
}
