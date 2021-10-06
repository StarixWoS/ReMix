
#ifndef REMIX_HPP
#define REMIX_HPP

#include "prototypes.hpp"

//Required Qt Includes..
#include <QMainWindow>
#include <QModelIndex>

namespace Ui {
    class ReMix;
}

class ReMix : public QMainWindow
{
    Q_OBJECT

    static ReMix* instance;

    QSystemTrayIcon* trayObject{ nullptr };
    ReMixTabWidget* serverUI{ nullptr };
    QMenu* trayMenu{ nullptr };
    QIcon trayIcon;

    bool hasSysTray{ false };
    bool exiting{ false };

    private:
    #if !defined( Q_OS_LINUX ) && !defined( Q_OS_OSX )
        void initSysTray();
    #endif
        void initUIUpdate();

    public:
        explicit ReMix(QWidget* parent = nullptr);
        ~ReMix() override;

        static ReMix* getInstance();
        static void updateTitleBars(Server* server);

    private slots:
        //Handle Minimize events.
        #if !defined( Q_OS_LINUX ) && !defined( Q_OS_OSX )
            void changeEvent(QEvent* event) override;
        #endif

        void closeEvent(QCloseEvent* event) override;
        bool rejectCloseEvent();

    private:
        Ui::ReMix* ui;
};

#endif // REMIX_HPP
