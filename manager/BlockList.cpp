#include "BlockList.h"

#include "core/CoreController.h"

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

/*
 * Attention: addAllBlocks() is defined in BlockList_block.cpp!
 */


BlockList::BlockList(CoreController* controller)
    : QObject(controller)
    , m_controller(controller)
    , m_jsonBlockModelDeveloperMode(false)
{
	checkAvailableDependencies();
	addAllBlocks();
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

QString BlockList::getJsonBlockModel() const {
	// available blocks do not change at runtime
    // -> create model only if it doesn't already exist:
    if (m_jsonBlockModel.isEmpty() || m_jsonBlockModelDeveloperMode != m_controller->getDeveloperMode()) {
		QJsonArray model;
        for (const BlockInfo& info: m_orderedBlockList) {
            if (!blockIsVisible(info)) continue;
			QJsonObject jsonInfo;
			jsonInfo["name"] = info.typeName;
			jsonInfo["nameInUi"] = info.nameInUi;
			jsonInfo["category"] = QJsonArray::fromStringList(info.category);
			model.append(jsonInfo);
		}
		m_jsonBlockModel = QJsonDocument(model).toJson();
        m_jsonBlockModelDeveloperMode = m_controller->getDeveloperMode();
	}
	return m_jsonBlockModel;
}

QString BlockList::getSearchResult(QString query) const {
	QString queryLow = query.toLower();
	QJsonArray result;
    for (const BlockInfo& info: m_orderedBlockList) {
        if (!blockIsVisible(info)) continue;
		// check for query:
		if (blockMatchesQuery(info, queryLow)) {
			// block matches query -> add it to result:
			QJsonObject jsonInfo;
			jsonInfo["name"] = info.typeName;
			jsonInfo["nameInUi"] = info.nameInUi;
			jsonInfo["category"] = QJsonArray::fromStringList(info.category);
			result.append(jsonInfo);
		}
	}
	return QJsonDocument(result).toJson();
}

QStringList BlockList::getAllBlockTypes() const {
	QStringList blockTypes;
	for (auto entry: m_blockNames) {
		blockTypes.append(entry.first);
	}
    return blockTypes;
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

void BlockList::addBlock(const BlockInfo& info) {
	// check dependencies:
    for (AvailabilityRequirement dependency: info.availabilityRequirements) {
        if (!m_availabilityRequirements.contains(dependency)) {
			// dependency is not available
            qInfo() << info.typeName.toStdString().c_str() << "Block is not available on this system.";
			return;
		}
	}
	m_blockNames[info.typeName] = info;
    m_orderedBlockList.append(info);
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

bool BlockList::blockIsVisible(const BlockInfo& info) const {
    // check visibility:
    for (VisibilityRequirement visibility: info.visibilityRequirements) {
		if (m_visibilityRequirements.contains(visibility)
				|| (visibility == VisibilityRequirement::DeveloperMode && m_controller->getDeveloperMode())) {
			continue;
        } else {
            return false;
        }
    }
    return true;
}
