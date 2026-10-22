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
वर्ग __EXPORT Block : सार्वजनिक List_गाठ<Block *>
{
सार्वजनिक:
	मित्र वर्ग BlockParamBase;

	Block(SuperBlock *parent, स्थिरांक अक्षर *name);
	आभासी ~Block() = पूर्वनिर्धारित;

	// no copy, assignment, move, move assignment
	Block(स्थिरांक Block &) = delete;
	Block &चालक=(स्थिरांक Block &) = delete;
	Block(Block &&) = delete;
	Block &चालक=(Block &&) = delete;

	शून्य getName(अक्षर *name, माप_प्रकार n);

	आभासी शून्य updateParams();

	आभासी शून्य setDt(दशांश dt) { _dt = dt; }
	दशांश getDt() { परत _dt; }

संरक्षित:

	आभासी शून्य updateParamsSubclass() {}

	SuperBlock *getParent() { परत _parent; }
	List<BlockParamBase *> &getParams() { परत _params; }

	स्थिरांक अक्षर *_name;
	SuperBlock *_parent;
	दशांश _dt{0.0f};

	List<BlockParamBase *> _params;
};

वर्ग __EXPORT SuperBlock :
	सार्वजनिक Block
{
सार्वजनिक:
	मित्र वर्ग Block;

	SuperBlock(SuperBlock *parent, स्थिरांक अक्षर *name) : Block(parent, name) {}
	~SuperBlock() = पूर्वनिर्धारित;

	// no copy, assignment, move, move assignment
	SuperBlock(स्थिरांक SuperBlock &) = delete;
	SuperBlock &चालक=(स्थिरांक SuperBlock &) = delete;
	SuperBlock(SuperBlock &&) = delete;
	SuperBlock &चालक=(SuperBlock &&) = delete;

	शून्य setDt(दशांश dt) override;

	शून्य updateParams() override
	{
		Block::updateParams();

		जर (getChildren().getHead() != nullptr) { updateChildParams(); }
	}

संरक्षित:
	List<Block *> &getChildren() { परत _children; }
	शून्य updateChildParams();

	List<Block *> _children;
};


} // namespace control
