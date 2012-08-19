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

#include <VApplication>

#include "archiverwindow.h"

int main(int argc, char *argv[])
{
    VApplication app(argc, argv);
    app.setApplicationName("Archiver");
    app.setApplicationVersion("0.0.0");
    app.setOrganizationDomain("maui-project.org");
    app.setOrganizationName("Maui Project");
    app.setIdentifier("org.maui.Archiver");
    app.setWindowIcon(QIcon::fromTheme("utilities-file-archiver"));

    ArchiverWindow *window = new ArchiverWindow;
    window->show();

    return app.exec();
}
