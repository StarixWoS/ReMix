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

    static QHash<Server*, ChatView*> chatViewInstanceMap;
    static QStringList bleepList;

    Server* server{ nullptr };

    public:
        explicit ChatView(QWidget* parent = nullptr, Server* svr = nullptr);
        ~ChatView() override;

        static ChatView* getInstance(Server* server);
        static void deleteInstance(Server* server);

        bool parseChatEffect(const QString& packet);
        void bleepChat(QString& message);
        void insertChat(const QString& msg, const Colors& color, const bool& newLine);

        void scrollToBottom(const bool& forceScroll = false);
        static QString getTimeStr();

    public slots:
        void insertChatMsgSlot(const QString& msg, const Colors& color, const bool& newLine);
        void newUserCommentSlot(const QString& sernum, const QString& alias, const QString& message);

    private slots:
        void on_chatInput_returnPressed();
        void themeChangedSlot(const Themes& theme);

        void on_autoScrollCheckBox_toggled(bool checked);

    private:
        Ui::ChatView* ui;

    signals:
        void insertLogSignal(const QString& source, const QString& message, const LogTypes& type, const bool& logToFile, const bool& newLine) const;
        void sendChatSignal(const QString&);
        void newUserCommentSignal(const QString& message);
};

#endif // CHATVIEW_HPP
