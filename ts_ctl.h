#ifndef	__TS_CONTROL_H__
#define	__TS_CONTROL_H__

#include	"util.h"

typedef	struct	_SVT_CONTROL	SVT_CONTROL;
typedef	struct	_EIT_CONTROL	EIT_CONTROL;

struct	_SVT_CONTROL{
	SVT_CONTROL	*next ;
	SVT_CONTROL	*prev ;
	int		original_network_id ;		// OriginalNetworkID
	int		transport_stream_id ;		// TransporrtStreamID
	int		service_id ;				// ServiceID
	char	service_name[MAXSECLEN] ;	// サービス名
	EIT_CONTROL	*eit;					// EIT テーブル
	char	haveeitschedule;			// EITスケジュールがあるか。
};

struct	_EIT_CONTROL{
	EIT_CONTROL	*next ;
	EIT_CONTROL	*prev ;
	int		table_id ;
	int		service_id ;
	int		event_id ;			// イベントID
	int		content_type ;		// コンテントタイプ
	char	*title ;			// タイトル
	char	*subtitle ;			// サブタイトル
	int	duration;				// 時間
	time_t	start_time;			// 開始時刻
};
#endif
