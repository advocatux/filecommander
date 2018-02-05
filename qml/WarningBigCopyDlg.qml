import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

Dialog 
{
    id: warningBigCopyDlg
    
    Column
    {
        spacing: 2             
        Item 
        {
            width: parent.width
            height: units.gu(5)                               
            Row
            {
                spacing: 2
                CheckBox
                {
                    id: checkboxProceedCopy
                    checked: false
                }
                Text 
                { 
                    text:  "Proceed" 
                }
            }
        }
        Row
        {
            spacing: 2
            Button 
            {
                id:warningOk
                text: i18n.tr("OK")
                onClicked: 
                {
                    if(checkboxProceedCopy.checked)
                    {
                        PopupUtils.close(warningBigCopyDlg)
                        checkOverwriteAndPerformCopy()
                    }
                }
            }
            Button 
            {
                id:warningCancel
                text: i18n.tr("Cancel")
                onClicked: 
                {
                    PopupUtils.close(warningBigCopyDlg)
                }
            }
        }
    }
}
