import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

Dialog 
{
    id: startupDlg
    
    Column
    {
        spacing: 4
        TextEdit
        {
            id: introTextPart1
            width:  units.gu(23)
            height: units.gu(50)
            textFormat: TextEdit.RichText
            text: "<b>Welcome to File Commander</b><p>You are in protected mode - only safe files and folders are visible. To see everything navigate outside of the home folder.</p>\
<p>To enter a folder, double tap on it. Files/Folders have checkboxes to the right, to view/copy/rename items - check the box and choose appropriate action.</p>\
<p>Thanks for using the File Commander!</p>"
            readOnly: true
            wrapMode: TextEdit.WordWrap
        }        
        Button 
        {
            id:copyOK
            text: i18n.tr("OK")
            onClicked: 
            {
                PopupUtils.close(startupDlg)
            }
        }
    }
}
