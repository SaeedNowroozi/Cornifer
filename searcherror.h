#ifndef SEARCHERROR_H
#define SEARCHERROR_H

#include <stdexcept>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

class FileSearchError : public std::runtime_error {
public:
    explicit FileSearchError(const std::string& message, const fs::path& path = {})
        : std::runtime_error(message), path_(path) {}

    const fs::path& getPath() const { return path_; }

private:
    fs::path path_;
};

class FileNotFoundError : public FileSearchError {
public:
    explicit FileNotFoundError(const std::string& filename, const fs::path& searchRoot = {})
        : FileSearchError("File '" + filename + "' not found in " + searchRoot.string(), searchRoot),
          filename_(filename) {}

    const std::string& getFilename() const { return filename_; }

private:
    std::string filename_;
};

class DirectoryAccessError : public FileSearchError {
public:
    explicit DirectoryAccessError(const fs::path& path, const std::string& details = "")
        : FileSearchError("Cannot access directory: " + path.string() +
                         (details.empty() ? "" : " (" + details + ")"), path) {}
};

#endif // SEARCHERROR_H
