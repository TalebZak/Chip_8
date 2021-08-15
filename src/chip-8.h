#include <iostream>
#include <unordered_map>
#include <SDL.h>
using namespace std;


class Chip8{
    /*0x000(0) to 0x1FF(511) CHIP-8 interpreter
     0x050(80) to 0x0A0(160) for fontset
     0x200(512) to 0xFFF(4095) ROM and work RAM*/
public:
    Chip8();
    ~Chip8();
    unsigned short opcode;
    unsigned char memory[4096];
    unsigned char V[16];
    unsigned short I;
    unsigned short pc;
    bool gfx[64 * 32];
    unsigned char delay_timer;
    unsigned char sound_timer;
    unsigned short stack[16];
    unsigned short stack_pointer;
    unsigned char keyboard[16];
    bool drawFlag;
    bool loadrom(string);
    void emulateCycle();
private:
    void takeinput();
    void CLS();
    void RET();
    void JP_addr();
    void CALL_addr();
    void SE_vx_byte();
    void SNE_vx_byte();
    void SE_vx_vy();
    void LD_vx_byte();
    void ADD_vx_byte();
    void LD_vx_vy();
    void OR_vx_vy();
    void AND_vx_vy();
    void XOR_vx_vy();
    void ADD_vx_vy();
    void SUB_vx_vy();
    void SHR_vx_vy();
    void SUBN_vx_vy();
    void SHL_vx_vy();
    void SNE_vx_vy();
    void LD_I_addr();
    void JP_v0_addr();
    void RND_vx_byte();
    void DRW_vy_vy_nibble();
    void SKP_vx();
    void SKNP_vx();
    void LD_vx_dt();
    void LD_vx_k();
    void LD_dt_vx();
    void LD_st_vx();
    void ADD_i_vx();
    void LD_f_vx();
    void LD_B_vx();
    void LD_i_vx();
    void LD_vx_i();
    void decode();
    const unordered_map<SDL_Keycode,int> keyboard_map = {
            {SDLK_1,0x0},
            {SDLK_2,0x1},
            {SDLK_3,0x2},
            {SDLK_4,0x3},
            {SDLK_q,0x4},
            {SDLK_w,0x5},
            {SDLK_e,0x6},
            {SDLK_r,0x7},
            {SDLK_a,0x8},
            {SDLK_s,0x9},
            {SDLK_d,0xA},
            {SDLK_f,0xB},
            {SDLK_z,0xC},
            {SDLK_x,0xD},
            {SDLK_c,0xE},
            {SDLK_v,0xF},
    };
    const unordered_map<unsigned short,void(Chip8::*)()> decoder ={
            {0x0000,&Chip8::CLS},
            {0x000E,&Chip8::RET},
            {0x1000,&Chip8::JP_addr},
            {0x2000,&Chip8::CALL_addr},
            {0x3000,&Chip8::SE_vx_byte},
            {0x4000,&Chip8::SNE_vx_byte},
            {0x5000,&Chip8::SE_vx_vy},
            {0x6000,&Chip8::LD_vx_byte},
            {0x7000,&Chip8::ADD_vx_byte},
            {0x8000,&Chip8::LD_vx_vy},
            {0x8001,&Chip8::OR_vx_vy},
            {0x8002,&Chip8::AND_vx_vy},
            {0x8003,&Chip8::XOR_vx_vy},
            {0x8004,&Chip8::ADD_vx_vy},
            {0x8005,&Chip8::SUB_vx_vy},
            {0x8006,&Chip8::SHR_vx_vy},
            {0x8007,&Chip8::SUBN_vx_vy},
            {0x800E,&Chip8::SHL_vx_vy},
            {0x9000,&Chip8::SNE_vx_vy},
            {0xA000,&Chip8::LD_I_addr},
            {0xB000,&Chip8::JP_v0_addr},
            {0xC000,&Chip8::RND_vx_byte},
            {0xD000,&Chip8::DRW_vy_vy_nibble},
            {0xE00E,&Chip8::SKP_vx},
            {0xE001,&Chip8::SKNP_vx},
            {0xF007,&Chip8::LD_vx_dt},
            {0xF00A,&Chip8::LD_vx_k},
            {0xF015,&Chip8::LD_dt_vx},
            {0xF018,&Chip8::LD_st_vx},
            {0xF01E,&Chip8::ADD_i_vx},
            {0xF029,&Chip8::LD_f_vx},
            {0xF033,&Chip8::LD_B_vx},
            {0xF055,&Chip8::LD_i_vx},
            {0xF065,&Chip8::LD_vx_i},
            };
};

