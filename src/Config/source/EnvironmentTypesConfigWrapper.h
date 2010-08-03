/****************************************************************************
**
** Copyright 2010, Jaco Naude
**
****************************************************************************/

#ifndef ENVIRONMENTTYPESCONFIGWRAPPER_H
#define ENVIRONMENTTYPESCONFIGWRAPPER_H

#include "IConfigPage.h"
#include <QObject>
#include <QStringList>
#include <QIcon>
#include <QWidget>

namespace VisualWorkspace {
    namespace Core {
        /*!
          \struct EnvironmentTypesConfigWrapperData
          \brief The EnvironmentTypesConfigWrapperData class stores private data used by the EnvironmentTypesConfigWrapper class.
         */
        struct EnvironmentTypesConfigWrapperData;

        /*!
        \class EnvironmentTypesConfigWrapper
        \brief The config page for the session log.
          */
        class EnvironmentTypesConfigWrapper : public QObject, public IConfigPage
        {
            Q_OBJECT
            Q_INTERFACES(VisualWorkspace::Core::IConfigPage)

        public:
            EnvironmentTypesConfigWrapper(QObject* parent = 0);
            ~EnvironmentTypesConfigWrapper();

            void initialize();

            // --------------------------------------------
            // IConfigPage Implementation
            // --------------------------------------------
            QIcon configPageIcon() const;
            QWidget* configPageWidget();
            QStringList configPageTitle() const;
            void configPageApply();

        private:
            EnvironmentTypesConfigWrapperData* d;
        };
    }
}

#endif // ENVIRONMENTTYPESCONFIGWRAPPER_H
