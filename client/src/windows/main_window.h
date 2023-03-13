#pragma once

#include <QMainWindow>
#include <QTextBrowser>
#include <QMap>
#include <QStatusBar>
#include <QLabel>
#include <QAction>

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

    // TODO: This should instead be on a "MyWindow" child class
    inline ads::CDockWidget* GetDock(const QString& name)
    {
        auto it = docks.find(name);
        if(it == docks.end())
        {
            LOG_ERROR("Could not find dock named %s.", name.toLatin1().toStdString().c_str());
            return nullptr;
        }
        return it.value();
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

    inline void AddStatusWidget(const QString& name, QWidget* w, bool atFront = true)
    {
        status_widgets.insert(name, w);
        if(atFront) status_widgets_loadOrder.push_front(w);
        else status_widgets_loadOrder.push_back(w);
    }

    inline void AdjustStatus()
    {
        status_bar->adjustSize();
    }

    inline void UpdatePerspectives(QString name = "")
    {
        if(name == "")
        {
            for(auto p : dock_manager->perspectiveNames())
            {
                QAction* a = new QAction(p, this);
                (void)connect(a, &QAction::triggered, this, [=](){ dock_manager->openPerspective(p); });
                AddGenericAction(a, QIcon(), "Layout");
            }
        }
        else
        {
            QAction* a = new QAction(name, this);
            (void)connect(a, &QAction::triggered, this, [=](){ dock_manager->openPerspective(name); });
            InsertLayout(a, QIcon(), "Layout");
        }
    }

    inline void RemovePerspective(QString name)
    {
        RemoveLayout(name, "Layout");
    }

    void SetSubWIcon(const QString& title, QIcon icon);

private:
    void AddSeparator(const QString& menu);
    void AddGenericAction(QAction* action, const QIcon& icon, const QString& menu);
    void InsertLayout(QAction* action, const QIcon& icon, const QString& menu);
    void RemoveLayout(const QString& action, const QString& menu);
    void AddGenericWindow(const QString& title, QWidget* window, const QIcon& icon, const QString& menu, const ads::DockWidgetArea area);
    void AddMenu(const QString& name);
    void UpdateCPULoad();

    inline void LoadStatusWidgetsOrdered()
    {
        for(auto w : status_widgets_loadOrder)
        {
            status_bar->addPermanentWidget(w);
        }
        status_widgets_loadOrder.clear();
    }


private:
    ads::CDockManager* dock_manager;
	QStatusBar* status_bar;
    QMap<QString, QMenu*> menus;

    QMap<QString, QWidget*> windows;
    QMap<QString, ads::CDockWidget*> docks;

    QMap<QString, QWidget*> status_widgets;
    QVector<QWidget*> status_widgets_loadOrder;

    QApplication* app;
    QString lastLayout;

    QTimer* cpuLoadTimer;
    QLabel* cpuLoadText;
};
