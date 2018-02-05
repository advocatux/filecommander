#include <QDebug>

#include <algorithm>
#include "filecommanderplugin.h"
#include "filesOperations.h"

#define DEBUG
// #define DEBUGSHOWSTATS
#define MAX_OVERWRITE_SIZE 40000


FileCommanderGlobalData::FileCommanderGlobalData()
{
    m_warningNavigatingFromHomeShown = false;
}
//-------------------------------------------------------------------------------------------------
bool FileCommanderGlobalData::warningNavigatingFromHomeShown()
{
    return m_warningNavigatingFromHomeShown;
}
//-------------------------------------------------------------------------------------------------
void FileCommanderGlobalData::setWarningNavigatingFromHomeShown()
{
    m_warningNavigatingFromHomeShown = true;
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------

OverWriteListView::OverWriteListView()
{
}
//-------------------------------------------------------------------------------------------------
void OverWriteListView::setOverwriteList(QStringList data)
{
    m_overwriteList = data;
}
//-------------------------------------------------------------------------------------------------
QVariant OverWriteListView::getOverWriteListView()
{
    return QVariant::fromValue(m_overwriteList);
}


//-------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------


FileCommanderListAndFilesModel::FileCommanderListAndFilesModel(QObject *parent) : QAbstractListModel(parent)
{
    #ifdef DEBUG
    qDebug() << "FileCommanderListAndFilesModel constructor";
    #endif
    
    m_selectedDir = "/home/phablet";
    m_folderToCreate = "";
    m_editGuard = "";
    m_homePhabletShowOnlySafe = true;
    filesAndFoldersFromFolder(m_selectedDir);
}
//-------------------------------------------------------------------------------------------------
int FileCommanderListAndFilesModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return this->m_filesAndFoldersNames.size();
}
//-------------------------------------------------------------------------------------------------
QHash<int, QByteArray> FileCommanderListAndFilesModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[theTextRole] = "fileOrFolderRole";
    roles[checkedRole] = "checkedRole";
    
    #ifdef DEBUG
    qDebug()<< roles;
    #endif
    
    return roles;
}
//-------------------------------------------------------------------------------------------------
QVariant FileCommanderListAndFilesModel::data(const QModelIndex &index, int role) const
{
    if(m_filesAndFoldersNames.size() != 0)
    {
        if (role == theTextRole)
        {
            return m_filesAndFoldersNames[index.row()];
        }
        if (role == checkedRole)
        {
            return m_checkedStatus[index.row()];
        }

        return QVariant();
    }
    return QVariant();

}
//-------------------------------------------------------------------------------------------------
void  FileCommanderListAndFilesModel::setChecked(QVariant index, bool checked)
{
    if(m_checkedStatus.size() != 0)
    {
        m_checkedStatus[index.toInt()] = checked;        
    }    
}
//-------------------------------------------------------------------------------------------------
void  FileCommanderListAndFilesModel::clearAllChecked()
{
    filesAndFoldersFromFolder(m_selectedDir);
}
//-------------------------------------------------------------------------------------------------
QString FileCommanderListAndFilesModel::getImgSource(QVariant index)
{
    if(m_filesAndFoldersNames.size() != 0)
    {
        std::string path = m_selectedDir.toStdString();
        QVariant fileOrFolder = m_filesAndFoldersNames[index.toInt()];
        
        if(path.back() != '/')
        {
            path += std::string("/");
        }
        path += fileOrFolder.toString().toStdString();
        
        if(filesOperations::isDirectory(path.c_str()))
        {
            return QString("../assets/folder.png");
        }
    }
    return QString("../assets/file.png");
}   
//-------------------------------------------------------------------------------------------------
bool FileCommanderListAndFilesModel::isBigCopy()
{
    const unsigned int bigCopyLimit = 1000;
    
    for(unsigned int i = 0; i < m_filesAndFoldersNames.size(); i++)
    {        
        if(m_checkedStatus[i])
        {
            QVariant fileOrFolder = m_filesAndFoldersNames[i];
            
            if(fileOrFolder.toString().toStdString().compare("..") != 0)
            {
                std::string path = m_selectedDir.toStdString();                
                if(path.back() != '/')
                {
                    path += std::string("/");
                }
                path += fileOrFolder.toString().toStdString();
                                
                if(filesOperations::isDirectory(path.c_str()))
                {
                    unsigned int depth = 0;
                    filesOperations::countDepth(path.c_str(), depth);
                    if(depth > bigCopyLimit)
                    {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}
//-------------------------------------------------------------------------------------------------
void  FileCommanderListAndFilesModel::copyItemToOtherSidePerform(QVariant destination, bool performCopy, QStringList &filesOverwritten)
{
    #ifdef DEBUG
    qDebug() << "copyItemToOtherSide" ;
    #endif
    
    for(unsigned int i = 0; i < m_filesAndFoldersNames.size(); i++)
    {        
        if(m_checkedStatus[i])
        {
            #ifdef DEBUG
            qDebug() << "Copying" << i ;
            #endif
            
            QVariant fileOrFolder = m_filesAndFoldersNames[i];
            
            if(fileOrFolder.toString().toStdString().compare("..") != 0)
            {
                std::string path = m_selectedDir.toStdString();                
                if(path.back() != '/')
                {
                    path += std::string("/");
                }
                path += fileOrFolder.toString().toStdString();
                
                #ifdef DEBUG
                qDebug() << "Copying" << path.c_str() ;
                #endif
                
                if(filesOperations::isDirectory(path.c_str()))
                {
                    std::string destinationStr = destination.toString().toStdString();
                    destinationStr += std::string("/") + fileOrFolder.toString().toStdString();
                    
                    #ifdef DEBUG
                    qDebug() << "is dir";                    
                    qDebug() << "creating destination folder: " << destinationStr.c_str();
                    #endif
                    
                    bool statusCreateDestDir = true;
                    
                    if(performCopy)
                    {
                        statusCreateDestDir = filesOperations::createDir(destinationStr.c_str()) || filesOperations::isDirectory(destinationStr.c_str());

                        #ifdef DEBUG
                        qDebug() << "createdir status: " << statusCreateDestDir;
                        #endif                        
                    }
                    
                    if(statusCreateDestDir)
                    {
                        #ifdef DEBUG
                        qDebug() << "Copying" << path.c_str() << " to " << destinationStr.c_str();                                     
                        #endif
                        std::string dbgBuffer;
                        
                        if(performCopy)
                        {
                            filesOperations::copyDirToOtherDir(path.c_str(), destinationStr.c_str(), dbgBuffer);
                        }
                        else
                        {
                            std::vector<std::string> overWrittenFiles;
                            filesOperations::findOverwriteFilesOnCopyDirToOtherDir(path.c_str(), destinationStr.c_str(), overWrittenFiles);
                            
                            for(std::vector<std::string>::iterator itFiles = overWrittenFiles.begin(); itFiles != overWrittenFiles.end(); itFiles++)
                            {
                                // Avoid trashing the filesOverwritten if user has selected a path with enormous amount of files in it
                                if(filesOverwritten.size() < MAX_OVERWRITE_SIZE)
                                {
                                    std::string overwrittenFile = (*itFiles);
                                    if(overwrittenFile.length() > 30) // This will be to long to show in the GUI, chop of the begining - show maximum 30 letters here.
                                    {
                                        overwrittenFile = "...";
                                        int startOfFile = (*itFiles).length() - 30 - 3; // the 3 is for the ... we are starting of with
                                        overwrittenFile.append((*itFiles).substr(startOfFile));
                                    }
                                    filesOverwritten << overwrittenFile.c_str();

                                    #ifdef DEBUG
                                    qDebug() << "file that would be overwritten: " << (*itFiles).c_str();
                                    #endif                            
                                }
                            }                            
                        }
                        
                        #ifdef DEBUG
                        qDebug() << "copyDirToOtherDir debug info: " << dbgBuffer.c_str();
                        #endif
                    }
                }
                else if(filesOperations::isFile(path.c_str()))
                {
                    #ifdef DEBUG
                    qDebug() << "is file";
                    #endif
                    
                    std::string destPath = destination.toString().toStdString();
                    if(destPath.back() != '/')
                    {
                        destPath += std::string("/");
                    }
                    destPath += fileOrFolder.toString().toStdString();
                    
                    #ifdef DEBUG
                    qDebug() << "Copying" << path.c_str() << " to " << destPath.c_str();
                    #endif
                    
                    if(filesOperations::isFile(destPath.c_str()))
                    {
                        if(filesOverwritten.size() < MAX_OVERWRITE_SIZE)
                        {
                            std::string overwrittenFile = destPath;
                            if(overwrittenFile.length() > 30) // This will be to long to show in the GUI, chop of the begining - show maximum 30 letters here.
                            {
                                overwrittenFile = "...";
                                int startOfFile = destPath.length() - 30 - 3; // the 3 is for the ... we are starting of with
                                overwrittenFile.append(destPath.substr(startOfFile));
                            }                        
                            filesOverwritten << overwrittenFile.c_str();

                            #ifdef DEBUG
                            qDebug() << "file that would be overwritten: " << destPath.c_str();
                            #endif
                        }                        
                    }
                    
                    if(performCopy)
                    {
                        bool status = filesOperations::copyFile(path.c_str(), destPath.c_str());

                        #ifdef DEBUG
                        qDebug() << "Copy status:" << status;
                        #endif
                    }
                    
                }
            }
        }        
    }
}
//-------------------------------------------------------------------------------------------------
bool FileCommanderListAndFilesModel::hasOverwriteFiles(QVariant destination)
{
    QStringList overWrittenFiles;
    copyItemToOtherSidePerform(destination, false, overWrittenFiles);
    
    return overWrittenFiles.size() > 0 ? true : false;
}
//-------------------------------------------------------------------------------------------------
QStringList FileCommanderListAndFilesModel::overWriteListOnCopyToOtherSide(QVariant destination)
{   
    QStringList overWrittenFiles;
    copyItemToOtherSidePerform(destination, false, overWrittenFiles);
     
    return overWrittenFiles;
}
//-------------------------------------------------------------------------------------------------
void  FileCommanderListAndFilesModel::copyItemToOtherSide(QVariant destination)
{
    QStringList overWrittenFiles;
    copyItemToOtherSidePerform(destination, true, overWrittenFiles);
}
//-------------------------------------------------------------------------------------------------
void  FileCommanderListAndFilesModel::setCreateFolder(QVariant path)
{
    m_folderToCreate = path.toString();
}
//-------------------------------------------------------------------------------------------------
void FileCommanderListAndFilesModel::createFolder()
{
    if(m_folderToCreate.length() > 0)
    {
        #ifdef DEBUG
        qDebug() << "Creating folder:" << m_folderToCreate;
        #endif

        bool status = filesOperations::createDir(m_folderToCreate.toStdString().c_str());

        #ifdef DEBUG
        qDebug() << "Creating folder status: " << status;
        #endif    
        m_folderToCreate.clear();
    }
}
//-------------------------------------------------------------------------------------------------
void FileCommanderListAndFilesModel::deleteSelectedItems()
{
    for(unsigned int i = 0; i < m_filesAndFoldersNames.size(); i++)
    {        
        if(m_checkedStatus[i])
        {
            QVariant fileOrFolder = m_filesAndFoldersNames[i];
            
            if(fileOrFolder.toString().toStdString().compare("..") != 0)
            {
                std::string path = m_selectedDir.toStdString();                
                if(path.back() != '/')
                {
                    path += std::string("/");
                }
                path += fileOrFolder.toString().toStdString();
                
                // Let user do what he asked for
                if(!m_homePhabletShowOnlySafe)
                {
                    int status = filesOperations::removePath(path.c_str());

                    #ifdef DEBUG
                    qDebug() << "status for deleting folder " << path.c_str() << " : " << status;
                    #endif    
                }
                else
                {
                    // Dont let user delete certain directories
                    if(!PathIsHomePhabletNoDeleteDirectory(QString(path.c_str())))
                    {
                        int status = filesOperations::removePath(path.c_str());

                        #ifdef DEBUG
                        qDebug() << "status for deleting folder " << path.c_str() << " : " << status;
                        #endif                        
                    }
                }
                
            }
        }
    }
    filesAndFoldersFromFolder(m_selectedDir);
}
//-------------------------------------------------------------------------------------------------
bool  FileCommanderListAndFilesModel::SelectedItemsContainsPhabletNoDeleteDirectories()
{
    for(unsigned int i = 0; i < m_filesAndFoldersNames.size(); i++)
    {        
        if(m_checkedStatus[i])
        {
            QVariant fileOrFolder = m_filesAndFoldersNames[i];
            
            std::string path = m_selectedDir.toStdString();                
            if(path.back() != '/')
            {
                path += std::string("/");
            }
            path += fileOrFolder.toString().toStdString();
            
            if(PathIsHomePhabletNoDeleteDirectory(QString(path.c_str())))
            {
                return true;
            }
        }
    }
    
    return false;
}
//-------------------------------------------------------------------------------------------------
void  FileCommanderListAndFilesModel::renameSelectedItem(QVariant newName)
{
    if(GetNumberSelectedItems() == 1)
    {
        std::string newNamePath = m_selectedDir.toStdString() + std::string("/") + newName.toString().toStdString();
        
        #ifdef DEBUG
        qDebug() << "renaming " << GetFirstSelected() << " to " << newNamePath.c_str();
        #endif    
        
        // Let user do what he asked for
        int statusRename = -1;
        if(!m_homePhabletShowOnlySafe)
        {        
            statusRename = filesOperations::renamePath(GetFirstSelected().toStdString().c_str(), newNamePath.c_str());
        }
        else
        {
            // Dont let user delete certain directories
            if(!PathIsHomePhabletNoDeleteDirectory(QString(GetFirstSelected().toStdString().c_str())))
            {
                statusRename = filesOperations::renamePath(GetFirstSelected().toStdString().c_str(), newNamePath.c_str());
            }
        }        
        #ifdef DEBUG
        qDebug() << "status of rename: " << statusRename;
        #endif    
        
                
    }        
}
//-------------------------------------------------------------------------------------------------
void FileCommanderListAndFilesModel::setEditGuard(QVariant path)
{
    m_editGuard = path.toString();
}
//-------------------------------------------------------------------------------------------------
bool FileCommanderListAndFilesModel::equalToEditGuard(QVariant path)
{
    if(path.toString().toStdString().compare(m_editGuard.toStdString()) == 0)
    {
        return true;
    }
    return false;
}
//-------------------------------------------------------------------------------------------------
void FileCommanderListAndFilesModel::setNavigateToFolder(QVariant path)
{
    m_navigateToFolder = path.toString();
}
//-------------------------------------------------------------------------------------------------
void FileCommanderListAndFilesModel::navigateToFolderSet()
{
    if(!m_navigateToFolder.isEmpty())
    {
        filesAndFoldersFromFolder(m_navigateToFolder);
        m_navigateToFolder = "";
    }
}
//-------------------------------------------------------------------------------------------------
QString FileCommanderListAndFilesModel::FolderToVisitOnDoubleClick(QVariant index)
{
    if(m_filesAndFoldersNames.size() != 0)
    {
        QVariant itemDoubleClicked = m_filesAndFoldersNames[index.toInt()];
        // if it is a directory, traverse it        
        std::string folderToVisit = m_selectedDir.toStdString();
        std::string stdStringitemDoubleClicked = itemDoubleClicked.toString().toStdString();
        
        // .. means go up
        if(stdStringitemDoubleClicked.compare("..") == 0)
        {
            if(folderToVisit.find("/") != std::string::npos && folderToVisit.length() > 1)
            {
                if(folderToVisit.back() == '/')
                {
                    folderToVisit.pop_back();
                }
                
                if(folderToVisit.find_last_of("/") != std::string::npos)
                {
                    if(folderToVisit.find_last_of("/") == 0) // We are at the root
                    {
                        folderToVisit = "/";
                    }
                    else
                    {
                        folderToVisit = folderToVisit.substr(0, folderToVisit.find_last_of("/"));
                    }
                }
            }
        }
        else
        {
            #ifdef DEBUG
            qDebug() << "clicked on actual folder" ;
            #endif
            
            if(folderToVisit.back() != '/')
            {
                folderToVisit += std::string("/");
            }
            folderToVisit += itemDoubleClicked.toString().toStdString();
        }
        
        #ifdef DEBUG
        qDebug() << "doDoubleClicked: " << folderToVisit.c_str();
        #endif
        
        if(filesOperations::isDirectory(folderToVisit.c_str()))
        {
            return QString(folderToVisit.c_str());
        }        
    }    
    return m_selectedDir; // Stay in same folder
}
//-------------------------------------------------------------------------------------------------
bool FileCommanderListAndFilesModel::checkFolderToVisitIsInHome(QVariant index)
{
    QString folderToVisit = FolderToVisitOnDoubleClick(index);
    
    if(folderToVisit.toStdString().find("/home") == 0)
    {
        return true;
    }
    
    return false;
}
//-------------------------------------------------------------------------------------------------
bool  FileCommanderListAndFilesModel::checkFolderContainsHomePath(QVariant path)
{
    qDebug() << "checkFolderContainsHomePath: " << path;
    
    if(path.toString().toStdString().find("/home") == 0)
    {
        return true;
    }
    
    return false;    
}    
//-------------------------------------------------------------------------------------------------
QString FileCommanderListAndFilesModel::doDoubleClicked(QVariant index)
{
    return filesAndFoldersFromFolder(FolderToVisitOnDoubleClick(index));
}
//-------------------------------------------------------------------------------------------------
bool FileCommanderListAndFilesModel::appendRow(QString data, bool checked)
{
    int lastElemPos = this->m_filesAndFoldersNames.size();
    beginInsertRows(QModelIndex(), lastElemPos, lastElemPos);
    m_filesAndFoldersNames << data;
    m_checkedStatus << checked;
    

    endInsertRows();

    return true;

}
//-------------------------------------------------------------------------------------------------
bool FileCommanderListAndFilesModel::myRemoveRows(int position, int rows)
{
    beginRemoveRows(QModelIndex(), position, position+rows-1);

    for (int row=0; row < rows; ++row) 
    {
        m_filesAndFoldersNames.removeAt(position);
        m_checkedStatus.removeAt(position);
    }

    endRemoveRows();
    return true;
}
//-------------------------------------------------------------------------------------------------
void  FileCommanderListAndFilesModel::updateFolder()
{
    filesAndFoldersFromFolder(m_selectedDir);
}
//-------------------------------------------------------------------------------------------------
QString FileCommanderListAndFilesModel::HomePhabletSafeOnly()
{
    m_selectedDir = "/home/phablet";
    
    int lastElemPos = m_filesAndFoldersNames.size();
    myRemoveRows(0, lastElemPos);

    appendRow(QString(".."), false);
    appendRow(QString("Documents"), false);
    appendRow(QString("Downloads"), false);
    appendRow(QString("Music"), false);
    appendRow(QString("Pictures"), false);
    appendRow(QString("Videos"), false);
    
    return m_selectedDir;
}
//-------------------------------------------------------------------------------------------------
bool FileCommanderListAndFilesModel::PathIsHomePhabletNoDeleteDirectory(QVariant path)
{
    std::string pathStdString = path.toString().toStdString();
    
    if(pathStdString.compare("/home/phablet") == 0 ||
       pathStdString.compare("/home/phablet/Documents") == 0 ||
       pathStdString.compare("/home/phablet/Downloads") == 0 ||
       pathStdString.compare("/home/phablet/Music") == 0 ||
       pathStdString.compare("/home/phablet/Pictures") == 0 ||       
       pathStdString.compare("/home/phablet/Videos") == 0)
    {
        return true;
    }
    
    return false;
}       
//-------------------------------------------------------------------------------------------------
void FileCommanderListAndFilesModel::ClearHomePhabletShowOnlySafe()
{
    m_homePhabletShowOnlySafe = false;
}
//-------------------------------------------------------------------------------------------------
QString FileCommanderListAndFilesModel::filesAndFoldersFromFolder(QVariant folderName)
{
    #ifdef DEBUG
    qDebug() << "FileCommanderListAndFilesModel filesAndFoldersFromFolder" << folderName;
    #endif
    
    std::string folderStdString = folderName.toString().toStdString();
    std::transform(folderStdString.begin(), folderStdString.end(), folderStdString.begin(), tolower);
        
    if(m_homePhabletShowOnlySafe && folderStdString.compare("/home/phablet") == 0)
    {
        return HomePhabletSafeOnly();
    }
    
    #ifdef DEBUGSHOWSTATS
    char statsBuf[512];
    filesOperations::statsPath(folderName.toString().toStdString().c_str(), statsBuf);
    qDebug() << "path stats: " << statsBuf;
    #endif
    
    
    std::vector<std::string> filesAndFolders;   
    int status = filesOperations::getDirFiles(folderName.toString().toStdString().c_str(), filesAndFolders, true);    
    if(status == 0)
    {
        int lastElemPos = m_filesAndFoldersNames.size();
        myRemoveRows(0, lastElemPos);
        
        m_selectedDir = folderName.toString();
        
        for(std::vector<std::string>::iterator it = filesAndFolders.begin(); it != filesAndFolders.end(); it++)
        {
            if((*it).compare(".") != 0)
            {
                appendRow(QString((*it).c_str()), false);
            }
        }
    }
    
    return m_selectedDir;
}
//-------------------------------------------------------------------------------------------------
QString FileCommanderListAndFilesModel::GetFirstSelected()
{
    for(unsigned int i = 0; i < m_filesAndFoldersNames.size(); i++)
    {        
        if(m_checkedStatus[i])
        {
            QVariant fileOrFolder = m_filesAndFoldersNames[i];
            
            if(fileOrFolder.toString().toStdString().compare("..") != 0)
            {
                std::string path = m_selectedDir.toStdString();
                if(path.back() != '/')
                {
                    path += std::string("/");
                }
                path += fileOrFolder.toString().toStdString();
                
                return QString(path.c_str());
            }
        }
    }
    
    return QString("");
}
//-------------------------------------------------------------------------------------------------
QString  FileCommanderListAndFilesModel::GetFirstSelectedItem()
{
    for(unsigned int i = 0; i < m_filesAndFoldersNames.size(); i++)
    {        
        if(m_checkedStatus[i])
        {
            QVariant fileOrFolder = m_filesAndFoldersNames[i];
            
            return fileOrFolder.toString();
        }
    }
    
    return QString("");
}
//-------------------------------------------------------------------------------------------------
unsigned int FileCommanderListAndFilesModel::GetNumberSelectedItems()
{
    unsigned int retValue = 0;
    
    for(unsigned int i = 0; i < m_filesAndFoldersNames.size(); i++)
    {        
        if(m_checkedStatus[i])
        {
            retValue++;
        }
    }
    
    return retValue;
}
//-------------------------------------------------------------------------------------------------
QString FileCommanderListAndFilesModel::selectedFileContent()
{
    if(GetNumberSelectedItems() == 1)
    {
        std::string path = GetFirstSelected().toStdString();               
        if(filesOperations::isFile(path.c_str()))
        {
            std::string fileContent;
            filesOperations::getFileContents(path.c_str(), fileContent);

            if(fileContent.length() > 500) // Dont give the user back larger amounts of data than this...
            {
                fileContent = fileContent.substr(0, 500);
            }
            QString retValue = QString(fileContent.c_str());
            return retValue;
        }   
    }    
    return QString("");
}
//-------------------------------------------------------------------------------------------------
QString FileCommanderListAndFilesModel::selectedItem()
{
    if(GetNumberSelectedItems() == 1)
    {
        return GetFirstSelectedItem();
    }
    return QString("");
}
//-------------------------------------------------------------------------------------------------
QString FileCommanderListAndFilesModel::selectedDirectory()
{
    return m_selectedDir;
}
//-------------------------------------------------------------------------------------------------
bool FileCommanderListAndFilesModel::hasFilesChecked()
{
    for(unsigned int i = 0; i < m_checkedStatus.size(); i++)
    {
        if(m_checkedStatus[i])
        {
            return true;
        }
    }
    return false;
}
//-------------------------------------------------------------------------------------------------
bool FileCommanderListAndFilesModel::hasOneItemChecked()
{
    return GetNumberSelectedItems() == 1 ? true : false;
}


