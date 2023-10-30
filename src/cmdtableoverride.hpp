#ifndef CMDTABLEOVERRIDE_HPP
#define CMDTABLEOVERRIDE_HPP

#include "prototypes.hpp"

//Required Qt Includes..
#include <QModelIndex>
#include <QDialog>
#include <QMutex>
#include <QIcon>

#include <QTreeWidgetItem>
#include <QTreeWidget>

namespace Ui {
    class CmdTableOverride;
}

class CmdTableOverride : public QDialog
{
    Q_OBJECT

    static CmdTableOverride* instance;
    static const QMap<GMCmds, QString> commands;
    static const QMap<GMSubCmds, QString> subCommands;

    static bool usingOverrides;

    public:
        explicit CmdTableOverride(QWidget *parent = nullptr);
        ~CmdTableOverride();

        static CmdTableOverride* getInstance();
        static void setInstance(CmdTableOverride* overrides);

        static GMRanks getOverride(const GMCmds& cmd, const GMSubCmds& subCmd);
        static bool getUsingOverrides();

    private:
        void handleValues(QTreeWidget* tree, const bool& save = true);

        void setOverride(const QString& path, const QVariant& value);
        static QVariant getOverrideFromPath(const QString& path);

        void itemChangedSlot(QTreeWidgetItem *item, int column);

    private:
        Ui::CmdTableOverride *ui;
};

#endif // CMDTABLEOVERRIDE_HPP
