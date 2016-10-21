#include "object.h"

UINT8 objectCount;

void merge_objects(objects_t *obj_temp,objects_t *obj_cur)
{
	obj_temp->xmin = MIN(obj_temp->xmin,obj_cur->xmin);
	obj_temp->xmax = MAX(obj_temp->xmax,obj_cur->xmax);
	obj_temp->ymin = MIN(obj_temp->ymin,obj_cur->ymin);
	obj_temp->ymax = MAX(obj_temp->ymax,obj_cur->ymax);
	
	obj_temp->blockcount += obj_cur->blockcount;
	
	return;
}

UINT8 add_block_to_object(objects_t *object,blocks_t *blk)
{
	blocks_t *blk_cur;

	blk_cur = object->head;

	while(blk_cur!=NULL)
	{
		if((abs(blk_cur->x - blk->x)<=DIFF_THRESHOLD) && 
		   (abs(blk_cur->y - blk->y)<=DIFF_THRESHOLD))
		{
			blk->next = object->head;
			object->head = blk;
			object->blockcount += 1;
			
			object->xmin = MIN(object->xmin,blk->x);
			object->xmax = MAX(object->xmax,blk->x);
			object->ymin = MIN(object->ymin,blk->y);
			object->ymax = MAX(object->ymax,blk->y);
			
			return SUCCESS;
		}
		
		blk_cur = blk_cur->next;
	}
	
	return FAIL;
}

void enclose_objects(objects_t *obj_begin,UINT8 *image)
{
	UINT16 i,j;
	UINT16 xmin,xmax,ymin,ymax;
	UINT32 area;
	objects_t *obj_cur;
	
	obj_cur = obj_begin;
	
	while(obj_cur!=NULL)
	{
		xmin = ((obj_cur->xmin)<<2);
		xmax = ((obj_cur->xmax + 1)<<2);
		ymin = ((obj_cur->ymin)<<2);
		ymax = ((obj_cur->ymax + 1)<<2);
		
		area = ((xmax - xmin)*(ymax - ymin));
		
		if(area>AREA_THRESHOLD)
		{
			for(j=xmin;j<=xmax;j++)
			{
				image[(ymin*imWidth) + j] = 0;
			}
			
			for(i=ymin+1;i<=ymax;i++)
			{
				image[(i*imWidth) + xmin] = 0;
				image[(i*imWidth) + xmax] = 0;
			}
			
			for(j=xmin;j<=xmax;j++)
			{
				image[(ymax*imWidth) + j] = 0;
			}
		}
		
		obj_cur = obj_cur->next;
	}
	
	return;
}

void get_object_map(UINT8 *pixels_per_block,UINT8 *image)
{
	UINT8 val;
	UINT32 i,j,count,total_objects;
	blocks_t *blk_cur;
	objects_t *obj_begin,*obj_cur,*obj_prev,*obj_temp;
	
	obj_begin = NULL;
	
	total_objects  = 0;
	
	for(i=0;i<vblocks;i++)
	{
		for(j=0;j<hblocks;j++)
		{
			if(pixels_per_block[(i*hblocks) + j] > PCNT_THRESHOLD)
			{
				blk_cur = create_block();
				
				blk_cur->x = j;
				blk_cur->y = i;
				
				if(obj_begin==NULL)
				{
					total_objects += 1;
					
					obj_begin = create_object();
					
					obj_begin->head = blk_cur;
					obj_begin->xmin = blk_cur->x;
					obj_begin->xmax = blk_cur->x;
					obj_begin->ymin = blk_cur->y;
					obj_begin->ymax = blk_cur->y;
					
					obj_begin->blockcount += 1;
				}
				else
				{
					count = 0;
					
					obj_cur = obj_begin;
					
					while(obj_cur!=NULL)
					{
						val = add_block_to_object(obj_cur,blk_cur);
						
						if(val==SUCCESS)
						{
							count++;
							
							if(count==1)
							{
								obj_temp = obj_cur;
							}
							else if(count>1)
							{
								merge_objects(obj_temp,obj_cur);
								obj_prev->next = obj_cur->next;
								obj_cur = obj_prev;
								
								total_objects -= 1;
							}
						}
						
						obj_prev = obj_cur;
						obj_cur = obj_cur->next;
					}
					
					if(count==0)
					{
						total_objects += 1;
						
						obj_temp = create_object();
						
						obj_temp->next = obj_begin;
						obj_begin = obj_temp;
						
						obj_begin->head = blk_cur;
						obj_begin->xmin = blk_cur->x;
						obj_begin->xmax = blk_cur->x;
						obj_begin->ymin = blk_cur->y;
						obj_begin->ymax = blk_cur->y;
						
						obj_begin->blockcount += 1;
					}
				}
			}
		}
	}
	
	enclose_objects(obj_begin,image);
	
	destroy_objects(obj_begin);
	
	return;
}

void get_diff_image(UINT8 *curr_image,UINT8 *bckgnd,UINT8 *pixels_per_block,UINT8 *diff_image)
{
	UINT8 sum,k;
	UINT32 i,j,val;
	UINT8 *ppb;
	
	for(i=0;i<vblocks;i++)
	{
		for(j=0;j<hblocks;j++)
		{
			pixels_per_block[(i*hblocks) + j] = 0;
		}
	}
	
	for(i=0;i<imHeight;i++)
	{
		ppb = (UINT8 *)(pixels_per_block + ((i>>2)*hblocks));
		for(j=0;j<hblocks;j++)
		{
			sum = *ppb;
			for(k=0;k<4;k++)
			{
				val = abs(*curr_image++ - *bckgnd++);
				if(val > LUMINANCE_THRESHOLD)
				{
					*diff_image++ = 1;
					sum += 1;
				}
				else
				{
					*diff_image++ = 254;
				}
			}
			*ppb++ = sum;
		}
	}
	
	return;
}
