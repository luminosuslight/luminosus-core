#include "WebsocketConnection.h"

#include "core/MainController.h"

#include <QWebSocketServer>
#include <QUuid>
#include <QQuickAsyncImageProvider>
#include <QImage>
#include <QQmlEngine>
#include <QDir>
#include <QBuffer>
#include <QImageReader>
#include <QQueue>
#include <QPair>
#include <QtConcurrent>
#include <QSslKey>
#include <QGuiApplication>
#include <QScreen>
#include <QtConcurrent>

#include <QDebug>


class AsyncImageResponse : public QQuickImageResponse {

public:
    QQuickTextureFactory* textureFactory() const override {
        return QQuickTextureFactory::textureFactoryForImage(m_image);
    }
    QImage m_image;
};

class AsyncImageProvider : public QQuickAsyncImageProvider {

public:
    explicit AsyncImageProvider(WebsocketConnection* websocketConnection)
        : QQuickAsyncImageProvider()
        , m_websocketConnection(websocketConnection)
    { }

    QQuickImageResponse* requestImageResponse(const QString& id, const QSize& requestedSize) override {
        AsyncImageResponse* asyncResponse = new AsyncImageResponse();

        // check cache:
        QString cacheId = "/" + id + QString::number(requestedSize.width()) + QString::number(requestedSize.height());
        for (auto item: m_imageCache) {
            if (item.first == cacheId) {
                asyncResponse->m_image = item.second;
                emit asyncResponse->finished();
                return asyncResponse;
            }
        }

        QCborMap data;
        data[QString("path")] = "/" + id;
        if (requestedSize.isValid()) {
            data[QString("width")] = requestedSize.width();
            data[QString("height")] = requestedSize.height();
        } else {
            data[QString("width")] = 0;
            data[QString("height")] = 0;
        }

        emit m_websocketConnection->askServerMainthread(
                    WsRequestTypes::FILE,
                    data,
                    [this, id, requestedSize, asyncResponse, cacheId](QCborMap response) {
            QByteArray content = response[QString("content")].toByteArray();
            // thumbnail size: < 8kB
            // medium size: < 30kB
            // full size: > 1MB, up to 11 MB
            if (content.size() > 4*1000) {
                // everything bigger than a small thumbnail is processed in another thread
                // thread dispatching takes ~ 0.02 ms
                // processing a thumbnail takes ~1.8ms
                // threading has overhead of <0.2ms
                QtConcurrent::run([this, requestedSize, asyncResponse, cacheId, content]() mutable {
                    this->processImage(requestedSize, asyncResponse, cacheId, content);
                });
            } else {
                // small thumbnails are processed in main thread
                this->processImage(requestedSize, asyncResponse, cacheId, content);
            }
        });

        return asyncResponse;
    }

    void processImage(const QSize& requestedSize, AsyncImageResponse* asyncResponse,
                      QString cacheId, QByteArray content) {
        QBuffer readBuffer(&content);
        readBuffer.open(QIODevice::ReadOnly);
        QImageReader imageReader(&readBuffer);
        imageReader.setAutoTransform(true);
        asyncResponse->m_image = imageReader.read();

        if (requestedSize.isValid()) {
            double ratio = 1.0;
            if (requestedSize.height() <= 0 && requestedSize.width() > 0) {
                ratio = requestedSize.width() / double(asyncResponse->m_image.width());
            } else if (requestedSize.width() <= 0 && requestedSize.height() > 0) {
                ratio = requestedSize.height() / double(asyncResponse->m_image.height());
            }
            if (ratio < 1.0) {
                QSize newSize(int(asyncResponse->m_image.width() * ratio), int(asyncResponse->m_image.height() * ratio));
                asyncResponse->m_image = asyncResponse->m_image.scaled(newSize);
            }
        }

        if (asyncResponse->m_image.width() > 300) {
            m_imageCache << QPair<QString, QImage>(cacheId, asyncResponse->m_image);
            if (m_imageCache.size() > 7) {
                m_imageCache.dequeue();
            }
        }

        emit asyncResponse->finished();
    }

    bool isCached(const QString& id, const QSize& requestedSize) const {
        QSize realSize = requestedSize * QGuiApplication::primaryScreen()->devicePixelRatio();
        QString cacheId = id + QString::number(realSize.width()) + QString::number(realSize.height());
        for (auto item: m_imageCache) {
            if (item.first == cacheId) {
                return true;
            }
        }
        return false;
    }

private:
    WebsocketConnection* m_websocketConnection;
    QQueue<QPair<QString, QImage>> m_imageCache;
};


WebsocketConnection::WebsocketConnection(MainController* controller)
    : QObject(controller)
    , m_controller(controller)
    , m_imageProvider(nullptr)
    , m_initialized(false)
    , m_serverEnabled(this, "serverEnabled", false)
    , m_clientEnabled(this, "clientEnabled", false)
    , m_port(this, "port", 55487, 0, 65535)
    , m_serverUrl(this, "serverUrl", "ws://localhost:55487")
    , m_dynv6Hostname(this, "dynv6Hostname", "")
    , m_dynv6Token(this, "dynv6Username", "")
    , m_websocketServer(new QWebSocketServer(QStringLiteral("Luminosus Websocket Server"),
                                             QWebSocketServer::SecureMode, this))
    , m_connectedToServer(this, "connectedToServer", false, /*persistent*/ false)
{
    QSslConfiguration sslConfiguration;
    QFile certFile(QStringLiteral(":/core/luminosus_websocket.cert"));
    QFile keyFile(QStringLiteral(":/core/luminosus_websocket.key"));
    certFile.open(QIODevice::ReadOnly);
    keyFile.open(QIODevice::ReadOnly);
    QSslCertificate certificate(&certFile, QSsl::Pem);
    QSslKey sslKey(&keyFile, QSsl::Rsa, QSsl::Pem);
    certFile.close();
    keyFile.close();
    sslConfiguration.setPeerVerifyMode(QSslSocket::VerifyNone);
    sslConfiguration.setLocalCertificate(certificate);
    sslConfiguration.setPrivateKey(sslKey);
    sslConfiguration.setProtocol(QSsl::TlsV1SslV3);
    m_websocketServer->setSslConfiguration(sslConfiguration);

    QSslConfiguration clientSslConfiguration = QSslConfiguration::defaultConfiguration();
    clientSslConfiguration.setCaCertificates({certificate});
#ifdef Q_OS_MAC
    clientSslConfiguration.setPeerVerifyMode(QSslSocket::VerifyNone);
#endif
    m_asyncWebsocketClient.socket().setSslConfiguration(clientSslConfiguration);
    m_asyncWebsocketClient.socket().ignoreSslErrors({QSslError(QSslError::HostNameMismatch, certificate)});

    qRegisterMetaType<std::function<void (QCborMap)>>("std::function<void(QCborMap)>");
    connect(this, &WebsocketConnection::askServerMainthread,
            this, &WebsocketConnection::askServer, Qt::QueuedConnection);
    connect(this, &WebsocketConnection::sendToClientMainthread,
            this, &WebsocketConnection::handleSendToClientMainthread, Qt::QueuedConnection);


    connect(m_websocketServer, &QWebSocketServer::newConnection,
            this, &WebsocketConnection::onNewConnection);
    connect(m_websocketServer, &QWebSocketServer::closed,
            this, &WebsocketConnection::serverClosed);

    connect(&m_asyncWebsocketClient, &AsyncWebSocket::connected,
            this, &WebsocketConnection::onConnectedToServer, Qt::QueuedConnection);
    connect(&m_asyncWebsocketClient, &AsyncWebSocket::disconnected,
            this, &WebsocketConnection::onDisconnectedFromServer, Qt::QueuedConnection);

    connect(&m_serverEnabled, &BoolAttribute::valueChanged, this, &WebsocketConnection::updateServerState);
    connect(&m_port, &IntegerAttribute::valueChanged, this, &WebsocketConnection::updateServerState);
    connect(&m_clientEnabled, &BoolAttribute::valueChanged, this, &WebsocketConnection::updateClientState);
    connect(&m_serverUrl, &StringAttribute::valueChanged, this, &WebsocketConnection::updateClientState);

    m_imageProvider = new AsyncImageProvider(this);
    m_controller->guiManager()->qmlEngine()->addImageProvider(QLatin1String("server"),
                                                              m_imageProvider);

    // To get a static IPv6 address generated based on the MAC address:
    // If SLAAC is performed by NetworkManager:
    // nmcli con modify "Connection name" ipv6.addr-gen-mode eui64
    // see https://superuser.com/a/1297875
    m_dynDnsUpdateTimer.setInterval(1000*60*60);
    m_dynDnsUpdateTimer.setSingleShot(false);
    connect(&m_dynDnsUpdateTimer, &QTimer::timeout, this, &WebsocketConnection::updateDynDNS);
    m_dynDnsUpdateTimer.start();
}

WebsocketConnection::~WebsocketConnection() {
    m_websocketServer->close();
    m_asyncWebsocketClient.close();
    qDeleteAll(m_clients.begin(), m_clients.end());
}

QJsonObject WebsocketConnection::getState() const {
    QJsonObject state;
    m_serverEnabled.writeTo(state);
    m_clientEnabled.writeTo(state);
    m_port.writeTo(state);
    m_serverUrl.writeTo(state);
    m_dynv6Hostname.writeTo(state);
    m_dynv6Token.writeTo(state);
    return state;
}

void WebsocketConnection::setState(QJsonObject state) {
    m_serverEnabled.readFrom(state);
    m_clientEnabled.readFrom(state);
    m_port.readFrom(state);
    m_serverUrl.readFrom(state);
    m_dynv6Hostname.readFrom(state);
    m_dynv6Token.readFrom(state);
    m_initialized = true;
    updateServerState();
    updateClientState();
}

void WebsocketConnection::tellServer(QString contentType, QCborMap value) {
    value[QStringLiteral("requestType")] = contentType;
    m_asyncWebsocketClient.sendBinaryMessage(value.toCborValue().toCbor());
}

void WebsocketConnection::askServer(QString fnName, QCborMap value, std::function<void (QCborMap)> callback) {
    QString packetId = QUuid::createUuid().toString();
    value[QStringLiteral("requestType")] = fnName;
    value[QLatin1String("packetId")] = packetId;
    m_asyncWebsocketClient.sendBinaryMessage(value.toCborValue().toCbor());
    m_packetCallbacks.insert(packetId, callback);
}

void WebsocketConnection::registerFunction(const QString& requestType, std::function<QCborMap(QCborMap)> handler) {
    if (m_requestHandlers.contains(requestType)) {
        qWarning() << "WebsocketServer: Handler for request type" << requestType << "already registered.";
        return;
    }
    m_requestHandlers.insert(requestType, handler);
}

void WebsocketConnection::registerListener(const QString& contentType, std::function<void(QCborMap)> listener) {
    if (m_requestListeners.contains(contentType)) {
        m_requestListeners[contentType].append(listener);
    } else {
        m_requestListeners[contentType] = QVector<std::function<void(QCborMap)>>{listener};
    }
}

bool WebsocketConnection::isCached(const QString& id, const QSize& requestedSize) const {
    return m_imageProvider->isCached(id, requestedSize);
}

void WebsocketConnection::updateServerState() {
    if (!m_initialized) return;
    m_websocketServer->close();
    if (m_serverEnabled) {
        if (m_websocketServer->listen(QHostAddress::Any, quint16(m_port))) {
            qInfo() << "WebsocketServer: Listening on port" << m_port.getValue();
            updateDynDNS();
        } else {
            qWarning() << "WebsocketServer: Not listening.";
        }
    }
}

void WebsocketConnection::updateClientState() {
    if (!m_initialized) return;
    m_asyncWebsocketClient.close();
    if (m_clientEnabled) {
        m_asyncWebsocketClient.open(QUrl(m_serverUrl));
    }
}

void WebsocketConnection::updateDynDNS() {
    if (m_serverEnabled && !m_dynv6Hostname.getValue().isEmpty() && !m_dynv6Token.getValue().isEmpty()) {
        QString url = "https://ipv6.dynv6.com/api/update?hostname=" + m_dynv6Hostname + "&ipv6=auto&token=" + m_dynv6Token;
        QNetworkReply* reply = m_networkManager.get(QNetworkRequest(QUrl(url)));
        connect(reply, &QNetworkReply::finished, [reply](){
            qDebug() << "WebsocketServer: Dynv6 response:" << reply->readAll();
        });
        qInfo() << "WebsocketServer: Updated dynv6 entry for" << m_dynv6Hostname;
    }
}

void WebsocketConnection::onNewConnection() {
    QWebSocket* socket = m_websocketServer->nextPendingConnection();
    if (!socket) return;
    connect(socket, &QWebSocket::binaryMessageReceived, this, &WebsocketConnection::processBinaryMessageFromClient);
    connect(socket, &QWebSocket::disconnected, this, &WebsocketConnection::onClientDisconnected);
    m_clients << socket;
}

QByteArray loadFile(QString path) {
    QFile loadFile(path);
    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Couldn't open file " + path + ".";
        return QByteArray();
    }
    QByteArray content = loadFile.readAll();
    loadFile.close();
    return content;
}

QByteArray loadImage(QString path, int maxWidth, int maxHeight) {
    if (maxWidth > 0 && maxHeight > 0) {
        QImageReader imageReader(path);
        imageReader.setAutoTransform(true);
        QSize imageSize = imageReader.size();
        bool rotated = imageReader.transformation().testFlag(QImageIOHandler::TransformationRotate90)
                || imageReader.transformation().testFlag(QImageIOHandler::TransformationRotate270);
        if (rotated) imageSize.transpose();
        double viewportRatio = double(maxWidth) / maxHeight;
        double imageRatio = double(imageSize.width()) / imageSize.height();
        double scaleRatio = 1.0;
        if (viewportRatio < imageRatio) {
            // limit is width
            scaleRatio = maxWidth / double(imageSize.width());
        } else {
            // limit is height
            scaleRatio = maxHeight / double(imageSize.height());
        }
        if (scaleRatio < 1.0) {
            // max size is smaller than image
            // -> only return required size and compress it:
            QSize newSize(int(imageSize.width() * scaleRatio), int(imageSize.height() * scaleRatio));
            if (rotated) newSize.transpose();
            // using setScaledSize avoids loading the whole image first
            imageReader.setScaledSize(newSize);
            QImage image = imageReader.read();
            QByteArray content;
            QBuffer buffer(&content);
            buffer.open(QIODevice::WriteOnly);
            image.save(&buffer, "JPG", 75);
            return content;
        } else {
            // max width and height is larger than image,
            // return full, original image:
            return loadFile(path);
        }
    } else {
        // max size is not set,
        // return original image:
        return loadFile(path);
    }
}

void WebsocketConnection::processBinaryMessageFromClient(QByteArray data) {
    QWebSocket* client = qobject_cast<QWebSocket*>(sender());
    if (!client) {
        qWarning() << "WebsocketServer: Client of received message is invalid.";
        return;
    }
    QCborMap message = QCborValue::fromCbor(data).toMap();
    QString requestType = message.value(QLatin1String("requestType")).toString();

    if (requestType == WsRequestTypes::FILE) {
        // loading and sending files is done in separate thread:
        QtConcurrent::run([this, client, message]() mutable {
            QString path = message.value(QLatin1String("path")).toString();
            if (!QDir().exists(path)) {
                qWarning() << "File " + path + " does not exist.";
                return;
            }

            QByteArray content;
            if (path.toLower().endsWith(".jpg") || path.toLower().endsWith(".jpeg")) {
                int maxWidth = int(message[QString("width")].toInteger());
                int maxHeight = int(message[QString("height")].toInteger());
                content = loadImage(path, maxWidth, maxHeight);
            } else {
                content = loadFile(path);
            }

            message[QString("content")] = content;
            emit sendToClientMainthread(client, message.toCborValue().toCbor());
        });

    } else if (m_requestHandlers.contains(requestType)) {
        QCborMap result = m_requestHandlers[requestType](message);
        client->sendBinaryMessage(result.toCborValue().toCbor());
    } else if (m_requestListeners.contains(requestType)) {
        for (auto& listener: m_requestListeners[requestType]) {
            listener(message);
        }
    } else {
        // echo message if no known request type is given:
        client->sendBinaryMessage(data);
    }
}

void WebsocketConnection::onClientDisconnected() {
    QWebSocket* client = qobject_cast<QWebSocket *>(sender());
    qDebug() << "WebsocketServer: Client disconnect:" << client;
    if (client) {
        m_clients.removeAll(client);
        client->deleteLater();
    }
}

void WebsocketConnection::onConnectedToServer() {
    qDebug() << "WebsocketServer: Connected to server.";
    connect(&m_asyncWebsocketClient, &AsyncWebSocket::binaryMessageReceived,
            this, &WebsocketConnection::processBinaryMessageFromServer, Qt::QueuedConnection);
    m_connectedToServer = true;
    emit connectedToServer();
}

void WebsocketConnection::onDisconnectedFromServer() {
    m_connectedToServer = false;
}

void WebsocketConnection::processBinaryMessageFromServer(QByteArray data) {
    QCborMap message = QCborValue::fromCbor(data).toMap();
    if (message.contains(QLatin1String("packetId"))){
        QString packetId = message.value(QLatin1String("packetId")).toString();
        if (m_packetCallbacks.contains(packetId)) {
            m_packetCallbacks[packetId](message);
            m_packetCallbacks.remove(packetId);
        }
    } else if (message.contains(QLatin1String("requestType"))) {
        QString requestType = message.value(QLatin1String("requestType")).toString();
        if (m_requestListeners.contains(requestType)) {
            for (auto& listener: m_requestListeners[requestType]) {
                listener(message);
            }
        }
    }
}

void WebsocketConnection::handleSendToClientMainthread(QWebSocket* client, const QByteArray& data) {
    client->sendBinaryMessage(data);
}
