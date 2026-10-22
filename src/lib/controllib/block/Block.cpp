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

#अन्तर्भाव "Block.hpp"
#अन्तर्भाव "BlockParam.hpp"

#अन्तर्भाव <cstdio>
#अन्तर्भाव <cstring>

#अन्तर्भाव <px4_platform_common/log.h>

नामाकाश control
{

Block::Block(SuperBlock *parent, अचर वर्ण *name) :
	_name(name),
	_parent(parent)
{
	यदि (getParent() != nullptr) {
		getParent()->getChildren().add(एतत्);
	}
}

शून्य Block::getName(वर्ण *buf, माप_प्रकार n)
{
	यदि (getParent() == nullptr) {
		म_नकलन(buf, _name, n);
		// ensure string is terminated
		buf[n - 1] = '\0';

	} अन्यथा {
		वर्ण parentName[blockNameLengthMax];
		getParent()->getName(parentName, n);

		यदि (!म_भेद(_name, "")) {
			म_नकलन(buf, parentName, n);
			// ensure string is terminated
			buf[n - 1] = '\0';

		} अन्यथा {
			snprintf(buf, n, "%s_%s", parentName, _name);
		}
	}
}

शून्य Block::updateParams()
{
	BlockParamBase *param = getParams().getHead();
	पूर्णाङ्क गन्ती = 0;

	जबसम्म (param != nullptr) {
		यदि (गन्ती++ > maxParamsPerBlock) {
			वर्ण name[blockNameLengthMax];
			getName(name, blockNameLengthMax);
			PX4_ERR("exceeded max params for block: %s", name);
			रोक;
		}

		//printf("updating param: %s\n", param->getName());
		param->update();
		param = param->getSibling();
	}

	updateParamsSubclass();
}

शून्य SuperBlock::setDt(भिन्न dt)
{
	Block::setDt(dt);
	Block *child = getChildren().getHead();
	पूर्णाङ्क गन्ती = 0;

	जबसम्म (child != nullptr) {
		यदि (गन्ती++ > maxChildrenPerBlock) {
			वर्ण name[blockNameLengthMax];
			getName(name, blockNameLengthMax);
			PX4_ERR("exceeded max children for block: %s", name);
			रोक;
		}

		child->setDt(dt);
		child = child->getSibling();
	}
}

शून्य SuperBlock::updateChildParams()
{
	Block *child = getChildren().getHead();
	पूर्णाङ्क गन्ती = 0;

	जबसम्म (child != nullptr) {
		यदि (गन्ती++ > maxChildrenPerBlock) {
			वर्ण name[blockNameLengthMax];
			getName(name, blockNameLengthMax);
			PX4_ERR("exceeded max children for block: %s", name);
			रोक;
		}

		child->updateParams();
		child = child->getSibling();
	}
}

} // namespace control

ढाँचा वर्ग List<control::BlockParamBase *>;
