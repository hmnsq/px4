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

#அந்தர்பாவ <containers/List.hpp>
#அந்தர்பாவ <controllib/block/BlockParam.hpp>
#அந்தர்பாவ <cstdint>

நாமாகாஶ control
{

நிலையான constexpr uint8_t maxChildrenPerBlock = 100;
நிலையான constexpr uint8_t maxParamsPerBlock = 110;
நிலையான constexpr uint8_t blockNameLengthMax = 40;

// forward declaration
வகுப்பு BlockParamBase;
வகுப்பு SuperBlock;

/**
 */
வகுப்பு __EXPORT Block : பொது List_முனை<Block *>
{
பொது:
	friend வகுப்பு BlockParamBase;

	Block(SuperBlock *parent, மாறிலி எழுத்து *name);
	மெய்நிகர் ~Block() = இயல்பு;

	// no copy, assignment, move, move assignment
	Block(மாறிலி Block &) = delete;
	Block &operator=(மாறிலி Block &) = delete;
	Block(Block &&) = delete;
	Block &operator=(Block &&) = delete;

	வெறுமை getName(எழுத்து *name, size_t n);

	மெய்நிகர் வெறுமை updateParams();

	மெய்நிகர் வெறுமை setDt(தசம dt) { _dt = dt; }
	தசம getDt() { திரும்பு _dt; }

பாதுகாக்கப்பட்ட:

	மெய்நிகர் வெறுமை updateParamsSubclass() {}

	SuperBlock *getParent() { திரும்பு _parent; }
	List<BlockParamBase *> &getParams() { திரும்பு _params; }

	மாறிலி எழுத்து *_name;
	SuperBlock *_parent;
	தசம _dt{0.0f};

	List<BlockParamBase *> _params;
};

வகுப்பு __EXPORT SuperBlock :
	பொது Block
{
பொது:
	friend வகுப்பு Block;

	SuperBlock(SuperBlock *parent, மாறிலி எழுத்து *name) : Block(parent, name) {}
	~SuperBlock() = இயல்பு;

	// no copy, assignment, move, move assignment
	SuperBlock(மாறிலி SuperBlock &) = delete;
	SuperBlock &operator=(மாறிலி SuperBlock &) = delete;
	SuperBlock(SuperBlock &&) = delete;
	SuperBlock &operator=(SuperBlock &&) = delete;

	வெறுமை setDt(தசம dt) override;

	வெறுமை updateParams() override
	{
		Block::updateParams();

		எனில் (getChildren().getHead() != nullptr) { updateChildParams(); }
	}

பாதுகாக்கப்பட்ட:
	List<Block *> &getChildren() { திரும்பு _children; }
	வெறுமை updateChildParams();

	List<Block *> _children;
};


} // namespace control
