#include <vector>
#include <string>

class filesOperations
{
    public:    
    static int isDirectory(const char *path);
    static int isFile(const char *path);

    static void statsPath(const char* path, char *bufOut); // Second parameter must be a 512 long char array that you allocated yourself!
    
    static void getFileContents(const char* srcFilePath, std::string &dbgBuffer);
    static int getDirFiles(const char* dir, std::vector<std::string> &filesAndFolders, bool sort);
    static bool copyFile(const char* srcFilePath, const char* destFilePath);
    static bool createDir(const char* path);
    static bool copyFiles(const char* srcFilePath, std::vector<std::string> fileNames,  const char* destFilePath);
    
    // Recursively copies from srcPath to destPath. destPath must already exist. dbgBuffer is a std::string that you need to provide where debuginfo can be obtained    
    static bool copyDirToOtherDir(const char* srcPath, const char* destPath, std::string &dbgBuffer);     
    static void findOverwriteFilesOnCopyDirToOtherDir(const char* srcPath, const char* destPath, std::vector<std::string> &overWrittenFiles); 
        
    static void countDepth(const char* path, unsigned int &count);
    
    static int removePath(const char *path);
    static int renamePath(const char *oldPath, const char *newPath);    
    
    private:
    
    static bool copyDirToOtherDirPerform(const char* srcPath, const char* destPath, std::string &dbgBuffer, bool performCopy, std::vector<std::string> &filesOverwritten);
};
