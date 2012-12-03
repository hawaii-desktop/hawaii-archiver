/****************************************************************************
 * This file is part of Archiver.
 *
 * Copyright (C) 2012 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
 *
 * Author(s):
 *    Pier Luigi Fiorini
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
