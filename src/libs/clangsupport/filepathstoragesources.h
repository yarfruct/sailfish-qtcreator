/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
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

#include <utils/smallstring.h>

#include <cstdint>
#include <vector>
#include <tuple>
#include <unordered_map>

namespace ClangBackEnd {
namespace Sources {
class Directory
{
public:
    Directory(int directoryId, Utils::PathString &&directoryPath)
        : directoryId(directoryId), directoryPath(std::move(directoryPath))
    {}

    friend
    bool operator==(const Directory &first, const Directory &second)
    {
        return first.directoryId == second.directoryId && first.directoryPath == second.directoryPath;
    }

public:
    int directoryId;
    Utils::PathString directoryPath;
};

class Source
{
public:
    Source(int sourceId, Utils::PathString &&sourceName)
        : sourceId(sourceId), sourceName(std::move(sourceName))
    {}

    friend
    bool operator==(const Source &first, const Source &second)
    {
        return first.sourceId == second.sourceId && first.sourceName == second.sourceName;
    }

public:
    int sourceId;
    Utils::PathString sourceName;
};

class SourceNameAndDirectoryId
{
public:
    SourceNameAndDirectoryId(Utils::SmallStringView sourceName, int directoryId)
        : sourceName(sourceName), directoryId(directoryId)
    {}

    Utils::SmallString sourceName;
    int directoryId = -1;
};
} // namespace ClangBackEnd

} // namespace ClangBackEnd
