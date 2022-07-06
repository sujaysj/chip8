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
    Chip8 chip;

    // Initializing screen
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; ++i)
    {
            chip.screen[i] = 0xFF00FF00;
    }

    bool quit = false;

    // event handler
    SDL_Event e;

    // texture variables
    int pitch;
    uint32_t* pixels;

    // array for keyUp events to be used for Fx0A: wait for key press instruction
    bool keyUp[0x10];
    std::fill(keyUp, keyUp + 0x10, false);

    uint8_t* keyboardState = SDL_GetKeyboardState(NULL);

    while( !quit )
    {
        //Handle inputs
        while( SDL_PollEvent( &e ) != 0 )
        {
            if( e.type == SDL_QUIT )
            {
                quit = true;
            }
            if( e.type == SDLK_KEYUP )
            {
                switch (e.key.keysym.sym)
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

        chip.runCycle(keyboardState, keyUp);

        if ( chip.screenDrawn ) {
            SDL_LockTexture( texture, NULL, (void**)&pixels, &pitch );
            memcpy( pixels, chip.screen, sizeof(uint32_t) * SCREEN_WIDTH * SCREEN_HEIGHT );
            SDL_UnlockTexture( texture );
        }

        SDL_RenderCopy( renderer, texture, NULL, NULL );
        SDL_RenderPresent( renderer );
    }

    SDL_DestroyWindow( window );
    SDL_DestroyRenderer( renderer );
    SDL_DestroyTexture( texture );
    window = nullptr;
    renderer = nullptr;
    texture = nullptr;
}