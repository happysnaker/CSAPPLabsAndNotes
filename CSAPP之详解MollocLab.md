## 实验材料与规则
官网的实习手册是缺了测试材料的，所以我建议你直接从这里下载吧：
[https://github.com/happysnaker/CSAPPLabs/tree/CSAPP/molloc-Lab](https://github.com/happysnaker/CSAPPLabs/tree/CSAPP/molloc-Lab)

实验仅要求我们修改mm.c文件，要求完成init, molloc, free, recalloc 四个函数，其中部分与系统接口的函数已在memlib.c文件中给出。

完成之后，我们需要键入以下三条命令来跑分：
```
make clean
make mdriver
./mdriver

```
这个实验难度特别大，其中非常容易碰到段错误，以至于我最后一个分离式的链表仍然没有实现，实乃一个遗憾。立个flag，2021年一定要实现它！

## 实验部分
#### 隐式空闲链表加首次适配
首次适配从头遍历链表，找到空闲块。这个跑分不是很理想，甚至还不及格，只有56分。这个就是书上的思路，其中要自己完善两个函数。
代码：[隐式空闲链表加首次适配实现](https://paste.liumingye.cn/1949)

#### 隐式空闲链表加下一次次适配
下一次适配从上次操作的时候开始遍历。这种方式竟然出奇的理想，跑了82分:
```
/*
Team Name:happysnaker
Member 1 :qwewqewqqe:qwewqewqqe
Using default tracefiles in traces/
Perf index = 42 (util) + 40 (thru) = 82/100
*/
```
我们留下一个bookstr来表示上一次操作时的指针，由于我们每一次释放空闲块都让这个指针指向了空闲块，所以下一次分配时速度非常块，可以看到效率部分拿了满分(40)，但利用率却不理想。

代码：
```
/*
Team Name:happysnaker
Member 1 :qwewqewqqe:qwewqewqqe
Using default tracefiles in traces/
Perf index = 42 (util) + 40 (thru) = 82/100
*/
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include "mm.h"
#include "memlib.h"
team_t team = {
    /* Team name */
    "ateam",
    /* First member's full name */
    "Harry Bovik",
    /* First member's email address */
    "bovik@cs.cmu.edu",
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
#define DSIZE 8
#define CHUNKSIZE (1 << 12)
#define MAX(X, Y) ((X) > (Y) ? (X) : (Y))
#define PACK(size, alloc) ((size) | (alloc))  /*设置头部或脚部*/
#define GET(p) (*(unsigned int*)(p))
#define PUT(p, val) (*(unsigned int*)(p) = (val))
#define GET_SIZE(P) (GET(P) & ~0X7)  
#define GET_ALLOC(P) (GET(P) & 0X1)
#define HDRP(bp) ((char*)(bp) - WSIZE)  /*获取头部指针*/
#define FTRP(bp) ((char*)(bp) + GET_SIZE(HDRP(bp)) - DSIZE) /*根据头部获取脚部指针*/
#define NEXT_BLKP(bp) ((char*)(bp) + GET_SIZE(((char*)(bp) - WSIZE)))  /*下一个指向有效载荷的指针*/
#define PREV_BLKP(bp) ((char*)(bp) - GET_SIZE(((char*)(bp) - DSIZE)))
static void* extend_heap(size_t words);
static void* coalesce(void* bp);
static void *find_fit(size_t size);
static void place(void *bp,size_t asize);
static char *heap_listp = 0;
static void* bookptr;
/* 
 * mm_init - initialize the malloc package.
 */
int mm_init(void)
{
    if ((heap_listp = mem_sbrk(4 * WSIZE)) == (void*)-1)
	    return -1;
    PUT(heap_listp + (1 * WSIZE), PACK(DSIZE, 1));
    PUT(heap_listp + (2 * WSIZE), PACK(DSIZE, 1));
    PUT(heap_listp + (3 * WSIZE), PACK(0, 1));
    heap_listp += (2 * WSIZE);
    bookptr = (unsigned int)heap_listp + DSIZE;  /*初始化*/
    if (extend_heap(CHUNKSIZE/WSIZE) == NULL)
	    return -1;
    return 0;
}

static void* extend_heap(size_t words){
	char* bp;
	size_t size;
	size = (words % 2) ? (words + 1) * WSIZE : words * WSIZE;
	if ((long)(bp = mem_sbrk(size)) == -1)
		return NULL;
	PUT(HDRP(bp), PACK(size, 0));
	PUT(FTRP(bp), PACK(size, 0));
	PUT(HDRP(NEXT_BLKP(bp)), PACK(0, 1));
	bookptr = coalesce(bp);  /*让它指向空闲指针*/
	return bookptr;
}

static void* coalesce(void* bp){
	size_t prev_alloc = GET_ALLOC(FTRP(PREV_BLKP(bp)));
	size_t next_alloc = GET_ALLOC(HDRP(NEXT_BLKP(bp)));
	size_t size = GET_SIZE(HDRP(bp));
	if (prev_alloc && next_alloc)
		return bp;
	else if (prev_alloc && !next_alloc){
		size += GET_SIZE(HDRP(NEXT_BLKP(bp)));
		PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
		PUT(HDRP(bp), PACK(size, 0));
	}
	else if (!prev_alloc && next_alloc){
		size += GET_SIZE(HDRP(PREV_BLKP(bp)));
		PUT(FTRP(bp), PACK(size, 0));
		PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
		bp = PREV_BLKP(bp);
	}
	else{
		size += GET_SIZE(HDRP(PREV_BLKP(bp))) + GET_SIZE(HDRP(NEXT_BLKP(bp)));
		PUT(HDRP(PREV_BLKP(bp)), PACK(size, 0));
		PUT(FTRP(NEXT_BLKP(bp)), PACK(size, 0));
		bp = PREV_BLKP(bp);
	}
	return bp;
}
/* 
 * mm_malloc - Allocate a block by incrementing the brk pointer.
 *     Always allocate a block whose size is a multiple of the alignment.
 */
void *mm_malloc(size_t size)
{
	char *bp;
        size_t asize;
	size_t extendsize;
	if (size <= 0)
		return NULL;
	if (size <= DSIZE)
		asize = 2 * DSIZE;
	else
		asize = DSIZE * ((size + DSIZE + (DSIZE - 1)) / DSIZE);
	if ((bp = find_fit(asize)) != NULL){
		place(bp, asize);
		bookptr = bp;
		return bp;
	}
	extendsize = MAX(asize, CHUNKSIZE);
	if ((bp = extend_heap(extendsize / WSIZE)) == NULL)
		return NULL;
	place(bp, asize);
	bookptr = bp;  /*指向上一个分配的块*/
	return bp;
}

/*
 * mm_free - Freeing a block does nothing.
 */
void mm_free(void *ptr)
{
	size_t size = GET_SIZE(HDRP(ptr));
	PUT(HDRP(ptr), PACK(size, 0));
	PUT(FTRP(ptr), PACK(size, 0));
	bookptr = coalesce(ptr);  /*指向空闲块*/
	return;
}

/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */

static void *find_fit(size_t size){
    while (GET_SIZE(HDRP(bookptr)) > 0){
		if (GET_SIZE(HDRP(bookptr)) >= size && GET_ALLOC(HDRP(bookptr)) == 0)
			return bookptr;
		bookptr = NEXT_BLKP(bookptr); /*别忘了更新它*/
	}
    return NULL;
}
static void place(void *bp,size_t asize){
   size_t nowsize = GET_SIZE(HDRP(bp));
	if (nowsize - asize < 2 * DSIZE){
		PUT(HDRP(bp), PACK(nowsize, 1));
		PUT(FTRP(bp), PACK(nowsize, 1));
	}
	else{
		PUT(HDRP(bp), PACK(asize, 1));
		PUT(FTRP(bp), PACK(asize, 1));
		PUT(HDRP(NEXT_BLKP(bp)), PACK(nowsize - asize, 0));
		PUT(FTRP(NEXT_BLKP(bp)), PACK(nowsize - asize, 0));
	}
	return;
}
/*
 * mm_realloc - Implemented simply in terms of mm_malloc and mm_free
 */
void *mm_realloc(void *ptr, size_t size)
{
    size_t oldsize = GET_SIZE(HDRP(ptr));
	void* newptr = mm_malloc(size);
	if (!newptr)
		return 0;
	bookptr = newptr;  /*指向空闲块*/
	if (!ptr)
		return newptr;
	memcpy(newptr, ptr, (size < oldsize ? size : oldsize));
	mm_free(ptr);
	return newptr;
}

```


#### 隐式空闲链表加最佳适配
找到最小适合的块，提高利用率，这个跑分不是很理想，只有56分。代码：
[隐式空闲链表加最佳适配](https://paste.liumingye.cn/1950)


#### 显式空闲链表加FIFO
这一次采用双向链表，块的大小至少需要16字节，当前块指针bp指向的四字节保存着上一空闲块的指针，bp + 4保存着下一空闲块的指针。当我们有空闲块时，我们简单的插入到根节点处。
```
/*
Team Name:happysnaker
Member 1 :qwewqewqqe:qwewqewqqe
Using default tracefiles in traces/
Perf index = 45 (util) + 40 (thru) = 85/100
*/
```
代码：
```
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
#define PREV_BLKP(bp)   ((char *)(bp)-GET_SIZE(((char *)(bp)-DSIZE)))
#define NEXT_BLKP(bp)   ((char *)(bp)+GET_SIZE(((char *)(bp)-WSIZE)))
#define NEXT_RP(bp)  ((unsigned int)(bp)+WSIZE) /*指向下一空闲块指针*/

int mm_check(char *function);
static void *extend_heap(size_t dwords);
static void *coalesce(void *bp);
static void *find_fit(size_t size);
static void place(void *bp,size_t asize);
void __insert(char *p);
void __remove(char *p);
static char *heap_listp = NULL;
static char *root = NULL; /*这是我们的根节点*/

/*
 * mm_init - initialize the malloc package.
 * The return value should be -1 if there was a problem in performing the initialization, 0 otherwise
 */
int mm_init(void){
    if((heap_listp = mem_sbrk(6*WSIZE))==(void *)-1) return -1;
    PUT(heap_listp,0);
    PUT(heap_listp+(1*WSIZE),0);
    PUT(heap_listp+(2*WSIZE),0); /*内存对齐*/
    PUT(heap_listp+(3*WSIZE),PACK(DSIZE,1));
    PUT(heap_listp+(4*WSIZE),PACK(DSIZE,1)); /*内存对齐*/
    PUT(heap_listp+(5*WSIZE),PACK(0,1)); /*尾巴*/
    root = heap_listp + (1*WSIZE);/*初始化根节点*/
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
    PUT(NEXT_RP(bp),0); /*刚释放的块，设置前后指针为null*/
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
	__remove(bp);   /*使用了这个块，切记移除它*/
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
        __remove(NEXT_BLKP(bp)); /*使用了这个块，切记移除它*/
        PUT(HDRP(bp), PACK(size,0));
        PUT(FTRP(bp), PACK(size,0));
    }
    else if(!prev_alloc && next_alloc){
        size += GET_SIZE(HDRP(PREV_BLKP(bp)));
        __remove(PREV_BLKP(bp));/*使用了这个块，切记移除它*/
        PUT(FTRP(bp),PACK(size,0));
        PUT(HDRP(PREV_BLKP(bp)),PACK(size,0));
        bp = PREV_BLKP(bp);
    }
    else if (!prev_alloc && !next_alloc){
        size +=GET_SIZE(FTRP(NEXT_BLKP(bp)))+ GET_SIZE(HDRP(PREV_BLKP(bp)));
        __remove(PREV_BLKP(bp));/*使用了这个块，切记移除它*/
        __remove(NEXT_BLKP(bp));/*使用了这个块，切记移除它*/
        PUT(FTRP(NEXT_BLKP(bp)),PACK(size,0));
        PUT(HDRP(PREV_BLKP(bp)),PACK(size,0));
        bp = PREV_BLKP(bp);
    }
    __insert(bp);/*新的空闲块，记住插入*/
    return bp;
}
inline void __insert(char *bp)
{
    //PUT(bp, root); 这个操作有段错误，挺离谱的
    char *next = GET(root);
    if(next != NULL)
        PUT(next, bp);
    PUT(NEXT_RP(bp), next);
    PUT(root,bp);
}
inline void __remove(char *bp){
    char *pre = GET((bp));
    char *next = GET(NEXT_RP(bp));
    /*因为上面插入操作不能置bp的前驱为root,所以这个是必须考虑的，这个代表前面就是根节点了 */
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


```


插入操作设置前驱为root竟然有段错误，真是折杀我也，也许是因为不经意间会访问NEXT_RP(root)吧，这个值被我们设置成了NULL。哎，由衷敬佩操作系统开发者，和内存打交道太不容易了。

其实这个在插入时我觉得还可以以升序的方式插入，这样可以提高利用率。可惜了，还是我们的老朋友段错误，哎。

看来想要满绩(90)必须要采用分离适配了，最近被段错误弄的有点晕，不过立个flag，2021年一定要完成它！
