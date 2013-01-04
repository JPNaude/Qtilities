/****************************************************************************
**
** Copyright (c) 2009-2013, Jaco Naude
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

#ifndef QTILITIES_PROCESS_H
#define QTILITIES_PROCESS_H

#include "QtilitiesCore_global.h"
#include "Task.h"

#include <QObject>
#include <QProcess>

namespace Qtilities {
    namespace Core {
        /*!
        \struct QtilitiesProcessPrivateData
        \brief Structure used by QtilitiesProcess to store private data.
          */
        struct QtilitiesProcessPrivateData;

        /*!
        \class QtilitiesProcess
        \brief An easy to use way to launch external processes through an extended wrapper around QProcess.

        The QtilitiesProcess class simplifies usage of QProcess and provides ready to use logging and task integration capablities.

        When logging is enabled, QtilitiesProcess will automatically log all \p stdout and \p stderr outputs in a logger engine.
        It takes care of splitting up messages received from the QProcess buffer for you, thus individual messages are logged to the
        logger engine.
          */
        class QTILIITES_CORE_SHARED_EXPORT QtilitiesProcess : public Task
        {
            Q_OBJECT
            Q_INTERFACES(Qtilities::Core::Interfaces::ITask)

        public:
            //! Constructs a new QtilitiesProcess instance.
            /*!
             * \brief QtilitiesProcess
             * \param task_name The name of the task.
             * \param enable_logging Indicates if messages received from the task must be buffered and logged to a task log assigned to this task.
             * \param read_process_buffers Indicates if messages in the process's buffers must be processed. When false, the process buffer won't be touched and you can manually access it through the process() function.
             * \param parent The parent of this process.
             */
            QtilitiesProcess(const QString& task_name, bool enable_logging = true, bool read_process_buffers = true, QObject* parent = 0);
            virtual ~QtilitiesProcess();

            //! Starts the process, similar to QProcess::start().
            /*!
                \returns True when the task was started successfully (thus waitForStarted() returned true), false otherwise.
              */
            virtual bool startProcess(const QString& program, const QStringList& arguments, QProcess::OpenMode mode = QProcess::ReadWrite);

            //! Access to the QProcess instance contained and used within this object.
            QProcess* process();

            //! Sets the strings to be used for splitting logged messages.
            /*!
              By default this list is empty an a line break character \r is used to received split messages.

              You can however specify multiple strings to use using this function.

              <i>This function was added in %Qtilities v1.1.</i>

              \sa lineBreakStrings()
              */
            void setLineBreakStrings(const QStringList& line_break_strings);
            //! Gets the strings to be used for splitting logged messages.
            /*!
              <i>This function was added in %Qtilities v1.1.</i>

              \sa setLineBreakStrings()
              */
            QStringList lineBreakStrings();

        private slots:
            void procStarted();
            void procFinished(int exit_code, QProcess::ExitStatus exit_status);
            void procError(QProcess::ProcessError error);
            void procStateChanged(QProcess::ProcessState newState);
            void logProgressOutput();
            void logProgressError();

        public slots:
            //! Stops the process.
            /*!
                This function will first call terminate() on the process, wait and then call kill().

                Its important to call Task::stop() at the end of your implementation.
            */
            virtual void stopProcess();

        signals:
            //! Emitted when new standard output becomes available.
            /*!
             * <i>This function was added in %Qtilities v1.2.</i>
             */
            void newStandardOutputMessage(const QString& message);
            //! Emitted when new standard error becomes available.
            /*!
             * <i>This function was added in %Qtilities v1.2.</i>
             */
            void newStandardErrorMessage(const QString& message);

        private:
            QtilitiesProcessPrivateData* d;
        };
    }
}

#endif // QTILITIES_PROCESS_H
