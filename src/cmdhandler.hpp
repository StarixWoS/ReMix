#ifndef CMDHANDLER_HPP
#define CMDHANDLER_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QObject>

class CmdHandler : public QObject
{
    Q_OBJECT

    ServerInfo* server{ nullptr };
    User* user{ nullptr };
    RandDev* randDev{ nullptr };

    static const QString commands[ ADMIN_COMMAND_COUNT ];

    public:
        enum Ranks{ USER = 0, GMASTER = 1, COADMIN = 2, ADMIN = 3, OWNER = 4 };
        enum CMDS{ BAN = 0, UNBAN = 1, KICK = 2, MUTE = 3, UNMUTE = 4, MSG = 5,
                   LOGIN = 6, REGISTER = 7, SHUTDOWN = 8, RESTART = 9,
                   MKADMIN = 10, RMADMIN = 11, CHADMIN = 12, CHRULES = 13,
                   GETCOMMENTS = 14, CHSETTINGS = 15, VANISH = 16 };

        explicit CmdHandler(QObject* parent = nullptr, ServerInfo* svr = nullptr,
                            User* uDlg = nullptr);
        ~CmdHandler();

        bool canUseAdminCommands(Player* plr);
        void parseMix5Command(Player* plr, QString& packet);
        void parseMix6Command(Player* plr, QString& packet);
        bool parseCommandImpl(Player* plr, QString& packet);

    private:
        void banhandler(Player* plr, QString& arg1, QString& message, bool all);

        void unBanhandler(QString& argType, QString& arg1);
        void kickHandler(QString& arg1, QString& message, bool all);
        void muteHandler(QString& arg1, qint32 argIndex, bool all);
        void msgHandler(QString& arg1, QString& message, bool all);

        void loginHandler(Player* plr, QString& argType);
        void registerHandler(Player* plr, QString& argType);

        void shutDownHandler(Player* plr, bool restart = false);

//        void mkAdminHandler(Player* plr, QString& sernum, QString& arg);
//        void rmAdminHandler(Player* plr, QString& sernum);
//        void chAdminHandler(Player* plr, QString& sernum, QString& arg);

//        void chRulesHandler(Player* plr, QString& rules);
//        void getCommentsHandler(Player* plr, QString& arg);
//        void chSettingsHandler(Player* plr, QString& setting, QString& value);
//        void vanishHandler(Player* plr);

    signals:
        void newUserCommentSignal(QString& sernum, QString& alias,
                                  QString& message);
    public slots:
};

#endif // CMDHANDLER_HPP
