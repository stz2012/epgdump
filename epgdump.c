#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
#include <iconv.h>
#include <time.h>

#include "ts.h"
#include "sdt.h"
#include "eit.h"
#include "ts_ctl.h"

#include "xmldata.c"

SVT_CONTROL	*svttop = NULL;
#define		SECCOUNT	64
char	title[1024];
char	subtitle[1024];
char	Category[1024];
char	ServiceName[1024];

/* prototype */
extern int strrep(char *buf, char *mae, char *ato);

void	xmlspecialchars(char *str)
{
	strrep(str, "&", "&amp;");
	strrep(str, "'", "&apos;");
	strrep(str, "\"", "&quot;");
	strrep(str, "<", "&lt;");
	strrep(str, ">", "&gt;");
}

void	GetEITSDTInfo(FILE *infile, SECcache *secs, int count)
{
	SECcache  *bsecs;
	SVT_CONTROL *svtcur;
	int pid;

	while((bsecs = readTS(infile, secs, count)) != NULL) {
		pid = bsecs->pid & 0xFF;
		switch (pid) {
			case 0x11: // SDT
				dumpSDT(bsecs->buf, svttop);
				svtcur = svttop->next;
				while(svtcur) {
					if (svtcur->eit == NULL) {
						svtcur->eit = calloc(1, sizeof(EIT_CONTROL));
					}
					svtcur = svtcur->next;
				}
				break;
			case 0x12: // EIT
			case 0x26: // EIT(地デジ)
			case 0x27: // EIT(地デジ)
				dumpEIT(bsecs->buf, svttop);
				break;
		}
	}
}

void	dumpXML(FILE *outfile, char *header)
{
	SVT_CONTROL	*svtcur ;
	EIT_CONTROL	*eitcur ;
	char	cendtime[32];
	char	cstarttime[32];

	fprintf(outfile, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	fprintf(outfile, "<!DOCTYPE tv SYSTEM \"xmltv.dtd\">\n\n");
	fprintf(outfile, "<tv generator-info-name=\"tsEPG2xml\" generator-info-url=\"http://localhost/\">\n");

	svtcur = svttop->next;
	while(svtcur != NULL) {
		if (!svtcur->haveeitschedule) {
			svtcur = svtcur->next;
			continue;
		}
		memset(ServiceName, '\0', sizeof(ServiceName));
		strcpy(ServiceName, svtcur->service_name);
		xmlspecialchars(ServiceName);

		fprintf(outfile, "  <channel id=\"%s_%d\" tp=\"%s\">\n", (svtcur->original_network_id < 0x10) ? header : "GR", svtcur->service_id, (svtcur->original_network_id < 0x10) ? getTP(svtcur->transport_stream_id) : header);
		fprintf(outfile, "    <display-name lang=\"ja_JP\">%s</display-name>\n", ServiceName);
		fprintf(outfile, "    <transport_stream_id>%d</transport_stream_id>\n", svtcur->transport_stream_id);
		fprintf(outfile, "    <original_network_id>%d</original_network_id>\n", svtcur->original_network_id);
		fprintf(outfile, "    <service_id>%d</service_id>\n", svtcur->service_id);
		fprintf(outfile, "  </channel>\n");
		svtcur = svtcur->next;
	}

	svtcur = svttop->next;
	while(svtcur != NULL) {
		if (!svtcur->haveeitschedule) {
			svtcur = svtcur->next;
			continue;
		}
		eitcur = svtcur->eit;
		while(eitcur != NULL){
			if(!eitcur->service_id){
				eitcur = eitcur->next ;
				continue ;
			}
			if(eitcur->content_type > CAT_COUNT){
				eitcur->content_type = CAT_COUNT -1 ;
			}

			memset(title, '\0', sizeof(title));
			strcpy(title, eitcur->title);
			xmlspecialchars(title);

			memset(subtitle, '\0', sizeof(subtitle));
			strcpy(subtitle, eitcur->subtitle);
			xmlspecialchars(subtitle);

			memset(Category, '\0', sizeof(Category));
			strcpy(Category, ContentCatList[eitcur->content_type].japanese);
			xmlspecialchars(Category);

			memset(cendtime, '\0', sizeof(cendtime));
			memset(cstarttime, '\0', sizeof(cstarttime));
			strcpy(cendtime, strTime(eitcur->start_time + eitcur->duration , "%Y%m%d%H%M%S"));
			strcpy(cstarttime, strTime(eitcur->start_time, "%Y%m%d%H%M%S"));

			fprintf(outfile, "  <programme start=\"%s +0900\" stop=\"%s +0900\" channel=\"%s_%d\" event_id=\"%d\">\n",
					cstarttime, cendtime, (svtcur->original_network_id < 0x10) ? header : "GR", svtcur->service_id, eitcur->event_id);
			fprintf(outfile, "    <title lang=\"ja_JP\">%s</title>\n", title);
			fprintf(outfile, "    <desc lang=\"ja_JP\">%s</desc>\n", subtitle);
			fprintf(outfile, "    <category lang=\"ja_JP\">%s</category>\n", Category);
			fprintf(outfile, "    <category lang=\"en\">%s</category>\n", ContentCatList[eitcur->content_type].english);
			fprintf(outfile, "  </programme>\n");
			eitcur = eitcur->next;
		}
		svtcur = svtcur->next;
	}

	fprintf(outfile, "</tv>\n");
}

int main(int argc, char *argv[])
{

	FILE *infile = stdin;
	FILE *outfile = stdout;
	char	*arg_onTV ;
	int		staCount ;
	int   inclose = 0;
	int   outclose = 0;
	SECcache   secs[SECCOUNT];
	char	*ontvheader ;

	if(argc == 4){
		arg_onTV = argv[1];
		if(strcmp(argv[2], "-")) {
			infile = fopen(argv[2], "r");
			if ( !infile) {
			  printf( "tsFile not found (Can't open file: %s)\n", argv[2] );
			  exit( -1 );
			}
			inclose = 1;
		}
		else {
			infile = stdin;
		}
		if(strcmp(argv[3], "-")) {
			outfile = fopen(argv[3], "w+");
			if ( !outfile) {
			  printf( "xmlFile not found (Can't open file: %s)\n", argv[3] );
			  exit( -1 );
			}
			outclose = 1;
		}
		else {
			outfile = stdout;
		}
	}else{
		fprintf(stdout, "Usage : %s {/BS|/CS|<id>} <tsFile> <outfile>\n", argv[0]);
		fprintf(stdout, "\n");
		fprintf(stdout, "id       チャンネル識別子。地上波の物理チャンネルを与えます。\n");
		fprintf(stdout, "/BS      BSモード。一つのTSからBS全局のデータを読み込みます。\n");
		fprintf(stdout, "/CS      CSモード。一つのTSから複数局のデータを読み込みます。\n");
		return 0;
	}

	staCount = 0;
	svttop = calloc(1, sizeof(SVT_CONTROL));

	/* 興味のあるpidを指定 */
	memset(secs, 0,  sizeof(SECcache) * SECCOUNT);
	secs[0].pid = 0x10;
	secs[1].pid = 0x11;
	secs[2].pid = 0x12;
	secs[3].pid = 0x26;
	secs[4].pid = 0x27;
	GetEITSDTInfo(infile, secs, SECCOUNT);
	if(strcmp(arg_onTV, "/BS") == 0){
		ontvheader = "BS";
	}else if(strcmp(arg_onTV, "/CS") == 0){
		ontvheader = "CS";
	}else{
		ontvheader = arg_onTV;
	}
	dumpXML(outfile, ontvheader);

	if(inclose) {
		fclose(infile);
	}
	if(outclose) {
		fclose(outfile);
	}

	return 0;
}
