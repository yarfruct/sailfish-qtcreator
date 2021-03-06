/*
    Copyright (C) 2016 Volker Krause <vkrause@kde.org>

    Permission is hereby granted, free of charge, to any person obtaining
    a copy of this software and associated documentation files (the
    "Software"), to deal in the Software without restriction, including
    without limitation the rights to use, copy, modify, merge, publish,
    distribute, sublicense, and/or sell copies of the Software, and to
    permit persons to whom the Software is furnished to do so, subject to
    the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
    IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "syntaxhighlighter.h"
#include "abstracthighlighter_p.h"
#include "definition.h"
#include "foldingregion.h"
#include "format.h"
#include "state.h"
#include "theme.h"

#include <QDebug>

Q_DECLARE_METATYPE(QTextBlock)

using namespace KSyntaxHighlighting;

namespace KSyntaxHighlighting {
class TextBlockUserData : public QTextBlockUserData
{
public:
    State state;
    QVector<FoldingRegion> foldingRegions;
};

class SyntaxHighlighterPrivate : public AbstractHighlighterPrivate
{
public:
    static FoldingRegion foldingRegion(const QTextBlock &startBlock);
    QVector<FoldingRegion> foldingRegions;
};

}

FoldingRegion SyntaxHighlighterPrivate::foldingRegion(const QTextBlock& startBlock)
{
    const auto data = dynamic_cast<TextBlockUserData*>(startBlock.userData());
    if (!data)
        return FoldingRegion();
    for (int i = data->foldingRegions.size() - 1; i >= 0; --i) {
        if (data->foldingRegions.at(i).type() == FoldingRegion::Begin)
            return data->foldingRegions.at(i);
    }
    return FoldingRegion();
}

SyntaxHighlighter::SyntaxHighlighter(QObject* parent) :
    QSyntaxHighlighter(parent),
    AbstractHighlighter(new SyntaxHighlighterPrivate)
{
    qRegisterMetaType<QTextBlock>();
}

SyntaxHighlighter::SyntaxHighlighter(QTextDocument *document) :
    QSyntaxHighlighter(document),
    AbstractHighlighter(new SyntaxHighlighterPrivate)
{
    qRegisterMetaType<QTextBlock>();
}

SyntaxHighlighter::~SyntaxHighlighter()
{
}

void SyntaxHighlighter::setDefinition(const Definition& def)
{
    const auto needsRehighlight = definition() != def;
    AbstractHighlighter::setDefinition(def);
    if (needsRehighlight)
        rehighlight();
}

bool SyntaxHighlighter::startsFoldingRegion(const QTextBlock &startBlock) const
{
    return SyntaxHighlighterPrivate::foldingRegion(startBlock).type() == FoldingRegion::Begin;
}

QTextBlock SyntaxHighlighter::findFoldingRegionEnd(const QTextBlock &startBlock) const
{
    const auto region = SyntaxHighlighterPrivate::foldingRegion(startBlock);

    auto block = startBlock;
    int depth = 1;
    while (block.isValid()) {
        block = block.next();
        const auto data = dynamic_cast<TextBlockUserData*>(block.userData());
        if (!data)
            continue;
        for (auto it = data->foldingRegions.constBegin(); it != data->foldingRegions.constEnd(); ++it) {
            if ((*it).id() != region.id())
                continue;
            if ((*it).type() == FoldingRegion::End)
                --depth;
            else if ((*it).type() == FoldingRegion::Begin)
                ++depth;
            if (depth == 0)
                return block;
        }
    }

    return QTextBlock();
}

void SyntaxHighlighter::highlightBlock(const QString& text)
{
    Q_D(SyntaxHighlighter);

    State state;
    if (currentBlock().position() > 0) {
        const auto prevBlock = currentBlock().previous();
        const auto prevData = dynamic_cast<TextBlockUserData*>(prevBlock.userData());
        if (prevData)
            state = prevData->state;
    }
    d->foldingRegions.clear();
    state = highlightLine(text, state);

    auto data = dynamic_cast<TextBlockUserData*>(currentBlockUserData());
    if (!data) { // first time we highlight this
        data = new TextBlockUserData;
        data->state = state;
        data->foldingRegions = d->foldingRegions;
        setCurrentBlockUserData(data);
        return;
    }

    if (data->state == state && data->foldingRegions == d->foldingRegions) // we ended up in the same state, so we are done here
        return;
    data->state = state;
    data->foldingRegions = d->foldingRegions;

    const auto nextBlock = currentBlock().next();
    if (nextBlock.isValid())
        QMetaObject::invokeMethod(this, "rehighlightBlock", Qt::QueuedConnection, Q_ARG(QTextBlock, nextBlock));
}

void SyntaxHighlighter::applyFormat(int offset, int length, const KSyntaxHighlighting::Format& format)
{
    if (format.isDefaultTextStyle(theme()) || length == 0)
        return;

    QTextCharFormat tf;
    if (format.hasTextColor(theme()))
        tf.setForeground(format.textColor(theme()));
    if (format.hasBackgroundColor(theme()))
        tf.setBackground(format.backgroundColor(theme()));

    if (format.isBold(theme()))
        tf.setFontWeight(QFont::Bold);
    if (format.isItalic(theme()))
        tf.setFontItalic(true);
    if (format.isUnderline(theme()))
        tf.setFontUnderline(true);
    if (format.isStrikeThrough(theme()))
        tf.setFontStrikeOut(true);

    QSyntaxHighlighter::setFormat(offset, length, tf);
}

void SyntaxHighlighter::applyFolding(int offset, int length, FoldingRegion region)
{
    Q_UNUSED(offset);
    Q_UNUSED(length);
    Q_D(SyntaxHighlighter);

    if (region.type() == FoldingRegion::Begin)
        d->foldingRegions.push_back(region);

    if (region.type() == FoldingRegion::End) {
        for (int i = d->foldingRegions.size() - 1; i >= 0; --i) {
            if (d->foldingRegions.at(i).id() != region.id() || d->foldingRegions.at(i).type() != FoldingRegion::Begin)
                continue;
            d->foldingRegions.remove(i);
            return;
        }
        d->foldingRegions.push_back(region);
    }
}
