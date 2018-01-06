#ifndef CMDHANDLER_HPP
#define CMDHANDLER_HPP

#include "prototypes.hpp"
#include "cmdtable.hpp"

//Required Qt Includes.
#include <QObject>

class CmdHandler : public QObject
{
    Q_OBJECT

    CmdTable* cmdTable{ nullptr };
    ServerInfo* server{ nullptr };

    public:
        explicit CmdHandler(QObject* parent = nullptr,
                            ServerInfo* svr = nullptr);
        ~CmdHandler();

        bool canUseAdminCommands(Player* plr) const;
        void parseMix5Command(Player* plr, const QString& packet);
        void parseMix6Command(Player* plr, const QString& packet);
        bool parseCommandImpl(Player* plr, QString& packet);

        bool canIssueAction(Player* admin, Player* target, const QString& arg1,
                            const GMCmds& argIndex, const bool& all);
        void cannotIssueAction(Player* admin, Player* target,
                               const GMCmds& argIndex);

    private:
        bool validateAdmin(Player* plr, const GMRanks& rank);
        GMRanks getAdminRank(Player* plr);

        void motdHandler(Player* plr, const QString& subCmd,
                         const QString& arg1,
                         const QString& msg);

        void banhandler(Player* plr, const QString& arg1,
                        const GMCmds& argIndex, const QString& message,
                        const bool& all);

        void unBanhandler(const QString& subCmd, const QString& arg1);
        void kickHandler(Player* plr, const QString& arg1,
                         const GMCmds& argIndex, const QString& message,
                         const bool& all);
        void muteHandler(Player* plr, const QString& arg1,
                         const GMCmds& argIndex, const QString& message,
                         const bool& all);
        void msgHandler(const QString& arg1, const QString& message,
                        const bool& all);

        void loginHandler(Player* plr, const QString& subCmd);
        void registerHandler(Player* plr, const QString& subCmd);

        void shutDownHandler(Player* plr, const GMCmds& index,
                             const QString& subCmd, const QString& arg1,
                             const QString& msg, const bool& restart = false);

//        void mkAdminHandler(Player* plr, QString& sernum, QString& arg);
//        void rmAdminHandler(Player* plr, QString& sernum);
//        void chAdminHandler(Player* plr, QString& sernum, QString& arg);

        void chRulesHandler(Player* plr, const QString& rule,
                            const QVariant& value);
//        void getCommentsHandler(Player* plr, QString& arg);
//        void chSettingsHandler(Player* plr, QString& setting, QString& value);
        void vanishHandler(Player* plr, const QString& subCmd);

    signals:
        void newUserCommentSignal(const QString& sernum, const QString& alias,
                                  const QString& message);
    public slots:
};

#endif // CMDHANDLER_HPP
