#include <SDL2/SDL.h>
#include <array>
#include <vector>
#include <iostream>

#define SCREEN_WIDTH 1280 
#define SCREEN_HEIGHT 720

class ChipFrame
{
    SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
    std::array<int, 16> keyBuffer;
    std::array<int, 256> keyIdToKey;
    void fillKeyIds();

    public:
        ChipFrame();
        void keyPressed(char keyId);
        void keyReleased(char keyId);
        void keyTyped(char keyId);
        std::vector<int> getKeyBuffer();
        void draw(const std::vector<unsigned short int>& display);
};