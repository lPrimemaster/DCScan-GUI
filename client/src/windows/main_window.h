#pragma once

#include <QMainWindow>
#include <QTextBrowser>
#include <QMap>

#include <qtadvanceddocking/DockManager.h>

#include <DCS_Core/include/DCS_ModuleCore.h>
#include <DCS_EngineControl/include/DCS_ModuleEngineControl.h>
#include <DCS_Utils/include/DCS_ModuleUtils.h>
#include <DCS_Network/include/DCS_ModuleNetwork.h>

class QString;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QApplication* app, QWidget *parent = nullptr);
    ~MainWindow();

    inline ads::CDockManager* GetDockManager()
    {
        return dock_manager;
    }

    inline QApplication* GetApp()
    {
        return app;
    }

    template<typename W>
    inline W* GetWindow(const QString& name)
    {
        auto it = windows.find(name);
        if(it == windows.end())
        {
            LOG_ERROR("Could not find window named %s.", name.toLatin1().toStdString().c_str());
            return nullptr;
        }
        return dynamic_cast<W*>(it.value());
    }

private:
    void AddGenericWindow(const QString& title, QWidget* window, const QIcon& icon, const QString& menu, const ads::DockWidgetArea area);
    void AddMenu(const QString& name);

private:
    ads::CDockManager* dock_manager;
	QStatusBar* status_bar;
    QMap<QString, QMenu*> menus;

    QMap<QString, QWidget*> windows;

    QApplication* app;
};
