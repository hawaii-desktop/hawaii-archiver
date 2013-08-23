/****************************************************************************
 * This file is part of Archiver.
 *
 * Copyright (C) 2012-2013 Pier Luigi Fiorini <pierluigi.fiorini@gmail.com>
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

#include <QAction>
#include <QFileDialog>
#include <QMessageBox>

#include "archiverwindow.h"
#include "ui_archiverwindow.h"
#include "archivemodel.h"

ArchiverWindow::ArchiverWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ArchiverWindow)
    , m_busy(false)
{
    ui->setupUi(this);

    // Set the archive model
    m_model = new ArchiveModel(this);
    ui->treeView->setModel(m_model);

    connect(ui->actionFileNew, SIGNAL(triggered()),
            this, SLOT(slotFileNew()));
    connect(ui->actionFileOpen, SIGNAL(triggered()),
            this, SLOT(slotFileOpen()));
    connect(ui->actionFileSaveAs, SIGNAL(triggered()),
            this, SLOT(slotFileSaveAs()));
    connect(ui->actionFileQuit, SIGNAL(triggered()),
            this, SLOT(close()));
    connect(ui->actionActionAddFile, SIGNAL(triggered()),
            this, SLOT(slotActionAddFile()));
    connect(ui->actionActionAddFolder, SIGNAL(triggered()),
            this, SLOT(slotActionAddFolder()));
    connect(ui->actionActionDelete, SIGNAL(triggered()),
            this, SLOT(slotActionDelete()));
    connect(ui->actionActionExtract, SIGNAL(triggered()),
            this, SLOT(slotActionExtract()));
    connect(ui->actionHelpAbout, SIGNAL(triggered()),
            this, SLOT(slotHelpAbout()));
}

ArchiverWindow::~ArchiverWindow()
{
    delete ui;
}

bool ArchiverWindow::isBusy() const
{
    return m_busy;
}

void ArchiverWindow::setBusy(bool busy)
{
    m_busy = busy;

    foreach(QAction * action, actions())
    action->setEnabled(busy);
}

void ArchiverWindow::slotFileNew()
{

}

void ArchiverWindow::slotFileOpen()
{
    QString fileName =
        QFileDialog::getOpenFileName(this, tr("Open Archive"),
                                     QString(), tr("Tar Archives (*.tar *.tar.gz *.tar.xz *.tar.bz2 *.tgz *.tbz2"));
    if (!fileName.isEmpty()) {
        setBusy(true);

        if (m_model->openArchive(fileName)) {
            setWindowModified(false);
            setWindowFilePath(fileName);

            setBusy(false);

            ui->actionFileSaveAs->setEnabled(false);
        }
    }
}

void ArchiverWindow::slotFileSaveAs()
{

}

void ArchiverWindow::slotActionAddFile()
{

}

void ArchiverWindow::slotActionAddFolder()
{

}

void ArchiverWindow::slotActionDelete()
{

}

void ArchiverWindow::slotActionExtract()
{

}

void ArchiverWindow::slotHelpAbout()
{
    QMessageBox::about(this, "", tr("Archive manager."));
}

#include "moc_archiverwindow.cpp"
