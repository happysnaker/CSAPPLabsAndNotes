/*
Team Name:happysnaker
Member 1 :qwewqewqqe:qwewqewqqe
Using default tracefiles in traces/
Perf index = 44 (util) + 40 (thru) = 84/100
*/



/*
 * mm-naive.c - The fastest, least memory-efficient malloc package.
 *
 * In this naive approach, a block is allocated by simply incrementing
 * the brk pointer.  A block is pure payload. There are no headers or
 * footers.  Blocks are never coalesced or reused. Realloc is
 * implemented directly using mm_malloc and mm_free.
 *
 * NOTE TO STUDENTS: Replace this header comment with your own header
 * comment that gives a high level description of your solution.
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "mm.h"
#include "memlib.h"

/*********************************************************
 * NOTE TO STUDENTS: Before you do anything else, please
 * provide your team information in the following struct.
 ********************************************************/
team_t team =
{
    /* Team name */
    "Xwqe",
    /* First member's full name */
    "qewqe",
    /* First member's email address */
    "sadsa",
    /* Second member's full name (leave blank if none) */
    "",
    /* Second member's email address (leave blank if none) */
    ""
};

/* single word (4) or double word (8) alignment */
#define ALIGNMENT 8

/* rounds up to the nearest multiple of ALIGNMENT */
#define ALIGN(size) (((size) + (ALIGNMENT-1)) & ~0x7)


#define SIZE_T_SIZE (ALIGN(sizeof(size_t)))

#define WSIZE 4
#define DSIZE 8           /*Double word size*/
#define CHUNKSIZE    (1<<12) /*the page size in bytes is 4K*/
#define MAX(x,y)     ((x)>(y)?(x):(y))
#define PACK(size,alloc)    ((size) | (alloc))
#define GET(p)       (*(unsigned int *)(p))
#define PUT(p,val)   (*(unsigned int *)(p) = (val))
#define GET_SIZE(p)  (GET(p) & ~0x7)
#define GET_ALLOC(p) (GET(p) & 0x1)
#define HDRP(bp)     ((char *)(bp)-WSIZE)
#define FTRP(bp)     ((char *)(bp)+GET_SIZE(HDRP(bp))-DSIZE)
#define NEXT_RP(bp)  ((unsigned int)(bp)+WSIZE)
#define NEXT_BLKP(bp)   ((char *)(bp)+GET_SIZE(((char *)(bp)-WSIZE)))
#define PREV_BLKP(bp)   ((char *)(bp)-GET_SIZE(((char *)(bp)-DSIZE)))


int mm_check(char *function);
static void *extend_heap(size_t dwords);
static void *coalesce(void *bp);
static void *find_fit(size_t size);
static void place(void *bp,size_t asize);
void __insert(char *p);
void __remove(char *p);
static char *heap_listp = NULL;
static char *root = NULL;

/*
 * mm_init - initialize the malloc package.
 * The return value should be -1 if there was a problem in performing the initialization, 0 otherwise
 */
int mm_init(void){
    if((heap_listp = mem_sbrk(6*WSIZE))==(void *)-1) return -1;
    PUT(heap_listp,0);
    PUT(heap_listp+(1*WSIZE),0);
    PUT(heap_listp+(2*WSIZE),0);
    PUT(heap_listp+(3*WSIZE),PACK(DSIZE,1));
    PUT(heap_listp+(4*WSIZE),PACK(DSIZE,1));
    PUT(heap_listp+(5*WSIZE),PACK(0,1));
    root = heap_listp + (1*WSIZE);
    heap_listp += (4*WSIZE);
    if((extend_heap(CHUNKSIZE/DSIZE)) == NULL) 
		return -1;
    return 0;
}


/*
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size){
    size_t asize;
    size_t extendsize;
    char *bp;
    if(size ==0)
	 return NULL;
    if(size <= DSIZE)
        asize = 2*(DSIZE);
    else
        asize = (DSIZE) * ((size + DSIZE + (DSIZE-1)) / (DSIZE));
    if((bp = find_fit(asize))!= NULL){
        place(bp,asize);
        return bp;
    }

    extendsize = MAX(asize,CHUNKSIZE);
    if((bp = extend_heap(extendsize/DSIZE))==NULL)
        return NULL;
    place(bp,asize);
    return bp;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *bp){
    if(bp == NULL)
        return;
    size_t size = GET_SIZE(HDRP(bp));
    PUT(HDRP(bp), PACK(size, 0));
    PUT(FTRP(bp), PACK(size, 0));
    PUT(NEXT_RP(bp),0);
    PUT(bp,0);
    coalesce(bp);
}
/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size){
    
    void *newptr = mm_malloc(size);
	if(!newptr){
        return 0;
    }
    if(ptr == NULL){
        return newptr;
    }

    size_t oldsize = GET_SIZE(HDRP(ptr));
    memcpy(newptr, ptr, oldsize < size ? oldsize : size);
    mm_free(ptr);

    return newptr;
}
static void *find_fit(size_t size){
    /*first fit*/
    char *bp = GET(root);
    while(bp != NULL){
        if(GET_SIZE(HDRP(bp)) >= size) 
			return bp;	
        bp = GET(NEXT_RP(bp));
    }
    return NULL;

}

static void place(void *bp,size_t asize)
{
    size_t csize = GET_SIZE(HDRP(bp));
	__remove(bp);
    if(csize-asize >= 2 *DSIZE){
        PUT(HDRP(bp),PACK(asize,1));
        PUT(FTRP(bp),PACK(asize,1));
        PUT(HDRP(NEXT_BLKP(bp)),PACK(csize-asize,0));
        PUT(FTRP(NEXT_BLKP(bp)),PACK(csize-asize,0));
        PUT(NEXT_RP(bp),0);
        PUT((NEXT_BLKP(bp)),0);
        coalesce(NEXT_BLKP(bp));
    }
    else{
        PUT(HDRP(bp),PACK(csize,1));
        PUT(FTRP(bp),PACK(csize,1));
    }
}

static void *extend_heap(size_t words)
{
    char *bp;
    size_t size;
    size = (words % 2) ? (words+1) * DSIZE : words * DSIZE;
    if((long)(bp = mem_sbrk(size))==(void *)-1)
        return NULL;
    PUT(HDRP(bp),PACK(size,0));
    PUT(FTRP(bp),PACK(size,0));
    PUT(HDRP(NEXT_BLKP(bp)),PACK(0,1));
    PUT(NEXT_RP(bp),0);
    PUT(bp,0);
    return coalesce(bp);
}

/*coalesce the empty block*/
static void *coalesce(void *bp)
{
    size_t  prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
    size_t  next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
    size_t size = GET_SIZE(HDRP(bp));

    /*coalesce the block and change the point*/
    if(prev_alloc && !next_alloc){
        size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
        __remove(NEXT_BLKP(bp));/*remove from empty list*/
        PUT(HDRP(bp), PACK(size,0));
        PUT(FTRP(bp), PACK(size,0));
    }
    else if(!prev_alloc && next_alloc){
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        __remove(PREV_BLKP(bp));
        PUT(FTRP(bp),PACK(size,0));
        PUT(HDRP(PREV_BLKP(bp)),PACK(size,0));
        bp = PREV_BLKP(bp);
    }
    else if (!prev_alloc && !next_alloc){
        size +=GET_SIZE(FTRP(NEXT_BLKP(bp)))+ GET_SIZE(HDRP(PREV_BLKP(bp)));
        __remove(PREV_BLKP(bp));
        __remove(NEXT_BLKP(bp));
        PUT(FTRP(NEXT_BLKP(bp)),PACK(size,0));
        PUT(HDRP(PREV_BLKP(bp)),PACK(size,0));
        bp = PREV_BLKP(bp);
    }
    __insert(bp);
    return bp;
}
inline void __insert(char *bp)
{
	//PUT(bp, root);
    char *next = GET(root);
    if(next != NULL)
        PUT(next, bp);
    PUT(NEXT_RP(bp), next);
    PUT(root,bp);
}
inline void __remove(char *bp){
    char *pre = GET((bp));
    char *next = GET(NEXT_RP(bp));
    if(pre == NULL){
        if(next != NULL)
			PUT(next,0);
        PUT(root,next);
    }
    else{
        if(next != NULL)
			PUT((next),pre);
        PUT(NEXT_RP(pre),next);
    }
    PUT(NEXT_RP(bp),0);
    PUT(bp,0);
}






