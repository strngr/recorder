#include <stdio.h>
#include <stdlib.h>
#include "utstring.h"
#include "ctype.h"
#include "udata.h"
#include "misc.h"

char *bindump(char *buf, long buflen)
{
	static UT_string *out = NULL;
	int i, ch;

	utstring_renew(out);

	for (i = 0; i < buflen; i++) {
		ch = buf[i];
		if (isprint(ch)) {
			utstring_printf(out, "%c", ch);
		} else {
			utstring_printf(out, " %02X ", ch & 0xFF);
		}
	}
	return (utstring_body(out));
}

/*
 * At each received message, the recorder invokes this function with the
 * current epoch time and the topic being handled. Use this to update
 * a monitoring hoook. If we have Redis, use that exclusively.
 */

void monitorhook(struct udata *userdata, time_t now, char *topic)
{
	struct udata *ud = (struct udata *)userdata;

#ifdef HAVE_REDIS
        if (ud->useredis) {
		monitor_update(ud, now, topic);
		return;
	}
#else
	if (ud->usefiles) {
		char mpath[BUFSIZ];
		FILE *fp;

		sprintf(mpath, "%s/monitor", JSONDIR);
		if ((fp = fopen(mpath, "w")) != NULL) {
			fprintf(fp, "%ld %s\n", now, topic);
			fclose(fp);
		}
	}
#endif
}