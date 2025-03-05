/*
 * Source/SourceBuffer.cpp
 * © suhas pai
 */

#include <cstdlib>
#include <cstring>

#include <errno.h>
#include <fcntl.h>

#include <sys/mman.h>
#include <sys/stat.h>

#include "Source/SourceBuffer.h"

namespace ADT {
    auto SourceBuffer::fromFile(const std::string_view Path) noexcept
        -> std::expected<SourceBuffer *, Error>
    {
        const auto Fd = open(Path.data(), O_RDONLY);
        if (Fd == -1) {
            return std::unexpected<Error>({
                Error::Kind::FailedToOpenFile,
                strerror(errno)
            });
        }

        struct stat Stat;
        if (fstat(Fd, &Stat) == -1) {
            return std::unexpected<Error>({
                Error::Kind::FailedToStatFile,
                strerror(errno)
            });
        }

        const auto Map =
            mmap(nullptr, Stat.st_size, PROT_READ, MAP_PRIVATE, Fd, 0);

        if (Map == MAP_FAILED) {
            return std::unexpected<Error>({
                Error::Kind::FailedToMapFile,
                strerror(errno)
            });
        }

        return new SourceBuffer(Map, Stat.st_size, DestroyMapKind::Mapped);
    }

    auto SourceBuffer::fromAlloc(void *const Base, const size_t Size) noexcept
        -> SourceBuffer *
    {
        return new SourceBuffer(Base, Size, DestroyMapKind::Allocated);
    }

    SourceBuffer::~SourceBuffer() noexcept {
        switch (DestroyKind) {
            case DestroyMapKind::Normal:
                break;
            case DestroyMapKind::Mapped:
                munmap(Base, Size);
                break;
            case DestroyMapKind::Allocated:
                free(Base);
                break;
        }
    }
}