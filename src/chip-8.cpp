#include "chip-8.h"
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <unordered_map>
#include <utility>
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
    // Initialize registers and memory once
    pc = 0x200;
    opcode = 0;
    I = 0;
    stack_pointer = 0;
    drawFlag = false;
    sound_timer = 0;
    delay_timer = 0;
    int i;
    //load fontset
    for(i = 0; i<4096; i++){
        memory[i] = 0;
    }
    for(i = 0 ; i < 80 ; i++){
        memory[i] = chip8_fontset[i];
    }
    for(i = 0; i<16; i++){
        V[i] = 0;
    }
    for(int i = 0; i< 64*32 ; i++){
        gfx[i] = false;
    }
}
Chip8::~Chip8(){

}
bool Chip8::loadrom(string rom) {
    char* buffer;
    streampos size;

    string source_name = move(rom);
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
void Chip8::CLS(){
    for (int i = 0; i < 64*32; i++) {
        gfx[i] = false;
    }

    drawFlag = true;
    pc += 2;
}
void Chip8::RET(){
    pc = stack[--stack_pointer];
    pc+=2;
}
void Chip8::JP_addr(){
    pc = opcode & 0x0FFF;
}
void Chip8::CALL_addr(){
    stack[stack_pointer++] = pc;
    pc = opcode & 0x0FFF;
}
void Chip8::SE_vx_byte(){
    if(V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
        pc += 2;
    pc += 2;
}
void Chip8::SNE_vx_byte(){
    if(V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
        pc += 2;
    pc += 2;
}
void Chip8::SE_vx_vy(){
    if(V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4])
        pc += 2;
    pc += 2;
}
void Chip8::LD_vx_byte(){
    V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
    pc += 2;
}
void Chip8::ADD_vx_byte(){
    V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
    pc += 2;
}
void Chip8::LD_vx_vy(){
    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
    pc += 2;
}
void Chip8::OR_vx_vy(){
    V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
    pc += 2;
}
void Chip8::AND_vx_vy(){
    V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
    pc += 2;
}
void Chip8::XOR_vx_vy(){
    V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
    pc += 2;
}
void Chip8::ADD_vx_vy(){
    if(V[(opcode & 0x00F0) >> 4] + V[(opcode & 0x0F00) >> 8] > 255)
        V[0xF] = 1; //carry
    else
        V[0xF] = 0;
    V[(opcode & 0x0F00) >> 8] = (V[(opcode & 0x00F0) >> 4] + V[(opcode & 0x0F00) >> 8])&255;
    pc += 2;
}
void Chip8::SUB_vx_vy(){
    if(V[(opcode & 0x0F00) >> 8]>=V[(opcode & 0x00F0) >> 4])
        V[0xF] = 1;
    else
        V[0xF] = 0;//borrow
    V[(opcode & 0x0F00) >> 8] -= V[(opcode & 0x00F0) >> 4];
    pc += 2;
}
void Chip8::SHR_vx_vy(){
    V[0xF] = V[(opcode & 0x0F00) >> 8] <<7;
    V[(opcode & 0x0F00) >> 8]  /= 2;//to verify
    pc += 2;
}
void Chip8::SUBN_vx_vy(){
    if(V[(opcode & 0x00F0) >> 4] >= V[(opcode & 0x0F00) >> 8])
        V[0xF] = 1;
    else
        V[0xF] = 0;//borrow
    V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4]-V[(opcode & 0x0F00) >> 8];
    pc += 2;
}
void Chip8::SHL_vx_vy(){
    V[0xF] = V[(opcode & 0x0F00) >> 8] >>7;
    V[(opcode & 0x0F00) >> 8] *= 2;
    pc += 2;//to verify
}
void Chip8::SNE_vx_vy(){
    if(V[(opcode & 0x0F00) >> 8] != V[(opcode & 0x00F0) >> 4])
        pc += 2;
    pc += 2;
}
void Chip8::LD_I_addr(){
    I = opcode & 0x0FFF;
    pc += 2;
}
void Chip8::JP_v0_addr(){
    pc = V[0] + (opcode & 0x0FFF);
}
void Chip8::RND_vx_byte(){
    V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF & (rand()%255);
    pc+=2;
}
void Chip8::DRW_vy_vy_nibble(){
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
void Chip8::SKP_vx(){
    if (keyboard[V[(opcode & 0x0F00) >> 8]] != 0)
        pc += 2;
    pc += 2;
}
void Chip8::SKNP_vx(){
    if (keyboard[V[(opcode & 0x0F00) >> 8]] == 0)
        pc += 2;
    pc += 2;
}
void Chip8::LD_vx_dt(){
    V[(opcode & 0x0F00) >> 8] = delay_timer;
    pc += 2;
}
void Chip8::LD_vx_k(){
    bool press = false;
    for(int i=0; i<16; i++){
        if(keyboard[i] != 0){
            V[(opcode & 0x0F00) >> 8] = i;
            press = true;
        }
    }
    if(!press){
        return;
    }
    pc += 2;
}
void Chip8::LD_dt_vx(){
    delay_timer = V[(opcode & 0x0F00) >> 8];
    pc += 2;
}
void Chip8::LD_st_vx(){
    sound_timer = V[(opcode & 0x0F00) >> 8];
    pc += 2;
}
void Chip8::ADD_i_vx(){
    I += V[(opcode & 0x0F00) >> 8];
    pc += 2;
}
void Chip8::LD_f_vx(){
    I = chip8_fontset[V[(opcode & 0x0F00) >> 8]];
    pc += 2;
}
void Chip8::LD_B_vx(){
    printf("%d\n",V[(opcode & 0x0F00) >> 8]);
    memory[I] = V[(opcode & 0x0F00) >> 8]/100;
    printf("%d\n",memory[I]);
    memory[I+1] = (V[(opcode & 0x0F00) >> 8]/10)%10;
    printf("%d\n",memory[I+1]);
    memory[I+2] = V[(opcode & 0x0F00) >> 8]%10;
    printf("%d\n",memory[I+2]);
    pc += 2;
}
void Chip8::LD_i_vx(){
    printf("%d\n",(opcode & 0x0F00) >> 8);
    for(int i = 0 ; i<=(opcode & 0x0F00) >> 8; i++ ){
        memory[I+i] = V[i];
        printf("%d\n",memory[I+i]);
    }
    pc += 2;
}
void Chip8::LD_vx_i(){
    for(int i = 0 ; i<=(opcode & 0x0F00) >> 8; i++ ){
        V[i] = memory[I+i];
    }
    pc += 2;
}
void Chip8::decode() {
    unsigned short calculated_opcode = opcode & 0xF000;
    switch(calculated_opcode){
        case 0x0000:
        case 0x8000:
        case 0xE000:
            calculated_opcode = opcode & 0xF00F;
            break;
        case 0xF000:
            calculated_opcode = opcode & 0xF0FF;
        default:
            break;
    }
    if(decoder.contains(calculated_opcode)){
        using OpcodeHandler = void(Chip8::*)();
        OpcodeHandler opcode_handler = decoder.at(calculated_opcode);
        (this->*opcode_handler)();
    }

    else
        printf ("Unknown opcode [0x%X]: 0x%X\n",
                calculated_opcode, opcode);

}
void Chip8::emulateCycle(){
    // Fetch Opcode
    opcode = memory[pc] << 8 | memory[pc+1];
    printf("0x%X-------",opcode);
    // Execute Opcode
    decode();
    // Update timers
    if(delay_timer > 0)
        delay_timer--;
    if(sound_timer > 0){
        if(sound_timer == 1)
            printf("Beep\n");
        sound_timer--;
    }
}
/*void Chip8::takeinput() {
    SDL_Event event;

    for (unsigned char & i : keyboard) {
        i = 0;
    }
    while( SDL_PollEvent( &event ) ){
        SDL_Keycode keypressed = event.key.keysym.sym;
        if(event.type == SDL_KEYDOWN){
            //to edit later hadchi ila b9at
            if(keyboard_map.find(keypressed) != keyboard_map.end()){
                keyboard[keyboard_map[keypressed]] = 1;
            }
        }
        else if(event.type == SDL_KEYUP){
            if(keyboard_map.find(keypressed) != keyboard_map.end()){
                keyboard[keyboard_map[keypressed]] = 0;
            }
        }
    }

}*/


