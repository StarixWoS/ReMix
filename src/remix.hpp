
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

    ReMixTabWidget* serverInstance{ nullptr };
    ServerInfo* server{ nullptr };
    RandDev* randDev{ nullptr };
    User* user{ nullptr };

    QSystemTrayIcon* trayObject{ nullptr };
    QMenu* trayMenu{ nullptr };
    QIcon trayIcon;

    bool hasSysTray{ false };

    QString serverID{ "0" };

    private:
    #if !defined( Q_OS_LINUX ) && !defined( Q_OS_OSX )
        void initSysTray();
    #endif
        void initUIUpdate();

    public:
        explicit ReMix(QWidget *parent = 0);
        ~ReMix();

        static void getSynRealData(ServerInfo* svr);

    private slots:
        //Handle Minimize events.
        #if !defined( Q_OS_LINUX ) && !defined( Q_OS_OSX )
            void changeEvent(QEvent* event);
        #endif

        void closeEvent(QCloseEvent* event);
        bool rejectCloseEvent();

    private:
        Ui::ReMix *ui;
};

#endif // REMIX_HPP
