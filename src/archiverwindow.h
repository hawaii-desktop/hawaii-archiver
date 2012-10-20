/****************************************************************************
 * This file is part of Archiver.
 *
 * Copyright (c) 2012 Pier Luigi Fiorini
 *
 * Author(s):
 *    Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * $BEGIN_LICENSE:GPL$
 *
 * Archiver is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Archiver is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Archiver.  If not, see <http://www.gnu.org/licenses/>.
 *
 * $END_LICENSE$
 ***************************************************************************/

#ifndef ARCHIVERWINDOW_H
#define ARCHIVERWINDOW_H

#include <QMainWindow>

namespace Ui
{
    class ArchiverWindow;
}

class ArchiveModel;

class ArchiverWindow : public QMainWindow
{
    Q_OBJECT
    Q_PROPERTY(bool busy READ isBusy WRITE setBusy)
public:
    explicit ArchiverWindow(QWidget *parent = 0);
    ~ArchiverWindow();

    bool isBusy() const;
    void setBusy(bool busy);

private:
    Ui::ArchiverWindow *ui;
    ArchiveModel *m_model;
    bool m_busy;

private slots:
    void slotFileNew();
    void slotFileOpen();
    void slotFileSaveAs();
    void slotActionAddFile();
    void slotActionAddFolder();
    void slotActionDelete();
    void slotActionExtract();
    void slotHelpAbout();
};

#endif // ARCHIVERWINDOW_H