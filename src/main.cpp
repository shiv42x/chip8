#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "chip8.h"

// Override base class with your custom functionality
class Example : public olc::PixelGameEngine
{
public:
	Chip8 chip;
	Example() : chip(600)
	{
		// Name your application
		sAppName = "Example";
	}

public:
	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
		chip.load("roms/Pong (alt).ch8");
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// Collect input
		if (GetKey(olc::Key::X).bPressed) chip.keypad.set(0, 0b1);
		if (GetKey(olc::Key::K1).bPressed) chip.keypad.set(1, 0b1);
		if (GetKey(olc::Key::K2).bPressed) chip.keypad.set(2, 0b1);
		if (GetKey(olc::Key::K3).bPressed) chip.keypad.set(3, 0b1);
		if (GetKey(olc::Key::Q).bPressed) chip.keypad.set(4, 0b1);
		if (GetKey(olc::Key::W).bPressed) chip.keypad.set(5, 0b1);
		if (GetKey(olc::Key::E).bPressed) chip.keypad.set(6, 0b1);
		if (GetKey(olc::Key::A).bPressed) chip.keypad.set(7, 0b1);
		if (GetKey(olc::Key::S).bPressed) chip.keypad.set(8, 0b1);
		if (GetKey(olc::Key::D).bPressed) chip.keypad.set(9, 0b1);
		if (GetKey(olc::Key::Z).bPressed) chip.keypad.set(10, 0b1);
		if (GetKey(olc::Key::C).bPressed) chip.keypad.set(11, 0b1);
		if (GetKey(olc::Key::K4).bPressed) chip.keypad.set(12, 0b1);
		if (GetKey(olc::Key::R).bPressed) chip.keypad.set(13, 0b1);
		if (GetKey(olc::Key::F).bPressed) chip.keypad.set(14, 0b1);
		if (GetKey(olc::Key::V).bPressed) chip.keypad.set(15, 0b1);

		if (GetKey(olc::Key::X).bReleased) chip.keypad.set(0, 0b0);
		if (GetKey(olc::Key::K1).bReleased) chip.keypad.set(1, 0b0);
		if (GetKey(olc::Key::K2).bReleased) chip.keypad.set(2, 0b0);
		if (GetKey(olc::Key::K3).bReleased) chip.keypad.set(3, 0b0);
		if (GetKey(olc::Key::Q).bReleased) chip.keypad.set(4, 0b0);
		if (GetKey(olc::Key::W).bReleased) chip.keypad.set(5, 0b0);
		if (GetKey(olc::Key::E).bReleased) chip.keypad.set(6, 0b0);
		if (GetKey(olc::Key::A).bReleased) chip.keypad.set(7, 0b0);
		if (GetKey(olc::Key::S).bReleased) chip.keypad.set(8, 0b0);
		if (GetKey(olc::Key::D).bReleased) chip.keypad.set(9, 0b0);
		if (GetKey(olc::Key::Z).bReleased) chip.keypad.set(10, 0b0);
		if (GetKey(olc::Key::C).bReleased) chip.keypad.set(11, 0b0);
		if (GetKey(olc::Key::K4).bReleased) chip.keypad.set(12, 0b0);
		if (GetKey(olc::Key::R).bReleased) chip.keypad.set(13, 0b0);
		if (GetKey(olc::Key::F).bReleased) chip.keypad.set(14, 0b0);
		if (GetKey(olc::Key::V).bReleased) chip.keypad.set(15, 0b0);

		chip.cycle();
		for (int i = 0; i < 2048; i++) 
		{
			int x = i % 64;
			int y = i / 64;
			if (chip.display[i])
				Draw(x, y, olc::GREY);
			else
				Draw(x, y, olc::BLACK);
		}
		return true;
	}
};

int main()
{
	Example demo;
	if (demo.Construct(64, 32, 16, 16))
		demo.Start();
	return 0;
}