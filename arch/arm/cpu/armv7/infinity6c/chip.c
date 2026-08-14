/*
 * chip.c - Sigmastar
 *
 * Copyright (c) [2019~2020] SigmaStar Technology.
 *
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License version 2 for more details.
 *
 */

#include <common.h>
#include <drvSPINOR.h>
#include "environment.h"
#include "asm/arch/mach/ms_types.h"
#include "asm/arch/mach/platform.h"
#include "asm/arch/mach/io.h"
#include "ms_version.h"

#define STORAGE_SPI_NONE            (0x00)
#define STORAGE_SPI_NAND_SKIP_SD    (BIT2)
#define STORAGE_SPI_NAND            (BIT4)
#define STORAGE_SPI_NOR             (BIT5)
#define STORAGE_SPI_NOR_SKIP_SD     (BIT1)
#define STORAGE_USB                 (BIT12)
#define STORAGE_EMMC_8              (BIT7|BIT3)
#define STORAGE_EMMC_4              (BIT3)
#define STORAGE_BOOT_TYPES          (BIT12|BIT7|BIT5|BIT4|BIT3|BIT2|BIT1)
typedef struct
{
	unsigned char                       tagPrefix[3];
	unsigned char                       headerVersion[1];
	unsigned char                       libType[2];
	unsigned char                       chip[4];
	unsigned char                       changelist[8];
	unsigned char                       component[10];
	unsigned char                       reserved[1];
	unsigned char                       tagSuffix[3];


} MS_VERSION;

MS_VERSION UBT_VERSION =
{
	{'M','V','X'},
	MVXV_HEAD_VER,
	MVXV_LIB_TYPE, //R = general release version
	MVXV_CHIP_ID,
	MVXV_CHANGELIST,
	MVXV_COMP_ID,
	{'#'},
	{'X','V','M'},
};
DECLARE_GLOBAL_DATA_PTR;

//#define DEV_BUF_ADDR (PHYS_SDRAM_1+0x3F800)


int arch_cpu_init(void)
{

    gd->xtal_clk=CONFIG_PIUTIMER_CLOCK;



	/* following code is not worked since bd has not allocated yet!!*/
//
//	gd->bd->bi_arch_number = MACH_TYPE_COLUMBUS2;
//
//	/* adress of boot parameters */
//	gd->bd->bi_boot_params = BOOT_PARAMS;


    return 0;
}

// we borrow the DRAN init to do the devinfo setting...
int dram_init(void)
{
    u16 rsize = 1 << INREG16(0x1F2CB234);
    gd->ram_size = rsize * 1024 * 1024;

    return 0;
}

DEVINFO_CHIP_TYPE ms_check_chip(void)
{
/*
	U16 chipType;
	chipType = INREG16(GET_REG_ADDR(REG_ADDR_BASE_MIU, 0x69)) & 0xF000;

	if (chipType == 0x6000)
		return DEVINFO_313E;
	else if (chipType == 0x8000)
		return DEVINFO_318;
	else if (chipType == 0x9000)
		return DEVINFO_318;
	else
		return DEVINFO_NON;
*/
	return DEVINFO_NON;
}

int checkboard(void)
{
/*
    DEVINFO_st* devinfo;
    int i;


	devinfo = (DEVINFO_st*)(DEV_BUF_ADDR);    // start address: 30K, size:  2K

//	for(i=0;i<sizeof(version.bn);i++)
//	{
//		putc(version.bn[i]);
//	}
//	for(i=0;i<sizeof(version.lib_type);i++)
//	{
//		putc(version.lib_type[i]);
//	}
//
//	putc('@');
	for(i=0;i<sizeof(version.changelist);i++)
	{
		putc(version.changelist[i]);
	}
	putc('\n');

	if (devinfo->header[0]!='D' || devinfo->header[1]!='E' || devinfo->header[2]!='V' || devinfo->header[3]!='I' ||
		devinfo->header[4]!='N' || devinfo->header[5]!='F' || devinfo->header[6]!='O' || devinfo->header[7]!='.')
	{
		printf("***********************************************************\r\n");
		printf("* DEVINFO. header check failed !!!                        *\r\n");
		printf("* Please program DEVINFO section.                         *\r\n");
		printf("***********************************************************\r\n");
		while(1);
	}
*/
        int i=0;

        printf("Version: ");
        for(i=0;i<4;i++)
        {
            printf("%c",UBT_VERSION.chip[i]);
        }
        for(i=0;i<8;i++)
        {
            printf("%c",UBT_VERSION.changelist[i]);
        }
        printf("\n");
        /*
		switch (ms_check_chip())
		{
			case DEVINFO_313E:
				printf("DEVINFO: 313E\n");
				break;
			case DEVINFO_318:
				printf("DEVINFO: 318\n");
				break;
			case DEVINFO_NON:
				printf("DEVINFO: NON\n");
				break;
			default:
				printf("DEVINFO:ERROR\n");
		}
        */

#ifdef CONFIG_CMD_BDI
        printf("***********************************************************\r\n");
        printf("* MEMORY LAYOUT                                            \r\n");
        printf("* PHYS_SDRAM_1:           0X%08x                           \r\n",PHYS_SDRAM_1);
        printf("* PHYS_SDRAM_1_SIZE:      0X%08x                           \r\n",PHYS_SDRAM_1_SIZE);
        printf("* CONFIG_SYS_TEXT_BASE:   0X%08x                           \r\n",CONFIG_SYS_TEXT_BASE);
        printf("* CONFIG_SYS_SDRAM_BASE:  0X%08x                           \r\n",CONFIG_SYS_SDRAM_BASE);
        printf("* CONFIG_SYS_INIT_SP_ADDR:0X%08x  (gd_t *)pointer          \r\n",CONFIG_SYS_INIT_SP_ADDR);
        printf("* SCFG_MEMP_START:        0X%08x                           \r\n",SCFG_MEMP_START);
        printf("* SCFG_PNLP_START:        0X%08x                           \r\n",SCFG_PNLP_START);
        printf("* BOOT_PARAMS:            0X%08x                           \r\n",BOOT_PARAMS);
        printf("* CONFIG_SYS_LOAD_ADDR:   0X%08x                           \r\n",CONFIG_SYS_LOAD_ADDR);
        //printf("* KERNEL_RAM_BASE:0X%08x                                   \r\n",KERNEL_RAM_BASE);
        printf("* CONFIG_UNLZO_DST_ADDR:  0X%08x                           \r\n",CONFIG_UNLZO_DST_ADDR);
        printf("\r\n");
        printf("* CONFIG_ENV_SIZE:        0X%08x                           \r\n",CONFIG_ENV_SIZE);
        printf("* CONFIG_SYS_MALLOC_LEN:  0X%08x                           \r\n",CONFIG_SYS_MALLOC_LEN);
        printf("* CONFIG_STACKSIZE:       0X%08x                           \r\n",CONFIG_STACKSIZE);
        printf("* KERNEL_IMAGE_SIZE:      0X%08x                           \r\n",KERNEL_IMAGE_SIZE);
        printf("***********************************************************\r\n");
#endif
	return 0;
}



DEVINFO_BOOT_TYPE ms_devinfo_boot_type(void)
{
    //U16 hwstrap,b_extEcc;
    //U16 bootFlow;
    U16 u16Storage;
    //hwstrap = INREG16( GET_REG_ADDR(MS_BASE_REG_DID_KEY_PA, 0x70));
	//b_extEcc = INREG16( GET_REG_ADDR(REG_ADDR_BASE_PADTOP, 0x72)) & BIT0;
	//bootFlow = INREG16( GET_REG_ADDR(REG_ADDR_BASE_PADTOP, 0x5C));
    u16Storage = (INREG16( GET_REG_ADDR(MS_BASE_REG_DID_KEY_PA, 0x70) )) & STORAGE_BOOT_TYPES;

    if(u16Storage == STORAGE_SPI_NOR || u16Storage == STORAGE_SPI_NOR_SKIP_SD)
    {
        return DEVINFO_BOOT_TYPE_SPI;
    }
    else if(u16Storage == STORAGE_SPI_NAND || u16Storage == STORAGE_SPI_NAND_SKIP_SD)
    {
        return DEVINFO_BOOT_TYPE_SPINAND_INT_ECC;
    }
    else if(u16Storage == STORAGE_EMMC_4 || u16Storage == STORAGE_EMMC_8)
    {
        return DEVINFO_BOOT_TYPE_EMMC;
    }

    return DEVINFO_BOOT_TYPE_NONE;
}

//DEVINFO_BOARD_TYPE ms_devinfo_board_type(void)
//{
//	return (DEVINFO_BOARD_TYPE)gd->board_type;
//}
//
//DEVINFO_BOARD_TYPE ms_devinfo_panel_type(void)
//{
//	return (DEVINFO_PANEL_TYPE)gd->panel_type;
//}
//
//DEVINFO_RTK_FLAG ms_devinfo_rtk_flag(void)
//{
//    return (DEVINFO_RTK_FLAG)gd->rtk_flag;
//}
//
//U16 OALReadBootSource(void)
//{
//	return ((INREG16(0x1F003C00+0x0065*4)&BIT2) == BIT2);
//}

#ifdef CONFIG_BOARD_LATE_INIT
extern void *malloc(unsigned int);
#define BUF_SIZE 256

/*
 * GIVE THE BOARD A MAC ADDRESS OF ITS OWN
 *
 * This SoC has no die ID to derive an identity from. The Infinity6E parts carry
 * a 48-bit one in OTP at 0x1F203150, and the same registers read back as zero
 * here, as does ipctool's chip-serial query -- so the usual trick of folding a
 * die ID into an address has nothing to fold. Left alone, every board built
 * from one image answers to the vendor default 00:00:23:34:45:66 and two of
 * them on a network are indistinguishable.
 *
 * The SPI NOR part has what the SoC lacks. Its Read Unique ID (4Bh) returns 64
 * factory-programmed read-only bits, unique per device, and the flash is
 * soldered to the board, so it identifies the unit as well as a die ID would.
 * It is also the one identity that survives a full chip erase: derived rather
 * than stored, the same address comes back after any reflash, which an assigned
 * one written into the environment does not.
 *
 * Doing it here rather than in Linux is what the vendor's own design expects --
 * MDev_EMAC_get_mac_address reads the MAC out of the EMAC's SA1/SA2 registers
 * and says "check if bootloader set address" -- and it means the address exists
 * before the first packet, not after userspace has come up and reconfigured the
 * interface.
 *
 * The value is written to the environment once, on a board that has none, so
 * `fw_printenv ethaddr` under Linux agrees with what U-Boot computed. It is
 * derived identically on every boot, so the save is a convenience for userspace
 * rather than the source of truth, and a lost environment costs nothing.
 *
 * An address already set by hand is never touched: this only fills in an empty
 * or vendor-default ethaddr.
 */
#define ETHADDR_VENDOR_DEFAULT "00:00:23:34:45:66"

static void board_provision_ethaddr(void)
{
    u8   au8_uid[SPI_NOR_RDUID_BYTE_CNT];
    char ac_mac[18];
    char *pc_current;
    u8   au8_mac[6];
    u8   u8_i;

    pc_current = getenv("ethaddr");
    if (pc_current && *pc_current && strcmp(pc_current, ETHADDR_VENDOR_DEFAULT))
    {
        return;
    }

    if (ERR_SPINOR_SUCCESS != mdrv_spinor_read_unique_id(au8_uid))
    {
        printf("MAC: no unique ID from the flash, leaving ethaddr alone\n");
        return;
    }

    /*
     * 64 bits into 40. The low five bytes are taken whole and the high three
     * are folded over them, so every bit of the ID reaches the address and two
     * parts differing anywhere differ here.
     *
     * 0x02 is a locally administered unicast prefix: bit 1 set says the address
     * is assigned here rather than from an OUI, and bit 0 clear says unicast.
     * That is the honest thing to claim -- this is not a purchased address and
     * should not pretend to be globally unique.
     */
    au8_mac[0] = 0x02;
    for (u8_i = 0; 5 > u8_i; u8_i++)
    {
        au8_mac[u8_i + 1] = au8_uid[u8_i + 3];
    }
    au8_mac[1] ^= au8_uid[0];
    au8_mac[2] ^= au8_uid[1];
    au8_mac[3] ^= au8_uid[2];

    /*
     * Odd last byte, so a wireless address derived as ethaddr + 1 can never
     * carry into the byte above and collide with the wired one. No SigmaStar
     * board here has wifi yet; the convention is shared with the Infinity6E
     * side and breaking it here would surprise the next one that does.
     */
    au8_mac[5] |= 0x01;

    sprintf(ac_mac, "%02x:%02x:%02x:%02x:%02x:%02x", au8_mac[0], au8_mac[1], au8_mac[2], au8_mac[3],
            au8_mac[4], au8_mac[5]);

    setenv("ethaddr", ac_mac);
    printf("MAC: %s, from flash unique ID %02x%02x%02x%02x%02x%02x%02x%02x\n", ac_mac, au8_uid[0],
           au8_uid[1], au8_uid[2], au8_uid[3], au8_uid[4], au8_uid[5], au8_uid[6], au8_uid[7]);

    /*
     * Best effort. The address is in the environment either way for this boot,
     * and the next boot derives the same one, so a failed save costs a line of
     * output and nothing else.
     */
    if (saveenv())
    {
        printf("MAC: could not save ethaddr; it is derived again on every boot\n");
    }
}

int board_late_init(void)
{
    /*
    U16 u16Status;
    char *chainmode="CBC";
    char *keytype="EFUSE";
    char *buffer;

    printf("*******************************************\nboard_late_init for upgrade image\n");

    buffer =(char *)malloc(BUF_SIZE);
    if((buffer==NULL))
    {
        printf("no memory for command string!!\n");
        return -1;
    }

    u16Status=INREG16(REG_ADDR_STATUS);
    if(u16Status)
    {
        printf("Status register=0x%08X\n", u16Status);
        if(u16Status & KEY_CUST_BIT)
        {
            memset(buffer, 0 , BUF_SIZE);
            sprintf(buffer, "aes enc %x %s %s KEY_CUST", KEY_CUST_LOAD_ADDRESS, chainmode, keytype);
            printf("%s\n", buffer);
            run_command(buffer, 0);
        }
        if(u16Status & ZBOOT_BIT)
        {
            memset(buffer, 0 , BUF_SIZE);
            sprintf(buffer, "aes enc %x %s %s ZBOOT", ZBOOT_LOAD_ADDRESS, chainmode, keytype);
            printf("%s\n", buffer);
            run_command(buffer, 0);
        }
        if(u16Status & UBOOT_BIT)
        {
            memset(buffer, 0 , BUF_SIZE);
            sprintf(buffer, "aes enc %x %s %s UBOOT", UBOOT_LOAD_ADDRESS, chainmode, keytype);
            printf("%s\n", buffer);
            run_command(buffer, 0);
        }
        if(u16Status & KERNEL_BIT)
        {
            memset(buffer, 0 , BUF_SIZE);
            sprintf(buffer, "aes enc %x %s %s KERNEL", KERNEL_LOAD_ADDRESS, chainmode, keytype);
            printf("%s\n", buffer);
            run_command(buffer, 0);
        }
        // *(volatile U16*)0x1F001CB8 = 0xFF;
        // *(volatile U16*)0x1F001CB8 = 0x79;
        //run_command("reset", 0);
        reset_cpu(0);
    }
    printf("*******************************************\n");
    */

    char msize[32];
    u16 rsize = gd->ram_size / 1024 / 1024;

    sprintf(msize, "%dM", rsize);
    setenv("totalmem", msize);

    if (rsize == 256) {
        setenv("memlx", "0x10000000");
        setenv("memsz", "0x0A000000");
    } else if (rsize == 128) {
        setenv("memlx", "0x8000000");
        setenv("memsz", "0x4600000");
    } else {
        setenv("memlx", "0x4000000");
        setenv("memsz", "0x2000000");
    }

    board_provision_ethaddr();

    return 0;
}
#endif

#ifndef CONFIG_MS_SAVE_ENV_IN_ISP_FLASH
#ifdef CONFIG_ENV_IS_IN_NAND
extern int nand_env_init(void);
extern int nand_saveenv(void);
extern void nand_env_relocate_spec(void);
#endif

#ifdef CONFIG_ENV_IS_IN_MMC
extern int mmc_env_init(void);
extern int mmc_saveenv(void);
extern void mmc_env_relocate_spec(void);
#endif

env_t *env_ptr;

void env_relocate_spec(void)
{
#ifdef CONFIG_ENV_IS_IN_MMC
    mmc_env_relocate_spec();
#elif defined(CONFIG_ENV_IS_IN_NAND)
    nand_env_relocate_spec();
#endif
}


int env_init(void)
{
#ifdef CONFIG_ENV_IS_IN_MMC
    return mmc_env_init();
#elif defined(CONFIG_ENV_IS_IN_NAND)
    return nand_env_init();
#endif
}

int saveenv(void)
{
#ifdef CONFIG_ENV_IS_IN_MMC
    return mmc_saveenv();
#elif defined(CONFIG_ENV_IS_IN_NAND)
    return nand_saveenv();
#endif
}
#endif

#define  reg_flush_op_on_fire           (0x1F000000 + 0x102200*2 + 0x05*4)
#define  reg_Flush_miu_pipe_done_flag   (0x1F000000 + 0x102200*2 + 0x10*4)

inline void _chip_flush_miu_pipe(void)
{

	unsigned short dwReadData = 0;

	//toggle the flush miu pipe fire bit
	*(volatile unsigned short *)(reg_flush_op_on_fire) = 0x10;
	*(volatile unsigned short *)(reg_flush_op_on_fire) = 0x11;

	do
	{
		dwReadData = *(volatile unsigned short *)(reg_Flush_miu_pipe_done_flag);
		dwReadData &= (1<<12);  //Check Status of Flush Pipe Finish

	} while(dwReadData == 0);
}

extern void Chip_Flush_Memory(void);
extern void Chip_Read_Memory(void);

inline void Chip_Flush_Memory(void)
{
	_chip_flush_miu_pipe();
}

inline void Chip_Read_Memory(void)
{
	_chip_flush_miu_pipe();
}
