//
// Created by tate on 24-07-18.
//

#ifndef YODA_STRINGIFY_HPP
#define YODA_STRINGIFY_HPP

#include <string>
#include <stack>
#include <vector>
#include <cstdio>
#include <cstdlib>

#include "calc_value.hpp"

struct  {
    enum {

    };

};

std::string top_ref_stringify (std::stack<CalcValue>&mainStack, std::vector<UserVar>& var_nodes, const bool showErrors, std::vector<void*>& freeable);


#endif //YODA_STRINGIFY_HPP
