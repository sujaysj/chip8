#include <SDL2/SDL.h>
#include <algorithm>
#include <limits>
#include <fstream>
#include "chip8.h"

// Font
constexpr int FONT_SIZE = 80;
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

const std::array<uint8_t, 0x10> keyBindings
{
    
    SDL_SCANCODE_X, // 0
    SDL_SCANCODE_1, // 1
    SDL_SCANCODE_2, // 2
    SDL_SCANCODE_3, // 3
    SDL_SCANCODE_Q, // 4
    SDL_SCANCODE_W, // 5
    SDL_SCANCODE_E, // 6
    SDL_SCANCODE_A, // 7
    SDL_SCANCODE_S, // 8
    SDL_SCANCODE_D, // 9
    SDL_SCANCODE_Z, // A
    SDL_SCANCODE_C, // B
    SDL_SCANCODE_4, // C
    SDL_SCANCODE_R, // D
    SDL_SCANCODE_F, // E
    SDL_SCANCODE_V  // F
};

Chip8::Chip8(): mem{}, screen{}, V{}, stack{}, I(0), DT(0), ST(0), PC(0), SP(0), screenDrawn(false)
{
    std::srand(1);
    for (auto i=0; i < FONT_SIZE; ++i)
    {
        mem[i] = font[i];
    }
}

bool Chip8::loadFile(std::string filename)
{
    std::ifstream chipFile(filename);
    if (!chipFile)
    {
        printf("File not found");
        return false;
    }

    chipFile.ignore(std::numeric_limits<std::streamsize>::max());
    std::streamsize length = chipFile.gcount();
    chipFile.clear();
    chipFile.seekg(0, std::ios_base::beg);

    if (length > (MEM_SIZE - PROGRAM_ADDRESS))
    {
        printf("File too large");
        chipFile.close();
        return false;
    }

    chipFile.read(reinterpret_cast<char*>(mem[PROGRAM_ADDRESS]), length);
    chipFile.close();
    return true;

}

void Chip8::runCycle(uint8_t* keyBoardState, bool* keyUp)
{
    uint8_t leftByte = mem[PC];
    uint8_t rightByte = mem[PC + 1];
    uint16_t opcode = ((uint16_t)leftByte << 8) + (rightByte);

    // variables for readability when these nibbles are used by an instruction to specify a register
    uint8_t x = leftByte & 0x0F;
    uint8_t y = rightByte >> 4;

    switch (opcode & 0xF000)
    {
        case (0x0000):
            switch (opcode & 0x0FFF)
            {
                case (0x00E0):
                    clearScreen();
                    break;
                case (0x00EE):
                    returnFromSubroutine();
                    break;
                default:
                    invalidOpcode(opcode);
            }
            break;
        case (0x1000):
            jump(opcode & 0x0FFF);
            break;
        case (0x2000):
            call(opcode & 0x0FFF);
            break;
        case (0x3000):
            skipEquals(V[x], rightByte);
            break;
        case (0x4000):
            skipNotEquals(V[x], rightByte);
            break;
        case (0x5000):
            if (opcode & 0x000F == 0x0000)
            {
                skipEquals(V[x], V[y]);
            }
            else
            {
                invalidOpcode(opcode);
            }
            break;
        case (0x6000):
            loadRegister(x, rightByte);
            break;
        case (0x7000):
            add(x, rightByte);
            break;
        case (0x8000):
            switch (opcode & 0x000F)
            {
                case (0x0):
                    loadRegister(x, V[y]);
                    break;
                case (0x1):
                    orOp(x, y);
                    break;
                case (0x2):
                    andOp(x, y);
                    break;
                case (0x3):
                    xorOp(x, y);
                    break;
                case (0x4):
                    addCarry(x, y);
                    break;
                case (0x5):
                    subtract(x, y);
                    break;
                case (0x6):
                    shiftRight(x);
                    break;
                case (0x7):
                    subtractSwapped(x, y);
                    break;
                case (0xE):
                    shiftLeft(x);
                    break;
                default:
                    invalidOpcode(opcode);
            }
            break;
        case (0x9000):
            if (opcode & 0x000F == 0x0000)
            {
                skipEquals(V[x], V[y]);
            }
            else
            {
                invalidOpcode(opcode);
            }
            break;
        case (0xA000):
            loadAddr(opcode & 0x0FFF);
            break;
        case (0xB000):
            jump(V[0] + (opcode & 0x0FFF));
            break;
        case (0xC000):
            random(x, rightByte);
            break;
        case (0xD000):
            draw(x, y, opcode & 0x000F);
            break;
        case (0xE000):
            switch (rightByte)
            {
                case (0x9E):
                    skipKeyPressed(x, keyBoardState);
                    break;
                case (0xA1):
                    skipNotPressed(x, keyBoardState);
                    break;
                default:
                    invalidOpcode(opcode);
            }
            break;
        case (0xF000):
            switch (rightByte)
            {
                case (0x07):
                    loadFromDelayTimer(x);
                    break;
                case (0x0A):
                    waitKeyPress(x, keyUp);
                    break;
                case (0x15):
                    setDelayTimer(x);
                    break;
                case (0x18):
                    setSoundTimer(x);
                    break;
                case (0x1E):
                    addAddressRegister(x);
                    break;
                case (0x29):
                    loadFont(x);
                    break;
                case(0x33):
                    loadBCD(x);
                    break;
                case(0x55):
                    storeRegisters(x);
                    break;
                case (0x65):
                    readRegisters(x);
                    break;
                default:
                    invalidOpcode(opcode);
            }
            break;
        default:
            invalidOpcode(opcode);
    }

    PC += 2; // increment program counter
}

void Chip8::invalidOpcode(uint16_t opcode)
{
    printf("Invalid opcode %04X at address %04X, program terminated", &opcode, &PC);
}

void Chip8::clearScreen() 
{
    std::fill(screen, screen+(SCREEN_WIDTH*SCREEN_HEIGHT), 0);
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
    V[0xf] = ((0b1000'0000 & V[x]) >> 7);

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
        // check left-most bit
        if ((byte & 0x80) == 1) 
        {
            screen[(y*SCREEN_WIDTH) + ((x+i) % SCREEN_WIDTH)] = WHITE_PIXEL;
        }
        else
        {
            // set carry flag if a collision occurs
            if (screen[(y*SCREEN_WIDTH) + ((x+i) % SCREEN_WIDTH)] == WHITE_PIXEL) 
            {
                V[0xf] = 1;
            }
            screen[(y*SCREEN_WIDTH) + ((x+i) % SCREEN_WIDTH)] = BLACK_PIXEL;
        }
        byte = byte << 1;
    }
}

void Chip8::draw(uint8_t x, uint8_t y, uint8_t n) 
{
    // carry flag set by default to 0 (no collision)
    V[0xf] = 0;

    for (auto i=0; i<n; ++i) {
        drawByte(mem[I+i], V[x], V[y] + (SCREEN_WIDTH * i));
    }

    screenDrawn = true;
}

void Chip8::skipKeyPressed(uint8_t x, uint8_t* keyboardState)
{
    if (keyboardState[keyBindings[x]])
    {
        PC += 2;
    }
}

void Chip8::skipNotPressed(uint8_t x, uint8_t* keyboardState)
{
    if (!keyboardState[keyBindings[x]])
    {
        PC += 2;
    }
}

void Chip8::loadFromDelayTimer(uint8_t x);
{
    V[x] = DT;
}

// behavior of Fx0A implemented from (my understanding of) this stackexchange answer:
// https://retrocomputing.stackexchange.com/a/361
//
// because of inconsistencies with simultaneous key input, I chose to just wait for the first key release,
// meaning if a key is held down before Fx0A is reached, then another key is pressed and released while the original is held down,
// the second key is registered. This may not be consistent with the original interpreter.
void Chip8::waitKeyPress(uint8_t x, bool* keyUp)
{
    for (int i = 0; i < 0x10; ++i)
    {
        if (keyUp[keyBindings[i]])
        {
            V[x] = i;
            PC += 2;
            break;
        }
    }
    PC -=2; // decrement program counter so after runCycle() the net change is 0 if not pressed, +2 if pressed.
}

void Chip8::setDelayTimer(uint8_t x)
{
    DT = V[x];
}

void Chip8::setSoundTimer(uint8_t x)
{
    ST = V[x];
}

void Chip8::addAddressRegister(uint8_t x)
{
    I = I + V[x];
}

void Chip8::loadFont(uint8_t x)
{
    I = mem[V[x] * 5];
}

void Chip8::loadBCD(uint8_t x)
{
    mem[I] = (V[x] / 100) % 10;
    mem[I+1] = (V[x] / 10) % 10;
    mem[I+2] = V[x] % 10;
}

void Chip8::storeRegisters(uint8_t x)
{
    for (int i = 0; i < 0x10, ++i)
    {
        mem[I+i] = V[i];
    }
}

void Chip8::readRegisters(uint8_t x)
{
    for (int i = 0; i < 0x10; ++i)
    {
        V[i] = mem[I+i];
    }
}