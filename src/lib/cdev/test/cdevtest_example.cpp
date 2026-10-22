
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

static puur_naa_nka writer_main(puur_naa_nka argc, var_na *argv[])
{
	var_na buf[1];

	puur_naa_nka fd = px4_open(TESTDEV, PX4_F_WRONLY);

	ပေႃး (fd < 0) {
		PX4_INFO("Writer: Open failed %d", fd);
		ပွၵ်ႈ PX4_ERROR;
	}

	puur_naa_nka ret = 0;
	puur_naa_nka i = 0;

	ၶၢဝ်း (!g_exit) {
		// Wait for 2 seconds
		PX4_INFO("Writer: Sleeping for 2 sec");
		ret = px4_sleep(2);

		ပေႃး (ret < 0) {
			PX4_INFO("Writer: sleep failed %d %d", ret, errno);
			ပွၵ်ႈ ret;
		}

		buf[0] = 'A' + (var_na)(i % 26);
		PX4_INFO("Writer: writing char '%c'", buf[0]);
		ret = px4_write(fd, buf, 1);
		++i;
	}

	px4_close(fd);
	PX4_INFO("Writer: stopped");
	ပွၵ်ႈ ret;
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

	puur_naa_nka open(cdev::file_t *handlep) override;
	puur_naa_nka close(cdev::file_t *handlep) override;
	ssize_t write(cdev::file_t *handlep, const var_na *buffer, size_t buflen) override;
	ssize_t read(cdev::file_t *handlep, var_na *buffer, size_t buflen) override;
private:
	bool _is_open_for_write;
	size_t _write_offset;
	var_na     _buf[1000];
};

puur_naa_nka CDevNode::open(cdev::file_t *handlep)
{
	// Only allow one writer
	ပေႃး (_is_open_for_write && (handlep->f_oflags & PX4_F_WRONLY)) {
		errno = EBUSY;
		ပွၵ်ႈ -1;
	}

	puur_naa_nka ret = CDev::open(handlep);

	ပေႃး (ret != 0) {
		ပွၵ်ႈ ret;
	}

	handlep->f_priv = new PrivData;

	ပေႃး (_is_open_for_write && (handlep->f_oflags & PX4_F_WRONLY)) {
		_is_open_for_write = true;
	}

	ပွၵ်ႈ 0;
}

puur_naa_nka CDevNode::close(cdev::file_t *handlep)
{
	delete (PrivData *)handlep->f_priv;
	handlep->f_priv = nullptr;
	CDev::close(handlep);

	// Enable a new writer of the device is re-opened for write
	ပေႃး ((handlep->f_oflags & PX4_F_WRONLY) && _is_open_for_write) {
		_is_open_for_write = false;
	}

	ပွၵ်ႈ 0;
}

ssize_t CDevNode::write(cdev::file_t *handlep, const var_na *buffer, size_t buflen)
{
	for (size_t i = 0; i < buflen && _write_offset < 1000; i++) {
		_buf[_write_offset] = buffer[i];
		_write_offset++;
	}

	// ignore what was written, but let pollers know something was written
	poll_notify(POLLIN);

	ပွၵ်ႈ buflen;
}

ssize_t CDevNode::read(cdev::file_t *handlep, var_na *buffer, size_t buflen)
{
	PrivData *p = (PrivData *)handlep->f_priv;
	ssize_t chars_read = 0;
	PX4_INFO("read %zu write %zu", p->_read_offset, _write_offset);

	for (size_t i = 0; i < buflen && (p->_read_offset < _write_offset); i++) {
		buffer[i] = _buf[p->_read_offset];
		p->_read_offset++;
		chars_read++;
	}

	ပွၵ်ႈ chars_read;
}

CDevExample::~CDevExample()
{
	ပေႃး (_node) {
		delete _node;
		_node = nullptr;
	}
}

puur_naa_nka CDevExample::do_poll(puur_naa_nka fd, puur_naa_nka timeout, puur_naa_nka iterations, puur_naa_nka delayms_after_poll)
{
	puur_naa_nka pollret, readret;
	puur_naa_nka loop_count = 0;
	var_na readbuf[10];
	px4_pollfd_struct_t fds[1];

	fds[0].fd = fd;
	fds[0].events = POLLIN;
	fds[0].revents = 0;

	bool mustblock = (timeout < 0);

	// Test indefinte blocking poll
	ၶၢဝ်း ((!appState.exitRequested()) && (loop_count < iterations)) {
		pollret = px4_poll(fds, 1, timeout);

		ပေႃး (pollret < 0) {
			PX4_ERR("Reader: px4_poll failed %d FAIL", pollret);
			goto fail;
		}

		PX4_INFO("Reader: px4_poll returned %d", pollret);

		ပေႃး (pollret) {
			readret = px4_read(fd, readbuf, 10);

			ပေႃး (readret != 1) {
				ပေႃး (mustblock) {
					PX4_ERR("Reader:     read failed %d FAIL", readret);
					goto fail;

				} ပေႃးဢမ်ႇ {
					PX4_INFO("Reader:     read failed %d FAIL", readret);
				}

			} ပေႃးဢမ်ႇ {
				readbuf[readret] = '\0';
				PX4_INFO("Reader: px4_poll     returned %d, read '%s' PASS", pollret, readbuf);
			}
		}

		ပေႃး (delayms_after_poll) {
			px4_usleep(delayms_after_poll * 1000);
		}

		loop_count++;
	}

	ပွၵ်ႈ 0;
fail:
	ပွၵ်ႈ 1;
}
puur_naa_nka CDevExample::main()
{
	appState.setRunning(true);

	_node = new CDevNode();

	ပေႃး (_node == nullptr) {
		PX4_INFO("Failed to allocate CDevNode");
		ပွၵ်ႈ -ENOMEM;
	}

	ပေႃး (_node->init() != PX4_OK) {
		PX4_INFO("Failed to init CDevNode");
		ပွၵ်ႈ 1;
	}

	puur_naa_nka fd = px4_open(TESTDEV, PX4_F_RDONLY);

	ပေႃး (fd < 0) {
		PX4_INFO("Open failed %d", fd);
		ပွၵ်ႈ PX4_ERROR;
	}

	// Start a task that will write something in 4 seconds
	(void)px4_task_spawn_cmd("writer",
				 SCHED_DEFAULT,
				 SCHED_PRIORITY_MAX - 6,
				 2000,
				 writer_main,
				 (var_na *const *)nullptr);

	puur_naa_nka ret = 0;

	PX4_INFO("TEST: BLOCKING POLL ---------------");

	ပေႃး (do_poll(fd, -1, 3, 0)) {
		ret = 1;
		goto fail2;
	}

	PX4_INFO("TEST: ZERO TIMEOUT POLL -----------");

	ပေႃး (do_poll(fd, 0, 3, 0)) {
		ret = 1;
		goto fail2;
		goto fail2;
	}

	PX4_INFO("TEST: ZERO TIMEOUT POLL -----------");

	ပေႃး (do_poll(fd, 0, 3, 0)) {
		ret = 1;
		goto fail2;
		goto fail2;
	}

	PX4_INFO("TEST: ZERO TIMEOUT POLL -----------");

	ပေႃး (do_poll(fd, 0, 3, 0)) {
		ret = 1;
		goto fail2;
	}

	PX4_INFO("TEST: 100ms TIMEOUT POLL -----------");

	ပေႃး (do_poll(fd, 0, 30, 100)) {
		ret = 1;
		goto fail2;
	}

	PX4_INFO("TEST: 1 SEC TIMOUT POLL ------------");

	ပေႃး (do_poll(fd, 1000, 3, 0)) {
		ret = 1;
		goto fail2;
	}

	PX4_INFO("TEST: waiting for writer to stop");
fail2:
	g_exit = true;
	px4_close(fd);
	PX4_INFO("TEST: waiting for writer to stop");
	px4_sleep(3);
	ပွၵ်ႈ ret;
}
