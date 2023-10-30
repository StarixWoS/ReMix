#ifndef CMDTABLE_HPP
#define CMDTABLE_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QVariant>
#include <QObject>

class CmdTable
{
    static CmdTable* instance;

    struct SubCommandStructure
    {
        QString subCommand;
        QString subInfo;
        QString subSyntax;
        GMRanks subRank;
        GMSubCmds index;
    };

    struct CmdStructure
    {
        QStringList cmdActivators;
        qint32 cmdActivatorCount;
        QVector<SubCommandStructure> subCmd;
        qint32 subCmdCount;
        QString cmdInfo;
        QString cmdSyntax;
        GMRanks cmdRank;
        bool cmdIsActive;
        GMCmds index;
    };
    static const QVector<CmdStructure> cmdTable;

    public:

        explicit CmdTable() = default;
        ~CmdTable() = default;

        static CmdTable* getInstance();
        bool cmdIsActive(const GMCmds& index);
        bool isSubCommand(const GMCmds& index, const QString& cmd, const bool& time = false);
        bool getCmdHasSubCmd(const GMCmds& index);

        QString getCmdNames(const GMCmds& index);
        GMCmds getCmdIndex(const QString& cmd);
        GMSubCmdIndexes getSubCmdIndex(const GMCmds& cmdIndex, const QString& subCmd, const bool& time = false);
        GMRanks getCmdRank(const GMCmds& index);
        GMRanks getSubCmdRank(const GMCmds& index, const GMSubCmdIndexes& subIndex);
        QString getCmdString(const CmdTable::CmdStructure& cmdStruct, const GMRanks rank, const bool isAuth);
        QString collateCmdStrings(const QSharedPointer<Player> admin);
        QString getCommandInfo(const GMCmds& index, const GMSubCmdIndexes& subIndex, const bool& syntax);
};

#endif // CMDTABLE_HPP
