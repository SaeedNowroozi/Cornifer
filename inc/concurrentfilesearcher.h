#ifndef CONCURRENTFILESEARCHER_H
#define CONCURRENTFILESEARCHER_H

#include <filesystem>
#include <string>
#include <vector>
#include <future>
#include <atomic>
#include <mutex>
#include <memory>

#include "filematcher.h"

#include "customerrors.h"
#include "BS_thread_pool.hpp"

namespace fs = std::filesystem;

class ConcurrentFileSearcher {
public:
    ConcurrentFileSearcher(const std::vector<std::string>& patterns,
                           bool case_insensitive = false);

    ~ConcurrentFileSearcher();

    std::vector<fs::path> search(const fs::path& root_dir);

    void stop();

private:
    void enqueue_path(const fs::path& path);
    void worker_task(fs::path path);

    void wait_for_all_tasks();

    std::unique_ptr<FileMatcher> matcher_;
    BS::thread_pool<> pool_;
    std::vector<std::future<void>> futures_;

    std::vector<fs::path> found_files_;
    std::mutex results_mutex_;
    std::atomic<bool> stop_requested_;
};

#endif // CONCURRENTFILESEARCHER_H

