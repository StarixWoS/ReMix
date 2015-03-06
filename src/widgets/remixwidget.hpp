#ifndef REMIXWIDGET_HPP
#define REMIXWIDGET_HPP

#include "prototypes.hpp"

//Required Qt Includes..
#include <QModelIndex>
#include <QWidget>

namespace Ui {
    class ReMixWidget;
}

class ReMixWidget : public QWidget
{
    Q_OBJECT

    PlrListWidget* plrWidget{ nullptr };

    Server* tcpServer{ nullptr };
    RandDev* randDev{ nullptr };
    Settings* settings{ nullptr };

    ServerInfo* server{ nullptr };
    Admin* admin{ nullptr };

    QPalette defaultPalette;
    bool nightMode{ false };

    enum CMDLArgs{ GAME = 0, MASTER = 1, PUBLIC = 2, LISTEN = 3, NAME = 4,
                   FUDGE = 5 };
    static const QStringList cmdlArgs;

    public:
        explicit ReMixWidget(QWidget* parent = nullptr, Admin* adm = nullptr,
                             QStringList* argList = nullptr);
        ~ReMixWidget();

        void sendServerMessage(QString msg, Player* plr, bool toAll);
        qint32 getPlayerCount();
        QString getServerName() const;

    private:
        void parseCMDLArgs(QStringList* argList);
        void initUIUpdate();

        void applyThemes(QString& name);

    private slots:
        void on_openSettings_clicked();
        void on_openBanDialog_clicked();
        void on_openUserComments_clicked();
        void on_enableNetworking_clicked();
        void on_openRemoteAdmins_clicked();
        void on_isPublicServer_stateChanged(int arg1);

        void on_serverPort_textChanged(const QString &arg1);
        void on_serverName_textChanged(const QString &arg1);

        void on_nightMode_clicked();

    private:
        Ui::ReMixWidget *ui;
};

#endif // REMIXWIDGET_HPP
