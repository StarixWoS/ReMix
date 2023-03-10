
#ifndef REMIXTABWIDGET_HPP
#define REMIXTABWIDGET_HPP

#include "prototypes.hpp"

//Required Qt Includes..
#include <QTabWidget>
#include <QThread>
#include <QObject>
#include <QTimer>
#include <QMutex>
#include <QMap>

class ReMixTabWidget : public QTabWidget
{
    Q_OBJECT

    static QList<Games> activeGames;
    static QMap<int, ReMixWidget*> serverMap;
    static CreateInstance* createDialog;
    static ReMixTabWidget* tabInstance;
    static Theme* themeInstance;

    static qint32 instanceCount;

    QThread* masterMixThread{ nullptr };
    QTimer createInstanceTimer;

    QToolButton* nightModeButton{ nullptr };
    QToolButton* newTabButton{ nullptr };

    Themes themeType{ Themes::Light };

    public:
        explicit ReMixTabWidget(QWidget* parent = nullptr);
        ~ReMixTabWidget() override;

        void sendMultiServerMessage(const QString& msg);
        quint32 getPlayerCount() const;
        quint32 getServerCount() const;

        static qint32 getInstanceCount();
        static ReMixTabWidget* getInstance(QWidget* parent = nullptr);

        void remoteCloseServer(QSharedPointer<Server> server, const bool restart = false);
        static void setToolTipString(ReMixWidget* widget);

        Theme* getThemeInstance() const;
        void setThemeInstance(Theme* newThemeInstance);

        QToolButton* getNewTabButton();
        void setNewTabButton(QToolButton* button);

        Themes getThemeType() const;
        void setThemeType(Themes newThemeType);

        QToolButton* getNightModeButton();
        void setNightModeButton(QToolButton* newNightModeButton);

    private:
        mutable QMutex mutex;
        void removeServer(const qint32& index, const bool& remote = false, const bool& restart = false);
        void repositionServerIndices();
        void createTabButtons();
        void createServer();
        int tabAt(const QPoint &position, const qint32& cornerButtonWidth) const;

    private slots:
        void customContextMenuRequestedSlot(const QPoint& point);
        void tabCloseRequestedSlot(const qint32& index);
        void currentChangedSlot(const qint32& newTab);
        void renameServerTabSlot(int index);
        void nightModeButtonClickedSlot();
        void themeChangedSlot(const Themes& theme);

    public slots:
        void crossServerCommentSlot(QSharedPointer<Server> server, const QString& comment);
        void createServerAcceptedSlot(QSharedPointer<Server> server);
        void restartServerListSlot(const QStringList& restartList);

    signals:
        void crossServerCommentSignal(QSharedPointer<Server> server, const QString& comment);
        void themeChangedSignal(const Themes& theme);
        void removeConnectedGameSignal(const Games& game);
};

#endif // REMIXTABWIDGET_HPP
