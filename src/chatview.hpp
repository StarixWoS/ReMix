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
    Games gameID{ Games::Invalid };

    public:
        explicit ChatView(QWidget* parent = 0);
        ~ChatView();

        void setTitle(const QString& name);
        void setGameID(const Games& gID);
        Games getGameID() const;

        void parsePacket(const QString& packet, const QString& alias = "");
        void parseChatEffect(const QString& packet);
        void insertChat(const QString& msg, const QString& color,
                        const bool& newLine);

    private slots:
        void on_chatInput_returnPressed();

    private:
        Ui::ChatView* ui;

    signals:
        void sendChat(const QString&);
};

#endif // CHATVIEW_HPP
