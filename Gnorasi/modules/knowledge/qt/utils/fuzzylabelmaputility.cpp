#include <QDebug>
#include <QFileDialog>

#include "fuzzylabelmaputility.h"

#include "otbAttributesMapLabelObjectWithClassLabel.h"

#include "itkShapeLabelObject.h"
#include "itkPolyLineParametricPath.h"
#include "itkVectorContainer.h"

using namespace otb;

FuzzyLabelMapUtility::FuzzyLabelMapUtility(QObject *parent) :
    QObject(parent)
{
}


void FuzzyLabelMapUtility::parse(LabelMapType *lblmap){

    m_attributeList.clear();

    for(unsigned int i = 0; i < lblmap->GetNumberOfLabelObjects(); i++){
        LabelObjectType* lblObject = lblmap->GetLabelObject(i);
	//LabelObjectType* lblObject = lblmap->GetNthLabelObject(i);

        std::vector<std::string> attrList = lblObject->GetAvailableAttributes();
        if(!attrList.empty()){
            for(int j = 0; j < attrList.size(); j ++){
                QString name = QString::fromStdString(attrList.at(j));
                m_attributeList << name;
            }
            break;
        }
        else{

        }
    }
}


void FuzzyLabelMapUtility::calculateValues(LabelMapType *lblMap, const QString &propName){
    for(unsigned int i = 0; i < lblMap->GetNumberOfLabelObjects(); i++){
        LabelObjectType* lblObject = lblMap->GetLabelObject(i);
	//LabelObjectType* lblObject = lblMap->GetNthLabelObject(i);

        double val = (double)lblObject->GetAttribute(propName.toLatin1().constData());

        double calval = calculateValue(val,m_a,m_b);

        int idx = propName.lastIndexOf("::");
        idx += 2;

        QString newName = propName;
        newName = newName.insert(idx,QLatin1String("Fuzzy"));

        lblObject->SetAttribute(newName.toLatin1().constData(),calval);
    }
}



QString FuzzyLabelMapUtility::constructCsvFromLabelMap(LabelMapType *lblMap){
    QString text;

    bool headersSetup = false;

    for(unsigned int i = 0; i < lblMap->GetNumberOfLabelObjects(); i++){
        LabelObjectType* lblObject = lblMap->GetLabelObject(i);
	//LabelObjectType* lblObject = lblMap->GetNthLabelObject(i);

        std::vector<std::string> atList = lblObject->GetAvailableAttributes();

        if(!headersSetup){
            QStringList headers;
            headers << "ID";
            for(int k = 0; k < atList.size(); k++){
                std::string name = atList.at(k);
                headers << QString::fromStdString(name);
                text = headers.join(";");
                text += "\n";
            }

            headersSetup = true;
        }

        QStringList values;
        unsigned long lblId = lblObject->GetLabel();
        values << QString::number(lblId);

        for(int j = 0; j < atList.size(); j++){
            std::string name = atList.at(j);

            double val = (double)lblObject->GetAttribute(name.c_str());

            values << QString::number(val,'f',6);
        }

        text += values.join(";");
        text += "\n";
    }

    return text;
}
