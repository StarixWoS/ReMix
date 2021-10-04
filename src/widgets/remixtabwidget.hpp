
#ifndef REMIXTABWIDGET_H
#define REMIXTABWIDGET_H

#include "prototypes.hpp"

//Required Qt Includes..
#include <QTabWidget>
#include <QObject>
#include <QTimer>
#include <QMutex>
#include <QMap>

class ReMixTabWidget : public QTabWidget
{
    Q_OBJECT

    static QMap<int, ReMixWidget*> serverMap;
    static CreateInstance* createDialog;
    static ReMixTabWidget* tabInstance;

    static qint32 instanceCount;

    QTimer createInstanceTimer;

    QToolButton* nightModeButton{ nullptr };
    QToolButton* newTabButton{ nullptr };

    bool nightMode{ false };

    public:
        explicit ReMixTabWidget(QWidget* parent = nullptr);
        ~ReMixTabWidget() override;

        void sendMultiServerMessage(const QString& msg);
        quint32 getPlayerCount() const;
        quint32 getServerCount() const;

        static qint32 getInstanceCount();
        static ReMixTabWidget* getTabInstance(QWidget* parent = nullptr);
        static void remoteCloseServer(ServerInfo* server, const bool restart = false);
        static void setToolTipString(ReMixWidget* widget);

    private:
        mutable QMutex mutex;
        static void removeServer(const qint32& index, const bool& remote = false, const bool& restart = false);
        void repositionServerIndices();
        void createTabButtons();
        void createServer();

    private slots:
        void tabCloseRequestedSlot(const qint32& index);
        void currentChangedSlot(const qint32& newTab);

    public slots:
        void crossServerCommentSlot(ServerInfo* server, const QString& comment);
        void createServerAcceptedSlot(ServerInfo* server = nullptr);
        void restartServerListSlot(const QStringList& restartList);

    signals:
        void crossServerCommentSignal(ServerInfo* server, const QString& comment);
};

#endif // REMIXTABWIDGET_H
