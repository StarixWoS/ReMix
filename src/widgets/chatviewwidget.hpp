#ifndef CHATVIEWWIDGET_HPP
#define CHATVIEWWIDGET_HPP

#include "prototypes.hpp"

//Required ReMix includes.
#include "theme.hpp"

//Required Qt Includes.
#include <QDialog>


namespace Ui {
    class ChatViewWidget;
}

class ChatViewWidget : public QWidget
{
    Q_OBJECT

    private:
        Games gameID{ Games::Invalid };
        static QString bleepList[31];

        PacketForge* pktForge{ nullptr };
        ServerInfo* server{ nullptr };

    public:
        explicit ChatViewWidget(QWidget* parent = nullptr,
                                ServerInfo* svr = nullptr);
        ~ChatViewWidget();

        void setTitle(const QString& name);
        void setGameID(const Games& gID);
        Games getGameID() const;

        void parsePacket(const QString& packet, const QString& alias = "");
        void parseChatEffect(const QString& packet);
        void insertChat(const QString& msg, const Colors& color,
                            const bool& newLine);

        void bleepChat(QString& message);

    private slots:
        void on_chatInput_returnPressed();

    private:
        Ui::ChatViewWidget *ui;

    signals:
        void sendChat(const QString&);
};

#endif // CHATVIEWWIDGET_HPP
