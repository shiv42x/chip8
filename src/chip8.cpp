#include <iostream>
#include <fstream>
#include <streambuf>
#include <vector>
#include <random>
#include <chrono>
#include "time.h"

#include "chip8.h"

void Chip8::init()
{
	program_counter = 0x200;
	index_register = 0;	
	stack_pointer = 0;
	delay_timer = 0;
	sound_timer = 0;

	display.reset();
	keypad.reset();

	for (int i = 0; i < 16; i++) 
	{
		stack[i] = 0;
		registers[i] = 0;
	}

	for (int i = 0; i < 4096; i++)
	{
		memory[i] = 0;
	}

	// font will live in 050-09F
	for (int i = 0; i < 80; i++)
	{
		memory[0x050 + i] = FONTSET[i];
	}

	// register instruction's function at idx given by opcode
	_table[0x0] = &Chip8::table0;
	_table[0x1] = &Chip8::op_1NNN;
	_table[0x2] = &Chip8::op_2NNN;
	_table[0x3] = &Chip8::op_3XNN;
	_table[0x4] = &Chip8::op_4XNN;
	_table[0x5] = &Chip8::op_5XY0;
	_table[0x6] = &Chip8::op_6XNN;
	_table[0x7] = &Chip8::op_7XNN;
	_table[0x8] = &Chip8::table8;
	_table[0x9] = &Chip8::op_9XY0;
	_table[0xA] = &Chip8::op_ANNN;
	_table[0xB] = &Chip8::op_BNNN;
	_table[0xC] = &Chip8::op_CXNN;
	_table[0xD] = &Chip8::op_DXYN;
	_table[0xE] = &Chip8::tableE;
	_table[0xF] = &Chip8::tableF;

	_table0[0x0] = &Chip8::op_00E0;
	_table0[0xE] = &Chip8::op_00EE;

	_table8[0x0] = &Chip8::op_8XY0;
	_table8[0x1] = &Chip8::op_8XY1;
	_table8[0x2] = &Chip8::op_8XY2;
	_table8[0x3] = &Chip8::op_8XY3;
	_table8[0x4] = &Chip8::op_8XY4;
	_table8[0x5] = &Chip8::op_8XY5;
	_table8[0x6] = &Chip8::op_8XY6;
	_table8[0x7] = &Chip8::op_8XY7;
	_table8[0xE] = &Chip8::op_8XYE;

	_tableE[0x1] = &Chip8::op_EXA1;
	_tableE[0xE] = &Chip8::op_EX9E;

	_tableF[0x07] = &Chip8::op_FX07;
	_tableF[0x0A] = &Chip8::op_FX0A;
	_tableF[0x15] = &Chip8::op_FX15;
	_tableF[0x18] = &Chip8::op_FX18;
	_tableF[0x1E] = &Chip8::op_FX1E;
	_tableF[0x29] = &Chip8::op_FX29;
	_tableF[0x33] = &Chip8::op_FX33;
	_tableF[0x55] = &Chip8::op_FX55;
	_tableF[0x65] = &Chip8::op_FX65;
}

Chip8::Chip8() 
{
	init();
}

Chip8::~Chip8()
{

}

void Chip8::emulate_cycle() 
{
	opcode = (memory[program_counter] << 8u) | memory[program_counter + 1];
	program_counter += 2;

	((*this).*(_table[(opcode & 0xF000u) >> 12u]))();

	if (delay_timer > 0)
		--delay_timer;
	if (sound_timer > 0)
		--sound_timer;
}

bool Chip8::load(const char* filename)
{
	init();

	std::ifstream file(filename, std::ios::binary);

	if (!file.is_open()) 
		return false;

	file.seekg(0, std::ios::end);
	std::streamsize size = file.tellg();
	file.seekg(0, std::ios::beg);

	if (size > (4096 - START_ADDR)) 
		return false;	
	
	std::vector<uint8_t> buffer(size);
	if (!file.read(reinterpret_cast<char*>(buffer.data()), size))
		return false;

	std::copy(buffer.begin(), buffer.end(), memory.begin() + START_ADDR);
	return true;
}

bool Chip8::check_key(uint8_t key)
{
	if (key < 16)
		return keypad.test(key);
	else
		return false;
}

void Chip8::table0()
{
	((*this).*(_table0[opcode & 0x000Fu]))();
}

void Chip8::table8()
{
	((*this).*(_table8[opcode & 0x000Fu]))();
}

void Chip8::tableE()
{
	((*this).*(_tableE[opcode & 0x000Fu]))();
}

void Chip8::tableF()
{
	((*this).*(_tableF[opcode & 0x00FFu]))();
}

/* clear screen */
void Chip8::op_00E0()
{
	display.reset();
}

/* return from subroutine */
void Chip8::op_00EE()
{
	program_counter = stack[--stack_pointer];
}

/* jump to nnn */
void Chip8::op_1NNN()
{
	program_counter = (opcode & 0x0FFFu);
}

/* call subroutine at nnn */
void Chip8::op_2NNN()
{
	stack[stack_pointer++] = program_counter;
	program_counter = (opcode & 0x0FFFu);
}

/* skip next instruction if register x = NN */
void Chip8::op_3XNN()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t nn = (opcode & 0x00FFu);
	if (registers[Vx] == nn)
	{
		program_counter += 2;
	}
}

/* skip next instruction if register x != NN */
void Chip8::op_4XNN()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t nn = (opcode & 0x00FFu);
	if (registers[Vx] != nn)
	{
		program_counter += 2;
	}
}

/* skip next instruction if register x == y */
void Chip8::op_5XY0()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	if (registers[Vx] == registers[Vy])
	{
		program_counter += 2;
	}
}

/* set register x to nn */
void Chip8::op_6XNN()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = (opcode & 0x00FFu);
	registers[Vx] = byte;
}

/* add nn to register x */
void Chip8::op_7XNN()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = (opcode & 0x00FFu);
	registers[Vx] += byte;
}

/* skip next instruction if register x != y */
void Chip8::op_9XY0()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	if (registers[Vx] != registers[Vy])
	{
		program_counter += 2;
	}
}

/* set index register to nnn */
void Chip8::op_ANNN()
{
	index_register = (opcode & 0x0FFFu);
}

/* jump to nnn + register 0 */
void Chip8::op_BNNN()
{
	program_counter = (opcode & 0x0FFFu) + registers[0];
}

/* set regsiter x to random byte AND nn */
void Chip8::op_CXNN()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t nn = (opcode & 0x00FFu);
	registers[Vx] = (nn & (std::rand() % 0xFFu));
}

/* draw n-byte sprite at i starting at value in register x and y wrapped around*/
void Chip8::op_DXYN()
{
	uint8_t x = registers[(opcode & 0x0F00u) >> 8u] % VIDEO_WIDTH;
	uint8_t y = registers[(opcode & 0x00F0u) >> 4u] % VIDEO_HEIGHT;
	uint8_t height = (opcode & 0x000Fu);
	
	registers[0xF] = 0;

	for (unsigned int row = 0; row < height; row++)
	{
		uint8_t byte = memory[index_register + row];
		for (unsigned int col = 0; col < 8; col++)
		{
			uint8_t sprite_pixel = (byte & (0x80u >> col));
			if (sprite_pixel)
			{
				uint16_t idx = ((x + col) + VIDEO_WIDTH * (y + row)) % (VIDEO_WIDTH * VIDEO_HEIGHT);
				if (display.test(idx))
					registers[0xF] = 1;
				display.flip(idx);
			}
		}
	}
}

/* set register x to y */
void Chip8::op_8XY0()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	registers[Vx] = registers[Vy];
}

/* set register x to x OR y */
void Chip8::op_8XY1()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	registers[Vx] = registers[Vx] | registers[Vy];
}

/* set register x to x AND y */
void Chip8::op_8XY2()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	registers[Vx] = registers[Vx] & registers[Vy];
}

/* set register x to x XOR y */
void Chip8::op_8XY3()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	registers[Vx] = registers[Vx] ^ registers[Vy];
}

/* set register x to x plus y, set register f to carry */
void Chip8::op_8XY4()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;
	uint16_t result = registers[Vx] + registers[Vy];

	if (result > 0xFF)
		registers[0xF] = 1;
	else
		registers[0xF] = 0;

	registers[Vx] = result & 0xFFu;
}

/* set register x to x subtract y, set register f to NOT borrow */
void Chip8::op_8XY5()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (registers[Vx] > registers[Vy])
		registers[0xF] = 1;
	else
		registers[0xF] = 0;

	registers[Vx] -= registers[Vy];
}

/* set register x to x divide 2, set register f to remainder */
void Chip8::op_8XY6()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	registers[0xF] = (registers[Vx] & 0x1u);

	registers[Vx] >>= 1;
}

/* set regsiter x to y subtract x, set register f to NOT borrow */
void Chip8::op_8XY7()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (registers[Vy] > registers[Vx])
		registers[0xF] = 1;
	else
		registers[0xF] = 0;

	registers[Vx] = registers[Vy] - registers[Vx];
}

/* set register x to x multiply 2, set register f to overflow(?) */
void Chip8::op_8XYE()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	registers[0xF] = (registers[Vx] & 0x80u) >> 7u;

	registers[Vx] <<= 1;
}

/* skip next instruction if key in Vx pressed */
void Chip8::op_EX9E()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t key = registers[Vx];

	if (check_key(key))
		program_counter += 2;
}

/* skip next instruction if key in Vx not pressed */
void Chip8::op_EXA1()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t key = registers[Vx];

	if (!check_key(key))
		program_counter += 2;
}

/* set Vx to delay timer value */
void Chip8::op_FX07()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	registers[Vx] = delay_timer;
}

/* wait for key press, store value of key in Vx */
void Chip8::op_FX0A()
{
 	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	for (uint8_t i = 0; i < 16; i++)
	{
		if (check_key(i))
		{
			registers[Vx] = i;
			return;
		}
	}
	program_counter -= 2;
}

/* set delay timer to value in register x */
void Chip8::op_FX15()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	delay_timer = registers[Vx];
}

/* set sound timer to value in register x */
void Chip8::op_FX18()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	sound_timer = registers[Vx];
}

/* set index register to value in index register plus register x */
void Chip8::op_FX1E()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	index_register += registers[Vx];
}

/* set index register to location of sprite in register x */
void Chip8::op_FX29()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	index_register = FONT_START_ADDR + (5 * registers[Vx]);
}

/* store bcd representation of register x in index register, +1, +2 */
void Chip8::op_FX33()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t value = registers[Vx];

	memory[index_register + 2] = value % 10;
	value /= 10;

	memory[index_register + 1] = value % 10;
	value /= 10;

	memory[index_register] = value % 10;
}

/* store register 0 through x starting at i */
void Chip8::op_FX55()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	for (uint8_t i = 0; i <= Vx; i++)
	{
		memory[index_register + i] = registers[i];
	}
}

/* store into register 0 through x starting at i */
void Chip8::op_FX65()
{
	uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	for (uint8_t i = 0; i <= Vx; i++)
	{
		registers[i] = memory[index_register + i];
	}
}