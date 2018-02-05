import QtQuick 2.4
import Ubuntu.Components 1.3
import Ubuntu.Components.Popups 1.3

Dialog 
{
    id: warningLeavingHomeDlg
    
    Row
    {
        spacing: 2
        
        Button 
        {
            id:copyOK
            text: i18n.tr("OK")
            onClicked: 
            {
                m_fileCommanderGlobalData.setWarningNavigatingFromHomeShown() // User has been informed and choose to navigate away from home
                m_listAndFilesModelInstance.ClearHomePhabletShowOnlySafe()
                m_listAndFilesModelInstanceRight.ClearHomePhabletShowOnlySafe()
                m_listAndFilesModelInstance.navigateToFolderSet()
                m_listAndFilesModelInstanceRight.navigateToFolderSet()
                
                inputLeftPath.text = m_listAndFilesModelInstance.selectedDirectory()
                inputRightPath.text = m_listAndFilesModelInstanceRight.selectedDirectory()
                
                PopupUtils.close(warningLeavingHomeDlg)
            }
        }
        Button 
        {
            id:copyCancel
            text: i18n.tr("Cancel")
            onClicked: 
            {
                PopupUtils.close(warningLeavingHomeDlg)
            }
        }
    }
}
