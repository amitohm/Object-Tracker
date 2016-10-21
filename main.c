#include "object.h"

UINT32 imWidth;
UINT32 imHeight;
UINT32 imDepth;
UINT32 hblocks;
UINT32 vblocks;

FILE *fp;

blocks_t * create_block()
{
	blocks_t *block;
	
	block = (blocks_t *)malloc(sizeof(blocks_t));
	if(NULL==block)
	{
		printf("%s: Out Of Memory\n",__func__);
		return block;
	}	

	block->x = 0;
	block->y = 0;
	block->next = NULL;
	
	return block;
}

objects_t * create_object()
{
	objects_t *object;
	
	object = (objects_t *)malloc(sizeof(objects_t));
	if(NULL==object)
	{
		printf("%s: Out Of Memory\n",__func__);
		return object;
	}

	object->blockcount = 0;
	object->xmin = object->xmax = 0;
	object->ymin = object->ymax = 0;

	object->head = NULL;
	object->next = NULL;
	
	return object;
}

void destroy_blocks(blocks_t *block)
{
	blocks_t *dead_block;
	
	while(block!=NULL)
	{
		dead_block = block;
		block = block->next;
		free(dead_block);
	}
	
	return;
}

void destroy_objects(objects_t *object)
{
	objects_t *dead_object;
	
	while(object!=NULL)
	{
		destroy_blocks(object->head);
		dead_object = object;
		object = object->next;
		free(dead_object);
	}
	
	return;
}

void read_pgm_header()
{
	INT8 byte;
	
	byte = fgetc(fp);
	if(byte != 'P')
	{
		printf("Fatal error: 'P' not found. Invalid ppm file.\n");
		exit(-1);
	}
	
	byte = fgetc(fp);
	if(byte != '5')
	{
		printf("Fatal error: 'P' not followed by 6. Unsupported ppm file.\n");
		exit(-1);
	}
	
	byte = fgetc(fp);//newline
	
	//get rid of those damn comments!
	byte = fgetc(fp);
	if (byte=='#')
	{
		do{ byte = fgetc(fp); } while (byte != '\n');//tolerate crap
	}
	else
	{
		ungetc((INT32)byte,fp);//wow! no crap!
	}
	
	fscanf(fp,"%d %d\n%d",&imWidth,&imHeight,&imDepth);
	fgetc(fp);//the damn '\n' after depth!
	
	return;
}

INT32 main()
{
	UINT8 first_file = 1;
	UINT8 fileIn[100];
	INT32 fileNo = 1;
	
	UINT8 *bckgnd,*curr_image,*diff_image,*pixels_per_block;
	
	//fp = fopen("input_images/image0600.pgm","rb");
	fp = fopen("bckgnd.pgm","rb");
	if(fp==NULL)
	{
		printf("Error opening file image0600.pgm.\n");
		exit(1);
	}
	
	read_pgm_header();
	
	hblocks = (imWidth>>2);
	vblocks = (imHeight>>2);
	
	bckgnd = (UINT8 *)malloc(imWidth*imHeight);
	if(bckgnd==NULL)
	{
		printf("Error allocating memory for bckgnd.\n");
		exit(1);
	}
	curr_image = (UINT8 *)malloc(imWidth*imHeight);
	if(curr_image==NULL)
	{
		printf("Error allocating memory for curr_image.\n");
		exit(1);
	}
	diff_image = (UINT8 *)malloc(imWidth*imHeight);
	if(diff_image==NULL)
	{
		printf("Error allocating memory for diff_image.\n");
		exit(1);
	}
	pixels_per_block = (UINT8 *)malloc(hblocks*vblocks);
	if(pixels_per_block==NULL)
	{
		printf("Error allocating memory for pixels_per_block.\n");
		exit(1);
	}
	
	fread(bckgnd,1,(imHeight * imWidth),fp);
	fclose(fp);
	
	//while(fileNo < 500)
	while(fileNo < 2)
	{
		printf("current file number = %d\n",fileNo);
		sprintf(fileIn,"input_images/image%04d.pgm",fileNo);
		//fp = fopen(fileIn,"rb");
		fp = fopen("forgnd.pgm","rb");
		if(fp==NULL)
		{
			printf("Error opening file %s.\n",fileIn);
			exit(1);
		}
		
		read_pgm_header();
		
		fread(curr_image,1,(imHeight * imWidth),fp);
		fclose(fp);
		
		first_file = 0;
		
		sprintf(fileIn,"output_images/output%04d.pgm",fileNo);
		//fp = fopen(fileIn,"wb");
		fp = fopen("diff.pgm","wb");
		if(fp==NULL)
		{
			printf("Error opening file %s.\n",fileIn);
			exit(1);
		}
		fprintf(fp,"P5\n%d %d\n%d\n",imWidth,imHeight,imDepth);
		
		fileNo += 1;
		
		get_diff_image(curr_image,bckgnd,pixels_per_block,diff_image);
		
		get_object_map(pixels_per_block,diff_image);
		
		fwrite(diff_image,1,(imWidth*imHeight),fp);
		fclose(fp);
	}
	
	free(bckgnd);
	free(curr_image);
	free(diff_image);
	//free(pixels_per_block);
	
	return 0;
}
