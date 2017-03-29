#include <QApplication>
#include <QDir>

#include "sheetwidget.h"

#ifdef _WIN32
    #include <QSettings>
#endif

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QString commandString;
    bool isRActive = false;

    #ifdef _WIN32

    QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\R-Core\\R", QSettings::NativeFormat);

    if (settings.contains("InstallPath"))
    {
        QString mKey = "\"" + settings.value("InstallPath").toString() + "\\bin\\Rscript.exe\"";
        mKey.replace("\\", "/");
        commandString = mKey;

        isRActive = true;
    }

    #elif TARGET_OS_MAC

    if(QFile::exists("/usr/local/bin/Rscript"))
    {
        isRActive = true;

        commandString = "/usr/local/bin/Rscript";
    }

    #endif

    SheetWidget mNewSheet(isRActive, commandString);
    mNewSheet.setWindowIcon(QPixmap(":/images/applications-other.png"));
    mNewSheet.show();

    return app.exec();
}
