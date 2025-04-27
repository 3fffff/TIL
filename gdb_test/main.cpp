#include "iostream"
#include "vector"

int main()
{
    std::vector<int> test = {1, 2, 3, 4, 5, 6, 7, 8};
    for (auto &x : test)
        std::cout << x << std::endl;
}