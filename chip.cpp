// Chip8Emulator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "chip.hpp"

#include <iostream>
#include <cstring>
#include <fstream>

Chip8::Chip8() 
 : stack{0}, display{0}, memory{0}, V{0}, keys{false},
	pc(PC_START), I(0), sp(0),
	st(0), dt(0),
	drawFlag(false)
{
	memcpy(memory, FONT_SET, sizeof(FONT_SET));
}

bool Chip8::loadGame(const char* path)
{
    std::ifstream file(path, std::ios::in | std::ios::binary | std::ios::ate);

    if (!file.is_open()) 
    {
        std::cerr << "ERROR: File Does Not Exist" << std::endl;
        return false;
    }

	if (file.tellg() > MEM_SIZE - PC_START)
	{
		std::cerr << "ERROR: File Size Too Large" << std::endl;
		return false;
	}

	uint8_t buffer[4096];

    file.seekg(0, std::ios::beg);
    file.read(reinterpret_cast<char*>(buffer), sizeof(buffer) / sizeof(uint8_t));


	for (int i = 0; i < file.gcount(); i++)
		memory[pc + i] = buffer[i];

	file.close();

    return true;
}

void Chip8::executeInstruction()
{
	// Fetch Opcode
	uint16_t opcode = memory[pc] << 8 | memory[pc + 1];

	// Get X and Y register values for certain CPU instructions
	// Such as 0x7XY0, 0x8XY0, and 0x9XY0
	uint8_t Y = (opcode & 0x00F0) >> 4;
	uint8_t X = (opcode & 0x0F00) >> 8;
	
	// Used for CPU instructions containing addresses, 8-bit constants, and 4-bit constants
	// Such as 0x1NNN, 0x2NNN, 0x6XNN
	uint16_t NNN = (opcode & 0x0FFF);

	pc += 2;

	drawFlag = false;

	// Execute instructions based on opcode
	switch (opcode & 0xF000) 
	{
		case 0x0000:
			// Clear Screen
			if ((opcode & 0x00FF) == 0xE0) 
			{
				memset(display, 0, sizeof(display));
			} 
			else if ((opcode & 0x00FF) == 0xEE)
			{ 
				// Return from subroutine
				stack[sp] = 0;
				--sp;
				pc = stack[sp];
			}
			break;

		// (0x1NNN) Jumps to address NNN
		case 0x1000:
			pc = NNN;
			break;

		// (0x2NNN) Do subroutine at address NNN
		// push pc onto stack and increment stack pointer
		case 0x2000:
			stack[sp] = pc;
			++sp;
			pc = NNN;
			break;

		// (0x3XNN) Skip next instruction if Vx == NN
		case 0x3000:
			if (V[X] == (NNN & 0x00FF)) pc += 2;
			break;

		// (0x4XNN) Skip instruction if Vx != NN
		case 0x4000:
			if (V[X] != (NNN & 0x00FF)) pc += 2;
			break;

		// (0x5XY0) Skip instruction if Vx == Vy
		case 0x5000:
			if (V[X] == V[Y]) pc += 2;
			break;

		// (0x6XNN) Sets VX register to NN
		case 0x6000:
			V[X] = NNN & 0x00FF;
			break;

		// (0x7XNN) Adds NN to register VX
		case 0x7000:
			V[X] += NNN & 0x00FF;
			break;

		// (0x8XY0) Handle bit and math operations on VX, VY registers
		case 0x8000:
			switch (opcode & 0x000F) 
			{
				case 0x0:
					V[X] = V[Y];
					break;

				case 0x1:
					V[X] |= V[Y];
					break;

				case 0x2:
					V[X] &= V[Y];
					break;

				case 0x3:
					V[X] ^= V[Y];
					break;
				
				case 0x4:
					// Check for overflow and set flag register accordingly
					if (V[X] > (0xFF - V[Y])) 
						V[0xF] = 1;
					else 
						V[0xF] = 0;

					V[X] += V[Y];
					break;

				case 0x5:
					// Check for underflow and set flag register accordingly
					if (V[X] <= V[Y]) 
						V[0xF] = 1;
					else 
						V[0xF] = 0;

					V[X] -= V[Y];
					break;

				case 0x6:
					// Store LSB before shift into VF
					if ((V[X] & 0x1) == 0x1) 
						V[0xF] = 1;
					else 
						V[0xF] = 0;

					V[X] >>= 1;
					break;

				case 0x7:
					// Set flags for  Overflow
					if (V[X] > V[Y]) 
						V[0xF] = 1;
					else 
						V[0xF] = 0;

					V[X] = V[Y] - V[X];
					break;

				case 0xE:
					// Store MSB before shift
					if ((V[X] & 0x80) == 0x80) 
						V[0xF] = 1;
					else 
						V[0xF] = 0;

					V[X] <<= 1;
					break;

				default:
					// throw std::runtime_error("Invalid Opcode at 0x8000");
					break;
			}
			break;

		// (0x9XY0) Skip instruction if Vx != Vy
		case 0x9000:
			if (V[X] != V[Y]) pc += 2;
			break;

		// (0xANNN) Set I address register to NNN
		case 0xA000:
			I = NNN;
			break;

		// (0xBNNN) Jump to address NNN plus V0
		case 0xB000:
			pc = NNN + V[0];
			break;

		// (0xCXNN) Set VX to the result of bitwise AND operation on random number and NN
		case 0xC000:
			V[X] = (rand() % 256) & (NNN & 0x00FF);
			break;

		// (0xDXYN) Draws a sprite at coordinates (Vx, Vy) w/ a width of 8 pixels
		// and a height of N pixels
		case 0xD000:
			{
				unsigned char spriteRow;

				int height = NNN & 0x000F;

				// Reset collision flag
				V[0xF] = 0;

				for (int h = 0; h < height; h++)
				{
					spriteRow = memory[I + h];

					for (int w = 0; w < 8; w++)
					{
						// Bit mask one bit at a time from MSB to LSB
						if ((spriteRow & (0x80 >> w)) != 0)
						{
							int index = (w + V[X]) + ((h + V[Y]) * DISPLAY_WIDTH);

							// Collision detected
							if (display[index] == 1)
								V[0xF] = 1;

							display[index] ^= 1;
						}
					}
				}
			}
			drawFlag = true;
			break;

		// (0xEXNN) Skips next instruction if key is/isn't pressed
		case 0xE000:
			if ((opcode & 0x00FF) == 0x9E) 
			{
				if (keys[V[X]]) pc += 2;
			} 
			else if ((opcode & 0x00FF) == 0xA1) 
			{
				if (!keys[V[X]]) pc += 2;
			}
			break;

		// (0xFXNN)
		case 0xF000:
			switch (opcode & 0x00FF) 
			{
				case 0x07:
					V[X] = dt;
					break;

				case 0x0A:
					pc -= 2;
					for (auto& key : keys) 
					{
						if (key) 
						{
							pc += 2;
							break;
						}
					}
					break;

				case 0x15:
					V[X] = dt;
					break;

				case 0x18:
					V[X] = st;
					break;

				case 0x1E:
					I += V[X];
					break;

				case 0x29:
					I = V[X] * 5;
					break;

				// Store BCD representation of value in Vx in memory locations I, I+1, I+2
				case 0x33:
					memory[I] = V[X] / 100;
					memory[I + 1] = (V[X] / 10) % 10;
					memory[I + 2] = (V[X] % 100) % 10;
					break;

				case 0x55:
					for (int i = 0; i <= X; i++)
						memory[I + i] = V[i];
					break;

				case 0x65:
					for (int i = 0; i <= X; i++)
						V[i] = memory[I + i];
					break;

				default: 
					std::cout << "Invalid Opcode at 0xF000: " << opcode << '\n';
					break;
			}
			break;
	
		default:
			std::cout << "Invalid Opcode: " << opcode << '\n';
			break;
	}

	// Decrement timers if > 0
	if (dt > 0) dt--;

	if (st > 0) st--;

	// std::cout << std::hex << opcode << '\n';
}