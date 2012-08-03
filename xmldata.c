typedef		struct	_ContentTYPE{
	char	*japanese ;
	char	*english ;
}CONTENT_TYPE;

#define		CAT_COUNT		16
static  CONTENT_TYPE	ContentCatList[CAT_COUNT] = {
	{ "ニュース・報道", "news" },
	{ "スポーツ", "sports" },
	{ "情報", "information" },
	{ "ドラマ", "drama" },
	{ "音楽", "music" },
	{ "バラエティ", "variety" },
	{ "映画", "cinema" },
	{ "アニメ・特撮", "anime" },
	{ "ドキュメンタリー・教養", "documentary" },
	{ "演劇", "stage" },
	{ "趣味・実用", "hobby" },
	{ "福祉", "welfare" },
	{ "予備", "etc" },
	{ "予備", "etc" },
	{ "予備", "etc" },
	{ "その他", "etc" }
};

typedef struct _TAG_STATION
{
	char	*name;
	char	*ontv;
	int		tsId;		// OriginalNetworkID
	int		onId;		// TransportStreamID
	int		svId;		// ServiceID
	char	*tpnm;
} STATION;

static char *getTP(int tsid) {
	char *ret;
	ret="";
	switch (tsid) {
		/* BS */
		case 16400: ret = "B1"; break;		//BS朝日
		case 16401: ret = "B2"; break;		//BS-TBS
		case 16433: ret = "B3"; break;		//BSジャパン
		case 16432: ret = "B4"; break;		//WOWOWプライム
		case 17488: ret = "B5"; break;		//WOWOWライブ
		case 17489: ret = "B6"; break;		//WOWOWシネマ
		case 17520: ret = "B7"; break;		//スターチャンネル2/3
		case 18033: ret = "B8"; break;		//BSアニマックス
		case 18034: ret = "B9"; break;		//ディズニーチャンネル
		case 16528: ret = "B10"; break;		//BS11
		case 16529: ret = "B11"; break;		//スターチャンネル1
		case 16530: ret = "B12"; break;		//TwellV
		case 18096: ret = "B13"; break;		//FOX bs238
		case 18097: ret = "B14"; break;		//BSスカパー!
		case 18098: ret = "B15"; break;		//放送大学
		case 16592: ret = "B16"; break;		//BS日テレ
		case 16593: ret = "B17"; break;		//BSフジ
		case 16625: ret = "B18"; break;		//NHK BS1
		case 16626: ret = "B19"; break;		//NHK BSプレミアム
		case 17168: ret = "B20"; break;		//地デジ難視聴1(NHK/NHK-E/CX)
		case 17169: ret = "B21"; break;		//地デジ難視聴2(NTV/TBS/EX/TX)
		case 18224: ret = "B22"; break;		//グリーンチャンネル
		case 18225: ret = "B23"; break;		//J SPORTS 1
		case 18226: ret = "B24"; break;		//J SPORTS 2
		case 18256: ret = "B25"; break;		//IMAGICA BS
		case 18257: ret = "B26"; break;		//J SPORTS 3
		case 18258: ret = "B27"; break;		//J SPORTS 4
		case 18288: ret = "B28"; break;		//BS釣りビジョン
		case 18289: ret = "B29"; break;		//日本映画専門チャンネル
		case 18290: ret = "B30"; break;		//D-Life
		/* CS110 */
		case 24608: ret = "C1"; break;		//ND2
		case 28736: ret = "C2"; break;		//ND4
		case 28768: ret = "C3"; break;		//ND6
		case 24704: ret = "C4"; break;		//ND8
		case 24736: ret = "C5"; break;		//ND10
		case 28864: ret = "C6"; break;		//ND12
		case 28896: ret = "C7"; break;		//ND14
		case 28928: ret = "C8"; break;		//ND16
		case 28960: ret = "C9"; break;		//ND18
		case 28992: ret = "C10"; break;		//ND20
		case 29024: ret = "C11"; break;		//ND22
		case 29056: ret = "C12"; break;		//ND24
	}
	return ret;
}
