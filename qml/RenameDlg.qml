import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

Dialog 
{
    id: renameDlg
    
    Column
    {
        spacing: 4             
        
        Label
        {
            text:"Give new name, press Enter to confirm"
        }        
        Item 
        {
            id: oldPath
            width: units.gu(23)
            height: units.gu(3)
            clip: true
            Rectangle
            {
                id: inputNewNameRect
                color: "white"
                border.width: 2
                border.color: "black"                
                radius: 2                
                width: units.gu(23); height: units.gu(3)
                TextInput 
                {
                    id: inputNewName                
                    color: "black"                    
                    text: "New name"

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
                        // Only one of the listview models will have an item selected so this is safe
                        m_listAndFilesModelInstance.renameSelectedItem(text)
                        m_listAndFilesModelInstanceRight.renameSelectedItem(text)

                        m_listAndFilesModelInstance.updateFolder()
                        m_listAndFilesModelInstanceRight.updateFolder()
                        PopupUtils.close(renameDlg)                   
                    }
                    
                }
            }
        }            
        Row
        {
            spacing: 2
            Button 
            {
                id:warningCancel
                text: i18n.tr("Cancel")
                onClicked: 
                {
                    PopupUtils.close(renameDlg)
                }
            }
        }
    }}
