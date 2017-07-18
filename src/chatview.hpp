#ifndef CHATVIEW_HPP
#define CHATVIEW_HPP

#include "prototypes.hpp"
#include <QDialog>

namespace Ui {
    class ChatView;
}

class ChatView : public QDialog
{
    Q_OBJECT

    PacketForge* pktForge{ nullptr };

    public:
        explicit ChatView(QWidget *parent = 0);
        ~ChatView();

        void setTitle(QString name);

        void parsePacket(QString& packet);
        void parseChatEffect(QString packet);
        void insertChat(QString msg, QString color, bool newLine);

    private slots:
        void on_chatInput_returnPressed();

    private:
        Ui::ChatView *ui;

    signals:
        void sendChat(QString);
};

#endif // CHATVIEW_HPP
