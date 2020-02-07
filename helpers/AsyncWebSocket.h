#ifndef ASYNCWEBSOCKET_H
#define ASYNCWEBSOCKET_H

#include <QObject>
#include <QWebSocket>

#ifdef THREADS_ENABLED
#include <QThread>
#endif


class AsyncWebSocket : public QObject {

    Q_OBJECT

public:
    AsyncWebSocket();

    QWebSocket& socket() { return m_socket; }

    qint64 sendBinaryMessage(const QByteArray& data);

    void open(const QUrl &url);
    void close();

signals:
    void connected();
    void disconnected();
    void binaryMessageReceived(QByteArray data);

private slots:
    void sendBinaryMessageInThread(const QByteArray& data);

protected:
    QWebSocket m_socket;
#ifdef THREADS_ENABLED
    QThread* m_thread;
#else
    void* m_thread;
#endif

};

#endif // ASYNCWEBSOCKET_H
