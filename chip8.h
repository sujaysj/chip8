#ifndef CHIP8
#define CHIP8

#include <SDL2/SDL.h>
#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <array>
#include <string>

constexpr int SCREEN_WIDTH = 64;
constexpr int SCREEN_HEIGHT = 32;
constexpr int MEM_SIZE = 4096;
constexpr int PROGRAM_ADDRESS = 0x200;
constexpr uint32_t WHITE_PIXEL = 0xFFFFFFFF;
constexpr uint32_t BLACK_PIXEL = 0xFF000000;

namespace Keypad 
{
    inline constexpr int KEY_1 = SDL_SCANCODE_1;
    inline constexpr int KEY_2 = SDL_SCANCODE_2;
    inline constexpr int KEY_3 = SDL_SCANCODE_3;
    inline constexpr int KEY_C = SDL_SCANCODE_4;
    inline constexpr int KEY_4 = SDL_SCANCODE_Q;
    inline constexpr int KEY_5 = SDL_SCANCODE_W;
    inline constexpr int KEY_6 = SDL_SCANCODE_E;
    inline constexpr int KEY_D = SDL_SCANCODE_R;
    inline constexpr int KEY_7 = SDL_SCANCODE_A;
    inline constexpr int KEY_8 = SDL_SCANCODE_S;
    inline constexpr int KEY_9 = SDL_SCANCODE_D;
    inline constexpr int KEY_E = SDL_SCANCODE_F;
    inline constexpr int KEY_A = SDL_SCANCODE_Z;
    inline constexpr int KEY_0 = SDL_SCANCODE_X;
    inline constexpr int KEY_B = SDL_SCANCODE_C;
    inline constexpr int KEY_F = SDL_SCANCODE_V;
}

struct Chip8 {
    Chip8();

    uint8_t mem[MEM_SIZE]; // Chip-8 memory
    uint32_t screen[SCREEN_WIDTH * SCREEN_HEIGHT]; // screen buffer
    uint8_t V[16]; // general purpose registers
    uint16_t stack[16]; // stack stores return addresses for subroutines
    uint16_t I; // 16 bit register, used for storing memory addresses
    uint8_t DT; // delay timer
    uint8_t ST; // sound timer
    uint16_t PC; // program counter
    uint8_t SP; // stack pointer

    bool screenDrawn;

    bool loadFile(std::string filename);

    void runCycle(uint8_t* keyboardState, bool* keyUp);

    void clearScreen();
    void returnFromSubroutine();
    void jump(uint16_t addr);
    void call(uint16_t addr);
    void skipEquals(uint8_t byte1, uint8_t byte2);
    void skipNotEquals(uint8_t byte1, uint8_t byte2);
    void loadRegister(uint8_t x, uint8_t byte);
    void add(uint8_t x, uint8_t byte);
    void orOp(uint8_t x, uint8_t y);
    void andOp(uint8_t x, uint8_t y);
    void xorOp(uint8_t x, uint8_t y);
    void addCarry(uint8_t x, uint8_t y);
    void subtract(uint8_t x, uint8_t y);
    void shiftRight(uint8_t x);
    void subtractSwapped(uint8_t x, uint8_t y);
    void shiftLeft(uint8_t x);
    void loadAddr(uint16_t addr);
    void random(uint8_t x, uint16_t byte);
    void drawByte(uint8_t byte, uint8_t x, uint8_t y);
    void draw(uint8_t x, uint8_t y, uint8_t n);
    void skipKeyPressed(uint8_t x, uint8_t* keyboardState);
    void skipNotPressed(uint8_t x, uint8_t* keyboardState);
    void loadFromDelayTimer(uint8_t x);
    void waitKeyPress(uint8_t x, uint8_t key, uint8_t* keyUp);
    void setDelayTimer(uint8_t x);
    void setSoundTimer(uint8_t x);
    void addAddressRegister(uint8_t x);
    void loadFont(uint8_t x);
    void loadBCD(uint8_t x);
    void storeRegisters(uint8_t x);
    void readRegisters(uint8_t x);
};

#endif