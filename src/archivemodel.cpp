/****************************************************************************
 * This file is part of Archiver.
 *
 * Copyright (C) 2012-2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 * Copyright (C) 2008-2009 Harald Hvaal <haraldhv@stud.ntnu.no>
 * Copyright (C) 2007 Henrique Pinto <henrique.pinto@kdemail.net>
 *
 * Author(s):
 *    Pier Luigi Fiorini
 *    Harald Hvaal
 *    Henrique Pinto
 *
 * $BEGIN_LICENSE:GPL2+$
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#include <QDebug>
#include <QFileInfo>
#include <QDateTime>
#include <QLocale>
#include <QIcon>
#include <QFont>
#include <QVariant>
#include <QMimeDatabase>
#include <QMimeType>

#include <VArchive>
#include <VFileString>

#include "archivemodel.h"

static ArchiveNode *s_previousMatch = 0;
Q_GLOBAL_STATIC(QStringList, s_previousPieces)

/*
 * ArchiveNode
 */

class ArchiveNode
{
public:
    ArchiveNode(ArchiveDirNode *parent, const ArchiveEntry &entry) :
        m_parent(parent) {
        setEntry(entry);
    }

    virtual ~ArchiveNode() {
    }

    const ArchiveEntry &entry() const {
        return m_entry;
    }

    void setEntry(const ArchiveEntry &entry) {
        m_entry = entry;

        const QStringList pieces = entry[ArchiveModel::FileName].toString().split(QLatin1Char('/'), QString::SkipEmptyParts);
        m_name = pieces.isEmpty() ? QString() : pieces.last();

        if (m_entry[ArchiveModel::IsDirectory].toBool())
            m_icon = QIcon::fromTheme("folder");
        else {
            QMimeDatabase mimeDatabase;
            QMimeType mime = mimeDatabase.mimeTypeForFile(m_entry[ArchiveModel::FileName].toString());
            if (mime.isValid())
                m_icon = QIcon::fromTheme(mime.iconName());
            else
                m_icon = QIcon::fromTheme("unknown");
        }
    }

    ArchiveDirNode *parent() const {
        return m_parent;
    }

    int row() const;

    virtual bool isDirectory() const {
        return false;
    }

    QIcon icon() const {
        return m_icon;
    }

    QString name() const {
        return m_name;
    }

protected:
    void setIcon(const QIcon &icon) {
        m_icon = icon;
    }

private:
    ArchiveEntry m_entry;
    QIcon m_icon;
    QString m_name;
    ArchiveDirNode *m_parent;
};

/*
 * ArchiveDirNode
 */

class ArchiveDirNode: public ArchiveNode
{
public:
    ArchiveDirNode(ArchiveDirNode *parent, const ArchiveEntry &entry) :
        ArchiveNode(parent, entry) {
    }

    ~ArchiveDirNode() {
        clear();
    }

    // TODO: Do not return a reference, add methods to change m_entries.
    //       Returning a reference makes the code less readable, as m_entries,
    //       which is private, ends up being changed all over this file.
    QList<ArchiveNode *> &entries() {
        return m_entries;
    }

    virtual bool isDirectory() const {
        return true;
    }

    ArchiveNode *find(const QString &name) {
        foreach(ArchiveNode * node, m_entries) {
            if (node && (node->name() == name))
                return node;
        }

        return 0;
    }

    ArchiveNode *findByPath(const QStringList &pieces, int index = 0) {
        if (index == pieces.count())
            return 0;

        ArchiveNode *next = find(pieces.at(index));

        if (index == pieces.count() - 1)
            return next;
        if (next && next->isDirectory())
            return static_cast<ArchiveDirNode *>(next)->findByPath(pieces, index + 1);

        return 0;
    }

    void returnDirNodes(QList<ArchiveDirNode *> *store) {
        foreach(ArchiveNode * node, m_entries) {
            if (node->isDirectory()) {
                store->prepend(static_cast<ArchiveDirNode *>(node));
                static_cast<ArchiveDirNode *>(node)->returnDirNodes(store);
            }
        }
    }

    void clear() {
        qDeleteAll(m_entries);
        m_entries.clear();
    }

private:
    QList<ArchiveNode *> m_entries;
};

int ArchiveNode::row() const
{
    if (parent())
        return parent()->entries().indexOf(const_cast<ArchiveNode *>(this));
    return 0;
}

/*
 * ArchiveModelSorter
 */

class ArchiveModelSorter
{
public:
    ArchiveModelSorter(int column, Qt::SortOrder order)
        : m_sortColumn(column)
        , m_sortOrder(order) {
    }

    virtual ~ArchiveModelSorter() {
    }

    inline bool operator()(const QPair<ArchiveNode *, int> &left, const QPair<ArchiveNode *, int> &right) const {
        if (m_sortOrder == Qt::AscendingOrder) {
            return lessThan(left, right);
        } else {
            return !lessThan(left, right);
        }
    }

protected:
    bool lessThan(const QPair<ArchiveNode *, int> &left, const QPair<ArchiveNode *, int> &right) const {
        const ArchiveNode *const leftNode = left.first;
        const ArchiveNode *const rightNode = right.first;

        // Sort folders before files
        if ((leftNode->isDirectory()) && (!rightNode->isDirectory())) {
            return (m_sortOrder == Qt::AscendingOrder);
        } else if ((!leftNode->isDirectory()) && (rightNode->isDirectory())) {
            return !(m_sortOrder == Qt::AscendingOrder);
        }

        const QVariant &leftEntry = leftNode->entry()[m_sortColumn];
        const QVariant &rightEntry = rightNode->entry()[m_sortColumn];

        switch (m_sortColumn) {
            case ArchiveModel::FileName:
                return leftNode->name() < rightNode->name();
            case ArchiveModel::Size:
            case ArchiveModel::CompressedSize:
                return leftEntry.toInt() < rightEntry.toInt();
            default:
                return leftEntry.toString() < rightEntry.toString();
        }

        // We should not get here.
        Q_ASSERT(false);
    }

private:
    int m_sortColumn;
    Qt::SortOrder m_sortOrder;
};

/*
 * ArchiveModel
 */

ArchiveModel::ArchiveModel(QObject *parent) :
    QAbstractItemModel(parent),
    m_rootNode(new ArchiveDirNode(0, ArchiveEntry())),
    m_archive(0)
{
}

ArchiveModel::~ArchiveModel()
{
    delete m_rootNode;

    if (m_archive) {
        m_archive->close();
        delete m_archive;
    }
}

QVariant ArchiveModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    ArchiveNode *node = static_cast<ArchiveNode *>(index.internalPointer());
    switch (role) {
        case Qt::DisplayRole: {
            // TODO: cols
            int columnId = m_showColumns.at(index.column());
            switch (columnId) {
                case FileName:
                    return node->name();
                case Size:
                    if (node->isDirectory()) {
                        int dirs, files;
                        if (childCount(index, dirs, files) == 0)
                            return tr("%1 folders, %2 filers").arg(dirs).arg(files);
                        return QVariant();
                    } else if (node->entry().contains(Link)) {
                        return QVariant();
                    } else {
                        return VFileString::forSize(node->entry()[Size].toReal());
                        //return KIO::convertSize(node->entry()[ Size ].toULongLong());
                    }
                case CompressedSize:
                    if (node->isDirectory() || node->entry().contains(Link)) {
                        return QVariant();
                    } else {
                        qulonglong compressedSize = node->entry()[CompressedSize].toULongLong();
                        if (compressedSize != 0)
                            return VFileString::forSize((qreal)compressedSize);
                        else
                            return QVariant();
                    }
                case Ratio: // TODO: Use node->entry()[Ratio] when available
                    if (node->isDirectory() || node->entry().contains(Link)) {
                        return QVariant();
                    } else {
                        qulonglong compressedSize = node->entry()[CompressedSize].toULongLong();
                        qulonglong size = node->entry()[Size].toULongLong();
                        if (compressedSize == 0 || size == 0) {
                            return QVariant();
                        } else {
                            int ratio = int(100 * ((double)size - compressedSize) / size);
                            return QString(QString::number(ratio) + QLatin1String(" %"));
                        }
                    }
                case Timestamp: {
                    const QDateTime timeStamp = node->entry().value(Timestamp).toDateTime();
                    QLocale locale;
                    return locale.toString(timeStamp, QLocale::ShortFormat);
                }
                default:
                    return node->entry().value(columnId);
            }
            break;
        }
        case Qt::DecorationRole:
            if (index.column() == 0)
                return node->icon();
            return QVariant();
        case Qt::FontRole: {
            QFont f;
            f.setItalic(node->entry()[IsPasswordProtected].toBool());
            return f;
        }
        default:
            return QVariant();
    }
}

Qt::ItemFlags ArchiveModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags defaultFlags = QAbstractItemModel::flags(index);

    if (index.isValid())
        return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled | defaultFlags;

    return defaultFlags;
}

QVariant ArchiveModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (section >= m_showColumns.size()) {
        qDebug() << "WEIRD: showColumns.size = " << m_showColumns.size()
                 << " and section = " << section;
        return QVariant();
    }

    int columnId = m_showColumns.at(section);

    switch (columnId) {
        case FileName:
            return tr("Name", "Name of a file inside an archive");
        case Size:
            return tr("Size", "Uncompressed size of a file inside an archive");
        case CompressedSize:
            return tr("Compressed", "Compressed size of a file inside an archive");
        case Ratio:
            return tr("Rate", "Compression rate of file");
        case Owner:
            return tr("Owner", "File's owner username");
        case Group:
            return tr("Group", "File's group");
        case Permissions:
            return tr("Mode", "File permissions");
        case CRC:
            return tr("CRC", "CRC hash code");
        case Method:
            return tr("Method", "Compression method");
        case Version:
            return tr("Version", "File version");
        case Timestamp:
            return tr("Date", "Timestamp");
        case Comment:
            return tr("Comment", "File comment");
        default:
            return tr("Unknown", "Unnamed column");
    }

    return QVariant();
}

QModelIndex ArchiveModel::index(int row, int column, const QModelIndex &parent) const
{
    if (hasIndex(row, column, parent)) {
        ArchiveDirNode *parentNode = parent.isValid()
                                     ? static_cast<ArchiveDirNode *>(parent.internalPointer())
                                     : m_rootNode;

        Q_ASSERT(parentNode->isDirectory());

        ArchiveNode *item = parentNode->entries().value(row, 0);
        if (item)
            return createIndex(row, column, item);
    }

    return QModelIndex();
}

QModelIndex ArchiveModel::parent(const QModelIndex &child) const
{
    if (child.isValid()) {
        ArchiveNode *item = static_cast<ArchiveNode *>(child.internalPointer());
        Q_ASSERT(item);
        if (item->parent() && (item->parent() != m_rootNode))
            return createIndex(item->parent()->row(), 0, item->parent());
    }

    return QModelIndex();
}

int ArchiveModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() <= 0) {
        ArchiveNode *parentNode = parent.isValid() ? static_cast<ArchiveNode *>(parent.internalPointer()) : m_rootNode;

        if (parentNode && parentNode->isDirectory())
            return static_cast<ArchiveDirNode *>(parentNode)->entries().count();
    }
    return 0;
}

int ArchiveModel::columnCount(const QModelIndex &parent) const
{
    return m_showColumns.size();
}

void ArchiveModel::sort(int column, Qt::SortOrder order)
{
    if (m_showColumns.size() <= column)
        return;

    emit layoutAboutToBeChanged();

    QList<ArchiveDirNode *> dirNodes;
    m_rootNode->returnDirNodes(&dirNodes);
    dirNodes.append(m_rootNode);

    const ArchiveModelSorter modelSorter(m_showColumns.at(column), order);

    foreach(ArchiveDirNode * dir, dirNodes) {
        QVector < QPair<ArchiveNode *, int> > sorting(dir->entries().count());
        for (int i = 0; i < dir->entries().count(); ++i) {
            ArchiveNode *item = dir->entries().at(i);
            sorting[i].first = item;
            sorting[i].second = i;
        }

        qStableSort(sorting.begin(), sorting.end(), modelSorter);

        QModelIndexList fromIndexes;
        QModelIndexList toIndexes;
        for (int r = 0; r < sorting.count(); ++r) {
            ArchiveNode *item = sorting.at(r).first;
            toIndexes.append(createIndex(r, 0, item));
            fromIndexes.append(createIndex(sorting.at(r).second, 0, sorting.at(r).first));
            dir->entries()[r] = sorting.at(r).first;
        }

        changePersistentIndexList(fromIndexes, toIndexes);

        emit dataChanged(
            index(0, 0, indexForNode(dir)),
            index(dir->entries().size() - 1, 0, indexForNode(dir)));
    }

    emit layoutChanged();
}

bool ArchiveModel::openArchive(const QString &fileName)
{
    QFileInfo fileInfo(fileName);
    QIODevice::OpenMode mode;

    // Determine open mode
    if (fileInfo.isWritable())
        mode = QIODevice::ReadWrite;
    else
        mode = QIODevice::ReadOnly;

    // Close previously open archive
    if (m_archive) {
        m_archive->close();
        delete m_archive;
    }

    // Open the archive
    m_archive = new VArchive(fileName);
    if (!m_archive->isOpen()) {
        if (!m_archive->open(QIODevice::ReadOnly))
            return false;

        processDirectory(m_archive->directory());
    }

    return true;
}

bool ArchiveModel::processDirectory(const VArchiveDirectory *dir, const QString &prefix)
{
    foreach(const QString & entryName, dir->entries()) {
        const VArchiveEntry *entry = dir->entry(entryName);
        createEntryFor(entry, prefix);

        if (entry->isDirectory()) {
            QString newPrefix = (prefix.isEmpty() ? prefix : prefix + QLatin1Char('/')) + entryName;
            processDirectory(static_cast<const VArchiveDirectory *>(entry), newPrefix);
        }
    }

    return true;
}

void ArchiveModel::createEntryFor(const VArchiveEntry *entry, const QString &prefix)
{
    ArchiveEntry e;
    e[FileName] = prefix.isEmpty() ? entry->name() : prefix + QLatin1Char('/') + entry->name();
    e[InternalID] = e[FileName];
    e[Permissions] = VFileString::forPermissions(entry->permissions());
    e[Owner] = entry->user();
    e[Group] = entry->group();
    e[IsDirectory] = entry->isDirectory();
    e[Timestamp] = entry->datetime();
    if (!entry->symLinkTarget().isEmpty())
        e[Link] = entry->symLinkTarget();
    if (entry->isFile())
        e[Size] = static_cast<const VArchiveFile *>(entry)->size();
    else
        e[Size] = 0;

    processEntry(e);
}

void ArchiveModel::processEntry(ArchiveEntry entry)
{
    if (m_showColumns.isEmpty()) {
        static const QList<int> columns =
            QList<int>()
            << FileName
            << Size
            << CompressedSize
            << Permissions
            << Owner
            << Group
            << Ratio
            << CRC
            << Method
            << Version
            << Timestamp
            << Comment;
        QList<int> toInsert;

        foreach(int column, columns) {
            if (entry.contains(column) && entry[column].isValid())
                toInsert << column;
        }

        beginInsertColumns(QModelIndex(), 0, toInsert.size() - 1);
        m_showColumns << toInsert;
        endInsertColumns();
    }

    // Skip existing nodes
    if (m_rootNode) {
        ArchiveNode *existing = m_rootNode->findByPath(entry[FileName].toString().split(QLatin1Char('/')));
        if (existing) {
            // Multi-volume files are repeated at least in RAR archives.
            // In that case, we need to sum the compressed size for each volume
            qulonglong currentCompressedSize = existing->entry()[CompressedSize].toULongLong();
            entry[CompressedSize] = currentCompressedSize + entry[CompressedSize].toULongLong();

            //TODO: benchmark whether it's a bad idea to reset the entry here.
            existing->setEntry(entry);
            return;
        }
    }

    // Find parent node creating missing ArchiveDirNodes in the process
    ArchiveDirNode *parent = parentFor(entry);
    QString entryName = entry[FileName].toString().split(QLatin1Char('/'), QString::SkipEmptyParts).last();
    ArchiveNode *node = parent->find(entryName);
    if (node)
        node->setEntry(entry);
    else {
        if (entryName.endsWith(QLatin1Char('/')) ||
                (entry.contains(IsDirectory) && entry[IsDirectory].toBool()))
            node = new ArchiveDirNode(parent, entry);
        else
            node = new ArchiveNode(parent, entry);
        insertNode(node);
    }
}

ArchiveDirNode *ArchiveModel::parentFor(const ArchiveEntry &entry)
{
    QStringList pieces = entry[ FileName ].toString().split(QLatin1Char('/'), QString::SkipEmptyParts);
    if (pieces.isEmpty())
        return 0;
    pieces.removeLast();

#if 0
    if (s_previousMatch) {
        // The number of path elements must be the same for the shortcut to work
        if (s_previousPieces->count() == pieces.count()) {
            bool equal = true;

            // Make sure all the pieces match up
            for (int i = 0; i < s_previousPieces->count(); ++i) {
                if (s_previousPieces->at(i) != pieces.at(i)) {
                    equal = false;
                    break;
                }
            }

            // If match return it
            if (equal)
                return static_cast<ArchiveDirNode *>(s_previousMatch);
        }
    }
#endif

    ArchiveDirNode *parent = m_rootNode;

    foreach(const QString & piece, pieces) {
        ArchiveNode *node = parent->find(piece);

        if (!node) {
            ArchiveEntry e;
            e[FileName] = (parent == m_rootNode) ?
                          piece : parent->entry()[FileName].toString() + QLatin1Char('/') + piece;
            e[IsDirectory] = true;
            node = new ArchiveDirNode(parent, e);
            insertNode(node);
        }

        if (!node->isDirectory()) {
            ArchiveEntry e(node->entry());
            node = new ArchiveDirNode(parent, e);

            // Maybe we have both a file and a directory of the same name, avoid
            // avoid removing previous entries unless necessary
            insertNode(node);
        }
        parent = static_cast<ArchiveDirNode *>(node);
    }

#if 0
    s_previousMatch = parent;
    *s_previousPieces = pieces;
#endif

    return parent;
}

QModelIndex ArchiveModel::indexForNode(ArchiveNode *node)
{
    Q_ASSERT(node);
    if (node != m_rootNode) {
        Q_ASSERT(node->parent());
        Q_ASSERT(node->parent()->isDirectory());
        return createIndex(node->row(), 0, node);
    }
    return QModelIndex();
}

void ArchiveModel::insertNode(ArchiveNode *node)
{
    Q_ASSERT(node);
    ArchiveDirNode *parent = node->parent();
    Q_ASSERT(parent);
    //beginInsertRows(indexForNode(parent), parent->entries().count(), parent->entries().count());
    parent->entries().append(node);
    //endInsertRows();
}

int ArchiveModel::childCount(const QModelIndex &index, int &dirs, int &files) const
{
    if (index.isValid()) {
        dirs = files = 0;
        ArchiveNode *item = static_cast<ArchiveNode *>(index.internalPointer());
        Q_ASSERT(item);
        if (item->isDirectory()) {
            const QList<ArchiveNode *> entries = static_cast<ArchiveDirNode *>(item)->entries();
            foreach(const ArchiveNode * node, entries) {
                if (node->isDirectory())
                    dirs++;
                else
                    files++;
            }

            return entries.count();
        }

        return 0;
    }

    return -1;
}

#include "moc_archivemodel.cpp"
