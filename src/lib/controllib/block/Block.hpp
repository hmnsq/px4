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

#ଅନ୍ତର୍ଭାଵ <containers/List.hpp>
#ଅନ୍ତର୍ଭାଵ <controllib/block/BlockParam.hpp>
#ଅନ୍ତର୍ଭାଵ <cstdint>

ନାମାକାଶ control
{

ସ୍ଥିର constexpr uint8_t maxChildrenPerBlock = 100;
ସ୍ଥିର constexpr uint8_t maxParamsPerBlock = 110;
ସ୍ଥିର constexpr uint8_t blockNameLengthMax = 40;

// forward declaration
ଶ୍ରେଣୀ BlockParamBase;
ଶ୍ରେଣୀ SuperBlock;

/**
 */
ଶ୍ରେଣୀ __EXPORT Block : ସର୍ବସାଧାରଣ List_ଗଣ୍ଠି<Block *>
{
ସର୍ବସାଧାରଣ:
	friend ଶ୍ରେଣୀ BlockParamBase;

	Block(SuperBlock *parent, ଅଚଳ ଅକ୍ଷର *name);
	ଆଭାସୀ ~Block() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

	// no copy, assignment, move, move assignment
	Block(ଅଚଳ Block &) = delete;
	Block &operator=(ଅଚଳ Block &) = delete;
	Block(Block &&) = delete;
	Block &operator=(Block &&) = delete;

	ଶୂନ୍ୟ getName(ଅକ୍ଷର *name, size_t n);

	ଆଭାସୀ ଶୂନ୍ୟ updateParams();

	ଆଭାସୀ ଶୂନ୍ୟ setDt(ଦଶମିକ dt) { _dt = dt; }
	ଦଶମିକ getDt() { ଫେରନ୍ତୁ _dt; }

ସୁରକ୍ଷିତ:

	ଆଭାସୀ ଶୂନ୍ୟ updateParamsSubclass() {}

	SuperBlock *getParent() { ଫେରନ୍ତୁ _parent; }
	List<BlockParamBase *> &getParams() { ଫେରନ୍ତୁ _params; }

	ଅଚଳ ଅକ୍ଷର *_name;
	SuperBlock *_parent;
	ଦଶମିକ _dt{0.0f};

	List<BlockParamBase *> _params;
};

ଶ୍ରେଣୀ __EXPORT SuperBlock :
	ସର୍ବସାଧାରଣ Block
{
ସର୍ବସାଧାରଣ:
	friend ଶ୍ରେଣୀ Block;

	SuperBlock(SuperBlock *parent, ଅଚଳ ଅକ୍ଷର *name) : Block(parent, name) {}
	~SuperBlock() = ପୂର୍ବନିର୍ଦ୍ଧାରିତ;

	// no copy, assignment, move, move assignment
	SuperBlock(ଅଚଳ SuperBlock &) = delete;
	SuperBlock &operator=(ଅଚଳ SuperBlock &) = delete;
	SuperBlock(SuperBlock &&) = delete;
	SuperBlock &operator=(SuperBlock &&) = delete;

	ଶୂନ୍ୟ setDt(ଦଶମିକ dt) override;

	ଶୂନ୍ୟ updateParams() override
	{
		Block::updateParams();

		ଯଦି (getChildren().getHead() != nullptr) { updateChildParams(); }
	}

ସୁରକ୍ଷିତ:
	List<Block *> &getChildren() { ଫେରନ୍ତୁ _children; }
	ଶୂନ୍ୟ updateChildParams();

	List<Block *> _children;
};


} // namespace control
