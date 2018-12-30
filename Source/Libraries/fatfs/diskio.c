/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
//#include "usbdisk.h"	/* Example: USB drive control */
//#include "atadrive.h"	/* Example: ATA drive control */
//#include "Drv_SD.h"
#include "Drv_W25Q80.h"

/* Definitions of physical drive number for each media */
#define ATA		0
#define MMC		1
#define USB		2


/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber (0..) */
)
{
	DSTATUS stat;
//	int result;

//	switch (pdrv) {
//	case ATA :
//		result = ATA_disk_initialize();

		// translate the reslut code here

//		return stat;

//	case MMC :
		//SD_Init();
        

		// translate the reslut code here
		stat = RES_OK;
		return stat;

//	case USB :
//		result = USB_disk_initialize();

		// translate the reslut code here

//		return stat;
//	}
//	return STA_NOINIT;
}

/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/
DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber (0..) */
)
{
//	DSTATUS stat;
//	int result;

//	switch (pdrv) {
//	case ATA :
//		result = ATA_disk_status();

		// translate the reslut code here

//		return stat;

//	case MMC :
//		result = MMC_disk_status();

		// translate the reslut code here

		return RES_OK;

//	case USB :
//		result = USB_disk_status();

		// translate the reslut code here

//		return stat;
//	}
//	return STA_NOINIT;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	UINT count		/* Number of sectors to read (1..128) */
)
{
	//DRESULT res;
	//int result;

//	switch (pdrv) {
//	case ATA :
		// translate the arguments here
//
//		result = ATA_disk_read(buff, sector, count);

		// translate the reslut code here

//		return res;

//	case MMC :
		// translate the arguments here

		ReadW25Q80Operation(sector*4096, buff, count*4096);

		// translate the reslut code here

		return RES_OK;

//	case USB :
		// translate the arguments here

//		result = USB_disk_read(buff, sector, count);

		// translate the reslut code here

//		return res;
//	}
//	return RES_PARERR;
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	UINT count			/* Number of sectors to write (1..128) */
)
{
	//DRESULT res;
	//int result;

//	switch (pdrv) {
//	case ATA :
		// translate the arguments here

//		result = ATA_disk_write(buff, sector, count);

		// translate the reslut code here

//		return res;

//	case MMC :
		// translate the arguments here

		//result = SD_WriteSectors((BYTE *)buff, sector, count);
        DWORD len = count * 4096;
        DWORD address = sector * 4096;
        DWORD i = 0;
        
        for (; i<count; i++) {
             EraseW25Q80Operation((sector+i)*4096);
        }
        
        i=0;
        
        while(i<len) {
            WriteW25Q80Operation(address, (uint8_t *)buff+i, 256);
            i+=256;
            address += 256;
        }
        
        //static BYTE test[4096]={0};
        //ReadW25Q80Operation(0, test, 4096);
		// translate the reslut code here

		return RES_OK;

//	case USB :
		// translate the arguments here

//		result = USB_disk_write(buff, sector, count);

		// translate the reslut code here

//		return res;
//	}
//	return RES_PARERR;
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
    switch(cmd) {
    case CTRL_ERASE_SECTOR:
        EraseW25Q80Operation(*(DWORD *)buff);
        break;
    case GET_SECTOR_SIZE:
        *(DWORD *)buff = 4096;
        break;
    case GET_SECTOR_COUNT:
        *(DWORD *)buff = 2048;
        break;
    case GET_BLOCK_SIZE:
        *(DWORD *)buff = 1;
        break;

    default:
        break;
    }
    
    return RES_OK;
}

DWORD get_fattime (void)
{
    return 0;
}
