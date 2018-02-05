import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

Dialog 
{
    id: inspectFileDlg
    
    TextEdit
    {
        width:  units.gu(23)
        height: units.gu(55)    
        text: m_listAndFilesModelInstanceRight.selectedFileContent()
        readOnly: true
        wrapMode: TextEdit.WordWrap
    }
    
    Row
    {
        Button 
        {
            id:copyOK
            text: i18n.tr("OK")
            onClicked: 
            {
                PopupUtils.close(inspectFileDlg)
            }
        }
    }
}
