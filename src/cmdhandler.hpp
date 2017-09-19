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

        bool canUseAdminCommands(Player* plr) const;
        void parseMix5Command(Player* plr, const QString& packet);
        void parseMix6Command(Player* plr, const QString& packet);
        bool parseCommandImpl(Player* plr, QString& packet);

    private:
        void banhandler(Player* plr, const QString& arg1,
                        const QString& message, const bool& all);

        void unBanhandler(const QString& argType, const QString& arg1);
        void kickHandler(const QString& arg1, const QString& message,
                         const bool& all);
        void muteHandler(Player* plr, const QString& arg1,
                         const qint32& argIndex, const QString& message,
                         const bool& all);
        void msgHandler(const QString& arg1, const QString& message,
                        const bool& all);

        void loginHandler(Player* plr, const QString& argType);
        void registerHandler(Player* plr, const QString& argType);

        void shutDownHandler(Player* plr, const bool restart = false);

//        void mkAdminHandler(Player* plr, QString& sernum, QString& arg);
//        void rmAdminHandler(Player* plr, QString& sernum);
//        void chAdminHandler(Player* plr, QString& sernum, QString& arg);

//        void chRulesHandler(Player* plr, QString& rules);
//        void getCommentsHandler(Player* plr, QString& arg);
//        void chSettingsHandler(Player* plr, QString& setting, QString& value);
//        void vanishHandler(Player* plr);

    signals:
        void newUserCommentSignal(const QString& sernum, const QString& alias,
                                  const QString& message);
    public slots:
};

#endif // CMDHANDLER_HPP
