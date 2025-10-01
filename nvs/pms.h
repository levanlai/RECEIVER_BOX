#ifndef __PMS_H__
#define __PMS_H__


int pms_init(void);
int pms_get_word(WORD key, WORD *val);
int pms_set_word(WORD key, WORD val);
int pms_get_dword(WORD key, DWORD *val);
int pms_set_dword(WORD key, DWORD val);
int pms_get_bufs(WORD key, WORD *val, WORD nwords);
int pms_set_bufs(WORD key, WORD *val, WORD nwords);
int pms_delete_bufs(WORD key);

#endif /* __PMS_H__ */