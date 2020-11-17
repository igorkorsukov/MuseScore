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

#include "telemetrysetup.h"

#include <QQmlEngine>

#include "modularity/ioc.h"

#include "itelemetryservice.h"
#include "internal/telemetryservice.h"
#include "view/telemetrypermissionmodel.h"

using namespace mu::telemetry;

static void telemetry_init_qrc()
{
    Q_INIT_RESOURCE(telemetry);
}


std::string TelemetrySetup::moduleName() const
{
    return "telemetry";
}

void TelemetrySetup::registerResources()
{
    telemetry_init_qrc();
}

void TelemetrySetup::registerExports()
{
    mu::framework::ioc()->registerExport<ITelemetryService>("telemetry", new TelemetryService());
}

void TelemetrySetup::registerUiTypes()
{
    qmlRegisterType<TelemetryPermissionModel>("MuseScore.Telemetry", 3, 3, "TelemetryPermissionModel");
}
