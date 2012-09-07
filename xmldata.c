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
		case 16400: ret = "BS01_0"; break;	//BS朝日
		case 16401: ret = "BS01_1"; break;	//BS-TBS
		case 16432: ret = "BS03_0"; break;	//WOWOWプライム
		case 16433: ret = "BS03_1"; break;	//BSジャパン
		case 17488: ret = "BS05_0"; break;	//WOWOWライブ
		case 17489: ret = "BS05_1"; break;	//WOWOWシネマ
		case 17520: ret = "BS07_0"; break;	//スターチャンネル2/3
		case 18033: ret = "BS07_1"; break;	//BSアニマックス
		case 18034: ret = "BS07_2"; break;	//ディズニーチャンネル
		case 16528: ret = "BS09_0"; break;	//BS11
		case 16529: ret = "BS09_1"; break;	//スターチャンネル1
		case 16530: ret = "BS09_2"; break;	//TwellV
		case 18096: ret = "BS11_0"; break;	//FOX bs238
		case 18097: ret = "BS11_1"; break;	//BSスカパー!
		case 18098: ret = "BS11_2"; break;	//放送大学
		case 16592: ret = "BS13_0"; break;	//BS日テレ
		case 16593: ret = "BS13_1"; break;	//BSフジ
		case 16625: ret = "BS15_0"; break;	//NHK BS1
		case 16626: ret = "BS15_1"; break;	//NHK BSプレミアム
		case 17168: ret = "BS17_0"; break;	//地デジ難視聴1(NHK/NHK-E/CX)
		case 17169: ret = "BS17_1"; break;	//地デジ難視聴2(NTV/TBS/EX/TX)
		case 18224: ret = "BS19_0"; break;	//グリーンチャンネル
		case 18225: ret = "BS19_1"; break;	//J SPORTS 1
		case 18226: ret = "BS19_2"; break;	//J SPORTS 2
		case 18256: ret = "BS21_0"; break;	//IMAGICA BS
		case 18257: ret = "BS21_1"; break;	//J SPORTS 3
		case 18258: ret = "BS21_2"; break;	//J SPORTS 4
		case 18288: ret = "BS23_0"; break;	//BS釣りビジョン
		case 18289: ret = "BS23_1"; break;	//日本映画専門チャンネル
		case 18290: ret = "BS23_2"; break;	//D-Life
		/* CS110 */
		case 24608: ret = "CS2"; break;		//ND2
		case 28736: ret = "CS4"; break;		//ND4
		case 28768: ret = "CS6"; break;		//ND6
		case 24704: ret = "CS8"; break;		//ND8
		case 24736: ret = "CS10"; break;	//ND10
		case 28864: ret = "CS12"; break;	//ND12
		case 28896: ret = "CS14"; break;	//ND14
		case 28928: ret = "CS16"; break;	//ND16
		case 28960: ret = "CS18"; break;	//ND18
		case 28992: ret = "CS20"; break;	//ND20
		case 29024: ret = "CS22"; break;	//ND22
		case 29056: ret = "CS24"; break;	//ND24
	}
	return ret;
}
