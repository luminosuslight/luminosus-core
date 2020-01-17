#include "FileSystemManager.h"

#include <QObject>
#include <QDir>
#include <QFile>
#include <QCborMap>
#include <QCborArray>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QString>
#include <QStandardPaths>
#include <QDebug>


FileSystemManager::FileSystemManager()
	: QObject()
{
    if (QSysInfo::productType() == "android") {
        //m_dataRoot = "file:///sdcard/luminosus_data/";
        m_dataRoot = "";
    } else {
        m_dataRoot = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        if (!m_dataRoot.endsWith("/")) {
            m_dataRoot += "/";
        }
    }
    QDir().mkpath(m_dataRoot);
    QDir().mkpath(m_dataRoot + "projects");
    QDir().mkpath(m_dataRoot + "combinations");
    qDebug() << "App data directory: " << m_dataRoot;
}

QString FileSystemManager::saveFile(QString dir, QString filename, QByteArray content) const {
    QString path = QString(m_dataRoot);
    if (dir.length()) {
        path += dir + "/";
        QDir().mkpath(path);
    }
    return saveLocalFile(path + filename, content);
}

QString FileSystemManager::saveLocalFile(QString path, QByteArray content) const {
    QFile saveFile(path);
    if (!saveFile.open(QIODevice::WriteOnly)) {
        qWarning() << "Couldn't write to file " + path + ": " << saveFile.errorString();
        return {};
    }
    saveFile.write(content);
    saveFile.close();
    return path;
}

QString FileSystemManager::saveFile(QString dir, QString filename, QCborMap content) const {
    QByteArray raw = content.toCborValue().toCbor();
    return saveFile(dir, filename, raw);
}

QString FileSystemManager::saveFile(QString dir, QString filename, QCborArray content) const {
    QByteArray raw = content.toCborValue().toCbor();
    return saveFile(dir, filename, raw);
}

QByteArray FileSystemManager::loadFile(QString dir, QString filename) const {
    QString path = getDataDir(dir) + filename;
    return loadLocalFile(path);
}

QByteArray FileSystemManager::loadLocalFile(QString path) const {
    if (!path.startsWith("assets-library:") && !QDir().exists(path)) {
        qWarning() << "File " + path + " does not exist.";
        return QByteArray();
    }
    QFile loadFile(path);
    if (!loadFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Couldn't open file " + path + ".";
        return QByteArray();
    }
    QByteArray content = loadFile.readAll();
    loadFile.close();
    return content;
}

QCborMap FileSystemManager::loadCborMap(QString dir, QString filename) const {
    QByteArray content = loadFile(dir, filename);
    QCborMap cborMap = QCborValue::fromCbor(content).toMap();
    if (cborMap.isEmpty()) {
        // backward compatibility / migration path: check if file is in JSON format:
        QJsonDocument loadDoc(QJsonDocument::fromJson(content));
        cborMap = QCborMap::fromJsonObject(loadDoc.object());
    }
    return cborMap;
}

QCborArray FileSystemManager::loadCborArray(QString dir, QString filename) const {
    QByteArray content = loadFile(dir, filename);
    QCborArray cborArray = QCborValue::fromCbor(content).toArray();
    if (cborArray.isEmpty()) {
        // backward compatibility / migration path: check if file is in JSON format:
        QJsonDocument loadDoc(QJsonDocument::fromJson(content));
        cborArray = QCborArray::fromJsonArray(loadDoc.array());
    }
    return cborArray;
}

bool FileSystemManager::fileExists(QString dir, QString filename) const {
    QString path = getDataDir(dir) + filename;
	return QDir().exists(path);
}

QStringList FileSystemManager::getFilenames(QString dir, QString filter) const {
	QString path = getDataDir(dir);
	return QDir(path).entryList(QStringList {filter});
}

void FileSystemManager::deleteFile(QString dir, QString filename) const {
    QString path = getDataDir(dir) + filename;
    deleteLocalFile(path);
}

void FileSystemManager::deleteLocalFile(QString path) const {
    if (!QDir().exists(path)) return;
    QFile::remove(path);
}

void FileSystemManager::importFile(QString inputPath, QString dir, bool overwrite) const {
    QString outputPath = getDataDir(dir) + QFileInfo(inputPath).fileName();
    if (QDir().exists(outputPath)) {
        if (overwrite) {
            QFile::remove(outputPath);
        } else {
            return;
        }
    }
    QFile::copy(inputPath, outputPath);
    // if inputPath is a resource file (":/...") fix the permissions:
    if (inputPath.startsWith(":")) {
        QFile(outputPath).setPermissions(QFile::ReadOwner|QFile::WriteOwner
                                         |QFile::ReadGroup|QFile::ReadOther);
    }
}

void FileSystemManager::exportFile(QString dir, QString filename, QString outputPath, bool overwrite) const {
    QString inputPath = getDataDir(dir) + filename;
    if (QDir().exists(outputPath)) {
        if (overwrite) {
            QFile::remove(outputPath);
        } else {
            return;
        }
    }
    QFile::copy(inputPath, outputPath);
}

bool FileSystemManager::appendToFile(QString dir, QString filename, QString text) const {
    QString path = QString(m_dataRoot);
    if (dir.length()) {
        path += dir + "/";
        QDir().mkpath(path);
    }
    QFile saveFile(path + filename);
    if (!saveFile.open(QIODevice::Append)) {
        qWarning() << "Couldn't write to file " + path + filename + ": " << saveFile.errorString();
        return false;
    }
    saveFile.write(text.toUtf8());
    saveFile.close();
    return true;
}

QString& FileSystemManager::removeFilePrefix(QString& path) const {
#ifdef Q_OS_WIN
    // under Windows, filename often starts with three slashes:
    if (path.startsWith("file:///")) {
        path.remove("file:///");
    }
#endif
    if (path.startsWith("file://")) {
        path.remove("file://");
    }
    return path;
}

QString FileSystemManager::withoutFilePrefix(const QString& path) const {
    QString filePath = path;
    removeFilePrefix(filePath);
    return filePath;
}

QString FileSystemManager::getDataDir(QString dir) const {
    if (dir.length()) {
        return m_dataRoot + dir + "/";
    }
    return m_dataRoot;
}

QString FileSystemManager::getDir(QString dir, QString filename) const {
    return getDataDir(dir) + filename;
}
