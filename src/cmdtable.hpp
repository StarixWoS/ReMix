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
        typedef struct CmdStructure
        {
            QString command;
            QString subCommands[ GM_SUBCOMMAND_COUNT ];
            qint32 subCommandCount;
            QString commandInfo;
            QString commandSyntax;
            GMRanks commandRank;
            bool commandIsActive;
        }CmdStruct;

        static const CmdStruct cmdTable[ GM_COMMAND_COUNT ];

        explicit CmdTable(){ }
        ~CmdTable(){ }

        static CmdTable* getInstance();
        bool cmdIsActive(const GMCmds& index);
        bool isSubCommand(const GMCmds& index, const QString& cmd);
        bool getCmdHasSubCmd(const GMCmds& index);

        QString getCmdName(const GMCmds& index);
        GMCmds getCmdIndex(const QString& cmd);
        GMRanks getCmdRank(const GMCmds& index);
        QString collateCmdList(const GMRanks& rank);
        QString getCommandInfo(const GMCmds& index, const bool& syntax);
};

#endif // CMDTABLE_HPP
