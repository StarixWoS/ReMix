#ifndef CMDTABLE_HPP
#define CMDTABLE_HPP

#include "prototypes.hpp"

//Required Qt Includes.
#include <QObject>
#include <QVariant>

class CmdTable
{
    static CmdTable* instance;

    public:
        struct CmdStructure
        {
            QStringList cmdActivators;
            qint32 cmdActivatorCount;
            QStringList subCmd;
            qint32 subCmdCount;
            QString cmdInfo;
            QString cmdSyntax;
            GMRanks cmdRank;
            bool cmdIsActive;
            GMCmds index;
        };
        using CmdStruct = CmdStructure;
        static const QVector<CmdStruct> cmdTable;

        explicit CmdTable() = default;
        ~CmdTable() = default;

        static CmdTable* getInstance();
        bool cmdIsActive(const GMCmds& index);
        bool isSubCommand(const GMCmds& index, const QString& cmd, const bool& time = false);
        bool getCmdHasSubCmd(const GMCmds& index);

        QStringList getCmdName(const GMCmds& index);
        GMCmds getCmdIndex(const QString& cmd);
        GMSubCmds getSubCmdIndex(const GMCmds& cmdIndex, const QString& subCmd, const bool& time = false);
        GMRanks getCmdRank(const GMCmds& index);
        QString getCmdString(const CmdTable::CmdStruct& cmdStruct, const GMRanks rank, const bool isAuth);
        QString collateCmdStrings(const QSharedPointer<Player> admin);
        QString getCommandInfo(const GMCmds& index, const bool& syntax);
};

#endif // CMDTABLE_HPP
