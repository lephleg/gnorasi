/********************************************************************************
 *                                                                              *
 * GNORASI - The Knowlegde-Based Remote Sensing Engine                          *
 *                                                                              *
 * Language:  C++                                                               *
 *                                                                              *
 * Copyright (c) Ioannis Tsampoulatidis <itsam@iti.gr>. All rights reserved. 	*
 * Copyright (c) Informatics and Telematics Institute                           *
 *	  Centre for Research and Technology Hellas. All rights reserved.           *
 * Copyright (c) National Technical University of Athens. All rights reserved.	*
 *                                                                              *
 *                                                                              *
 * This file is part of the GNORASI software package. GNORASI is free           *
 * software: you can redistribute it and/or modify it under the terms           *
 * of the GNU General Public License version 2 as published by the              *
 * Free Software Foundation.                                                    *
 *                                                                              *
 * GNORASI is distributed in the hope that it will be useful,                   *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of               *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the                 *
 * GNU General Public License for more details.                                 *
 *                                                                              *
 * You should have received a copy of the GNU General Public License            *
 * in the file "LICENSE.txt" along with this program.                           *
 * If not, see <http://www.gnu.org/licenses/>.                                  *
 *                                                                              *
 ********************************************************************************/

#include <QtCore>
#include <QtGui>
#include <QtOpenGL>

#include "itiotbvectorimageviewer.h"

#include "itiotbvectorqglwidgetscrollable.h"
#include "../../utils/itiotbimagemanager.h"
#include "../../observables/itiviewerobservableregion.h"
#include "../../../../ports/otbvectorimageport.h"

#include "itkImageRegionConstIteratorWithIndex.h"
#include "otbVectorRescaleIntensityImageFilter.h"


#include "../../utils/itiotbImageModelRendererScrollable.h"
#include "../../utils/itiotbImageViewManipulatorScrollable.h"
#include "../../utils/itiotblevelutility.h"
#include "../../utils/itiotblevel.h"
#include "../../utils/itiotbregion.h"

#include "../../models/itiotbVectorImageModel.h"
//
// Monteverdi includes (sorted by alphabetic order)
#include "../../models/itiotbAbstractImageModel.h"


using namespace otb;
using namespace itiviewer;
using namespace voreen;

ItiOtbVectorQGLWidgetScrollable::ItiOtbVectorQGLWidgetScrollable(ItiOtbVectorImageViewer *parent) :
    m_IsotropicZoom(1.0),
    m_SubsamplingRate(1),
    m_pImageViewManipulator( NULL ),
    m_pImageModelRenderer( NULL ),
    m_pItiOtbVectorImageViewer(parent),
    m_currentLevelId(1),
    m_helperZoomCounter(1.0),
    m_moving(false),
    QGLWidget(parent)
{
    setAutoFillBackground(false);

    m_pImageViewManipulator = new ImageViewManipulatorScrollable( this );
    m_pImageModelRenderer   = new ImageModelRendererScrollable( this );

    m_pen = QPen(QBrush(Qt::red),2.0);

    QTime time = QTime::currentTime();
    qsrand((uint)time.msec());
}

//
void ItiOtbVectorQGLWidgetScrollable::setupFocusRegionAndSendNotification() {

    //!
    //! setup the number of rows and columns to be visualized
    unsigned int f_d_c = m_pImageModelRenderer->firstDisplayColumn();
    unsigned int f_d_r = m_pImageModelRenderer->firstDisplayRow();
    ImageRegionType extent = m_pImageViewManipulator->extent();

    int _x = f_d_c + m_focusRegion.x();
    if(extent.GetIndex()[0] > 0)
        _x -= extent.GetIndex()[0];

    int _y = f_d_r + height() - m_focusRegion.y() - m_focusRegion.height();
    if(extent.GetIndex()[1] > 0)
        _y -= extent.GetIndex()[1];

    int _w = m_focusRegion.width();
    int _h = m_focusRegion.height();

    if(_x < 0) _x = 0; else if(_x + _w > extent.GetSize()[0]) _x = extent.GetSize()[0] - _w;
    if(_y < 0) _y = 0; else if(_y + _h > extent.GetSize()[1]) _y  = extent.GetSize()[1] - _h;

    QRect rect(_x,_y,_w,_h);

    //! emit signal in order to update the zoomable view
    emit focusRegionChanged(rect);
}


/*!
 * \brief ItiOtbVectorQGLWidgetScrollable::initializeGL
 */
void ItiOtbVectorQGLWidgetScrollable::initializeGL()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
}

void ItiOtbVectorQGLWidgetScrollable::setupPaintingParametersAndVisibleArea(){
    ImageRegionType extent = m_pImageViewManipulator->extent();
    ImageRegionType bufferRegion = m_pImageViewManipulator->bufferRegion();

    //!
    //! setup the number of rows and columns to be visualized
    unsigned int nb_d_cs = m_pImageModelRenderer->nbDisplayColumns();
    unsigned int nb_d_rs = m_pImageModelRenderer->nbDisplayRows();
    unsigned int f_d_c = m_pImageModelRenderer->firstDisplayColumn();
    unsigned int f_d_r = m_pImageModelRenderer->firstDisplayRow();

    //!
    //! check if the extend's index x value is greater than zero , if yes then the number of columns equals to the number of the buffered region
    if( extent.GetIndex()[0] > 0 ){
        nb_d_cs = bufferRegion.GetSize()[0];
        f_d_c = 0;
    } else {

        nb_d_cs = width() / m_IsotropicZoom;

        //!
        //! setup the fisrt display column
        //!
        if(f_d_c + nb_d_cs > bufferRegion.GetSize()[0])
            f_d_c = bufferRegion.GetSize()[0] - nb_d_cs;
    }

    //!
    //! check if the extend's index y value is greater than zero , if yes then the number of columns equals to the number of the buffered region
    if( extent.GetIndex()[1] > 0 ){
        nb_d_rs = bufferRegion.GetSize()[1];
        f_d_r = extent.GetSize()[1] - nb_d_rs;
    } else {
        nb_d_rs = height() / m_IsotropicZoom;

        //!
        //! setup first display row
        //!
        if(f_d_r + nb_d_rs > bufferRegion.GetSize()[1])
            f_d_r = bufferRegion.GetSize()[0] - nb_d_rs;
    }

    m_pImageModelRenderer->setPaintingParameters(nb_d_cs,nb_d_rs,f_d_c,f_d_r);

    //! create a rect
    QRect rect(f_d_c,bufferRegion.GetSize()[1] - nb_d_rs - f_d_r,nb_d_cs,nb_d_rs);

    //! emit the signal
    emit visibleAreaChanged(rect);
}

void ItiOtbVectorQGLWidgetScrollable::resizeGL(int w, int h)
{
    //! firstly setup the viweport with the new width and height
    setupViewport(w,h);

    setupPaintingParametersAndVisibleArea();

}

//!
void ItiOtbVectorQGLWidgetScrollable::setupViewport(int w, int h){
    ImageRegionType extent;
    ImageRegionType bufferRegion = m_pImageViewManipulator->bufferRegion();
    ImageRegionType::SizeType size;

    size [0] = static_cast<unsigned int>(m_IsotropicZoom * static_cast<double>(bufferRegion.GetSize()[0]));
    size [1] = static_cast<unsigned int>(m_IsotropicZoom * static_cast<double>(bufferRegion.GetSize()[1]));

    ImageRegionType::IndexType index;
    index[0] = (w - static_cast<int>(size[0])) / 2;
    index[1] = (h - static_cast<int>(size[1])) / 2;

    extent.SetIndex(index);
    extent.SetSize(size);

    m_pImageViewManipulator->setExtent(extent);

    // initialize with the given size
    if(w && h)
        m_pImageViewManipulator->InitializeContext(w,h);
    else
        m_pImageViewManipulator->InitializeContext(100,100);

    glViewport(0, 0, (GLint)w, (GLint)h);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, (GLint)w, 0, (GLint)h, -1, 1);
}

//!
void ItiOtbVectorQGLWidgetScrollable::initializeColumnRowParameters(){

    setupViewport(width(),height());

    ImageRegionType extent = m_pImageViewManipulator->extent();
    ImageRegionType bufferedRegion = m_pImageViewManipulator->bufferRegion();

    unsigned int nb_d_cs    = 0;
    unsigned int nb_d_rs    = 0;
    unsigned int f_d_c      = 0;
    unsigned int f_d_r      = 0;

    if( extent.GetIndex()[0] > 0 )
        nb_d_cs = bufferedRegion.GetSize()[0];
    else
        nb_d_cs = width() / m_IsotropicZoom;

    if( extent.GetIndex()[1] > 0 )
        nb_d_rs = bufferedRegion.GetSize()[1];
    else
        nb_d_rs = height() / m_IsotropicZoom;

    //!
    //! the values along the y-axes follow some kind of a transormation matrix [1,-1]
    //!
    f_d_r = extent.GetSize()[1] - nb_d_rs;

    //!
    m_pImageModelRenderer->setPaintingParameters(nb_d_cs,nb_d_rs,f_d_c,f_d_r);
}

//!
void ItiOtbVectorQGLWidgetScrollable::paintEvent(QPaintEvent *event){
    Q_UNUSED(event);

    //!
    makeCurrent();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    // Set the new rendering context to be known in the ModelRendere
    const AbstractImageModel* aiModel=  qobject_cast<AbstractImageModel*>(m_pItiOtbVectorImageViewer->model());

    // setup the rendering context
    if (aiModel)
    {
        setupViewport(width(), height());

        // Get the region to draw from the ImageViewManipulator navigation
        // context
        const ImageRegionType region(
          m_pImageViewManipulator->bufferRegion());

        ImageRegionType extent(m_pImageViewManipulator->extent());

        ImageModelRendererScrollable::RenderingContext context(aiModel, region, extent, this->width(), this->height(), m_IsotropicZoom);

        // use the model renderer to paint the requested region of the image
        m_pImageModelRenderer->paintGL( context );
    }

    //! overpainting
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QRectF rect = constructHelperRect();

    //!
    Level *pLevel = m_pItiOtbVectorImageViewer->manager()->levelById(m_currentLevelId);
    if(pLevel){

        ImageRegionType extent = m_pImageViewManipulator->extent();

        QList<Region*> regions = pLevel->regions();
        QList<Region*>::const_iterator i;
        for(i = regions.constBegin(); i != regions.constEnd(); i++){
            Region *pRegion = *i;
            if(pRegion->visible())
                pRegion->drawRegion(&painter, extent, rect,  m_IsotropicZoom);
        }
    }

    //! draw focus region
    m_pen.setWidth(2.0);
    m_pen.setColor(Qt::red);
    painter.setPen(m_pen);
    painter.setBrush(QBrush(Qt::transparent));
    painter.setOpacity(1.0);
    painter.drawRect(m_focusRegion);

    painter.end();
}

//!
void ItiOtbVectorQGLWidgetScrollable::draw(){

    // Set the new rendering context to be known in the ModelRendere
    const VectorImageModel* vModel=  qobject_cast<VectorImageModel*>(m_pItiOtbVectorImageViewer->model());

    if(!vModel)
        return;

    m_pImageViewManipulator->InitializeContext(this->width(),this->height());

    const VectorRegionType region = vModel->GetLargestPossibleRegion();

    m_pImageViewManipulator->SetImageLargestRegion(region);

    //! initialize the column and row related parameters
    initializeColumnRowParameters();
}

//! The observer gets notified on a zoomable view's change ( either zooming or resizing events)
void ItiOtbVectorQGLWidgetScrollable::updateObserver(ItiViewerObservable *observable){    

    ItiViewerObservableRegion *region = qobject_cast<ItiViewerObservableRegion*>(observable);
    if(!region)
        return;

    ImageRegionType extent(m_pImageViewManipulator->extent());

    //! get the current rect from the region
    QRect rregion = region->region();

    // update the zoom value
    m_helperZoomCounter = region->zoom();

    //! create helper values from the rect's parameters
    int w       = rregion.width();
    int h       = rregion.height();
    int x       = rregion.x() - m_pImageModelRenderer->firstDisplayColumn();
    int y       = m_pImageModelRenderer->firstDisplayRow() + height() - rregion.y() - h;

    if(extent.GetIndex()[0] > 0){
        x += extent.GetIndex()[0];
        if(x < extent.GetIndex()[0])
            x = extent.GetIndex()[0];
    }

    if(extent.GetIndex()[1] > 0){
        y -= extent.GetIndex()[1];
        if(y < extent.GetIndex()[1])
            y = extent.GetIndex()[1];
    }

    if(w > extent.GetSize()[0]){
        w = extent.GetSize()[0];
        x = extent.GetIndex()[0];
    }

    if(h > extent.GetSize()[1]){
        h = extent.GetSize()[1];
        y = extent.GetIndex()[1];
    }

    m_focusRegion.setX(x);
    m_focusRegion.setY(y);
    m_focusRegion.setWidth(w);
    m_focusRegion.setHeight(h);

    //! finally update the view
    update();
}

//!
void ItiOtbVectorQGLWidgetScrollable::wheelEvent(QWheelEvent *event){
    double deltaval = (double)event->delta();

    //! check the delta value if is more than zero, if yes zoomin, else zoomout
    if(deltaval>0)
        zoomIn();
    else
        zoomOut();

    event->accept();
}

//!
void ItiOtbVectorQGLWidgetScrollable::mousePressEvent(QMouseEvent *event){

    //! setup translating functionality only on left button pressed mouse events
    if(event->button() == Qt::LeftButton){

        ImageRegionType bregion = m_pImageViewManipulator->bufferRegion();

        if(!bregion.GetSize()[0] || !bregion.GetSize()[1]){
            QGLWidget::mousePressEvent(event);
            return;
        }

        QCursor dragCursor;
        dragCursor.setShape(Qt::ClosedHandCursor) ;
        this->setCursor(dragCursor);

        m_moving = true;

        //! create a helper point value
        QPoint previousCenter = m_focusRegion.center();

        //! get the position of the event
        QPoint point = event->pos();

        //! create a helper line object
        QLine line(previousCenter,point);

        ImageRegionType extent = m_pImageViewManipulator->extent();

        //! create helper values
        int dx = 0, dy = 0;


        if(line.dx() > 0 && extent.GetIndex()[0] < 0 && m_pImageModelRenderer->firstDisplayColumn() + m_focusRegion.x() + line.dx() + m_focusRegion.width() > extent.GetSize()[0] ){
            dx = extent.GetSize()[0] - m_pImageModelRenderer->firstDisplayColumn() - m_focusRegion.width() - m_focusRegion.x();
            if(extent.GetIndex()[0] > 0)
                dx += extent.GetIndex()[0];
        }
        else if(line.dx() > 0 && extent.GetIndex()[0] >= 0 && m_focusRegion.x() + line.dx() + m_focusRegion.width() - extent.GetIndex()[0] > extent.GetSize()[0] ){
            dx = extent.GetSize()[0] - m_focusRegion.width() - m_focusRegion.x() + extent.GetIndex()[0];
        }
        else if( line.dx() < 0 && (int)m_pImageModelRenderer->firstDisplayColumn() + m_focusRegion.x() + line.dx() < 0){
            dx = -m_focusRegion.x();
            if(extent.GetIndex()[0] > 0)
                dx += extent.GetIndex()[0];
        }
        else if( line.dx() < 0 && extent.GetIndex()[0] > 0 && (int)m_pImageModelRenderer->firstDisplayColumn() + m_focusRegion.x() + line.dx() < extent.GetIndex()[0]){
            dx = -m_focusRegion.x() + extent.GetIndex()[0];
        }else
            dx = line.dx();

        //

        if(line.dy() > 0 && extent.GetIndex()[1] < 0 && extent.GetSize()[1] - m_pImageModelRenderer->nbDisplayRows() - m_pImageModelRenderer->firstDisplayRow() + m_focusRegion.y() + m_focusRegion.height() + line.dy() > extent.GetSize()[1]){
            dy = height() - m_focusRegion.y() - m_focusRegion.height();
            if(extent.GetIndex()[1] > 0)
                dy -= extent.GetIndex()[1];
        }
        else if(line.dy() > 0 && extent.GetIndex()[1] >= 0 && extent.GetSize()[1] -extent.GetIndex()[1] - m_pImageModelRenderer->nbDisplayRows() + m_focusRegion.y() + m_focusRegion.height() + line.dy() > extent.GetSize()[1] ){
            dy = height() - m_focusRegion.y() - m_focusRegion.height() - extent.GetIndex()[1];
        }
        else if( line.dy() < 0 && extent.GetIndex()[1] < 0 && m_pImageModelRenderer->firstDisplayRow() + height() - m_focusRegion.y() - line.dy() > extent.GetSize()[1]){
            dy = -m_focusRegion.y();
            if(extent.GetIndex()[1] > 0)
                dy -= extent.GetIndex()[1];
        }
        else if( line.dy() < 0 && extent.GetIndex()[1] >= 0 && m_focusRegion.y() + line.dy() < extent.GetIndex()[1]){
            dy = -m_focusRegion.y() + extent.GetIndex()[1];
        }
        else
            dy = line.dy();


        //!translate the focus region
        m_focusRegion.translate(dx,dy);

        //
        setupFocusRegionAndSendNotification();

        //! update widget
        update();
    }

    QGLWidget::mousePressEvent(event);
}

//!
void ItiOtbVectorQGLWidgetScrollable::mouseMoveEvent(QMouseEvent *event){
    //! setup translating functionality only on left button pressed mouse events
    if(m_moving){

        //! create a helper point value
        QPoint previousCenter = m_focusRegion.center();

        //! get the position of the event
        QPoint point = event->pos();

        //! create a helper line object
        QLine line(previousCenter,point);

        ImageRegionType extent = m_pImageViewManipulator->extent();

        //! create helper values
        int dx = 0, dy = 0;


        if(line.dx() > 0 && extent.GetIndex()[0] < 0 && m_pImageModelRenderer->firstDisplayColumn() + m_focusRegion.x() + line.dx() + m_focusRegion.width() > extent.GetSize()[0] ){
            dx = extent.GetSize()[0] - m_pImageModelRenderer->firstDisplayColumn() - m_focusRegion.width() - m_focusRegion.x();
            if(extent.GetIndex()[0] > 0)
                dx += extent.GetIndex()[0];
        }
        else if(line.dx() > 0 && extent.GetIndex()[0] >= 0 && m_focusRegion.x() + line.dx() + m_focusRegion.width() - extent.GetIndex()[0] > extent.GetSize()[0] ){
            dx = extent.GetSize()[0] - m_focusRegion.width() - m_focusRegion.x() + extent.GetIndex()[0];
        }
        else if( line.dx() < 0 && (int)m_pImageModelRenderer->firstDisplayColumn() + m_focusRegion.x() + line.dx() < 0){
            dx = -m_focusRegion.x();
            if(extent.GetIndex()[0] > 0)
                dx += extent.GetIndex()[0];
        }
        else if( line.dx() < 0 && extent.GetIndex()[0] > 0 && (int)m_pImageModelRenderer->firstDisplayColumn() + m_focusRegion.x() + line.dx() < extent.GetIndex()[0]){
            dx = -m_focusRegion.x() + extent.GetIndex()[0];
        }else
            dx = line.dx();

        //

        if(line.dy() > 0 && extent.GetIndex()[1] < 0 && extent.GetSize()[1] - m_pImageModelRenderer->nbDisplayRows() - m_pImageModelRenderer->firstDisplayRow() + m_focusRegion.y() + m_focusRegion.height() + line.dy() > extent.GetSize()[1]){
            dy = height() - m_focusRegion.y() - m_focusRegion.height();
            if(extent.GetIndex()[1] > 0)
                dy -= extent.GetIndex()[1];
        }
        else if(line.dy() > 0 && extent.GetIndex()[1] >= 0 && extent.GetSize()[1] -extent.GetIndex()[1] - m_pImageModelRenderer->nbDisplayRows() + m_focusRegion.y() + m_focusRegion.height() + line.dy() > extent.GetSize()[1] ){
            dy = height() - m_focusRegion.y() - m_focusRegion.height() - extent.GetIndex()[1];
        }
        else if( line.dy() < 0 && extent.GetIndex()[1] < 0 && m_pImageModelRenderer->firstDisplayRow() + height() - m_focusRegion.y() - line.dy() > extent.GetSize()[1]){
            dy = -m_focusRegion.y();
            if(extent.GetIndex()[1] > 0)
                dy -= extent.GetIndex()[1];
        }
        else if( line.dy() < 0 && extent.GetIndex()[1] >= 0 && m_focusRegion.y() + line.dy() < extent.GetIndex()[1]){
            dy = -m_focusRegion.y() + extent.GetIndex()[1];
        }
        else
            dy = line.dy();


        //!translate the focus region
        m_focusRegion.translate(dx,dy);

        //
        setupFocusRegionAndSendNotification();

        //! update widget
        update();
    }

    // pixel info related functionality follows

    if(hasMouseTracking()){

        VectorImageModel *vModel = qobject_cast<VectorImageModel*>(m_pItiOtbVectorImageViewer->model());
        if(vModel){

            VectorImageType *img = m_pItiOtbVectorImageViewer->manager()->image();
            if(img){

                QString text;

                ImageRegionType extent = m_pImageViewManipulator->extent();

                // check if both x y extent values are negative then this means that the
                // mouse position is definetely inside the image boundaries
                if(extent.GetIndex()[0] < 0 && extent.GetIndex()[1] < 0){
                    ImageRegionType::IndexType idx  = indexFromPoint(event->pos());

                    VectorImageType::PixelType pixelValue = img->GetPixel(idx);

                    RenderingFilterType* filter = vModel->filter();

                    const std::string pixeldata = filter->GetRenderingFunction()->Describe(pixelValue);

                    QString pdt = QString::fromStdString(pixeldata);

        //            qDebug() << "idx[0] : " << idx[0] << "\t" << "idx[1] : " << idx[1];

                    text = m_pItiOtbVectorImageViewer->manager()->constructInfoByIndex(idx,pdt);
                }else{
                    QPoint point(event->pos().x()- extent.GetIndex()[0],event->pos().y()- extent.GetIndex()[1]);

                    ImageRegionType::IndexType idx;
                    idx[0] = point.x();
                    idx[1] = point.y();

                    // check whether the point is inside the image boundaries
                    if(!ItiOtbImageManager::isInsideTheImage(extent,point))
                        text = m_pItiOtbVectorImageViewer->manager()->constructInfoByIndexAlt(idx);
                    else{
                        VectorImageType::PixelType pixelValue = img->GetPixel(idx);

                        RenderingFilterType* filter = vModel->filter();

                        const std::string pixeldata = filter->GetRenderingFunction()->Describe(pixelValue);

                        QString pdt = QString::fromStdString(pixeldata);

                        text = m_pItiOtbVectorImageViewer->manager()->constructInfoByIndex(idx,pdt);
                    }
                }

                // emit the signal containing the text of the pixel info
                emit currentIndexChanged(text);
            }
        }
    }

    // now call the parent widget class mousemoveevent
    QGLWidget::mouseMoveEvent(event);
}


void ItiOtbVectorQGLWidgetScrollable::mouseReleaseEvent(QMouseEvent *event){
    m_moving = false;

    QCursor dragCursor;
    dragCursor.setShape(Qt::ArrowCursor) ;
    this->setCursor(dragCursor);

    QGLWidget::mouseReleaseEvent(event);
}


/*!
 * \brief ItiOtbVectorQGLWidgetScrollable::translate
 * \param rect
 */
void ItiOtbVectorQGLWidgetScrollable::translate(int dx, int dy){

    unsigned int nb_d_cs = m_pImageModelRenderer->nbDisplayColumns();
    unsigned int nb_d_rs = m_pImageModelRenderer->nbDisplayRows();
    unsigned int f_d_c = m_pImageModelRenderer->firstDisplayColumn();
    unsigned int f_d_r = m_pImageModelRenderer->firstDisplayRow();

    ImageRegionType bufferedRegion  = m_pImageViewManipulator->bufferRegion();

    //! create a helper value equal to the f_d_c plus the dx parameter
    int helperX = f_d_c + dx;

    //! check that the helperX value is less than zero
    if(helperX < 0)
        f_d_c = 0;
    //! check if it is beween the buffered region extends
    else if(helperX > bufferedRegion.GetSize()[0] - nb_d_cs)
        f_d_c = bufferedRegion.GetSize()[0] - nb_d_cs;
    else
        f_d_c = helperX;

    //!
    int helperY = f_d_r - dy;

    //! check that it is less than zero
    if(helperY < 0)
        f_d_r = 0;
    //! check that it is between the buffered region's y-axes size
    else if(helperY > bufferedRegion.GetSize()[1] - nb_d_rs)
        f_d_r = bufferedRegion.GetSize()[1] - nb_d_rs;
    else //! else set the f_d_r equal to the helperY value
        f_d_r = helperY;

    //!
    m_pImageModelRenderer->setPaintingParameters(nb_d_cs,nb_d_rs,f_d_c,f_d_r);

    //! update painting
    update();

    setupFocusRegionAndSendNotification();
}


void ItiOtbVectorQGLWidgetScrollable::zoomIn(){

    m_helperZoomCounter += ZOOM_VALUE;

    int prevWidth = m_focusRegion.width();
    int prevHeight = m_focusRegion.height();

    double newWidth = (double)prevWidth - (double)prevWidth * ZOOM_VALUE ;
    if(newWidth > m_pImageModelRenderer->nbDisplayColumns())
        return;

    double newHeight = prevHeight  - prevHeight * ZOOM_VALUE;
    if(newHeight > m_pImageModelRenderer->nbDisplayRows())
        return;


    m_focusRegion.setWidth(qRound(newWidth));
    m_focusRegion.setHeight(qRound(newHeight));

    m_focusRegion.translate(qRound((prevWidth - (double)m_focusRegion.width())/2.0),qRound((prevHeight - (double)m_focusRegion.height())/2.0));

    update();

    setupFocusRegionAndSendNotification();
}


void ItiOtbVectorQGLWidgetScrollable::zoomOut(){
    if(m_helperZoomCounter -ZOOM_VALUE < 1.0){
        return;
    }

    m_helperZoomCounter -= ZOOM_VALUE;

    int prevWidth = m_focusRegion.width();
    int prevHeight = m_focusRegion.height();

    double newWidth = (double)prevWidth + (double)prevWidth * ZOOM_VALUE ;
    if(newWidth > m_pImageModelRenderer->nbDisplayColumns())
        return;

    double newHeight = prevHeight + prevHeight * ZOOM_VALUE;
    if(newHeight > m_pImageModelRenderer->nbDisplayRows())
        return;

    m_focusRegion.setWidth(qRound(newWidth));
    m_focusRegion.setHeight(qRound(newHeight));

    m_focusRegion.translate(qRound((prevWidth - (double)m_focusRegion.width())/2.0),qRound((prevHeight - (double)m_focusRegion.height())/2.0));

    ImageRegionType extent = m_pImageViewManipulator->extent();


    // applying margin checks
    if(m_focusRegion.x() < extent.GetIndex()[0])
        m_focusRegion.translate(extent.GetIndex()[0]-m_focusRegion.x(),0);
    else if(m_focusRegion.x() + m_focusRegion.width() > extent.GetSize()[0] + extent.GetIndex()[0])
        m_focusRegion.translate(extent.GetSize()[0]+extent.GetIndex()[0]-m_focusRegion.x() - m_focusRegion.width(),0);

    if(m_focusRegion.y() < extent.GetIndex()[1])
        m_focusRegion.translate(0,extent.GetIndex()[1]-m_focusRegion.y());
    else if( m_focusRegion.y() + m_focusRegion.height() > extent.GetSize()[1]+ extent.GetIndex()[1])
        m_focusRegion.translate(0,extent.GetSize()[1]+extent.GetIndex()[1]-m_focusRegion.y()-m_focusRegion.height());

    update();

    setupFocusRegionAndSendNotification();
}

QRectF ItiOtbVectorQGLWidgetScrollable::constructHelperRect() const {
    ImageRegionType extent = m_pImageViewManipulator->extent();

    QRectF rect;
    rect.setX(m_pImageModelRenderer->firstDisplayColumn());
    if(extent.GetIndex()[1] > 0)
        rect.setY(0);
    else
        rect.setY(extent.GetSize()[1] - m_pImageModelRenderer->firstDisplayRow()-m_pImageModelRenderer->nbDisplayRows());

    rect.setWidth(m_pImageModelRenderer->nbDisplayColumns());
    rect.setHeight(m_pImageModelRenderer->nbDisplayRows());

    return rect;

}


ImageRegionType::IndexType ItiOtbVectorQGLWidgetScrollable::indexFromPoint(const QPoint &p){
    ImageRegionType::IndexType idx;

    ImageRegionType region = m_pImageViewManipulator->bufferRegion();

    unsigned int f_d_c      = m_pImageModelRenderer->firstDisplayColumn();
    unsigned int f_d_r      = m_pImageModelRenderer->firstDisplayRow();
    unsigned int nb_d_rs    = m_pImageModelRenderer->nbDisplayRows();

    idx[0] = f_d_c + p.x();
    idx[1] = region.GetSize()[1] - nb_d_rs - f_d_r + p.y();

    if(idx[0] < 0)
        idx[0] = 0;

    if(idx[0] > region.GetSize()[0]-1)
        idx[0]  = region.GetSize()[0]-1;

    if(idx[1] < 0)
        idx[1] = 0;

    if(idx[1] > region.GetSize()[1]-1)
        idx[1] = region.GetSize()[1]-1;


    return idx;
}


void ItiOtbVectorQGLWidgetScrollable::onModelReady(){
    setupViewport(this->width(),this->height());

    setupPaintingParametersAndVisibleArea();
}


//!
ItiOtbVectorQGLWidgetScrollable::~ItiOtbVectorQGLWidgetScrollable(){
//    ClearBuffer();
}
