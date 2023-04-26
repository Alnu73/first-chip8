#pragma once
#pragma once
//CHIP-8 Header

#define V_AMOUNT 16
#define RAM_AMOUNT 4096
#define KEYS_AMOUNT 16
#define PIXEL_AMOUNT 2048 //64*32

class chip8 {
public:
    //Constructor and destructor
    chip8();
    ~chip8();
    bool draw_flag;

    void emulate_cycle();
    bool load_application(const char*);

    unsigned char gfx[PIXEL_AMOUNT];     //Graphic card
    unsigned char keys[KEYS_AMOUNT];   //Buttons

private:
    //CHIP-8 registers
    unsigned short pc;      //Program counter
    unsigned short I;       //Index register
    unsigned short sp;      //Stack pointer register
    unsigned short opcode;
    unsigned char delay_timer;
    unsigned char sound_timer;

    unsigned char v[V_AMOUNT];
    unsigned short stack[16];
    unsigned char memory[RAM_AMOUNT];

    void init();
};
