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

class ChatView : public QDialog
{
    Q_OBJECT

    Games gameID{ Games::Invalid };
    static QString bleepList[ 439 ];

    PacketForge* pktForge{ nullptr };
    ServerInfo* server{ nullptr };

    public:
        explicit ChatView(QWidget* parent = nullptr, ServerInfo* svr = nullptr);
        ~ChatView();

        void setTitle(const QString& name);
        void setGameID(const Games& gID);
        Games getGameID() const;

        void parsePacket(const QString& packet, Player* plr = nullptr);
        void parseChatEffect(const QString& packet);
        void insertChat(const QString& msg, const Colors& color,
                        const bool& newLine);

        void bleepChat(QString& message);

    private slots:
        void on_chatInput_returnPressed();

    private:
        Ui::ChatView* ui;

    signals:
        void sendChat(const QString&);
};

#endif // CHATVIEW_HPP
