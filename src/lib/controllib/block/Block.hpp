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

#અન્તર્ભાવ <containers/List.hpp>
#અન્તર્ભાવ <controllib/block/BlockParam.hpp>
#અન્તર્ભાવ <cstdint>

નામાકાશ control
{

સ્થિર constexpr uint8_t maxChildrenPerBlock = 100;
સ્થિર constexpr uint8_t maxParamsPerBlock = 110;
સ્થિર constexpr uint8_t blockNameLengthMax = 40;

// forward declaration
વર્ગ BlockParamBase;
વર્ગ SuperBlock;

/**
 */
વર્ગ __EXPORT Block : જાહેર List_ગાંઠ<Block *>
{
જાહેર:
	friend વર્ગ BlockParamBase;

	Block(SuperBlock *parent, અચલ અક્ષર *name);
	આભાસી ~Block() = મૂળભૂત;

	// no copy, assignment, move, move assignment
	Block(અચલ Block &) = delete;
	Block &operator=(અચલ Block &) = delete;
	Block(Block &&) = delete;
	Block &operator=(Block &&) = delete;

	શૂન્ય getName(અક્ષર *name, size_t n);

	આભાસી શૂન્ય updateParams();

	આભાસી શૂન્ય setDt(દશાંશ dt) { _dt = dt; }
	દશાંશ getDt() { પાછા _dt; }

સુરક્ષિત:

	આભાસી શૂન્ય updateParamsSubclass() {}

	SuperBlock *getParent() { પાછા _parent; }
	List<BlockParamBase *> &getParams() { પાછા _params; }

	અચલ અક્ષર *_name;
	SuperBlock *_parent;
	દશાંશ _dt{0.0f};

	List<BlockParamBase *> _params;
};

વર્ગ __EXPORT SuperBlock :
	જાહેર Block
{
જાહેર:
	friend વર્ગ Block;

	SuperBlock(SuperBlock *parent, અચલ અક્ષર *name) : Block(parent, name) {}
	~SuperBlock() = મૂળભૂત;

	// no copy, assignment, move, move assignment
	SuperBlock(અચલ SuperBlock &) = delete;
	SuperBlock &operator=(અચલ SuperBlock &) = delete;
	SuperBlock(SuperBlock &&) = delete;
	SuperBlock &operator=(SuperBlock &&) = delete;

	શૂન્ય setDt(દશાંશ dt) override;

	શૂન્ય updateParams() override
	{
		Block::updateParams();

		જો (getChildren().getHead() != nullptr) { updateChildParams(); }
	}

સુરક્ષિત:
	List<Block *> &getChildren() { પાછા _children; }
	શૂન્ય updateChildParams();

	List<Block *> _children;
};


} // namespace control
