#ifndef BLOCKLIST_H
#define BLOCKLIST_H

#include "core/block_basics/BlockInterface.h"
#include "core/helpers/utils.h"

#include <QObject>
#include <QString>
#include <QVector>
#include <map>

class CoreController;

/**
 * @brief The BlockList class manages the information about all available block types.
 */
class BlockList : public QObject {

	Q_OBJECT

private:
    /**
     * @brief BlockList creates a BlockList object and adds all available blocks
     */
    explicit BlockList();

public:
    static BlockList& getInstance();

public slots:
	/**
	 * @brief blockExists checks if a block type is available
	 * @param name type of the block to check
	 * @return true if the block is available
	 */
	bool blockExists(QString name) const;

	/**
	 * @brief getBlockInfoByName returns the BlockInfo of a block by its type name
	 * @param name type of the block to check
	 * @return BlockInfo struct
	 */
	const BlockInfo& getBlockInfoByName(QString name) const;

    /**
     * @brief getNameInUi return the name of the blokc to show in the UI
     * @param name type of the block
     * @return name in UI as QString
     */
    QString getNameInUi(QString name) const;

	/**
	 * @brief getJsonBlockModel creates a JSON array of all available blocks
	 * @return the string of a JSON array
	 */
    QString getJsonBlockModel(bool developerMode) const;

	/**
	 * @brief getSearchResult creates a JSON array of all blocks that match the query
	 * @param query to find matching blocks
	 * @return the string of a JSON array
	 */
    QString getSearchResult(QString query, bool developerMode) const;

	/**
	 * @brief getAllBlockTypes returns a list of all available block type names (i.e. for debugging)
	 * @return a list of block type names
	 */
    QStringList getAllBlockTypes() const;

    /**
     * @brief addBlock adds the info of a single block to m_blockNames
     * @param info Block info struct to add
     */
    bool addBlock(const BlockInfo& info);

private:
	/**
	 * @brief checkAvailableDependencies adds all available dependencies to m_availableDependencies
	 */
    void checkAvailableDependencies();

	/**
	 * @brief blockMatchesQuery checks if a block matches a query
	 * (i.e. the query is part of the name etc.)
	 * @param blockInfo the block
	 * @param query i.e. text from a search field (should be all lower case)
	 * @return true if the block matches that query
	 */
	bool blockMatchesQuery(const BlockInfo& info, const QString& query) const;

    bool blockIsVisible(const BlockInfo& info, bool developerMode) const;


protected:
	/**
     * @brief m_availabilityRequirements is a set of all available hard- and software dependencies
	 */
    QSet<AvailabilityRequirement> m_availabilityRequirements;

    QSet<VisibilityRequirement> m_visibilityRequirements;

	/**
	 * @brief m_blockNames maps the block type name to its BlockInfo struct
	 */
	std::map<QString, BlockInfo> m_blockNames;

    /**
     * @brief m_orderedBlockList contains all BlockInfos in the order they should appear in the UI
     */
    QVector<BlockInfo> m_orderedBlockList;

	/**
	 * @brief m_jsonBlockModel is the cached result of getJsonBlockModel for all blocks
	 *
	 * Is mutable because it is a cached result that doesn't change the objects state.
	 */
	mutable QString m_jsonBlockModel;

    /**
     * @brief m_jsonBlockModelDeveloperMode true if developer mode was enabled when model was created
     */
    mutable bool m_jsonBlockModelDeveloperMode;
};



#endif // BLOCKLIST_H

