/*
 * Basic/SourceManager.cpp
 */

#include <fcntl.h>

#include <sys/mman.h>
#include <sys/stat.h>

#include "Basic/SourceManager.h"

auto SourceManager::fromFile(const std::string_view Path) noexcept ->
    ErrorOr<SourceManager *, Error>
{
    const auto Fd = open(Path.data(), O_RDONLY);
    if (Fd == -1) {
        return Error::createOpenError(strerror(errno));
    }

    struct stat Stat;
    if (fstat(Fd, &Stat) == -1) {
        return Error::createStatError(strerror(errno));
    }

    const auto Map = mmap(NULL, Stat.st_size, PROT_READ, MAP_PRIVATE, Fd, 0);
    if (Map == MAP_FAILED) {
        return Error::createMapError(strerror(errno));
    }

    return new SourceManager(Map, Stat.st_size, /*IsMapped=*/true);
}

SourceManager::~SourceManager() noexcept {
    if (IsMapped) {
        munmap(Base, Size);
    }
}