
#ifndef QT_INCLUDES_HPP
    #define QT_INCLUDES_HPP

    #include <QSortFilterProxyModel>
    #include <QCryptographicHash>
    #include <QStandardItemModel>
    #include <QNetworkInterface>
    #include <QSystemTrayIcon>
    #include <QStringBuilder>
    #include <QElapsedTimer>
    #include <QStandardItem>
    #include <QApplication>
    #include <QHostAddress>
    #include <QInputDialog>
    #include <QMainWindow>
    #include <QMessageBox>
    #include <QStringList>
    #include <QTextCursor>
    #include <QCloseEvent>
    #include <QScrollBar>
    #include <QTcpServer>
    #include <QUdpSocket>
    #include <QTcpSocket>
    #include <QTableView>
    #include <QByteArray>
    #include <QTextEdit>
    #include <QHostInfo>
    #include <QSettings>
    #include <QDateTime>
    #include <QFileInfo>
    #include <QProcess>
    #include <QtEndian>
    #include <QVariant>
    #include <QObject>
    #include <QString>
    #include <QDialog>
    #include <QDebug>
    #include <QTimer>
    #include <QEvent>
    #include <QMenu>
    #include <QChar>
    #include <QFile>
    #include <QChar>
    #include <QDir>

    #ifdef DECRYPT_PACKET_PLUGIN
        #include <QCoreApplication>
        #include <QPluginLoader>
    #endif // DECRYPT_PACKET_PLUGIN

#endif // QT_INCLUDES_HPP

#ifndef REMIX_INCLUDES_HPP
    #define REMIX_INCLUDES_HPP

    //Table-View Classes.
    #include "tblview/adminsortproxymodel.hpp"
    #include "tblview/plrsortproxymodel.hpp"
    #include "tblview/dvsortproxymodel.hpp"
    #include "tblview/ipsortproxymodel.hpp"
    #include "tblview/snsortproxymodel.hpp"
    #include "tblview/tbleventfilter.hpp"

    //Widget GUI Classes.
    #include "widgets/dabanwidget.hpp"
    #include "widgets/dvbanwidget.hpp"
    #include "widgets/ipbanwidget.hpp"
    #include "widgets/snbanwidget.hpp"

    //GUI and Data Storage Clases.
    #include "packethandler.hpp"
    #include "serverinfo.hpp"
    #include "cmdhandler.hpp"
    #include "bandialog.hpp"
    #include "messages.hpp"
    #include "settings.hpp"
    #include "comments.hpp"
    #include "randdev.hpp"
    #include "helper.hpp"
    #include "player.hpp"
    #include "server.hpp"
    #include "remix.hpp"
    #include "admin.hpp"

    #ifdef DECRYPT_PACKET_PLUGIN
        #include "packetdecryptinterface.hpp"
    #endif // DECRYPT_PACKET_PLUGIN

#endif // REMIX_INCLUDES_HPP
