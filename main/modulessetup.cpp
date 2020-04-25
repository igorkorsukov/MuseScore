//=============================================================================
//  MuseScore
//  Music Composition & Notation
//
//  Copyright (C) 2019 MuseScore BVBA and others
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License version 2.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
//=============================================================================

#include "modulessetup.h"
#include "config.h"

#include "framework/uicomponents/uicomponentsmodule.h"

#ifdef BUILD_TELEMETRY_MODULE
#include "telemetry/telemetrysetup.h"
#endif

#ifdef AVSOMR
#include "avsomr/avsomrsetup.h"
#endif

//---------------------------------------------------------
//   ModulesSetup
//---------------------------------------------------------

ModulesSetup::ModulesSetup()
{

    m_modulesSetupList
            << new muf::UiComponentsModule()
#ifdef BUILD_TELEMETRY_MODULE
            << new TelemetrySetup()
#endif
#ifdef AVSOMR
            << new Ms::Avs::AvsOmrSetup()
#endif
               ;
}

//---------------------------------------------------------
//   setup
//---------------------------------------------------------

void ModulesSetup::setup()
{
    for (muf::IModuleSetup* m : m_modulesSetupList) {
        m->registerExports();
    }

    for (muf::IModuleSetup* m : m_modulesSetupList) {
        m->resolveImports();

        m->registerResources();
        m->registerUiTypes();
    }

    //! NOTE Need to move to the place where the application finishes initializing
    for (muf::IModuleSetup* m : m_modulesSetupList) {
        m->onStartInit();
    }
}

