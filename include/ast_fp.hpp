#pragma once
#include <iostream>
#include <string>
#include "ast_context.hpp"


namespace ast {

    extern void setupFramePointer(std::ostream& stream, int localVarSpace);

    extern void restoreFramePointer(std::ostream& stream, int localVarSpace);

}

