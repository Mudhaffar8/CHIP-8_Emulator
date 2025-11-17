#pragma once

#include <cstdint>

namespace C8Resolution
{
	constexpr int WIDTH = 64;
	constexpr int HEIGHT = 32;
}

namespace C8InitValues
{
	constexpr uint16_t MEM_SIZE = 0x1000;
	constexpr uint8_t NUM_REGISTERS = 0x10;
	constexpr uint8_t STACK_SIZE = 0x10;
	constexpr uint8_t NUM_KEYS = 0x10;

	constexpr uint16_t PC_START = 0x200;

	constexpr uint16_t PROGRAM_SIZE = MEM_SIZE - PC_START;
}

const uint8_t FONT_SET[] = {
	0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
	0x20, 0x60, 0x20, 0x20, 0xF0, // 1
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
	0xF0, 0x80, 0xF0, 0x80, 0x80, // F
};

class Chip8 
{
public:
    Chip8();

    /**
     * @brief Searches for ROM file and loads into main memory.
     * @returns true if success, false if failure
     */
	bool load_game(const char* name);

    /**
	 * @brief Fetches and executes instructions based on opcode.
	 */
	void execute_instruction();

	// Getters and Setters
	inline bool check_draw_flag() { return draw_flag; }
	inline uint8_t get_display_pixel(int index) { return display[index]; }
	
	// Keyboard
	inline void set_key(int index) { keys[index] = true; }
	inline void unset_key(int index) { keys[index] = false; }

private:
	/**
	 * @brief Stack. Used to store return addresses.
	 */
	uint16_t stack[C8InitValues::STACK_SIZE]{};

	/**
	 * @brief Pixels to be display on the screen
	 */
	uint8_t display[C8Resolution::WIDTH * C8Resolution::HEIGHT]{};

	/**
	 * @brief Main memory. First 512 bytes reserved for fonts. Programs begin at 0x200.
	 */
	uint8_t memory[C8InitValues::MEM_SIZE]{};

	/**
	 * @brief 8-bit registers.
	 *
	 * Last byte reserved as a flag for certain instructions.
	 */
	uint8_t V[C8InitValues::NUM_REGISTERS]{};

	/**
	 * @brief Stores key inputs.
	 */
	bool keys[C8InitValues::NUM_KEYS]{};

	/**
	 * @brief Program Counter. Only uses 12 least significant bits.
	 */
	uint16_t pc = C8InitValues::PC_START;

	/**
	 * @brief Address Register. Only uses 12 least significant bits.
	 */
	uint16_t I = 0;

	/**
	 * @brief Stack Pointer.
	 */
	uint16_t sp = 0;

	/**
	 * @brief 8-bit register for sound timer. Automatically decrements if non-zero.
	*/
	uint8_t st = 0;

	/**
	 * @brief 8-bit register for delay timer. Automatically decrements if non-zero.
	 */
	uint8_t dt = 0;

	/**
	 * @brief dictates whether to redraw or not
	 */
	bool draw_flag = false;
};