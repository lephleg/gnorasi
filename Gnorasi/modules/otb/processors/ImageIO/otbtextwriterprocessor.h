/********************************************************************************
 *                                                                    		*
 * GNORASI - The Knowlegde-Based Remote Sensing Engine                		*
 * 								      		*
 * Language:  C++						      		*
 * 										*
 * Copyright (c) Angelos Tzotsos <tzotsos@gmail.com>. All rights reserved. 	*
 * Copyright (c) National Technical University of Athens. All rights reserved.	*
 * Copyright (c) Informatics and Telematics Institute				*
 *	  Centre for Research and Technology Hellas. All rights reserved.	*
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

#ifndef VRN_OTBTEXTWRITERPROCESSOR_H
#define VRN_OTBTEXTWRITERPROCESSOR_H

#include "voreen/core/processors/processor.h"
#include "voreen/core/properties/filedialogproperty.h"
#include "voreen/core/properties/buttonproperty.h"
#include "voreen/core/properties/stringproperty.h"
#include "voreen/core/ports/textport.h"
#include <fstream>
#include <sstream>

namespace voreen {

class OTBTextWriterProcessor : public Processor {
public:
    OTBTextWriterProcessor();
    virtual Processor* create() const;

    virtual std::string getClassName() const { return "Text File Writer"; }
    virtual std::string getCategory() const  { return "Image IO"; }
    virtual CodeState getCodeState() const   { return CODE_STATE_TESTING; } //STABLE, TESTING, EXPERIMENTAL
    virtual std::string getProcessorInfo() const;
    
    virtual bool isReady() const;
    
    void saveCSV();
    void clearCSV();

protected:
    virtual void setDescriptions() {
	setDescription("processor.");
    }
    virtual void process();
    virtual void initialize() throw (VoreenException);
    virtual void deinitialize() throw (VoreenException);

private:
   
    bool hasFileName;
  
    TextPort inPort_;
    
    std::string inData;
    
    FileDialogProperty CSVFile_;  ///< Path of the saved image file.
    ButtonProperty clearFile_;      ///< Executes clearImage().
    ButtonProperty saveButton_;    ///< saves the image.
    
    static const std::string loggerCat_; ///< category used in logging
};

} // namespace

#endif // VRN_OTBCSVWRITERPROCESSOR_H
