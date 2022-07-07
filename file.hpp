/*
 *  Module file represents basic filesystem file
 *  and memory mapped file
 */

#ifndef FILE_HPP
#define FILE_HPP

#include <string>
#include <vector>
#include <list>

#include <windows.h>

class file {
protected:

    std::string path;
    HANDLE hFile = INVALID_HANDLE_VALUE;
    uint64_t FileSize; 
    bool _readonly;

public:

    file(std::string fpath, bool readonly);

    virtual uint64_t read(void *dest, uint64_t f_offset, uint64_t size) const = 0;

    virtual uint64_t write(void *src, uint64_t f_offset, uint64_t size) = 0;

    virtual void resize(uint64_t new_size) = 0;

    std::string get_path();

    uint64_t size();

    virtual ~file();

};

#define MMFILE_SIZE_MAX 4294967296

/* memory mapped file object
 */
class mm_file : public file {
protected:
    LPVOID pView = NULL;	// file content in memory
    
    HANDLE hFileMapping;

public:

    virtual uint64_t read(void *dest, uint64_t f_offset, uint64_t count) const;

    virtual uint64_t write(void *src, uint64_t f_offset, uint64_t count);

    virtual void resize(uint64_t new_size);

    std::string read_cstr(int offset);

    mm_file(std::string fpath, bool readonly);

    ~mm_file();

};

#endif // FILE_HPP
