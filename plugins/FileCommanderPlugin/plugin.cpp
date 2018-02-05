#include <QtQml>
#include <QtQml/QQmlContext>

#include "plugin.h"
#include "filecommanderplugin.h"

void FileCommanderPluginPlugin::registerTypes(const char *uri) 
{
    //@uri FileCommanderPlugin
    qmlRegisterType<FileCommanderGlobalData>(uri, 1, 0, "FileCommanderGlobalData");
    qmlRegisterType<FileCommanderListAndFilesModel>(uri, 1, 0, "FileCommanderListAndFilesModel");
    qmlRegisterType<OverWriteListView>(uri, 1, 0, "OverWriteListView");    
}