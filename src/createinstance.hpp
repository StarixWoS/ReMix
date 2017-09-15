#ifndef CREATEINSTANCE_H
#define CREATEINSTANCE_H

#include "prototypes.hpp"

//Required Qt Includes.
#include <QCollator>
#include <QDialog>

namespace Ui {
    class CreateInstance;
}

class CreateInstance : public QDialog
{
        Q_OBJECT

        static const QString gameNames[ GAME_NAME_COUNT ];
        RandDev* randDev{ nullptr };
        QCollator collator;

    public:
        explicit CreateInstance(QWidget *parent = 0);
        ~CreateInstance();

        void updateServerList(bool firstRun);
        bool testPort(quint16 port);
        quint16 genPort();

    private slots:
        void on_servers_currentTextChanged(const QString &arg1);
        void on_portNumber_textChanged(const QString &arg1);
        void on_servers_currentIndexChanged(int);
        void closeEvent(QCloseEvent* event);
        void on_initializeServer_clicked();
        void showEvent(QShowEvent* event);
        void on_close_clicked();

    signals:
        void createServerAcceptedSignal(ServerInfo* server);
        void closeServer();

    private:
        Ui::CreateInstance *ui;
};

#endif // CREATEINSTANCE_H
