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

#ifndef OBSERVERSTRINGSUBJECT_H
#define OBSERVERSTRINGSUBJECT_H

#include <IExportable.h>
#include <Factory.h>
#include <IModificationNotifier.h>

#include <QObject>
#include <QString>

using namespace Qtilities::Core::Interfaces;

/*!
  \struct ObserverStringSubjectData
  \brief The ObserverStringSubjectData struct stores private data used by the ObserverStringSubject class.
 */
struct ObserverStringSubjectData;

/*!
\class ObserverStringSubject
\brief Provides an observer subject which can be used to build simple trees.

ObserverStringSubject classes can be used to build simple & exportable trees using the Observer architecture.
Each instance can only be attach to a single observer at a time. The string is usually set in the constructor, or
can be set using setString(). The current displayed string is retreived using string().
  */
class ObserverStringSubject : public QObject, public IExportable, public IModificationNotifier
{
    Q_OBJECT
    Q_INTERFACES(Qtilities::Core::Interfaces::IExportable)
    Q_INTERFACES(Qtilities::Core::Interfaces::IModificationNotifier)

    Q_PROPERTY(bool Modified READ isModified());

public:
    explicit ObserverStringSubject(const QString& string = QString(), QObject *parent = 0);
    ~ObserverStringSubject();

    //! Sets the string to be displayed.
    void setString(const QString& string);
    //! Gets the string that is displayed.
    QString string() const;

    // --------------------------------
    // IObjectBase Implemenation
    // --------------------------------
    QObject* objectBase() { return this; }

    // --------------------------------
    // IExportable Implemenation
    // --------------------------------
    ExportModeFlags supportedFormats() const;
    IFactoryData factoryData() const;
    IExportable::Result exportBinary(QDataStream& stream) const;
    IExportable::Result importBinary(QDataStream& stream, QList<QPointer<QObject> >& import_list);

    // --------------------------------
    // IModificationNotifier Implemenation
    // --------------------------------
    bool isModified() const;
    QObject* modifierBase() { return this; }
public slots:
    void setModificationState(bool new_state, bool notify_listeners = true, bool notify_subjects = false);
signals:
    void modificationStateChanged(bool is_modified) const;
    void partialStateChanged(const QString& part_name) const;

public:
    // --------------------------------
    // Factory Interface Implemenation
    // --------------------------------
    static FactoryItem<QObject, ObserverStringSubject> factory;

private:
    ObserverStringSubjectData* d;

};

#endif // OBSERVERSTRINGSUBJECT_H
