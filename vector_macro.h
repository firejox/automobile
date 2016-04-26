#ifndef __AUTO_MOBILE_VECTOR_MACRO_H__
#define __AUTO_MOBILE_VECTOR_MACRO_H__


#define VECTOR(TYPE) vector_##TYPE##_t 
#define VECTOR_CALLBACK(TYPE) vector_##TYPE##_callback_t
#define VECTOR_CREATE(TYPE) vector_##TYPE##_create
#define VECTOR_DESTROY(TYPE) vector_##TYPE##_destroy
#define VECTOR_APPEND(TYPE) vector_##TYPE##_append
#define VECTOR_GET_ARRAY(TYPE) vector_##TYPE##_get_array
#define VECTOR_GET_SIZE(TYPE) vector_##TYPE##_get_size
#define VECTOR_ASSIGN(TYPE) vector_##TYPE##_assign
#define VECTOR_FOR_EACH(TYPE) vector_##TYPE##_for_each
#define VECTOR_CLEAR(TYPE) vector_##TYPE##_clear

#define VECTOR_MODULE_DEC(TYPE)                                         \
    typedef struct _vector_##TYPE VECTOR(TYPE);                         \
    typedef void (*VECTOR_CALLBACK(TYPE)) (const TYPE *, void*);        \
    VECTOR(TYPE) *VECTOR_CREATE(TYPE)(void);                            \
    void VECTOR_DESTROY(TYPE)(VECTOR(TYPE)*);                           \
    size_t VECTOR_GET_SIZE(TYPE)(const VECTOR(TYPE) *);                 \
    const TYPE* VECTOR_GET_ARRAY(TYPE)(const VECTOR(TYPE) *);           \
    void VECTOR_FOR_EACH(TYPE)(const VECTOR(TYPE) *,                    \
            VECTOR_CALLBACK(TYPE), void *);                             \
    void VECTOR_APPEND(TYPE)(VECTOR(TYPE) *, TYPE);                     \
    void VECTOR_ASSIGN(TYPE)(VECTOR(TYPE) *, size_t, TYPE);             \
    void VECTOR_CLEAR(TYPE) (VECTOR(TYPE) *);                           \
    
   
#define VECTOR_MODULE_DEF(TYPE, DESTROY_FUNC)                           \
    struct _vector_##TYPE {                                             \
        size_t alloc_sz;                                                \
        size_t sz;                                                      \
        TYPE arr[];                                                     \
    };                                                                  \
    VECTOR(TYPE) *VECTOR_CREATE(TYPE)(void) {                           \
        VECTOR(TYPE) *tmp;                                              \
        tmp = xmalloc (sizeof (VECTOR(TYPE)) + sizeof(TYPE) * 64);      \
        tmp->alloc_sz = 64;                                             \
        tmp->sz = 0;                                                    \
        return tmp;                                                     \
    }                                                                   \
                                                                        \
    void VECTOR_DESTROY(TYPE)(VECTOR(TYPE) *vec) {                      \
        if (vec != NULL)                                                \
            for (int i = 0; i < vec->sz; i++)                           \
                DESTROY_FUNC(vec->arr[i]);                              \
        xfree (vec);                                                    \
    }                                                                   \
                                                                        \
    size_t VECTOR_GET_SIZE(TYPE)(const VECTOR(TYPE) *vec) {             \
        if (vec != NULL)                                                \
            return vec->sz;                                             \
        return 0;                                                       \
    }                                                                   \
                                                                        \
    const TYPE* VECTOR_GET_ARRAY(TYPE)(const VECTOR(TYPE) *vec) {       \
        if (vec != NULL)                                                \
            return vec->arr;                                            \
        return NULL;                                                    \
    }                                                                   \
                                                                        \
    void VECTOR_FOR_EACH(TYPE) (const VECTOR(TYPE) *vec,                \
            VECTOR_CALLBACK(TYPE) cb, void *user_data) {                \
        if (vec != NULL)                                                \
            for (int i = 0; i < vec->sz; i++)                           \
                cb(&vec->arr[i], user_data);                            \
    }                                                                   \
                                                                        \
    void VECTOR_APPEND(TYPE) (VECTOR(TYPE) *vec, TYPE t) {              \
        if (vec != NULL) {                                              \
            if (vec->sz == vec->alloc_sz) {                             \
                const size_t alloc_sz =                                 \
                    (vec->alloc_sz >= (SIZE_MAX>>1)) ?                  \
                        SIZE_MAX : vec->alloc_sz * 2;                   \
                vec = xrealloc (vec, sizeof (VECTOR(TYPE))              \
                        + alloc_sz * sizeof (TYPE));                    \
                vec->alloc_sz = alloc_sz;                               \
            }                                                           \
                                                                        \
            vec->arr[vec->sz] = t;                                      \
            vec->sz++;                                                  \
        }                                                               \
    }                                                                   \
    void VECTOR_ASSIGN(TYPE) (VECTOR(TYPE) *vec, size_t idx, TYPE t) {  \
        if (vec != NULL && idx < vec->sz) {                             \
            vec->arr[idx] = t;                                          \
        }                                                               \
    }                                                                   \
    void VECTOR_CLEAR(TYPE) (VECTOR(TYPE) *vec) {                       \
        if (vec != NULL) {                                              \
            vec->sz = 0;                                                \
        }                                                               \
    }

#endif
