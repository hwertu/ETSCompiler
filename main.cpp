#include "ets_compiler.hpp"
#include <iostream>
#include <string>

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::cout << "Usage: " << argv[0] << " <tests-folder> <threads:optional>" << std::endl;
        return EXIT_FAILURE;
    }

    if (argc == 2)
    {
        ETSCompiler compiler(argv[1]);
        compiler.Run();
    }
    else
    {
        try {
            int threadCount = std::stoi(argv[2]);
            if (threadCount >= 1 && threadCount <= 4)
            {
                ETSCompiler compiler(argv[1], threadCount);
                compiler.Run();       
            }
            else
                std::cout << "Thread count range is [1, 4]" << std::endl;
        }
        catch (const std::invalid_argument&) {
            std::cout << "Thread count range is [1, 4]" << std::endl;
        }
    }
}