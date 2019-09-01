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
            QString cmd;
            QStringList subCmd;
            qint32 subCmdCount;
            QStringList timeSubCmd;
            qint32 timeSubCmdCount;
            QString cmdInfo;
            QString cmdSyntax;
            GMRanks cmdRank;
            bool cmdIsActive;
        }CmdStruct;

        static const CmdStruct cmdTable[ GM_COMMAND_COUNT ];

        explicit CmdTable(){ }
        ~CmdTable(){ }

        static CmdTable* getInstance();
        bool cmdIsActive(const GMCmds& index);
        bool isSubCommand(const GMCmds& index, const QString& cmd,
                          const bool& time = false);
        bool getCmdHasSubCmd(const GMCmds& index);

        QString getCmdName(const GMCmds& index);
        GMCmds getCmdIndex(const QString& cmd);
        qint32 getSubCmdIndex(const GMCmds& cmdIndex, const QString& subCmd,
                              const bool& time = false);
        GMRanks getCmdRank(const GMCmds& index);
        QString collateCmdList(const GMRanks& rank);
        QString getCommandInfo(const GMCmds& index, const bool& syntax);
};

#endif // CMDTABLE_HPP
