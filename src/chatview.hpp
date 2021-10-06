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
    static QStringList bleepList;

    PacketForge* pktForge{ nullptr };
    Server* server{ nullptr };
    CmdHandler* cmdHandle{ nullptr };

    public:
        explicit ChatView(QWidget* parent = nullptr, Server* svr = nullptr);
        ~ChatView() override;

        void setTitle(const QString& name);
        void setGameID(const Games& gID);
        Games getGameID() const;

        bool parsePacket(const QByteArray& packet, Player* plr = nullptr);
        bool parseChatEffect(const QString& packet);
        void bleepChat(QString& message);
        void insertChat(const QString& msg, const Colors& color, const bool& newLine);

        CmdHandler* getCmdHandle() const;
        void setCmdHandle(CmdHandler* value);

    private slots:
        void on_chatInput_returnPressed();

    private:
        Ui::ChatView* ui;

    signals:
        void insertLogSignal(const QString& source, const QString& message, const LogTypes& type, const bool& logToFile, const bool& newLine) const;
        void sendChatSignal(const QString&);
};

#endif // CHATVIEW_HPP
