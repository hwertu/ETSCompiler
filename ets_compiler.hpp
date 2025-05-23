#ifndef ETS_COMPILER_HPP
#define ETS_COMPILER_HPP

#include <atomic>
#include <functional>
#include <vector>
#include <filesystem>
#include <thread>
#include <mutex>

// Runner for es2panda && ark
class ETSCompiler
{
public:
    ETSCompiler(const std::filesystem::path& testsPath, int threadCount = 1);
    ~ETSCompiler();
    void Run();
private:
    std::function<void(size_t, int)> getTask();
private:
    std::filesystem::path testsPath; 
    std::vector<std::filesystem::path> tests;
    std::vector<std::filesystem::path> etsFilesSuccess;
    std::vector<std::filesystem::path> etsFilesFail;
    std::vector<std::filesystem::path> generatedAbcFiles;
    std::array<std::thread, 4> threads;
    std::mutex mutex;
    std::atomic_int32_t completedTasks = 0;
    int threadCount;
};

#endif