#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <cstdio>
#include <sys/stat.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <ftw.h>
#include <iostream>
#include <algorithm>
#include <sstream> 
#include "filesOperations.h"

using namespace std;

#define DEBUG

int unlink_cb(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf)
{
    int rv = remove(fpath);

    if (rv)
    {
        perror(fpath);
    }
    return rv;
}
//----------------------------------------------------------------------------------------
int filesOperations::removePath(const char *path)
{
    return nftw(path, unlink_cb, 64, FTW_DEPTH | FTW_PHYS);
}
//----------------------------------------------------------------------------------------
int filesOperations::renamePath(const char *oldPath, const char *newPath)
{
    return rename(oldPath, newPath);
}
//----------------------------------------------------------------------------------------
void filesOperations::statsPath(const char* path, char *bufOut)
{
    const int bufSize = 512;    
    memset(bufOut, '\0', bufSize);
    
    struct stat sb;
    
    if (stat(path, &sb) != -1)
    {   
        std::stringstream ss;    
        ss << "File type: ";

        switch (sb.st_mode & S_IFMT) 
        {
            case S_IFBLK:  ss << "block device\n";            break;
            case S_IFCHR:  ss << "character device\n";        break;
            case S_IFDIR:  ss << "directory\n";               break;
            case S_IFIFO:  ss << "FIFO/pipe\n";               break;
            case S_IFLNK:  ss << "symlink\n";                 break;
            case S_IFREG:  ss << "regular file\n";            break;
            case S_IFSOCK: ss << "socket\n";                  break;
            default:       ss << "unknown?\n";                break;
        }

        ss << "I-node number:" << sb.st_ino << "\n";

        ss << "Mode (permission):" << sb.st_mode << "(octal)\n";
        ss << "Mode as text: \t";
        char isdir =  S_ISDIR(sb.st_mode) ? 'd' : '-';
        char irusr =  sb.st_mode & S_IRUSR ? 'r' : '-';
        char iwusr =  sb.st_mode & S_IWUSR ? 'w' : '-';
        char ixusr = sb.st_mode & S_IXUSR ? 'x' : '-';
        char irgrp =  sb.st_mode & S_IRGRP ? 'r' : '-';
        char iwgrp =  sb.st_mode & S_IWGRP ? 'w' : '-';
        char ixgrp =  sb.st_mode & S_IXGRP ? 'x' : '-';
        char iroth = sb.st_mode & S_IROTH ? 'r' : '-';
        char iwoth =  sb.st_mode & S_IWOTH ? 'w' : '-';
        char ixoth =  sb.st_mode & S_IXOTH ? 'x' : '-';
        ss << isdir << irusr << iwusr << ixusr << ", " << irgrp << iwgrp << ixgrp << ", " << iroth << iwoth << ixoth << "\n";
        ss << "Link count:" << sb.st_nlink << "\n";
        ss << "Ownership: UID=" << sb.st_uid << ", GID=" << sb.st_gid << "\n";
        ss << "Preferred I/O block size: " <<  sb.st_blksize << " bytes\n";
        ss << "File size: " << sb.st_size << "bytes\n";
        ss << "Blocks allocated: " << sb.st_blocks << "\n";
        ss << "Last status change:" << ctime(&sb.st_ctime);
        ss << "Last file access:" << ctime(&sb.st_atime);
        ss << "Last file modification: " << ctime(&sb.st_mtime);

        // get access rights on path
        int rval;

        // Check file existence. 
        ss << "*** Access info *** \n";        
        rval = access (path, F_OK);
        if (rval == 0) 
        {
            ss <<  "exists\n";
        }
        else 
        {
            if (errno == ENOENT) 
            {
                ss << "does not exist\n";
            }
            else if (errno == EACCES) 
            {
                ss << "is not accessible\n";
            }
        }
        
        // Check read access.         
        rval = access (path, R_OK);
        if (rval == 0)
        {
            ss << "is readable\n";
        }
        else
        {
            ss << "is not readable (access denied)\n";
        }

        // Check write access. 
        rval = access (path, W_OK);
        if (rval == 0)
        {
            ss << "is writable\n" ;
        }
        else if (errno == EACCES)
        {
            ss <<  "is not writable (access denied)\n";
        }
        else if (errno == EROFS)
        {
            ss << "is not writable (read-only filesystem)\n";
        }
        ss << "*** Access info *** \n";

        std::string ssStdString = ss.str();
        int length = strlen(ssStdString.c_str());
        if(length > bufSize - 1)
        {
            length = bufSize - 1;
        }
        strncpy(bufOut, ssStdString.c_str(), length);
    }
    else
    {
        strcmp(bufOut, "failed to call stat for supplied path");
    }
}
//----------------------------------------------------------------------------------------
bool sortString(string a,string b) 
{    
    return (a < b);
}
//----------------------------------------------------------------------------------------
int filesOperations::isDirectory(const char *path) 
{
   struct stat statbuf;
   if (stat(path, &statbuf) != 0)
       return 0;
   return S_ISDIR(statbuf.st_mode);
}
//----------------------------------------------------------------------------------------
int filesOperations::isFile(const char *path) 
{
   struct stat statbuf;
   if (stat(path, &statbuf) != 0)
   {
       return 0;
   }
   return S_ISREG(statbuf.st_mode);
}            
//----------------------------------------------------------------------------------------
int filesOperations::getDirFiles(const char* dir, vector<string> &filesAndFolders, bool sort)        
{
    DIR *dp;
    struct dirent *dirp;                
    errno = 0;
    dp  = opendir(dir);
    if(!dp) 
    {
        return errno;
    }

    while ((dirp = readdir(dp)) != NULL) 
    {                
        filesAndFolders.push_back(string(dirp->d_name));
    }                      
    closedir(dp);

    if(sort)
    {
        std::sort (filesAndFolders.begin(), filesAndFolders.end(), sortString);
    }

    return 0;
}
//----------------------------------------------------------------------------------------
void filesOperations::getFileContents(const char* srcFilePath, std::string &dbgBuffer)
{
    // First, make sure we are dealing with a regular file    
    if(!filesOperations::isFile(srcFilePath))
    {
        return;
    }

    const int BUFSIZE = 1024;
    char buf[BUFSIZE];
    memset(buf, '\0', BUFSIZE);
    size_t size;
    
    FILE* srcFile = fopen(srcFilePath, "rb");
    
    if(srcFile)
    {
        while(size = fread(buf, 1, BUFSIZE, srcFile))
        {
            dbgBuffer.append(buf, size);
            memset(buf, '\0', BUFSIZE);
        }
    }        
}
//----------------------------------------------------------------------------------------
bool filesOperations::copyFile(const char* srcFilePath, const char* destFilePath)
{
    // First, make sure we are dealing with a regular file    
    if(!filesOperations::isFile(srcFilePath))
    {
        return false;
    }
    
    const int BUFSIZE = 1024;
    
    char buf[BUFSIZE];
    size_t size;
    
    FILE* srcFile = fopen(srcFilePath, "rb");
    FILE* destFile = fopen(destFilePath, "wb");           
    
    if(srcFile && destFile)
    {
        while(size = fread(buf, 1, BUFSIZE, srcFile))
        {
            fwrite(buf, 1, size, destFile);
        }        
    }
    else
    {
        if(srcFile)
        {
            fclose(srcFile);
        }

        if(destFile)
        {
            fclose(destFile);
        }
        return false;
    }
        
    if(srcFile)
    {
        fclose(srcFile);
    }
    
    if(destFile)
    {
        fclose(destFile);
    }
    
    return true;
}
//----------------------------------------------------------------------------------------
bool filesOperations::createDir(const char* path)
{
    const int dir_err = mkdir(path, S_IRWXU|S_IRWXG|S_IROTH|S_IXOTH);
    
    if(dir_err != 0)
    {
        return false;
    }
    return true;
} 
//----------------------------------------------------------------------------------------
bool filesOperations::copyFiles(const char* srcFilePath, std::vector<std::string> fileNames,  const char* destFilePath)
{
    for(unsigned int i  = 0; i < fileNames.size(); i++)
    {
        std::string fileName = fileNames.at(i);
        if(fileName.compare(".") != 0 && fileName.compare("..") != 0)
        {       
            std::string pathSrcFile = std::string(srcFilePath) + fileName;
            std::string pathDestFile = std::string(destFilePath) + fileName;
            
            bool cpyFileStatus = filesOperations::copyFile(pathSrcFile.c_str(), pathDestFile.c_str());
            if(!cpyFileStatus)
            {
                return false;
            }
        }
    }
    
    return true;
}    
//----------------------------------------------------------------------------------------
void copyStringStreamToCharBuf(std::string theStreamAsString, std::string &dbgBuffer)
{
    dbgBuffer.append(theStreamAsString);
}
//----------------------------------------------------------------------------------------
bool filesOperations::copyDirToOtherDirPerform(const char* srcPath, const char* destPath, std::string &dbgBuffer, bool performCopy, std::vector<std::string> &filesOverwritten)
{
    std::stringstream dbgStrStream;
    vector<string> filesAndFolders;
    int status = filesOperations::getDirFiles(srcPath, filesAndFolders, true);   
    for(unsigned int i  = 0; i < filesAndFolders.size(); i++)
    {
        std::string fileName = filesAndFolders.at(i);
        if(fileName.compare(".") != 0 && fileName.compare("..") != 0)
        {
            std::string thePath = std::string(srcPath);
            std::string newDestPath = std::string(destPath);
           
            if(thePath.back() != '/')
            {
                thePath.append("/");
            }
            if(newDestPath.back() != '/')
            {
                newDestPath.append("/");
            }
            thePath.append(fileName);
            newDestPath.append(fileName);
            
            
            #ifdef DEBUG            
            dbgStrStream << "thePath: " << thePath << "\n";
            dbgStrStream << "newDestPath: " << newDestPath << "\n";
            #endif
            
            if(filesOperations::isDirectory(thePath.c_str()))
            {
                #ifdef DEBUG
                dbgStrStream << "dir found, creating it\n";               
                #endif
                
                if(performCopy)
                {
                    if(!filesOperations::createDir(newDestPath.c_str()) && !filesOperations::isDirectory(newDestPath.c_str()))
                    {
                        #ifdef DEBUG
                        dbgStrStream << "failed to create/open dir: " << newDestPath << "\n";
                        #endif

                        copyStringStreamToCharBuf(dbgStrStream.str(), dbgBuffer);
                        return false;
                    }
                }
                
                std::string innerPath = thePath;
                std::string innerDestPath = newDestPath;
                if(innerPath.back() != '/')
                {
                    innerPath.append("/");
                }
                if(innerDestPath.back() != '/')
                {
                    innerDestPath.append("/");
                }
                bool copySubPath = filesOperations::copyDirToOtherDirPerform(innerPath.c_str(), innerDestPath.c_str(), dbgBuffer, performCopy, filesOverwritten);
                if(!copySubPath)
                {
                    copyStringStreamToCharBuf(dbgStrStream.str(), dbgBuffer);                    
                    return false;
                }
            }
            else if(filesOperations::isFile(thePath.c_str()))
            {
                #ifdef DEBUG
                dbgStrStream << "copying file: " << fileName << "\n";
                #endif
                
                // If the file 'newDestPath' already exists, store it in filesOverwritten
                if(filesOperations::isFile(newDestPath.c_str()))
                {
                    filesOverwritten.push_back(newDestPath);
                }                
                
                if(performCopy)
                {
                    bool cpyFileStatus = filesOperations::copyFile(thePath.c_str(), newDestPath.c_str());
                    if(!cpyFileStatus)
                    {
                        #ifdef DEBUG
                        dbgStrStream << "failed to copy: " << fileName << "\n";
                        #endif

                        copyStringStreamToCharBuf(dbgStrStream.str(), dbgBuffer);
                        return false;
                    }
                }                
                
            }
            else
            {
                #ifdef DEBUG                
                dbgStrStream << "path: " << thePath << " is not a directory and not a file, skipping \n";
                #endif
            }                
                
        }
    }
    copyStringStreamToCharBuf(dbgStrStream.str(), dbgBuffer);
    return true;    
}
//----------------------------------------------------------------------------------------
bool filesOperations::copyDirToOtherDir(const char* srcPath, const char* destPath, std::string &dbgBuffer)
{
    std::vector<std::string> overWrittenFiles;
    return copyDirToOtherDirPerform(srcPath, destPath, dbgBuffer, true, overWrittenFiles);
}
//----------------------------------------------------------------------------------------
void filesOperations::findOverwriteFilesOnCopyDirToOtherDir(const char* srcPath, const char* destPath, std::vector<std::string> &overWrittenFiles)
{
    std::string dbgBuffer;
    copyDirToOtherDirPerform(srcPath, destPath, dbgBuffer, false, overWrittenFiles);
}
//----------------------------------------------------------------------------------------
void filesOperations::countDepth(const char* path, unsigned int &count)
{
    const unsigned int maxDepth = 10000;
    if(count < maxDepth)
    {
        vector<string> filesAndFolders;
        int status = filesOperations::getDirFiles(path, filesAndFolders, true);
        count += filesAndFolders.size() - 2; // Dont count "." and ".."
        for(unsigned int i  = 0; i < filesAndFolders.size(); i++)
        {
            std::string fileName = filesAndFolders.at(i);
            if(fileName.compare(".") != 0 && fileName.compare("..") != 0)
            {
                std::string thePath = std::string(path) + std::string("/") + fileName;
                if(filesOperations::isDirectory(thePath.c_str()))
                {               
                    std::string innerPath = thePath + std::string("/");
                    filesOperations::countDepth(innerPath.c_str(), count);
                }
            }
        }
    }
    
}

// Test cases - dont remove me, can be used to test functionallity of this file
/*
int main()
{
    
    vector<string> files;
    int status = filesOperations::getDirFiles("/home/jimmie/Documents", files, true);

    if(status == 0)
    {
        for (unsigned int i = 0;i < files.size();i++) 
        {
            cout << files[i] << endl;
        }
    }
    else
    {
        cout << "Error status: " << status << endl;
    }
    
    
    //bool statusCopy = filesOperations::copyFile("/home/jimmie/Documents/tmp/a/blah.mp4", "/home/jimmie/Documents/tmp/b/blah.mp4");
    //cout << "statusCopy:" << statusCopy << endl;
    
    //bool dirCreateStatus = filesOperations::createDir("/home/jimmie/Documents/tmp/c");
    //cout << "dirCreateStatus:" << dirCreateStatus << endl;
    

    vector<string> folderFiles;
    int getDirStatus = filesOperations::getDirFiles("/home/jimmie/Documents/tmp/a", folderFiles, true);
    if(status == 0)
    {
        bool copyFilesStatus = filesOperations::copyFiles("/home/jimmie/Documents/tmp/a/", folderFiles, "/home/jimmie/Documents/tmp/b/");
        
        cout << "copyFilesStatus:" << copyFilesStatus << endl;
    }
    
    
    bool folderCopyTest = filesOperations::copyDirToOtherDir("/home/jimmie/Documents/tmp/a/", "/home/jimmie/Documents/tmp/b/");
    cout << "folderCopyTest:" << folderCopyTest << endl;    
    
    return 0;
}
*/