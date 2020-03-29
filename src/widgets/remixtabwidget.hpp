
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

    static QMap<int, ReMixWidget*> serverMap;
    static CreateInstance* createDialog;
    static ReMixTabWidget* tabInstance;
    static qint32 instanceCount;

    QToolButton* nightModeButton{ nullptr };
    QToolButton* newTabButton{ nullptr };

    User* user{ nullptr };

    qint32 prevTabIndex{ 0 };
    bool nightMode{ false };

    public:
        explicit ReMixTabWidget(QWidget* parent = nullptr);
        ~ReMixTabWidget() override;

        void sendMultiServerMessage(const QString& msg);
        quint32 getPlayerCount() const;
        quint32 getServerCount() const;

        qint32 getPrevTabIndex() const;
        void setPrevTabIndex(const qint32& value);

        static qint32 getInstanceCount();
        static ReMixTabWidget* getTabInstance(QWidget* parent = nullptr);
        static CreateInstance* getCreateDialog(QWidget* parent = nullptr);
        static void remoteCloseServer(ServerInfo* server, const bool restart = false);
        static void setToolTipString(ReMixWidget* widget);

    private:
        static void removeServer(const qint32& index, const bool& remote = false, const bool& restart = false);
        void repositionServerIndices();
        void createTabButtons();
        void createServer();

    private slots:
        void tabCloseRequestedSlot(const qint32& index);
        void currentChangedSlot(const qint32& newTab);
        void createServerAcceptedSlot(ServerInfo* server = nullptr);
};

#endif // REMIXTABWIDGET_H
