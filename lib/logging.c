/* -*-  mode:c; tab-width:8; c-basic-offset:8; indent-tabs-mode:nil;  -*- */
/*
   Copyright (C) 2012 by Ronnie Sahlberg <ronniesahlberg@gmail.com>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation; either version 2.1 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program; if not, see <http://www.gnu.org/licenses/>.
*/
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#if defined(WIN32)
#include "win32/win32_compat.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "iscsi.h"
#include "iscsi-private.h"
#include "scsi-lowlevel.h"

static FILE *output;

void iscsi_log_to_stderr(int level, const char *message) {
	char now_str[64];
	time_t curtime;
	time(&curtime);
	ctime_r(&curtime, now_str);
	/* no new line*/
	now_str[strlen(now_str) - 1] = '\0';

	if (output) {
		fprintf(output, "%s libiscsi:%d %s\n", now_str, level, message);
		fflush(output);
	}

	fprintf(stderr, "%s libiscsi:%d %s\n", now_str, level, message);
}

void
iscsi_set_log_fn(struct iscsi_context *iscsi, iscsi_log_fn fn)
{
	iscsi->log_fn = fn;
}

void
iscsi_log_message(struct iscsi_context *iscsi, int level, const char *func, const char *file, int line, const char *format, ...)
{
	va_list ap;
	static char message[1024];
	int ret;

	if (iscsi->log_fn == NULL) {
		return;
	}

	va_start(ap, format);
	ret = vsnprintf(message, 1024, format, ap);
	va_end(ap);

	if (ret < 0) {
		return;
	}

	char message2[1024] = {};
	if (iscsi->target_name[0]) {
		snprintf(message2, 1024, "%s(%s:%d) %s [%s]", func, file, line, message,
				 iscsi->target_name);
	} else {
		snprintf(message2, 1024, "%s(%s:%d) %s", func, file, line, message);
	}

	static int init = 0;
	if (!init) {
		init = 1;
		char *filename = getenv("LIBISCSI_DEBUG_FILE");
		if (filename) output = fopen(filename, "a");
	}

	iscsi->log_fn(level, message2);
}


