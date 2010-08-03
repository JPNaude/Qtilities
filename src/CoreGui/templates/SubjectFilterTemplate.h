/****************************************************************************
**
** Copyright 2009-2010, Jaco Naude
**
** This library is free software; you can redistribute it and/or
** modify it under the terms of the GNU Lesser General Public
** License as published by the Free Software Foundation; either
** version 2.1 of the License, or (at your option) any later version.
**
** This library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details.
**
** You should have received a copy of the GNU Lesser General Public
** License along with this library;  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#ifndef SUBJECTFILTERTEMPLATE_H
#define SUBJECTFILTERTEMPLATE_H

#include "AbstractSubjectFilter.h"
#include <QValidator>

namespace Qtilities {
    namespace ObjManagement {
        /*!
          \struct SubjectFilterTemplateData
          \brief The SubjectFilterTemplateData class stores private data used by the SubjectFilterTemplate class.
         */
        struct SubjectFilterTemplateData;	
	
        /*!
        \class SubjectFilterTemplate
        \brief A template subject filter.
          */	
        class OBJMANAGEMENT_SHARED_EXPORT SubjectFilterTemplate : public AbstractSubjectFilter
        {
            Q_OBJECT

        public:
            SubjectFilterTemplate(QObject* parent = 0);
            ~SubjectFilterTemplate() {}
            QString filterName() { return tr("Subject Filter Template"); }

            AbstractSubjectFilter::EvaluationResult evaluateAttachment(QObject* obj) const;
            bool initializeAttachment(QObject* obj);
            void finalizeAttachment(QObject* obj, bool attachment_successful);
            AbstractSubjectFilter::EvaluationResult evaluateDetachment(QObject* obj) const;
            bool initializeDetachment(QObject* obj);
            void finalizeDetachment(QObject* obj, bool detachment_successful);

            QStringList reservedProperties();
            bool reservedPropertyChanged(QObject* obj, const char* property_name, QDynamicPropertyChangeEvent* propertyChangeEvent);

        private:
			SubjectFilterTemplateData* d;
        };
    }
}


#endif // SUBJECTFILTERTEMPLATE_H
