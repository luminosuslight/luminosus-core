#include "AsyncWebSocket.h"


AsyncWebSocket::AsyncWebSocket()
    : m_socket()
    , m_thread(nullptr)
{
    qRegisterMetaType<QAbstractSocket::SocketState>("QAbstractSocket::SocketState");
    m_thread = new QThread();
    this->moveToThread(m_thread);
    m_socket.moveToThread(m_thread);

    connect(&m_socket, &QWebSocket::connected,
            this, &AsyncWebSocket::connected);
    connect(&m_socket, &QWebSocket::disconnected,
            this, &AsyncWebSocket::disconnected);
    connect(&m_socket, &QWebSocket::binaryMessageReceived,
            this, &AsyncWebSocket::binaryMessageReceived);

    connect(&m_socket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error),
            [this](QAbstractSocket::SocketError error){ qDebug() << error << m_socket.errorString(); });

    m_thread->start();
}

qint64 AsyncWebSocket::sendBinaryMessage(const QByteArray& data) {
    qint64 bytesSent = 0;
    QMetaObject::invokeMethod(this,
                              "sendBinaryMessageInThread",
                              Qt::QueuedConnection,
                              Q_ARG(const QByteArray&, data));
    return bytesSent;
}

void AsyncWebSocket::open(const QUrl& url) {
    QMetaObject::invokeMethod(&m_socket,
                              "open",
                              Qt::QueuedConnection,
                              Q_ARG(QUrl, url));
}

void AsyncWebSocket::close() {
    QMetaObject::invokeMethod(&m_socket,
                              "close",
                              Qt::QueuedConnection);
}

void AsyncWebSocket::sendBinaryMessageInThread(const QByteArray& data) {
    m_socket.sendBinaryMessage(data);
}
