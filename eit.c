#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "eit.h"

extern void enqueue_sdt(SVT_CONTROL *top, SVT_CONTROL *sdtptr);

char		*subtitle_cnv_str[] = {
	NULL
};

int parseEIThead(unsigned char *data, EIThead *h) {
	int boff = 0;

	memset(h, 0, sizeof(EIThead));

	h->table_id = getBit(data, &boff, 8);
	h->section_syntax_indicator = getBit(data, &boff, 1);
	h->reserved_future_use = getBit(data, &boff, 1);
	h->reserved1 = getBit(data, &boff, 2);
	h->section_length =getBit(data, &boff,12);
	h->service_id = getBit(data, &boff, 16);
	h->reserved2 = getBit(data, &boff, 2);
	h->version_number = getBit(data, &boff, 5);
	h->current_next_indicator = getBit(data, &boff, 1);
	h->section_number = getBit(data, &boff, 8);
	h->last_section_number = getBit(data, &boff, 8);
	h->transport_stream_id = getBit(data, &boff, 16);
	h->original_network_id = getBit(data, &boff, 16);
	h->segment_last_section_number = getBit(data, &boff, 8);
	h->last_table_id = getBit(data, &boff, 8);

	return 14;
}

int parseEITbody(unsigned char *data, EITbody *b)
{
	int boff = 0;

	memset(b, 0, sizeof(EITbody));

	b->event_id = getBit(data, &boff, 16);

	memcpy(b->start_time, data + boff / 8, 5);
	/* b->start_time = getBit(data, &boff, 40); */
	boff += 40;
	memcpy(b->duration, data + boff / 8, 3);
	/* b->duration = getBit(data, &boff, 24); */
	boff += 24;
	b->running_status = getBit(data, &boff, 3);
	b->free_CA_mode = getBit(data, &boff, 1);
	b->descriptors_loop_length = getBit(data, &boff, 12);

	return 12;
}

int parseSEVTdesc(unsigned char *data, SEVTdesc *desc) {
	int boff = 0;

	memset(desc, 0, sizeof(SEVTdesc));

	desc->descriptor_tag = getBit(data, &boff, 8);
	if((desc->descriptor_tag & 0xFF) != 0x4D) {
		return -1;
	}
	desc->descriptor_length = getBit(data, &boff, 8);
	memcpy(desc->ISO_639_language_code, data + boff / 8, 3);
	/* desc->ISO_639_language_code = getBit(data, &boff, 24); */
	boff += 24;
	desc->event_name_length = getBit(data, &boff, 8);
	getStr(desc->event_name, data, &boff, desc->event_name_length);
	desc->text_length = getBit(data, &boff, 8);
	getStr(desc->text, data, &boff, desc->text_length);

	return desc->descriptor_length + 2;
}

int parseContentDesc(unsigned char *data, ContentDesc *desc) {
	int boff = 0;

	memset(desc, 0, sizeof(ContentDesc));

	desc->descriptor_tag = getBit(data, &boff, 8);
	if((desc->descriptor_tag & 0xFF) != 0x54) {
		return -1;
	}
	desc->descriptor_length = getBit(data, &boff, 8);
	memcpy(desc->content, data+(boff/8), desc->descriptor_length);
	//getStr(desc->content, data, &boff, desc->descriptor_length);
	return desc->descriptor_length + 2;
}

int parseSeriesDesc(unsigned char *data, SeriesDesc *desc) {
	int boff = 0;

	memset(desc, 0, sizeof(SeriesDesc));

	desc->descriptor_tag = getBit(data, &boff, 8);
	if((desc->descriptor_tag & 0xFF) != 0xD5) {
		return -1;
	}
	desc->descriptor_length = getBit(data, &boff, 8);
	desc->series_id = getBit(data, &boff, 16);
	desc->repeat_label = getBit(data, &boff, 4);
	desc->program_pattern = getBit(data, &boff, 3);
	desc->expire_date_valid_flag = getBit(data, &boff, 1);

	desc->expire_date = getBit(data, &boff, 16);
	//memcpy(desc->expire_date, data + boff / 8, 2);
	//boff += 16;

	desc->episode_number = getBit(data, &boff, 12);
	desc->last_episode_number = getBit(data, &boff, 12);

	getStr(desc->series_name_char, data, &boff, desc->descriptor_length - 8);
	return desc->descriptor_length + 2;
}

int parseEEVTDhead(unsigned char *data, EEVTDhead *desc) {
	int boff = 0;

	memset(desc, 0, sizeof(EEVTDhead));

	desc->descriptor_tag = getBit(data, &boff, 8);
	if((desc->descriptor_tag & 0xFF) != 0x4E) {
		return -1;
	}
	desc->descriptor_length = getBit(data, &boff, 8);
	desc->descriptor_number = getBit(data, &boff, 4);
	desc->last_descriptor_number = getBit(data, &boff, 4);
	memcpy(desc->ISO_639_language_code, data + boff / 8, 3);
	/* desc->ISO_639_language_code = getBit(data, &boff, 24); */
	boff += 24;

	desc->length_of_items = getBit(data, &boff, 8);

	return 7;
}

int parseEEVTDitem(unsigned char *data, EEVTDitem *desc) {
	int boff = 0;

	memset(desc, 0, sizeof(EEVTDitem));

	desc->item_description_length = getBit(data, &boff, 8);
	getStr(desc->item_description, data, &boff, desc->item_description_length);

	desc->item_length = getBit(data, &boff, 8);
	memcpy(desc->item, data + (boff / 8), desc->item_length);
	/* getStr(desc->item, data, &boff, desc->item_length); */

	return desc->item_description_length + desc->item_length + 2;
}

int parseEEVTDtail(unsigned char *data, EEVTDtail *desc) {
	int boff = 0;

	memset(desc, 0, sizeof(EEVTDtail));

	desc->text_length = getBit(data, &boff, 8);
	getStr(desc->text, data, &boff, desc->text_length);

	return desc->text_length + 1;
}

int checkEEVTDitem(EEVTDitem *save, EEVTDitem *new, int descriptor_number) {

	EEVTDitem swap;
	int boff = 0;

	if(new == NULL) {
		if(save->item_length != 0) {
			swap = *save;
			memset(save->item,0,MAXSECLEN);
			getStr(save->item, (unsigned char*)swap.item, &boff, swap.item_length);
			return 1;
		} else {
			return 0;
		}
	}

	if(new->item_description_length == 0) {
		/* 続き 保存 */
		memcpy(save->item + save->item_length, new->item, new->item_length);
		save->item_length += new->item_length;
		return 0;
	} else {
		/* ブレーク。saveを印刷対象にする。saveをクリア? */
		if(save->item_length != 0) {
			/* 退避済みがあり */
			swap = *save;
			memset(save->item,0,MAXSECLEN);
			getStr(save->item, (unsigned char*)swap.item, &boff, swap.item_length);
			swap = *new;
			*new = *save;
			*save = swap;
			save->descriptor_number = descriptor_number;
		} else {
			*save = *new;
			save->descriptor_number = descriptor_number;
			return 0;
		}
	}

	return 1;
}

EIT_CONTROL	*search_eit(EIT_CONTROL *top, int event_id)
{
	EIT_CONTROL	*cur = top ;
	while(cur != NULL){
		if(cur->event_id == event_id){
			return cur ;
		}
		cur = cur->next ;
	}
	return NULL ;
}

char	*strstr_eucjp(const char *str, const char *search)
{
	char *pos ;
	pos = (char *)str ;

	while (*pos != '\0') {
		if (*pos == *search) {
			if (strncmp(pos, search, strlen(search)) == 0) {
				return pos ;
			}
		}
		if ((unsigned char)*pos == 0x8Fu) {
			pos += 3 ;
		} else if ((unsigned char)*pos >= 0x80u) {
			pos += 2 ;
		} else {
			pos += 1 ;
		}
	}

	return NULL ;
}

void	conv_title_subtitle(EIT_CONTROL *eitptr)
{
	int		lp = 0 ;
//	size_t	addsize ;
	char	*ptr ;
	char	*ptr2 ;
	char	*newsubtitle ;

	for(lp = 0 ; subtitle_cnv_str[lp] != NULL ; lp++){
		ptr = strstr(eitptr->title, subtitle_cnv_str[lp]);
		if(ptr == NULL){
			continue ;
		}
		// タイトルがなくならないように
		if(ptr == eitptr->title){
			continue ;
		}
		ptr2 = ptr ;
		for( ; (unsigned char)*ptr2 == 0x20u ; ptr2++ );
		for( ; (unsigned char)*ptr2 == 0xA1u && (unsigned char)*(ptr2+1) == 0xA1u ; ptr2 += 2);
		for( ; (unsigned char)*ptr2 == 0x20u ; ptr2++ );
		newsubtitle = calloc(1, ((strlen(ptr2) + 3) + (strlen(eitptr->subtitle) + 1)));
		memcpy(newsubtitle, ptr2, strlen(ptr2));
//		*(newsubtitle+strlen(ptr)) = ' ';
		strcat(newsubtitle, "▽");
		*ptr = '\0';
		strcat(newsubtitle, eitptr->subtitle);
		free(eitptr->subtitle);
		eitptr->subtitle = newsubtitle ;
		return ;
	}
}

void	enqueue_eit(EIT_CONTROL *top, EIT_CONTROL *eitptr)
{
	EIT_CONTROL	*cur ;
	cur = top ;
	time_t		rc ;

	if(top->next == NULL){
		top->next = eitptr;
		eitptr->prev = top;
		return ;
	}
	cur = top->next ;
	while(cur != NULL){
		rc = cur->start_time - eitptr->start_time;
		if(rc == 0){
			free(eitptr->title);
			free(eitptr->subtitle);
			free(eitptr);
			return ;
		}
		if(rc > 0){
			if(cur->prev != 0){
				cur->prev->next = eitptr;
				eitptr->prev = cur->prev;
			}
			else {
				top->next = eitptr;
			}
			cur->prev = eitptr ;
			eitptr->next = cur ;
			conv_title_subtitle(eitptr);
			return ;
		}
		if(cur->next == NULL){
			cur->next = eitptr ;
			eitptr->prev = cur ;
			conv_title_subtitle(eitptr);
			return ;
		}
		cur = cur->next ;
	}
	return ;

}

void dumpEIT(unsigned char *ptr, SVT_CONTROL *top)
{
	EIThead  eith;
	EITbody  eitb;
	SEVTdesc sevtd;

	EEVTDhead eevthead;
	EEVTDitem eevtitem;
	EEVTDtail eevttail;

	EEVTDitem save_eevtitem;

	EIT_CONTROL	*cur ;
	EIT_CONTROL	*eittop;
	SVT_CONTROL	*svtcur;

	int len = 0;
	int loop_len = 0;
	int loop_blen = 0;
	int loop_elen = 0;

	/* EIT */
	len = parseEIThead(ptr, &eith); 
	// EIT ヘッダから、どのSVTのEIT情報か特定する
	svtcur=top->next;
	eittop = NULL;
	while(svtcur) {
		if ((eith.transport_stream_id == svtcur->transport_stream_id) &&
			(eith.original_network_id == svtcur->original_network_id) &&
			(eith.service_id          == svtcur->service_id)) {
			eittop = svtcur->eit;
			break;
		}
		svtcur = svtcur->next;
	}
	if (eittop == NULL) {
#if 0
		printf("Not Match %x  %x %x \n",eith.transport_stream_id,eith.original_network_id,eith.service_id);
#endif
		svtcur = calloc(1, sizeof(SVT_CONTROL));
		svtcur->transport_stream_id = eith.transport_stream_id;
		svtcur->original_network_id = eith.original_network_id;
		svtcur->service_id = eith.service_id;
		svtcur->eit = calloc(1, sizeof(EIT_CONTROL));
		enqueue_sdt(top, svtcur);
		if ((eith.table_id >= 0x50) && (!svtcur->haveeitschedule))
			svtcur->haveeitschedule = 1;
		eittop = svtcur->eit;
	}
#if 0
	printf("SV  0x%x Table  [0x%x] \n",eith.service_id,eith.table_id);
#endif
	ptr += len;
	loop_len = eith.section_length - (len - 3 + 4); // 3は共通ヘッダ長 4はCRC
	while(loop_len > 0) {
		/* 連続する拡張イベントは、漢字コードが泣き別れして
		   分割されるようだ。連続かどうかは、item_description_lengthが
		   設定されているかどうかで判断できるようだ。 */
		memset(&save_eevtitem, 0, sizeof(EEVTDitem));

		len = parseEITbody(ptr, &eitb);
#if 0
		printf("evtid:%d\n", eitb.event_id);
#endif
		cur = search_eit(eittop, eitb.event_id);

		ptr += len;
		loop_len -= len;

		loop_blen = eitb.descriptors_loop_length;
		loop_len -= loop_blen;
		while(loop_blen > 0) {
			unsigned char desctag;
			int sboff = 0;
			ContentDesc contentDesc;
			SeriesDesc seriesDesc;
			desctag = getBit(ptr, &sboff, 8);
			switch (desctag) {
				case 0x4D:
					len = parseSEVTdesc(ptr, &sevtd);
					if(len > 0) {
						if(cur == NULL){
							cur = calloc(1, sizeof(EIT_CONTROL));
							cur->event_id = eitb.event_id ;
							cur->service_id = eith.service_id ;
							cur->title = calloc(1, (strlen(sevtd.event_name) + 1));
							memcpy(cur->title, sevtd.event_name, strlen(sevtd.event_name));
							cur->subtitle = calloc(1, (strlen(sevtd.text) + 1));
							memcpy(cur->subtitle, sevtd.text, strlen(sevtd.text));
							cur->table_id = eith.table_id ;
							cur->duration = getDurationSec(eitb.duration);
							cur->start_time = getStartTime(eitb.start_time);
							enqueue_eit(eittop, cur);
							if ((eith.table_id >= 0x50) && (!svtcur->haveeitschedule))
								svtcur->haveeitschedule=1;
						}
					}
					break;
				case 0x4E:
					len = parseEEVTDhead(ptr, &eevthead);
					if(len > 0) {
						ptr += len;
						loop_blen -= len;

						loop_elen = eevthead.length_of_items;
						loop_len -= loop_elen;
						while(loop_elen > 0) {
							len = parseEEVTDitem(ptr, &eevtitem);

							ptr += len;
							loop_elen -= len;
							loop_blen -= len;

							if(checkEEVTDitem(&save_eevtitem, &eevtitem, 
											  eevthead.descriptor_number)) {
#if 0
								fprintf(out, "EEVT,%d,%d,%d,%s,%s\n", 
										eith.service_id,
										eitb.event_id,
										eevtitem.descriptor_number, /* 退避項目 */
										eevtitem.item_description,
										eevtitem.item);
#endif
							}
						}

						len = parseEEVTDtail(ptr, &eevttail);
#if 0
						fprintf(out, "EEVTt,%d,%d,%d,%s\n", 
								eith.service_id,
								eitb.event_id,
								eevthead.descriptor_number,
								eevttail.text);
#endif
					}
					break;
				case 0x54:
					len = parseContentDesc(ptr, &contentDesc);
					if (len > 0) {
						for (int i = 0; i < contentDesc.descriptor_length - 1; i+=2) {
#if 0
							if (!header_printed) {
								fprintf(out, "Content,%d,%d", eith.service_id, eitb.event_id);
								header_printed = 1;
							}
							fprintf(out, ",%02x%02x", (unsigned char)contentDesc.content[i], (unsigned char)contentDesc.content[i+1]);
#endif
						}
						if(cur != NULL){
							cur->content_type = (unsigned char)(contentDesc.content[0] >> 4);
#if 0
							fprintf(stdout, "%s:", cur->title);
							fprintf(stdout, ",%02x%02x", (unsigned char)contentDesc.content[0], (unsigned char)contentDesc.content[1]);
							fprintf(stdout, ",%02x%02x\n", (unsigned char)contentDesc.content[2], (unsigned char)contentDesc.content[3]);
#endif

						}
#if 0
						if (header_printed) {
							fprintf(out, "\n");
						}
#endif
					}
					break;
				case 0xD5:
					len = parseSeriesDesc(ptr, &seriesDesc);
					if (len > 0) {
#if 0
						fprintf(out, "Series,%d,%d,series=%d,repeat=%01x,pattern=%d,expire_valid=%d,expire=%04x,epinum=%d,lastepinum=%d,%s\n",
							eith.service_id,
							eitb.event_id,
							seriesDesc.series_id,
							seriesDesc.repeat_label,
							seriesDesc.program_pattern,
							seriesDesc.expire_date_valid_flag,
							seriesDesc.expire_date,
							seriesDesc.episode_number,
							seriesDesc.last_episode_number,
							seriesDesc.series_name_char);
#endif
					}
					break;
				default:
					len = parseOTHERdesc(ptr);
					break;
			}
			ptr += len;
			loop_blen -= len;
		}
		/* 最後のブレークチェック */
		if(checkEEVTDitem(&save_eevtitem, NULL, 0)) {
#if 0
			fprintf(out, "EEVT,%d,%d,%d,%s,%s\n", 
					eith.service_id,
					eitb.event_id,
					save_eevtitem.descriptor_number,
					save_eevtitem.item_description,
					save_eevtitem.item);
#endif
		}
	}

	return ;
}

