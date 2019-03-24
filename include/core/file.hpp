#pragma once

#ifndef FILE_HPP
#define FILE_HPP

#include <string>
#include <fstream>
#include <3ds.h>

/*
    File Class
    => Class for all file operations.
    This class is based on CTRPluginFramework::File from Nanquitas' CTRPluginFramework.
*/

class File {
public:  

    enum SeekPos
    {
        CUR, //Current Pos
        SET, //Beginning
        END //End
    };

    enum OpenMode
    {
        READ = 1,           ///< Gives read permission
        WRITE = 1 << 1,     ///< Gives write permission
        CREATE = 1 << 2,    ///< The file will be created if it doesn't exist
        APPEND = 1 << 3,    ///< You'll be unable to overwrite the file, only append data to it
        TRUNCATE = 1 << 4,  ///< Will clear the file
        SYNC = 1 << 5,      ///< Will flush on each write
        BINARY = 1 << 6,    ///< Operations are performed in binary mode rather than text.

        RW = READ | WRITE,
        RB = READ | BINARY,
        WB = WRITE | BINARY,
        RWB = READ | WRITE | BINARY,
        RWC = READ | WRITE | CREATE,
        RWCB = READ | WRITE | CREATE | BINARY
    };

    enum OPResult
    {
        SUCCESS = 0,             ///< Operation succeeded
        FAILED = -1,             ///< Generic Failed
        FILE_NOT_OPEN = -2,      ///< The File instance is not opened
        INVALID_MODE = -3,       ///< The mode passed when opened the file doesn't allow this operation
        INVALID_ARG = -4,        ///< One of the args passed to the operation is invalid (nullptr, etc)
        UNEXPECTED_ERROR = -5    ///< An error occured
    };

    /* 'static' methods are functions which only use the class as a namespace, and do not require an instance. */
    static int Create(std::string filePath);
    static int Rename(std::string OldFilePath, std::string NewFilePath);
    static int Remove(std::string filePath);
    static bool Exists(std::string filePath);

    static int Open(File &output, const std::string &filePath, int mode = READ | WRITE | SYNC);


    File(void);
    File(const std::string &filePath, int mode = READ | WRITE | SYNC);
    ~File(void);

    //Returns OPResult or an error code from FS service
    int Close(void);

    int Read(void *buffer, u32 size);                   //Read data into buffer, with length being size to read in bytes
    int Write(const void *data, u32 size);              //Write data into buffer, with length being size to write in bytes
    int Flush(void) const;                              //Flushes a file's content
    u64 GetSize(void) const;                            //Get the size of the file
    bool IsOpen(void) const;                            //Check if the file is open or not (true if open)

    int Seek(s64 offset, SeekPos origin = SET);         //Set the position in the file
    u64 Tell(void) const;                               //Get current position in the file
    void Rewind(void);                                  //Set the position to the beginning of the file

    
    template <typename T>
    int Read(T &buffer) {
        if (!m_FileIsOpen) return OPResult::FILE_NOT_OPEN;
        if (!(m_Mode&File::READ)) return OPResult::INVALID_MODE;

        T buf;
        m_File->read((char *)&buf, sizeof(buf));
        if (m_File->good()) {
            buffer = buf;
            return OPResult::SUCCESS;
        }
        return OPResult::FAILED;
    }

    template <typename T>
    int Write(T data) {
        if (!m_FileIsOpen) return OPResult::FILE_NOT_OPEN;
        if (!(m_Mode&File::WRITE)) return OPResult::INVALID_MODE;

        (*m_File) << data;
        m_File->write((char *)&data, sizeof(data));
        if (m_File->good()) {
            if (m_Mode&File::SYNC) m_File->flush();
            return OPResult::SUCCESS;
        }
        return OPResult::FAILED;
    }
    

private:
    static void PathFixups(std::string &filePath);

    std::fstream *m_File;
    std::string m_Path;
    int    m_Mode;
    bool   m_FileIsOpen;
};

#endif