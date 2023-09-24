/*
 * ****************************************************************************
 *  @author      xqyjlj
 *  @file        testcase_pinout_table.cpp
 *  @brief
 *
 * ****************************************************************************
 *  @attention
 *  Licensed under the GNU General Public License v. 3 (the "License");
 *  You may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      https://www.gnu.org/licenses/gpl-3.0.html
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  Copyright (C) 2023-2023 xqyjlj<xqyjlj@126.com>
 *
 * ****************************************************************************
 *  Change Logs:
 *  Date           Author       Notes
 *  ------------   ----------   -----------------------------------------------
 *  2023-05-28     xqyjlj       initial version
 */
#include <QDebug>
#include <QtTest>

#include "config.h"
#include "pinout_table.h"

class testcase_pinout_table final : public QObject {
    Q_OBJECT

private slots:

    static void initTestCase()
    {
        config::init();
    }

    static void load_pinout()
    {
        const auto pinout = pinout_table::load_pinout(":/pinout.yml");
        QVERIFY(!pinout.isEmpty());
    }

    static void cleanupTestCase()
    {
        config::deinit();
    }
};

QTEST_MAIN(testcase_pinout_table)

#include "testcase_pinout_table.moc"
