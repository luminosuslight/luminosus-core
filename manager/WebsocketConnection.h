#ifndef WEBSOCKETCONNECTION_H
#define WEBSOCKETCONNECTION_H

#include "core/helpers/SmartAttribute.h"
#include "core/helpers/AsyncWebSocket.h"

#include <QObject>
#include <QList>
#include <QByteArray>
#include <QWebSocket>
#include <QCborMap>
#include <QMap>
#include <QTimer>
#include <QNetworkAccessManager>

#include <functional>

class QWebSocketServer;
class QWebSocket;
class CoreController;
class AsyncImageProvider;


namespace WsRequestTypes {
    static const QString FILE = "file";
    static const QString USER_INPUT = "userInput";
    static const QString SYSTEM_OUTPUT = "systemOutput";
    static const QString ANSWER_OPTIONS = "answerOptions";
}


class WebsocketConnection : public QObject {

    Q_OBJECT

public:
    explicit WebsocketConnection(CoreController* controller);
    ~WebsocketConnection();

    QJsonObject getState() const;
    void setState(QJsonObject state);

signals:
    void serverClosed();
    void askServerMainthread(QString fnName, QCborMap value, std::function<void (QCborMap)> callback);
    void sendToClientMainthread(QWebSocket* client, const QByteArray& data);
    void connectedToServer();

public slots:
    void tellServer(QString contentType, QCborMap value);
    void askServer(QString fnName, QCborMap value, std::function<void (QCborMap)> callback);
    void registerFunction(const QString& name, std::function<QCborMap(QCborMap)> handler);
    void registerListener(const QString& contentType, std::function<void(QCborMap)> listener);

    QObject* serverEnabled() { return &m_serverEnabled; }
    QObject* clientEnabled() { return &m_clientEnabled; }

    QObject* port() { return &m_port; }
    QObject* serverUrl() { return &m_serverUrl; }
    QObject* dynv6Hostname() { return &m_dynv6Hostname; }
    QObject* dynv6Token() { return &m_dynv6Token; }
    QObject* isConnectedToServer() { return &m_connectedToServer; }
    QList<QWebSocket*>& clients() { return m_clients; }

    bool isCached(const QString& id, const QSize& requestedSize=QSize()) const;

private slots:
    void updateServerState();
    void updateClientState();
    void updateDynDNS();

    void onNewConnection();
    void onClientDisconnected();
    void processBinaryMessageFromClient(QByteArray message);

    void onConnectedToServer();
    void onDisconnectedFromServer();
    void processBinaryMessageFromServer(QByteArray data);

    void handleSendToClientMainthread(QWebSocket* client, const QByteArray& data);

private:
    CoreController* const m_controller;
    AsyncImageProvider* m_imageProvider;
    bool m_initialized;

    BoolAttribute m_serverEnabled;
    BoolAttribute m_clientEnabled;

    IntegerAttribute m_port;
    StringAttribute m_serverUrl;
    StringAttribute m_dynv6Hostname;
    StringAttribute m_dynv6Token;

    QTimer m_dynDnsUpdateTimer;
    QNetworkAccessManager m_networkManager;

    QWebSocketServer* m_websocketServer;
    QList<QWebSocket*> m_clients;

    AsyncWebSocket m_asyncWebsocketClient;
    QMap<QString, std::function<void(QCborMap)>> m_packetCallbacks;
    BoolAttribute m_connectedToServer;

    QMap<QString, std::function<QCborMap(QCborMap)>> m_requestHandlers;
    QMap<QString, QVector<std::function<void(QCborMap)>>> m_requestListeners;
};

#endif // WEBSOCKETCONNECTION_H
