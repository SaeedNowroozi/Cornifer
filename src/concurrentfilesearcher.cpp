#include "concurrentfilesearcher.h"
#include <atomic>
#include "logger.h"
#include "customerrors.h"

ConcurrentFileSearcher::ConcurrentFileSearcher(const std::vector<std::string>& patterns,
                                               bool case_insensitive)
    : stop_requested_(false)
{
    if (patterns.empty()) {
        throw FileSearchError("Pattern list cannot be empty");
    }
    matcher_ = std::make_unique<FileMatcher>(patterns, case_insensitive);

}

ConcurrentFileSearcher::~ConcurrentFileSearcher()
{
    stop();
}

std::vector<fs::path> ConcurrentFileSearcher::search(const fs::path& root_dir)
{
    if (!fs::exists(root_dir)) {
        throw FileNotFoundError(root_dir.filename().string(), root_dir.parent_path());
    }
    if (!fs::is_directory(root_dir)) {
        throw DirectoryAccessError(root_dir, "Path is not a directory");
    }
    {
        std::lock_guard<std::mutex> lock(results_mutex_);
        found_files_.clear();
    }
    stop_requested_ = false;

    enqueue_path(root_dir);

    wait_for_all_tasks();

    std::lock_guard<std::mutex> lock(results_mutex_);
    if (found_files_.empty()) {
        Logger::warn("No files found matching patterns.");
    }
    return found_files_;
}

void ConcurrentFileSearcher::stop()
{
    stop_requested_ = true;
    wait_for_all_tasks();
}

void ConcurrentFileSearcher::enqueue_path(const fs::path& path)
{
    if (stop_requested_)
        return;

    futures_.push_back(pool_.submit_task([this, path]() {
        worker_task(path);
    }));
}

void ConcurrentFileSearcher::worker_task(fs::path path)
{
    try {
        Logger::debug("Starting to search in: " + path.string());

        for (const auto& entry : fs::recursive_directory_iterator(path, fs::directory_options::skip_permission_denied)) {
            if (stop_requested_) return;
            if (entry.is_symlink()) continue;

            if (entry.is_regular_file()) {
                auto filename = entry.path().filename().string();
                if (matcher_->match(filename)) {
                    std::lock_guard<std::mutex> lock(results_mutex_);
                    found_files_.push_back(entry.path());
                    Logger::info("Found file: " + entry.path().string());
                }
            }
        }
    }
    catch (const fs::filesystem_error& e) {
        Logger::error("Filesystem error: " + std::string(e.what()) + " at " + path.string());
        throw DirectoryAccessError(path, "Directory access failed: " + std::string(e.what()));
    }
    catch (const std::exception& e) {
        Logger::error("Exception: " + std::string(e.what()));
        throw FileSearchError("Search error in " + path.string() +
                              ": " + e.what(), path);
    }
}

void ConcurrentFileSearcher::wait_for_all_tasks()
{
    for (auto& fut : futures_) {
        if (fut.valid()) {
            try {
                fut.get();
            }
            catch (const FileSearchError& e) {
                Logger::error("Search task failed: " + std::string(e.what()));
                throw;
            }
            catch (const std::exception& e) {
                Logger::error("Exception in thread task: " + std::string(e.what()));
                throw FileSearchError("Task error", {});
            }
        }
    }
    futures_.clear();
}
