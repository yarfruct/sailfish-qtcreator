/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of Qt Creator.
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
****************************************************************************/

#include "rmmertargetoperation.h"
#include "addmertargetoperation.h"
#include "addkeysoperation.h"
#include "rmkeysoperation.h"
#include "../../plugins/mer/merconstants.h"
#include "../../plugins/mer/mertargetsxmlparser.h"
#include <iostream>
#include <QTextStream>
#include <QDir>

const char MER_PARAM_MER_TARGETS_DIR[] = "--mer-targets-dir";
const char MER_PARAM_TARGET_NAME[] = "--target-name";

RmMerTargetOperation::RmMerTargetOperation()
{
}

QString RmMerTargetOperation::name() const
{
    return QLatin1String("removeMerTarget");
}

QString RmMerTargetOperation::helpText() const
{
    return QLatin1String("remove mer target to Qt Creator configuration");
}

QString RmMerTargetOperation::argumentsHelpText() const
{
    const QString indent = QLatin1String("    ");
    return indent + QLatin1String(MER_PARAM_MER_TARGETS_DIR) + QLatin1String(" <PATH>   shared \"targets\" folder (required).\n")
         + indent + QLatin1String(MER_PARAM_TARGET_NAME) + QLatin1String(" <NAME>       display name (required).\n");
}

bool RmMerTargetOperation::setArguments(const QStringList &args)
{
    for (int i = 0; i < args.count(); ++i) {
        const QString current = args.at(i);
        const QString next = ((i + 1) < args.count()) ? args.at(i + 1) : QString();

        if (current == QLatin1String(MER_PARAM_MER_TARGETS_DIR)) {
            if (next.isNull())
                return false;
            ++i; // skip next;
            m_targetsDir = next;
            continue;
        }

        if (current == QLatin1String(MER_PARAM_TARGET_NAME)) {
            if (next.isNull())
                return false;
            ++i; // skip next;
            m_targetName = next;
            continue;
        }
    }
    return !m_targetsDir.isEmpty() && !m_targetName.isEmpty();
}


int RmMerTargetOperation::execute() const
{
    QVariantMap map = AddMerTargetOperation::load(m_targetsDir);
    if (map.isEmpty())
        map = AddMerTargetOperation::initializeTargets();

    const QVariantMap result = removeTarget(map, m_targetName);

    if (result.isEmpty() || map == result)
        return 2;

    return AddMerTargetOperation::save(result, m_targetsDir) ? 0 : 3;
}

QVariantMap RmMerTargetOperation::removeTarget(const QVariantMap &map, const QString &targetName)
{
    QVariantMap result = AddMerTargetOperation::initializeTargets();

    QVariantList targetList;
    QVariantList targetData = map.value(QLatin1String(Mer::Constants::MER_TARGET_KEY)).toList();
    foreach (const QVariant &data, targetData) {
        Mer::MerTargetData d = data.value<Mer::MerTargetData>();
        if ( d.name == targetName)
            continue;
        targetList << data;
    }

    if (targetList.count() == targetData.count()) {
        std::cerr << "Error: Target was not found." << std::endl;
        return map;
    }

    KeyValuePairList data;
    data << KeyValuePair(QStringList() << QLatin1String(Mer::Constants::MER_TARGET_KEY), targetList);
    return AddKeysOperation::addKeys(result, data);
}

#ifdef WITH_TESTS
bool RmMerTargetOperation::test() const
{
    QFile file(QLatin1String("./test"));
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream out(&file);
    out << "This file is generated by unit test\n";
    file.close();

    QVariantMap map = AddMerTargetOperation::addTarget(AddMerTargetOperation::initializeTargets(),
                                                       QLatin1String("target1"),
                                                       QLatin1String("./test"),
                                                       QLatin1String("./test"),
                                                       QLatin1String("./test"),
                                                       QLatin1String("./test"),
                                                       QLatin1String("./test"));


    map = AddMerTargetOperation::addTarget(map,
                                           QLatin1String("target2"),
                                           QLatin1String("./test"),
                                           QLatin1String("./test"),
                                           QLatin1String("./test"),
                                           QLatin1String("./test"),
                                           QLatin1String("./test"));
    if (map.count() != 2
            || !map.contains(QLatin1String(Mer::Constants::MER_TARGET_FILE_VERSION_KEY))
            || map.value(QLatin1String(Mer::Constants::MER_TARGET_FILE_VERSION_KEY)).toInt() != 2
            || !map.contains(QLatin1String(Mer::Constants::MER_TARGET_KEY)))
        return false;

    QVariantMap result = removeTarget(map, QLatin1String("target1"));
    if (result.count() != 2
            || !map.contains(QLatin1String(Mer::Constants::MER_TARGET_FILE_VERSION_KEY))
            || map.value(QLatin1String(Mer::Constants::MER_TARGET_FILE_VERSION_KEY)).toInt() != 2
            || !map.contains(QLatin1String(Mer::Constants::MER_TARGET_KEY)))
        return false;

    QVariantList targetData = result.value(QLatin1String(Mer::Constants::MER_TARGET_KEY)).toList();

    foreach (const QVariant &data, targetData) {
         Mer::MerTargetData d = data.value<Mer::MerTargetData>();
         if ( d.name == QLatin1String("target1"))
             return false;
    }

    result = removeTarget(map, QLatin1String("unknown"));
    if (result != map)
        return false;

    result = removeTarget(map, QLatin1String("target2"));
    if (result.count() != 2
            || !map.contains(QLatin1String(Mer::Constants::MER_TARGET_FILE_VERSION_KEY))
            || map.value(QLatin1String(Mer::Constants::MER_TARGET_FILE_VERSION_KEY)).toInt() != 2
            || !map.contains(QLatin1String(Mer::Constants::MER_TARGET_KEY)))
        return false;

    targetData = result.value(QLatin1String(Mer::Constants::MER_TARGET_KEY)).toList();
    foreach (const QVariant &data, targetData) {
         Mer::MerTargetData d = data.value<Mer::MerTargetData>();
         if ( d.name == QLatin1String("target2"))
             return false;
    }

    QDir().remove(file.fileName());

    return true;
}
#endif
