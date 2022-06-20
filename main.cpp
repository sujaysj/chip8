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

    // flag set to true when a draw instruction is executed
    bool screenDrawn = true;

    while( !quit )
    {
        //Handle inputs
        while( SDL_PollEvent( &e ) != 0 )
        {
            if( e.type == SDL_QUIT )
            {
                quit = true;
            }
        }
        
        if ( screenDrawn ) {
            SDL_LockTexture( texture, NULL, (void**)&pixels, &pitch );
            memcpy( pixels, chip.screen, sizeof(uint32_t) * SCREEN_WIDTH * SCREEN_HEIGHT );
            SDL_UnlockTexture(texture);
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