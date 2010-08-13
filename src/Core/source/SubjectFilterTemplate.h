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

namespace Qtilities {
    namespace Core {
        /*!
          \struct SubjectFilterTemplateData
          \brief The SubjectFilterTemplateData class stores private data used by the SubjectFilterTemplate class.
         */
        struct SubjectFilterTemplateData;	
	
        /*!
        \class SubjectFilterTemplate
        \brief A template subject filter.

        Creating custom subject filters is an easy task if you understand how the initialization and
        finalization diagrams shown in the \ref subject_filters section of the \ref page_observers article work.
        The SubjectFilterTemplate class should be used as a starting point when creating new subject filters.
        The new filter class needs to inherit from Qtilities::Core::AbstractSubjectFilter and reimplement the virtual
        abstract functions in order to work. The source code of the subject filters which comes as part of
        %Qtilities is a good place to start when looking for examples of filter implementations.
          */	
        class QTILIITES_CORE_SHARED_EXPORT SubjectFilterTemplate : public AbstractSubjectFilter
        {
            Q_OBJECT

        public:
            SubjectFilterTemplate(QObject* parent = 0);
            ~SubjectFilterTemplate() {}
            QString filterName() { return tr("Subject Filter Template"); }

            // --------------------------------
            // AbstractSubjectFilter Implemenation
            // --------------------------------
            AbstractSubjectFilter::EvaluationResult evaluateAttachment(QObject* obj) const;
            bool initializeAttachment(QObject* obj, bool import_cycle);
            void finalizeAttachment(QObject* obj, bool attachment_successful, bool import_cycle);
            AbstractSubjectFilter::EvaluationResult evaluateDetachment(QObject* obj) const;
            bool initializeDetachment(QObject* obj, bool subject_deleted = false);
            void finalizeDetachment(QObject* obj, bool detachment_successful, bool subject_deleted = false);

            QStringList monitoredProperties();
            bool monitoredPropertyChanged(QObject* obj, const char* property_name, QDynamicPropertyChangeEvent* propertyChangeEvent);

        private:
            SubjectFilterTemplateData* d;
        };
    }
}


#endif // SUBJECTFILTERTEMPLATE_H
