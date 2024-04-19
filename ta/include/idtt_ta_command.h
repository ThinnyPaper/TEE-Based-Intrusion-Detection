/*
 * IDTT (Intrusion Detection Tool Based on TEE)
 *
 * Copyright (C) 2024 Yang Zhe
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef _IDTT_TA_COMMAND_H
#define _IDTT_TA_COMMAND_H

#define TA_CMD_INITDB 0
#define TA_CMD_STORE 1
#define TA_CMD_CHECK 2

#define TA_CHECK_RESULT_CONSISTENT 0
#define TA_CHECK_RESULT_MODIFIED 1
#define TA_CHECK_RESULT_NO_MATCH_FILE 2

#define CHECK_PERM_INCONSIS (1<<0)
#define CHECK_UID_INCONSIS (1<<1)
#define CHECK_GID_INCONSIS (1<<2)
#define CHECK_ATIME_INCONSIS (1<<3)
#define CHECK_CTIME_INCONSIS (1<<4)
#define CHECK_MTIME_INCONSIS (1<<5)
#define CHECK_INODE_INCONSIS (1<<6)
#define CHECK_NLINK_INCONSIS (1<<7)
#define CHECK_SIZE_INCONSIS (1<<8)
#define CHECK_BCOUNT_INCONSIS (1<<9)
#define CHECK_SHA256_INCONSIS (1<<10)
#define CHECK_WHIRLPOOL_INCONSIS (1<<11)

#endif 