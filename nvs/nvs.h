#ifndef __NVS_FLASH_H__
#define __NVS_FLASH_H__

#include <system.h>
#include <trace.h>

#define MIN(x, y)   ((x) < (y) ? (x) : (y))

#define FLASH_ERASE_VALUE       (0xFFFF)

#define FLASH_BLOCK_SIZE        (32) // WORDS

// SECTOR SIZE 4096 BYTES (2048 words )//
#define FLASH_ADDR_SECT_SHIFT   (11)
#define FLASH_ADDR_OFFS_MASK    (DWORD)(((DWORD)1 << FLASH_ADDR_SECT_SHIFT) - 1)
#define FLASH_ADDR_SECT_MASK     (~FLASH_ADDR_OFFS_MASK)
#define FLASH_SECTOR_SIZE        (2048) // size of words //
#define FLASH_PAGE_SIZE          (128) // size of words //
#define FLASH_PAGE_SIZE_MASK     (128 - 1) // size of words //

struct nvs_ate
{
    WORD id;
    WORD offset;
    WORD len;
    WORD part;
    WORD crc16;
};

struct nvs_fs
{
    DWORD offset;
    DWORD ate_wra;
    DWORD data_wra;
    DWORD sector_size;
    DWORD sector_count;
    WORD ready;
    WORD lock;
};

/**
 * @}
 */

/**
 * @brief Non-volatile Storage APIs
 * @defgroup nvs_high_level_api Non-volatile Storage APIs
 * @ingroup nvs
 * @{
 */

/**
 * @brief nvs_init
 *
 * Initializes a NVS file system in flash.
 *
 * @param fs Pointer to file system
 * @param dev_name Pointer to flash device name
 * @retval 0 Success
 * @retval -ERRNO errno code if error
 */
int nvs_init(struct nvs_fs *fs);

/**
 * @brief nvs_clear
 *
 * Clears the NVS file system from flash.
 * @param fs Pointer to file system
 * @retval 0 Success
 * @retval -ERRNO errno code if error
 */
int nvs_clear(struct nvs_fs *fs);

/**
 * @brief nvs_write
 *
 * Write an entry to the file system.
 *
 * @param fs Pointer to file system
 * @param id Id of the entry to be written
 * @param data Pointer to the data to be written
 * @param len Number of bytes to be written
 *
 * @return Number of bytes written. On success, it will be equal to the number of bytes requested
 * to be written. When a rewrite of the same data already stored is attempted, nothing is written
 * to flash, thus 0 is returned. On error, returns negative value of errno.h defined error codes.
 */
int nvs_write(struct nvs_fs *fs, WORD id, void *data, WORD len);

/**
 * @brief nvs_delete
 *
 * Delete an entry from the file system
 *
 * @param fs Pointer to file system
 * @param id Id of the entry to be deleted
 * @retval 0 Success
 * @retval -ERRNO errno code if error
 */
int nvs_delete(struct nvs_fs *fs, WORD id);

/**
 * @brief nvs_read
 *
 * Read an entry from the file system.
 *
 * @param fs Pointer to file system
 * @param id Id of the entry to be read
 * @param data Pointer to data buffer
 * @param len Number of bytes to be read
 *
 * @return Number of bytes read. On success, it will be equal to the number of bytes requested
 * to be read. When the return value is larger than the number of bytes requested to read this
 * indicates not all bytes were read, and more data is available. On error, returns negative
 * value of errno.h defined error codes.
 */
int nvs_read(struct nvs_fs *fs, WORD id, void *data, WORD len);

/**
 * @brief nvs_read_hist
 *
 * Read a history entry from the file system.
 *
 * @param fs Pointer to file system
 * @param id Id of the entry to be read
 * @param data Pointer to data buffer
 * @param len Number of bytes to be read
 * @param cnt History counter: 0: latest entry, 1: one before latest ...
 *
 * @return Number of bytes read. On success, it will be equal to the number of bytes requested
 * to be read. When the return value is larger than the number of bytes requested to read this
 * indicates not all bytes were read, and more data is available. On error, returns negative
 * value of errno.h defined error codes.
 */
int nvs_read_hist(struct nvs_fs *fs, WORD id, void *data, WORD len, WORD cnt);

/**
 * @brief nvs_calc_free_space
 *
 * Calculate the available free space in the file system.
 *
 * @param fs Pointer to file system
 *
 * @return Number of bytes free. On success, it will be equal to the number of bytes that can
 * still be written to the file system. Calculating the free space is a time consuming operation,
 * especially on spi flash. On error, returns negative value of errno.h defined error codes.
 */
int nvs_calc_free_space(struct nvs_fs *fs);

/**
 * @}
 */
#ifdef TEST_NVS
void test_read(struct nvs_fs *fs, WORD id, WORD *buf, WORD buf_len);
void test_read_cmp(struct nvs_fs *fs, WORD id, WORD *buf, WORD buf_len, WORD *strexp);
void test_write(struct nvs_fs *fs, WORD id, WORD *buf, WORD buf_len);
void test_delete(struct nvs_fs *fs, WORD id);
void test_space(struct nvs_fs *fs);
#endif
#ifdef __cplusplus
}
#endif

#endif
