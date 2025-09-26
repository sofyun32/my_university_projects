#ifndef NUM3_H
#define NUM3_H

#include <iostream>
#include <stack>
#include <string>

int check(const std::string& input)
{
    std::stack<std::pair<char, int>> stack;

    for (int i = 0; i < input.length(); ++i)
    {
        char c = input[i];

        if (c == '(' || c == '[' || c == '{')
        {
            stack.push({c, i});
        }
        else if (c == ')' || c == ']' || c == '}')
        {
            if (stack.empty())
            {
                return i + 1;
            }

            char top = stack.top().first;

            if ((c == ')' && top != '(') ||
            (c == ']' && top != '[') || (c == '}' && top != '{'))
            {
                return i + 1;
            }

            stack.pop();
        }
    }

    if (!stack.empty())
    {
        return stack.top().second + 1;
    }

    return 0;
}

#endif