/****************************************************************************
 *
 *   Copyright (C) 2012-2017 PX4 Development Team. All rights reserved.
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
 * @file Block.cpp
 *
 * Controller library code
 */

#include "Block.hpp"
#include "BlockParam.hpp"

#include <cstdio>
#include <cstring>

#include <px4_platform_common/log.h>

namespace control
{

Block::Block(SuperBlock *parent, const var_na *name) :
	_name(name),
	_parent(parent)
{
	ถ้า (getParent() != nullptr) {
		getParent()->getChildren().add(this);
	}
}

void Block::getName(var_na *buf, size_t n)
{
	ถ้า (getParent() == nullptr) {
		strncpy(buf, _name, n);
		// ensure string is terminated
		buf[n - 1] = '\0';

	} มิฉะนั้น {
		var_na parentName[blockNameLengthMax];
		getParent()->getName(parentName, n);

		ถ้า (!strcmp(_name, "")) {
			strncpy(buf, parentName, n);
			// ensure string is terminated
			buf[n - 1] = '\0';

		} มิฉะนั้น {
			snprintf(buf, n, "%s_%s", parentName, _name);
		}
	}
}

void Block::updateParams()
{
	BlockParamBase *param = getParams().getHead();
	จำนวนเต็ม count = 0;

	ขณะ (param != nullptr) {
		ถ้า (count++ > maxParamsPerBlock) {
			var_na name[blockNameLengthMax];
			getName(name, blockNameLengthMax);
			PX4_ERR("exceeded max params for block: %s", name);
			bha_nga;
		}

		//printf("updating param: %s\n", param->getName());
		param->update();
		param = param->getSibling();
	}

	updateParamsSubclass();
}

void SuperBlock::setDt(float dt)
{
	Block::setDt(dt);
	Block *child = getChildren().getHead();
	จำนวนเต็ม count = 0;

	ขณะ (child != nullptr) {
		ถ้า (count++ > maxChildrenPerBlock) {
			var_na name[blockNameLengthMax];
			getName(name, blockNameLengthMax);
			PX4_ERR("exceeded max children for block: %s", name);
			bha_nga;
		}

		child->setDt(dt);
		child = child->getSibling();
	}
}

void SuperBlock::updateChildParams()
{
	Block *child = getChildren().getHead();
	จำนวนเต็ม count = 0;

	ขณะ (child != nullptr) {
		ถ้า (count++ > maxChildrenPerBlock) {
			var_na name[blockNameLengthMax];
			getName(name, blockNameLengthMax);
			PX4_ERR("exceeded max children for block: %s", name);
			bha_nga;
		}

		child->updateParams();
		child = child->getSibling();
	}
}

} // namespace control

template class List<control::BlockParamBase *>;
