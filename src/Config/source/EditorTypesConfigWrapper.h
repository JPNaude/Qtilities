/****************************************************************************
**
** Copyright 2010, Jaco Naude
**
****************************************************************************/

#ifndef EDITORTYPESCONFIGWRAPPER_H
#define EDITORTYPESCONFIGWRAPPER_H

#include "IConfigPage.h"
#include <QObject>
#include <QStringList>
#include <QIcon>
#include <QWidget>

namespace VisualWorkspace {
    namespace Core {
        /*!
          \struct EditorTypesConfigWrapperData
          \brief The EditorTypesConfigWrapperData class stores private data used by the EditorTypesConfigWrapper class.
         */
        struct EditorTypesConfigWrapperData;

        /*!
        \class EditorTypesConfigWrapper
        \brief The config page for the session log.
          */
        class EditorTypesConfigWrapper : public QObject, public IConfigPage
        {
            Q_OBJECT
            Q_INTERFACES(VisualWorkspace::Core::IConfigPage)

        public:
            EditorTypesConfigWrapper(QObject* parent = 0);
            ~EditorTypesConfigWrapper();

            void initialize();

            // --------------------------------------------
            // IConfigPage Implementation
            // --------------------------------------------
            QIcon configPageIcon() const;
            QWidget* configPageWidget();
            QStringList configPageTitle() const;
            void configPageApply();

        private:
            EditorTypesConfigWrapperData* d;
        };
    }
}

#endif // EDITORTYPESCONFIGWRAPPER_H
