#include <system.h>
#include <trace.h>
#include "sys_flash.h"

#define MPSPI_FLASH_BASE_ADDR          0xC0000000
#define MPSPI_STD_CMD_READ             0x03
#define MPSPI_STD_CMD_READ_STATUS      0x05
#define MPSPI_STD_CMD_READ_STATUS2     0x35
#define MPSPI_STD_CMD_WRITE_ENABLE     0x06
#define MPSPI_STD_CMD_WRITE_DISABLE    0x04
#define MPSPI_STD_CMD_WRITE_STATUS     0x01
#define MPSPI_STD_CMD_PAGE_PROGRAM     0x02
#define MPSPI_STD_CMD_SECTOR4k_ERASE   0x20
#define MPSPI_STD_CMD_BLOCK32k_ERASE   0x52
#define MPSPI_STD_CMD_BLOCK64k_ERASE   0xD8
#define MPSPI_STD_CMD_JEDEC_ID         0x9F

#define MPSPI_6B_CLOCK_DIV_MASK        0x0FFF
#define MPSPI_6B_HD_MASK               1 << 15
#define MPSPI_6B_PORT                  QSPIGP_CONFIGURATION3_PORT

#define MPSPI_6A_NB_DATA_BITS_MASK     0x001F
#define MPSPI_6A_NB_CMD_MASK           1 << 7
#define MPSPI_6A_MUL_BURST_MASK        1 << 10
#define MPSPI_6A_DOUBLE_QSPI_MODE_MASK 1 << 11
#define MPSPI_6A_CYCLES_2_CMD_SHIFT    12
#define MPSPI_6A_CYCLES_2_CMD_MASK     7 << MPSPI_6A_CYCLES_2_CMD_SHIFT

#define MPSPI_6A_STATUS_BIT_SHIFT      15
#define MPSPI_6A_STATUS_BIT_MASK       1 << MPSPI_6A_STATUS_BIT_SHIFT
#define MPSPI_6A_PORT                  QSPIGP_CONFIGURATION2_PORT

#define MPSPI_69_QADDR_MASK            1 << 0 
#define MPSPI_69_NB_ADDR_SHIFT         1
#define MPSPI_69_NB_ADDR_MASK          0x07 << MPSPI_69_NB_ADDR_SHIFT
#define MPSPI_69_NB_DUMP_CL_SHIFT      4
#define MPSPI_69_NB_DUMP_CL_MASK       0x0F << MPSPI_69_NB_DUMP_CL_SHIFT
#define MPSPI_69_QDATA_MASK            1 << 8
#define MPSPI_69_QCMD_MASK             1 << 11
#define MPSPI_69_MODE_SHIFT            12
#define MPSPI_69_MODE_MASK             0x0F << MPSPI_69_MODE_SHIFT
#define MPSPI_69_PORT                  QSPIGP_CONFIGURATION1_PORT


#define MPSPI_68_RD_CMD_SHIFT           0
#define MPSPI_68_RD_CMD_MASK            0x00FF
#define MPSPI_68_WR_CMD_SHIFT           8
#define MPSPI_68_WR_CMD_MASK            0xFF00
#define MPSPI_68_PORT                   QSPIGP_CONFIGURATION0_PORT

void mpspi_init(WORD clockdiv)
{
    //WORD rw;
    // REG 0x6B //
    // clock div spi clock //
    _andio(MPSPI_6B_PORT, ~MPSPI_6B_CLOCK_DIV_MASK);
    _orio(MPSPI_6B_PORT, clockdiv & MPSPI_6B_CLOCK_DIV_MASK);
	// rw = _rdio(MPSPI_6B_PORT);
	// TRACE("mpgspi_init 6B[%x]", rw);
    // REG 0x6A //
    // number of cmd send //
    _andio(MPSPI_6A_PORT, ~(MPSPI_6A_DOUBLE_QSPI_MODE_MASK| 
                MPSPI_6A_MUL_BURST_MASK| MPSPI_6A_NB_DATA_BITS_MASK));
    _orio(MPSPI_6A_PORT, MPSPI_6A_NB_CMD_MASK | (0x7 << MPSPI_6A_CYCLES_2_CMD_SHIFT));
	// rw = _rdio(MPSPI_6A_PORT);
	// TRACE("mpgspi_init 6A[%x]", rw);

    // REG 0x69 //
    _andio(MPSPI_69_PORT, ~(MPSPI_69_QCMD_MASK| 
                MPSPI_69_QDATA_MASK| MPSPI_69_QADDR_MASK| MPSPI_69_NB_DUMP_CL_MASK| MPSPI_69_NB_ADDR_MASK));
	// rw = _rdio(MPSPI_69_PORT);
	// TRACE("mpgspi_init 69[%x]", rw);
    // rw = _rdio(MPSPI_68_PORT);
	// TRACE("mpgspi_init 68[%x]", rw);
}

int mpspi_read_jedecid(PDWORD data)
{
    //WORD rw;
    WORD _data;
    WORD status = _rdio(MPSPI_6A_PORT);
    //TRACE("mpspi_read_jedecid 6A[%x]", status);

    // if((status & MPSPI_6A_STATUS_BIT_MASK) == MPSPI_6A_STATUS_BIT_MASK)
    // {
    //     TRACE("mpspi_read_jedecid busy", status);
    //     return -1;
    // }

    while ((status & MPSPI_6A_STATUS_BIT_MASK) == MPSPI_6A_STATUS_BIT_MASK)
    {

        TRACE("mpspi_read_dword busy", status);
        status = _rdio(MPSPI_6A_PORT);
    } 

    // REG 0x68 //
    // set cmd spi //
    _andio(MPSPI_68_PORT, ~MPSPI_68_RD_CMD_MASK);
    _orio(MPSPI_68_PORT, ((MPSPI_STD_CMD_JEDEC_ID << MPSPI_68_RD_CMD_SHIFT) & MPSPI_68_RD_CMD_MASK));
	// rw = _rdio(MPSPI_68_PORT);
	//TRACE("mpspi_read_jedecid 68[%x]", rw);
    // REG 0x69 //
    // set number of addr byte //
    _andio(MPSPI_69_PORT,  ~MPSPI_69_NB_ADDR_MASK);
    _orio(MPSPI_69_PORT, ((0 << MPSPI_69_NB_ADDR_SHIFT ) & MPSPI_69_NB_ADDR_MASK));
	// rw = _rdio(MPSPI_69_PORT);
	//TRACE("mpspi_read_jedecid 69[%x]", rw);

    // REG 0x69 //
    // set number of addr byte //

    _andio(MPSPI_6A_PORT, ~MPSPI_6A_NB_DATA_BITS_MASK);
    _orio(MPSPI_6A_PORT, (0 & MPSPI_6A_NB_DATA_BITS_MASK));
    _orio(MPSPI_6A_PORT, MPSPI_6A_MUL_BURST_MASK);
	// rw = _rdio(MPSPI_6A_PORT);
	//TRACE("mpspi_read_jedecid 69[%x]", rw);
    _data = _rdxtmemEx(MPSPI_FLASH_BASE_ADDR);
    //TRACE("mpspi_read_jedecid 1 data[%x]", _data);
    *data = (DWORD)_data << 8;
    _data = _rdxtmemEx(MPSPI_FLASH_BASE_ADDR);
    //TRACE("mpspi_read_jedecid 2 data[%x]", _data);

    *data |= _data >> 8;
    _andio(MPSPI_6A_PORT, ~(MPSPI_6A_MUL_BURST_MASK | MPSPI_6A_NB_DATA_BITS_MASK));
	// rw = _rdio(MPSPI_6A_PORT);
	//TRACE("mpspi_read_jedecid 69[%x]", rw);
    return 0;
}

int mpspi_read_word(DWORD addr, PWORD data)
{
    //WORD rw;
    WORD status = _rdio(MPSPI_6A_PORT);
    // TRACE("mpspi_read_byte 6A[%x]", status);

    // if((status & MPSPI_6A_STATUS_BIT_MASK) == MPSPI_6A_STATUS_BIT_MASK)
    // {
    //     TRACE("mpspi_read_byte busy", status);
    //     return -1;
    // }

    while ((status & MPSPI_6A_STATUS_BIT_MASK) == MPSPI_6A_STATUS_BIT_MASK)
    {

        TRACE("mpspi_read_dword busy", status);
        status = _rdio(MPSPI_6A_PORT);
    } 

    // REG 0x68 //
    // set cmd spi //
    _andio(MPSPI_68_PORT, ~MPSPI_68_RD_CMD_MASK);
    _orio(MPSPI_68_PORT, ((MPSPI_STD_CMD_READ << MPSPI_68_RD_CMD_SHIFT) & MPSPI_68_RD_CMD_MASK));
	// rw = _rdio(MPSPI_68_PORT);
	// TRACE("mpspi_read_byte 68[%x]", rw);
    // REG 0x69 //
    // set number of addr byte //
    _andio(MPSPI_69_PORT,  ~MPSPI_69_NB_ADDR_MASK);
    _orio(MPSPI_69_PORT, ((3 << MPSPI_69_NB_ADDR_SHIFT ) & MPSPI_69_NB_ADDR_MASK));
	// rw = _rdio(MPSPI_69_PORT);
	// TRACE("mpspi_read_byte 69[%x]", rw);

    // REG 0x69 //
    // set number of addr byte //
    // _orio(MPSPI_6A_PORT, MPSPI_6A_MUL_BURST_MASK);
	// rw = _rdio(MPSPI_6A_PORT);
	// TRACE("mpspi_read_byte 69[%x]", rw);

    // TRACE("mpspi_read_byte addr [%x]", addr);
    *data = _rdxtmemEx(MPSPI_FLASH_BASE_ADDR + addr);

    // TRACE("mpspi_read_byte addr[%x]", addr);
    // TRACE("mpspi_read_byte data[%x]", *data);

    // _andio(MPSPI_6A_PORT, ~MPSPI_6A_MUL_BURST_MASK);
	// rw = _rdio(MPSPI_6A_PORT);
	// TRACE("mpspi_read_byte 69[%x]", rw);

    return 0;
}

int mpspi_read_dword(DWORD addr, PDWORD data)
{
    //WORD rw;
    WORD status = _rdio(MPSPI_6A_PORT);
    // TRACE("mpspi_read_byte 6A[%x]", status);

    // if((status & MPSPI_6A_STATUS_BIT_MASK) == MPSPI_6A_STATUS_BIT_MASK)
    // {
    //     TRACE("mpspi_read_byte busy", status);
    //     return -1;
    // }

    while ((status & MPSPI_6A_STATUS_BIT_MASK) == MPSPI_6A_STATUS_BIT_MASK)
    {

        TRACE("mpspi_read_dword busy", status);
        status = _rdio(MPSPI_6A_PORT);
    } 
    // REG 0x68 //
    // set cmd spi //
    _andio(MPSPI_68_PORT, ~MPSPI_68_RD_CMD_MASK);
    _orio(MPSPI_68_PORT, ((MPSPI_STD_CMD_READ << MPSPI_68_RD_CMD_SHIFT) & MPSPI_68_RD_CMD_MASK));
	// rw = _rdio(MPSPI_68_PORT);
	// TRACE("mpspi_read_byte 68[%x]", rw);
    // REG 0x69 //
    // set number of addr byte //
    _andio(MPSPI_69_PORT,  ~MPSPI_69_NB_ADDR_MASK);
    _orio(MPSPI_69_PORT, ((3 << MPSPI_69_NB_ADDR_SHIFT ) & MPSPI_69_NB_ADDR_MASK));
	// rw = _rdio(MPSPI_69_PORT);
	// TRACE("mpspi_read_byte 69[%x]", rw);

    // REG 0x69 //
    // set number of addr byte //
    // _orio(MPSPI_6A_PORT, MPSPI_6A_MUL_BURST_MASK);
	// rw = _rdio(MPSPI_6A_PORT);
	// TRACE("mpspi_read_byte 69[%x]", rw);

    // TRACE("mpspi_read_dword addr [%x]", addr);
    *data = _rdxtmem32Ex(MPSPI_FLASH_BASE_ADDR + addr);

    // TRACE("mpspi_read_byte addr[%x]", addr);
    // TRACE("mpspi_read_byte data[%x]", *data);

    // _andio(MPSPI_6A_PORT, ~MPSPI_6A_MUL_BURST_MASK);
	// rw = _rdio(MPSPI_6A_PORT);
	// TRACE("mpspi_read_byte 69[%x]", rw);

    return 0;
}

int mpspi_read_words(DWORD addr, PWORD data, WORD nwords)
{
    //WORD rw;
    WORD i;
    PWORD pData = data;
    WORD status = _rdio(MPSPI_6A_PORT);
    // TRACE("mpspi_read_byte 6A[%x]", status);

    // if((status & MPSPI_6A_STATUS_BIT_MASK) == MPSPI_6A_STATUS_BIT_MASK)
    // {
    //     TRACE("mpspi_read_words busy", status);
    //     return -1;
    // }

    while ((status & MPSPI_6A_STATUS_BIT_MASK) == MPSPI_6A_STATUS_BIT_MASK)
    {

        TRACE("mpspi_read_words busy", status);
        status = _rdio(MPSPI_6A_PORT);
    } 

    // REG 0x68 //
    // set cmd spi //
    _andio(MPSPI_68_PORT, ~MPSPI_68_RD_CMD_MASK);
    _orio(MPSPI_68_PORT, ((MPSPI_STD_CMD_READ << MPSPI_68_RD_CMD_SHIFT) & MPSPI_68_RD_CMD_MASK));
	// rw = _rdio(MPSPI_68_PORT);
	// TRACE("mpspi_read_words 68[%x]", rw);
    // REG 0x69 //
    // set number of addr byte //
    _andio(MPSPI_69_PORT,  ~MPSPI_69_NB_ADDR_MASK);


    _orio(MPSPI_69_PORT, ((3 << MPSPI_69_NB_ADDR_SHIFT ) & MPSPI_69_NB_ADDR_MASK));
	// rw = _rdio(MPSPI_69_PORT);
	// TRACE("mpspi_read_words 69[%x]", rw);

    // REG 0x69 //
    // set number of addr byte //
    _orio(MPSPI_6A_PORT, MPSPI_6A_MUL_BURST_MASK);
	// rw = _rdio(MPSPI_6A_PORT);
	// TRACE("mpspi_read_words 69[%x]", rw);

    //TRACE("mpspi_read_words addr [%x]", addr);
    for (i = 0; i < nwords + 1; i++)
    {
        if(i==0) // read align address dump word
        {
            *pData = _rdxtmemEx(MPSPI_FLASH_BASE_ADDR + addr - 1);
            //TRACE("mpspi_read_byte dump data[%x]", *(pData));
        }
        else
        {
            *pData++ = _rdxtmemEx(MPSPI_FLASH_BASE_ADDR + addr);
            //TRACE("mpspi_read_byte data[%x]", *(pData - 1));
        }
    }




    _andio(MPSPI_6A_PORT, ~(MPSPI_6A_MUL_BURST_MASK | MPSPI_6A_NB_DATA_BITS_MASK));
	// rw = _rdio(MPSPI_6A_PORT);
	// TRACE("mpspi_read_words 6A[%x]", rw);


    return 0;
}





int mpspi_read_status_reg1(PWORD data)
{
    //WORD rw;
    WORD status = _rdio(MPSPI_6A_PORT);
    // TRACE("mpspi_read_status_reg1 6A[%x]", status);

    // if((status & MPSPI_6A_STATUS_BIT_MASK) == MPSPI_6A_STATUS_BIT_MASK)
    // {
    //     TRACE("mpspi_read_status_reg1 busy", status);
    //     return -1;
    // }
    while ((status & MPSPI_6A_STATUS_BIT_MASK) == MPSPI_6A_STATUS_BIT_MASK)
    {

        TRACE("mpspi_read_status_reg1 busy", status);
        status = _rdio(MPSPI_6A_PORT);
    } 

    // REG 0x68 //
    // set cmd spi //
    _andio(MPSPI_68_PORT, ~MPSPI_68_RD_CMD_MASK);
    _orio(MPSPI_68_PORT, ((MPSPI_STD_CMD_READ_STATUS << MPSPI_68_RD_CMD_SHIFT) & MPSPI_68_RD_CMD_MASK));
	// rw = _rdio(MPSPI_68_PORT);
	// TRACE("mpspi_read_status_reg1 68[%x]", rw);
    // REG 0x69 //
    // set number of addr byte //
    _andio(MPSPI_69_PORT,  ~MPSPI_69_NB_ADDR_MASK);
    _orio(MPSPI_69_PORT, ((0 << MPSPI_69_NB_ADDR_SHIFT ) & MPSPI_69_NB_ADDR_MASK));
	// rw = _rdio(MPSPI_69_PORT);
	// TRACE("mpspi_read_status_reg1 69[%x]", rw);

    // REG 0x69 //
    // set number of data byte //
    _andio(MPSPI_6A_PORT, ~MPSPI_6A_NB_DATA_BITS_MASK);
    _orio(MPSPI_6A_PORT, (8 & MPSPI_6A_NB_DATA_BITS_MASK));
	// rw = _rdio(MPSPI_6A_PORT);
	// TRACE("mpspi_read_status_reg1 69[%x]", rw);
    *data = _rdxtmemEx(MPSPI_FLASH_BASE_ADDR);

    // TRACE("mpspi_read_status_reg1 data[%x]", *data);

    _andio(MPSPI_6A_PORT, ~MPSPI_6A_NB_DATA_BITS_MASK);
	// rw = _rdio(MPSPI_6A_PORT);
	// TRACE("mpspi_read_status_reg1 69[%x]", rw);

    return 0;
}

int mpspi_read_status_reg2(PWORD data)
{
    //WORD rw;
    WORD status = _rdio(MPSPI_6A_PORT);
    // TRACE("mpspi_read_status_reg2 6A[%x]", status);

    // if((status & MPSPI_6A_STATUS_BIT_MASK) == MPSPI_6A_STATUS_BIT_MASK)
    // {
    //     TRACE("mpspi_read_status_reg2 busy", status);
    //     return -1;
    // }
    while ((status & MPSPI_6A_STATUS_BIT_MASK) == MPSPI_6A_STATUS_BIT_MASK)
    {

        TRACE("mpspi_read_status_reg2 busy", status);
        status = _rdio(MPSPI_6A_PORT);
    } 
    // REG 0x68 //
    // set cmd spi //
    _andio(MPSPI_68_PORT, ~MPSPI_68_RD_CMD_MASK);
    _orio(MPSPI_68_PORT, ((MPSPI_STD_CMD_READ_STATUS2 << MPSPI_68_RD_CMD_SHIFT) & MPSPI_68_RD_CMD_MASK));
	// rw = _rdio(MPSPI_68_PORT);
	// TRACE("mpspi_read_status_reg2 68[%x]", rw);
    // REG 0x69 //
    // set number of addr byte //
    _andio(MPSPI_69_PORT,  ~MPSPI_69_NB_ADDR_MASK);
    _orio(MPSPI_69_PORT, ((0 << MPSPI_69_NB_ADDR_SHIFT ) & MPSPI_69_NB_ADDR_MASK));
	// rw = _rdio(MPSPI_69_PORT);
	// TRACE("mpspi_read_status_reg2 69[%x]", rw);

    // REG 0x69 //
    // set number of addr byte //
    _andio(MPSPI_6A_PORT, ~MPSPI_6A_NB_DATA_BITS_MASK);
    _orio(MPSPI_6A_PORT, (8 & MPSPI_6A_NB_DATA_BITS_MASK));
	// rw = _rdio(MPSPI_6A_PORT);
	// TRACE("mpspi_read_status_reg2 69[%x]", rw);
    *data = _rdxtmemEx(MPSPI_FLASH_BASE_ADDR);

    // TRACE("mpspi_read_status_reg2 data[%x]", *data);

    _andio(MPSPI_6A_PORT, ~MPSPI_6A_NB_DATA_BITS_MASK);
	// rw = _rdio(MPSPI_6A_PORT);
	// TRACE("mpspi_read_status_reg2 69[%x]", rw);
    
    return 0;
}

int mpspi_write_status_reg(PWORD data)
{
    //WORD rw;
    WORD status = _rdio(MPSPI_6A_PORT);
    //TRACE("mpspi_write_status_reg 6A[%x]", status);
    while ((status & MPSPI_6A_STATUS_BIT_MASK) == MPSPI_6A_STATUS_BIT_MASK)
    {

        TRACE("mpspi_write_status_reg busy", status);
        status = _rdio(MPSPI_6A_PORT);
    } 
    // REG 0x68 //
    // set cmd spi //
    _andio(MPSPI_68_PORT, ~MPSPI_68_RD_CMD_MASK);
    _orio(MPSPI_68_PORT, ((MPSPI_STD_CMD_WRITE_STATUS << MPSPI_68_RD_CMD_SHIFT) & MPSPI_68_RD_CMD_MASK));
	// rw = _rdio(MPSPI_68_PORT);
	//TRACE("mpspi_write_status_reg 68[%x]", rw);
    // REG 0x69 //
    // set number of addr byte //
    _andio(MPSPI_69_PORT,  ~MPSPI_69_NB_ADDR_MASK);
    _orio(MPSPI_69_PORT, ((0 << MPSPI_69_NB_ADDR_SHIFT ) & MPSPI_69_NB_ADDR_MASK));
	// rw = _rdio(MPSPI_69_PORT);
	//TRACE("mpspi_write_status_reg 69[%x]", rw);

    // REG 0x69 //
    // set number of data byte //
    _andio(MPSPI_6A_PORT, ~MPSPI_6A_NB_DATA_BITS_MASK);
    _orio(MPSPI_6A_PORT, (0 & MPSPI_6A_NB_DATA_BITS_MASK));
	// rw = _rdio(MPSPI_6A_PORT);
	//TRACE("mpspi_write_status_reg 69[%x]", rw);
    *data = _rdxtmemEx(MPSPI_FLASH_BASE_ADDR);

    //TRACE("mpspi_write_status_reg data[%x]", *data);

    _andio(MPSPI_6A_PORT, ~MPSPI_6A_NB_DATA_BITS_MASK);
	// rw = _rdio(MPSPI_6A_PORT);
	// TRACE("mpspi_write_status_reg 69[%x]", rw);
    return 0;
}

int mpspi_write_enable(void)
{
    //WORD rw;
    WORD status = _rdio(MPSPI_6A_PORT);

    // TRACE("mpspi_write_enable 6A[%x]", status);

    while ((status & MPSPI_6A_STATUS_BIT_MASK) == MPSPI_6A_STATUS_BIT_MASK)
    {

        TRACE("mpspi_write_enable busy", status);
        status = _rdio(MPSPI_6A_PORT);
    } 

    // REG 0x68 //
    // set cmd spi //
    _andio(MPSPI_68_PORT, ~MPSPI_68_WR_CMD_MASK);
    _orio(MPSPI_68_PORT, ((MPSPI_STD_CMD_WRITE_ENABLE << MPSPI_68_WR_CMD_SHIFT) & MPSPI_68_WR_CMD_MASK));

	// rw = _rdio(MPSPI_68_PORT);
	// TRACE("mpspi_write_enable 68[%x]", rw);
    // REG 0x69 //
    // set number of addr byte //
    _andio(MPSPI_69_PORT,  ~MPSPI_69_NB_ADDR_MASK);
    _orio(MPSPI_69_PORT, ((0 << MPSPI_69_NB_ADDR_SHIFT ) & MPSPI_69_NB_ADDR_MASK));
	// rw = _rdio(MPSPI_69_PORT);
	// TRACE("mpspi_write_enable 69[%x]", rw);

    _wrxtmemEx(MPSPI_FLASH_BASE_ADDR, 0);

    return 0;
}

int mpspi_write_disable(void)
{
    //WORD rw;
    WORD status = _rdio(MPSPI_6A_PORT);

    // TRACE("mpspi_write_disable 6A[%x]", status);

    while ((status & MPSPI_6A_STATUS_BIT_MASK) == MPSPI_6A_STATUS_BIT_MASK)
    {
        TRACE("mpspi_write_disable busy", status);
        status = _rdio(MPSPI_6A_PORT);
    } 


    // REG 0x68 //
    // set cmd spi //
    _andio(MPSPI_68_PORT, ~MPSPI_68_WR_CMD_MASK);
    _orio(MPSPI_68_PORT, ((MPSPI_STD_CMD_WRITE_DISABLE << MPSPI_68_WR_CMD_SHIFT) & MPSPI_68_WR_CMD_MASK));
	// rw = _rdio(MPSPI_68_PORT);
	// TRACE("mpspi_write_disable 68[%x]", rw);
    // REG 0x69 //
    // set number of addr byte //
    _andio(MPSPI_69_PORT,  ~MPSPI_69_NB_ADDR_MASK);
    _orio(MPSPI_69_PORT, ((0 << MPSPI_69_NB_ADDR_SHIFT ) & MPSPI_69_NB_ADDR_MASK));
	// rw = _rdio(MPSPI_69_PORT);
	// TRACE("mpspi_write_disable 69[%x]", rw);

    _wrxtmemEx(MPSPI_FLASH_BASE_ADDR, 0);


    return 0;
}

int mpspi_sector_erase(DWORD page_addr)
{
    WORD rw;
    WORD status = _rdio(MPSPI_6A_PORT);
    // TRACE("mpspi_sector_erase 6A[%x]", status);

    while ((status & MPSPI_6A_STATUS_BIT_MASK) == MPSPI_6A_STATUS_BIT_MASK)
    {

        TRACE("mpspi_sector_erase busy", status);
        status = _rdio(MPSPI_6A_PORT);
    } 

    // REG 0x68 //
    // set cmd spi //
    _andio(MPSPI_68_PORT, ~MPSPI_68_WR_CMD_MASK);
    _orio(MPSPI_68_PORT, ((MPSPI_STD_CMD_SECTOR4k_ERASE << MPSPI_68_WR_CMD_SHIFT) & MPSPI_68_WR_CMD_MASK));
	// rw = _rdio(MPSPI_68_PORT);
	// TRACE("mpspi_sector_erase 68[%x]", rw);
    // REG 0x69 //
    // set number of addr byte //
    _andio(MPSPI_69_PORT,  ~MPSPI_69_NB_ADDR_MASK);
    _orio(MPSPI_69_PORT, ((3 << MPSPI_69_NB_ADDR_SHIFT ) & MPSPI_69_NB_ADDR_MASK));
	// rw = _rdio(MPSPI_69_PORT);
	// TRACE("mpspi_sector_erase 69[%x]", rw);

    // REG 0x69 //
    // set number of addr byte //
    // _orio(MPSPI_6A_PORT, MPSPI_6A_MUL_BURST_MASK);
	// rw = _rdio(MPSPI_6A_PORT);
	// TRACE("mpspi_read_byte 69[%x]", rw);
    _wrxtmemEx(MPSPI_FLASH_BASE_ADDR + page_addr, 0);

    // TRACE("mpspi_read_byte addr[%x]", addr);
    // TRACE("mpspi_read_byte data[%x]", *data);

    // _andio(MPSPI_6A_PORT, ~MPSPI_6A_MUL_BURST_MASK);
	// rw = _rdio(MPSPI_6A_PORT);
	// TRACE("mpspi_read_byte 69[%x]", rw);

	//TRACE("mpspi_sector_erase page_addr[%x]", page_addr);

    do{
        mpspi_read_status_reg1(&rw);
    } while (rw & 0x01 == 0x01);

    return 0;
}

int mpspi_page_program(DWORD addr, PWORD data, WORD nwords)
{
    WORD i,rw;
    PWORD pData = data;
    WORD status = _rdio(MPSPI_6A_PORT);
    //TRACE("mpspi_page_program 6A[%x]", status);

    if(pData == NULL || nwords <= 0)
    {
        TRACE("mpspi_page_program data null or data length zero", status);
        return -2;
    }

    while ((status & MPSPI_6A_STATUS_BIT_MASK) == MPSPI_6A_STATUS_BIT_MASK)
    {

        TRACE("mpspi_page_program busy", status);
        status = _rdio(MPSPI_6A_PORT);
    } 

    // REG 0x68 //
    // set cmd spi //
    _andio(MPSPI_68_PORT, ~MPSPI_68_WR_CMD_MASK);
    _orio(MPSPI_68_PORT, ((MPSPI_STD_CMD_PAGE_PROGRAM << MPSPI_68_WR_CMD_SHIFT) & MPSPI_68_WR_CMD_MASK));
	// rw = _rdio(MPSPI_68_PORT);
	// TRACE("mpspi_page_program 68[%x]", rw);
    // REG 0x69 //
    // set number of addr byte //
    _andio(MPSPI_69_PORT,  ~MPSPI_69_NB_ADDR_MASK);
    _orio(MPSPI_69_PORT, ((3 << MPSPI_69_NB_ADDR_SHIFT ) & MPSPI_69_NB_ADDR_MASK));
	// rw = _rdio(MPSPI_69_PORT);
	// TRACE("mpspi_page_program 69[%x]", rw);

    // REG 0x69 //
    // set number of addr byte //
    _orio(MPSPI_6A_PORT, MPSPI_6A_MUL_BURST_MASK);
    _andio(MPSPI_6A_PORT, ~MPSPI_6A_NB_DATA_BITS_MASK);
    _orio(MPSPI_6A_PORT, (16 & MPSPI_6A_NB_DATA_BITS_MASK));
	// rw = _rdio(MPSPI_6A_PORT);
	// TRACE("mpspi_page_program 6A[%x]", rw);
	// TRACE("mpspi_page_program at [%x]", addr);
	// TRACE("mpspi_page_program nwords ", nwords);
    for (i = 0; i < nwords; i++)
    {
        _wrxtmemEx(MPSPI_FLASH_BASE_ADDR + addr, pData[i]); 
        // TRACE("mpspi_page_program pData at", i);
        // TRACE("mpspi_page_program pData val", pData[i]);
    }

    // do {
    //     rw = _rdio(MPSPI_6B_PORT);
    // }while((rw >> 15) != 0);

    _andio(MPSPI_6A_PORT, ~(MPSPI_6A_MUL_BURST_MASK | MPSPI_6A_NB_DATA_BITS_MASK));
	// rw = _rdio(MPSPI_6A_PORT);
	// TRACE("mpspi_page_program 6A[%x]", rw);

    do {
        mpspi_read_status_reg1(&rw);
        //TRACE("mpspi_read_status_reg1 data[%x]", rw);
    }while(rw & 0x01 == 0x01);
    
    return 0;
}
