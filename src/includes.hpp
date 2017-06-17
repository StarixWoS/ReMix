
#ifndef QT_INCLUDES_HPP
    #define QT_INCLUDES_HPP

    #include <QNetworkAccessManager>
    #include <QSortFilterProxyModel>
    #include <QNetworkAddressEntry>
    #include <QCryptographicHash>
    #include <QStandardItemModel>
    #include <QNetworkInterface>
    #include <QTableWidgetItem>
    #include <QStringListModel>
    #include <QXmlStreamReader>
    #include <QSystemTrayIcon>
    #include <QNetworkRequest>
    #include <QStringBuilder>
    #include <QElapsedTimer>
    #include <QStyleFactory>
    #include <QStandardItem>
    #include <QNetworkReply>
    #include <QApplication>
    #include <QHostAddress>
    #include <QInputDialog>
    #include <QtConcurrent>
    #include <QFileDialog>
    #include <QToolButton>
    #include <QMainWindow>
    #include <QMessageBox>
    #include <QStringList>
    #include <QTextCursor>
    #include <QCloseEvent>
    #include <QGridLayout>
    #include <QTextStream>
    #include <QScrollBar>
    #include <QTcpServer>
    #include <QUdpSocket>
    #include <QTcpSocket>
    #include <QTableView>
    #include <QByteArray>
    #include <QTabWidget>
    #include <QTextEdit>
    #include <QHostInfo>
    #include <QSettings>
    #include <QDateTime>
    #include <QFileInfo>
    #include <QToolTip>
    #include <QProcess>
    #include <QtEndian>
    #include <QVariant>
    #include <QFuture>
    #include <QObject>
    #include <QString>
    #include <QLayout>
    #include <QDialog>
    #include <QDebug>
    #include <QTimer>
    #include <QEvent>
    #include <QMenu>
    #include <QChar>
    #include <QFile>
    #include <QChar>
    #include <QDir>
    #include <QUrl>

    #ifdef DECRYPT_PACKET_PLUGIN
        #include <QCoreApplication>
        #include <QPluginLoader>
    #endif // DECRYPT_PACKET_PLUGIN

#endif // QT_INCLUDES_HPP

#ifndef REMIX_INCLUDES_HPP
    #define REMIX_INCLUDES_HPP

    //Table-View Classes.
    #include "tblview/usersortproxymodel.hpp"
    #include "tblview/plrsortproxymodel.hpp"
    #include "tblview/tbleventfilter.hpp"

    //Widget GUI Classes.
    #include "widgets/settingswidget.hpp"
    #include "widgets/messageswidget.hpp"
    #include "widgets/remixtabwidget.hpp"
    #include "widgets/plrlistwidget.hpp"
    #include "widgets/userdelegate.hpp"
    #include "widgets/remixwidget.hpp"
    #include "widgets/ruleswidget.hpp"

    //GUI and Data Storage Clases.
    #include "createinstance.hpp"
    #include "packethandler.hpp"
    #include "serverinfo.hpp"
    #include "cmdhandler.hpp"
    #include "settings.hpp"
    #include "comments.hpp"
    #include "randdev.hpp"
    #include "sendmsg.hpp"
    #include "helper.hpp"
    #include "player.hpp"
    #include "server.hpp"
    #include "rules.hpp"
    #include "remix.hpp"
    #include "user.hpp"
    #include "upnp.hpp"

    #ifdef DECRYPT_PACKET_PLUGIN
        #include "packetdecryptinterface.hpp"
    #endif // DECRYPT_PACKET_PLUGIN

#endif // REMIX_INCLUDES_HPP
