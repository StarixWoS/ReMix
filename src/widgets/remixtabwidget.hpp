
#ifndef REMIXTABWIDGET_H
#define REMIXTABWIDGET_H

#include "prototypes.hpp"

//Required Qt Includes..
#include <QTabWidget>
#include <QObject>
#include <QMap>

class ReMixTabWidget : public QTabWidget
{
    Q_OBJECT

    static CreateInstance* createDialog;
    static ReMixTabWidget* tabInstance;
    static qint32 instanceCount;

    static QPalette defaultPalette;
    static QPalette customPalette;

    QToolButton* nightModeButton{ nullptr };
    QToolButton* newTabButton{ nullptr };

    QMap<int, ReMixWidget*> serverMap;
    User* user{ nullptr };

    qint32 prevTabIndex{ 0 };
    bool nightMode{ false };

    public:
        explicit ReMixTabWidget(QWidget *parent = 0);
        ~ReMixTabWidget();

        void sendMultiServerMessage(QString msg);
        qint32 getPlayerCount();
        quint32 getServerCount();

        qint32 getPrevTabIndex() const;
        void setPrevTabIndex(const qint32& value);

        static qint32 getInstanceCount();
        static ReMixTabWidget* getTabInstance(QWidget* parent = nullptr);
        static CreateInstance* getCreateDialog(QWidget* parent = nullptr);

    private:
        void createTabButtons();
        void createServer();
        void applyThemes(qint32 type);

    private slots:
        void tabCloseRequestedSlot(qint32 index);
        void currentChangedSlot(qint32 newTab);
        void createServerAcceptedSlot(ServerInfo* server = nullptr);
};

#endif // REMIXTABWIDGET_H
