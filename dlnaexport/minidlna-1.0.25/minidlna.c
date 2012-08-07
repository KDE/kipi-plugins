/* MiniDLNA project
 *
 * http://sourceforge.net/projects/minidlna/
 *
 * MiniDLNA media server
 * Copyright (C) 2008-2009  Justin Maggard
 *
 * This file is part of MiniDLNA.
 *
 * MiniDLNA is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * MiniDLNA is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with MiniDLNA. If not, see <http://www.gnu.org/licenses/>.
 *
 * Portions of the code from the MiniUPnP project:
 *
 * Copyright (c) 2006-2007, Thomas Bernard
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * The name of the author may not be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include <sys/param.h>
#include <errno.h>
#include <pthread.h>
#include <pwd.h>

#include "config.h"

#ifdef ENABLE_NLS
#include <libintl.h>
#endif

#include "upnpglobalvars.h"
#include "sql.h"
#include "upnphttp.h"
#include "upnpdescgen.h"
#include "minidlnapath.h"
#include "getifaddr.h"
#include "upnpsoap.h"
#include "options.h"
#include "utils.h"
#include "minissdp.h"
#include "minidlnatypes.h"
#include "daemonize.h"
#include "upnpevents.h"
#include "scanner.h"
#include "inotify.h"
#include "log.h"
#ifdef TIVO_SUPPORT
#include "tivo_beacon.h"
#include "tivo_utils.h"
#endif

#if SQLITE_VERSION_NUMBER < 3005001
# warning "Your SQLite3 library appears to be too old!  Please use 3.5.1 or newer."
# define sqlite3_threadsafe() 0
#endif
 
/* OpenAndConfHTTPSocket() :
 * setup the socket used to handle incoming HTTP connections. */
static int
OpenAndConfHTTPSocket(unsigned short port)
{
	int s;
	int i = 1;
	struct sockaddr_in listenname;

	/* Initialize client type cache */
	memset(&clients, 0, sizeof(struct client_cache_s));

	if( (s = socket(PF_INET, SOCK_STREAM, 0)) < 0)
	{
		DPRINTF(E_ERROR, L_GENERAL, "socket(http): %s\n", strerror(errno));
		return -1;
	}

	if(setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &i, sizeof(i)) < 0)
	{
		DPRINTF(E_WARN, L_GENERAL, "setsockopt(http, SO_REUSEADDR): %s\n", strerror(errno));
	}

	memset(&listenname, 0, sizeof(struct sockaddr_in));
	listenname.sin_family = AF_INET;
	listenname.sin_port = htons(port);
	listenname.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(s, (struct sockaddr *)&listenname, sizeof(struct sockaddr_in)) < 0)
	{
		DPRINTF(E_ERROR, L_GENERAL, "bind(http): %s\n", strerror(errno));
		close(s);
		return -1;
	}

	if(listen(s, 6) < 0)
	{
		DPRINTF(E_ERROR, L_GENERAL, "listen(http): %s\n", strerror(errno));
		close(s);
		return -1;
	}

	return s;
}

/* Handler for the SIGTERM signal (kill) 
 * SIGINT is also handled */
static void
sigterm(int sig)
{
	/*int save_errno = errno;*/
	signal(sig, SIG_IGN);	/* Ignore this signal while we are quitting */

	DPRINTF(E_WARN, L_GENERAL, "received signal %d, good-bye\n", sig);

	quitting = 1;
	/*errno = save_errno;*/
}

/* record the startup time, for returning uptime */
static void
set_startup_time(void)
{
	startup_time = time(NULL);
}

/* parselanaddr()
 * parse address with mask
 * ex: 192.168.1.1/24
 * return value : 
 *    0 : ok
 *   -1 : error */
static int
parselanaddr(struct lan_addr_s * lan_addr, const char * str)
{
	const char * p;
	int nbits = 24;
	int n;
	p = str;
	while(*p && *p != '/' && !isspace(*p))
		p++;
	n = p - str;
	if(*p == '/')
	{
		nbits = atoi(++p);
		while(*p && !isspace(*p))
			p++;
	}
	if(n>15)
	{
		DPRINTF(E_OFF, L_GENERAL, "Error parsing address/mask: %s\n", str);
		return -1;
	}
	memcpy(lan_addr->str, str, n);
	lan_addr->str[n] = '\0';
	if(!inet_aton(lan_addr->str, &lan_addr->addr))
	{
		DPRINTF(E_OFF, L_GENERAL, "Error parsing address: %s\n", str);
		return -1;
	}
	lan_addr->mask.s_addr = htonl(nbits ? (0xffffffff << (32 - nbits)) : 0);
	return 0;
}

static void
getfriendlyname(char * buf, int len)
{
	char * dot = NULL;
	char * hn = calloc(1, 256);
	int off;

	if( gethostname(hn, 256) == 0 )
	{
		strncpyt(buf, hn, len);
		dot = strchr(buf, '.');
		if( dot )
			*dot = '\0';
	}
	else
	{
		strcpy(buf, "Unknown");
	}
	free(hn);

	off = strlen(buf);
	off += snprintf(buf+off, len-off, ": ");
#ifdef READYNAS
	FILE * info;
	char ibuf[64], *key, *val;
	snprintf(buf+off, len-off, "ReadyNAS");
	info = fopen("/proc/sys/dev/boot/info", "r");
	if( !info )
		return;
	while( (val = fgets(ibuf, 64, info)) != NULL )
	{
		key = strsep(&val, ": \t");
		val = trim(val);
		if( strcmp(key, "model") == 0 )
		{
			snprintf(buf+off, len-off, "%s", val);
			key = strchr(val, ' ');
			if( key )
			{
				strncpyt(modelnumber, key+1, MODELNUMBER_MAX_LEN);
				*key = '\0';
			}
			snprintf(modelname, MODELNAME_MAX_LEN,
				"Windows Media Connect compatible (%s)", val);
		}
		else if( strcmp(key, "serial") == 0 )
		{
			strncpyt(serialnumber, val, SERIALNUMBER_MAX_LEN);
			if( serialnumber[0] == '\0' )
			{
				char mac_str[13];
				if( getsyshwaddr(mac_str, sizeof(mac_str)) == 0 )
					strcpy(serialnumber, mac_str);
				else
					strcpy(serialnumber, "0");
			}
			break;
		}
	}
	fclose(info);
#if PNPX
	memcpy(pnpx_hwid+4, "01F2", 4);
	if( strcmp(modelnumber, "NVX") == 0 )
		memcpy(pnpx_hwid+17, "0101", 4);
	else if( strcmp(modelnumber, "Pro") == 0 ||
	         strcmp(modelnumber, "Pro 6") == 0 ||
	         strncmp(modelnumber, "Ultra 6", 7) == 0 )
		memcpy(pnpx_hwid+17, "0102", 4);
	else if( strcmp(modelnumber, "Pro 2") == 0 ||
	         strncmp(modelnumber, "Ultra 2", 7) == 0 )
		memcpy(pnpx_hwid+17, "0103", 4);
	else if( strcmp(modelnumber, "Pro 4") == 0 ||
	         strncmp(modelnumber, "Ultra 4", 7) == 0 )
		memcpy(pnpx_hwid+17, "0104", 4);
	else if( strcmp(modelnumber+1, "100") == 0 )
		memcpy(pnpx_hwid+17, "0105", 4);
	else if( strcmp(modelnumber+1, "200") == 0 )
		memcpy(pnpx_hwid+17, "0106", 4);
	/* 0107 = Stora */
	else if( strcmp(modelnumber, "Duo v2") == 0 )
		memcpy(pnpx_hwid+17, "0108", 4);
	else if( strcmp(modelnumber, "NV+ v2") == 0 )
		memcpy(pnpx_hwid+17, "0109", 4);
#endif
#else
	char * logname;
	logname = getenv("LOGNAME");
#ifndef STATIC // Disable for static linking
	if( !logname )
	{
		struct passwd * pwent;
		pwent = getpwuid(getuid());
		if( pwent )
			logname = pwent->pw_name;
	}
#endif
	snprintf(buf+off, len-off, "%s", logname?logname:"Unknown");
#endif
}

static int
open_db(void)
{
	char path[PATH_MAX];
	int new_db = 0;

	snprintf(path, sizeof(path), "%s/files.db", db_path);
	if( access(path, F_OK) != 0 )
	{
		new_db = 1;
		make_dir(db_path, S_ISVTX|S_IRWXU|S_IRWXG|S_IRWXO);
	}
	if( sqlite3_open(path, &db) != SQLITE_OK )
	{
		DPRINTF(E_FATAL, L_GENERAL, "ERROR: Failed to open sqlite database!  Exiting...\n");
	}
	sqlite3_busy_timeout(db, 5000);
	sql_exec(db, "pragma page_size = 4096");
	sql_exec(db, "pragma journal_mode = OFF");
	sql_exec(db, "pragma synchronous = OFF;");
	sql_exec(db, "pragma default_cache_size = 8192;");
	return new_db;
}

/* init phase :
 * 1) read configuration file
 * 2) read command line arguments
 * 3) daemonize
 * 4) check and write pid file
 * 5) set startup time stamp
 * 6) compute presentation URL
 * 7) set signal handlers */
static int
init(int argc, char * * argv)
{
	int i;
	int pid;
	int debug_flag = 0;
	int verbose_flag = 0;
	int options_flag = 0;
	struct sigaction sa;
	const char * presurl = NULL;
	const char * optionsfile = "/etc/minidlna.conf";
	char mac_str[13];
	char * string, * word;
	enum media_types type;
	char * path;
	char buf[PATH_MAX];
	char ip_addr[INET_ADDRSTRLEN + 3] = {'\0'};
	char log_str[72] = "general,artwork,database,inotify,scanner,metadata,http,ssdp,tivo=warn";
	char *log_level = NULL;

	/* first check if "-f" option is used */
	for(i=2; i<argc; i++)
	{
		if(0 == strcmp(argv[i-1], "-f"))
		{
			optionsfile = argv[i];
			options_flag = 1;
			break;
		}
	}

	/* set up uuid based on mac address */
	if( getsyshwaddr(mac_str, sizeof(mac_str)) < 0 )
	{
		DPRINTF(E_OFF, L_GENERAL, "No MAC address found.  Falling back to generic UUID.\n");
		strcpy(mac_str, "554e4b4e4f57");
	}
	strcpy(uuidvalue+5, "4d696e69-444c-164e-9d41-");
	strncat(uuidvalue, mac_str, 12);

	getfriendlyname(friendly_name, FRIENDLYNAME_MAX_LEN);
	
	runtime_vars.port = -1;
	runtime_vars.notify_interval = 895;	/* seconds between SSDP announces */
	runtime_vars.root_container = NULL;

	/* read options file first since
	 * command line arguments have final say */
	if(readoptionsfile(optionsfile) < 0)
	{
		/* only error if file exists or using -f */
		if(access(optionsfile, F_OK) == 0 || options_flag)
			DPRINTF(E_ERROR, L_GENERAL, "Error reading configuration file %s\n", optionsfile);
	}
	else
	{
		for(i=0; i<num_options; i++)
		{
			switch(ary_options[i].id)
			{
			case UPNPIFNAME:
				for( string = ary_options[i].value; (word = strtok(string, ",")); string = NULL )
				{
					if(n_lan_addr < MAX_LAN_ADDR)
					{
						if(getifaddr(word, ip_addr, sizeof(ip_addr)) >= 0)
						{
							if( *ip_addr && parselanaddr(&lan_addr[n_lan_addr], ip_addr) == 0 )
								if(n_lan_addr < MAX_LAN_ADDR)
									n_lan_addr++;
						}
					}
					else
					{
						DPRINTF(E_ERROR, L_GENERAL, "Too many listening ips (max: %d), ignoring %s\n",
				    		    MAX_LAN_ADDR, word);
					}
				}
				break;
			case UPNPLISTENING_IP:
				if(n_lan_addr < MAX_LAN_ADDR)
				{
					if(parselanaddr(&lan_addr[n_lan_addr],
					             ary_options[i].value) == 0)
						n_lan_addr++;
				}
				else
				{
					DPRINTF(E_ERROR, L_GENERAL, "Too many listening ips (max: %d), ignoring %s\n",
			    		    MAX_LAN_ADDR, ary_options[i].value);
				}
				break;
			case UPNPPORT:
				runtime_vars.port = atoi(ary_options[i].value);
				break;
			case UPNPPRESENTATIONURL:
				presurl = ary_options[i].value;
				break;
			case UPNPNOTIFY_INTERVAL:
				runtime_vars.notify_interval = atoi(ary_options[i].value);
				break;
			case UPNPSERIAL:
				strncpyt(serialnumber, ary_options[i].value, SERIALNUMBER_MAX_LEN);
				break;				
			case UPNPMODEL_NAME:
				strncpyt(modelname, ary_options[i].value, MODELNAME_MAX_LEN);
				break;
			case UPNPMODEL_NUMBER:
				strncpyt(modelnumber, ary_options[i].value, MODELNUMBER_MAX_LEN);
				break;
			case UPNPFRIENDLYNAME:
				strncpyt(friendly_name, ary_options[i].value, FRIENDLYNAME_MAX_LEN);
				break;
			case UPNPMEDIADIR:
				type = ALL_MEDIA;
				char * myval = NULL;
				switch( ary_options[i].value[0] )
				{
				case 'A':
				case 'a':
					if( ary_options[i].value[0] == 'A' || ary_options[i].value[0] == 'a' )
						type = AUDIO_ONLY;
				case 'V':
				case 'v':
					if( ary_options[i].value[0] == 'V' || ary_options[i].value[0] == 'v' )
						type = VIDEO_ONLY;
				case 'P':
				case 'p':
					if( ary_options[i].value[0] == 'P' || ary_options[i].value[0] == 'p' )
						type = IMAGES_ONLY;
					myval = index(ary_options[i].value, '/');
				case '/':
					path = realpath(myval ? myval:ary_options[i].value, buf);
					if( !path )
						path = (myval ? myval:ary_options[i].value);
					if( access(path, F_OK) != 0 )
					{
						DPRINTF(E_ERROR, L_GENERAL, "Media directory \"%s\" not accessible! [%s]\n",
						        path, strerror(errno));
						break;
					}
					struct media_dir_s * this_dir = calloc(1, sizeof(struct media_dir_s));
					this_dir->path = strdup(path);
					this_dir->type = type;
					if( !media_dirs )
					{
						media_dirs = this_dir;
					}
					else
					{
						struct media_dir_s * all_dirs = media_dirs;
						while( all_dirs->next )
							all_dirs = all_dirs->next;
						all_dirs->next = this_dir;
					}
					break;
				default:
					DPRINTF(E_ERROR, L_GENERAL, "Media directory entry not understood! [%s]\n",
					        ary_options[i].value);
					break;
				}
				break;
			case UPNPALBUMART_NAMES:
				for( string = ary_options[i].value; (word = strtok(string, "/")); string = NULL )
				{
					struct album_art_name_s * this_name = calloc(1, sizeof(struct album_art_name_s));
					int len = strlen(word);
					if( word[len-1] == '*' )
					{
						word[len-1] = '\0';
						this_name->wildcard = 1;
					}
					this_name->name = strdup(word);
					if( !album_art_names )
					{
						album_art_names = this_name;
					}
					else
					{
						struct album_art_name_s * all_names = album_art_names;
						while( all_names->next )
							all_names = all_names->next;
						all_names->next = this_name;
					}
				}
				break;
			case UPNPDBDIR:
				path = realpath(ary_options[i].value, buf);
				if( !path )
					path = (ary_options[i].value);
				make_dir(path, S_ISVTX|S_IRWXU|S_IRWXG|S_IRWXO);
				if( access(path, F_OK) != 0 )
				{
					DPRINTF(E_FATAL, L_GENERAL, "Database path not accessible! [%s]\n", path);
					break;
				}
				strncpyt(db_path, path, PATH_MAX);
				break;
			case UPNPLOGDIR:
				path = realpath(ary_options[i].value, buf);
				if( !path )
					path = (ary_options[i].value);
				make_dir(path, S_ISVTX|S_IRWXU|S_IRWXG|S_IRWXO);
				if( access(path, F_OK) != 0 )
				{
					DPRINTF(E_FATAL, L_GENERAL, "Log path not accessible! [%s]\n", path);
					break;
				}
				strncpyt(log_path, path, PATH_MAX);
				break;
			case UPNPLOGLEVEL:
				log_level = ary_options[i].value;
				break;
			case UPNPINOTIFY:
				if( (strcmp(ary_options[i].value, "yes") != 0) && !atoi(ary_options[i].value) )
					CLEARFLAG(INOTIFY_MASK);
				break;
			case ENABLE_TIVO:
				if( (strcmp(ary_options[i].value, "yes") == 0) || atoi(ary_options[i].value) )
					SETFLAG(TIVO_MASK);
				break;
			case ENABLE_DLNA_STRICT:
				if( (strcmp(ary_options[i].value, "yes") == 0) || atoi(ary_options[i].value) )
					SETFLAG(DLNA_STRICT_MASK);
				break;
			case ROOT_CONTAINER:
				switch( ary_options[i].value[0] )
				{
				case '.':
					runtime_vars.root_container = NULL;
					break;
				case 'B':
				case 'b':
					runtime_vars.root_container = BROWSEDIR_ID;
					break;
				case 'M':
				case 'm':
					runtime_vars.root_container = MUSIC_ID;
					break;
				case 'V':
				case 'v':
					runtime_vars.root_container = VIDEO_ID;
					break;
				case 'P':
				case 'p':
					runtime_vars.root_container = IMAGE_ID;
					break;
				default:
					DPRINTF(E_ERROR, L_GENERAL, "Invalid root container! [%s]\n",
						ary_options[i].value);
					break;
				}
				break;
			case UPNPMINISSDPDSOCKET:
				minissdpdsocketpath = ary_options[i].value;
				break;
			default:
				DPRINTF(E_DEBUG, L_GENERAL, "Unknown option in file %s\n",
				        optionsfile);
			}
		}
	}
	if( log_path[0] == '\0' )
	{
		if( db_path[0] == '\0' )
			strncpyt(log_path, DEFAULT_LOG_PATH, PATH_MAX);
		else
			strncpyt(log_path, db_path, PATH_MAX);
	}
	if( db_path[0] == '\0' )
		strncpyt(db_path, DEFAULT_DB_PATH, PATH_MAX);

	/* command line arguments processing */
	for(i=1; i<argc; i++)
	{
		if(argv[i][0]!='-')
		{
			DPRINTF(E_ERROR, L_GENERAL, "Unknown option: %s\n", argv[i]);
		}
		else if(strcmp(argv[i], "--help")==0)
		{
			runtime_vars.port = 0;
			break;
		}
		else switch(argv[i][1])
		{
		case 't':
			if(i+1 < argc)
				runtime_vars.notify_interval = atoi(argv[++i]);
			else
				DPRINTF(E_ERROR, L_GENERAL, "Option -%c takes one argument.\n", argv[i][1]);
			break;
		case 's':
			if(i+1 < argc)
				strncpyt(serialnumber, argv[++i], SERIALNUMBER_MAX_LEN);
			else
				DPRINTF(E_ERROR, L_GENERAL, "Option -%c takes one argument.\n", argv[i][1]);
			break;
		case 'm':
			if(i+1 < argc)
				strncpyt(modelnumber, argv[++i], MODELNUMBER_MAX_LEN);
			else
				DPRINTF(E_ERROR, L_GENERAL, "Option -%c takes one argument.\n", argv[i][1]);
			break;
		case 'p':
			if(i+1 < argc)
				runtime_vars.port = atoi(argv[++i]);
			else
				DPRINTF(E_ERROR, L_GENERAL, "Option -%c takes one argument.\n", argv[i][1]);
			break;
		case 'P':
			if(i+1 < argc)
				pidfilename = argv[++i];
			else
				DPRINTF(E_ERROR, L_GENERAL, "Option -%c takes one argument.\n", argv[i][1]);
			break;
		case 'd':
			debug_flag = 1;
		case 'v':
			verbose_flag = 1;
			break;
		case 'L':
			SETFLAG(NO_PLAYLIST_MASK);
			break;
		case 'w':
			if(i+1 < argc)
				presurl = argv[++i];
			else
				DPRINTF(E_ERROR, L_GENERAL, "Option -%c takes one argument.\n", argv[i][1]);
			break;
		case 'a':
			if(i+1 < argc)
			{
				int address_already_there = 0;
				int j;
				i++;
				for(j=0; j<n_lan_addr; j++)
				{
					struct lan_addr_s tmpaddr;
					parselanaddr(&tmpaddr, argv[i]);
					if(0 == strcmp(lan_addr[j].str, tmpaddr.str))
						address_already_there = 1;
				}
				if(address_already_there)
					break;
				if(n_lan_addr < MAX_LAN_ADDR)
				{
					if(parselanaddr(&lan_addr[n_lan_addr], argv[i]) == 0)
						n_lan_addr++;
				}
				else
				{
					DPRINTF(E_ERROR, L_GENERAL, "Too many listening ips (max: %d), ignoring %s\n",
				    	    MAX_LAN_ADDR, argv[i]);
				}
			}
			else
				DPRINTF(E_ERROR, L_GENERAL, "Option -%c takes one argument.\n", argv[i][1]);
			break;
		case 'i':
			if(i+1 < argc)
			{
				int address_already_there = 0;
				int j;
				i++;
				if( getifaddr(argv[i], ip_addr, sizeof(ip_addr)) < 0 )
				{
					DPRINTF(E_FATAL, L_GENERAL, "Required network interface '%s' not found.\n",
						argv[i]);
				}
				for(j=0; j<n_lan_addr; j++)
				{
					struct lan_addr_s tmpaddr;
					parselanaddr(&tmpaddr, ip_addr);
					if(0 == strcmp(lan_addr[j].str, tmpaddr.str))
						address_already_there = 1;
				}
				if(address_already_there)
					break;
				if(n_lan_addr < MAX_LAN_ADDR)
				{
					if(parselanaddr(&lan_addr[n_lan_addr], ip_addr) == 0)
						n_lan_addr++;
				}
				else
				{
					DPRINTF(E_ERROR, L_GENERAL, "Too many listening ips (max: %d), ignoring %s\n",
				    	    MAX_LAN_ADDR, argv[i]);
				}
			}
			else
				DPRINTF(E_ERROR, L_GENERAL, "Option -%c takes one argument.\n", argv[i][1]);
			break;
		case 'f':
			i++;	/* discarding, the config file is already read */
			break;
		case 'h':
			runtime_vars.port = 0; // triggers help display
			break;
		case 'R':
			snprintf(buf, sizeof(buf), "rm -rf %s/files.db %s/art_cache", db_path, db_path);
			if( system(buf) != 0 )
				DPRINTF(E_WARN, L_GENERAL, "Failed to clean old file cache.\n");
			break;
		case 'V':
			printf("Version " MINIDLNA_VERSION "\n");
			exit(0);
			break;
		default:
			DPRINTF(E_ERROR, L_GENERAL, "Unknown option: %s\n", argv[i]);
		}
	}
	/* If no IP was specified, try to detect one */
	if( n_lan_addr < 1 )
	{
		if( getsysaddrs() <= 0 )
		{
			DPRINTF(E_FATAL, L_GENERAL, "No IP address automatically detected!\n");
		}
	}

	if( (n_lan_addr==0) || (runtime_vars.port<=0) )
	{
		DPRINTF(E_ERROR, L_GENERAL, "Usage:\n\t"
		        "%s [-d] [-v] [-f config_file]\n"
			"\t\t[-a listening_ip] [-p port]\n"
			/*"[-l logfile] " not functionnal */
			"\t\t[-s serial] [-m model_number] \n"
			"\t\t[-t notify_interval] [-P pid_filename]\n"
			"\t\t[-w url] [-R] [-V] [-h]\n"
		        "\nNotes:\n\tNotify interval is in seconds. Default is 895 seconds.\n"
			"\tDefault pid file is %s.\n"
			"\tWith -d minidlna will run in debug mode (not daemonize).\n"
			"\t-w sets the presentation url. Default is http address on port 80\n"
			"\t-h displays this text\n"
			"\t-R forces a full rescan\n"
			"\t-L do note create playlists\n"
			"\t-V print the version number\n",
		        argv[0], pidfilename);
		return 1;
	}

	if( verbose_flag )
	{
		strcpy(log_str+65, "debug");
		log_level = log_str;
	}
	else if( !log_level )
	{
		log_level = log_str;
	}
	if(debug_flag)
	{
		pid = getpid();
		log_init(NULL, log_level);
	}
	else
	{
		pid = daemonize();
		#ifdef READYNAS
		log_init("/var/log/upnp-av.log", log_level);
		#else
		if( access(db_path, F_OK) != 0 )
			make_dir(db_path, S_ISVTX|S_IRWXU|S_IRWXG|S_IRWXO);
		sprintf(buf, "%s/minidlna.log", log_path);
		log_init(buf, log_level);
		#endif
	}

	if(checkforrunning(pidfilename) < 0)
	{
		DPRINTF(E_ERROR, L_GENERAL, "MiniDLNA is already running. EXITING.\n");
		return 1;
	}	

	set_startup_time();

	/* presentation url */
	if(presurl)
		strncpyt(presentationurl, presurl, PRESENTATIONURL_MAX_LEN);
	else
		strcpy(presentationurl, "/");

	/* set signal handler */
	memset(&sa, 0, sizeof(struct sigaction));
	sa.sa_handler = sigterm;
	if (sigaction(SIGTERM, &sa, NULL))
	{
		DPRINTF(E_FATAL, L_GENERAL, "Failed to set SIGTERM handler. EXITING.\n");
	}
	if (sigaction(SIGINT, &sa, NULL))
	{
		DPRINTF(E_FATAL, L_GENERAL, "Failed to set SIGINT handler. EXITING.\n");
	}

	if(signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
		DPRINTF(E_FATAL, L_GENERAL, "Failed to ignore SIGPIPE signals. EXITING.\n");
	}

	writepidfile(pidfilename, pid);

	return 0;
}

/* === main === */
/* process HTTP or SSDP requests */
int
main(int argc, char * * argv)
{
	int i;
	int sudp = -1, shttpl = -1;
	int snotify[MAX_LAN_ADDR];
	LIST_HEAD(httplisthead, upnphttp) upnphttphead;
	struct upnphttp * e = 0;
	struct upnphttp * next;
	fd_set readset;	/* for select() */
	fd_set writeset;
	struct timeval timeout, timeofday, lastnotifytime = {0, 0};
	time_t lastupdatetime = 0;
	int max_fd = -1;
	int last_changecnt = 0;
	pid_t scanner_pid = 0;
	pthread_t inotify_thread = 0;
	struct media_dir_s *media_path, *last_path;
	struct album_art_name_s *art_names, *last_name;
#ifdef TIVO_SUPPORT
	unsigned short int beacon_interval = 5;
	int sbeacon = -1;
	struct sockaddr_in tivo_bcast;
	struct timeval lastbeacontime = {0, 0};
#endif

	for (i = 0; i < L_MAX; i++)
		log_level[i] = E_WARN;
#ifdef ENABLE_NLS
	setlocale(LC_MESSAGES, "");
	setlocale(LC_CTYPE, "en_US.utf8");
	DPRINTF(E_DEBUG, L_GENERAL, "Using locale dir %s\n", bindtextdomain("minidlna", getenv("TEXTDOMAINDIR")));
	textdomain("minidlna");
#endif

	if (init(argc, argv) != 0)
		return 1;

#ifdef READYNAS
	DPRINTF(E_WARN, L_GENERAL, "Starting " SERVER_NAME " version " MINIDLNA_VERSION ".\n");
	unlink("/ramfs/.upnp-av_scan");
#else
	DPRINTF(E_WARN, L_GENERAL, "Starting " SERVER_NAME " version " MINIDLNA_VERSION " [SQLite %s].\n", sqlite3_libversion());
	if( !sqlite3_threadsafe() )
	{
		DPRINTF(E_ERROR, L_GENERAL, "SQLite library is not threadsafe!  "
		                            "Scanning must be finished before file serving can begin, "
		                            "and inotify will be disabled.\n");
	}
	if( sqlite3_libversion_number() < 3005001 )
	{
		DPRINTF(E_WARN, L_GENERAL, "SQLite library is old.  Please use version 3.5.1 or newer.\n");
	}
#endif
	LIST_INIT(&upnphttphead);

	if( open_db() == 0 )
	{
		updateID = sql_get_int_field(db, "SELECT UPDATE_ID from SETTINGS");
	}
	i = db_upgrade(db);
	if( i != 0 )
	{
		if( i < 0 )
		{
			DPRINTF(E_WARN, L_GENERAL, "Creating new database...\n");
		}
		else
		{
			DPRINTF(E_WARN, L_GENERAL, "Database version mismatch; need to recreate...\n");
		}
		sqlite3_close(db);
		char *cmd;
		i = asprintf(&cmd, "rm -rf %s/files.db %s/art_cache", db_path, db_path);
		if( i > 0 )
			i = system(cmd);
		else
			cmd = NULL;
		if( i != 0 )
		{
			DPRINTF(E_WARN, L_GENERAL, "Failed to clean old file cache.\n");
		}
		free(cmd);
		open_db();
		if( CreateDatabase() != 0 )
		{
			DPRINTF(E_FATAL, L_GENERAL, "ERROR: Failed to create sqlite database!  Exiting...\n");
		}
#if USE_FORK
		scanning = 1;
		sqlite3_close(db);
		scanner_pid = fork();
		open_db();
		if( !scanner_pid ) // child (scanner) process
		{
			start_scanner();
			sqlite3_close(db);
			media_path = media_dirs;
			art_names = album_art_names;
			while( media_path )
			{
				free(media_path->path);
				last_path = media_path;
				media_path = media_path->next;
				free(last_path);
			}
			while( art_names )
			{
				free(art_names->name);
				last_name = art_names;
				art_names = art_names->next;
				free(last_name);
			}
			freeoptions();
			exit(EXIT_SUCCESS);
		}
#else
		start_scanner();
#endif
	}
	signal(SIGCHLD, SIG_IGN);
	if( sqlite3_threadsafe() && sqlite3_libversion_number() >= 3005001 &&
	    GETFLAG(INOTIFY_MASK) && pthread_create(&inotify_thread, NULL, start_inotify, NULL) )
	{
		DPRINTF(E_FATAL, L_GENERAL, "ERROR: pthread_create() failed for start_inotify.\n");
	}

	for( i = 0; i < n_lan_addr; i++ )
	{
		DPRINTF(E_INFO, L_GENERAL, "Enabled interface %s/%s\n",
		        lan_addr[i].str, inet_ntoa(lan_addr[i].mask));
	}

	sudp = OpenAndConfSSDPReceiveSocket(n_lan_addr, lan_addr);
	if(sudp < 0)
	{
		DPRINTF(E_INFO, L_GENERAL, "Failed to open socket for receiving SSDP. Trying to use MiniSSDPd\n");
		if(SubmitServicesToMiniSSDPD(lan_addr[0].str, runtime_vars.port) < 0) {
			DPRINTF(E_FATAL, L_GENERAL, "Failed to connect to MiniSSDPd. EXITING");
			return 1;
		}
	}
	/* open socket for HTTP connections. Listen on the 1st LAN address */
	shttpl = OpenAndConfHTTPSocket(runtime_vars.port);
	if(shttpl < 0)
	{
		DPRINTF(E_FATAL, L_GENERAL, "Failed to open socket for HTTP. EXITING\n");
	}
	DPRINTF(E_WARN, L_GENERAL, "HTTP listening on port %d\n", runtime_vars.port);

	/* open socket for sending notifications */
	if(OpenAndConfSSDPNotifySockets(snotify) < 0)
	{
		DPRINTF(E_FATAL, L_GENERAL, "Failed to open sockets for sending SSDP notify "
	                "messages. EXITING\n");
	}

#ifdef TIVO_SUPPORT
	if( GETFLAG(TIVO_MASK) )
	{
		DPRINTF(E_WARN, L_GENERAL, "TiVo support is enabled.\n");
		/* Add TiVo-specific randomize function to sqlite */
		if( sqlite3_create_function(db, "tivorandom", 1, SQLITE_UTF8, NULL, &TiVoRandomSeedFunc, NULL, NULL) != SQLITE_OK )
		{
			DPRINTF(E_ERROR, L_TIVO, "ERROR: Failed to add sqlite randomize function for TiVo!\n");
		}
		/* open socket for sending Tivo notifications */
		sbeacon = OpenAndConfTivoBeaconSocket();
		if(sbeacon < 0)
		{
			DPRINTF(E_FATAL, L_GENERAL, "Failed to open sockets for sending Tivo beacon notify "
		                "messages. EXITING\n");
		}
		tivo_bcast.sin_family = AF_INET;
		tivo_bcast.sin_addr.s_addr = htonl(getBcastAddress());
		tivo_bcast.sin_port = htons(2190);
	}
	else
	{
		sbeacon = -1;
	}
#endif

	SendSSDPGoodbye(snotify, n_lan_addr);

	/* main loop */
	while(!quitting)
	{
		/* Check if we need to send SSDP NOTIFY messages and do it if
		 * needed */
		if(gettimeofday(&timeofday, 0) < 0)
		{
			DPRINTF(E_ERROR, L_GENERAL, "gettimeofday(): %s\n", strerror(errno));
			timeout.tv_sec = runtime_vars.notify_interval;
			timeout.tv_usec = 0;
		}
		else
		{
			/* the comparaison is not very precise but who cares ? */
			if(timeofday.tv_sec >= (lastnotifytime.tv_sec + runtime_vars.notify_interval))
			{
				SendSSDPNotifies2(snotify,
			                  (unsigned short)runtime_vars.port,
			                  (runtime_vars.notify_interval << 1)+10);
				memcpy(&lastnotifytime, &timeofday, sizeof(struct timeval));
				timeout.tv_sec = runtime_vars.notify_interval;
				timeout.tv_usec = 0;
			}
			else
			{
				timeout.tv_sec = lastnotifytime.tv_sec + runtime_vars.notify_interval
				                 - timeofday.tv_sec;
				if(timeofday.tv_usec > lastnotifytime.tv_usec)
				{
					timeout.tv_usec = 1000000 + lastnotifytime.tv_usec
					                  - timeofday.tv_usec;
					timeout.tv_sec--;
				}
				else
				{
					timeout.tv_usec = lastnotifytime.tv_usec - timeofday.tv_usec;
				}
			}
#ifdef TIVO_SUPPORT
			if( GETFLAG(TIVO_MASK) )
			{
				if(timeofday.tv_sec >= (lastbeacontime.tv_sec + beacon_interval))
				{
   					sendBeaconMessage(sbeacon, &tivo_bcast, sizeof(struct sockaddr_in), 1);
					memcpy(&lastbeacontime, &timeofday, sizeof(struct timeval));
					if( timeout.tv_sec > beacon_interval )
					{
						timeout.tv_sec = beacon_interval;
						timeout.tv_usec = 0;
					}
					/* Beacons should be sent every 5 seconds or so for the first minute,
					 * then every minute or so thereafter. */
					if( beacon_interval == 5 && (timeofday.tv_sec - startup_time) > 60 )
					{
						beacon_interval = 60;
					}
				}
				else if( timeout.tv_sec > (lastbeacontime.tv_sec + beacon_interval + 1 - timeofday.tv_sec) )
				{
					timeout.tv_sec = lastbeacontime.tv_sec + beacon_interval - timeofday.tv_sec;
				}
			}
#endif
		}

		if( scanning )
		{
			if( !scanner_pid || kill(scanner_pid, 0) )
			{
				scanning = 0;
				updateID++;
			}
		}

		/* select open sockets (SSDP, HTTP listen, and all HTTP soap sockets) */
		FD_ZERO(&readset);

		if (sudp >= 0) 
		{
			FD_SET(sudp, &readset);
			max_fd = MAX(max_fd, sudp);
		}
		
		if (shttpl >= 0) 
		{
			FD_SET(shttpl, &readset);
			max_fd = MAX(max_fd, shttpl);
		}
#ifdef TIVO_SUPPORT
		if (sbeacon >= 0) 
		{
			FD_SET(sbeacon, &readset);
			max_fd = MAX(max_fd, sbeacon);
		}
#endif
		i = 0;	/* active HTTP connections count */
		for(e = upnphttphead.lh_first; e != NULL; e = e->entries.le_next)
		{
			if((e->socket >= 0) && (e->state <= 2))
			{
				FD_SET(e->socket, &readset);
				max_fd = MAX(max_fd, e->socket);
				i++;
			}
		}
#ifdef DEBUG
		/* for debug */
		if(i > 1)
		{
			DPRINTF(E_DEBUG, L_GENERAL, "%d active incoming HTTP connections\n", i);
		}
#endif
		FD_ZERO(&writeset);
		upnpevents_selectfds(&readset, &writeset, &max_fd);

		if(select(max_fd+1, &readset, &writeset, 0, &timeout) < 0)
		{
			if(quitting) goto shutdown;
			DPRINTF(E_ERROR, L_GENERAL, "select(all): %s\n", strerror(errno));
			DPRINTF(E_FATAL, L_GENERAL, "Failed to select open sockets. EXITING\n");
		}
		upnpevents_processfds(&readset, &writeset);
		/* process SSDP packets */
		if(sudp >= 0 && FD_ISSET(sudp, &readset))
		{
			/*DPRINTF(E_DEBUG, L_GENERAL, "Received UDP Packet\n");*/
			ProcessSSDPRequest(sudp, (unsigned short)runtime_vars.port);
		}
#ifdef TIVO_SUPPORT
		if(sbeacon >= 0 && FD_ISSET(sbeacon, &readset))
		{
			/*DPRINTF(E_DEBUG, L_GENERAL, "Received UDP Packet\n");*/
			ProcessTiVoBeacon(sbeacon);
		}
#endif
		/* increment SystemUpdateID if the content database has changed,
		 * and if there is an active HTTP connection, at most once every 2 seconds */
		if( i && (timeofday.tv_sec >= (lastupdatetime + 2)) )
		{
			if( scanning || sqlite3_total_changes(db) != last_changecnt )
			{
				updateID++;
				last_changecnt = sqlite3_total_changes(db);
				upnp_event_var_change_notify(EContentDirectory);
				lastupdatetime = timeofday.tv_sec;
			}
		}
		/* process active HTTP connections */
		for(e = upnphttphead.lh_first; e != NULL; e = e->entries.le_next)
		{
			if(  (e->socket >= 0) && (e->state <= 2)
				&&(FD_ISSET(e->socket, &readset)) )
			{
				Process_upnphttp(e);
			}
		}
		/* process incoming HTTP connections */
		if(shttpl >= 0 && FD_ISSET(shttpl, &readset))
		{
			int shttp;
			socklen_t clientnamelen;
			struct sockaddr_in clientname;
			clientnamelen = sizeof(struct sockaddr_in);
			shttp = accept(shttpl, (struct sockaddr *)&clientname, &clientnamelen);
			if(shttp<0)
			{
				DPRINTF(E_ERROR, L_GENERAL, "accept(http): %s\n", strerror(errno));
			}
			else
			{
				struct upnphttp * tmp = 0;
				DPRINTF(E_DEBUG, L_GENERAL, "HTTP connection from %s:%d\n",
					inet_ntoa(clientname.sin_addr),
					ntohs(clientname.sin_port) );
				/*if (fcntl(shttp, F_SETFL, O_NONBLOCK) < 0) {
					DPRINTF(E_ERROR, L_GENERAL, "fcntl F_SETFL, O_NONBLOCK\n");
				}*/
				/* Create a new upnphttp object and add it to
				 * the active upnphttp object list */
				tmp = New_upnphttp(shttp);
				if(tmp)
				{
					tmp->clientaddr = clientname.sin_addr;
					LIST_INSERT_HEAD(&upnphttphead, tmp, entries);
				}
				else
				{
					DPRINTF(E_ERROR, L_GENERAL, "New_upnphttp() failed\n");
					close(shttp);
				}
			}
		}
		/* delete finished HTTP connections */
		for(e = upnphttphead.lh_first; e != NULL; )
		{
			next = e->entries.le_next;
			if(e->state >= 100)
			{
				LIST_REMOVE(e, entries);
				Delete_upnphttp(e);
			}
			e = next;
		}
	}

shutdown:
	/* kill the scanner */
	if( scanning && scanner_pid )
	{
		kill(scanner_pid, 9);
	}
	/* close out open sockets */
	while(upnphttphead.lh_first != NULL)
	{
		e = upnphttphead.lh_first;
		LIST_REMOVE(e, entries);
		Delete_upnphttp(e);
	}

	if (sudp >= 0) close(sudp);
	if (shttpl >= 0) close(shttpl);
	#ifdef TIVO_SUPPORT
	if (sbeacon >= 0) close(sbeacon);
	#endif
	
	if(SendSSDPGoodbye(snotify, n_lan_addr) < 0)
	{
		DPRINTF(E_ERROR, L_GENERAL, "Failed to broadcast good-bye notifications\n");
	}
	for(i=0; i<n_lan_addr; i++)
		close(snotify[i]);

	if( inotify_thread )
		pthread_join(inotify_thread, NULL);

	sql_exec(db, "UPDATE SETTINGS set UPDATE_ID = %u", updateID);
	sqlite3_close(db);

	upnpevents_removeSubscribers();

	media_path = media_dirs;
	art_names = album_art_names;
	while( media_path )
	{
		free(media_path->path);
		last_path = media_path;
		media_path = media_path->next;
		free(last_path);
	}
	while( art_names )
	{
		free(art_names->name);
		last_name = art_names;
		art_names = art_names->next;
		free(last_name);
	}

	if(unlink(pidfilename) < 0)
	{
		DPRINTF(E_ERROR, L_GENERAL, "Failed to remove pidfile %s: %s\n", pidfilename, strerror(errno));
	}

	freeoptions();

	exit(EXIT_SUCCESS);
}

