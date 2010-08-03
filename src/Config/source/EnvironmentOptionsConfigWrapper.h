/****************************************************************************
**
** Copyright 2010, Jaco Naude
**
****************************************************************************/

#ifndef ENVIRONMENTOPTIONSCONFIGWRAPPER_H
#define ENVIRONMENTOPTIONSCONFIGWRAPPER_H

#include "IConfigPage.h"
#include <QObject>
#include <QStringList>
#include <QIcon>
#include <QWidget>

namespace VisualWorkspace {
    namespace Core {
        /*!
          \struct EnvironmentOptionsConfigWrapperData
          \brief The EnvironmentOptionsConfigWrapperData class stores private data used by the EnvironmentOptionsConfigWrapper class.
         */
        struct EnvironmentOptionsConfigWrapperData;

        /*!
        \class EnvironmentOptionsConfigWrapper
        \brief The config page for the session log.
          */
        class EnvironmentOptionsConfigWrapper : public QObject, public IConfigPage
        {
            Q_OBJECT
            Q_INTERFACES(VisualWorkspace::Core::IConfigPage)

        public:
            EnvironmentOptionsConfigWrapper(QObject* parent = 0);
            ~EnvironmentOptionsConfigWrapper();

            void initialize();

            // --------------------------------------------
            // IConfigPage Implementation
            // --------------------------------------------
            QIcon configPageIcon() const;
            QWidget* configPageWidget();
            QStringList configPageTitle() const;
            void configPageApply();

        private:
            EnvironmentOptionsConfigWrapperData* d;
        };
    }
}

#endif // ENVIRONMENTOPTIONSCONFIGWRAPPER_H
