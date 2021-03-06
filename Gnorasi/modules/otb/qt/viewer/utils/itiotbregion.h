#ifndef ITIOTBREGION_H
#define ITIOTBREGION_H

#include <QObject>
#include <QPolygonF>
#include <QColor>

#include <QPainter>

#include "../vector_globaldefs.h"

namespace itiviewer {

/*!
 * \brief The Region class
 */
class Region : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int          segmentationId      READ segmentationId     WRITE setSegmentationId     NOTIFY  segmentationIdChanged)
    Q_PROPERTY(int          classificationId    READ classificationId   WRITE setClassificationId   NOTIFY  classificationIdChanged)
    Q_PROPERTY(QPolygon     area                READ area               WRITE setArea               NOTIFY  areaChanged)
    Q_PROPERTY(QColor       color               READ color              WRITE setColor              NOTIFY  colorChanged)
    Q_PROPERTY(bool         visible             READ visible            WRITE setVisible            NOTIFY  visibleChanged)
public:
    /*!
     * \brief Region
     * \param parent
     */
    explicit    Region(QObject *parent = 0);

    int         segmentationId() const {return m_segmentationId;}
    void        setSegmentationId( int i) { m_segmentationId = i ; }
    
    int         classificationId() const { return m_classificationId; }
    void        setClassificationId(int i) { m_classificationId = i; }

    QPolygon    area() const { return m_area; }
    void        setArea(QPolygon p);

    QColor      color() const { return m_color; }
    void        setColor(QColor c) { m_color = c; m_brush.setColor(m_color); }

    bool        visible() const {return m_visible; }
    void        setVisible(bool v) { m_visible = v; }

    void        drawRegion(QPainter * painter, ImageRegionType &extent, const QRectF&, double iz = 1.0);

    static int randInt(int low, int high)
    {
        // Random number between low and high
        return qrand() % ((high + 1) - low) + low;
    }

signals:
    void        segmentationIdChanged();
    void        classificationIdChanged();
    void        areaChanged();
    void        colorChanged();
    void        visibleChanged();

public slots:

private:
    void        modifyPolygonByExtent(ImageRegionType &, const QRectF &, double);

    int         m_segmentationId;
    int         m_classificationId;
    QPolygon    m_area;
    QColor      m_color;
    QPen        m_pen;
    QBrush      m_brush;
    QPolygon    m_paintedArea;
    bool        m_visible;
    
};

} // using namesoace itiviewer

#endif // ITIOTBREGION_H
