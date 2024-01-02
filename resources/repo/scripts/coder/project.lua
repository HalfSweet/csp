--
-- Licensed under the GNU General Public License v. 3 (the "License")
-- You may not use this file except in compliance with the License.
-- You may obtain a copy of the License at
--
--     https://www.gnu.org/licenses/gpl-3.0.html
--
-- Unless required by applicable law or agreed to in writing, software
-- distributed under the License is distributed on an "AS IS" BASIS,
-- WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
-- See the License for the specific language governing permissions and
-- limitations under the License.
--
-- Copyright (C) 2023-2023 xqyjlj<xqyjlj@126.com>
--
-- @author      xqyjlj
-- @file        project.lua
--
-- Change Logs:
-- Date           Author       Notes
-- ------------   ----------   -----------------------------------------------
-- 2023-12-29     xqyjlj       initial version
--
import("core.base.json")

local license = [[
--
-- ----------------------------------------------------------------------------
--  @author      csplink (${{version}})
--  @file        xmake.lua
--  @brief       file automatically-generated by tool: [csp] version: [${{version}}]
--                  date: ${{date_t}}
--  @note        See more information at https://xmake.io
--
-- ----------------------------------------------------------------------------
--  @attention
--
--  Copyright (C) ${{date}} csplink software.
--  All rights reserved.
--
-- ----------------------------------------------------------------------------
--
]]

local user_code_begin_template = "----< add user code begin %s"
local user_code_end_template = "----> add user code end %s"
local user_code_begin_match = "----< add user code begin " -- .. "(.-)"
local user_code_end_match = "----> add user code end " -- .. "(.-)"

function _generate_header(file, proj, coder, user)
    user = user or {}
    file:print(user_code_begin_template, "header")
    if user.header then
        file:printf(user.header)
    else
        local kind = path.basename(file:path())
        local builtinvars = {}
        builtinvars.version = string.lower(proj["version"] or "v0.0.0.0")
        builtinvars.date_t = os.date()
        builtinvars.date = os.date("%Y")

        local header = string.gsub(license:trim(), "%${{(.-)}}", function(variable)
            variable = variable:trim()
            local value = builtinvars[variable]
            return type(value) == "function" and value() or value
        end)
        file:print(header)
    end
    file:print(user_code_end_template, "header")
    file:print("")
end

function _generate_user(file, kind, user, prefix, is_end)
    is_end = is_end or false
    prefix = prefix or ""
    file:print(prefix .. user_code_begin_template, kind)
    if user[kind] then
        file:printf(user[kind])
    else
        file:print("")
    end
    file:print(prefix .. user_code_end_template, kind)
    if not is_end then
        file:print("")
    end
end

function _generate_includes(file, proj, coder, user)
    file:print("---------------------------------- includes -----------------------------------")
    file:print("includes(\"hal/xmake.lua\")")
    _generate_user(file, "includes", user)
end

function _generate_project_rules(file, proj, coder, user)
    file:print("-------------------------------- project rules --------------------------------")
    file:print("add_rules(\"mode.debug\", \"mode.release\")")
    file:print("add_rules(\"plugin.compile_commands.autoupdate\", {outputdir = \".vscode\"})")
    _generate_user(file, "project rules", user)
end

function _generate_project_config(file, proj, coder, user)
    file:print("-------------------------------- project config -------------------------------")
    file:print(user_code_begin_template, "project config")
    if user[kind] then
        file:printf(user[kind])
    else
        file:print("set_project(\"%s\") -- Set project name", proj["name"])
        file:print("set_version(\"0.0.0\") -- Set version")
        file:print("set_xmakever(\"2.8.3\") -- Set minimal xmake version")
    end
    file:print(user_code_end_template, "project config")
    file:print("")
end

function _generate_toolchains(file, proj, coder, user)
    file:print("---------------------------------- toolchains ---------------------------------")
    file:print("-- Set toolchains, which will be automatically downloaded and installed by xmake")
    file:print("add_requires(\"%s\")", proj["core"]["toolchains"])
    file:print("set_toolchains(\"@%s\")", proj["core"]["toolchains"])
    file:print("")
end

function _generate_flags(file, proj, coder, user)
    file:print("------------------------------------ flags ------------------------------------")
    file:print(user_code_begin_template, "flags")
    if user["flags"] then
        file:printf(user["flags"])
    else
        local flags = coder.get_flags(proj)
        if flags.ldflags and #flags.ldflags > 0 then
            file:print("add_ldflags(\"%s\", {force = true})", table.concat(flags.ldflags, "\", \""))
        end
        if flags.cflags and #flags.cflags > 0 then
            file:print("add_cflags(\"%s\", {force = true})", table.concat(flags.cflags, "\", \""))
        end
        if flags.cxxflags and #flags.cxxflags > 0 then
            file:print("add_cxxflags(\"%s\", {force = true})", table.concat(flags.cxxflags, "\", \""))
        end
        if flags.cxflags and #flags.cxflags > 0 then
            file:print("add_cxflags(\"%s\", {force = true})", table.concat(flags.cxflags, "\", \""))
        end
        if flags.asflags and #flags.asflags > 0 then
            file:print("add_asflags(\"%s\", {force = true})", table.concat(flags.asflags, "\", \""))
        end
        if flags.defines and #flags.defines > 0 then
            file:print("add_defines(\"%s\", {force = true})", table.concat(flags.defines, "\", \""))
        end
    end
    file:print(user_code_end_template, "flags")
    file:print("")
end

function _generate_target_configs(file, proj, coder, user)
    file:print("    ----------------------------- target configs ------------------------------")
    for _, warning in ipairs(proj["core"]["warnings"] or {"allextra", "error"}) do
        file:print("    set_warnings(\"%s\")", warning)
    end
    for _, language in ipairs(proj["core"]["languages"] or {"c99", "cxx11"}) do
        file:print("    set_languages(\"%s\")", language)
    end
    file:print("    add_deps(\"%s\")", proj["core"]["hal"])
    file:print("    add_rules(\"asm\")")
    file:print("    add_rules(\"csp.bin\")")
    file:print("    add_rules(\"csp.map\")")
    _generate_user(file, "target configs", user, "    ")
end

function _generate_target_files(file, proj, coder, user)
    file:print("    ------------------------------ target files -------------------------------")
    file:print("    add_files(\"%s\")", "core/src/*.c")
    local data = coder.get_files_and_includes(proj)
    for _, f in ipairs(data.files) do
        file:print("    add_files(\"%s\")", f)
    end
    file:print("    add_includedirs(\"%s\", {public = true})", "core/inc")
    for _, i in ipairs(data.includes) do
        file:print("    add_includedirs(\"%s\", {public = true}))", i)
    end
    _generate_user(file, "target files", user, "    ", true)
end

function _generate_target(file, proj, coder, user)
    file:print("target(\"%s\")", proj["name"])
    file:print("do")
    file:print("    set_kind(\"binary\")")
    _generate_target_configs(file, proj, coder, user)
    _generate_target_files(file, proj, coder, user)
    file:print("end")
    file:print("target_end()")
end

function _match_user(file_path)
    if not os.isfile(file_path) then
        return {}
    end

    local user = {}
    local data = io.readfile(file_path)
    for s in string.gmatch(data, user_code_end_match .. "(.-)\n") do
        local matcher = user_code_begin_match .. s .. "\n(.-)" .. user_code_end_match .. s
        local match = string.match(data, matcher)
        if match and string.len(match) > 0 then
            match = string.rtrim(match, " ") -- we must ignore right whitespace
            if string.len(match) > 0 then
                user[s] = match
            end
        end
    end
    return user
end

function _generate(proj, coder, outputdir)
    local file_path = path.join(outputdir, "xmake.lua")
    local user = _match_user(file_path)
    local file = io.open(file_path, "w")
    _generate_header(file, proj, coder, user)
    _generate_project_rules(file, proj, coder, user)
    _generate_project_config(file, proj, coder, user)
    _generate_includes(file, proj, coder, user)
    _generate_toolchains(file, proj, coder, user)
    _generate_flags(file, proj, coder, user)
    _generate_target(file, proj, coder, user)
    file:close()

    cprint("${color.success}create %s ok!", file_path)
end

function main(proj, coder, outputdir)
    _generate(proj, coder, outputdir)
end
