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

#അന്തര്ഭാവ <containers/List.hpp>
#അന്തര്ഭാവ <controllib/block/BlockParam.hpp>
#അന്തര്ഭാവ <cstdint>

നാമാകാശ control
{

സ്ഥിതം constexpr uint8_t maxChildrenPerBlock = 100;
സ്ഥിതം constexpr uint8_t maxParamsPerBlock = 110;
സ്ഥിതം constexpr uint8_t blockNameLengthMax = 40;

// forward declaration
വർഗ്ഗം BlockParamBase;
വർഗ്ഗം SuperBlock;

/**
 */
വർഗ്ഗം __EXPORT Block : പൊതു List_കണ്ണി<Block *>
{
പൊതു:
	friend വർഗ്ഗം BlockParamBase;

	Block(SuperBlock *parent, സ്ഥിരം അക്ഷരം *name);
	മായാ ~Block() = സ്ഥിരസ്ഥിതി;

	// no copy, assignment, move, move assignment
	Block(സ്ഥിരം Block &) = delete;
	Block &operator=(സ്ഥിരം Block &) = delete;
	Block(Block &&) = delete;
	Block &operator=(Block &&) = delete;

	ശൂന്യം getName(അക്ഷരം *name, size_t n);

	മായാ ശൂന്യം updateParams();

	മായാ ശൂന്യം setDt(ദശാംശം dt) { _dt = dt; }
	ദശാംശം getDt() { തിരികെ _dt; }

സംരക്ഷിത:

	മായാ ശൂന്യം updateParamsSubclass() {}

	SuperBlock *getParent() { തിരികെ _parent; }
	List<BlockParamBase *> &getParams() { തിരികെ _params; }

	സ്ഥിരം അക്ഷരം *_name;
	SuperBlock *_parent;
	ദശാംശം _dt{0.0f};

	List<BlockParamBase *> _params;
};

വർഗ്ഗം __EXPORT SuperBlock :
	പൊതു Block
{
പൊതു:
	friend വർഗ്ഗം Block;

	SuperBlock(SuperBlock *parent, സ്ഥിരം അക്ഷരം *name) : Block(parent, name) {}
	~SuperBlock() = സ്ഥിരസ്ഥിതി;

	// no copy, assignment, move, move assignment
	SuperBlock(സ്ഥിരം SuperBlock &) = delete;
	SuperBlock &operator=(സ്ഥിരം SuperBlock &) = delete;
	SuperBlock(SuperBlock &&) = delete;
	SuperBlock &operator=(SuperBlock &&) = delete;

	ശൂന്യം setDt(ദശാംശം dt) override;

	ശൂന്യം updateParams() override
	{
		Block::updateParams();

		എങ്കിൽ (getChildren().getHead() != nullptr) { updateChildParams(); }
	}

സംരക്ഷിത:
	List<Block *> &getChildren() { തിരികെ _children; }
	ശൂന്യം updateChildParams();

	List<Block *> _children;
};


} // namespace control
