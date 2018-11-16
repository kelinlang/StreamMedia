//
// Created by dengjun on 2018/11/16.
//

#ifndef STREAMMEDIADEMO_CL_SDL_ARRAY_H
#define STREAMMEDIADEMO_CL_SDL_ARRAY_H


#include <stdlib.h>
#include <string.h>

typedef struct SdlArray{
    void **elements;
    int capacity;
    int cur_size;
} SdlArray;


inline static int sdl_array_grow(SdlArray *array, int new_capacity){
    if (array == NULL){
        return -1;
    }

    if (array->capacity >= new_capacity){
        return 0;
    }
    void **new_elements = (void **)realloc(array->elements, sizeof(void*)* new_capacity);
    if(!new_elements){
        return -1;
    }

    array->elements = new_elements;
    array->capacity = new_capacity;
    return 0;
}


inline static int sdl_array_init(SdlArray *array,int capacity){
    if (array == NULL){
        return -1;
    }

    memset(array,0, sizeof(array));
    if (sdl_array_grow(array,capacity)){
        return -1;
    }
    return 0;
}

inline static int sdl_array_push_back(SdlArray *array, void *element){
    if (array == NULL){
        return -1;
    }
    if (array->cur_size >= array->capacity){
        if (sdl_array_grow(array,array->capacity * 2)){
            return -1;
        }
    }
    array->elements[array->cur_size++] = element;
    return 0;
}

inline static void *sdl_array_pop_back(SdlArray *array){
    if (array == NULL){
        return NULL;
    }
    return array->elements[--array->cur_size];
}

inline static void sdl_array_clear(SdlArray *array){
    if (array != NULL){
        array->cur_size = 0;
    }
}

inline static void *sdl_array_at(SdlArray *array, int pos){
    if (array == NULL){
        return NULL;
    }
    if (pos < 0 || pos >= array->cur_size){
        return NULL;
    }
    return array->elements[pos];
}

inline static int sdl_array_size(SdlArray *array){
    if (array == NULL){
        return -1;
    }
    return array->cur_size;
}

inline static void  **sdl_array_begin(SdlArray *array){
    if(array == NULL){
        return NULL;
    }
    return array->elements;
}

inline static void  **sdl_array_end(SdlArray *array){
    if(array == NULL){
        return NULL;
    }
    return array->elements + array->cur_size;
}


inline static void  *sdl_array_back(SdlArray *array){
    if(array == NULL){
        return NULL;
    }
    if (array->cur_size < 1){
        return NULL;
    }
    return array->elements[array->cur_size -1];
}
#endif //STREAMMEDIADEMO_CL_SDL_ARRAY_H
