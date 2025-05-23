#include "ets_compiler.hpp"
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

ETSCompiler::ETSCompiler(const std::filesystem::path& testsPath, int threadCount)
    : testsPath(testsPath), threadCount(threadCount)
{
    for (const auto& test : std::filesystem::directory_iterator(testsPath))
        if (test.path().extension() == ".ets")
            tests.emplace_back(test.path());
}

ETSCompiler::~ETSCompiler()
{
}

void ETSCompiler::Run()
{
    auto task = getTask();

    if (threadCount > 1 && tests.size() >= threadCount)
    {
        std::cout << "Running all tests async!" << std::endl;

        int taskCount = tests.size() / threadCount;
        int remainingCount = tests.size() % taskCount;

        threads[0] = std::thread(task, 0, taskCount + remainingCount);

        for (size_t i = 1; i < threadCount; i++)
            threads[i] = std::thread(task, (i * taskCount), taskCount);
    }
    else 
    {
        std::cout << "Running all tests sync!" << std::endl;
        threads[0] = std::thread(task, 0, tests.size());
    }

    for (size_t i = 0; i < threadCount; i++)
        threads[i].join();

    std::cout << "In Total" << std::endl
              << "Task count: " << tests.size() << std::endl
              << "Success: " << etsFilesSuccess.size() << std::endl
              << "Fail: " << etsFilesFail.size() << std::endl;
}

std::function<void(size_t, int)> ETSCompiler::getTask()
{
    return [this](size_t sIndex, int taskCount) -> void {
        for (size_t i = sIndex; i < sIndex + taskCount; i++)
        {
            auto& test = tests[i];
            std::filesystem::path path = test;
            std::filesystem::path abcFile = path.replace_extension(".abc");
            std::string command 
                = "./build/bin/es2panda " + test.string() + " --output " + abcFile.string();

            int failed = std::system(command.c_str());

            {
                std::scoped_lock lock(mutex);
                if (!failed)
                {
                    etsFilesSuccess.emplace_back(test);
                    generatedAbcFiles.emplace_back(abcFile);
                }
                else
                {

                    etsFilesFail.emplace_back(test);
                }
            }
        }

        completedTasks++;
        std::cout << std::setprecision(2) << "Progress: %" << ((float)completedTasks / threadCount) * 100 << std::endl;
    };
}