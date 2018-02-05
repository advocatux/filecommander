import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

Dialog 
{
    id: warningOverwriteDlg
    
    Column
    {
        spacing: 4
        
        ListView 
        {            
            width:  units.gu(23)
            height: units.gu(20)

            spacing: 2
            model : m_overWriteListView.getOverWriteListView
            delegate: Rectangle 
            {
                height: units.gu(2)
                width: parent.width  
                Text 
                {
                    text: modelData 
                }
            }
        }        
        
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
                        copyFiles()
                        PopupUtils.close(warningOverwriteDlg)
                    }                    
                    PopupUtils.close(warningOverwriteDlg)
                }
            }
            Button 
            {
                id:warningCancel
                text: i18n.tr("Cancel")
                onClicked: 
                {
                    PopupUtils.close(warningOverwriteDlg)
                }
            }
        }
    }
}
