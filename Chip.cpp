#include "Chip.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <iomanip>
#include <windows.h> // Beep -> need to make this platform independent

Chip::Chip():
    memory(4096),
    V(16),
    stack(16),
    SP(0),
    I(0x0),
    PC(0x200),
    delay_timer(0),
    sound_timer(0),
    keypad(16),
    display(64 * 32), 
    drawFlag(false)
    {}

void Chip::run()
{
    unsigned short int opcode = (unsigned short int)(((memory[PC]) << 8) | (memory[PC + 1]));
    std::cout << std::hex << opcode << ": ";

    switch( opcode & 0xF000 )
    {
        case 0x0000:
            switch(opcode & 0x00FF)
            {
                case 0x00E0:
                    for(int i = 0; i < display.size(); i++)
                    {
                        display[i] = 0;
                    }
                    PC += 2;
                    drawFlag = true;
                    break;
                case 0x00EE:
                    SP--;
                    PC = (unsigned short int)(stack[SP] + 2);
                    std::cout << "Returning from subroutine to " << std::hex << PC << std::endl;
                    break;
                default:
                    std::cerr << "Unknown opcode: " << std::hex << opcode << std::endl;
                    exit(1);
                    break;
            }
            break;

        case 0x1000:
            PC = (unsigned short int)(opcode & 0x0FFF);
            std::cout << "Jumping to " << std::hex << PC << std::endl;
            break;
        
        case 0x2000:
            stack[SP] = PC;
            SP++;
            PC = (unsigned short int)(opcode & 0x0FFF);
            std::cout << "Calling subroutine at " << std::hex << PC << std::endl;
            break;
        
        case 0x3000:
            if(V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
            {
                PC += 4;
                std::cout << "Skipping next instruction" << std::endl;
            }
            else
            {
                PC += 2;
                std::cout << "Not skipping next instruction" << std::endl;
            }
            break;
        
        case 0x4000:
            if(V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
            {
                PC += 4;
                std::cout << "Skipping next instruction" << std::endl;
            }
            else
            {
                PC += 2;
                std::cout << "Not skipping next instruction" << std::endl;
            }
            break;
        
        case 0x5000:
            if(V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
            {
                PC += 4;
                std::cout << "Skipping next instruction" << std::endl;
            }
            else
            {
                PC += 2;
                std::cout << "Not skipping next instruction" << std::endl;
            }
            break;
        
        case 0x6000:
            V[(opcode & 0x0F00) >> 8] = (char)(opcode & 0x00FF);
            PC += 2;
            std::cout << "Setting V" << std::hex << ((opcode & 0x0F00) >> 8) << " to " << std::hex << (opcode & 0x00FF) << std::endl;
            break;
        
        case 0x7000:
            V[(opcode & 0x0F00) >> 8] += (char)(opcode & 0x00FF);
            PC += 2;
            std::cout << "Adding " << std::hex << (opcode & 0x00FF) << " to V" << std::hex << ((opcode & 0x0F00) >> 8) << std::endl;
            break;
        
        case 0x8000:
            switch( opcode & 0x000F )
            {
                case 0x0000:
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
                    PC += 2;
                    std::cout << "Setting V" << std::hex << ((opcode & 0x0F00) >> 8) << " to V" << std::hex << ((opcode & 0x00F0) >> 4) << std::endl;
                    break;
                
                case 0x0001:
                    V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
                    PC += 2;
                    std::cout << "Setting V" << std::hex << ((opcode & 0x0F00) >> 8) << " to V" << std::hex << ((opcode & 0x00F0) >> 4) << std::endl;
                    break;
                
                case 0x0002:
                    V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
                    PC += 2;
                    std::cout << "Setting V" << std::hex << ((opcode & 0x0F00) >> 8) << " to V" << std::hex << ((opcode & 0x00F0) >> 4) << std::endl;
                    break;
                
                case 0x0003:
                    V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
                    PC += 2;
                    std::cout << "Setting V" << std::hex << ((opcode & 0x0F00) >> 8) << " to V" << std::hex << ((opcode & 0x00F0) >> 4) << std::endl;
                    break;
                
                case 0x0004:
                    if(V[(opcode & 0x0F00) >> 8] + V[(opcode & 0x00F0) >> 4] > 0xFF)
                    {
                        V[0xF] = 1;
                    }
                    else
                    {
                        V[0xF] = 0;
                    }
                    V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
                    PC += 2;
                    std::cout << "Adding V" << std::hex << ((opcode & 0x0F00) >> 8) << " to V" << std::hex << ((opcode & 0x00F0) >> 4) << " with carry" << std::hex << V[0xF] << std::endl;
                    break;
                
                case 0x0005:
                    if(V[(opcode & 0x0F00) >> 8] > V[(opcode & 0x00F0) >> 4])
                    {
                        V[0xF] = 1;
                    }
                    else
                    {
                        V[0xF] = 0;
                    }
                    V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
                    PC += 2;
                    std::cout << "Subtracting V" << std::hex << ((opcode & 0x00F0) >> 4) << " from V" << std::hex << ((opcode & 0x0F00) >> 8) << " with borrow" << std::hex << V[0xF] << std::endl;
                    break;
                
                case 0x0006:
                    V[0xF] = V[(opcode & 0x0F00) >> 8] & 0x1;
                    V[(opcode & 0x0F00) >> 8] >>= 1;
                    PC += 2;
                    std::cout << "Shifting V" << std::hex << ((opcode & 0x0F00) >> 8) << " right by 1" << std::endl;
                    break;
                
                case 0x0007:
                    if(V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8])
                    {
                        V[0xF] = 1;
                    }
                    else
                    {
                        V[0xF] = 0;
                    }
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4] - V[(opcode & 0x0F00) >> 8];
                    PC += 2;
                    std::cout << "Subtracting V" << std::hex << ((opcode & 0x0F00) >> 8) << " from V" << std::hex << ((opcode & 0x00F0) >> 4) << " with borrow" << std::hex << V[0xF] << std::endl;
                    break;
                
                case 0x000E:
                    V[0xF] = V[(opcode & 0x0F00) >> 8] >> 7;
                    V[(opcode & 0x0F00) >> 8] <<= 1;
                    PC += 2;
                    std::cout << "Shifting V" << std::hex << ((opcode & 0x0F00) >> 8) << " left by 1" << std::endl;
                    break;
                
                default:
                    std::cerr << "Unknown opcode: " << std::hex << opcode << std::endl;
                    exit(1);
                    break;
            }
            break;

        case 0x9000:
            if(V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
            {
                PC += 4;
                std::cout << "Skipping next instruction" << std::endl;
            }
            else
            {
                PC += 2;
                std::cout << "Not skipping next instruction" << std::endl;
            }
            break;
        
        case 0xA000:
            I = (unsigned short int)(opcode & 0x0FFF);
            PC += 2;
            std::cout << "Setting I to " << std::hex << I << std::endl;
            break;
        
        case 0xB000:
            PC = (unsigned short int)((opcode & 0x0FFF) + V[0]);
            std::cout << "Jumping to " << std::hex << PC << std::endl;
            break;
        
        case 0xC000:
            V[(opcode & 0x0F00) >> 8] = (char)((rand()%256) & (opcode & 0x00FF));
            PC += 2;
            std::cout << "Setting V" << std::hex << ((opcode & 0x0F00) >> 8) << " to " << std::hex << (opcode & 0x00FF) << std::endl;
            break;
        
        case 0xD000:
        {
            unsigned short int x = V[(opcode & 0x0F00) >> 8];
            unsigned short int y = V[(opcode & 0x00F0) >> 4];
            unsigned short int height = opcode & 0x000F;
            unsigned short int pixel;

            V[0xF] = 0;
            for(int yline = 0; yline < height; yline++)
            {
                pixel = memory[I + yline];
                for(int xline = 0; xline < 8; xline++)
                {
                    if((pixel & (0x80 >> xline)) != 0)
                    {
                        if(display[(x + xline + ((y + yline) * 64))] == 1)
                        {
                            V[0xF] = 1;
                        }
                        display[x + xline + ((y + yline) * 64)] ^= 1;
                    }
                }
            }

            drawFlag = true;
            PC += 2;
            std::cout << "Drawing sprite at V" << std::hex << ((opcode & 0x0F00) >> 8) << ", V" << std::hex << ((opcode & 0x00F0) >> 4) << " with height " << std::hex << (opcode & 0x000F) << std::endl;
            break;
        }
        
        case 0xE000:
            switch( opcode & 0x00FF )
            {
                case 0x009E:
                    if(keypad[V[(opcode & 0x0F00) >> 8]] != 0)
                    {
                        PC += 4;
                        std::cout << "Skipping next instruction" << std::endl;
                    }
                    else
                    {
                        PC += 2;
                        std::cout << "Not skipping next instruction" << std::endl;
                    }
                    break;
                
                case 0x00A1:
                    if(keypad[V[(opcode & 0x0F00) >> 8]] == 0)
                    {
                        PC += 4;
                        std::cout << "Skipping next instruction" << std::endl;
                    }
                    else
                    {
                        PC += 2;
                        std::cout << "Not skipping next instruction" << std::endl;
                    }
                    break;
                
                default:
                    std::cerr << "Unknown opcode: " << std::hex << opcode << std::endl;
                    exit(1);
                    break;
            }
            break;
        
        case 0xF000:
            switch( opcode & 0x00FF )
            {
                
                case 0x0007:
                    V[(opcode & 0x0F00) >> 8] = (char)delay_timer;
                    PC += 2;
                    std::cout << "Setting V" << std::hex << ((opcode & 0x0F00) >> 8) << " to " << std::hex << delay_timer << std::endl;
                    break;
                
                case 0x000A:
                    for(int i = 0; i < keypad.size(); i++)
                    {
                        if(keypad[i]== 1)
                        {
                            V[(opcode & 0x0F00) >> 8] = (char)i;
                            PC += 2;
                            std::cout << "Setting V" << std::hex << ((opcode & 0x0F00) >> 8) << " to " << std::hex << i << std::endl;
                            break;
                        }
                    }
                    break;
                
                case 0x0015:
                    delay_timer = V[(opcode & 0x0F00) >> 8];
                    PC += 2;
                    std::cout << "Setting delay timer to V" << std::hex << ((opcode & 0x0F00) >> 8) << std::endl;
                    break;
                
                case 0x0018:
                    sound_timer = V[(opcode & 0x0F00) >> 8];
                    PC += 2;
                    std::cout << "Setting sound timer to V" << std::hex << ((opcode & 0x0F00) >> 8) << std::endl;
                    break;
                
                case 0x001E:
                    I += V[(opcode & 0x0F00) >> 8];
                    PC += 2;
                    std::cout << "Adding V" << std::hex << ((opcode & 0x0F00) >> 8) << " to I" << std::hex << I << std::endl;
                    break;
                
                case 0x0029:
                    I = (unsigned short int)(V[(opcode & 0x0F00) >> 8] * 0x5);
                    PC += 2;
                    std::cout << "Setting I to the location of the sprite for digit V" << std::hex << ((opcode & 0x0F00) >> 8) << std::endl;
                    break;
                
                case 0x0033:
                    memory[I] = V[(opcode & 0x0F00) >> 8] / 100;
                    memory[I + 1] = (V[(opcode & 0x0F00) >> 8] / 10) % 10;
                    memory[I + 2] = V[(opcode & 0x0F00) >> 8] % 10;
                    PC += 2;
                    std::cout << "Storing BCD representation of V" << std::hex << ((opcode & 0x0F00) >> 8) << " at I" << std::hex << I << std::endl;
                    break;
                
                case 0x0055:
                    for(int i = 0; i <= ((opcode & 0x0F00) >> 8); i++)
                    {
                        memory[I + i] = V[i];
                    }
                    I += ((opcode & 0x0F00) >> 8) + 1;
                    PC += 2;
                    std::cout << "Storing V0 to V" << std::hex << ((opcode & 0x0F00) >> 8) << " in memory starting at I" << std::hex << I << std::endl;
                    break;
                
                case 0x0065:
                    for(int i = 0; i <= ((opcode & 0x0F00) >> 8); i++)
                    {
                        V[i] = memory[I + i];
                    }
                    I += ((opcode & 0x0F00) >> 8) + 1;
                    PC += 2;
                    std::cout << "Loading V0 to V" << std::hex << ((opcode & 0x0F00) >> 8) << " from memory starting at I" << std::hex << I << std::endl;
                    break;
                
                default:
                    std::cerr << "Unknown opcode: " << std::hex << opcode << std::endl;
                    exit(1);
                    break;
            }
            break;

        default:
            std::cerr << "Unknown opcode: " << std::hex << opcode << std::endl;
            exit(1);
            break;

    }

    if(sound_timer > 0)
    {
        sound_timer--;
        Beep(523,500); // need to make this platform independent
        
    }

    if(delay_timer > 0)
    {
        delay_timer--;
    }

    
}

void Chip::loadFile(const std::string& filepath)
{
    std::ifstream input(filepath, std::ios::binary);
    if(!input)
    {
        std::cerr << "Error opening file: " << filepath << std::endl;
        exit(1);
    }

    int offset = 0;
    char byte;
    while(input.get(byte))
    {
        memory[0x200 + offset] = (char)byte & 0xFF;
        offset++;
    }
    input.close();

    std::cout << "Printing for testing 80 bytes from 0x200:" << std::endl;
    for(int i = 0; i < 80; i++)
    {
        std::cout << std::hex << (memory[0x200 + i] & 0xFF) << " ";
    }
    std::cout << std::endl;

}

void Chip::loadFontset()
{
    std::vector<int> fontset = {
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

    for(int i = 0; i < fontset.size(); i++)
    {
        memory[0x50 + i] = (char)fontset[i] & 0xFF;
    }
}

void Chip::setKeyBuffer(const std::vector<int>& keyBuffer)
{
    for(int i = 0; i < keypad.size(); i++)
    {
        keypad[i] = keyBuffer[i];
    }
}

std::vector<unsigned short int> Chip::getDisplay()
{
    return display;
}

bool Chip::needsRedraw()
{
    return drawFlag;
}

void Chip::removeDrawFlag()
{
    drawFlag = false;
}
