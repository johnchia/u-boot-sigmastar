/*
 * sstar_ethaddr.c - Sigmastar
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

#include <asm/sstar_ethaddr.h>

/*
 * GIVE THE BOARD A MAC ADDRESS OF ITS OWN
 *
 * Left alone, every board built from one image answers to the vendor default
 * 00:00:23:34:45:66 and two of them on a network are indistinguishable.
 *
 * WHY THE FLASH AND NOT THE DIE
 *
 * The obvious source is a die ID, and it is not available on both parts. The
 * Infinity6E carries a 48-bit one in OTP at 0x1F203150; the same registers read
 * back as zero on Infinity6C, as does ipctool's chip-serial query, so on that
 * part the usual trick of folding a die ID into an address has nothing to fold.
 *
 * The SPI NOR part has what the SoC may lack, on every board here. Its Read
 * Unique ID (4Bh) returns 64 factory-programmed read-only bits, unique per
 * device, and the flash is soldered down, so it identifies the unit as well as
 * a die ID would. It is also the one identity that survives a full chip erase:
 * derived rather than stored, the same address comes back after any reflash,
 * which an assigned one written into the environment does not.
 *
 * So this is one method for both SoCs rather than a die ID on the part that has
 * one and something else on the part that does not. A second derivation would
 * be a second thing to be wrong, and would give the same board two different
 * addresses depending on which path ran.
 *
 * WHY HERE AND NOT IN LINUX
 *
 * It is what the vendor's own design expects -- MDev_EMAC_get_mac_address reads
 * the MAC out of the EMAC's SA1/SA2 registers and says "check if bootloader set
 * address" -- and it means the address exists before the first packet, not
 * after userspace has come up and reconfigured the interface.
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

void sstar_provision_ethaddr(void)
{
    u8   au8_uid[SPI_NOR_RDUID_BYTE_CNT];
    char ac_mac[18];
    char ac_uid[20];
    char ac_stat[8];
    char *pc_current;
    u8   au8_mac[6];
    u8   u8_uid_status;
    u8   u8_i;

    pc_current = getenv("ethaddr");
    if (pc_current && *pc_current && strcmp(pc_current, ETHADDR_VENDOR_DEFAULT))
    {
        return;
    }

    /*
     * There is no console on a deployed board, so the outcome is recorded in
     * the environment where Linux can read it back with fw_printenv. uidstat is
     * the driver's status code and uidraw the bytes it returned, both written
     * whatever happens -- a failure that says nothing is the one thing this
     * cannot afford, since it is indistinguishable from the code never running.
     */
    memset(au8_uid, 0, sizeof(au8_uid));
    u8_uid_status = mdrv_spinor_read_unique_id(au8_uid);

    sprintf(ac_uid, "%02x%02x%02x%02x%02x%02x%02x%02x", au8_uid[0], au8_uid[1], au8_uid[2],
            au8_uid[3], au8_uid[4], au8_uid[5], au8_uid[6], au8_uid[7]);
    setenv("uidraw", ac_uid);
    sprintf(ac_stat, "%d", u8_uid_status);
    setenv("uidstat", ac_stat);

    if (ERR_SPINOR_SUCCESS != u8_uid_status)
    {
        printf("MAC: no unique ID from the flash (status %d), leaving ethaddr alone\n",
               u8_uid_status);
        saveenv();
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
     * board here has wifi yet, and breaking the convention would surprise the
     * first one that does.
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
