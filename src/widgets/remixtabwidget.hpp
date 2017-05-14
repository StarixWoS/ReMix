
#ifndef REMIXTABWIDGET_H
#define REMIXTABWIDGET_H

#include "prototypes.hpp"

//Required Qt Includes..
#include <QTabWidget>
#include <QObject>

class ReMixTabWidget : public QTabWidget
{
    Q_OBJECT

    static ReMixTabWidget* tabInstance;

    QToolButton* nightModeButton{ nullptr };
    QToolButton* newTabButton{ nullptr };

    ReMixWidget* servers[ MAX_SERVER_COUNT ]{ nullptr };
    User* user{ nullptr };

    static qint32 instanceCount;
    quint32 prevTabIndex{ 0 };

    QPalette defaultPalette;
    bool nightMode{ false };

    public:
        explicit ReMixTabWidget(QWidget *parent = 0);
        ~ReMixTabWidget();

        void sendMultiServerMessage(QString msg, Player* plr, bool toAll);
        quint32 getPlayerCount();
        quint32 getServerCount();

        quint32 getPrevTabIndex() const;
        void setPrevTabIndex(const quint32& value);

        static qint32 getInstanceCount();
        static ReMixTabWidget* getTabInstance(QWidget* parent = nullptr);

    private:
        void createTabButtons();
        void createServer();
        void applyThemes(qint32 type);

    private slots:
        void tabCloseRequestedSlot(quint32 index);
        void currentChangedSlot(quint32 newTab);
};

#endif // REMIXTABWIDGET_H
