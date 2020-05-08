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
#include "notation.h"

#include <QPointF>
#include <QPainter>

#include "libmscore/score.h"
#include "libmscore/page.h"

#include "scorehelpful.h"

//! HACK Temporary hack to link libmscore
Q_LOGGING_CATEGORY(undoRedo, "undoRedo", QtCriticalMsg)

namespace Ms {

QString revision;
MasterSynthesizer* synti;
QString dataPath;
QString mscoreGlobalShare;

}
//! ---------


using namespace mu::domain::notation;

Notation::Notation()
{
    m_scoreGlobal = new Ms::MScore();
    m_score = new Ms::MasterScore(m_scoreGlobal->baseStyle());

    m_inputController = new NotationInputController(this /*IGetScore*/);
    m_scoreCallbacks = new ScoreCallbacks();
}

void Notation::init()
{
    Ms::MScore::init();         // initialize libmscore
}

bool Notation::load(const std::string& path, const Params &params)
{
    Ms::Score::FileError rv = m_score->loadMsc(QString::fromStdString(path), true);
    if (rv != Ms::Score::FileError::FILE_NO_ERROR) {
        return false;
    }

    //    Ms::MStyle& styleRef = m_score->style();
    //    styleRef.set(Ms::Sid::pageWidth, params.pageSize.width / Ms::DPI);
    // styleRef.set(Ms::Sid::pageHeight, params.pageSize.height / Ms::DPI);

    //    styleRef.set(Ms::Sid::pagePrintableWidth, (pageSize.pageWidth - pageSize.margingLeft
    //                                               - pageSize.margingRight) / Ms::DPI);
    //    styleRef.set(Ms::Sid::pageEvenLeftMargin, pageSize.margingLeft / Ms::DPI);
    //    styleRef.set(Ms::Sid::pageOddLeftMargin, pageSize.margingLeft / Ms::DPI);

    //    styleRef.set(Ms::Sid::pageEvenTopMargin, pageSize.margingTop / Ms::DPI);
    //    styleRef.set(Ms::Sid::pageOddTopMargin, pageSize.margingTop / Ms::DPI);
    //    styleRef.set(Ms::Sid::pageEvenBottomMargin, pageSize.margingBottom / Ms::DPI);
    //    styleRef.set(Ms::Sid::pageOddBottomMargin, pageSize.margingBottom / Ms::DPI);

    m_score->setUpdateAll();
    m_score->doLayout();

    return true;
}

void Notation::paint(QPainter* p, const QRect& r)
{
    const QList<Ms::Page*>& mspages = score()->pages();

    if (mspages.isEmpty()) {
        p->drawText(10, 10, "no pages");
        return;
    }

    Ms::Page* page = mspages.first();
    page->draw(p);

    p->fillRect(page->bbox(), QColor("#ffffff"));

    QList<Ms::Element*> ell = page->elements();
    for (const Ms::Element* e : ell) {
        if (!e->visible()) {
            continue;
        }

        e->itemDiscovered = false;
        QPointF pos(e->pagePos());
        //LOGI() << e->name() << ", x: " << pos.x() << ", y: " << pos.y() << "\n";

        p->translate(pos);

        e->draw(p);

        p->translate(-pos);
    }
}

void Notation::select(const Elem& e, SelectType type)
{
    score()->select(ScoreHelpful::toMsElement(e), ScoreHelpful::toMsSelectType(type));
}

void Notation::cmd(const QString& cmd)
{
    Ms::EditData ed;
    ed.view = m_scoreCallbacks;
    score()->cmd(cmd, ed);
}

Ms::MasterScore* Notation::score() const
{
    return m_score;
}

INotationInputController* Notation::inputController() const
{
    return m_inputController;
}

Ms::MasterScore* Notation::masterScore() const
{
    return m_score;
}
