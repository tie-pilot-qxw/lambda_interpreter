#pragma once

#include "check.hpp"

expr * simpl (expr * root);
expr * subsitute (expr * func, expr * var);