/*
 * Block CTCP UnrealIRCd Module (Channel Mode +C)
 * (C) Copyright 2000-.. Bram Matthys (Syzop) and the UnrealIRCd team
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 1, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "config.h"
#include "struct.h"
#include "common.h"
#include "sys.h"
#include "numeric.h"
#include "msg.h"
#include "proto.h"
#include "channel.h"
#include <time.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef _WIN32
#include <io.h>
#endif
#include <fcntl.h>
#include "h.h"
#ifdef _WIN32
#include "version.h"
#endif

DLLFUNC CMD_FUNC(noctcp);

ModuleHeader MOD_HEADER(noctcp)
  = {
	"chanmodes/noctcp",
	"$Id$",
	"Channel Mode +C",
	"3.2-b8-1",
	NULL 
    };

Cmode_t EXTCMODE_NOCTCP;

#define IsNoCTCP(chptr)    (chptr->mode.extmode & EXTCMODE_NOCTCP)

DLLFUNC char *noctcp_prechanmsg(aClient *sptr, aChannel *chptr, char *text, int notice);

DLLFUNC int MOD_TEST(noctcp)(ModuleInfo *modinfo)
{
	return MOD_SUCCESS;
}

DLLFUNC int MOD_INIT(noctcp)(ModuleInfo *modinfo)
{
CmodeInfo req;

	memset(&req, 0, sizeof(req));
	req.paracount = 0;
	req.flag = 'C';
	req.is_ok = extcmode_default_requirechop;
	CmodeAdd(modinfo->handle, req, &EXTCMODE_NOCTCP);
	
	HookAddPCharEx(modinfo->handle, HOOKTYPE_PRE_CHANMSG, noctcp_prechanmsg);
	
	MARK_AS_OFFICIAL_MODULE(modinfo);
	return MOD_SUCCESS;
}

DLLFUNC int MOD_LOAD(noctcp)(int module_load)
{
	return MOD_SUCCESS;
}

DLLFUNC int MOD_UNLOAD(noctcp)(int module_unload)
{
	return MOD_SUCCESS;
}

static int IsACTCP(char *s)
{
	if (!s)
		return 0;

	if ((*s == '\001') && strncmp(&s[1], "ACTION ", 7))
		return 1;

	return 0;
}

DLLFUNC char *noctcp_prechanmsg(aClient *sptr, aChannel *chptr, char *text, int notice)
{
	if (MyClient(sptr) && IsNoCTCP(chptr) && IsACTCP(text))
	{
		if (!notice)
		{
			sendto_one(sptr, err_str(ERR_CANNOTSENDTOCHAN),
				   me.name, sptr->name, chptr->chname,
				   "CTCPs are not permitted in this channel", chptr->chname);
		}

		return NULL;
	}
	return text;
}
