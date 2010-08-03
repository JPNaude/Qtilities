/****************************************************************************
**
** Copyright 2010, Jaco Naude
**
****************************************************************************/

#ifndef SCRIPTINGOPTIONSCONFIGWRAPPER_H
#define SCRIPTINGOPTIONSCONFIGWRAPPER_H

#include "IConfigPage.h"
#include <QObject>
#include <QStringList>
#include <QIcon>
#include <QWidget>

namespace VisualWorkspace {
    namespace Core {
        /*!
          \struct ScriptingOptionsConfigWrapperData
          \brief The ScriptingOptionsConfigWrapperData class stores private data used by the ScriptingOptionsConfigWrapper class.
         */
        struct ScriptingOptionsConfigWrapperData;

        /*!
        \class ScriptingOptionsConfigWrapper
        \brief The config page for the session log.
          */
        class ScriptingOptionsConfigWrapper : public QObject, public IConfigPage
        {
            Q_OBJECT
            Q_INTERFACES(VisualWorkspace::Core::IConfigPage)

        public:
            ScriptingOptionsConfigWrapper(QObject* parent = 0);
            ~ScriptingOptionsConfigWrapper();

            void initialize();

            // --------------------------------------------
            // IConfigPage Implementation
            // --------------------------------------------
            QIcon configPageIcon() const;
            QWidget* configPageWidget();
            QStringList configPageTitle() const;
            void configPageApply();

        private:
            ScriptingOptionsConfigWrapperData* d;
        };
    }
}

#endif // SCRIPTINGOPTIONSCONFIGWRAPPER_H
