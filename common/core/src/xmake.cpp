/*
 * ****************************************************************************
 *  @author      xqyjlj
 *  @file        xmake.cpp
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
 *  2023-08-14     xqyjlj       initial version
 */

#include <QDebug>
#include <QFile>
#include <QRegularExpression>

#include "config.h"
#include "path.h"
#include "xmake.h"

QString xmake::version(const QString &program)
{
    QByteArray output;
    QString version;

    Q_ASSERT(!program.isEmpty());

    if (os::execv(program, QStringList() << "--version", {}, 10000, "", &output, nullptr))
    {
        const QRegularExpression regex(R"(v(\d+\.\d+\.\d+\+\w+\.\w+))");
        const QRegularExpressionMatch match = regex.match(output);

        if (match.hasMatch())
        {
            version = "v" + match.captured(1);
        }
    }
    return version;
}

QString xmake::lua(const QString &lua_path, const QStringList &args, const QString &program, const QString &workdir)
{
    QByteArray output;

    Q_ASSERT(!lua_path.isEmpty());
    Q_ASSERT(!program.isEmpty());

    if (os::execv(program, QStringList() << "lua" << path::absolute(lua_path) << args, {}, 10000, workdir, &output,
                  nullptr))
    {
    }
    else
    {
        qDebug()
            << QString("%1 lua %2 %3 failed. < %4 >").arg(program, path::absolute(lua_path), args.join(" "), output);
    }

    return output;
}

xmake::packages_t xmake::load_packages_byfile(const QString &file)
{
    Q_ASSERT(!file.isEmpty());
    Q_ASSERT(os::isfile(file));

    try
    {
        const std::string buffer = os::readfile(file).toStdString();
        const YAML::Node yaml_data = YAML::Load(buffer);
        return yaml_data.as<xmake::packages_t>();
    }
    catch (YAML::BadFile &e)
    {
        os::show_error_and_exit(e.what());
        throw;
    }
    catch (YAML::BadConversion &e)
    {
        os::show_error_and_exit(e.what());
        throw;
    }
    catch (std::exception &e)
    {
        qDebug() << e.what();
        throw;
    }
}

xmake::packages_t xmake::load_packages(const QString &program, const QString &workdir)
{
    const QString repodir = config::repodir();
    const QString script_path = QString("%1/tools/csp/dump_package.lua").arg(repodir);

    Q_ASSERT(!script_path.isEmpty());
    Q_ASSERT(os::isfile(script_path));
    Q_ASSERT(!program.isEmpty());

    const QString yml = xmake::lua(script_path, {"--json"}, program, workdir);
    try
    {
        const std::string buffer = yml.toStdString();
        const YAML::Node yaml_data = YAML::Load(buffer);
        return yaml_data.as<xmake::packages_t>();
    }
    catch (YAML::BadFile &e)
    {
        os::show_error_and_exit(e.what());
        throw;
    }
    catch (YAML::BadConversion &e)
    {
        os::show_error_and_exit(e.what());
        throw;
    }
    catch (std::exception &e)
    {
        qDebug() << e.what();
        throw;
    }
}
