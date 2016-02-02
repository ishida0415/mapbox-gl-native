#ifndef QMAPBOXGL_H
#define QMAPBOXGL_H

#include <QMapbox>
#include <QObject>
#include <QPointF>

class QImage;
class QMargins;
class QSize;
class QString;
class QStringList;

class QMapboxGLPrivate;

// This header follows the Qt coding style: https://wiki.qt.io/Qt_Coding_Style

class Q_DECL_EXPORT QMapboxGLSettings
{
public:
    QMapboxGLSettings();

    enum MapMode {
        ContinuousMap = 0,
        StillMap
    };

    enum GLContextMode {
        UniqueGLContext = 0,
        SharedGLContext
    };

    enum ConstrainMode {
        ConstrainHeightOnly = 0,
        ConstrainWidthAndHeight
    };

    MapMode mapMode() const;
    void setMapMode(MapMode);

    GLContextMode contextMode() const;
    void setContextMode(GLContextMode);

    ConstrainMode constrainMode() const;
    void setConstrainMode(ConstrainMode);

    unsigned cacheDatabaseMaximumSize() const;
    void setCacheDatabaseMaximumSize(unsigned);

    QString cacheDatabasePath() const;
    void setCacheDatabasePath(const QString &);

    QString assetPath() const;
    void setAssetPath(const QString &);

    QString accessToken() const;
    void setAccessToken(const QString &);

private:
    MapMode m_mapMode;
    GLContextMode m_contextMode;
    ConstrainMode m_constrainMode;

    unsigned m_cacheMaximumSize;
    QString m_cacheDatabasePath;
    QString m_assetPath;
    QString m_accessToken;
};

class Q_DECL_EXPORT QMapboxGL : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double latitude READ latitude WRITE setLatitude)
    Q_PROPERTY(double longitude READ longitude WRITE setLongitude)
    Q_PROPERTY(double zoom READ zoom WRITE setZoom)
    Q_PROPERTY(double bearing READ bearing WRITE setBearing)
    Q_PROPERTY(double pitch READ pitch WRITE setPitch)

public:
    // Determines the orientation of the map.
    enum NorthOrientation {
        NorthUpwards, // Default
        NorthRightwards,
        NorthDownwards,
        NorthLeftwards,
    };

    QMapboxGL(QObject *parent = 0, const QMapboxGLSettings& = QMapboxGLSettings());
    virtual ~QMapboxGL();

    void cycleDebugOptions();

    QString styleJSON() const;
    QString styleURL() const;

    void setStyleJSON(const QString &);
    void setStyleURL(const QString &);

    double latitude() const;
    void setLatitude(double latitude);

    double longitude() const;
    void setLongitude(double longitude);

    double scale() const;
    void setScale(double scale, const QPointF &center = QPointF());

    double zoom() const;
    void setZoom(double zoom);

    double minimumZoom() const;
    double maximumZoom() const;

    double bearing() const;
    void setBearing(double degrees);
    void setBearing(double degrees, const QPointF &center);

    double pitch() const;
    void setPitch(double pitch);

    NorthOrientation northOrientation() const;
    void setNorthOrientation(NorthOrientation);

    QMapbox::Coordinate coordinate() const;
    void setCoordinate(const QMapbox::Coordinate &);
    void setCoordinateZoom(const QMapbox::Coordinate &, double zoom);

    void setGestureInProgress(bool inProgress);

    void addClass(const QString &);
    void removeClass(const QString &);
    bool hasClass(const QString &) const;
    void setClasses(const QStringList &);
    QStringList getClasses() const;

    QMapbox::AnnotationID addPointAnnotation(const QMapbox::PointAnnotation &);
    QMapbox::AnnotationIDs addPointAnnotations(const QMapbox::PointAnnotations &);

    QMapbox::AnnotationID addShapeAnnotation(const QMapbox::ShapeAnnotation &);
    QMapbox::AnnotationIDs addShapeAnnotations(const QMapbox::ShapeAnnotations &);

    void removeAnnotation(QMapbox::AnnotationID);
    void removeAnnotations(const QMapbox::AnnotationIDs &);

    bool isRotating() const;
    bool isScaling() const;
    bool isPanning() const;
    bool isFullyLoaded() const;

    void moveBy(const QPointF &offset);
    void scaleBy(double scale, const QPointF &center = QPointF());
    void rotateBy(const QPointF &first, const QPointF &second);

    void resize(const QSize &size);

    void addAnnotationIcon(const QString &name, const QImage &sprite);

    QPointF pixelForCoordinate(const QMapbox::Coordinate &) const;
    QMapbox::Coordinate coordinateForPixel(const QPointF &) const;

    void setMargins(const QMargins &margins);
    QMargins margins() const;

public slots:
    void render();
    void connectionEstablished();

signals:
    void needsRendering();
    void mapRegionDidChange();

private:
    Q_DISABLE_COPY(QMapboxGL)

    QMapboxGLPrivate *d_ptr;
};

#endif // QMAPBOXGL_H
