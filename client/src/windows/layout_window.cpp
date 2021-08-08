#include "layout_window.h"
#include "./ui_layout_window.h"

#include <QEvent>
#include <QBitmap>
#include <QPushButton>
#include <QString>
#include <QListWidget>
#include <QSettings>
#include <QFile>

LayoutWindow::LayoutWindow(QWidget* parent) : ui(new Ui::LayoutWindow)
{
    ui->setupUi(this);
    main_window = dynamic_cast<MainWindow*>(parent);

    QStringList names = main_window->GetDockManager()->perspectiveNames();

    for(auto n : names)
    {
        if(n != "Default") ui->listWidget->addItem(n);
        perspectives.push_back(n);
    }

    for(int i = 0; i < perspectives.size(); i++)
    {
        auto item = ui->listWidget->item(i);

        QPixmap pixmap(15, 15);
        pixmap.fill(QColor("black"));
        item->setIcon(QIcon(pixmap));
    }

    (void)connect(ui->pushButton, &QPushButton::clicked, this, [=]() {
        QString t = ui->lineEdit->text();

        if(!t.isEmpty() && std::find(perspectives.begin(), perspectives.end(), t) == perspectives.end())
        {
            LOG_DEBUG("Adding perspective: %s", t.toStdString().c_str());
            ui->listWidget->addItem(t);
            perspectives.push_back(t);
            main_window->GetDockManager()->addPerspective(t);

            ui->lineEdit->clear();

            QPixmap pixmap(15, 15);
            pixmap.fill(QColor("black"));
            ui->listWidget->item((int)perspectives.size() - 1)->setIcon(QIcon(pixmap));

            QFile("ads_perspectives.ini").remove(); // Enforce a clean file
            main_window->GetDockManager()->savePerspectives(QSettings("ads_perspectives.ini", QSettings::Format::IniFormat));
            main_window->UpdatePerspectives(t);
        }
        else
        {
            LOG_ERROR("Cannot save an unnamed or existing perspective.");
        }
    });

    (void)connect(main_window->GetDockManager(), &ads::CDockManager::perspectiveOpened, this, [=](const QString& name) {
        auto pit = std::find(perspectives.begin(), perspectives.end(), name);

        for(int i = 0; i < perspectives.size(); i++)
        {
            auto item = ui->listWidget->item(i);

            QPixmap pixmap(15, 15);
            pixmap.fill(QColor("black"));
            item->setIcon(QIcon(pixmap));
        }

        if(pit != perspectives.end())
        {
            auto item = ui->listWidget->item(pit - perspectives.begin());

            QPixmap pixmap(15, 15);
            pixmap.fill(QColor("lime"));
            item->setIcon(QIcon(pixmap));
        }
    });

    (void)connect(ui->listWidget, &QListWidget::itemDoubleClicked, this, [=](QListWidgetItem* item) {
        main_window->GetDockManager()->openPerspective(item->text());
    });

    (void)connect(ui->listWidget, &QListWidget::currentItemChanged, this, [=](QListWidgetItem* now, QListWidgetItem* then) {
        ui->pushButton_2->setEnabled(now->text() != "Default");
    });

    (void)connect(ui->pushButton_2, &QPushButton::clicked, this, [=]() {
        // if(!ui->listWidget->selectedItems().isEmpty())
        // {
        LOG_DEBUG("Removing perspective: %s", ui->listWidget->selectedItems()[0]->text().toStdString().c_str());
        auto pit = std::find(perspectives.begin(), perspectives.end(), ui->listWidget->selectedItems()[0]->text());
        auto sval = ui->listWidget->selectedItems()[0]->text();

        if(pit != perspectives.end())
        {
            main_window->GetDockManager()->removePerspective(sval);
            main_window->RemovePerspective(sval);
            perspectives.erase(pit);

            for(auto p : main_window->GetDockManager()->perspectiveNames())
            {
                LOG_DEBUG("Left %s", p.toStdString().c_str());
            }

            delete ui->listWidget->selectedItems()[0];
            QFile("ads_perspectives.ini").remove(); // Enforce a clean file
            main_window->GetDockManager()->savePerspectives(QSettings("ads_perspectives.ini", QSettings::Format::IniFormat));
        }

        // }
    });
}

LayoutWindow::~LayoutWindow()
{
    
}
