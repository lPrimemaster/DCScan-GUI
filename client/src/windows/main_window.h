#pragma once

#include <QMainWindow>
#include <QTextBrowser>
#include <QMap>
#include <QStatusBar>
#include <QLabel>

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

    /**
     * @brief Display a status message in the man window's status bar.
     * 
     * @param value text to display. Period terminated preferencially.
     */
    inline void IssueStatusBarText(const QString& value)
    {
        status_bar->showMessage(value);
    }

    template<typename W>
    inline W* GetStatusWidget(const QString& name)
    {
        auto it = status_widgets.find(name);
        if(it == status_widgets.end())
        {
            LOG_ERROR("Could not find status widget named %s.", name.toLatin1().toStdString().c_str());
            return nullptr;
        }
        return dynamic_cast<W*>(it.value());
    }

    inline void AddStatusWidget(const QString& name, QWidget* w)
    {
        status_widgets.insert(name, w);
        status_bar->addPermanentWidget(w);
    }

    inline void AdjustStatus()
    {
        status_bar->adjustSize();
    }

    void SetSubWIcon(const QString& title, QIcon icon);

private:
    void AddSeparator(const QString& menu);
    void AddGenericAction(QAction* action, const QIcon& icon, const QString& menu);
    void AddGenericWindow(const QString& title, QWidget* window, const QIcon& icon, const QString& menu, const ads::DockWidgetArea area);
    void AddMenu(const QString& name);


private:
    ads::CDockManager* dock_manager;
	QStatusBar* status_bar;
    QMap<QString, QMenu*> menus;

    QMap<QString, QWidget*> windows;
    QMap<QString, ads::CDockWidget*> docks;

    QMap<QString, QWidget*> status_widgets;

    QApplication* app;
};
