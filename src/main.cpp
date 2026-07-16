#include <iostream>
#include <vector>
#include "reflection.h"

int main(void)
{
    reflection::Reflection::parse("./data/Player.h", nullptr);
    return 0;
}
