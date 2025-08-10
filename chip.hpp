#include <string>

const int WIDTH = 64;
const int HEIGHT = 32;

const unsigned char FONT_SET[80] = {
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

class Chip8 {
public:
	/**
	 * @brief Main memory. First 512 bytes reserved for fonts. Programs begin at 0x200.
	 */
	unsigned char memory[4096];

	/**
	 * @brief 8-bit registers.
	 *
	 * Last byte reserved as a flag for certain instructions.
	 */
	unsigned char V[16];

	/**
	 * @brief Program Counter. Only uses 12 least significant bits.
	 */
	unsigned short int pc;

	/**
	 * @brief Address Register. Only uses 12 least significant bits.
	 */
	unsigned short int I;

	/**
	 * @brief Stack. Used to store return addresses.
	 */
	unsigned short int stack[16];

	/**
	 * @brief Stack Pointer.
	 */
	unsigned short int sp;

	/**
	 * @brief Stores key inputs. Key Bindings:
	 * @example
     * +-+-+-+-+
     * |1|2|3|4|
     * +-+-+-+-+
     * |Q|W|E|R|
     * +-+-+-+-+
     * |A|S|D|F|
     * +-+-+-+-+
     * |Z|X|C|V|
     * +-+-+-+-+
	 */
	bool keys[16];

	/**
	 * @brief 8-bit register for sound timer. Automatically decrements if non-zero.
	*/
	unsigned char st;

	/**
	 * @brief 8-bit register for delay timer. Automatically decrements if non-zero.
	 */
	unsigned char dt;

	/**
	 * @brief Pixels to be display on the screen
	 */
	unsigned char display[WIDTH * HEIGHT];

	/**
	 * @brief dictates whether to redraw or not
	 */
	bool drawFlag;

    Chip8();

    /**
     * @brief Sets register values to 0 and loads font set into main memory
     * @return void
     */
	void reset();

    /**
	 * @brief Fetches and executes instructions based on opcode. Emulates chip-8 cpu cycle.
	 */
	void emulateCycle();

    /**
     * @brief Searches rom folder if game exists and loads into main memory.
     * @returns true if success, false if failure
     */
	bool loadGame(std::string name);

    /**
     * @brief Loads font set into main memory
     * @return void
     */
	void loadFontSet();
};