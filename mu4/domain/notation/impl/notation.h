//=============================================================================
//  MuseScore
//  Music Composition & Notation
//
//  Copyright (C) 2020 MuseScore BVBA and others
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
#ifndef MU_DOMAIN_NOTATION_H
#define MU_DOMAIN_NOTATION_H

#include "../inotation.h"
#include "igetscore.h"

#include "notationinputcontroller.h"
#include "scorecallbacks.h"

namespace Ms {
class MScore;
class Element;
class Page;
}

namespace mu::domain::notation {

class Notation : public INotation, public IGetScore /*internal*/
{
public:
    Notation();

    //! NOTE Needed at the moment to initialize libmscore
    static void init();

    bool load(const std::string& path, const Params& params) override;
    void paint(QPainter* p, const QRect &r) override;

    INotationInputController* inputController() const override;

    void select(const Elem& e, SelectType type) override;
    void startNoteEntry() override;
    void cmd(const QString& cmd) override;

    // Internal
    Ms::MasterScore* masterScore() const override;

private:

    Ms::MasterScore* score() const;

    Ms::MScore* m_scoreGlobal = nullptr;
    Ms::MasterScore* m_score = nullptr;
    NotationInputController* m_inputController = nullptr;
    ScoreCallbacks* m_scoreCallbacks = nullptr;
};

}

#endif // MU_DOMAIN_NOTATION_H
