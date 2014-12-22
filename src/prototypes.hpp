
#ifndef QT_PROTOTYPES_HPP
    #define QT_PROTOTYPES_HPP

//Required Qt Includes. --Unable to prototype in most instances.
//    #include <QElapsedTimer>
//    #include <QMainWindow>
//    #include <QModelIndex>
//    #include <QTcpServer>
//    #include <QObject>
//    #include <QDialog>
//    #include <QTimer>

    class QSortFilterProxyModel;
    class QCryptographicHash;
    class QNetworkInterface;
    class QStandardItemModel;
    class QSystemTrayIcon;
    class QStandardItem;
    class QElapsedTimer;  //Requires Include within Class Headers.
    class QApplication;
    class QHostAddress;
    class QInputDialog;
    class QModelIndex;  //Requires Include within Class Headers.
    class QMainWindow;  //Requires Include within Class Headers.
    class QMessageBox;
    class QCloseEvent;
    class QStringList;
    class QTextCursor;
    class QByteArray;
    class QScrollBar;
    class QTcpServer;  //Requires Include within Class Headers.
    class QTcpSocket;
    class QUdpSocket;
    class QHostInfo;
    class QDateTime;
    class QFileInfo;
    class QSettings;
    class QTextEdit;
    class QVariant;
    class QtEndian;
    class QDialog;  //Requires Include within Class Headers.
    class QString;
    class QObject;  //Requires Include within Class Headers.
    class QTimer;  //Requires Include within Class Headers.
    class QDebug;
    class QEvent;
    class QPoint;
    class QChar;
    class QFile;
    class QMenu;
    class QDir;

    #ifdef DECRYPT_PACKET_PLUGIN
        class QCoreApplication;
        class QPluginLoader;
    #endif // DECRYPT_PACKET_PLUGIN

#endif // QT_PROTOTYPES_HPP

#ifndef REMIX_PROTOTYPES_HPP
    #define REMIX_PROTOTYPES_HPP

    namespace AdminHelper
    {}

    namespace Help
    {}

    class UserMessage;
    class ServerInfo;
    class BanDialog;
    class Messages;
    class Settings;
    class RandDev;
    class Player;
    class Server;
    class Remix;
    class Admin;

    #ifdef DECRYPT_PACKET_PLUGIN
        class PacketDecryptInterface;
    #endif // DECRYPT_PACKET_PLUGIN

#endif // REMIX_PROTOTYPES_HPP
