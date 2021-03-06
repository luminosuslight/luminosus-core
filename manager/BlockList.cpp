#include "BlockList.h"

#include "core/CoreController.h"
#include "core/helpers/qstring_literal.h"

#include <QCborArray>
#include <QCborMap>
#include <QJsonArray>
#include <QJsonDocument>


BlockList::BlockList()
    : QObject(nullptr)
    , m_jsonBlockModelDeveloperMode(false)
{
    checkAvailableDependencies();
}

BlockList& BlockList::getInstance() {
    static BlockList blockListSingleton;
    return blockListSingleton;
}

bool BlockList::blockExists(QString name) const {
    return mapContains(m_blockNames, name);
}

const BlockInfo& BlockList::getBlockInfoByName(QString name) const {
	auto it = m_blockNames.find(name);
	if (it == m_blockNames.end()) {
		qCritical() << "Could not find block info for block type: " << name;
        // TODO: replace with dummy stack variable?
		return *(new BlockInfo());
	}
    return it->second;
}

QString BlockList::getNameInUi(QString name) const {
    auto it = m_blockNames.find(name);
    if (it == m_blockNames.end()) {
        qCritical() << "Could not find block info for block type: " << name;
        return "";
    }
    return it->second.nameInUi;
}

QString BlockList::getJsonBlockModel(bool developerMode) const {
	// available blocks do not change at runtime
    // -> create model only if it doesn't already exist:
    if (m_jsonBlockModel.isEmpty() || m_jsonBlockModelDeveloperMode != developerMode) {
		QCborArray model;
        for (const BlockInfo& info: m_orderedBlockList) {
            if (!blockIsVisible(info, developerMode)) continue;
			QCborMap jsonInfo;
            jsonInfo["name"_q] = info.typeName;
            jsonInfo["nameInUi"_q] = info.nameInUi;
            jsonInfo["category"_q] = QCborArray::fromStringList(info.category);
			model.append(jsonInfo);
		}
        m_jsonBlockModel = QJsonDocument(model.toJsonArray()).toJson();
        m_jsonBlockModelDeveloperMode = developerMode;
	}
	return m_jsonBlockModel;
}

QString BlockList::getSearchResult(QString query, bool developerMode) const {
	QString queryLow = query.toLower();
	QCborArray result;
    for (const BlockInfo& info: m_orderedBlockList) {
        if (!blockIsVisible(info, developerMode)) continue;
		// check for query:
		if (blockMatchesQuery(info, queryLow)) {
			// block matches query -> add it to result:
			QCborMap jsonInfo;
            jsonInfo["name"_q] = info.typeName;
            jsonInfo["nameInUi"_q] = info.nameInUi;
            jsonInfo["category"_q] = QCborArray::fromStringList(info.category);
			result.append(jsonInfo);
		}
	}
    return QJsonDocument(result.toJsonArray()).toJson();
}

QStringList BlockList::getAllBlockTypes() const {
	QStringList blockTypes;
	for (auto entry: m_blockNames) {
		blockTypes.append(entry.first);
	}
    return blockTypes;
}

bool BlockList::addBlock(const BlockInfo& info) {
    // check dependencies:
    for (AvailabilityRequirement dependency: info.availabilityRequirements) {
        if (!m_availabilityRequirements.contains(dependency)) {
            // dependency is not available
            qInfo() << info.typeName.toStdString().c_str() << "Block is not available on this system.";
            return false;
        }
    }
    m_blockNames[info.typeName] = info;
    m_orderedBlockList.append(info);
    std::sort(m_orderedBlockList.begin(), m_orderedBlockList.end());
    return true;
}

void BlockList::checkAvailableDependencies() {
    m_availabilityRequirements.insert(AvailabilityRequirement::Experimental);
    m_availabilityRequirements.insert(AvailabilityRequirement::AudioInput);
#ifndef Q_OS_IOS
    m_availabilityRequirements.insert(AvailabilityRequirement::Scripting);
    m_availabilityRequirements.insert(AvailabilityRequirement::LocalFileSelection);
#endif
#ifdef RT_MIDI_AVAILABLE
    m_availabilityRequirements.insert(AvailabilityRequirement::Midi);
#endif
#ifdef Q_OS_LINUX
    m_availabilityRequirements.insert(AvailabilityRequirement::PdfUtils);
#endif
#ifdef Q_OS_WIN
    m_visibilityRequirements.insert(VisibilityRequirement::KeyboardEmulation);
#endif
    m_visibilityRequirements.insert(VisibilityRequirement::CompleteLicense);
    m_visibilityRequirements.insert(VisibilityRequirement::StandaloneVersion);
}

bool BlockList::blockMatchesQuery(const BlockInfo& info, const QString& query) const {
	// depending on length of query:
	if (query.size() < 3) {
		// if query is shorter than 3 characters, look only for beginning of names:
		if (info.typeName.toLower().startsWith(query)) {
			return true;
		} else if (info.nameInUi.toLower().startsWith(query)) {
			return true;
		} else if (!info.category.isEmpty() && info.category[0].toLower().startsWith(query)) {
			return true;
		}
	} else {
		// if query is 3 chars or longer, checks the whole strings for a match:
		if (info.typeName.toLower().contains(query)) {
			return true;
		} else if (info.nameInUi.toLower().contains(query)) {
			return true;
		} else if (!info.category.isEmpty() && info.category[0].toLower().contains(query)) {
			return true;
		}
	}
    return false;
}

bool BlockList::blockIsVisible(const BlockInfo& info, bool developerMode) const {
    // check visibility:
    for (VisibilityRequirement visibility: info.visibilityRequirements) {
		if (m_visibilityRequirements.contains(visibility)
                || (visibility == VisibilityRequirement::DeveloperMode && developerMode)) {
			continue;
        } else {
            return false;
        }
    }
    return true;
}
