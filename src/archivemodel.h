/****************************************************************************
 * This file is part of Archiver.
 *
 * Copyright (C) 2012 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#ifndef ARCHIVEMODEL_H
#define ARCHIVEMODEL_H

#include <QAbstractItemModel>

class VArchive;
class VArchiveDirectory;
class VArchiveEntry;

class ArchiveNode;
class ArchiveDirNode;

typedef QHash<int, QVariant> ArchiveEntry;

class ArchiveModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    /**
     * Meta data related to one entry in a compressed archive.
     *
     * When creating a plugin, information about every single entry in
     * an archive is contained in an ArchiveEntry, and metadata
     * is set with the entries in this enum.
     *
     * Please notice that not all archive formats support all the properties
     * below, so set those that are available.
     */
    enum EntryMetaDataType {
        FileName = 0,        /**< The entry's file name */
        InternalID,          /**< The entry's ID for Ark's internal manipulation */
        Permissions,         /**< The entry's permissions */
        Owner,               /**< The user the entry belongs to */
        Group,               /**< The user group the entry belongs to */
        Size,                /**< The entry's original size */
        CompressedSize,      /**< The compressed size for the entry */
        Link,                /**< The entry is a symbolic link */
        Ratio,               /**< The compression ratio for the entry */
        CRC,                 /**< The entry's CRC */
        Method,              /**< The compression method used on the entry */
        Version,             /**< The archiver version needed to extract the entry */
        Timestamp,           /**< The timestamp for the current entry */
        IsDirectory,         /**< The entry is a directory */
        Comment,
        IsPasswordProtected, /**< The entry is password-protected */
        Custom = 1048576
    };

    explicit ArchiveModel(QObject *parent = 0);
    ~ArchiveModel();

    QVariant data(const QModelIndex &index, int role) const;
    Qt::ItemFlags flags(const QModelIndex &index) const;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;
    int rowCount(const QModelIndex &parent) const;
    int columnCount(const QModelIndex &parent) const;

    virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);

    bool openArchive(const QString &fileName);

private:
    ArchiveDirNode *m_rootNode;
    QList<int> m_showColumns;
    VArchive *m_archive;

    bool processDirectory(const VArchiveDirectory *dir,
                          const QString &prefix = QString());
    void createEntryFor(const VArchiveEntry *entry, const QString &prefix);
    void processEntry(ArchiveEntry entry);
    ArchiveDirNode *parentFor(const ArchiveEntry &entry);
    QModelIndex indexForNode(ArchiveNode *node);
    void insertNode(ArchiveNode *node);
    int childCount(const QModelIndex &index, int &dirs, int &files) const;
};

#endif // ARCHIVEMODEL_H
