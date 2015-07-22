#include "mapwindow.hpp"

#include <QApplication>

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QMapboxGLSettings settings;
    settings.setAccessToken(qgetenv("MAPBOX_ACCESS_TOKEN"));
    settings.setCacheDatabasePath("/tmp/mbgl-cache.db");
    settings.setCacheDatabaseMaximumSize(20 * 1024 * 1024);

    MapWindow window(settings);

    window.resize(800, 600);
    window.show();

    return app.exec();
}
