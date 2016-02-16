
/* Copyright (c) 2010-2016, Stefan Eilemann <eile@eyescale.ch>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 2.1 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef PRESSION_LOG_H
#define PRESSION_LOG_H

#include <lunchbox/log.h>

namespace pression
{
/** Defines selective logging classes enabled by setting LB_LOG_TOPICS. */
enum LogTopics
{
    LOG_PLUGIN = lunchbox::LOG_CUSTOM << 0, //!< Log plugin handling (16)
    LOG_CUSTOM = lunchbox::LOG_CUSTOM << 1 // 32
};
}
#endif // PRESSION_LOG_H
