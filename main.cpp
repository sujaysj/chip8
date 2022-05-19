#include <SDL2/SDL.h>
#include <stdio.h>

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

SDL_Window* window = NULL;

SDL_Renderer* renderer = NULL;

//Current display image
SDL_Texture* texture = NULL;

uint8_t memory[4096];

int main( int argc, char* argv[] )
{
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL initialization failed, %s\n",  SDL_GetError() );
        exit(1);
    }

    window = SDL_CreateWindow( "CHIP8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH * 16, SCREEN_HEIGHT * 16, 0 );
    if( window == NULL )
    {
        printf( "window failed to initialize, %s\n", SDL_GetError() );
        exit(1);
    }

    renderer = SDL_CreateRenderer( window, -1, 0 );
    if( renderer == NULL )
    {
        printf( "renderer failed to initialize, %s\n", SDL_GetError() );
        exit(1);
    }

    texture = SDL_CreateTexture( renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, SCREEN_HEIGHT );
    if( texture == NULL )
    {
        printf( "texture failed to initialize, %s\n", SDL_GetError() );
        exit(1);
    }

    // Initializing screen
    uint32_t screen[SCREEN_WIDTH * SCREEN_HEIGHT];
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; ++i)
    {
            screen[i] = 0xFF00FF00;
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
            
            // According to SDL Documentation, LockTexture is faster for frequent writes than UpdateTexture. Consensus online varies on this point.
            // TODO: profiling to determine speed of lock vs update
            SDL_LockTexture( texture, NULL, (void**)&pixels, &pitch );
            memcpy( pixels, screen, sizeof(uint32_t) * SCREEN_WIDTH * SCREEN_HEIGHT );
            SDL_UnlockTexture(texture);

        }

        SDL_RenderCopy( renderer, texture, NULL, NULL );
        SDL_RenderPresent( renderer );
    }
}