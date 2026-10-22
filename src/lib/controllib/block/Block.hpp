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

#ضمن <containers/List.hpp>
#ضمن <controllib/block/BlockParam.hpp>
#ضمن <cstdint>

نطاق control
{

ساكن constexpr uint8_t maxChildrenPerBlock = 100;
ساكن constexpr uint8_t maxParamsPerBlock = 110;
ساكن constexpr uint8_t blockNameLengthMax = 40;

// forward declaration
صنف BlockParamBase;
صنف SuperBlock;

/**
 */
صنف __EXPORT Block : عام List_نود<Block *>
{
عام:
	friend صنف BlockParamBase;

	Block(SuperBlock *parent, ثابت حرف *name);
	virtual ~Block() = افتراضي;

	// no copy, assignment, move, move assignment
	Block(ثابت Block &) = delete;
	Block &operator=(ثابت Block &) = delete;
	Block(Block &&) = delete;
	Block &operator=(Block &&) = delete;

	فراغ getName(حرف *name, size_t n);

	virtual فراغ updateParams();

	virtual فراغ setDt(عائم dt) { _dt = dt; }
	عائم getDt() { أرجع _dt; }

محمي:

	virtual فراغ updateParamsSubclass() {}

	SuperBlock *getParent() { أرجع _parent; }
	List<BlockParamBase *> &getParams() { أرجع _params; }

	ثابت حرف *_name;
	SuperBlock *_parent;
	عائم _dt{0.0f};

	List<BlockParamBase *> _params;
};

صنف __EXPORT SuperBlock :
	عام Block
{
عام:
	friend صنف Block;

	SuperBlock(SuperBlock *parent, ثابت حرف *name) : Block(parent, name) {}
	~SuperBlock() = افتراضي;

	// no copy, assignment, move, move assignment
	SuperBlock(ثابت SuperBlock &) = delete;
	SuperBlock &operator=(ثابت SuperBlock &) = delete;
	SuperBlock(SuperBlock &&) = delete;
	SuperBlock &operator=(SuperBlock &&) = delete;

	فراغ setDt(عائم dt) override;

	فراغ updateParams() override
	{
		Block::updateParams();

		إذا (getChildren().getHead() != nullptr) { updateChildParams(); }
	}

محمي:
	List<Block *> &getChildren() { أرجع _children; }
	فراغ updateChildParams();

	List<Block *> _children;
};


} // namespace control
