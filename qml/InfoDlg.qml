import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

Dialog 
{
    id: informationDlg
    
    Row
    {
        spacing: 2
        
        Button 
        {
            id:copyOK
            text: i18n.tr("OK")
            onClicked: 
            {
                PopupUtils.close(informationDlg)
            }
        }
    }
}
