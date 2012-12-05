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

#ifndef ITIOTBIMAGEVIEWERPANELSETUPDATATAB_H
#define ITIOTBIMAGEVIEWERPANELSETUPDATATAB_H

#include <QWidget>
#include <QGroupBox>
#include <QRadioButton>
#include <QSpinBox>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>

namespace itiviewer{

/*!
 * \brief The ItiOtbImageViewerPanelSetupTab class
 */
class ItiOtbImageViewerPanelSetupTab : public QWidget
{
    Q_OBJECT
public:
    //! ctor
    explicit ItiOtbImageViewerPanelSetupTab(QWidget *parent = 0);


    
signals:
    //! emitted when the apply button for color composition has been clicked
    void colorCompositionApplyButtonClicked();

    //! emitted when the apply button for contrast enhancement has been clicked
    void contrastEnhancementApplyButtonClicked();

    //! emitted when the grey scale channel selection has been altered
    void greyScaleColorCompositionChannelChanged(int);

    //! emitted when the rgb channel selection has been altered
    void rgbColorCompositionChannelsChanged(int red, int green, int blue);

    //! emitted when the contrast enhancement method has been changed
    void contrastEnhancementChanged(int method, double aval, double bval);
    
public slots:
    //!
    void onRadioButtonColorCompositionChanged();

    //!
    void onComboBoxContrastEnhancementMethodCurrentIndexChanged(int);

    //!
    void onSpinBoxRedChannelChanged(int val);

    //!
    void onSpinBoxGreenChannelChanged(int val);

    //!
    void onSpinBoxBlueChannelChanged(int val);

private:
    void initialize();

    //!
    void setupColorCompositionGroupBox();

    //!
    void setupContrastEnhancememtGroupBox();

    //! Classes declarations for color composition group box
    QGroupBox       *m_pGroupBoxColorComposition;
    QRadioButton    *m_pRadioButtonGrayscaleMode;
    QRadioButton    *m_pRadioButtonRGBCompositionMode;
    QSpinBox        *m_pSpinBoxGreyscaleChannel;
    QSpinBox        *m_pSpinBoxRedChannel;
    QSpinBox        *m_pSpinBoxGreenChannel;
    QSpinBox        *m_pSpinBoxBlueChannel;
    QPushButton     *m_pButtonApplyColorComposition;
    QLabel          *m_pLabelGreyChannel;
    QLabel          *m_pLabelRedChannel;
    QLabel          *m_pLabelGreenChannel;
    QLabel          *m_pLabelBlueChannel;

    //! Classes declarations for contrast enhancement group box
    QGroupBox       *m_pGroupBoxContrastEnhancement;
    QGroupBox       *m_pGroupBoxMethod;
    QComboBox       *m_pComboBoxMethod;
    QPushButton     *m_pButtonApplyContrastEnhancement;
    QLabel          *m_pLabelUpperQuantile;
    QLabel          *m_pLabelLowerQuantile;
    QLabel          *m_pLabelStandardDeviation;
    QLineEdit       *m_pLineEditUpperQuantileValue;
    QLineEdit       *m_pLineEditLowerQuantileValue;
    QLineEdit       *m_pLineEditStandardDeviationValue;
};

}

#endif // ITIOTBIMAGEVIEWERPANELSETUPDATATAB_H