/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-CLA-applies
 *
 * MuseScore
 * Music Composition & Notation
 *
 * Copyright (C) 2025 MuseScore Limited and others
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
import QtQuick
import MuseApi.Theme

//! NOTE Root namespace for all Qml Controls types
/** APIDOC
 * Qml UI Controls.  
 * When using controls and types from this module, you do not need to specify the namespace name, unlike others.
 * @namespace Controls
 * @example
 * import MuseApi.Controls
 * import MuseApi.Interactive
 * 
 * ExtensionBlank {
 *      FlatButton {
 *          icon: IconCode.STAR
 *          text: "Click me"
 *          onClicked: {
 *              Interactive.info("Test", "Clicked on button")
 *          }
 *      }
 * }
 */

/** APIDOC
 * Base class for extension form
 * @class ExtensionBlank
 * @hideconstructor
 * @example
 * import MuseApi.Controls
 *
 * ExtensionBlank {
 *     // content of extension form
 * }
*/
Rectangle {

    color: Theme.backgroundPrimaryColor

}
