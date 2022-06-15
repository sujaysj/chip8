#include <SDL2/SDL.h>
#include <algorithm>
#include "chip8.h"

const int FONT_SIZE = 80;
// Font
const std::array<uint8_t, FONT_SIZE> font
{
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

Chip8::Chip8(): mem{}, screen{}, V{}, stack{}, I(0), DT(0), ST(0), PC(0), SP(0), screenDrawn(false)
{
    std::srand(1);
    for (auto i=0; i < FONT_SIZE; ++i)
    {
        mem[i] = font[i];
    }
}

void Chip8::clearScreen() 
{
    std:fill(screen, screen+(SCREEN_WIDTH*SCREEN_HEIGHT), 0);
    screenDrawn = true;
}

void Chip8::returnFromSubroutine() 
{
    PC = stack[SP];
    --SP;
}

void Chip8::jump(uint16_t addr) 
{
    PC = addr;
}

void Chip8::call(uint16_t addr) 
{
    ++SP;
    stack[SP] = PC;
    PC = addr;
}

void Chip8::skipEquals(uint8_t byte1, uint8_t byte2) 
{
    if (byte1 == byte2) {
        PC += 2;
    }
}

void Chip8::skipNotEquals(uint8_t byte1, uint8_t byte2) 
{
    if (byte1 != byte2) {
        PC += 2;
    }
}

void Chip8::loadRegister(uint8_t x, uint8_t byte) 
{
    V[x] = byte;
}

void Chip8::add(uint8_t x, uint8_t byte) 
{
    V[x] += byte;
}

void Chip8::orOp(uint8_t x, uint8_t y) 
{
    V[x] = (V[x] | V[y]);
}

void Chip8::andOp(uint8_t x, uint8_t y) 
{
    V[x] = (V[x] && V[y]);
}

void Chip8::xorOp(uint8_t x, uint8_t y) 
{
    V[x] = (V[x] ^ V[y]);
}

void Chip8::addCarry(uint8_t x, uint8_t y)
{
    uint8_t result = V[x] + V[y];
    if (result < (x|y)) {
        V[0xf] = 1;
    } 
    else {
        V[0xf] = 0;
    }
    V[x] = result;
}

void Chip8::subtract(uint8_t x, uint8_t y) 
{
    if ( V[x] > V[y] ) {
        V[0xf] = 1;
    }
    else {
        V[0xf] = 0;
    }

    V[x] = V[x] - V[y];
}

void Chip8::shiftRight(uint8_t x) 
{
    V[0xf] = (1 & V[x]);

    V[x] = V[x] >> 2;
}

void Chip8::subtractSwapped(uint8_t x, uint8_t y) 
{
    if ( V[y] > V[x] ) {
        V[0xf] = 1;
    }
    else {
        V[0xf] = 0;
    }

    V[x] = V[y] - V[x];
}

void Chip8::shiftLeft(uint8_t x) 
{
    V[0xf] = (1 & V[x]);

    V[x] = V[x] << 2;
}

void Chip8::loadAddr(uint16_t addr) 
{
    I = addr;
}

void Chip8::random(uint8_t x, uint16_t byte) 
{
    uint16_t randInt = std::rand();
    V[x] = (randInt & byte);
}

void Chip8::drawByte(uint8_t byte, uint8_t x, uint8_t y) 
{
    for (auto i=0; i<8; ++i) {
        if ((byte & 1) == 1) 
        {
            screen[(y*SCREEN_WIDTH) + (x % SCREEN_WIDTH)] = 0xFFFFFFFF;
        }
        else
        {
            screen[(y*SCREEN_WIDTH) + (x % SCREEN_WIDTH)] = 0xFF000000;
        }
        byte = byte >> 1;
    }
}

void Chip8::draw(uint8_t x, uint8_t y, uint8_t n) 
{
    for (auto i=0; i<n; ++i) {
        drawByte(mem[I+i], V[x], V[y] + (SCREEN_WIDTH * i));
    }
    screenDrawn = true;
}