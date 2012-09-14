typedef		struct	_ContentTYPE{
	char	*japanese ;
	char	*english ;
}CONTENT_TYPE;

#define		CAT_COUNT		16
static  CONTENT_TYPE	ContentCatList[CAT_COUNT] = {
	{ "ニュース／報道", "news" },
	{ "スポーツ", "sports" },
	{ "情報／ワイドショー", "information" },
	{ "ドラマ", "drama" },
	{ "音楽", "music" },
	{ "バラエティ", "variety" },
	{ "映画", "cinema" },
	{ "アニメ／特撮", "anime" },
	{ "ドキュメンタリー／教養", "documentary" },
	{ "劇場／公演", "stage" },
	{ "趣味／教育", "hobby" },
	{ "福祉", "welfare" },
	{ "予備", "etc" },
	{ "予備", "etc" },
	{ "予備", "etc" },
	{ "その他", "etc" }
};

static unsigned short getTSID2TP(unsigned short tsid) {
	return (tsid & 0x01f0)>>4;
}

static unsigned short getTSID2SLOT(unsigned short tsid) {
	/* for NHK BS */
	if (tsid == 16625 || tsid == 16626) {
		tsid = tsid - 1;
	}
	return tsid & 0x0007;
}

static char *getTP(unsigned short tsid) {
	static char ret[10];
	ret[0] = 0;
	switch ((tsid & 0xf000)>>12) {
		case 4:
			sprintf(ret, "BS%d_%d", getTSID2TP(tsid), getTSID2SLOT(tsid));
			break;
		case 6:
		case 7:
			sprintf(ret, "CS%d", getTSID2TP(tsid));
			break;
	}
	return ret;
}
