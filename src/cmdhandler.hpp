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
    QTimer* shutdownTimer{ nullptr };

    public:
        explicit CmdHandler(QObject* parent = nullptr, ServerInfo* svr = nullptr);
        ~CmdHandler() override;

        bool canUseAdminCommands(Player* plr, const GMRanks rank, const QString& cmdStr);
        void parseMix5Command(Player* plr, const QString& packet);
        void parseMix6Command(Player* plr, const QString& packet);
        bool parseCommandImpl(Player* plr, QString& packet);

        bool canIssueAction(Player* admin, Player* target, const QString& arg1, const GMCmds& argIndex, const bool& all);
        void cannotIssueAction(Player* admin, const QString& arg1, const GMCmds& argIndex);

        bool isTarget(Player* target, const QString& arg1, const bool isAll = false);

    private:
        bool validateAdmin(Player* plr, GMRanks& rank, const QString& cmdStr);
        GMRanks getAdminRank(Player* plr);

        void motdHandler(Player* plr, const QString& subCmd, const QString& arg1, const QString& msg);

        void banHandler(Player* plr, const QString& arg1, const QString& duration, const QString& reason, const bool& all);
        void unBanHandler(const QString& subCmd, const QString& arg1);

        void kickHandler(Player* plr, const QString& arg1, const GMCmds& argIndex, const QString& message, const bool& all);
        void muteHandler(Player* plr, const QString& arg1, const QString& duration, const QString& reason, const bool& all);
        void unMuteHandler(const QString& subCmd, const QString& arg1);

        void msgHandler(const QString& arg1, const QString& message, const bool& all);

        void loginHandler(Player* plr, const QString& subCmd);
        void registerHandler(Player* plr, const QString& subCmd);

        void shutDownHandler(Player* plr, const QString& duration, const QString& reason, bool& stop, bool& restart);

        void vanishHandler(Player* plr, const QString& subCmd);

        void campHandler(Player* plr, const QString& serNum, const QString& subCmd, const GMCmds& index, const bool& soulSubCmd);

        void parseTimeArgs(const QString& str, QString& timeArg, QString& reason);
        qint32 getTimePeriodFromString(const QString& str, QString& timeTxt);

    signals:
        void newUserCommentSignal(const QString& sernum, const QString& alias, const QString& message);

        void insertLogSignal(const QString& source, const QString& message, const LogTypes& type, const bool& logToFile, const bool& newLine) const;
};

#endif // CMDHANDLER_HPP
