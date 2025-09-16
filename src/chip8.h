#ifndef CHIP_8_H
#define CHIP_8_H

#include <cstdint>
#include <bitset>
#include <array>

class Chip8 {
private:
	static const unsigned int	VIDEO_WIDTH		= 64;
	static const unsigned int	VIDEO_HEIGHT	= 32;
	const unsigned int			START_ADDR		= 0x200;
	const unsigned int			FONT_START_ADDR	= 0x050;
	const uint8_t				FONTSET[80]		= {
		0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
		0x20, 0x60, 0x20, 0x20, 0x70, // 1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
		0x90, 0x90, 0xF0, 0x10, 0x10, // 4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
		0xF0, 0x10, 0x20, 0x40, 0x40, // 7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
		0xF0, 0x90, 0xF0, 0x90, 0x90, // A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
		0xF0, 0x80, 0x80, 0x80, 0xF0, // C
		0xE0, 0x90, 0x90, 0x90, 0xE0, // D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
		0xF0, 0x80, 0xF0, 0x80, 0x80  // F
	};

	std::array<uint8_t, 4096>	memory;
	std::array<uint8_t, 16>		registers;
	uint16_t					index_register;
	uint16_t					program_counter;
	uint16_t					opcode;
	std::array<uint16_t, 16>	stack;
	uint8_t						stack_pointer;
	uint8_t						delay_timer;
	uint8_t						sound_timer;

	void init();
	bool check_key(uint8_t key);

public:
	Chip8();
	~Chip8();

	std::bitset<16>								keypad;
	std::bitset<(VIDEO_WIDTH * VIDEO_HEIGHT)>	display;
	bool										drawn_to = false; 

	void emulate_cycle();
	bool load(const char* filename);

private:
	/* ARRAY OF POINTERS TO INSTRUCTIONS */
	void table0();
	void table8();
	void tableE();
	void tableF();

	/* INSTRUCTIONS */
	/* 00E prefixed */
	void op_00E0();
	void op_00EE();

	/* entirely unique  */
	void op_1NNN();
	void op_2NNN();
	void op_3XNN();
	void op_4XNN();
	void op_5XY0();
	void op_6XNN();
	void op_7XNN();
	void op_9XY0();
	void op_ANNN();
	void op_BNNN();
	void op_CXNN();
	void op_DXYN();

	/* 8 prefixed */
	void op_8XY0();
	void op_8XY1();
	void op_8XY2();
	void op_8XY3();
	void op_8XY4();
	void op_8XY5();
	void op_8XY6();
	void op_8XY7();
	void op_8XYE();

	/* E prefixed */
	void op_EXA1();
	void op_EX9E();

	/* F prefixed */
	void op_FX07();
	void op_FX0A();
	void op_FX15();
	void op_FX18();
	void op_FX1E();
	void op_FX29();
	void op_FX33();
	void op_FX55();
	void op_FX65();

	// typedef for pointer to member function
	typedef void (Chip8::*Chip8Func)();
	Chip8Func _table[0xF + 1]{};
	Chip8Func _table0[0xE + 1]{};
	Chip8Func _table8[0xE + 1]{};
	Chip8Func _tableE[0xE + 1]{};
	Chip8Func _tableF[0x65 + 1]{};
};
#endif CHIP_8_H