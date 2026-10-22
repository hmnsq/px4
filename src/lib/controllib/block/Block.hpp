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

#অন্তর্ভাৱ <containers/List.hpp>
#অন্তর্ভাৱ <controllib/block/BlockParam.hpp>
#অন্তর্ভাৱ <cstdint>

নামাকাশ control
{

স্থির constexpr uint8_t maxChildrenPerBlock = 100;
স্থির constexpr uint8_t maxParamsPerBlock = 110;
স্থির constexpr uint8_t blockNameLengthMax = 40;

// forward declaration
শ্ৰেণী BlockParamBase;
শ্ৰেণী SuperBlock;

/**
 */
শ্ৰেণী __EXPORT Block : সার্ৱজনিক List_গ্ৰন্থি<Block *>
{
সার্ৱজনিক:
	friend শ্ৰেণী BlockParamBase;

	Block(SuperBlock *parent, ধ্রুৱ ৱর্ণ *name);
	আভাস ~Block() = স্ৱভাৱ;

	// no copy, assignment, move, move assignment
	Block(ধ্রুৱ Block &) = delete;
	Block &operator=(ধ্রুৱ Block &) = delete;
	Block(Block &&) = delete;
	Block &operator=(Block &&) = delete;

	শূন্য getName(ৱর্ণ *name, size_t n);

	আভাস শূন্য updateParams();

	আভাস শূন্য setDt(ভিন্ন dt) { _dt = dt; }
	ভিন্ন getDt() { ঘূৰাই _dt; }

রক্ষিত:

	আভাস শূন্য updateParamsSubclass() {}

	SuperBlock *getParent() { ঘূৰাই _parent; }
	List<BlockParamBase *> &getParams() { ঘূৰাই _params; }

	ধ্রুৱ ৱর্ণ *_name;
	SuperBlock *_parent;
	ভিন্ন _dt{0.0f};

	List<BlockParamBase *> _params;
};

শ্ৰেণী __EXPORT SuperBlock :
	সার্ৱজনিক Block
{
সার্ৱজনিক:
	friend শ্ৰেণী Block;

	SuperBlock(SuperBlock *parent, ধ্রুৱ ৱর্ণ *name) : Block(parent, name) {}
	~SuperBlock() = স্ৱভাৱ;

	// no copy, assignment, move, move assignment
	SuperBlock(ধ্রুৱ SuperBlock &) = delete;
	SuperBlock &operator=(ধ্রুৱ SuperBlock &) = delete;
	SuperBlock(SuperBlock &&) = delete;
	SuperBlock &operator=(SuperBlock &&) = delete;

	শূন্য setDt(ভিন্ন dt) override;

	শূন্য updateParams() override
	{
		Block::updateParams();

		যদি (getChildren().getHead() != nullptr) { updateChildParams(); }
	}

রক্ষিত:
	List<Block *> &getChildren() { ঘূৰাই _children; }
	শূন্য updateChildParams();

	List<Block *> _children;
};


} // namespace control
