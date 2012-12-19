#ifndef ITIVIEWERPIXELINFOWIDGET_H
#define ITIVIEWERPIXELINFOWIDGET_H

#include <QWidget>

class QTextEdit;

namespace itiviewer{

/*!
 * \brief The ItiViewerPixelInfoWidget class shows pixel information related to the position of
 *  the cursor over the visualized image.
 */
class ItiViewerPixelInfoWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(QString text     READ text   WRITE setText   NOTIFY textChanged)

public:
    /*!
     * \brief ItiViewerPixelInfoWidget , ctor
     * \param parent
     */
    explicit ItiViewerPixelInfoWidget(QWidget *parent = 0);

    /*!
     * \brief text , getter
     * \return the text variable
     */
    QString text() const { return m_text; }

    /*!
     * \brief setText , setter
     * \param s , the new text
     */
    void setText(const QString &s);
    
public slots:

    void updateText(const QString &s) { setText(s); }

signals:
    /*!
     * \brief textChanged
     */
    void textChanged();

private:

    QTextEdit *m_pTextEdit;

    /*!
     * \brief setupLayout
     */
    void setupLayout();

    /*!
     * \brief m_text
     */
    QString m_text;

};

}

#endif // ITIVIEWERPIXELINFOWIDGET_H