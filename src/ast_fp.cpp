#include "ast_fp.hpp"

#define MAX_STACK_FRAME_SIZE 2040 // Almost 2kB

namespace ast {

    void setupFramePointer(std::ostream& stream, int) {
        stream << "addi sp, sp, -" << MAX_STACK_FRAME_SIZE << std::endl;
        stream << "sw s0, " << (MAX_STACK_FRAME_SIZE - 4) << "(sp)" << std::endl;
        stream << "sw ra, " << (MAX_STACK_FRAME_SIZE - 8) << "(sp)" << std::endl;
        stream << "addi s0, sp, " << (MAX_STACK_FRAME_SIZE / 2) << std::endl;
    }

    void restoreFramePointer(std::ostream& stream, int) {
        stream << "lw ra, " << (MAX_STACK_FRAME_SIZE - 8) << "(sp)" << std::endl;
        stream << "lw s0, " << (MAX_STACK_FRAME_SIZE - 4) << "(sp)" << std::endl;
        stream << "addi sp, sp, " << MAX_STACK_FRAME_SIZE << std::endl;
    }

}
