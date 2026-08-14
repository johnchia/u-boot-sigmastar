/*
 * sstar_ethaddr.h - Sigmastar
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

#ifndef __SSTAR_ETHADDR_H__
#define __SSTAR_ETHADDR_H__

/*
 * Fill ethaddr from the NOR part's factory unique ID, for a board that has no
 * address of its own -- an empty ethaddr or the shared vendor default. Called
 * from board_late_init; does nothing to an address that was set deliberately.
 *
 * Built only where CONFIG_SSTAR_ETHADDR_FROM_NOR_UID is defined, which is also
 * what puts the flash driver's header on the include path (see the armv7
 * Makefile). A SoC whose defconfig lacks CONFIG_MS_NOR_ONEBIN has no
 * mdrv_spinor_read_unique_id to call and must not define it.
 */
void sstar_provision_ethaddr(void);

#endif /* __SSTAR_ETHADDR_H__ */
