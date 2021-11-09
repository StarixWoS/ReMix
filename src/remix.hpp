
#ifndef REMIX_HPP
#define REMIX_HPP

#include "prototypes.hpp"

//Required Qt Includes..
#include <QSystemTrayIcon>
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

    public:
        explicit ReMix(QWidget* parent = nullptr);
        ~ReMix() override;

        static ReMix* getInstance();
        static void updateTitleBars(QSharedPointer<Server> server);

    public slots:
        void quitSlot();

    private slots:
        //Handle Minimize events.
        #if !defined( Q_OS_LINUX ) && !defined( Q_OS_OSX )
            void changeEvent(QEvent* event) override;
        #endif

        void closeEvent(QCloseEvent* event) override;
        bool rejectCloseEvent();
        void quitActionTriggeredSlot();
        void trayObjectActivatedSlot(QSystemTrayIcon::ActivationReason reason);

    private:
        Ui::ReMix* ui;
};

#endif // REMIX_HPP
