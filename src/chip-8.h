
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
    unsigned char gfx[64 * 32];
    unsigned char delay_timer;
    unsigned char sound_timer;
    unsigned short stack[16];
    unsigned short stack_pointer;
    unsigned char keyboard[16];
    bool drawFlag;

    void initialize();
    void emulateCycle();
};
