#include "chip-8.h"
#include <iostream>
#include <fstream>
#include <unistd.h>
#define FIRSTNIBBLEDIVIDER 0xF000
using namespace std;
unsigned char chip8_fontset[80] =
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
Chip8::Chip8(){

}
Chip8::~Chip8(){

}
void Chip8::initialize(){
    // Initialize registers and memory once
    pc = 0x200;
    opcode = 0;
    I = 0;
    stack_pointer = 0;
    drawFlag = true;
    sound_timer = 0;
    delay_timer = 0;

    //load fontset
    for(int i = 0; i<4096; i++){
        memory[i] = 0;
    }
    for(int i = 0 ; i < 80 ; i++){
        memory[i] = chip8_fontset[i];
    }
    for(int i = 0; i<16; i++){
        V[i] = 0;
    }
}
bool Chip8::loadrom(string rom) {
    char* buffer;
    streampos size;

    string source_name = rom;//Tofix later
    ifstream sourcefile(source_name,ios::binary|ios::ate);
    if(sourcefile.is_open()){

        size = sourcefile.tellg();
        buffer = new char[size];
        sourcefile.seekg(0,ios::beg);
        sourcefile.read(buffer,size);
        sourcefile.close();
        for(int i = 0;i<size;i++){
            memory[i+0x200] = buffer[i];
        }
        return true;
    }

    return false;
}
void Chip8::emulateCycle(){
    // Fetch Opcode
    opcode = memory[pc] << 8 | memory[pc+1];
    printf("0x%X-------",opcode);
    sleep(1);
    // Decode Opcode
    switch(opcode & FIRSTNIBBLEDIVIDER){
        case 0x0000:
            switch(opcode & 0x000F){
                case 0x0000:
                    //clear
                    for (int i = 0; i < 64*32; i++) {
                        gfx[i] = 0;
                    }
                    drawFlag = true;
                    pc += 2;
                    break;
                case 0x000E:
                    pc = stack[stack_pointer--];
                    break;
                default:
                    printf ("Unknown opcode [0x0000]: 0x%X\n", opcode);
            }
            break;
        case 0x1000:
            pc = opcode & 0x0FFF;

            break;
        case 0x2000:
            stack[++stack_pointer] = pc;
            pc = opcode & 0x0FFF;
            break;
        case 0x3000:
            if(V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
                pc += 2;
            pc += 2;
            break;
        case 0x4000:
            if(V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
                pc += 2;
            pc += 2;
            break;
        case 0x5000:
            if(V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
                pc += 2;
            pc += 2;
            break;
        case 0x6000:
            V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
            pc += 2;
            break;
        case 0x7000:
            V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
            pc += 2;
            break;
            break;
        case 0x8000:
            switch(opcode & 0x000F){
                case 0x0000:
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                case 0x0001:
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] | V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                case 0x0002:
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] & V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                case 0x0003:
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x0F00) >> 8] ^ V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                case 0x0004:
                    if(V[(opcode & 0x00F0) >> 4] + (0xFF - V[(opcode & 0x0F00) >> 8]))
                        V[0xF] = 1; //carry
                    else
                        V[0xF] = 0;
                    V[(opcode & 0x0F00) >> 8] += V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                case 0x0005:
                    if(V[(opcode & 0x0F00) >> 8]<V[(opcode & 0x00F0) >> 4])
                        V[0xF] = 1; //borrow
                    else
                        V[0xF] = 0;
                    V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
                    pc += 2;
                    break;
                case 0x0006:
                    V[0xF] = V[(opcode & 0x0F00) >> 8] >>15;
                    V[(opcode & 0x0F00) >> 8]  >>=1;//to verify

                    pc += 2;
                    break;
                case 0x0007:
                    if(V[(opcode & 0x00F0) >> 4] > V[(opcode & 0x0F00) >> 8])
                        V[0xF] = 1; //borrow
                    else
                        V[0xF] = 0;
                    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4]-V[(opcode & 0x0F00) >> 8];
                    pc += 2;
                    break;
                case 0x000E:
                    V[0xF] = V[(opcode & 0x0F00) >> 8] <<15;
                    V[(opcode & 0x0F00) >> 8] <<= 1;
                    pc += 2;//to verify
                    break;
                default:
                    printf ("Unknown opcode [0x8000]: 0x%X\n", opcode);
            }
            break;
        case 0x9000:
            if(V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
                pc += 2;
            break;
        case 0xA000:
            I = opcode & 0x0FFF;
            pc += 2;
            break;
        case 0xB000:
            pc = V[0] + (opcode & 0x0FFF);
            break;
        case 0xC000:
            V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF & (rand()%255);
            pc+=2;
            break;
        case 0xD000:
            {
                unsigned short x = V[(opcode & 0x0F00) >> 8];
                unsigned short y = V[(opcode & 0x00F0) >> 4];
                unsigned short height = opcode & 0x000F;
                unsigned short pixel;

                V[0xF] = 0;
                for (int yline = 0; yline < height; yline++)
                {
                    pixel = memory[I + yline];
                    for(int xline = 0; xline < 8; xline++)
                    {
                        if((pixel & (0x80 >> xline)) != 0)
                        {
                            if(gfx[(x + xline + ((y + yline) * 64))] == 1)
                                V[0xF] = 1;
                            gfx[x + xline + ((y + yline) * 64)] ^= 1;
                        }
                    }
                }

                drawFlag = true;
                pc += 2;
            }
            break;
        case 0xE000:
            switch(opcode & 0x00F0) {
                case 0x0090:
                    if (keyboard[V[(opcode & 0x0F00) >> 8]] != 0)
                        pc += 2;
                    break;
                case 0x00A0:
                    if (keyboard[V[(opcode & 0x0F00) >> 8]] == 0)
                        pc += 2;
                    break;
            }
            pc += 2;
            break;
        case 0xF000:
            switch(opcode & 0x00F0){
                case 0x0000:
                    switch(opcode & 0x000F){
                        case 0x0007:
                            V[(opcode & 0x0F00) >> 8] = delay_timer;
                            break;
                        case 0x000A:
                            break;
                        default:
                            printf("Todo,line213");
                    }
                    break;
                case 0x0010:
                    switch(opcode & 0x000F){
                        case 0x0005:
                            delay_timer = V[(opcode & 0x0F00) >> 8];
                            pc += 2;
                            break;
                        case 0x0008:
                            sound_timer = V[(opcode & 0x0F00) >> 8];
                            pc += 2;
                            break;
                        case 0x000E:
                            I += V[(opcode & 0x0F00) >> 8];
                            pc += 2;
                            break;
                        default:
                            printf("Unknown opcode [0xFx10]: 0x%X\n", opcode);
                    }
                    break;
                case 0x0020:
                    I = chip8_fontset[V[(opcode & 0x0F00) >> 8]];
                    pc += 2;
                    break;
                case 0x0030:
                    memory[I] = V[(opcode & 0x0F00) >> 8]/100;
                    memory[I+1] = (V[(opcode & 0x0F00) >> 8]/10)%10;
                    memory[I+2] = V[(opcode & 0x0F00) >> 8]%10;
                    break;
                case 0x0050:
                    for(int i = 0 ; i<(opcode & 0x0F00) >> 8; i++ ){
                        memory[I+i] = V[i];
                    }
                    break;
                case 0x0060:
                    for(int i = 0 ; i<(opcode & 0x0F00) >> 8; i++ ){
                        V[i] = memory[I+i];
                    }
                    break;
                default:
                    printf("Unknown opcode [0xF000]: 0x%X\n", opcode);
            }
            break;
        default:
            printf("unknown opcode: 0x%X\n", opcode);
    }
    // Execute Opcode

    // Update timers
    if(delay_timer > 0)
        delay_timer--;
    if(sound_timer > 0){
        if(sound_timer == 1)
            printf("Beep\n");
        sound_timer--;
    }
}