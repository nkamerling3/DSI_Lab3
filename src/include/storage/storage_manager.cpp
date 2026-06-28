#include "storage/storage_manager.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

StorageManager::StorageManager(bool truncate_mode)
{
    int flags = O_RDWR | O_CREAT;
    if (truncate_mode)
    {
        flags |= O_TRUNC;
    }

    fd_ = ::open(database_filename.c_str(), flags, 0666);
    if (fd_ < 0)
    {
        std::cerr << "Error: Unable to open database file\n";
        exit(-1);
    }

    struct stat stat_buf;
    if (::fstat(fd_, &stat_buf) == 0)
    {
        num_pages = stat_buf.st_size / SlottedPage::PAGE_SIZE;
    }
    else
    {
        num_pages = 0;
    }

    if (num_pages == 0)
    {
        extend();
    }
}

StorageManager::~StorageManager()
{
    if (fd_ >= 0)
    {
        ::close(fd_);
    }
}

std::unique_ptr<SlottedPage> StorageManager::load(uint16_t page_id)
{
    std::cout << "storageManager load, page_id = " << page_id << " num_pages = " << num_pages << std::endl;
    assert(page_id < num_pages);

    auto page = std::make_unique<SlottedPage>();
    size_t offset = page_id * SlottedPage::PAGE_SIZE;

    ssize_t bytes_read = ::pread(fd_, page->page_data.get(), SlottedPage::PAGE_SIZE, offset);
    if (bytes_read != static_cast<ssize_t>(SlottedPage::PAGE_SIZE))
    {
        std::cerr << "Error: Unable to read page " << page_id << "\n";
        exit(-1);
    }
    return page;
}

void StorageManager::flush(uint16_t page_id, const SlottedPage &page)
{
    size_t offset = page_id * SlottedPage::PAGE_SIZE;

    ssize_t bytes_written = ::pwrite(fd_, page.page_data.get(), SlottedPage::PAGE_SIZE, offset);
    if (bytes_written != static_cast<ssize_t>(SlottedPage::PAGE_SIZE))
    {
        std::cerr << "Error: Unable to write page " << page_id << "\n";
        exit(-1);
    }
}

void StorageManager::extend()
{
    std::lock_guard<std::mutex> io_guard(io_mutex);
    auto empty_page = std::make_unique<SlottedPage>();
    size_t offset = num_pages * SlottedPage::PAGE_SIZE;

    ssize_t bytes_written = ::pwrite(fd_, empty_page->page_data.get(), SlottedPage::PAGE_SIZE, offset);
    if (bytes_written != static_cast<ssize_t>(SlottedPage::PAGE_SIZE))
    {
        std::cerr << "Error: Unable to extend file\n";
        exit(-1);
    }
    num_pages += 1;
}

void StorageManager::extend(uint64_t till_page_id)
{
    std::lock_guard<std::mutex> io_guard(io_mutex);
    if (till_page_id < num_pages)
        return;

    uint64_t pages_to_add = till_page_id + 1 - num_pages;
    uint64_t write_size = pages_to_add * SlottedPage::PAGE_SIZE;
    size_t offset = num_pages * SlottedPage::PAGE_SIZE;

    auto buffer = std::make_unique<char[]>(write_size);
    std::memset(buffer.get(), 0, write_size);

    ssize_t bytes_written = ::pwrite(fd_, buffer.get(), write_size, offset);
    if (bytes_written != static_cast<ssize_t>(write_size))
    {
        std::cerr << "Error: Unable to extend file up to " << till_page_id << "\n";
        exit(-1);
    }
    num_pages = till_page_id + 1;
}
