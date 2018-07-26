#ifndef YODA_TOP_MODIFIERS_HPP
#define YODA_TOP_MODIFIERS_HPP

#include <stack>
#include "process_line.hpp"

// recursively evaluate the top value
CalcValue* conv_top(std::stack<CalcValue>& mainStack, std::vector<UserVar>& var_nodes, const bool showErrors, std::vector<void*>& freeable);

CalcValue* get_top(std::stack<CalcValue>& mainStack, std::vector<UserVar>& var_nodes, bool showErrors, std::vector<void*>& freeable);

CalcValue* conv_top_keep_refs(std::stack<CalcValue>& mainStack, std::vector<UserVar>& var_nodes, const bool showErrors, std::vector<void*>& freeable);

#endif //YODA_TOP_MODIFIERS_HPP
