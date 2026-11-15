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
    size16_t node_name_size,
    size16_t node_data_size,
    size16_t node_flags_size,
    size16_t metadata_size
);

GALNS_API GALNS_Nodesystem *GALNS_LoadNodesystem(
    const GALNS_IOInterface *io_stream,
    const char *filename
);

GALNS_API GALNS_NodeIndexOrError GALNS_GetEndNodeIndex(GALNS_Nodesystem *nodesystem);

/**
 * @param dst_metadata_size Get fix metadata size. It may be NULL to pass
 * @return Return metadata pointer directly. No need to free it manually
 */
GALNS_API void *GALNS_GetNodesystemMetadata(GALNS_Nodesystem *nodesystem, size_t *dst_metadata_size);

GALNS_API int GALNS_SyncNodeHeader(GALNS_Nodesystem *nodesystem);

GALNS_API void GALNS_ReleaseNodesystem(GALNS_Nodesystem *nodesystem);

#endif