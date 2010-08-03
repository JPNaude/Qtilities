/****************************************************************************
**
** Copyright 2010, Jaco Naude
**
****************************************************************************/

#ifndef SCRIPTBACKENDTYPESCONFIGWRAPPER_H
#define SCRIPTBACKENDTYPESCONFIGWRAPPER_H

#include "IConfigPage.h"
#include <QObject>
#include <QStringList>
#include <QIcon>
#include <QWidget>

namespace VisualWorkspace {
    namespace Core {
        /*!
          \struct ScriptBackendTypesConfigWrapperData
          \brief The ScriptBackendTypesConfigWrapperData class stores private data used by the ScriptBackendTypesConfigWrapper class.
         */
        struct ScriptBackendTypesConfigWrapperData;

        /*!
        \class ScriptBackendTypesConfigWrapper
        \brief The config page for the script backend types config widget.
          */
        class ScriptBackendTypesConfigWrapper : public QObject, public IConfigPage
        {
            Q_OBJECT
            Q_INTERFACES(VisualWorkspace::Core::IConfigPage)

        public:
            ScriptBackendTypesConfigWrapper(QObject* parent = 0);
            ~ScriptBackendTypesConfigWrapper();

            void initialize();

            // --------------------------------------------
            // IConfigPage Implementation
            // --------------------------------------------
            QIcon configPageIcon() const;
            QWidget* configPageWidget();
            QStringList configPageTitle() const;
            void configPageApply();

        private:
            ScriptBackendTypesConfigWrapperData* d;
        };
    }
}

#endif // SCRIPTBACKENDTYPESCONFIGWRAPPER_H
