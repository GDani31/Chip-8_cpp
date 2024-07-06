#include "Chip.h"
#include "ChipFrame.h"
#include <SDL2/SDL.h>
#include <iostream>

int main(int argc, char** argv)
{
    if(argc < 2)
    {
        std::cerr << "Usage: chip8 <ROM file>" << std::endl;
        return 1;
    }

    Chip chip8;
    ChipFrame chipFrame;
    chip8.loadFile(argv[1]);

    const int FPS = 60;
    const int frameDelay = 1000 / FPS;

    Uint32 frameStart;
    int frameTime;

    while(1)
    {
        frameStart = SDL_GetTicks();

        chip8.setKeyBuffer(chipFrame.getKeyBuffer());
        chip8.run();
        if(chip8.needsRedraw())
        {
            chipFrame.draw(chip8.getDisplay());
            chip8.removeDrawFlag();
        }

        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_QUIT:
                    return 0;
                case SDL_KEYDOWN:
                    chipFrame.keyPressed( event.key.keysym.sym);
                    break;
                case SDL_KEYUP:
                    chipFrame.keyReleased( event.key.keysym.sym);
                    break;
            }
        }

        frameTime = SDL_GetTicks() - frameStart;

        if(frameTime < frameDelay)
        {
            SDL_Delay(frameDelay - frameTime);
        }

    }

    return 0;
}