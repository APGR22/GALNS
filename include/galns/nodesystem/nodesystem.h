#ifndef _GALNS_NODESYSTEM_H_
#define _GALNS_NODESYSTEM_H_

#include "../macro.h"
#include "../io.h"
#include "../type.h"

#define GALNS_MAX_NODESYSTEM_METADATA_SIZE 2048

typedef struct GALNS_Nodesystem GALNS_Nodesystem;

GALNS_API GALNS_Nodesystem *GALNS_CreateNodesystem(
    const GALNS_IOInterface *io_stream,
    const char *filename,
    size16_t node_data_size,
    size16_t node_flags_size,
    size16_t node_hash_size,
    size16_t metadata_size
);

/**
 * @param dst_node_data_size Get node data size. It may be NULL to pass
 * @param dst_node_flags_size Get node flags size. It may be NULL to pass
 * @param dst_node_hash_size Get node hash size. It may be NULL to pass
 * @param dst_metadata_size Get metadata size. It may be NULL to pass
 */
GALNS_API GALNS_Nodesystem *GALNS_LoadNodesystem(
    const GALNS_IOInterface *io_stream,
    const char *filename,
    size16_t *dst_node_data_size,
    size16_t *dst_node_flags_size,
    size16_t *dst_node_hash_size,
    size16_t *dst_metadata_size
);

GALNS_API GALNS_NodeIndexOrError GALNS_GetEndNodeIndex(GALNS_Nodesystem *nodesystem);

/**
 * @param dst_metadata_size Get fix metadata size. It may be NULL to pass
 * @return Return metadata pointer directly. No need to free it manually
 */
GALNS_API void *GALNS_GetNodesystemMetadata(GALNS_Nodesystem *nodesystem);

GALNS_API int GALNS_SyncNodeHeader(GALNS_Nodesystem *nodesystem);

GALNS_API void GALNS_ReleaseNodesystem(GALNS_Nodesystem *nodesystem);

#endif