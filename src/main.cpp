#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "chip8.h"

// Override base class with your custom functionality
class Example : public olc::PixelGameEngine
{
public:
	Chip8 chip;
	Example()
	{
		// Name your application
		sAppName = "Example";
	}

public:
	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
		chip.load("roms/test_opcode.ch8");
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// Called once per frame, draws random coloured pixels
		chip.emulate_cycle();
		for (int i = 0; i < 2048; i++) 
		{
			if (chip.display[i])
			{
				int x = i % 64;
				int y = i / 64;
				Draw(x, y, olc::GREY);
			}
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