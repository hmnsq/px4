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
 * @file Block.hpp
 *
 * Controller library code
 */

#pragma once

#अन्तर्भाव <containers/List.hpp>
#अन्तर्भाव <controllib/block/BlockParam.hpp>
#अन्तर्भाव <cstdint>

नामाकाश control
{

स्थिर constexpr uint8_t maxChildrenPerBlock = 100;
स्थिर constexpr uint8_t maxParamsPerBlock = 110;
स्थिर constexpr uint8_t blockNameLengthMax = 40;

// forward declaration
वर्ग BlockParamBase;
वर्ग SuperBlock;

/**
 */
वर्ग __EXPORT Block : सार्वजनिक List_गाँठ<Block *>
{
सार्वजनिक:
	friend वर्ग BlockParamBase;

	Block(SuperBlock *parent, ध्रुव वर्ण *name);
	आभास ~Block() = स्वभाव;

	// no copy, assignment, move, move assignment
	Block(ध्रुव Block &) = delete;
	Block &operator=(ध्रुव Block &) = delete;
	Block(Block &&) = delete;
	Block &operator=(Block &&) = delete;

	शून्य getName(वर्ण *name, size_t n);

	आभास शून्य updateParams();

	आभास शून्य setDt(भिन्न dt) { _dt = dt; }
	भिन्न getDt() { प्रत्यागम _dt; }

रक्षित:

	आभास शून्य updateParamsSubclass() {}

	SuperBlock *getParent() { प्रत्यागम _parent; }
	List<BlockParamBase *> &getParams() { प्रत्यागम _params; }

	ध्रुव वर्ण *_name;
	SuperBlock *_parent;
	भिन्न _dt{0.0f};

	List<BlockParamBase *> _params;
};

वर्ग __EXPORT SuperBlock :
	सार्वजनिक Block
{
सार्वजनिक:
	friend वर्ग Block;

	SuperBlock(SuperBlock *parent, ध्रुव वर्ण *name) : Block(parent, name) {}
	~SuperBlock() = स्वभाव;

	// no copy, assignment, move, move assignment
	SuperBlock(ध्रुव SuperBlock &) = delete;
	SuperBlock &operator=(ध्रुव SuperBlock &) = delete;
	SuperBlock(SuperBlock &&) = delete;
	SuperBlock &operator=(SuperBlock &&) = delete;

	शून्य setDt(भिन्न dt) override;

	शून्य updateParams() override
	{
		Block::updateParams();

		यदि (getChildren().getHead() != nullptr) { updateChildParams(); }
	}

रक्षित:
	List<Block *> &getChildren() { प्रत्यागम _children; }
	शून्य updateChildParams();

	List<Block *> _children;
};


} // namespace control
