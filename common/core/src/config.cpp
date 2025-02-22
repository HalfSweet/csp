/*
 * ****************************************************************************
 *  @author      xqyjlj
 *  @file        csp_config.cpp
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
 *  2023-05-14     xqyjlj       initial version
 */
#include <QFile>
#include <QProcess>
#include <QSysInfo>

#include "config.h"
#include "os.h"
#include "path.h"

static constexpr const char *csp_config_file_path = "config.ini";
static constexpr const char *csp_config_default_value = "null";

static constexpr const char *csp_config_key_repo_dir = "core/repodir";
static constexpr const char *csp_config_value_default_repo_dir = "repo";

static constexpr const char *csp_config_key_xmake_repo_dir = "core/xmake_repodir";
static constexpr const char *csp_config_value_default_xmake_repo_dir = "xmake";

static constexpr const char *csp_config_key_language = "core/language";
static constexpr const char *csp_config_value_default_language = "zh_CN";

static constexpr const char *csp_config_key_workspace = "core/workspace";
static constexpr const char *csp_config_value_default_workspace = "workspace";

static constexpr const char *csp_config_key_repositories = "core/repositories";
static constexpr const char *csp_config_value_default_repositories = "repositories";

static constexpr const char *csp_config_key_tool_xmake = "tool/xmake";
static constexpr const char *csp_config_value_default_tool_xmake = "xmake";

static constexpr const char *csp_config_key_tool_git = "tool/git";
static constexpr const char *csp_config_value_default_tool_git = "git";

bool config::is_config(const QString &key)
{
    return _settings->value(key, csp_config_default_value).toString() != csp_config_default_value;
}

void config::init()
{
    _settings = new QSettings(csp_config_file_path, QSettings::IniFormat);

    if (!is_config(csp_config_key_repo_dir))
    {
        _settings->setValue(csp_config_key_repo_dir,
                            QString("%1/%2").arg(path::appdir(), csp_config_value_default_repo_dir));
    }
    if (!is_config(csp_config_key_xmake_repo_dir))
    {
        _settings->setValue(csp_config_key_xmake_repo_dir,
                            QString("%1/%2").arg(path::appdir(), csp_config_value_default_xmake_repo_dir));
    }
    if (!is_config(csp_config_key_language))
    {
        _settings->setValue(csp_config_key_language, csp_config_value_default_language);
    }

    if (!is_config(csp_config_key_repositories))
    {
        _settings->setValue(csp_config_key_repositories,
                            QString("%1/%2").arg(path::appdir(), csp_config_value_default_repositories));
    }

    if (!is_config(csp_config_key_workspace))
    {
        const auto appdir = QString("%1/%2").arg(path::appdir(), csp_config_value_default_workspace);
        if (!os::exists(appdir))
        {
            os::mkdir(appdir);
        }
        else
        {
            if (!os::isdir(appdir)) // check if it not is a directory
            {
                os::show_error_and_exit(QObject::tr("The workspace <%1> path is not a directory!").arg(appdir));
            }
        }
        _settings->setValue(csp_config_key_workspace, appdir);
    }
    if (!is_config(csp_config_key_tool_xmake))
    {
        _settings->setValue(csp_config_key_tool_xmake, find_tool_xmake());
    }
    if (!is_config(csp_config_key_tool_git))
    {
        _settings->setValue(csp_config_key_tool_git, find_tool_git());
    }
}

void config::deinit()
{
    delete _settings;
    _settings = nullptr;
}

QString config::get(const QString &key)
{
    Q_ASSERT(_settings != nullptr);
    Q_ASSERT(!key.isEmpty());
    return _settings->value(key, csp_config_default_value).toString();
}

QString config::repodir()
{
    Q_ASSERT(_settings != nullptr);
    return _settings->value(csp_config_key_repo_dir, csp_config_value_default_repo_dir).toString();
}

QString config::xmake_repodir()
{
    Q_ASSERT(_settings != nullptr);
    return _settings->value(csp_config_key_xmake_repo_dir, csp_config_value_default_xmake_repo_dir).toString();
}

void config::set(const QString &key, const QString &value)
{
    Q_ASSERT(_settings != nullptr);
    _settings->setValue(key, value);
}

QString config::language()
{
    Q_ASSERT(_settings != nullptr);
    return _settings->value(csp_config_key_language, csp_config_value_default_language).toString();
}

QString config::workspace_dir()
{
    Q_ASSERT(_settings != nullptr);
    const auto dir = QString("%1/%2").arg(path::appdir(), csp_config_value_default_workspace);
    return _settings->value(csp_config_key_workspace, dir).toString();
}

QString config::default_workdir()
{
    const QString workdir = QString("%1/workdir").arg(path::appdir());
    const QString xmake_lua = QString("%1/xmake.lua").arg(workdir);
    os::mkdir(workdir);

    if (!os::exists(xmake_lua))
    {
        os::writefile(xmake_lua, "", true);
    }

    return workdir;
}

QMap<QString, QString> config::env()
{
    QMap<QString, QString> map;
#ifdef Q_OS_WINDOWS
    const QProcessEnvironment environment = QProcessEnvironment::systemEnvironment();
    QString env_path = environment.value("Path");
    const QString arch = QSysInfo::currentCpuArchitecture();
    const QString tooldir = QString("%1/tools").arg(path::appdir());
    const QString gitpath = QString("%1/git/cmd/git.exe").arg(tooldir);
    if (os::exists(gitpath))
    {
        env_path = QString("%1/git/cmd;%2").arg(tooldir, env_path);
        if ("x86_64" == arch)
        {
            env_path = QString("%1;%2/git/mingw64/bin").arg(env_path, tooldir);
        }
        else if ("i386" == arch)
        {
            env_path = QString("%1;%2/git/mingw32/bin").arg(env_path, tooldir);
        }
        map.insert("Path", env_path);
    }
#endif

    map.insert("XMAKE_RCFILES", QString("%1/tools/scripts/xmake.lua").arg(xmake_repodir()));
    map.insert("XMAKE_THEME", "plain");

    return map;
}

QString config::repositories_dir()
{
    Q_ASSERT(_settings != nullptr);
    const QString dir = QString("%1/%2").arg(path::appdir(), csp_config_value_default_repositories);
    return _settings->value(csp_config_key_repositories, dir).toString();
}

QString config::tool_xmake()
{
    Q_ASSERT(_settings != nullptr);
    return _settings->value(csp_config_key_tool_xmake, csp_config_value_default_tool_xmake).toString();
}

QString config::tool_git()
{
    Q_ASSERT(_settings != nullptr);
    return _settings->value(csp_config_key_tool_git, csp_config_value_default_tool_git).toString();
}

QString config::find_tool_xmake()
{
    const QString tooldir = QString("%1/tools").arg(path::appdir());
#ifdef Q_OS_WINDOWS
    QString xmakepath = QString("%1/xmake/xmake.exe").arg(tooldir);
    if (os::exists(xmakepath))
    {
        return xmakepath;
    }
#endif

    return csp_config_value_default_tool_xmake;
}

QString config::find_tool_git()
{
    const QString tooldir = QString("%1/tools").arg(path::appdir());
#ifdef Q_OS_WINDOWS
    QString gitpath = QString("%1/git/cmd/git.exe").arg(tooldir);
    if (os::exists(gitpath))
    {
        return gitpath;
    }
#endif

    return csp_config_value_default_tool_git;
}
