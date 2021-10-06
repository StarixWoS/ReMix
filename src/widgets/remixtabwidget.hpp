
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
    static Theme* themeInstance;

    static qint32 instanceCount;

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
        static ReMixTabWidget* getTabInstance(QWidget* parent = nullptr);
        static void remoteCloseServer(Server* server, const bool restart = false);
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
        static void removeServer(const qint32& index, const bool& remote = false, const bool& restart = false);
        void repositionServerIndices();
        void createTabButtons();
        void createServer();

    private slots:
        void customContextMenuRequestedSlot(const QPoint& point);
        void tabCloseRequestedSlot(const qint32& index);
        void currentChangedSlot(const qint32& newTab);
        void renameServerTabSlot(int index);
        void nightModeButtonClickedSlot();
        void themeChangedSlot(const Themes& theme);

    public slots:
        void crossServerCommentSlot(Server* server, const QString& comment);
        void createServerAcceptedSlot(Server* server = nullptr);
        void restartServerListSlot(const QStringList& restartList);

    signals:
        void crossServerCommentSignal(Server* server, const QString& comment);
        void themeChangedSignal(const Themes& theme);
};

#endif // REMIXTABWIDGET_H
