/********************************************************************************
 *                                                                    		*
 * GNORASI - The Knowlegde-Based Remote Sensing Engine                		*
 * 								      		*
 * Language:  C++						      		*
 * 										*
 * Copyright (c) Ioannis Tsampoulatidis <itsam@iti.gr>. All rights reserved. 	*
 * Copyright (c) Informatics and Telematics Institute				*
 *	  Centre for Research and Technology Hellas. All rights reserved.	*
 * Copyright (c) National Technical University of Athens. All rights reserved.	*
 * 										*
 *                                                                    		*
 * This file is part of the GNORASI software package. GNORASI is free  		*
 * software: you can redistribute it and/or modify it under the terms 		*
 * of the GNU General Public License version 2 as published by the    		*
 * Free Software Foundation.                                          		*
 *                                                                    		*
 * GNORASI is distributed in the hope that it will be useful,          		*
 * but WITHOUT ANY WARRANTY; without even the implied warranty of     		*
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the       		*
 * GNU General Public License for more details.                       		*
 *                                                                    		*
 * You should have received a copy of the GNU General Public License  		*
 * in the file "LICENSE.txt" along with this program.                 		*
 * If not, see <http://www.gnu.org/licenses/>.                        		*
 *                                                                    		*
 ********************************************************************************/

#include "knowledgeprocessorwidgetfactory.h"

#include "../../processors/dummysegmentationprocessor.h"
#include "dummysegmentationwidget.h"

#include "../../processors/classifierwsprocessor.h"
#include "classifierwswidget.h"

#include "voreen/qt/voreenapplicationqt.h"
#include <QWidget>
#include <QMainWindow>

namespace voreen {

ProcessorWidget* KnowledgeProcessorWidgetFactory::createWidget(Processor* processor) const {

    if (!VoreenApplicationQt::qtApp()) {
        LERRORC("voreen.KnowledgeProcessorWidgetFactory", "VoreenApplicationQt not instantiated");
        return 0;
    }
    QWidget* parent = VoreenApplicationQt::qtApp()->getMainWindow();

    if (dynamic_cast<DummySegmentationProcessor*>(processor))
        return new DummySegmentationWidget(parent, static_cast<DummySegmentationProcessor*>(processor));

    if (dynamic_cast<ClassifierWSProcessor*>(processor))
        return new ClassifierWSWidget(parent, static_cast<ClassifierWSProcessor*>(processor));

    return 0;
}
} // namespace voreen