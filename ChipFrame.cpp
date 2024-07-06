#include "ChipFrame.h"

ChipFrame::ChipFrame()
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0, &window, &renderer);
	SDL_RenderSetScale(renderer, 1, 1);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);
	fillKeyIds();
	keyBuffer.fill(0);
}

void ChipFrame::fillKeyIds()
{
	keyIdToKey.fill(-1);
	keyIdToKey['1'] = 1;
	keyIdToKey['2'] = 2;
	keyIdToKey['3'] = 3;
	keyIdToKey['q'] = 4;
	keyIdToKey['w'] = 5;
	keyIdToKey['e'] = 6;
	keyIdToKey['a'] = 7;
	keyIdToKey['s'] = 8;
	keyIdToKey['d'] = 9;
	keyIdToKey['z'] = 0xA;
	keyIdToKey['x'] = 0;
	keyIdToKey['c'] = 0xB;
	keyIdToKey['4'] = 0xC;
	keyIdToKey['r'] = 0xD;
	keyIdToKey['f'] = 0xE;
	keyIdToKey['v'] = 0xF;
}

std::vector<int> ChipFrame::getKeyBuffer()
{
	return std::vector<int>(keyBuffer.begin(), keyBuffer.end());
}

void ChipFrame::draw(const std::vector<unsigned short int>& display)
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	for(int i = 0; i < 64; i++)
	{
		for(int j = 0; j < 32; j++)
		{
			if(display[i + j * 64] == 1)
			{
				SDL_Rect rect = {i * 20, j * 20, 20, 20};
				SDL_RenderFillRect(renderer, &rect);
			}
		}
	}
	SDL_RenderPresent(renderer);
}

void ChipFrame::keyPressed(char keyId)
{
	if(keyIdToKey[keyId] != -1)
	{
		keyBuffer[keyIdToKey[keyId]] = 1;
	}
}

void ChipFrame::keyReleased(char keyId)
{
	if(keyIdToKey[keyId] != -1)
	{
		keyBuffer[keyIdToKey[keyId]] = 0;
	}
}


