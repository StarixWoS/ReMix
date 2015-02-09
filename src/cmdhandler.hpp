#ifndef CMDHANDLER_HPP
#define CMDHANDLER_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QObject>

class CmdHandler : public QObject
{
    Q_OBJECT

    BanDialog* banDialog{ nullptr };
    ServerInfo* server{ nullptr };
    Admin* adminDialog{ nullptr };
    RandDev* randDev{ nullptr };

    static const QStringList commands;

    public:
        enum Ranks{ GMASTER = 0, COADMIN = 1, ADMIN = 2, OWNER = 3 };
        enum CMDS{ BAN = 0, UNBAN = 1, KICK = 2, MUTE = 3, UNMUTE = 4, MSG = 5,
                   LOGIN = 6, REGISTER = 7, SHUTDOWN = 8, RESTART = 9 };

        explicit CmdHandler(QObject* parent = nullptr, ServerInfo* svr = nullptr,
                            Admin* aDlg = nullptr);
        ~CmdHandler();

        bool canUseAdminCommands(Player* plr);
        void parseMix5Command(Player* plr, QString& packet);
        void parseMix6Command(Player* plr, QString& packet);
        bool parseCommandImpl(Player* plr, QString& packet);

    private:
        void banhandler(Player* plr, QString& argType, QString& arg1,
                        QString& message, bool all);

        void unBanhandler(QString& argType, QString& arg1);
        void kickHandler(QString& arg1, QString& message, bool all);
        void muteHandler(QString& arg1, qint32 argIndex, bool all);
        void msgHandler(QString& arg1, QString& message, bool all);

        void loginHandler(Player* plr, QString& argType);
        void registerHandler(Player* plr, QString& argType);

        void shutDownHandler(Player* plr, bool restart = false);

    signals:
        void newUserCommentSignal(QString& sernum, QString& alias,
                                  QString& message);
    public slots:
};

#endif // CMDHANDLER_HPP
