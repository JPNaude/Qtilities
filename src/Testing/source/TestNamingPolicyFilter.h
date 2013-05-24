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

#ifndef TEST_NAMING_POLICY_FILTER_H
#define TEST_NAMING_POLICY_FILTER_H

#include "Testing_global.h"

#include "ITestable.h"

#include <QtTest/QtTest>

namespace Qtilities {
    namespace Testing {
        using namespace Interfaces;

        //! Allows testing of Qtilities::CoreGui::NamingPolicyFilter.
        class TESTING_SHARED_EXPORT TestNamingPolicyFilter: public QObject, public ITestable
        {
            Q_OBJECT
            Q_INTERFACES(Qtilities::Testing::Interfaces::ITestable)

        public:
            // --------------------------------
            // IObjectBase Implementation
            // --------------------------------
            QObject* objectBase() { return this; }
            const QObject* objectBase() const { return this; }

            // --------------------------------
            // ITestable Implementation
            // --------------------------------
            int execTest(int argc = 0, char ** argv = 0);
             QString testName() const { return tr("NamingPolicyFilter"); }

        private slots:
             //! Tests management of properties of NamingPolicyFilter.
             void testPropertyManagement();
            //! Tests NamingPolicyFilter::UniquenessPolicy.
            void testSetUniquenessPolicies();
            //! Tests NamingPolicyFilter::Reject for uniqueness of subject names.
            void testRejectUniquenessResolutionPolicy();
            //! Tests NamingPolicyFilter::AutoRename for uniqueness of subject names.
            void testAutoRenameUniquenessResolutionPolicy();
            //! Tests NamingPolicyFilter::ResolutionPolicy for validity of subject names.
            void testRejectValidityResolutionPolicy();
            //! Tests NamingPolicyFilter::processingCycleValidationChecks().
            void testProcessingCycleValidationChecks();
        };
    }
}

#endif // TEST_NAMING_POLICY_FILTER_H
