/****************************************************************************
**
** Copyright (c) 2009-2013, Jaco Naudé
**
** This file is part of Qtilities.
**
** For licensing information, please see
** http://jpnaude.github.io/Qtilities/page_licensing.html
**
****************************************************************************/

#ifndef OBSERVERMIMEDATA_H
#define OBSERVERMIMEDATA_H

#include "QtilitiesCore_global.h"
#include "QtilitiesCoreConstants"

#include <QMimeData>
#include <QList>
#include <QPointer>

namespace Qtilities {
    namespace Core {
        /*!
          \class ObserverMimeData
          \brief The ObserverMimeData stores information about subjects when Qtilities does drag and drops between ObserverWidget views.

          ObserverMimeData will return Qtilities::Core::Constants::qti_def_OBSERVER_MIME_DATA_MIME_TYPE as its format in formats().
         */
        class QTILIITES_CORE_SHARED_EXPORT ObserverMimeData : public QMimeData {
            Q_OBJECT

        public:
            ObserverMimeData(QList<QPointer<QObject> > subject_list, int source_id, Qt::DropAction drop_action) : QMimeData() {
                d_source_id = source_id;
                d_subject_list = subject_list;
                d_drop_action = drop_action;
                QByteArray ba;
                setData(Qtilities::Core::Constants::qti_def_OBSERVER_MIME_DATA_MIME_TYPE,ba);
            }
            ObserverMimeData(const ObserverMimeData& other) : QMimeData(),
                d_source_id(other.sourceID()),
                d_subject_list(other.subjectList()),
                d_drop_action(other.dropAction()) {

                QByteArray ba;
                setData(Qtilities::Core::Constants::qti_def_OBSERVER_MIME_DATA_MIME_TYPE,ba);
            }

            //! Gets the ID of the observer which populated the mime data object.
            int sourceID() const { return d_source_id; }
            //! Gets the list of subjects to which the mime data object applies.
            QList<QPointer<QObject> > subjectList() const { return d_subject_list; }
            //! Gets the drop action assocaited with this mime data object when used during a drop action.
            Qt::DropAction dropAction() const { return d_drop_action; }

        private:
            int                         d_source_id;
            QList<QPointer<QObject> >   d_subject_list;
            Qt::DropAction              d_drop_action;
        };
    }
}

#endif // OBSERVERMIMEDATA_H
