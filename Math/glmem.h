#ifndef __GL_MEM_H__
#define __GL_MEM_H__

#define MALLOC(object_ptr) object_ptr = malloc(sizeof(object_ptr[0]))
#define MALLOCZ(object_ptr) object_ptr = malloc(sizeof(object_ptr[0])); memset(object_ptr, 0, sizeof(object_ptr[0]))
#define CALLOC(count, collection_ptr) collection_ptr = calloc(count, sizeof(collection_ptr[0]))
#define CALLOCZ(count, collection_ptr) collection_ptr = calloc(count, sizeof(collection_ptr[0])); memset(collection_ptr, 0, count * sizeof(collection_ptr[0]))

#endif