import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

Dialog 
{
    id: deletePathDlg
    
    Row
    {
        spacing: 2
        
        Button 
        {
            id:copyOK
            text: i18n.tr("OK")
            onClicked: 
            {
                // Only one of the instances have selected items
                m_listAndFilesModelInstance.deleteSelectedItems()
                m_listAndFilesModelInstanceRight.deleteSelectedItems()
                PopupUtils.close(deletePathDlg)
            }
        }
        Button 
        {
            id:copyCancel
            text: i18n.tr("Cancel")
            onClicked: 
            {
                PopupUtils.close(deletePathDlg)
            }
        }
    }
}
