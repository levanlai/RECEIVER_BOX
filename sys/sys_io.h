#ifndef __SYS_IO_H__
#define __SYS_IO_H__

#define IO_DIR_INPUT            0
#define IO_DIR_OUTPUT           1
#define IO_PORT_DIR(x)          (P0DIRPORT + (x))
#define IO_PORT_VAL(x)          (P0VALPORT + (x))

void sys_io_init(void);
void sys_timer0_init(void);
void set_Mute_value(WORD value);
#endif /* __SYS_H__ */