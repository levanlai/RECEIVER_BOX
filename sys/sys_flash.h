#ifndef __SYS_FLASH_H__
#define __SYS_FLASH_H__


void mpspi_init(WORD clockdiv);
int mpspi_read_jedecid(PDWORD data);
int mpspi_read_word(DWORD addr, PWORD data);
int mpspi_read_dword(DWORD addr, PDWORD data);
int mpspi_read_words(DWORD addr, PWORD data, WORD nwords);
int mpspi_read_status_reg1(PWORD data);
int mpspi_read_status_reg2(PWORD data);
int mpspi_write_status_reg(PWORD data);
int mpspi_write_enable(void);
int mpspi_write_disable(void);
/**
 * @brief mpspi_sector_erase erase sectore 4k bytes (2k words)
 * mpspi_write_enable have to run first before run this cmd
 * 
 * @param page_addr 
 * @return int 
 */
int mpspi_sector_erase(DWORD page_addr);
/**
 * @brief mpspi_page_program write data to flash (max page 256 bytes(128 words))
 * mpspi_write_enable have to run first before run this cmd
 * 
 * @param addr 
 * @param data  
 * @param nwords number of words
 * @return int 
 */
int mpspi_page_program(DWORD addr, PWORD data, WORD nwords);

#endif /* __SYS_FLASH */