/****************************************************************************
**
** Copyright (c) 2009-2010, Jaco Naude
**
** This file is part of Qtilities which is released under the following
** licensing options.
**
** Option 1: Open Source
** Under this license Qtilities is free software: you can
** redistribute it and/or modify it under the terms of the GNU General
** Public License as published by the Free Software Foundation, either
** version 3 of the License, or (at your option) any later version.
**
** Qtilities is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with Qtilities. If not, see http://www.gnu.org/licenses/.
**
** Option 2: Commercial
** Alternatively, this library is also released under a commercial license
** that allows the development of closed source proprietary applications
** without restrictions on licensing. For more information on this option,
** please see the project website's licensing page:
** http://www.qtilities.org/licensing.html
**
** If you are unsure which license is appropriate for your use, please
** contact support@qtilities.org.
**
****************************************************************************/

#ifndef IMODE_H
#define IMODE_H

#include "QtilitiesCoreGui_global.h"

#include <QWidget>
#include <QIcon>

namespace Qtilities {
    namespace CoreGui {
        namespace Interfaces {
            /*!
            \class IMode
            \brief Used by the ModeWidget to communicate with child modes.
              */
            class QTILITIES_CORE_GUI_SHARED_EXPORT IMode
            {

            public:
                IMode() {}
                virtual ~IMode() {}

                //! The main window's central widget for the mode.
                virtual QWidget* widget() = 0;
                //! This function is called in the initializeDependancies() function, thus all objects implementing interfaces in which a mode might be interested will be present in the global object pool.
                virtual void initialize() = 0;
                //! An icon for the mode.
                virtual QIcon icon() const = 0;
                //! The text used to represent the mode.
                virtual QString text() const = 0;
                //! Returns a context string for the context associated with this mode.
                virtual QString contextString() const = 0;
                //! Returns a help ID for this context.
                virtual QString contextHelpId() const { return QString(); }
                //! Returns a unique ID for this mode.
                virtual int modeID() const = 0;
            };
        }
    }
}

Q_DECLARE_INTERFACE(Qtilities::CoreGui::Interfaces::IMode,"com.qtilities.CoreGui.IMode/1.0");

#endif // IMODE_H
