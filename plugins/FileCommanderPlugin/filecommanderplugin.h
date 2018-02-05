#ifndef FILECOMMANDERPLUGIN_H
#define FILECOMMANDERPLUGIN_H

#include <QObject>
#include <QStringList>
#include <QAbstractListModel>
#include <QVariant>

class FileCommanderGlobalData: public QObject 
{
    Q_OBJECT

public:
    FileCommanderGlobalData();
    ~FileCommanderGlobalData() = default;

    Q_INVOKABLE bool warningNavigatingFromHomeShown();
    Q_INVOKABLE void setWarningNavigatingFromHomeShown();

private:
    bool m_warningNavigatingFromHomeShown;        
};

class OverWriteListView: public QObject 
{
    Q_OBJECT
    Q_PROPERTY(QVariant getOverWriteListView READ getOverWriteListView)

public:
    OverWriteListView();
    ~OverWriteListView() = default;
    
    Q_INVOKABLE void                setOverwriteList(QStringList data);
    Q_INVOKABLE QVariant            getOverWriteListView(); // Provides a list of files that would be overwritten on a copy
    
private:    
    QStringList m_overwriteList;
    
};


class FileCommanderListAndFilesModel : public QAbstractListModel
{
    Q_OBJECT

public:    
    enum theRoles {theTextRole= Qt::UserRole + 1, checkedRole};
    
    FileCommanderListAndFilesModel(QObject *parent = 0);
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    bool appendRow(QString data, bool checked);
    bool myRemoveRows(int position, int rows);
    
    Q_INVOKABLE void                updateFolder();
    Q_INVOKABLE QString             filesAndFoldersFromFolder(QVariant folderName);    
    Q_INVOKABLE void                ClearHomePhabletShowOnlySafe();
    
    Q_INVOKABLE QString             selectedFileContent();
    Q_INVOKABLE QString             selectedItem();
    Q_INVOKABLE QString             selectedDirectory(); // Current directory of the model
    
    Q_INVOKABLE bool                hasFilesChecked();
    Q_INVOKABLE bool                hasOneItemChecked();
    
    Q_INVOKABLE QString             FolderToVisitOnDoubleClick(QVariant index);
    Q_INVOKABLE bool                checkFolderToVisitIsInHome(QVariant index);
    Q_INVOKABLE bool                checkFolderContainsHomePath(QVariant path);
    
    Q_INVOKABLE void                setChecked(QVariant index, bool checked);    
    Q_INVOKABLE void                clearAllChecked();    
    
    Q_INVOKABLE QString             doDoubleClicked(QVariant index);
    Q_INVOKABLE QString             getImgSource(QVariant index);
    Q_INVOKABLE bool                isBigCopy(); // Check if the copy we are about to perform is big, so we can warn user who might be about to do something he did not intend...
    
    Q_INVOKABLE bool                hasOverwriteFiles(QVariant destination);
    Q_INVOKABLE QStringList         overWriteListOnCopyToOtherSide(QVariant destination); // Provides a list of files that would be overwritten on a copy
    Q_INVOKABLE void                copyItemToOtherSide(QVariant destination);    
    
    
    Q_INVOKABLE void                setCreateFolder(QVariant path);
    Q_INVOKABLE void                createFolder();
    Q_INVOKABLE void                deleteSelectedItems();
    Q_INVOKABLE bool                SelectedItemsContainsPhabletNoDeleteDirectories();
    Q_INVOKABLE void                renameSelectedItem(QVariant path);
    
    Q_INVOKABLE void                setEditGuard(QVariant path);
    Q_INVOKABLE bool                equalToEditGuard(QVariant path);
    Q_INVOKABLE void                setNavigateToFolder(QVariant path);
    Q_INVOKABLE void                navigateToFolderSet();
    
    
    
    
    
protected :    
    QHash<int, QByteArray> roleNames() const;
    
private:
    QString             GetFirstSelected(); // m_selectedDir + selected item
    QString             GetFirstSelectedItem();
    unsigned int        GetNumberSelectedItems();    
    
    /*
        Safe version of /home/phablet directory.
        Only shows user the folders that are safe to browse/copy etc.
        To minimize risk of the user accidentally doing something bad
    */
    QString             HomePhabletSafeOnly();
    bool                PathIsHomePhabletNoDeleteDirectory(QVariant path);
    
    /*
        destination = path to copy to, performCopy = if true, the copy is peformed - otherwise no copy is performed.
        filesOverwritten = set of files that would be overwritten,
    */
    void                copyItemToOtherSidePerform(QVariant destination, bool performCopy, QStringList &filesOverwritten);

    
    
    QList <QString> m_filesAndFoldersNames;
    QList <bool> m_checkedStatus;
    
    QString m_folderToCreate;
    
    QString m_selectedDir;
    
    QString m_editGuard;
    QString m_navigateToFolder;
    
    /*
        If true (default is true), only folders Documents, Downloads, Pictures and Videos are shown in home/phablet.
        This is to protect users from doing something they dont want to do, such as dig around in the .cache and accidentally mess things up
    */
    bool    m_homePhabletShowOnlySafe; 

};



#endif
