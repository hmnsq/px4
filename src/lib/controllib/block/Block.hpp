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

#شامل <containers/List.hpp>
#شامل <controllib/block/BlockParam.hpp>
#شامل <cstdint>

فضاینام control
{

ایستا constexpr uint8_t maxChildrenPerBlock = 100;
ایستا constexpr uint8_t maxParamsPerBlock = 110;
ایستا constexpr uint8_t blockNameLengthMax = 40;

// forward declaration
رده BlockParamBase;
رده SuperBlock;

/**
 */
رده __EXPORT Block : عمومی List_گره<Block *>
{
عمومی:
	friend رده BlockParamBase;

	Block(SuperBlock *parent, ثابت نویسه *name);
	مجازی ~Block() = پیشفرض;

	// no copy, assignment, move, move assignment
	Block(ثابت Block &) = delete;
	Block &operator=(ثابت Block &) = delete;
	Block(Block &&) = delete;
	Block &operator=(Block &&) = delete;

	تهی getName(نویسه *name, size_t n);

	مجازی تهی updateParams();

	مجازی تهی setDt(اعشاری dt) { _dt = dt; }
	اعشاری getDt() { بازگشت _dt; }

محافظتشده:

	مجازی تهی updateParamsSubclass() {}

	SuperBlock *getParent() { بازگشت _parent; }
	List<BlockParamBase *> &getParams() { بازگشت _params; }

	ثابت نویسه *_name;
	SuperBlock *_parent;
	اعشاری _dt{0.0f};

	List<BlockParamBase *> _params;
};

رده __EXPORT SuperBlock :
	عمومی Block
{
عمومی:
	friend رده Block;

	SuperBlock(SuperBlock *parent, ثابت نویسه *name) : Block(parent, name) {}
	~SuperBlock() = پیشفرض;

	// no copy, assignment, move, move assignment
	SuperBlock(ثابت SuperBlock &) = delete;
	SuperBlock &operator=(ثابت SuperBlock &) = delete;
	SuperBlock(SuperBlock &&) = delete;
	SuperBlock &operator=(SuperBlock &&) = delete;

	تهی setDt(اعشاری dt) override;

	تهی updateParams() override
	{
		Block::updateParams();

		اگر (getChildren().getHead() != nullptr) { updateChildParams(); }
	}

محافظتشده:
	List<Block *> &getChildren() { بازگشت _children; }
	تهی updateChildParams();

	List<Block *> _children;
};


} // namespace control
