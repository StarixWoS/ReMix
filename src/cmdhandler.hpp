#ifndef CMDHANDLER_HPP
#define CMDHANDLER_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QObject>

class CmdHandler : public QObject
{
    Q_OBJECT

    ServerInfo* server{ nullptr };
    RandDev* randDev{ nullptr };

    static const QString commands[ ADMIN_COMMAND_COUNT ];

    public:
        enum Ranks{ USER = 0, GMASTER, COADMIN, ADMIN, OWNER = 4 };
        enum CMDS{ BAN = 0, UNBAN, KICK, MUTE, UNMUTE, MSG, LOGIN, REGISTER,
                   SHUTDOWN, RESTART,  MKADMIN, RMADMIN, CHADMIN, CHRULES,
                   GETCOMMENTS, CHSETTINGS, VANISH, VERSION = 17 };

        explicit CmdHandler(QObject* parent = nullptr,
                            ServerInfo* svr = nullptr);
        ~CmdHandler();

        bool canUseAdminCommands(Player* plr);
        void parseMix5Command(Player* plr, QString& packet);
        void parseMix6Command(Player* plr, QString& packet);
        bool parseCommandImpl(Player* plr, QString& packet);

    private:
        void banhandler(Player* plr, QString& arg1, QString& message, bool all);

        void unBanhandler(QString& argType, QString& arg1);
        void kickHandler(QString& arg1, QString& message, bool all);
        void muteHandler(Player* plr, QString& arg1, qint32 argIndex,
                         QString& message, bool all);
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
