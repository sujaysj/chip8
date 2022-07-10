#include <SDL2/SDL.h>
#include <cstdlib>
#include <cstdio>
#include <stdint.h>
#include "chip8.h"

using std::printf; using std::exit;

SDL_Window* window = NULL;

SDL_Renderer* renderer = NULL;

//Current display image
SDL_Texture* texture = NULL;

int main(int argc, char* argv[])
{
    #pragma region
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL initialization failed, %s\n",  SDL_GetError() );
        exit(1);
    }

    window = SDL_CreateWindow( "CHIP8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH * 16, SCREEN_HEIGHT * 16, 0 );
    if( window == nullptr )
    {
        printf( "window failed to initialize, %s\n", SDL_GetError() );
        exit(1);
    }

    renderer = SDL_CreateRenderer( window, -1, 0 );
    if( renderer == nullptr )
    {
        printf( "renderer failed to initialize, %s\n", SDL_GetError() );
        exit(1);
    }

    texture = SDL_CreateTexture( renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT );
    if( texture == nullptr )
    {
        printf( "texture failed to initialize, %s\n", SDL_GetError() );
        exit(1);
    }
    #pragma endregion

    // Initializing Chip8
    Chip8 chip = Chip8();

    chip.loadFile( "tetris.ch8" );
    chip.PC = 0x200;

    bool quit = false;

    // event handler
    SDL_Event e;

    // texture variables
    int pitch;
    uint32_t* pixels;

    // array for keyUp events to be used for Fx0A: wait for key press instruction
    bool keyUp[0x10];
    std::fill(keyUp, keyUp + 0x10, false);
    const uint8_t* keyboardState = SDL_GetKeyboardState(NULL);

    // creating timing variables
    uint32_t cycle_currentTime = SDL_GetTicks();
    uint32_t cycle_lastTime = cycle_currentTime;
    uint32_t cycle_deltaTime = 0;

    uint32_t delay_currentTime = SDL_GetTicks();
    uint32_t delay_lastTime = delay_currentTime;
    uint32_t delay_deltaTime = 0;

    constexpr int CYCLE_FREQ = 500; // placeholder value, will be customizable on launch
    constexpr int DELAY_RATE = 50; // likely to be fixed at 60 hz, determines delay and sound timer frequency. 
    constexpr int cycleTime = float(1000) / CYCLE_FREQ; // length of instruction execution in ms
    constexpr int delayTime = float(1000) / DELAY_RATE; // length of emulation cycle

    while( !quit )
    {
        //Handle inputs
        while( SDL_PollEvent( &e ) != 0 )
        {
            if( e.type == SDL_QUIT )
            {
                quit = true;
            }
            if( e.type == SDL_KEYUP )
            {
                switch (e.key.keysym.scancode)
                {
                    case (Keypad::KEY_0):
                        keyUp[0x0] = true;
                        break;
                    case (Keypad::KEY_1):
                        keyUp[0x1] = true;
                        break;
                    case (Keypad::KEY_2):
                        keyUp[0x2] = true;
                        break;
                    case (Keypad::KEY_3):
                        keyUp[0x3] = true;
                        break;
                    case (Keypad::KEY_4):
                        keyUp[0x4] = true;
                        break;
                    case (Keypad::KEY_5):
                        keyUp[0x5] = true;
                        break;
                    case (Keypad::KEY_6):
                        keyUp[0x6] = true;
                        break;
                    case (Keypad::KEY_7):
                        keyUp[0x7] = true;
                        break;
                    case (Keypad::KEY_8):
                        keyUp[0x8] = true;
                        break;
                    case (Keypad::KEY_9):
                        keyUp[0x9] = true;
                        break;
                    case (Keypad::KEY_A):
                        keyUp[0xA] = true;
                        break;
                    case (Keypad::KEY_B):
                        keyUp[0xB] = true;
                        break;
                    case (Keypad::KEY_C):
                        keyUp[0xC] = true;
                        break;
                    case (Keypad::KEY_D):
                        keyUp[0xD] = true;
                        break;
                    case (Keypad::KEY_E):
                        keyUp[0xE] = true;
                        break;
                    case (Keypad::KEY_F):
                        keyUp[0xF] = true;
                        break;
                }
            }
        }

        cycle_deltaTime = cycle_currentTime - cycle_lastTime;
        if (cycle_deltaTime >= cycleTime)
        {
            cycle_deltaTime -= cycleTime;
            chip.runCycle(keyboardState, keyUp);
            cycle_lastTime = cycle_currentTime;
        }

        delay_deltaTime = delay_currentTime - delay_lastTime;
        if (delay_deltaTime >= delayTime)
        {
            delay_deltaTime -= delayTime;
            if (chip.ST)
                chip.ST--;
            if (chip.DT)
                chip.DT--;
            delay_lastTime = delay_currentTime;
        }

        if ( chip.screenDrawn ) {
            SDL_LockTexture( texture, NULL, (void**)&pixels, &pitch );
            memcpy( pixels, chip.screen, SCREEN_HEIGHT * pitch);
            // memcpy( pixels, &chip.screen[0], 8192 );
            SDL_UnlockTexture( texture );
            chip.screenDrawn = false;
            SDL_RenderCopy( renderer, texture, NULL, NULL );
            SDL_RenderPresent( renderer );
        }

        delay_currentTime = cycle_currentTime = SDL_GetTicks();
    }

    SDL_DestroyWindow( window );
    SDL_DestroyRenderer( renderer );
    SDL_DestroyTexture( texture );
    window = nullptr;
    renderer = nullptr;
    texture = nullptr;
}