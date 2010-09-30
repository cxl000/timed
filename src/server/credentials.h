/***************************************************************************
**                                                                        **
**   Copyright (C) 2010 Nokia Corporation.                                **
**                                                                        **
**   Author: Simo Piiroinen <simo.piiroinen@nokia.com>                    **
**                                                                        **
**     This file is part of Timed                                         **
**                                                                        **
**     Timed is free software; you can redistribute it and/or modify      **
**     it under the terms of the GNU Lesser General Public License        **
**     version 2.1 as published by the Free Software Foundation.          **
**                                                                        **
**     Timed is distributed in the hope that it will be useful, but       **
**     WITHOUT ANY WARRANTY;  without even the implied warranty  of       **
**     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.               **
**     See the GNU Lesser General Public License  for more details.       **
**                                                                        **
**   You should have received a copy of the GNU  Lesser General Public    **
**   License along with Timed. If not, see http://www.gnu.org/licenses/   **
**                                                                        **
***************************************************************************/

#ifndef CREDENTIALS_H
#define CREDENTIALS_H

# ifndef __cplusplus
#  error This is a C++ only header
# endif

#include <sys/types.h>
#include <sys/creds.h>
#include <QDBusMessage>
#include <QDBusConnection>

uint32_t get_name_owner_from_dbus_sync(const QDBusConnection &bus, const QString &name);

#endif // CREDENTIALS_H
