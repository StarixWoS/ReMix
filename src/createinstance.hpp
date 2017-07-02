#ifndef CREATEINSTANCE_H
#define CREATEINSTANCE_H

#include "prototypes.hpp"

//Required Qt Includes.
#include <QDialog>

namespace Ui {
    class CreateInstance;
}

class CreateInstance : public QDialog
{
        Q_OBJECT

        RandDev* randDev{ nullptr };
        QString serverArgs{ "" };

        static const QString gameNames[ GAME_NAME_COUNT ];

    public:
        explicit CreateInstance(QWidget *parent = 0);
        ~CreateInstance();

        void updateServerList(bool firstRun);
        QString getServerArgs() const;
        QString getServerName() const;
        void setServerArgs(const QString& value);

    private slots:
        void on_initializeServer_clicked();
        void on_close_clicked();

        quint16 genPort();
        bool testPort(quint16 port);

        void closeEvent(QCloseEvent* event);
        void showEvent(QShowEvent* event);

        void on_oldServers_currentIndexChanged(int);

    signals:
        void closeServer();

    private slots:
        void on_portNumber_textChanged(const QString &arg1);

    private:
        Ui::CreateInstance *ui;
};

#endif // CREATEINSTANCE_H
