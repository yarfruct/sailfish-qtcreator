/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
****************************************************************************/

#pragma once

#include "qmlprofiler_global.h"

#include <QString>

namespace QmlProfiler {

class QMLPROFILER_EXPORT QmlEventLocation
{
public:
    QmlEventLocation() : m_line(-1),m_column(-1) {}
    QmlEventLocation(const QString &file, int lineNumber, int columnNumber) : m_filename(file),
        m_line(lineNumber), m_column(columnNumber)
    {}

    void clear()
    {
        m_filename.clear();
        m_line = m_column = -1;
    }

    bool isValid() const
    {
        return !m_filename.isEmpty();
    }

    QString filename() const { return m_filename; }
    int line() const { return m_line; }
    int column() const { return m_column; }

private:
    friend QDataStream &operator>>(QDataStream &stream, QmlEventLocation &location);
    friend QDataStream &operator<<(QDataStream &stream, const QmlEventLocation &location);

    QString m_filename;
    int m_line;
    int m_column;
};

inline bool operator==(const QmlEventLocation &location1, const QmlEventLocation &location2)
{
    // compare filename last as it's expensive.
    return location1.line() == location2.line() && location1.column() == location2.column()
            && location1.filename() == location2.filename();
}

inline bool operator!=(const QmlEventLocation &location1, const QmlEventLocation &location2)
{
    return !(location1 == location2);
}

QDataStream &operator>>(QDataStream &stream, QmlEventLocation &location);
QDataStream &operator<<(QDataStream &stream, const QmlEventLocation &location);

} // namespace QmlProfiler
