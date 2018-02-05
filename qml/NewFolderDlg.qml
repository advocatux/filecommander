import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

Dialog 
{
    id: newFolderDlg
    
    Label
    {
        text:"Give new name, press Enter to confirm"
    }        
    Item 
    {
        id: folderName
        anchors.left: parent.left
        anchors.top: header.bottom            
        width: units.gu(23)
        height: units.gu(3)
        clip: true
        Rectangle
        {
            id: folderNameRect
            color: "white"
            border.width: 2
            border.color: "black"                
            radius: 2                
            width: units.gu(23); height: units.gu(3)
            TextInput 
            {
                id: inputfolderName
                color: "black"
                text: inputLeftPath.text                

                width: parent.width
                height: parent.height
                anchors.left: parent.left
                anchors.top: parent.top
                anchors.leftMargin : parent.border.width
                anchors.rightMargin : parent.border.width
                anchors.topMargin : parent.border.width
                anchors.bottomMargin : parent.border.width

                focus: true
                
                onAccepted: 
                {
                    m_listAndFilesModelInstance.setCreateFolder(text)
                    m_listAndFilesModelInstance.createFolder()
                    m_listAndFilesModelInstance.updateFolder()
                    m_listAndFilesModelInstanceRight.updateFolder()
                    PopupUtils.close(newFolderDlg)
                }
                
            }
        }
    }

    Row
    {
        spacing: 2
        Button 
        {
            text: i18n.tr("Cancel")
            onClicked: 
            {
                PopupUtils.close(newFolderDlg)
            }
        }
    }
}
