#pragma once

#include <deque>
using std::deque;

extern "C"{
    #include "lang.h"
}


struct checkResult { deque<struct type *> input; struct type * output; bool success; };

bool TypeComp(const struct type * type1, const struct type * type2 );

void DeleteResult(struct checkResult result);
void DeleteType(struct type * typ);
struct type * CopyType(struct type * typ);

struct checkResult check(struct expr * root);