#ifndef CREATEINSTANCE_H
#define CREATEINSTANCE_H

#include "prototypes.hpp"

//Required Qt Includes.
#include <QCollator>
#include <QDialog>
#include <QMutex>

namespace Ui {
    class CreateInstance;
}

class CreateInstance : public QDialog
{
        Q_OBJECT

        static const QStringList gameNames;
        QCollator collator;

    public:
        explicit CreateInstance(QWidget* parent = nullptr);
        ~CreateInstance() override;

        void updateServerList(const bool& firstRun);
        bool testPort(const quint16& port);
        quint16 genPort();

        void restartServer(const QString& name, const QString& gameName,
                           const quint16& port, const bool& useUPNP,
                           const bool& isPublic);

    private slots:
        void on_servers_currentTextChanged(const QString& arg1);
        void on_portNumber_textChanged(const QString& arg1);
        void on_servers_currentIndexChanged(int);
        void closeEvent(QCloseEvent* event) override;
        void on_initializeServer_clicked();
        void showEvent(QShowEvent* event) override;
        void on_close_clicked();

        void on_randomizePort_clicked();

    signals:
        void createServerAcceptedSignal(ServerInfo* server);
        void closeServerSignal();

    private:
        Ui::CreateInstance* ui;
        mutable QMutex mutex;
};

#endif // CREATEINSTANCE_H
