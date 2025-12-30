/****************************************************************************
 *
 *   Authors: Ilya Sopkin <ivsopkin@yandex.ru>
 *
 ****************************************************************************/

/**
 * @file manifest.c
 *
 * This module supplies the interface to the manifest of hardware that is
 * optional and dependent on the HW REV and HW VER IDs
 *
 * The manifest allows the system to know whether a hardware option
 * say for example the PX4IO is an no-pop option vs it is broken.
 *
 */

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <nuttx/config.h>
#include <board_config.h>

#include <inttypes.h>
#include <stdbool.h>
#include <syslog.h>

#include "systemlib/px4_macros.h"

/****************************************************************************
 * Pre-Processor Definitions
 ****************************************************************************/

typedef struct {
	uint32_t                hw_ver_rev; /* the version and revision */
	const px4_hw_mft_item_t *mft;       /* The first entry */
	uint32_t                entries;    /* the lenght of the list */
} px4_hw_mft_list_entry_t;

typedef px4_hw_mft_list_entry_t *px4_hw_mft_list_entry;
#define px4_hw_mft_list_uninitialized (px4_hw_mft_list_entry) -1

static const px4_hw_mft_item_t device_unsupported = {0, 0, 0};

// List of components on a specific board configuration
// The index of those components is given by the enum (px4_hw_mft_item_id_t)
// declared in board_common.h

static const px4_hw_mft_item_t hw_mft_list_v0101[] = {
	{
		//  PX4_MFT_PX4IO
		.present     = 0,
		.mandatory   = 0,
		.connection  = px4_hw_con_unknown,
	},
	{
		// PX4_MFT_USB
		.present     = 1,
		.mandatory   = 1,
		.connection  = px4_hw_con_onboard,
	},
	{
		// PX4_MFT_CAN2
		.present     = 0,
		.mandatory   = 0,
		.connection  = px4_hw_con_unknown,
	},
	{
		// PX4_MFT_CAN3
		.present     = 0,
		.mandatory   = 0,
		.connection  = px4_hw_con_unknown,
	},
};

static px4_hw_mft_list_entry_t mft_lists[] = {
	{V101, hw_mft_list_v0101,        arraySize(hw_mft_list_v0101)},
};

/************************************************************************************
 * Name: board_query_manifest
 *
 * Description:
 *   Optional returns manifest item.
 *
 * Input Parameters:
 *   manifest_id - the ID for the manifest item to retrieve
 *
 * Returned Value:
 *   0 - item is not in manifest => assume legacy operations
 *   pointer to a manifest item
 *
 ************************************************************************************/

__EXPORT px4_hw_mft_item board_query_manifest(px4_hw_mft_item_id_t id)
{
	static px4_hw_mft_list_entry boards_manifest = px4_hw_mft_list_uninitialized;

	if (boards_manifest == px4_hw_mft_list_uninitialized) {
		uint32_t ver_rev = board_get_hw_version() << 8;
		ver_rev |= board_get_hw_revision();

		for (unsigned i = 0; i < arraySize(mft_lists); i++) {
			if (mft_lists[i].hw_ver_rev == ver_rev) {
				boards_manifest = &mft_lists[i];
				break;
			}
		}

		if (boards_manifest == px4_hw_mft_list_uninitialized) {
			syslog(LOG_ERR, "[boot] Board %4"  PRIx32 " is not supported!\n", ver_rev);
		}
	}

	px4_hw_mft_item rv = &device_unsupported;

	if (boards_manifest != px4_hw_mft_list_uninitialized &&
	    id < boards_manifest->entries) {
		rv = &boards_manifest->mft[id];
	}

	return rv;
}
