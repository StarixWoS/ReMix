#ifndef CHATVIEW_HPP
#define CHATVIEW_HPP

#include "prototypes.hpp"

//Required ReMix includes.
#include "theme.hpp"

//Required Qt Includes.
#include <QDialog>

namespace Ui {
    class ChatView;
}

class ChatView : public QWidget
{
    Q_OBJECT

    static QHash<QSharedPointer<Server>, ChatView*> chatViewInstanceMap;
    static QVector<Colors> colors;
    static QStringList bleepList;
    static Themes currentTheme;

    QSharedPointer<Server> server;

    public:
        explicit ChatView(QSharedPointer<Server> svr, QWidget* parent = nullptr);
        ~ChatView() override;

        static QSharedPointer<Server> getServer(ChatView* chatView);
        static ChatView* getInstance(QSharedPointer<Server> server);
        static void deleteInstance(QSharedPointer<Server> server);

        bool parseChatEffect(const QString& packet);
        void bleepChat(QString& message);
        void insertChat(const QString& msg, const Colors& color, const bool& newLine);

        void scrollToBottom(const bool& forceScroll = false);
        static QString getTimeStr();

    private:
        void insertColoredSerNum(QSharedPointer<Player> plr);
        void insertColoredName(QSharedPointer<Player> plr);
        void insertTimeStamp();

    public slots:
        void insertChatMsgSlot(const QString& msg, const Colors& color, const bool& newLine);
        void insertAdminMessageSlot(const QString& msg, const bool& toAll, QSharedPointer<Player> admin, QSharedPointer<Player> target);
        void newUserCommentSlot(QSharedPointer<Player> plr, const QString& message);
        void colorOverrideSlot(const QString& oldColor, const QString& newColor);
        void insertMasterMessageSlot(const QString& message, QSharedPointer<Player> target, const bool& toAll);

    private slots:
        void on_chatInput_returnPressed();
        void themeChangedSlot(const Themes& theme);
        void on_autoScrollCheckBox_toggled(bool checked);
        void on_clearChat_clicked();

        void on_timeStampCheckBox_clicked(bool checked);

    private:
        Ui::ChatView* ui;

    signals:
        void insertLogSignal(const QString& source, const QString& message, const LKeys& type, const bool& logToFile, const bool& newLine) const;
        void sendChatSignal(const QString& message, QSharedPointer<Player> target, const bool& toAll);
        void newUserCommentSignal(const QString& message);
};

#endif // CHATVIEW_HPP
