#include <cstdint>
#include <stack>
#include <iostream>
#include <fstream>
#include <bits/stdc++.h>
#include <cstdlib>
#include "sdl_interface.hpp"

class Chip8 {
    private:
        uint8_t registers[16]{};
        uint8_t memory[4096]{};
        uint16_t index_reg{};
        uint16_t pc{};
        uint16_t func_stack[16]{};
        uint8_t stack_pointer{};
        uint8_t delay_timer{};
        uint8_t sound_timer{};
        uint8_t keypad{};

        static constexpr unsigned int DISPLAY_WIDTH = 64;
        static constexpr unsigned int DISPLAY_HEIGHT = 32;

        uint32_t display[DISPLAY_HEIGHT][DISPLAY_WIDTH]{};
	    uint16_t opcode;

        static constexpr unsigned int FONT_SET_SIZE = 80;

        uint8_t fontset[FONT_SET_SIZE] =
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
        
        const unsigned int ROM_LOAD_START_ADDRESS = 0x200;
        const unsigned int FONT_LOAD_START_ADDRESS = 0x50;
        
        void OP_00E0() {
            memset(display, 0, sizeof(display));
        }

        void OP_00EE() {
            pc = func_stack[--stack_pointer];
        }

        void OP_1nnn() {
            pc = opcode - 0x1000u;
        }

        void OP_2nnn() {
            func_stack[++stack_pointer] = pc;
            pc = opcode - 0x2000u;
        }

        void OP_3xkk() {
            int x = (opcode & 0x0F00u) >> 8u;
            uint8_t val = opcode & 0x00FFu;
            if (registers[x] == val) {
                pc+=2;
            }
        }

        void OP_4xkk() {
            int x = (opcode & 0x0F00u) >> 8u;
            uint8_t val = opcode & 0x00FFu;
            if (registers[x] == val) {
                pc+=2;
            }
        }

        void OP_5xy0() {
            int x = (opcode & 0x0F00u) >> 8u;
            int y = (opcode & 0x00F0u) >> 4u;
            if(registers[x] == registers[y]) {
                pc+=2;
            }
        }

        void OP_6xkk() {
            int x = (opcode & 0x0F00u) >> 8u;
            uint8_t val = opcode & 0x00FFu;
            registers[x] = val;
        }

        void OP_7xkk() {
            int x = (opcode & 0x0F00u) >> 8u;
            uint8_t val = opcode & 0x00FFu;
            registers[x] = registers[x] + val;
        }

        void OP_8xy0() {
            int x = (opcode & 0x0F00u) >> 8u;
            int y = (opcode & 0x00F0u) >> 4u;
            registers[x] = registers[y];
        }

        void OP_8xy1() {
            int x = (opcode & 0x0F00u) >> 8u;
            int y = (opcode & 0x00F0u) >> 4u;
            registers[x] = registers[x] | registers[y];
        }

        void OP_8xy2() {
            int x = (opcode & 0x0F00u) >> 8u;
            int y = (opcode & 0x00F0u) >> 4u;
            registers[x] = registers[x] & registers[y];
        }

        void OP_8xy3() {
            int x = (opcode & 0x0F00u) >> 8u;
            int y = (opcode & 0x00F0u) >> 4u;
            registers[x] = registers[x] ^ registers[y];
        }

        void OP_8xy4() {
            int x = (opcode & 0x0F00u) >> 8u;
            int y = (opcode & 0x00F0u) >> 4u;
            int result = registers[x] + registers[y];
            if (result > 255u) {
                registers[0xFu] = 1;
            }
            else {
                registers[0xFu] = 0;
            }
            registers[x] = result;
        }

        void OP_8xy5() {
            int x = (opcode & 0x0F00u) >> 8u;
            int y = (opcode & 0x00F0u) >> 4u;
            uint8_t result = registers[x] - registers[y];
            if (registers[x] > registers[y]) {
                registers[0xFu] = 1;
            }
            else {
                registers[0xFu] = 0;
            }
            registers[x] = result;
        }

        void OP_8xy6() {
            int x = (opcode & 0x0F00u) >> 8u;
            if (registers[x] & 0x01u == 1u) {
                registers[0xFu] = 1;
            }
            else {
                registers[0xFu] = 0;
            }
            registers[x] = registers[x] >> 1;
        }

        void OP_8xy7() {
            int x = (opcode & 0x0F00u) >> 8u;
            int y = (opcode & 0x00F0u) >> 4u;
            uint8_t result = registers[y] - registers[x];
            if (registers[y] > registers[x]) {
                registers[0xFu] = 1;
            }
            else {
                registers[0xFu] = 0;
            }
            registers[x] = result;
        }

        void OP_8xyE() {
            int x = (opcode & 0x0F00u) >> 8u;
            if (registers[x] & 0x80u == 1u) {
                registers[0xFu] = 1;
            }
            else {
                registers[0xFu] = 0;
            }
            registers[x] = registers[x] << 1;
        }

        void OP_9xy0() {
            int x = (opcode & 0x0F00u) >> 8u;
            int y = (opcode & 0x00F0u) >> 4u;
            if(registers[x] != registers[y]) {
                pc+=2;
            }
        }

        void OP_Annn() {
            index_reg = opcode & 0x0FFFu;
        }

        void OP_Bnnn() {
            pc = (opcode & 0x0FFFu) + registers[0];
        }

        void OP_Cxkk() {
            srand(time(0));
            uint8_t random_num = rand() % 256;
            int x = (opcode & 0x0F00u) >> 8u;
            registers[x] = (opcode & 0x00FFu) & random_num;
        }

        void OP_Dxyn() {
            int x = registers[(opcode & 0x0F00u) >> 8u];
            int y = registers[(opcode & 0x00F0u) >> 4u];
            int n = opcode & 0x000Fu;
            bool set_flag = false;

            for(int k = 0; k < n; k++) {
                int y_pos = (y+k) % 32;
                int mem_pos = index_reg + k;
                for(int l = 0; l < 8; l++) {
                    uint8_t sprite_bit = (memory[mem_pos] >> (7 - l)) & 1;
                    uint32_t &pixel = display[y_pos][(x + l) % 64];

                    if (sprite_bit && pixel) {
                        set_flag = true;
                    }

                    pixel ^= sprite_bit;
                }
            }
            
            registers[0xF] = set_flag ? 1 : 0;
        }

        void OP_Ex9E() {
            // Todo, requires SDL interface first
        }

        void OP_ExA1() {
            // Todo, requires SDL interface first
        }

        void OP_Fx07() {
            int x = (opcode & 0x0F00u) >> 8u;
            registers[x] = delay_timer;
        }

        void OP_Fx0A() {
            // Todo, requires SDL interface first
        }

        void OP_Fx15() {
            int x = (opcode & 0x0F00u) >> 8u;
            delay_timer = registers[x];
        }

        void OP_Fx18() {
            int x = (opcode & 0x0F00u) >> 8u;
            sound_timer = registers[x];
        }

        void OP_Fx1E() {
            int x = (opcode & 0x0F00u) >> 8u;
            index_reg += registers[x];
        }

        void OP_Fx29() {
            int x = (opcode & 0x0F00u) >> 8u;
            index_reg = FONT_LOAD_START_ADDRESS + registers[x] * 5;
        }

        void OP_Fx33() {
            int x = (opcode & 0x0F00u) >> 8u;
            memory[index_reg] = registers[x] / 100;
            memory[index_reg + 1] = (registers[x] % 100) /10;
            memory[index_reg + 2] = (registers[x] % 100) % 10;
        }

        void OP_Fx55() {
            int x = (opcode & 0x0F00u) >> 8u;
            for (int i = 0; i < x; i++) {
                memory[index_reg + i] = registers[i];
            }
        }

        void OP_Fx65() {
            int x = (opcode & 0x0F00u) >> 8u;
            for (int i = 0; i < x; i++) {
                registers[i] = memory[index_reg + i];
            }
        }

        void decode_opcode_execute() {
            uint16_t msn = opcode & 0xF000u;
            switch (msn)
            {
                case 0x0000u:
                    decode_OP0xxx_execute();
                    break;
                case 0x1000u:
                    OP_1nnn();
                    break;
                case 0x2000u:
                    OP_2nnn();
                    break;
                case 0x3000u:
                    OP_3xkk();
                    break;
                case 0x4000u:
                    OP_4xkk();
                    break;
                case 0x5000u:
                    OP_5xy0();
                    break;
                case 0x6000u:
                    OP_6xkk();
                    break;
                case 0x7000u:
                    OP_7xkk();
                    break;
                case 0x8000u:
                    decode_OP8xxx_execute();
                    break;
                case 0x9000u:
                    OP_9xy0();
                    break;
                case 0xA000u:
                    OP_Annn();
                    break;
                case 0xB000u:
                    OP_Bnnn();
                    break;
                case 0xC000u:
                    OP_Cxkk();
                    break;
                case 0xD000u:
                    OP_Dxyn();
                    break;
                case 0xE000u:
                    decode_OPExxx_execute();
                    break;
                case 0xF000u:
                    decode_OPFxxx_execute();
                    break;
                default:
                    break;
            }
        }

        void decode_OP0xxx_execute() {
            uint16_t lsn = opcode & 0x000Fu;
            switch (lsn)
            {
                case 0x0000u:
                    OP_00E0();
                    break;
                case 0x000Eu:
                    OP_00EE();
                    break;
                default:
                    break;
            }
        }

        void decode_OP8xxx_execute() {
            uint16_t lsn = opcode & 0x000Fu;
            switch (lsn)
            {
                case 0x0000u:
                    OP_8xy0();
                    break;
                case 0x0001u:
                    OP_8xy1();
                    break;
                case 0x0002u:
                    OP_8xy2();
                    break;
                case 0x0003u:
                    OP_8xy3();
                    break;
                case 0x0004u:
                    OP_8xy4();
                    break;
                case 0x0005u:
                    OP_8xy5();
                    break;
                case 0x0006u:
                    OP_8xy6();
                    break;
                case 0x0007u:
                    OP_8xy7();
                    break;
                case 0x000Eu:
                    OP_8xyE();
                    break;
                default:
                    break;
            }
        }

        void decode_OPExxx_execute() {
            uint16_t lsn = opcode & 0x000Fu;
            switch (lsn)
            {
                case 0x0001u:
                    OP_ExA1();
                    break;
                case 0x000Eu:
                    OP_Ex9E();
                    break;
                default:
                    break;
            }
        }

        void decode_OPFxxx_execute() {
            uint16_t lsn2 = opcode & 0x00FFu;
            switch (lsn2)
            {
                case 0x0007u:
                    OP_Fx07();
                    break;
                case 0x000Au:
                    OP_Fx0A();
                    break;
                case 0x0015u:
                    OP_Fx15();
                    break;
                case 0x0018u:
                    OP_Fx18();
                    break;
                case 0x001Eu:
                    OP_Fx1E();
                    break;
                case 0x0029u:
                    OP_Fx29();
                    break;
                case 0x0033u:
                    OP_Fx33();
                    break;
                case 0x0055u:
                    OP_Fx55();
                    break;
                case 0x0065u:
                    OP_Fx65();
                    break;
                default:
                    break;
            }
        }
        
    public:
        bool load_rom(std::string rom_name) {
            std::ifstream infile;
            infile.open(rom_name, std::ios_base::binary | std::ios::ate);
            
            if(!infile) {
                return false;
            }

            int size = infile.tellg();

            if(size > 3584) {
                return false;
            }

            infile.seekg(0, infile.beg);

            char* buffer = new char[size];
            infile.read(buffer, size);

            for(int i = 0; i < size; i++) {
                memory[i + ROM_LOAD_START_ADDRESS] = buffer[i];
            }
            
            delete[] buffer;
            infile.close();
            return true;
        };

        void load_font() {
            for(int i = 0; i<FONT_SET_SIZE; i++){
                memory[FONT_LOAD_START_ADDRESS + i] = fontset[i];
            }
        }

        void screen_test() {
            SDLInterface sdl_interface;
            sdl_interface.create_window();

            for(int y = 0; y < DISPLAY_HEIGHT; y++){
                for(int x = 0; x < DISPLAY_WIDTH; x++) {
                    if ((x+y) % 2 == 0) {
                        display[y][x] = 1;
                    }
                }
            }

            sdl_interface.draw_display(display);

            SDL_Delay(5000);

            sdl_interface.destroy_window();
        }
};

int main() {
    Chip8 chip8;
    chip8.screen_test();

    return 1;
}