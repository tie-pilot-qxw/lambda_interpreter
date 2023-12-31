#pragma once

extern "C"{
    #include "lang.h"
}


struct checkResult { struct type * t; bool success; };

bool TypeComp(const struct type * type1, const struct type * type2 );

void DeleteType(struct type * typ);
struct type * CopyType(struct type * typ);

struct checkResult check(struct expr * root, bool inner = false);