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

#ಅನ್ತರ್ಭಾವ <containers/List.hpp>
#ಅನ್ತರ್ಭಾವ <controllib/block/BlockParam.hpp>
#ಅನ್ತರ್ಭಾವ <cstdint>

ನಾಮಾಕಾಶ control
{

ಸ್ಥಿರ constexpr uint8_t maxChildrenPerBlock = 100;
ಸ್ಥಿರ constexpr uint8_t maxParamsPerBlock = 110;
ಸ್ಥಿರ constexpr uint8_t blockNameLengthMax = 40;

// forward declaration
ವರ್ಗ BlockParamBase;
ವರ್ಗ SuperBlock;

/**
 */
ವರ್ಗ __EXPORT Block : ಸಾರ್ವಜನಿಕ List_ಗಂಟು<Block *>
{
ಸಾರ್ವಜನಿಕ:
	friend ವರ್ಗ BlockParamBase;

	Block(SuperBlock *parent, ಧ್ರುವ ವರ್ಣ *name);
	ಆಭಾಸ ~Block() = ಸ್ವಭಾವ;

	// no copy, assignment, move, move assignment
	Block(ಧ್ರುವ Block &) = delete;
	Block &operator=(ಧ್ರುವ Block &) = delete;
	Block(Block &&) = delete;
	Block &operator=(Block &&) = delete;

	ಶೂನ್ಯ getName(ವರ್ಣ *name, size_t n);

	ಆಭಾಸ ಶೂನ್ಯ updateParams();

	ಆಭಾಸ ಶೂನ್ಯ setDt(ಭಿನ್ನ dt) { _dt = dt; }
	ಭಿನ್ನ getDt() { ಪ್ರತ್ಯಾಗಮ _dt; }

ರಕ್ಷಿತ:

	ಆಭಾಸ ಶೂನ್ಯ updateParamsSubclass() {}

	SuperBlock *getParent() { ಪ್ರತ್ಯಾಗಮ _parent; }
	List<BlockParamBase *> &getParams() { ಪ್ರತ್ಯಾಗಮ _params; }

	ಧ್ರುವ ವರ್ಣ *_name;
	SuperBlock *_parent;
	ಭಿನ್ನ _dt{0.0f};

	List<BlockParamBase *> _params;
};

ವರ್ಗ __EXPORT SuperBlock :
	ಸಾರ್ವಜನಿಕ Block
{
ಸಾರ್ವಜನಿಕ:
	friend ವರ್ಗ Block;

	SuperBlock(SuperBlock *parent, ಧ್ರುವ ವರ್ಣ *name) : Block(parent, name) {}
	~SuperBlock() = ಸ್ವಭಾವ;

	// no copy, assignment, move, move assignment
	SuperBlock(ಧ್ರುವ SuperBlock &) = delete;
	SuperBlock &operator=(ಧ್ರುವ SuperBlock &) = delete;
	SuperBlock(SuperBlock &&) = delete;
	SuperBlock &operator=(SuperBlock &&) = delete;

	ಶೂನ್ಯ setDt(ಭಿನ್ನ dt) override;

	ಶೂನ್ಯ updateParams() override
	{
		Block::updateParams();

		ಯದಿ (getChildren().getHead() != nullptr) { updateChildParams(); }
	}

ರಕ್ಷಿತ:
	List<Block *> &getChildren() { ಪ್ರತ್ಯಾಗಮ _children; }
	ಶೂನ್ಯ updateChildParams();

	List<Block *> _children;
};


} // namespace control
