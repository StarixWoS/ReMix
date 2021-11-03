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
        static QStringList restartServerList;
        static CreateInstance* instance;
        QCollator collator;
        bool loadingOldServers{ false };

    public:
        explicit CreateInstance(QWidget* parent = nullptr);
        ~CreateInstance() override;

        static CreateInstance* getInstance(QWidget* parent = nullptr);

        void updateServerList(const bool& firstRun);
        bool testPort(const quint16& port);
        quint16 genPort();

        void restartServer(const QString& name, const QString& gameName, const QString& netInterface, const quint16& port,
                           const bool& useUPNP, const bool& isPublic);
        QSharedPointer<Server> loadOldServer(const QString& name);
        QSharedPointer<Server> initializeServer(const QString& name, const QString& gameName, const QString& netInterface, const quint16& port,
                                                const bool& useUPNP, const bool& isPublic);

        bool getLoadingOldServers() const;
        void setLoadingOldServers(bool value);

    private slots:
        void on_servers_currentTextChanged(const QString& arg1);
        void on_portNumber_textChanged(const QString& arg1);
        void on_servers_currentIndexChanged(int);
        void closeEvent(QCloseEvent* event) override;
        void on_initializeServer_clicked();
        void showEvent(QShowEvent* event) override;
        void on_close_clicked();
        void on_randomizePort_clicked();
        void on_deleteServer_clicked();

        void on_useUPNP_clicked();

    signals:
        void getMasterMixInfoSignal(const Games& game);
        void createServerAcceptedSignal(QSharedPointer<Server> server);
        void closeServerSignal();
        void restartServerListSignal(const QStringList& restartList);
        void quitSignal();

    private:
        Ui::CreateInstance* ui;
        mutable QMutex mutex;
};

#endif // CREATEINSTANCE_H
