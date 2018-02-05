import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

Dialog 
{
    id: copyFileDlg
    
    Row
    {
        spacing: 2
        
        Button 
        {
            id:copyOK
            text: i18n.tr("OK")
            onClicked: 
            {
                copyFiles()
                PopupUtils.close(copyFileDlg)
            }
        }
        Button 
        {
            id:copyCancel
            text: i18n.tr("Cancel")
            onClicked: 
            {
                PopupUtils.close(copyFileDlg)
            }
        }
    }
}
