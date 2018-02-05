# File Commander

File browser program

Wish list:
* Search functionallity
* Better viewer (support images)
* Support "open with"
* Possibly add a menu with more option
* Favourite folders
* Support for copy/paste in textinput where you specify path
    - Some kind of bug with QT. Way to much work to workaround it.

2018-01-30:

100% * Add warning if user navigates away from the /home folder
    - Some kind of bug though, you have to doubleclick home folder outside of home again to update GUI
    
    
    
2018-01-31  
* Add warning if user navigates away from the /home folder by tying in the folder input field
* Overwrite of file warning
* Disable rotation of layout
* Make buttons larger (height mainly)
* Rename some functions in filecommanderplugin.h that start with big letter to small like all the others


2018-02-01:
100% * Doubleclick on folder leaving home, path is not updated
* Overwrite of file warning
* Fix rename/Folder so you can have an OK button instead of having to rely on "editfinished"


2018-02-03:
100% * Create overwrite dialog with listview of files to be overwritten
100% * Make sure that files are really overwritten when choosing to overwrite
100% * Fix rename/Folder so you can have an OK button instead of having to rely on "editfinished"
        - Cant make it happen, instead changed label so more obvious user needs to press enter to confirm action
        
100% * Bug, when editing rename/create folder and pressing Cancel - action is still taken        
    - use onAccepted instead of onEditingFinished
    
    
2018-02-04:    

100% * Make buttons larger (height mainly)
100% * Copy - popup dialog shown even if nothing is selected
100% * Rename - popup dialog is shown even if multiple items are selected
100% * Rename PopUpDlg.qml
        -now called CopyFileDlg.qml
100% * Crash if trying to perform Warning to big copy in some cases
        - Added a limit so you wont run into buffer overruns
        
100% * Add simple popups informing user when he is doing things incorrectly
        - Cant copy within the same folder
        - Please select one item to rename
        
100% * If user has not been outside of /home, when browsing /home/phablet, only show Documents, Downloads, Music, Pictures and Videos folders.
100%    * Disable delete Documents, Downloads, Music, Pictures and Videos folders unless user has gone outside of home
100%    * Disable rename Documents, Downloads, Music, Pictures and Videos folders unless user has gone outside of home
100% * Add startup info screen
* Go through code and remove unused things, maybe refactor some things
* Test to verify things are working


2018-02-05:

* Go through code and remove unused things, maybe refactor some things
    100% * WarningOverwriteDlg.qml
    100% * WarningLeavingHome.qml
    100% * WarningBigCopyDlg.qml
    100% * StartupDlg.qml
    100% * RenameDlg.qml
    100% * NewFolderDlg.qml
    100% * Main.qml
    100% * InfoDlg.qml
    100% * FileInspectRight.qml
    100% * FileInspect.qml
    100% * DeletePopupDlg.qml
    100% * CopyFileDlg.qml
    100% * plugin.cpp
    100% * plugin.h
    100% * filesOperations.h
    100% * filesOperations.cpp
    100% * filecommanderplugin.h
    100% * filecommanderplugin.cpp
    
* Test to verify things are working
    100% * Splash screen
    100% *  warning leave home
        100% * edit left path
        100% * edit right path
        100% * double click left path
        100% * double click right path
    
    100% * View
        100% * Left
        100% * Right
    
    100% * Copy
        100% * Single/multi file(s)
            100% * overwrite
            100% * no overwrite
        * directory
            100% * Big copy 1000 files
                100% * overwrite
                100% * no overwrite
            100% * Not big copy
                100% * overwrite
                100% * no overwrite
    
    100% * Rename
        100% * file
        100% * directory
        
    100% * Delete        
    
* Change version from 1.0.1 to 1.0.0 when uploading (have to have 1.0.1 in mine due to apparmor profile problem)