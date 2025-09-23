#include <system.h>
#include "nvs.h"
#include "../sys/sys_flash.h"


static const WORD CrcCCITTTable[256] = {
	0x0000, 0x1021, 0x2042, 0x3063, 0x4084, 0x50A5, 0x60C6, 0x70E7,
	0x8108, 0x9129, 0xA14A, 0xB16B, 0xC18C, 0xD1AD, 0xE1CE, 0xF1EF,
	0x1231, 0x0210, 0x3273, 0x2252, 0x52B5, 0x4294, 0x72F7, 0x62D6,
	0x9339, 0x8318, 0xB37B, 0xA35A, 0xD3BD, 0xC39C, 0xF3FF, 0xE3DE,
	0x2462, 0x3443, 0x0420, 0x1401, 0x64E6, 0x74C7, 0x44A4, 0x5485,
	0xA56A, 0xB54B, 0x8528, 0x9509, 0xE5EE, 0xF5CF, 0xC5AC, 0xD58D,
	0x3653, 0x2672, 0x1611, 0x0630, 0x76D7, 0x66F6, 0x5695, 0x46B4,
	0xB75B, 0xA77A, 0x9719, 0x8738, 0xF7DF, 0xE7FE, 0xD79D, 0xC7BC,
	0x48C4, 0x58E5, 0x6886, 0x78A7, 0x0840, 0x1861, 0x2802, 0x3823,
	0xC9CC, 0xD9ED, 0xE98E, 0xF9AF, 0x8948, 0x9969, 0xA90A, 0xB92B,
	0x5AF5, 0x4AD4, 0x7AB7, 0x6A96, 0x1A71, 0x0A50, 0x3A33, 0x2A12,
	0xDBFD, 0xCBDC, 0xFBBF, 0xEB9E, 0x9B79, 0x8B58, 0xBB3B, 0xAB1A,
	0x6CA6, 0x7C87, 0x4CE4, 0x5CC5, 0x2C22, 0x3C03, 0x0C60, 0x1C41,
	0xEDAE, 0xFD8F, 0xCDEC, 0xDDCD, 0xAD2A, 0xBD0B, 0x8D68, 0x9D49,
	0x7E97, 0x6EB6, 0x5ED5, 0x4EF4, 0x3E13, 0x2E32, 0x1E51, 0x0E70,
	0xFF9F, 0xEFBE, 0xDFDD, 0xCFFC, 0xBF1B, 0xAF3A, 0x9F59, 0x8F78,
	0x9188, 0x81A9, 0xB1CA, 0xA1EB, 0xD10C, 0xC12D, 0xF14E, 0xE16F,
	0x1080, 0x00A1, 0x30C2, 0x20E3, 0x5004, 0x4025, 0x7046, 0x6067,
	0x83B9, 0x9398, 0xA3FB, 0xB3DA, 0xC33D, 0xD31C, 0xE37F, 0xF35E,
	0x02B1, 0x1290, 0x22F3, 0x32D2, 0x4235, 0x5214, 0x6277, 0x7256,
	0xB5EA, 0xA5CB, 0x95A8, 0x8589, 0xF56E, 0xE54F, 0xD52C, 0xC50D,
	0x34E2, 0x24C3, 0x14A0, 0x0481, 0x7466, 0x6447, 0x5424, 0x4405,
	0xA7DB, 0xB7FA, 0x8799, 0x97B8, 0xE75F, 0xF77E, 0xC71D, 0xD73C,
	0x26D3, 0x36F2, 0x0691, 0x16B0, 0x6657, 0x7676, 0x4615, 0x5634,
	0xD94C, 0xC96D, 0xF90E, 0xE92F, 0x99C8, 0x89E9, 0xB98A, 0xA9AB,
	0x5844, 0x4865, 0x7806, 0x6827, 0x18C0, 0x08E1, 0x3882, 0x28A3,
	0xCB7D, 0xDB5C, 0xEB3F, 0xFB1E, 0x8BF9, 0x9BD8, 0xABBB, 0xBB9A,
	0x4A75, 0x5A54, 0x6A37, 0x7A16, 0x0AF1, 0x1AD0, 0x2AB3, 0x3A92,
	0xFD2E, 0xED0F, 0xDD6C, 0xCD4D, 0xBDAA, 0xAD8B, 0x9DE8, 0x8DC9,
	0x7C26, 0x6C07, 0x5C64, 0x4C45, 0x3CA2, 0x2C83, 0x1CE0, 0x0CC1,
	0xEF1F, 0xFF3E, 0xCF5D, 0xDF7C, 0xAF9B, 0xBFBA, 0x8FD9, 0x9FF8,
	0x6E17, 0x7E36, 0x4E55, 0x5E74, 0x2E93, 0x3EB2, 0x0ED1, 0x1EF0
};

static WORD CRC16(WORD *Buf, WORD BufLen, WORD CRC)
{
	WORD i;
    WORD bBuf;
    WORD *pBuf = Buf;
	for(i = 0 ; i < BufLen; i++)
	{
        bBuf = (*(pBuf+i) >> 8) & 0x00FF;
		CRC = (CRC << 8) ^ CrcCCITTTable[((CRC >> 8) ^ bBuf) & 0x00FF];
        bBuf = *(pBuf+i) & 0x00FF;
        CRC = (CRC << 8) ^ CrcCCITTTable[((CRC >> 8) ^ bBuf) & 0x00FF];
	}
	return CRC;
}

static  void nvs_lock(struct nvs_fs *fs)
{
}

static  void nvs_unlock(struct nvs_fs *fs)
{
}

/* flash routines */
/* basic routines */
/* nvs_al_size returns size aligned to fs->write_block_size */
static WORD nvs_al_size(struct nvs_fs *fs, WORD len)
{
    return len;
}
/* end basic routines */

/* basic aligned flash write to nvs address */
static int nvs_flash_al_wrt(struct nvs_fs *fs, DWORD addr,
                             const void *data, WORD nwords)
{
    int rc = 0;
	DWORD offset;
	WORD nwords_program;
	WORD words_page;
    if (!nwords)
    {
        /* Nothing to write, avoid changing the flash protection */
        TRACE("nvs_flash_al_wrt length zero!!!", nwords);
        return 0;
    }

    offset = fs->offset;
    offset += fs->sector_size * (addr >> FLASH_ADDR_SECT_SHIFT);
    offset += addr & FLASH_ADDR_OFFS_MASK;


	nwords_program = 0;

	while(nwords_program < nwords)
	{
		
		words_page = MIN((FLASH_PAGE_SIZE - (offset & FLASH_PAGE_SIZE_MASK)), (nwords - nwords_program)) ;


		if (mpspi_write_enable() != 0)
		{
			TRACE("nvs_flash_al_wrt enable error %x!!!", offset);
			return -1;
		}	
		
	    rc = mpspi_page_program(offset, (WORD *)((WORD *)data + nwords_program), words_page);	

		if (rc != 0)
		{
			TRACE("nvs_flash_al_wrt program error %x!!!", offset);
			return -1;
		}

		offset += words_page;
		nwords_program += words_page;
	}
    // if (mpspi_write_disable() != 0)
    //     return -1;

	return rc;
}

/* basic flash read from nvs address */
static int nvs_flash_rd(struct nvs_fs *fs, DWORD addr, void *data, WORD nwords)
{
    DWORD offset;
	int ret;
    offset = fs->offset;
    offset += fs->sector_size * (addr >> FLASH_ADDR_SECT_SHIFT);
    offset += addr & FLASH_ADDR_OFFS_MASK;
    ret =  mpspi_read_words(offset, data, nwords);

	// TRACE("nvs_flash_rd %x!!!", offset);
	// for (i = 0; i < nwords; i++)
	// {
	// 	TRACE("", ((WORD *)data)[i]);
	// }
	return ret;
}

/* allocation entry write */
static int nvs_flash_ate_wrt(struct nvs_fs *fs,  struct nvs_ate *entry)
{
    WORD rc;

    rc = nvs_flash_al_wrt(fs, fs->ate_wra, ( void *)entry,
                          sizeof(struct nvs_ate));
    fs->ate_wra -= nvs_al_size(fs, sizeof(struct nvs_ate));

    return rc;
}

/* data write */
static int nvs_flash_data_wrt(struct nvs_fs *fs,  void *data, WORD nwords)
{
    int rc;

    rc = nvs_flash_al_wrt(fs, fs->data_wra, data, nwords);
    fs->data_wra += nvs_al_size(fs, nwords);

    return rc;
}

/* flash ate read */
static int nvs_flash_ate_rd(struct nvs_fs *fs, DWORD addr, struct nvs_ate *entry)
{
	int rc;
	// DWORD offset;
	// int i;
    rc = nvs_flash_rd(fs, addr, entry, sizeof(struct nvs_ate));

	// TRACE("ate at %x ", addr);
	// TRACE("ate rc %x ", rc);
	// traceVal = entry->id;
	// TRACE("id %x", traceVal);

	// traceVal = entry->offset;
	// TRACE("offset %x", traceVal);

	// traceVal = entry->len;
	// TRACE("len %x", traceVal);

	// traceVal = entry->part;
	// TRACE("part %x", traceVal);

	// traceVal = entry->crc16;
	// TRACE("crc16 %x", traceVal);

    // offset = fs->offset;
    // offset += fs->sector_size * (addr >> FLASH_ADDR_SECT_SHIFT);
    // offset += addr & FLASH_ADDR_OFFS_MASK;	
	// TRACE("nvs_flash_ate_rd %x!!!", offset);
	// for (i = 0; i < sizeof(struct nvs_ate); i++)
	// {
	// 	TRACE("", ((WORD *)entry)[i]);
	// }
	return rc;

}

/* end of basic flash routines */

/* advanced flash routines */

/* nvs_flash_block_cmp compares the data in flash at addr to data
 * in blocks of size FLASH_BLOCK_SIZE aligned to fs->write_block_size
 * returns 0 if equal, 1 if not equal, errcode if error
 */
static int nvs_flash_block_cmp(struct nvs_fs *fs, DWORD addr, const void *data,
				WORD nwords)
{
	WORD *data16 = (WORD *)data;
	int rc;
	int bytes_remain = nwords;
	WORD bytes_to_cmp, block_size;
	WORD buf[FLASH_BLOCK_SIZE];
	block_size = FLASH_BLOCK_SIZE;



	while (bytes_remain) {
		bytes_to_cmp = MIN(block_size, bytes_remain);
		rc = nvs_flash_rd(fs, addr, buf, bytes_to_cmp);
		if (rc) {
			return rc;
		}
		rc = _memcmp((void *)data16, (void *)buf, bytes_to_cmp);
		if (rc) {
			return 1;
		}
		bytes_remain -= bytes_to_cmp;
		addr += bytes_to_cmp;
		data16 += bytes_to_cmp;
	}
	return 0;
}

/* nvs_flash_cmp_const compares the data in flash at addr to a constant
 * value. returns 0 if all data in flash is equal to value, 1 if not equal,
 * errcode if error
 */
static int nvs_flash_cmp_const(struct nvs_fs *fs, DWORD addr, WORD value,
				WORD nwords)
{
	int rc;
	int bytes_remain = nwords;
	WORD bytes_to_cmp, block_size;
	WORD cmp[FLASH_BLOCK_SIZE];

	block_size = FLASH_BLOCK_SIZE ;

	(void)_memset(cmp, value, block_size);
	while (bytes_remain) {
		bytes_to_cmp = MIN(block_size, bytes_remain);
		rc = nvs_flash_block_cmp(fs, addr, cmp, bytes_to_cmp);
		if (rc) {
			return rc;
		}
		bytes_remain -= bytes_to_cmp;
		addr += bytes_to_cmp;
	}
	return 0;
}

/* flash block move: move a block at addr to the current data write location
 * and updates the data write location.
 */
static int nvs_flash_block_move(struct nvs_fs *fs, DWORD addr, WORD nwords)
{
	int rc;
	int bytes_remain = nwords;
	WORD bytes_to_copy, block_size;
	WORD buf[FLASH_BLOCK_SIZE];

	block_size = FLASH_BLOCK_SIZE;

	while (bytes_remain) {
		bytes_to_copy = MIN(block_size, bytes_remain);
		rc = nvs_flash_rd(fs, addr, buf, bytes_to_copy);
		if (rc) {
			return rc;
		}
		rc = nvs_flash_data_wrt(fs, buf, bytes_to_copy);
		if (rc) {
			return rc;
		}
		bytes_remain -= bytes_to_copy;
		addr += bytes_to_copy;
	}
	return 0;
}

/* erase a sector and verify erase was OK.
 * return 0 if OK, errorcode on error.
 */
static int nvs_flash_erase_sector(struct nvs_fs *fs, DWORD addr)
{
	int rc = 0;
	DWORD offset;

    DWORD tmp;

	addr &= FLASH_ADDR_SECT_MASK;

	offset = fs->offset;
	offset += fs->sector_size * (addr >> FLASH_ADDR_SECT_SHIFT);


	TRACE("Erasing flash at %x ", offset);
    tmp = (offset >> FLASH_ADDR_SECT_SHIFT);
    TRACE("sector %d", tmp);
    tmp = fs->sector_size;
	TRACE("len %d", tmp);

    if(mpspi_write_enable() != 0)
    {
       	TRACE("Erasing flash write enable failed", rc);
		rc = -2; 
    }
	rc = mpspi_sector_erase(offset);
    if(rc)
    {
        TRACE("Erasing flash erase failed", rc);
		rc = -3; 
    }


	if (nvs_flash_cmp_const(fs, addr, FLASH_ERASE_VALUE,
			fs->sector_size)) {
		TRACE("Erasing flash nvs_flash_cmp_const", rc);
		rc = -1;
	}
	return rc;
}

/* crc update on allocation entry */
static void nvs_ate_crc8_update(struct nvs_ate *entry)
{
	// uint8_t crc8;

	// crc8 = crc8_ccitt(0xff, entry, offsetof(struct nvs_ate, crc8));
	// entry->crc8 = crc8;

    WORD crc16 = 0xFFFF;
    crc16 = CRC16((WORD *)entry, sizeof(struct nvs_ate) - 1, crc16);
    entry->crc16 = crc16;
}

/* crc check on allocation entry
 * returns 0 if OK, 1 on crc fail
 */
static int nvs_ate_crc8_check( struct nvs_ate *entry)
{
	// uint8_t crc8;

	// crc8 = crc8_ccitt(0xff, entry, offsetof(struct nvs_ate, crc8));
	// if (crc8 == entry->crc8) {
	// 	return 0;
	// }
	//WORD _crc16 = entry->crc16;
	WORD check = entry->crc16;
    WORD crc16 = 0xFFFF;
    crc16 = CRC16((WORD *)entry, sizeof(struct nvs_ate) - 1, crc16);
    check -= crc16;
	if (check != 0) {
		// TRACE("crc check err!!! cal crc [%x]", entry->id);
		// TRACE("crc check err!!! cal crc [%x]", entry->offset);
		// TRACE("crc check err!!! cal crc [%x]", entry->len);
		// TRACE("crc check err!!! cal crc [%x]", entry->part);
		// TRACE("crc check err!!! cal crc [%x]", entry->crc16);
		// TRACE("crc check err!!! cal crc [%x]", crc16);	
		return 1;
	}
	return 0;
}

/* nvs_ate_cmp_const compares an ATE to a constant value. returns 0 if
 * the whole ATE is equal to value, 1 if not equal.
 */

static int nvs_ate_cmp_const( struct nvs_ate *entry, WORD value)
{
	WORD *data = (WORD *)entry;
	WORD cmp[sizeof(struct nvs_ate)];

	(void)_memset(cmp, value, sizeof(struct nvs_ate));

	return _memcmp(data, cmp, sizeof(struct nvs_ate));

}

/* nvs_ate_valid validates an ate:
 *     return 1 if crc8 and offset valid,
 *            0 otherwise
 */
static int nvs_ate_valid(struct nvs_fs *fs,  struct nvs_ate *entry)
{
	WORD ate_size;

	ate_size = nvs_al_size(fs, sizeof(struct nvs_ate));

	if ((nvs_ate_crc8_check(entry)) ||
	    (entry->offset >= (fs->sector_size - ate_size))) {
		return 0;
	}

	return 1;

}

/* nvs_close_ate_valid validates an sector close ate: a valid sector close ate:
 * - valid ate
 * - len = 0 and id = 0xFFFF
 * - offset points to location at ate multiple from sector size
 * return 1 if valid, 0 otherwise
 */
static int nvs_close_ate_valid(struct nvs_fs *fs,  struct nvs_ate *entry)
{
	WORD ate_size;

	if ((!nvs_ate_valid(fs, entry)) || (entry->len != 0U) ||
	    (entry->id != 0xFFFF)) {
		return 0;
	}

	ate_size = nvs_al_size(fs, sizeof(struct nvs_ate));
	if ((fs->sector_size - entry->offset) % ate_size) {
		return 0;
	}

	return 1;
}

/* store an entry in flash */
static int nvs_flash_wrt_entry(struct nvs_fs *fs, WORD id, void *data,
				WORD nwords)
{
	int rc;
	struct nvs_ate entry;
	WORD ate_size;

	ate_size = nvs_al_size(fs, sizeof(struct nvs_ate));

	entry.id = id;
	entry.offset = (WORD)(fs->data_wra & FLASH_ADDR_OFFS_MASK);
	entry.len = (WORD)nwords;
	entry.part = 0xff;

	nvs_ate_crc8_update(&entry);

	rc = nvs_flash_data_wrt(fs, data, nwords);
	if (rc) {
		return rc;
	}


	rc = nvs_flash_ate_wrt(fs, &entry);
	if (rc) {
		return rc;
	}

	return 0;
}
/* end of flash routines */

/* If the closing ate is invalid, its offset cannot be trusted and
 * the last valod ate of the sector should instead try to be recovered by going
 * through all ate's.
 *
 * addr should point to the faulty closing ate and will be updated to the last
 * valid ate. If no valid ate is found it will be left untouched.
 */
static int nvs_recover_last_ate(struct nvs_fs *fs, DWORD *addr)
{
	long data_end_addr, ate_end_addr;
	struct nvs_ate end_ate;
	WORD ate_size;
	int rc;
	
	WORD traceVal = (*addr >> FLASH_ADDR_SECT_SHIFT);


	//TRACE("Recovering last ate from sector %d", traceVal);

	ate_size = nvs_al_size(fs, sizeof(struct nvs_ate));

	if(*addr < ate_size)
	{
		*addr = 0;
		TRACE("Recovering last ate from sector %d", fs->offset);
	}
	else
	{
		*addr -= ate_size;
	}



	ate_end_addr = *addr;
	data_end_addr = *addr & FLASH_ADDR_SECT_MASK;
	while (data_end_addr < ate_end_addr) {
		rc = nvs_flash_ate_rd(fs, ate_end_addr, &end_ate);
		if (rc) {
			return rc;
		}
		if (nvs_ate_valid(fs, &end_ate)) {
			/* found a valid ate, update data_end_addr and *addr */
			data_end_addr &= FLASH_ADDR_SECT_MASK;
			data_end_addr += end_ate.offset + end_ate.len;
			*addr = ate_end_addr;
		}

		if(ate_end_addr < ate_size)
			break;
		
		ate_end_addr -= ate_size;
        // TRACE("Recovering last ate  data_end_addr %d, data_end_addr %d",
		//     ate_end_addr, data_end_addr);
	}

	return 0;
}

/* walking through allocation entry list, from newest to oldest entries
 * read ate from addr, modify addr to the previous ate
 */
static int nvs_prev_ate(struct nvs_fs *fs, DWORD *addr, struct nvs_ate *ate)
{
	int rc;
	struct nvs_ate close_ate;
	WORD ate_size;

	ate_size = nvs_al_size(fs, sizeof(struct nvs_ate));

	rc = nvs_flash_ate_rd(fs, *addr, ate);
	if (rc) {
		return rc;
	}

	*addr += ate_size;
	if (((*addr) & FLASH_ADDR_OFFS_MASK) != (fs->sector_size - ate_size)) {
		return 0;
	}

	/* last ate in sector, do jump to previous sector */
	if (((*addr) >> FLASH_ADDR_SECT_SHIFT) == 0U) {
		*addr += ((fs->sector_count - 1) << FLASH_ADDR_SECT_SHIFT);
	} else {

		if(*addr >= (1 << FLASH_ADDR_SECT_SHIFT))
			*addr -= (1 << FLASH_ADDR_SECT_SHIFT);
	}

	rc = nvs_flash_ate_rd(fs, *addr, &close_ate);
	if (rc) {
		return rc;
	}

	rc = nvs_ate_cmp_const(&close_ate, FLASH_ERASE_VALUE);
	/* at the end of filesystem */
	if (!rc) {
		*addr = fs->ate_wra;
		return 0;
	}

	/* Update the address if the close ate is valid.
	 */
	if (nvs_close_ate_valid(fs, &close_ate)) {
		(*addr) &= FLASH_ADDR_SECT_MASK;
		(*addr) += close_ate.offset;
		return 0;
	}

	/* The close_ate was invalid, `lets find out the last valid ate
	 * and point the address to this found ate.
	 *
	 * remark: if there was absolutely no valid data in the sector *addr
	 * is kept at sector_end - 2*ate_size, the next read will contain
	 * invalid data and continue with a sector jump
	 */
	return nvs_recover_last_ate(fs, addr);
}

static void nvs_sector_advance(struct nvs_fs *fs, DWORD *addr)
{
	*addr += (1 << FLASH_ADDR_SECT_SHIFT);
	if ((*addr >> FLASH_ADDR_SECT_SHIFT) == fs->sector_count) {
		if(*addr >= (fs->sector_count << FLASH_ADDR_SECT_SHIFT))
			*addr -= (fs->sector_count << FLASH_ADDR_SECT_SHIFT);
	}
}

/* allocation entry close (this closes the current sector) by writing offset
 * of last ate to the sector end.
 */
static int nvs_sector_close(struct nvs_fs *fs)
{
	int rc;
	struct nvs_ate close_ate;
	WORD ate_size;

	ate_size = nvs_al_size(fs, sizeof(struct nvs_ate));

	close_ate.id = 0xFFFF;
	close_ate.len = 0U;
	close_ate.offset = (WORD)((fs->ate_wra + ate_size) & FLASH_ADDR_OFFS_MASK);

	fs->ate_wra &= FLASH_ADDR_SECT_MASK;
	fs->ate_wra += (fs->sector_size - ate_size);

	nvs_ate_crc8_update(&close_ate);

	rc = nvs_flash_ate_wrt(fs, &close_ate);

	nvs_sector_advance(fs, &fs->ate_wra);

	fs->data_wra = fs->ate_wra & FLASH_ADDR_SECT_MASK;

	return 0;
}

static int nvs_add_gc_done_ate(struct nvs_fs *fs)
{
	struct nvs_ate gc_done_ate;

	WORD traceVal = fs->ate_wra & FLASH_ADDR_OFFS_MASK;

	// TRACE("Adding gc done ate at %x", traceVal);
	gc_done_ate.id = 0xffff;
	gc_done_ate.len = 0U;
	gc_done_ate.offset = (WORD)(fs->data_wra & FLASH_ADDR_OFFS_MASK);
	nvs_ate_crc8_update(&gc_done_ate);

	return nvs_flash_ate_wrt(fs, &gc_done_ate);
}
/* garbage collection: the address ate_wra has been updated to the new sector
 * that has just been started. The data to gc is in the sector after this new
 * sector.
 */
static int nvs_gc(struct nvs_fs *fs)
{
	int rc;
	struct nvs_ate close_ate, gc_ate, wlk_ate;
	DWORD sec_addr, gc_addr, gc_prev_addr, wlk_addr, wlk_prev_addr,
	      data_addr, stop_addr;
	WORD ate_size;

	ate_size = nvs_al_size(fs, sizeof(struct nvs_ate));

	sec_addr = (fs->ate_wra & FLASH_ADDR_SECT_MASK);
	nvs_sector_advance(fs, &sec_addr);
	gc_addr = sec_addr + fs->sector_size - ate_size;
	TRACE("nvs_gc at %x", gc_addr);
	/* if the sector is not closed don't do gc */
	rc = nvs_flash_ate_rd(fs, gc_addr, &close_ate);
	if (rc < 0) {
		/* flash error */
        TRACE("nvs_gc  rd error %d", rc);
		return rc;
	}

	rc = nvs_ate_cmp_const(&close_ate, FLASH_ERASE_VALUE);
	if (!rc) {
		TRACE("nvs_gc nvs_ate_cmp_const %d", rc);
		goto gc_done;
	}

	stop_addr = gc_addr - ate_size;

	if (nvs_close_ate_valid(fs, &close_ate)) {
		gc_addr &= FLASH_ADDR_SECT_MASK;
		gc_addr += close_ate.offset;
	} else {
		rc = nvs_recover_last_ate(fs, &gc_addr);
		if (rc) {
            TRACE("recover last ate error %d", rc);
			return rc;
		}
	}

	do {
		gc_prev_addr = gc_addr;
		rc = nvs_prev_ate(fs, &gc_addr, &gc_ate);
		if (rc) {
            TRACE("prev ate error %d", rc);
			return rc;
		}

		if (!nvs_ate_valid(fs, &gc_ate)) {
			continue;
		}

		wlk_addr = fs->ate_wra;
		do {
			wlk_prev_addr = wlk_addr;
			rc = nvs_prev_ate(fs, &wlk_addr, &wlk_ate);
			if (rc) {
                TRACE("prev ate error %d", rc);
				return rc;
			}
			/* if ate with same id is reached we might need to copy.
			 * only consider valid wlk_ate's. Something wrong might
			 * have been written that has the same ate but is
			 * invalid, don't consider these as a match.
			 */
			if ((wlk_ate.id == gc_ate.id) &&
			    (nvs_ate_valid(fs, &wlk_ate))) {
				break;
			}
		} while (wlk_addr != fs->ate_wra);

		/* if walk has reached the same address as gc_addr copy is
		 * needed unless it is a deleted item.
		 */
		if ((wlk_prev_addr == gc_prev_addr) && gc_ate.len) {
			/* copy needed */
			

			data_addr = (gc_prev_addr & FLASH_ADDR_SECT_MASK);
			data_addr += gc_ate.offset;
			
			gc_ate.offset = (WORD)(fs->data_wra & FLASH_ADDR_OFFS_MASK);
			nvs_ate_crc8_update(&gc_ate);
			//TRACE("Moving %d, len %d, to 0x%08lx", gc_ate.id, gc_ate.len, data_addr);
			rc = nvs_flash_block_move(fs, data_addr, gc_ate.len);
			if (rc) {
                TRACE("flash block move error %d", rc);
				return rc;
			}

			rc = nvs_flash_ate_wrt(fs, &gc_ate);
			if (rc) {
                TRACE("flash ate  wrt error %d", rc);
				return rc;
			}
		}
	} while (gc_prev_addr != stop_addr);

gc_done:

	/* Make it possible to detect that gc has finished by writing a
	 * gc done ate to the sector. In the field we might have nvs systems
	 * that do not have sufficient space to add this ate, so for these
	 * situations avoid adding the gc done ate.
	 */

	if (fs->ate_wra >= (fs->data_wra + ate_size)) {
		rc = nvs_add_gc_done_ate(fs);
		if (rc) {
            TRACE("gc done ate error %d", rc);
			return rc;
		}
	}

	/* Erase the gc'ed sector */
	rc = nvs_flash_erase_sector(fs, sec_addr);
	if (rc) {
        TRACE("flash erase error %d", rc);
		return rc;
	}
	return 0;
}

static int nvs_startup(struct nvs_fs *fs)
{
	int rc;
	struct nvs_ate last_ate;
	WORD ate_size, empty_len;
	WORD ate_check_size = 0;
	/* Initialize addr to 0 for the case fs->sector_count == 0. This
	 * should never happen as this is verified in nvs_init() but both
	 * Coverity and GCC believe the contrary.
	 */
	DWORD addr = 0U;
	WORD i, closed_sectors = 0;
	WORD erase_value = FLASH_ERASE_VALUE;

    nvs_lock(fs);
    ate_size = nvs_al_size(fs, sizeof(struct nvs_ate));
    for (i = 0; i < fs->sector_count; i++)
    {
        addr = (i << FLASH_ADDR_SECT_SHIFT) +
               (WORD)(fs->sector_size - ate_size);
        rc = nvs_flash_cmp_const(fs, addr, FLASH_ERASE_VALUE,
                                 sizeof(struct nvs_ate));
        if (rc)
        {
            /* closed sector */
            closed_sectors++;
            nvs_sector_advance(fs, &addr);
            rc = nvs_flash_cmp_const(fs, addr, FLASH_ERASE_VALUE,
                                     sizeof(struct nvs_ate));
            if (!rc)
            {
                /* open sector */
                TRACE("next open sector %d", i);
                break;
            }
        }
    }

    /* step through the sectors to find a open sector following
	 * a closed sector, this is where NVS can to write.
	 */
	
	/* all sectors are closed, this is not a nvs fs */
	if (closed_sectors == fs->sector_count) {
		rc = -1;
        TRACE("all sectors are closed", rc);
		goto end;
	}

	if (i == fs->sector_count) {
		/* none of the sectors where closed, in most cases we can set
		 * the address to the first sector, except when there are only
		 * two sectors. Then we can only set it to the first sector if
		 * the last sector contains no ate's. So we check this first
		 */
		rc = nvs_flash_cmp_const(fs, addr - ate_size, FLASH_ERASE_VALUE,
				sizeof(struct nvs_ate));
		if (!rc) {
			/* empty ate */
            //TRACE("empty ate", rc);
			nvs_sector_advance(fs, &addr);
		}
	}
	/* addr contains address of closing ate in the most recent sector,
	 * search for the last valid ate using the recover_last_ate routine
	 */

	rc = nvs_recover_last_ate(fs, &addr);
	if (rc) {
        TRACE("recover last ate err", rc);
		goto end;
	}

	/* addr contains address of the last valid ate in the most recent sector
	 * search for the first ate containing all cells erased, in the process
	 * also update fs->data_wra.
	 */
	fs->ate_wra = addr;
	fs->data_wra = addr & FLASH_ADDR_SECT_MASK;

	while (fs->ate_wra >= fs->data_wra) {
		rc = nvs_flash_ate_rd(fs, fs->ate_wra, &last_ate);
		if (rc) {
            TRACE("read ate err", rc);
			goto end;
		}

		rc = nvs_ate_cmp_const(&last_ate, FLASH_ERASE_VALUE);

		if (!rc) {
            TRACE("found ate ff empty location", rc);
			/* found ff empty location */
			break;
		}

		if (nvs_ate_valid(fs, &last_ate)) {
			/* complete write of ate was performed */
			fs->data_wra = addr & FLASH_ADDR_SECT_MASK;
			/* Align the data write address to the current
			 * write block size so that it is possible to write to
			 * the sector even if the block size has changed after
			 * a software upgrade (unless the physical ATE size
			 * will change)."
			 */
			fs->data_wra += nvs_al_size(fs, last_ate.offset + last_ate.len);

			/* ate on the last possition within the sector is
			 * reserved for deletion an entry
			 */
			if (fs->ate_wra == fs->data_wra && last_ate.len) {
				/* not a delete ate */
                TRACE("not a delete ate", rc);
				rc = -1;
				goto end;
			}
		}

		if(fs->ate_wra < ate_size )
		{
			fs->ate_wra = 0;
			break;
		}

		fs->ate_wra -= ate_size;
	}
	/* if the sector after the write sector is not empty gc was interrupted
	 * we might need to restart gc if it has not yet finished. Otherwise
	 * just erase the sector.
	 * When gc needs to be restarted, first erase the sector otherwise the
	 * data might not fit into the sector.
	 */
	addr = fs->ate_wra & FLASH_ADDR_SECT_MASK;
	nvs_sector_advance(fs, &addr);
	rc = nvs_flash_cmp_const(fs, addr, FLASH_ERASE_VALUE, fs->sector_size);
	if (rc < 0) {
        TRACE("just erase the sector err", rc);
		goto end;
	}
	if (rc) {
		/* the sector after fs->ate_wrt is not empty, look for a marker
		 * (gc_done_ate) that indicates that gc was finished.
		 */
		WORD gc_done_marker = FALSE;
		struct nvs_ate gc_done_ate;

		addr = fs->ate_wra + ate_size;
		while ((addr & FLASH_ADDR_OFFS_MASK) < (fs->sector_size - ate_size)) {
			rc = nvs_flash_ate_rd(fs, addr, &gc_done_ate);
			if (rc) {
				goto end;
			}
			if (nvs_ate_valid(fs, &gc_done_ate) &&
			    (gc_done_ate.id == 0xffff) &&
			    (gc_done_ate.len == 0U)) {
				gc_done_marker = TRUE;
				break;
			}
			addr += ate_size;
		}

		if (gc_done_marker) {
			/* erase the next sector */
			TRACE("GC Done marker found", rc);
			addr = fs->ate_wra & FLASH_ADDR_SECT_MASK;
			nvs_sector_advance(fs, &addr);
			rc = nvs_flash_erase_sector(fs, addr);
			goto end;
		}
		TRACE("No GC Done marker found: restarting gc", rc);
		rc = nvs_flash_erase_sector(fs, fs->ate_wra);
		if (rc) {
			goto end;
		}
		fs->ate_wra &= FLASH_ADDR_SECT_MASK;
		fs->ate_wra += (fs->sector_size - 2 * ate_size);
		fs->data_wra = (fs->ate_wra & FLASH_ADDR_SECT_MASK);
		rc = nvs_gc(fs);
		goto end;
	}
	/* possible data write after last ate write, update data_wra */
	while (fs->ate_wra > fs->data_wra) {
		empty_len = fs->ate_wra - fs->data_wra;

		rc = nvs_flash_cmp_const(fs, fs->data_wra, FLASH_ERASE_VALUE,
				empty_len);
		if (rc < 0) {
			goto end;
		}
		if (!rc) {
			break;
		}

		fs->data_wra += empty_len;
	}
	/* If the ate_wra is pointing to the first ate write location in a
	 * sector and data_wra is not 0, erase the sector as it contains no
	 * valid data (this also avoids closing a sector without any data).
	 */
	if (((fs->ate_wra + 2 * ate_size) == fs->sector_size) &&
	    (fs->data_wra != (fs->ate_wra & FLASH_ADDR_SECT_MASK))) {
		rc = nvs_flash_erase_sector(fs, fs->ate_wra);
		if (rc) {
			goto end;
		}
		fs->data_wra = fs->ate_wra & FLASH_ADDR_SECT_MASK;
	}
end:
	/* If the sector is empty add a gc done ate to avoid having insufficient
	 * space when doing gc.
	 */
	if ((!rc) && ((fs->ate_wra & FLASH_ADDR_OFFS_MASK) ==
		      (fs->sector_size - 2 * ate_size))) {

		rc = nvs_add_gc_done_ate(fs);
	}
	nvs_unlock(fs);
	return rc;
}

int nvs_clear(struct nvs_fs *fs)
{
	int rc;
	DWORD addr;
    WORD i;

	if (!fs->ready) {
		TRACE("NVS not initialized", rc);
		return -1;
	}

	for (i = 0; i < fs->sector_count; i++) {
		addr = i << FLASH_ADDR_SECT_SHIFT;
		rc = nvs_flash_erase_sector(fs, addr);
		if (rc) {
			return rc;
		}
	}

	/* nvs needs to be reinitialized after clearing */
	fs->ready = FALSE;

	return 0;
}

int nvs_init(struct nvs_fs *fs)
{

	int rc;
	DWORD traceVal;
    // lock init //
	rc = nvs_startup(fs);
	if (rc) {
		TRACE("err rc:%d", rc);
		return rc;
	}

	/* nvs is ready for use */
	fs->ready = TRUE;

	traceVal = fs->sector_count;        
	TRACE("number sectors [%d]", traceVal);
	traceVal = fs->sector_size;   
	TRACE("sector size  [%d]", traceVal); 
	traceVal = fs->offset;        
	TRACE("sectors offset [%x]", traceVal);
	traceVal = (fs->ate_wra >> FLASH_ADDR_SECT_SHIFT);   
	TRACE("alloc wra: sector [%x]", traceVal); 
	traceVal = (fs->ate_wra & FLASH_ADDR_OFFS_MASK);   
	TRACE("alloc wra: offset [%x]", traceVal); 
	traceVal = (fs->data_wra >> FLASH_ADDR_SECT_SHIFT);   
	TRACE("data wra: sector  [%x]", traceVal); 
	traceVal = (fs->data_wra & FLASH_ADDR_OFFS_MASK);   
	TRACE("data wra: offset [%x]", traceVal); 
	return 0;
}


int nvs_write(struct nvs_fs *fs, WORD id, void *data, WORD nwords)
{
	int rc;
	WORD ate_size, data_size, gc_count;
	struct nvs_ate wlk_ate;
	DWORD wlk_addr, rd_addr;
	WORD required_space = 0U; /* no space, appropriate for delete ate */
	BOOL prev_found = FALSE;

	if (!fs->ready) {
		TRACE("NVS not initialized", rc);
		return -1;
	}

	ate_size = nvs_al_size(fs, sizeof(struct nvs_ate));
	data_size = nvs_al_size(fs, nwords);

	/* The maximum data size is sector size - 4 ate
	 * where: 1 ate for data, 1 ate for sector close, 1 ate for gc done,
	 * and 1 ate to always allow a delete.
	 */
	if ((nwords > (fs->sector_size - 4 * ate_size)) ||
	    ((nwords > 0) && (data == NULL))) {
        TRACE("data size is over maximum %d ", nwords);
		return -1;
	}

	/* find latest entry with same id */
	wlk_addr = fs->ate_wra;
	rd_addr = wlk_addr;

	while (1) {
		rd_addr = wlk_addr;
		rc = nvs_prev_ate(fs, &wlk_addr, &wlk_ate);
		if (rc) {
			TRACE("nvs_write nvs_prev_ate error %d ", rc);
			return rc;
		}
		if ((wlk_ate.id == id) && (nvs_ate_valid(fs, &wlk_ate))) {
			prev_found = TRUE;
			break;
		}
		if (wlk_addr == fs->ate_wra) {
			break;
		}
	}

	if (prev_found) {
		/* previous entry found */
		rd_addr &= FLASH_ADDR_SECT_MASK;
		rd_addr += wlk_ate.offset;

		if (nwords == 0) {
			/* do not try to compare with empty data */
			if (wlk_ate.len == 0U) {
				/* skip delete entry as it is already the
				 * last one
				 */
				return 0;
			}
		} else if (nwords == wlk_ate.len) {
			/* do not try to compare if lengths are not equal */
			/* compare the data and if equal return 0 */
			rc = nvs_flash_block_cmp(fs, rd_addr, data, nwords);
			if (rc <= 0) {
				return rc;
			}
		}
	} else {
		/* skip delete entry for non-existing entry */
		if (nwords == 0) {
			return 0;
		}
	}

	/* calculate required space if the entry contains data */
	if (data_size) {
		/* Leave space for delete ate */
		required_space = data_size + ate_size;
	}

	nvs_lock(fs);

	gc_count = 0;
	while (1) {
		if (gc_count == fs->sector_count) {
			/* gc'ed all sectors, no extra space will be created
			 * by extra gc.
			 */
			TRACE("nvs_write gc_count == fs->sector_count error %d ", rc);
			rc = -1;
			goto end;
		}

		if (fs->ate_wra >= (fs->data_wra + required_space)) {

			rc = nvs_flash_wrt_entry(fs, id, data, nwords);
			if (rc) {
				TRACE("nvs_write nvs_flash_wrt_entry error %d ", rc);
				goto end;
			}
			break;
		}


		rc = nvs_sector_close(fs);
		if (rc) {
			TRACE("nvs_write nvs_sector_close error %d ", rc);
			goto end;
		}

		rc = nvs_gc(fs);
		if (rc) {
			TRACE("nvs_write nvs_gc error %d ", rc);
			goto end;
		}
		gc_count++;
	}
	rc = nwords;
end:
	nvs_unlock(fs);
	return rc;
}

int nvs_delete(struct nvs_fs *fs, WORD id)
{
	return nvs_write(fs, id, NULL, 0);
}

int nvs_read_hist(struct nvs_fs *fs, WORD id, void *data, WORD nwords,
		      WORD cnt)
{
	int rc;
	DWORD wlk_addr, rd_addr;
	WORD cnt_his;
	struct nvs_ate wlk_ate;
	WORD ate_size;

	if (!fs->ready) {
		TRACE("NVS not initialized", id);
		return -1;
	}

	ate_size = nvs_al_size(fs, sizeof(struct nvs_ate));

	if (nwords > (fs->sector_size - 2 * ate_size)) {
        TRACE("data size over maximum %d ", nwords);
		return -1;

	}
	cnt_his = 0U;

	wlk_addr = fs->ate_wra;
	rd_addr = wlk_addr;

	while (cnt_his <= cnt) {
		rd_addr = wlk_addr;
		rc = nvs_prev_ate(fs, &wlk_addr, &wlk_ate);
		if (rc) {
			goto err;
		}
		if ((wlk_ate.id == id) &&  (nvs_ate_valid(fs, &wlk_ate))) {
			cnt_his++;
		}
		if (wlk_addr == fs->ate_wra) {
			break;
		}
	}

	if (((wlk_addr == fs->ate_wra) && (wlk_ate.id != id)) ||
	    (wlk_ate.len == 0U) || (cnt_his < cnt)) {
		return -1;
	}

	rd_addr &= FLASH_ADDR_SECT_MASK;
	rd_addr += wlk_ate.offset;
	rc = nvs_flash_rd(fs, rd_addr, data, MIN(nwords, wlk_ate.len));
	if (rc) {
		goto err;
	}

	return wlk_ate.len;

err:
	return rc;
}

int nvs_read(struct nvs_fs *fs, WORD id, void *data, WORD nwords)
{
	return nvs_read_hist(fs, id, data, nwords, 0);
}

int nvs_calc_free_space(struct nvs_fs *fs)
{

	int rc;
	struct nvs_ate step_ate, wlk_ate;
	DWORD step_addr, wlk_addr;
	int ate_size, free_space;
    WORD i;
	if (!fs->ready) {
		TRACE("NVS not initialized", rc);
		return -1;
	}

	ate_size = nvs_al_size(fs, sizeof(struct nvs_ate));

	free_space = 0;
	for (i = 1; i < fs->sector_count; i++) {
		free_space += (fs->sector_size - ate_size);
	}

	step_addr = fs->ate_wra;

	while (1) {
		rc = nvs_prev_ate(fs, &step_addr, &step_ate);
		if (rc) {
			return rc;
		}

		wlk_addr = fs->ate_wra;

		while (1) {
			rc = nvs_prev_ate(fs, &wlk_addr, &wlk_ate);
			if (rc) {
				return rc;
			}
			if ((wlk_ate.id == step_ate.id) ||
			    (wlk_addr == fs->ate_wra)) {
				break;
			}
		}

		if ((wlk_addr == step_addr) && step_ate.len &&
		    (nvs_ate_valid(fs, &step_ate))) {
			/* count needed */
			free_space -= nvs_al_size(fs, step_ate.len);
			free_space -= ate_size;
		}

		if (step_addr == fs->ate_wra) {
			break;
		}

	}
	return free_space;
}