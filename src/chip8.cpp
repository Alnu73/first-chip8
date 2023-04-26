#define _CRT_SECURE_NO_WARNINGS

#include "chip8.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define PC_START 0x200
#define FONTSET_AMOUNT 80

unsigned char chip8_fontset[FONTSET_AMOUNT] =
{
    0xF0, 0x90, 0x90, 0x90, 0xF0, //0
    0x20, 0x60, 0x20, 0x20, 0x70, //1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
    0x90, 0x90, 0xF0, 0x10, 0x10, //4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
    0xF0, 0x10, 0x20, 0x40, 0x40, //7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
    0xF0, 0x90, 0xF0, 0x90, 0x90, //A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
    0xF0, 0x80, 0x80, 0x80, 0xF0, //C
    0xE0, 0x90, 0x90, 0x90, 0xE0, //D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
    0xF0, 0x80, 0xF0, 0x80, 0x80  //F
};

chip8::chip8() {

}

chip8::~chip8() {

}

void chip8::init() {
    //Set registers
    pc = PC_START;      //512 MB offset
    opcode = 0;
    I = 0;
    sp = 0;
    //Clear graphics
    for (int i = 0; i < PIXEL_AMOUNT; i++)
        gfx[i] = 0;
    //Clear stack
    for (int i = 0; i < 16; i++)
        stack[i] = 0;
    //Clear general-purpose registers and button registers
    for (int i = 0; i < 16; i++)
        v[i] = keys[i] = 0;
    //Clear RAM
    for (int i = 0; i < RAM_AMOUNT; i++)
        memory[i] = 0;
    //Load fontset into memory
    for (int i = 0; i < FONTSET_AMOUNT; i++)
        memory[i] = chip8_fontset[i];
    // Reset timers
    delay_timer = 0;
    sound_timer = 0;
    // Clear screen once
    draw_flag = true;

    srand(time(NULL));
}

void chip8::emulate_cycle() {
    //Fetch current opcode
    opcode = memory[pc] << 8 | memory[pc + 1];
    //Opcodes implementation
    switch (opcode & 0xF000) {
    case 0x0000:
        switch (opcode & 0x000F) {
        case 0x0000:    //00E0
            for (int i = 0; i < PIXEL_AMOUNT; i++)
                gfx[i] = 0x0;
            draw_flag = true;
            pc += 2;    //Next instruction
            break;
        case 0x000E:    //00EE
            --sp;
            pc = stack[sp];
            pc += 2;
            break;
        default:
            printf("Unknown opcode [0x0000]: 0x%X\n", opcode);
        }
        break;

    case 0x1000:        //1NNN
        pc = opcode & 0x0FFF;
        break;

    case 0x2000:        //2NNN
        stack[sp] = pc;
        ++sp;           //Calling the subroutine at NNN
        pc = opcode & 0x0FFF;
        break;

    case 0x3000:        //3XNN
        if (v[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF))
            pc += 4;
        else
            pc += 2;
        break;

    case 0x4000:        //4XNN
        if (v[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF))
            pc += 4;
        else
            pc += 2;
        break;

    case 0x5000:        //5XY0
        if (v[(opcode & 0x0F00) >> 8] == v[(opcode & 0x00F0) >> 4])
            pc += 4;
        else
            pc += 2;
        break;

    case 0x6000:        //6XNN
        v[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
        pc += 2;
        break;

    case 0x7000:
        v[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
        pc += 2;
        break;

    case 0x8000:
        switch (opcode & 0x000F) {      //8XY-
        case 0x0000:
            v[(opcode & 0x0F00) >> 8] = v[(opcode & 0x00F0) >> 4];
            pc += 2;
            break;

        case 0x0001:
            v[(opcode & 0x0F00) >> 8] |= v[(opcode & 0x00F0) >> 4];
            pc += 2;
            break;

        case 0x0002:
            v[(opcode & 0x0F00) >> 8] &= v[(opcode & 0x00F0) >> 4];
            pc += 2;
            break;

        case 0x0003:
            v[(opcode & 0x0F00) >> 8] ^= v[(opcode & 0x00F0) >> 4];
            pc += 2;
            break;

        case 0x0004:
            if (v[(opcode & 0x00F0 >> 4)] > (0xFF - v[(opcode & 0x0F00) >> 8]))
                v[0xF] = 1;
            else
                v[0xF] = 0;
            v[(opcode & 0x0F00) >> 8] += v[(opcode & 0x00F0) >> 4];
            pc += 2;
            break;

        case 0x0005:
            if (v[(opcode & 0x00F0 >> 4)] > (v[(opcode & 0x0F00) >> 8]))
                v[0xF] = 0;
            else
                v[0xF] = 1;
            v[(opcode & 0x0F00) >> 8] -= v[(opcode & 0x00F0) >> 4];
            pc += 2;
            break;

        case 0x0006:
            v[0xF] = v[(opcode & 0x0F00) >> 8] & 0x1;
            v[(opcode & 0x0F00) >> 8] >>= 1;
            pc += 2;
            break;

        case 0x0007:
            if (v[(opcode & 0x0F00) >> 8] > v[(opcode & 0x00F0) >> 4])
                v[0xF] = 0;
            else
                v[0xF] = 1;
            v[(opcode & 0x0F00) >> 8] = v[(opcode & 0x00F0) >> 4] - v[(opcode & 0x0F00) >> 8];
            pc += 2;
            break;

        case 0x000E:
            v[0xF] = v[(opcode & 0x0F00) >> 8] >> 7;
            v[(opcode & 0x0F00) >> 8] <<= 1;
            pc += 2;
            break;
        }
        break;

    case 0x9000:        //9XY0
        if (v[(opcode & 0x0F00) >> 8] != v[(opcode & 0x00F0) >> 4])
            pc += 4;
        else
            pc += 2;
        break;

    case 0xA000:        //ANNN
        I = opcode & 0x0FFF;
        pc += 2;
        break;

    case 0xB000:        //BNNN
        pc += v[0] + (opcode & 0x0FFF);
        break;

    case 0xC000:        //CXNN
        v[(opcode & 0x0F00) >> 8] = (opcode & 0x00FF) & (rand() % 0xFF);
        pc += 2;
        break;

    case 0xD000: {      //DXYN
        unsigned short x = v[(opcode & 0x0F00) >> 8];
        unsigned short y = v[(opcode & 0x00F0) >> 4];
        unsigned short height = opcode & 0x000F;
        unsigned short pixel;

        v[0xF] = 0;
        for (int yline = 0; yline < height; yline++)
        {
            pixel = memory[I + yline];
            for (int xline = 0; xline < 8; xline++)
            {
                if ((pixel & (0x80 >> xline)) != 0)
                {
                    if (gfx[(x + xline + ((y + yline) * 64))] == 1)
                    {
                        v[0xF] = 1;
                    }
                    gfx[x + xline + ((y + yline) * 64)] ^= 1;
                }
            }
        }

        draw_flag = true;
        pc += 2;
        break;
    }

    case 0xE000:
        switch (opcode & 0x00FF) {
        case 0x009E:        //EX9E
            if (keys[v[(opcode & 0x0F00) >> 8]] != 0)    //Button pressed
                pc += 4;
            else
                pc += 2;
            break;

        case 0x00A1:        //EXA1
            if (keys[v[(opcode & 0x0F00) >> 8]] == 0)    //Button not pressed
                pc += 4;
            else
                pc += 2;
            break;
        }
        break;

    case 0xF000:
        switch (opcode & 0x00FF) {
        case 0x0007:        //FX07
            v[(opcode & 0x0F00) >> 8] = delay_timer;
            pc += 2;
            break;

        case 0x000A: {       //FX0A
            bool keyPress = false;
            for (int i = 0; i < KEYS_AMOUNT; ++i)
            {
                if (keys[i] != 0)
                {
                    v[(opcode & 0x0F00) >> 8] = i;
                    keyPress = true;
                }
            }
            // If we didn't received a keypress, skip this cycle and try again.
            if (!keyPress)
                return;

            pc += 2;
            break;
        }

        case 0x0015:        //FX15
            delay_timer = v[(opcode & 0x0F00) >> 8];
            pc += 2;
            break;

        case 0x0018:        //FX18
            sound_timer = v[(opcode & 0x0F00) >> 8];
            pc += 2;
            break;

        case 0x001E:        //FX1E
            if (I + v[(opcode & 0x0F00) >> 8] > 0xFFF)	// VF is set to 1 when range overflow (I+VX>0xFFF), and 0 when there isn't.
                v[0xF] = 1;
            else
                v[0xF] = 0;
            I += v[(opcode & 0x0F00) >> 8];
            pc += 2;
            break;

        case 0x0029:        //FX29
            I = v[(opcode & 0x0F00) >> 8] * 0x5;        //The address has to be multiplied by 5 to access the fontset correctly
            pc += 2;
            break;

        case 0x0033:        //FX33
            memory[I] = v[(opcode & 0x0F00) >> 8] / 100;
            memory[I + 1] = (v[(opcode & 0x0F00) >> 8] / 10) % 10;
            memory[I + 2] = (v[(opcode & 0x0F00) >> 8] % 100) % 10;
            pc += 2;
            break;

        case 0x0055:        //FX55
            for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
                memory[I + i] = v[i];

            // On the original interpreter, when the operation is done, I = I + X + 1.
            I += ((opcode & 0x0F00) >> 8) + 1;
            pc += 2;
            break;

        case 0x0065:        //FX65
            for (int i = 0; i <= ((opcode & 0x0F00) >> 8); ++i)
                v[i] = memory[I + i];

            // On the original interpreter, when the operation is done, I = I + X + 1.
            I += ((opcode & 0x0F00) >> 8) + 1;
            pc += 2;
            break;

        default:
            printf("Unknown opcode [0xF000]: 0x%X\n", opcode);
        }
        break;
    default:
        printf("Unknown opcode: 0x%X\n", opcode);
    }
    // Update timers
    if (delay_timer > 0)
        --delay_timer;

    if (sound_timer > 0)
    {
        if (sound_timer == 1)
            printf("BEEP!\n");
        --sound_timer;
    }
}

//TODO: Versione con fstream
bool chip8::load_application(const char* filename) {
    init();
    printf("Loading: %s\n", filename);

    // Open file
    FILE* pFile = fopen(filename, "rb");
    if (pFile == NULL)
    {
        fputs("File error", stderr);
        return false;
    }

    // Check file size
    fseek(pFile, 0, SEEK_END);
    long lSize = ftell(pFile);
    rewind(pFile);
    printf("Filesize: %d\n", (int)lSize);

    // Allocate memory to contain the whole file
    char* buffer = (char*)malloc(sizeof(char) * lSize);
    if (buffer == NULL)
    {
        fputs("Memory error", stderr);
        return false;
    }

    // Copy the file into the buffer
    size_t result = fread(buffer, 1, lSize, pFile);
    if (result != lSize)
    {
        fputs("Reading error", stderr);
        return false;
    }

    // Copy buffer to Chip8 memory
    if ((4096 - 512) > lSize)
    {
        for (int i = 0; i < lSize; ++i)
            memory[i + 512] = buffer[i];
    }
    else
        printf("Error: ROM too big for memory");

    // Close file, free buffer
    fclose(pFile);
    free(buffer);

    return true;
}