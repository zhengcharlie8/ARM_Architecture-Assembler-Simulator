#include "Reg.h"

#include <cstdio>
#include <cstdlib>

int
Reg::width() const {
    switch (type) {
        case Type::WIDTH8  : return 1;
        case Type::WIDTH16 : return 2;
        case Type::WIDTH32 : return 4;
        case Type::UNUSED  : {
            puts("BAD TYPE INFO");
            exit(-1);
        }
    }
}

Reg::Type
Reg::getType() const {
    return type;
}

uint32_t
Reg::getData() const {
    switch (type) {
        case Type::WIDTH8  : return w8_ll;
        case Type::WIDTH16 : return w16_l;
        case Type::WIDTH32 : return w32;
        case Type::UNUSED  : {
            puts("BAD TYPE INFO");
            exit(-1);
        }
    }
}

void
Reg::setType(Type type) {
    this->type = type;
}

void
Reg::setData(uint32_t data) {
    switch (type) {
        case Type::WIDTH8  : w8_ll = (uint8_t)  data; break;
        case Type::WIDTH16 : w16_l = (uint16_t) data; break;
        case Type::WIDTH32 : w32   = (uint32_t) data; break;
        case Type::UNUSED  : {
            puts("BAD TYPE INFO");
            exit(-1);
        }
    }
}
