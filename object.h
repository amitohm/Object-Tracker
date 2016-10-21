#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef NULL
#define NULL (void*)0
#endif

typedef char			INT8;
typedef unsigned char	UINT8;

typedef short			INT16;
typedef unsigned short	UINT16;

typedef int				INT32;
typedef unsigned int	UINT32;

#define		ABS(X)			((X) < (0) ? (-X): (X))

#define		MIN(X,Y)		((X) < (Y) ? (X) : (Y))

#define		MAX(X,Y)		((X) > (Y) ? (X) : (Y))

#define		SUCCESS					1
#define		FAIL					0

#define		LUMINANCE_THRESHOLD		16

#define		DIFF_THRESHOLD			1

#define		PCNT_THRESHOLD			8	//pixel count per block

#define		AREA_THRESHOLD			10

/* main.c */
extern UINT32 imWidth;
extern UINT32 imHeight;
extern UINT32 hblocks;
extern UINT32 vblocks;

typedef struct blocks_t blocks_t;
struct blocks_t
{
	UINT16 x;
	UINT16 y;
	blocks_t *next;
};

typedef struct objects_t objects_t;
struct objects_t
{
	UINT16 xmin;
	UINT16 xmax;
	UINT16 ymin;
	UINT16 ymax;
	
	UINT16 blockcount;
	
	blocks_t *head;
	
	objects_t *next;
};

void get_diff_image(UINT8 *curr_image,UINT8 *bckgnd,UINT8 *pixels_per_block,UINT8 *diff_image);

void get_object_map(UINT8 *pixels_per_block,UINT8 *image);

blocks_t * create_block();
objects_t * create_object();

UINT8 add_block_to_object(objects_t *object,blocks_t *blk);

void enclose_objects(objects_t *obj_begin,UINT8 *image);

void destroy_blocks(blocks_t *block);

void destroy_objects(objects_t *object);

#endif
