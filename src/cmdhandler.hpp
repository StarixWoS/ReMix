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
    QSharedPointer<Server> server;
    QTimer* shutdownTimer{ nullptr };

    static QHash<QSharedPointer<Server>, CmdHandler*> cmdInstanceMap;
    static const QMap<TimePeriods, QString> tPeriods;

    public:
        explicit CmdHandler(QSharedPointer<Server> svr, QObject* parent = nullptr);
        ~CmdHandler() override;

        static CmdHandler* getInstance(QSharedPointer<Server> server);
        static void deleteInstance(QSharedPointer<Server> server);

        bool canUseAdminCommands(QSharedPointer<Player> admin, const GMRanks rank, const QString& cmdStr);
        void parseMix5Command(QSharedPointer<Player> plr, const QString& packet);

        static bool canParseCommand(QSharedPointer<Player> admin, const QString& command);
        bool parseCommandImpl(QSharedPointer<Player> admin, QString& packet);

        bool canIssueAction(QSharedPointer<Player> admin, QSharedPointer<Player> target, const QString& arg1, const GMCmds& argIndex, const bool& all);
        bool isTargetingSelf(QSharedPointer<Player> admin, QSharedPointer<Player> target);

        void cannotIssueAction(QSharedPointer<Player> admin, QSharedPointer<Player> target, const GMCmds& argIndex, const bool& isAll = false);

        bool isTarget(QSharedPointer<Player> target, const QString& arg1, const bool isAll = false);

    private:
        bool validateAdmin(QSharedPointer<Player> admin, GMRanks& rank, const QString& cmdStr);
        GMRanks getAdminRank(QSharedPointer<Player> admin);

        void motdHandler(QSharedPointer<Player> admin, const QString& subCmd, const QString& arg1, const QString& msg);

        void banHandler(QSharedPointer<Player> admin, const QString& arg1, const QString& duration, const QString& reason, const bool& all);
        void unBanHandler(const QString& subCmd, const QString& arg1);

        void kickHandler(QSharedPointer<Player> admin, const QString& arg1, const GMCmds& argIndex, const QString& message, const bool& all);
        void muteHandler(QSharedPointer<Player> admin, const QString& arg1, const QString& duration, const QString& reason, const bool& all);
        void unMuteHandler(QSharedPointer<Player> admin, const QString& subCmd, const QString& arg1);

        void msgHandler(const QString& arg1, const QString& message, const bool& all);

        void loginHandler(QSharedPointer<Player> admin, const QString& subCmd);
        void registerHandler(QSharedPointer<Player> admin, const QString& subCmd);

        void shutDownHandler(QSharedPointer<Player> admin, const QString& duration, const QString& reason, bool& stop, bool& restart);

        void vanishHandler(QSharedPointer<Player> admin, const QString& subCmd);

        void campHandler(QSharedPointer<Player> admin, const QString& serNum, const QString& subCmd, const GMCmds& index, const bool& soulSubCmd);

        void parseTimeArgs(const QString& str, QString& timeArg, QString& reason);
        QPair<qint64, TimePeriods> getTimePeriodFromString(const QString& str);

    signals:
        void newUserCommentSignal(const QString& sernum, const QString& alias, const QString& message);

        void insertLogSignal(const QString& source, const QString& message, const LKeys& type, const bool& logToFile, const bool& newLine) const;
};

#endif // CMDHANDLER_HPP
