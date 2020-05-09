#pragma once

#include <cstdint>

using namespace std;

struct Reg {
    public:
        enum class Type : uint32_t {
            WIDTH8  = 0,
            WIDTH16 = 1,
            WIDTH32 = 2,
            UNUSED  = 3,
        };

        static Type chooseMaxWidth(Reg& arg1, Reg& arg2) {
            int a1_size = (int)arg1.getType();
            int a2_size = (int)arg2.getType();
            int max_size = a1_size >= a2_size ? a1_size : a2_size;
            return (Type) max_size;
        }

        static Type chooseMaxWidth(Reg& arg1, Type ti) {
            int a1_size = (int)arg1.getType();
            int a2_size = (int)ti;
            int max_size = a1_size >= a2_size ? a1_size : a2_size;
            return (Type) max_size;
        }

        int width() const;

        Type getType() const;
        uint32_t getData() const;

        void setType(Type);
        void setData(uint32_t);

    private:
        Type type;

        union {
            union {
                uint8_t w8_ll;
                uint8_t w8_lh;
                uint8_t w8_hl;
                uint8_t w8_hh;
            };
            union {
                uint16_t w16_l;
                uint16_t w16_h;
            };
            uint32_t w32;
        };
};

