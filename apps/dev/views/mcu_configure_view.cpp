/*
 * ****************************************************************************
 *  @author      xqyjlj
 *  @file        mcu_configure_view.cpp
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

#include "mcu_configure_view.h"
#include "ui_mcu_configure_view.h"

mcu_configure_view::mcu_configure_view(QWidget *parent) : QWidget(parent), ui(new Ui::mcu_configure_view)
{
    ui->setupUi(this);
}

mcu_configure_view::~mcu_configure_view()
{
    delete ui;
}
