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

#ifndef FUZZYOPERATORMANAGER_H
#define FUZZYOPERATORMANAGER_H

#include <QObject>

class FuzzyOperator;

// a helper macro
#define FUZZYOPERATORMANAGER FuzzyOperatorManager::instance()

class FuzzyOperatorManager : public QObject
{
    Q_OBJECT
public:
    
    static FuzzyOperatorManager* instance();
    static void deleteInstance();

    QList<FuzzyOperator*> fuzzyOperatorList() const { return m_fuzzyOperatorList; }
    void setFuzzyOperatorList(const QList<FuzzyOperator*> &l){ m_fuzzyOperatorList = l; }

    void addOperator(FuzzyOperator *f) { m_fuzzyOperatorList.append(f); }
    void removeOperator(FuzzyOperator *f) { m_fuzzyOperatorList.removeOne(f); }

    void clear() { qDeleteAll(m_fuzzyOperatorList); m_fuzzyOperatorList.clear(); }

    FuzzyOperator* fuzzyOperatorByName(const QString& );

    int count() const { return m_fuzzyOperatorList.count(); }


signals:
    
public slots:

private:
    explicit FuzzyOperatorManager(QObject *parent = 0);

    ~FuzzyOperatorManager();

    static FuzzyOperatorManager *m_pInstance;

    QList<FuzzyOperator*> m_fuzzyOperatorList;
    
};

#endif // FUZZYOPERATORMANAGER_H
