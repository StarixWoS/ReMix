
#ifndef REMIXTABWIDGET_H
#define REMIXTABWIDGET_H

#include "prototypes.hpp"

//Required Qt Includes..
#include <QTabWidget>
#include <QObject>

class ReMixTabWidget : public QTabWidget
{
    Q_OBJECT

    QToolButton* newTabButton{ nullptr };

    ReMixWidget* servers[ MAX_SERVER_COUNT ]{ nullptr };
    User* user{ nullptr };

    quint32 prevTabIndex{ 0 };

    public:
        explicit ReMixTabWidget(QWidget *parent = 0, User* usr = nullptr,
                                QStringList* argList = nullptr);

        void sendMultiServerMessage(QString msg, Player* plr, bool toAll);
        void connectNameChange(quint32 id);
        quint32 getPlayerCount();
        quint32 getServerCount();

        quint32 getPrevTabIndex() const;
        void setPrevTabIndex(const quint32& value);

    signals:

    private slots:
        void newTab();
};

#endif // REMIXTABWIDGET_H
