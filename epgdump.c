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

void	GetSDT(FILE *infile, SVT_CONTROL *svttop, SECcache *secs, int count)
{
	SECcache  *bsecs;

	while((bsecs = readTS(infile, secs, count)) != NULL) {
		/* SDT */
		if((bsecs->pid & 0xFF) == 0x11) {
			dumpSDT(bsecs->buf, svttop);
		}
	}
}

void	GetStation(STATION **station, int * station_count, char *header)
{
	SVT_CONTROL	*svtcur;
	STATION * pStation = *station;
	char sid[80];
	int stationi = *station_count;

	svtcur = svttop->next;
	while(svtcur != NULL) {
		sprintf(sid, "%s_%d", header, svtcur->service_id);
		pStation = realloc(pStation, (stationi + 1) * sizeof(STATION));
		memset(&pStation[stationi], 0, sizeof(STATION));

		pStation[stationi].name = malloc( strlen(svtcur->service_name) + 1 );
		pStation[stationi].ontv = malloc( strlen(sid) + 1 );
		pStation[stationi].tsId = svtcur->transport_stream_id;
		pStation[stationi].onId = svtcur->original_network_id;
		pStation[stationi].svId = svtcur->service_id;
		pStation[stationi].tpnm = getTP(svtcur->transport_stream_id);
		strcpy(pStation[stationi].name, svtcur->service_name);
		strcpy(pStation[stationi].ontv, sid);

		stationi++;
		svtcur = svtcur->next;
	}
	*station = pStation;
	*station_count = stationi;
}

void	GetEIT(FILE *infile, FILE *outfile, STATION *psta, SECcache *secs, int count)
{
	SECcache  *bsecs;
	EIT_CONTROL	*eitcur ;
	EIT_CONTROL	*eitnext ;
	EIT_CONTROL	*eittop = NULL;
	time_t	l_time ;
	time_t	end_time ;
	struct	tm	tl ;
	struct	tm	*endtl ;
	char	cendtime[32];
	char	cstarttime[32];

	eittop = calloc(1, sizeof(EIT_CONTROL));
	eitcur = eittop ;
	fseek(infile, 0, SEEK_SET);
	while((bsecs = readTS(infile, secs, SECCOUNT)) != NULL) {
		/* EIT */
		if((bsecs->pid & 0xFF) == 0x12) {
			dumpEIT(bsecs->buf, psta->svId, psta->onId, psta->tsId, eittop);
		}else if((bsecs->pid & 0xFF) == 0x26) {
			dumpEIT(bsecs->buf, psta->svId, psta->onId, psta->tsId, eittop);
		}else if((bsecs->pid & 0xFF) == 0x27) {
			dumpEIT(bsecs->buf, psta->svId, psta->onId, psta->tsId, eittop);
		}
	}
	eitcur = eittop ;
	while(eitcur != NULL){
		if(!eitcur->servid){
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

		tl.tm_sec = eitcur->ss ;
		tl.tm_min = eitcur->hm ;
		tl.tm_hour = eitcur->hh ;
		tl.tm_mday = eitcur->dd ;
		tl.tm_mon = (eitcur->mm - 1);
		tl.tm_year = (eitcur->yy - 1900);
		tl.tm_wday = 0;
		tl.tm_isdst = 0;
		tl.tm_yday = 0;
		l_time = mktime(&tl);
		if((eitcur->ehh == 0) && (eitcur->emm == 0) && (eitcur->ess == 0)){
			(void)time(&l_time);
			end_time = l_time + (60 * 5);		// ５分後に設定
		endtl = localtime(&end_time);
		}else{
			end_time = l_time + eitcur->ehh * 3600 + eitcur->emm * 60 + eitcur->ess;
			endtl = localtime(&end_time);
		}
		memset(cendtime, '\0', sizeof(cendtime));
		memset(cstarttime, '\0', sizeof(cstarttime));
		strftime(cendtime, (sizeof(cendtime) - 1), "%Y%m%d%H%M%S", endtl);
		strftime(cstarttime, (sizeof(cstarttime) - 1), "%Y%m%d%H%M%S", &tl);
#if 1
		fprintf(outfile, "  <programme start=\"%s +0900\" stop=\"%s +0900\" channel=\"%s\">\n",	
				cstarttime, cendtime, psta->ontv);
		fprintf(outfile, "    <title lang=\"ja_JP\">%s</title>\n", title);
		fprintf(outfile, "    <desc lang=\"ja_JP\">%s</desc>\n", subtitle);
		fprintf(outfile, "    <category lang=\"ja_JP\">%s</category>\n", Category);
		fprintf(outfile, "    <category lang=\"en\">%s</category>\n", ContentCatList[eitcur->content_type].english);
		fprintf(outfile, "  </programme>\n");
#else
		fprintf(outfile, "(%x:%x:%x)%s,%s,%s,%s,%s,%s\n",
					eitcur->servid, eitcur->table_id, eitcur->event_id,
					cstarttime, cendtime,
					title, subtitle,
					Category,
					ContentCatList[eitcur->content_type].english);
#endif
#if 0
		fprintf(outfile, "(%x:%x)%04d/%02d/%02d,%02d:%02d:%02d,%02d:%02d:%02d,%s,%s,%s,%s\n",
					eitcur->table_id, eitcur->event_id,
					eitcur->yy, eitcur->mm, eitcur->dd,
					eitcur->hh, eitcur->hm, eitcur->ss,
					eitcur->ehh, eitcur->emm, eitcur->ess,
					eitcur->title, eitcur->subtitle,
					ContentCatList[eitcur->content_type].japanese,
					ContentCatList[eitcur->content_type].english);
#endif
		eitnext = eitcur->next ;
		free(eitcur->title);
		free(eitcur->subtitle);
		free(eitcur);
		eitcur = eitnext ;
	}
	free(eittop);
	eittop = NULL;
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
	int		lp ;
	STATION	*pStas ;
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

	pStas = NULL;
	staCount = 0;
	svttop = calloc(1, sizeof(SVT_CONTROL));

	/* 興味のあるpidを指定 */
	memset(secs, 0,  sizeof(SECcache) * SECCOUNT);
	secs[0].pid = 0x10;
	secs[1].pid = 0x11;
	secs[2].pid = 0x12;
	secs[3].pid = 0x26;
	secs[4].pid = 0x27;
	GetSDT(infile, svttop, secs, SECCOUNT);
	if(strcmp(arg_onTV, "/BS") == 0){
		ontvheader = "BS";
	}else if(strcmp(arg_onTV, "/CS") == 0){
		ontvheader = "CS";
	}else{
		ontvheader = "GR";
	}
	GetStation(&pStas, &staCount, ontvheader);

	fprintf(outfile, "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
	fprintf(outfile, "<!DOCTYPE tv SYSTEM \"xmltv.dtd\">\n\n");
	fprintf(outfile, "<tv generator-info-name=\"tsEPG2xml\" generator-info-url=\"http://localhost/\">\n");
	for(lp = 0 ; lp < staCount ; lp++){
		memset(ServiceName, '\0', sizeof(ServiceName));
		strcpy(ServiceName, pStas[lp].name);
		xmlspecialchars(ServiceName);

		fprintf(outfile, "  <channel id=\"%s\" tp=\"%s\">\n", pStas[lp].ontv, (strcmp(ontvheader, "GR") == 0) ? arg_onTV : pStas[lp].tpnm);
		fprintf(outfile, "    <display-name lang=\"ja_JP\">%s</display-name>\n", ServiceName);
		fprintf(outfile, "    <transport_stream_id>%d</transport_stream_id>\n", pStas[lp].tsId);
		fprintf(outfile, "    <original_network_id>%d</original_network_id>\n", pStas[lp].onId);
		fprintf(outfile, "    <service_id>%d</service_id>\n", pStas[lp].svId);
		fprintf(outfile, "  </channel>\n");
	}
	for(lp = 0 ; lp < staCount ; lp++){
		GetEIT(infile, outfile, &pStas[lp], secs, SECCOUNT);
	}
	fprintf(outfile, "</tv>\n");

	if(inclose) {
		fclose(infile);
	}
	if(outclose) {
		fclose(outfile);
	}

	return 0;
}
