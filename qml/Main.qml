import QtQuick 2.4
import QtQuick.Layouts 1.1
import Ubuntu.Components 1.3
import FileCommanderPlugin 1.0
import Ubuntu.Components.Popups 1.3

MainView 
{
    id: mainView
    objectName: 'mainView'
    applicationName: 'filecommander.jimmiejohnsson'
    automaticOrientation: true
    
    width: units.gu(45)
    height: units.gu(75)
    
    FileCommanderListAndFilesModel
    {
        id:m_listAndFilesModelInstance
    }
    
    FileCommanderListAndFilesModel
    {
        id:m_listAndFilesModelInstanceRight
    }
    
    FileCommanderGlobalData
    {
        id:m_fileCommanderGlobalData
    }
    
    OverWriteListView
    {
        id:m_overWriteListView
    }
    
    function copyFiles() 
    {
        if(m_listAndFilesModelInstance.hasFilesChecked())
        {
            m_listAndFilesModelInstance.copyItemToOtherSide(inputRightPath.text)
            m_listAndFilesModelInstanceRight.updateFolder()
        }
        else if(m_listAndFilesModelInstanceRight.hasFilesChecked())
        {
            m_listAndFilesModelInstanceRight.copyItemToOtherSide(inputLeftPath.text)            
            m_listAndFilesModelInstance.updateFolder()
        }        
    }
    
    function checkOverwriteAndPerformCopy()
    {
        if(m_listAndFilesModelInstance.hasOverwriteFiles(inputRightPath.text) || m_listAndFilesModelInstanceRight.hasOverwriteFiles(inputLeftPath.text))
        {
            PopupUtils.open(Qt.resolvedUrl("WarningOverwriteDlg.qml"), mainView,
            {
                title: "Overwrite files",
                text: "These files will be overwritten if you continue"
            })
        }
        else
        {
            copyFiles()
        }    
    }
    
    function renameItems()
    {
        if(m_listAndFilesModelInstance.hasFilesChecked() && m_listAndFilesModelInstance.hasOneItemChecked())
        {
            PopupUtils.open(Qt.resolvedUrl("RenameDlg.qml"), mainView,
            {
                title: "Rename",
                text: m_listAndFilesModelInstance.selectedItem()
            })
        }
        else if(m_listAndFilesModelInstanceRight.hasFilesChecked() &&  m_listAndFilesModelInstanceRight.hasOneItemChecked())
        {
            PopupUtils.open(Qt.resolvedUrl("RenameDlg.qml"), mainView,
            {
                title: "Rename",
                text: m_listAndFilesModelInstanceRight.selectedItem()
            })
        }                            
    }
    
   Timer 
   {
        id: start_timer
        interval: 300;
        onTriggered:
        {
            PopupUtils.open(Qt.resolvedUrl("StartupDlg.qml"), mainView,
            {
                title: "Welcome"
            })
        
        }
    }    

    
    Page 
    {
        anchors.fill: parent

        header: PageHeader 
        {
            id: header
            title: i18n.tr('File Commander')
        }
        
        Item 
        {
            id: leftFieldPath
            anchors.left: parent.left
            anchors.top: header.bottom            
            width: units.gu(23)
            height: units.gu(3)
            clip: true
            Rectangle
            {
                id: inputLeftPathRect
                color: "white"
                border.width: 2
                border.color: "black"                
                radius: 2                
                width: units.gu(23); height: units.gu(3)
                TextInput 
                {
                    id: inputLeftPath                
                    color: "black"
                    text: "/home/phablet"
                    inputMethodHints: Qt.ImhNoPredictiveText
                    
                    
                    width: parent.width
                    height: parent.height
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.leftMargin : parent.border.width
                    anchors.rightMargin : parent.border.width
                    anchors.topMargin : parent.border.width
                    anchors.bottomMargin : parent.border.width
                    
                    focus: true
                    
                    onEditingFinished: 
                    {
                        if(!m_listAndFilesModelInstance.equalToEditGuard(text))
                        {
                            m_listAndFilesModelInstance.setEditGuard(text)
                            if(!m_listAndFilesModelInstance.checkFolderContainsHomePath(text)) // Folder is outside of the home folder - warn user
                            {
                                if(!m_fileCommanderGlobalData.warningNavigatingFromHomeShown())
                                {
                                    m_listAndFilesModelInstance.setNavigateToFolder(text) // Inside WarningLeavingHome we will navigate to this folder
                                    PopupUtils.open(Qt.resolvedUrl("WarningLeavingHome.qml"), mainView,
                                    {
                                        title: "Warning - Your leaving home",
                                        text: "Changing files outside of home could corrupt your phone, continue?"
                                    })                                                                      
                                }
                                else
                                {
                                    text: m_listAndFilesModelInstance.filesAndFoldersFromFolder(text)
                                }
                            }
                            else // Folder inside the home folder, user should be safe
                            {
                                text: m_listAndFilesModelInstance.filesAndFoldersFromFolder(text)
                            }
                        }
                    }
                }
            }
        }
               
        ListView 
        {
            id:leftFolderList
            anchors.left: parent.left
            anchors.top: leftFieldPath.bottom
            width:  units.gu(23)
            height: units.gu(60)
            
            spacing: 2
            model : m_listAndFilesModelInstance
            clip: true
            focus: true
            delegate: Component 
            {
                id: theComponentLeft
                Item 
                {
                    id: listItemLeft
                    width: parent.width
                    height: units.gu(5)
                    Row 
                    {
                        Image
                        {
                            source: m_listAndFilesModelInstance.getImgSource(index)

                        }                                        
                        Text { text:  model.fileOrFolderRole }
                    }
                    MouseArea
                    {
                        anchors.fill: parent
                        onClicked: leftFolderList.currentIndex = index
                        onDoubleClicked : 
                        {                            
                            if(!m_listAndFilesModelInstance.checkFolderToVisitIsInHome(index)) // Folder is outside of the home folder - warn user
                            {                                
                                if(!m_fileCommanderGlobalData.warningNavigatingFromHomeShown())
                                {
                                    m_listAndFilesModelInstance.setNavigateToFolder(m_listAndFilesModelInstance.FolderToVisitOnDoubleClick(index))
                                    PopupUtils.open(Qt.resolvedUrl("WarningLeavingHome.qml"), mainView,
                                    {
                                        title: "Warning - Your leaving home",
                                        text: "Changing files outside of home could corrupt your phone, continue?"
                                    })
                                }
                                else
                                {
                                    inputLeftPath.text = m_listAndFilesModelInstance.doDoubleClicked(index)
                                }
                            }
                            else // Folder inside the home folder, user should be safe
                            {
                                inputLeftPath.text = m_listAndFilesModelInstance.doDoubleClicked(index)
                            }
                            
                        }
                    }                    
                    CheckBox
                    {
                        id: checkbox
                        checked: model.checkedRole
                        anchors 
                        {
                            right: listItemLeft.right
                            verticalCenter: listItemLeft.verticalCenter 
                        } 
                        onClicked:
                        {
                            m_listAndFilesModelInstance.setChecked(index, checkbox.checked)
                            m_listAndFilesModelInstanceRight.clearAllChecked()
                        }
                    }                    
                }
            }            
        }
        
        Item 
        {
            id: rightFieldPath
            anchors.left: leftFieldPath.right
            anchors.top: header.bottom            
            width: units.gu(23)
            height: units.gu(3)
            clip: true
            Rectangle
            {
                id: inputRightPathRect
                color: "white"
                border.width: 2
                border.color: "black"                
                radius: 2                
                width: units.gu(23); height: units.gu(3)
                TextInput 
                {
                    id: inputRightPath                
                    color: "black"
                    text: "/home/phablet"
                    
                    width: parent.width
                    height: parent.height
                    anchors.left: parent.left
                    anchors.top: parent.top
                    anchors.leftMargin : parent.border.width
                    anchors.rightMargin : parent.border.width
                    anchors.topMargin : parent.border.width
                    anchors.bottomMargin : parent.border.width
                    
                    focus: true
                    
                    onEditingFinished: 
                    {
                        if(!m_listAndFilesModelInstanceRight.equalToEditGuard(text))
                        {
                            m_listAndFilesModelInstanceRight.setEditGuard(text)
                            if(!m_listAndFilesModelInstanceRight.checkFolderContainsHomePath(text)) // Folder is outside of the home folder - warn user
                            {
                                if(!m_fileCommanderGlobalData.warningNavigatingFromHomeShown())
                                {
                                    m_listAndFilesModelInstanceRight.setNavigateToFolder(text) // Inside WarningLeavingHome we will navigate to this folder
                                    PopupUtils.open(Qt.resolvedUrl("WarningLeavingHome.qml"), mainView,
                                    {
                                        title: "Warning - Your leaving home",
                                        text: "Changing files outside of home could corrupt your phone, continue?"
                                    })                                                                      
                                }
                                else
                                {
                                    text: m_listAndFilesModelInstanceRight.filesAndFoldersFromFolder(text)
                                }
                            }
                            else // Folder inside the home folder, user should be safe
                            {
                                text: m_listAndFilesModelInstanceRight.filesAndFoldersFromFolder(text)
                            }
                        }
                    }
                }
            }
        }
        
        ListView 
        {
            id:rightFolderList
            anchors.left: leftFolderList.right
            anchors.top: rightFieldPath.bottom
            width:  units.gu(23)
            height: units.gu(60)
            
            spacing: 2
            model : m_listAndFilesModelInstanceRight
            clip: true
            focus: true
            delegate: Component 
            {
                id: theComponentRight
                Item 
                {
                    id: listItemRight
                    width: parent.width
                    height: units.gu(5)
                    Row 
                    {
                        Image
                        {
                            source: m_listAndFilesModelInstanceRight.getImgSource(index)

                        }                                        
                        Text { text:  model.fileOrFolderRole }
                    }
                    MouseArea
                    {
                        anchors.fill: parent
                        onClicked: rightFolderList.currentIndex = index
                        onDoubleClicked : 
                        {
                            if(!m_listAndFilesModelInstanceRight.checkFolderToVisitIsInHome(index)) // Folder is outside of the home folder - warn user
                            {
                                if(!m_fileCommanderGlobalData.warningNavigatingFromHomeShown())
                                {
                                    m_listAndFilesModelInstanceRight.setNavigateToFolder(m_listAndFilesModelInstanceRight.FolderToVisitOnDoubleClick(index))
                                    PopupUtils.open(Qt.resolvedUrl("WarningLeavingHome.qml"), mainView,
                                    {
                                        title: "Warning - Your leaving home",
                                        text: "Changing files outside of home could corrupt your phone, continue?"
                                    })
                                }
                                else
                                {
                                    inputRightPath.text = m_listAndFilesModelInstanceRight.doDoubleClicked(index)
                                }
                            }
                            else // Folder inside the home folder, user should be safe
                            {
                                inputRightPath.text = m_listAndFilesModelInstanceRight.doDoubleClicked(index)
                            }
                            
                        }
                    }
                    CheckBox
                    {
                        id: checkbox
                        checked: model.checkedRole
                        anchors 
                        {
                            right: listItemRight.right
                            verticalCenter: listItemRight.verticalCenter 
                        } 
                        onClicked:
                        {
                            m_listAndFilesModelInstanceRight.setChecked(index, checkbox.checked)
                            m_listAndFilesModelInstance.clearAllChecked()
                        }
                    }
                    
                }
            }            
        }
        
        // Spacer
        Label
        {
            id: labelSpacer
            anchors.left: parent.left
            anchors.top: rightFolderList.bottom            
            height: units.gu(2)
            text:"                     "
        }                
        
        Item 
        {
            id: buttonArea
            anchors.left: parent.left
            anchors.top: labelSpacer.bottom
            anchors.leftMargin : 4
            width: units.gu(45)
            height: units.gu(9)  
            Row
            {
                spacing: units.gu(1)
                Button
                {
                    width: units.gu(8)
                    height: units.gu(9)
                    text: "View"
                    onClicked: 
                    {
                        if(m_listAndFilesModelInstance.hasFilesChecked())
                        {
                            PopupUtils.open(Qt.resolvedUrl("FileInspect.qml"), mainView,
                            {
                                title: "File content"                            
                            })
                        }
                        else if(m_listAndFilesModelInstanceRight.hasFilesChecked())
                        {
                            PopupUtils.open(Qt.resolvedUrl("FileInspectRight.qml"), mainView,
                            {
                                title: "File content"                            
                            })
                        }
                    }
                }
                Button
                {
                    width: units.gu(7)
                    height: units.gu(9)
                    text: "Copy"
                    onClicked: 
                    {
                        if(inputLeftPath.text == inputRightPath.text)
                        {
                            PopupUtils.open(Qt.resolvedUrl("InfoDlg.qml"), mainView,
                            {
                                title: "Info",
                                text: "You cant copy files within the same folder"
                            })
                        }
                        
                        // First check - do not allow copying within the same folder. That dose'nt mean anything and should not be allowed at all
                        if(inputLeftPath.text != inputRightPath.text)
                        {

                            if(m_listAndFilesModelInstance.hasFilesChecked())
                            {
                                m_overWriteListView.setOverwriteList(m_listAndFilesModelInstance.overWriteListOnCopyToOtherSide(inputRightPath.text))
                            }
                            else if(m_listAndFilesModelInstanceRight.hasFilesChecked())
                            {
                                m_overWriteListView.setOverwriteList(m_listAndFilesModelInstanceRight.overWriteListOnCopyToOtherSide(inputLeftPath.text))
                            }                        

                            // Only one will have any selected files, as checking a checkbox on one side unchecks all others on the other side
                            if(m_listAndFilesModelInstance.isBigCopy())
                            {
                                PopupUtils.open(Qt.resolvedUrl("WarningBigCopyDlg.qml"), mainView,
                                {
                                    title: "Warning",
                                    text: "You selected a folder that contains more than 1000 files/dirs, this could take a while - proceed?"
                                })
                            }
                            else if(m_listAndFilesModelInstanceRight.isBigCopy())
                            {
                                PopupUtils.open(Qt.resolvedUrl("WarningBigCopyDlg.qml"), mainView,
                                {
                                    title: "Warning",
                                    text: "You selected a folder that contains more than 1000 files/dirs, this could take a while - proceed?"
                                })
                            }
                            else
                            {
                                if(m_listAndFilesModelInstance.hasOverwriteFiles(inputRightPath.text) || m_listAndFilesModelInstanceRight.hasOverwriteFiles(inputLeftPath.text))
                                {
                                    PopupUtils.open(Qt.resolvedUrl("WarningOverwriteDlg.qml"), mainView,
                                    {
                                        title: "Overwrite files",
                                        text: "These files will be overwritten if you continue"
                                    })
                                }
                                else
                                {
                                    if(m_listAndFilesModelInstance.hasFilesChecked() || m_listAndFilesModelInstanceRight.hasFilesChecked())
                                    {
                                        PopupUtils.open(Qt.resolvedUrl("CopyFileDlg.qml"), mainView,
                                        {
                                            title: "Perform",
                                            text: "Copy file(s)/director(ies)?"
                                        })
                                    }
                                }
                            }
                        }                        
                    }
                }
                Button
                {
                    width: units.gu(8)
                    height: units.gu(9)
                    text: "Folder"
                    onClicked: 
                    {
                        PopupUtils.open(Qt.resolvedUrl("NewFolderDlg.qml"), mainView,
                        {
                            title: "Create new folder",
                            text: "Full name of path for new folder"
                        })
                    }
                    
                }                
                Button
                {
                    width: units.gu(10)
                    height: units.gu(9)
                    text: "Rename"                    
                    onClicked: 
                    {
                        if(!m_listAndFilesModelInstance.hasOneItemChecked() && !m_listAndFilesModelInstanceRight.hasOneItemChecked())
                        {
                            PopupUtils.open(Qt.resolvedUrl("InfoDlg.qml"), mainView,
                            {
                                title: "Info",
                                text: "Please select only one item to rename"
                            })
                        }
                        
                        if(m_listAndFilesModelInstance.SelectedItemsContainsPhabletNoDeleteDirectories() || m_listAndFilesModelInstanceRight.SelectedItemsContainsPhabletNoDeleteDirectories())
                        {
                                if(!m_fileCommanderGlobalData.warningNavigatingFromHomeShown())
                                {
                                    PopupUtils.open(Qt.resolvedUrl("InfoDlg.qml"), mainView,
                                    {
                                        title: "Info",
                                        text: "You tried to rename a protected folder. Leave home folder once to enable this feature"
                                    })
                                }
                                else if(m_fileCommanderGlobalData.warningNavigatingFromHomeShown())
                                {
                                    renameItems()
                                }
                        }
                        else
                        {
                            renameItems()
                        }                        
                    }                    
                }                
                
                Button
                {
                    width: units.gu(8)
                    height: units.gu(9)
                    text: "Delete"
                    onClicked:
                    {
                        if(m_listAndFilesModelInstance.hasFilesChecked() || m_listAndFilesModelInstanceRight.hasFilesChecked())
                        {
                            if(m_listAndFilesModelInstance.SelectedItemsContainsPhabletNoDeleteDirectories() || m_listAndFilesModelInstanceRight.SelectedItemsContainsPhabletNoDeleteDirectories())
                            {
                                if(!m_fileCommanderGlobalData.warningNavigatingFromHomeShown())
                                {
                                    PopupUtils.open(Qt.resolvedUrl("InfoDlg.qml"), mainView,
                                    {
                                        title: "Info",
                                        text: "You tried to delete a protected folder. Leave home folder once to enable this feature"
                                    })
                                }
                                else if(m_fileCommanderGlobalData.warningNavigatingFromHomeShown())
                                {
                                    PopupUtils.open(Qt.resolvedUrl("DeletePopupDlg.qml"), mainView,
                                    {
                                        title: "Delete",
                                        text: "Really delete selected?"
                                    })
                                }
                            }
                            else
                            {
                                PopupUtils.open(Qt.resolvedUrl("DeletePopupDlg.qml"), mainView,
                                {
                                    title: "Delete",
                                    text: "Really delete selected?"
                                })
                            }
                            
                            
                        }                    
                    }
                }                
                
            }
        }            
    } 
    
    Component.onCompleted:
    {
        start_timer.start()
    }        
}
