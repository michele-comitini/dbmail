/* $Id$
 * Functions for parsing a mime mailheader (actually just for scanning for email messages
	and parsing the messageID */

#include "config.h"
#include "mime.h"

extern char *header;
extern unsigned long headersize;

/* extern struct list mimelist;  */
extern struct list users;

/* 
 * mime_list()
 *
 * build a list of MIME header items
 * blkdata should be a NULL-terminated array
 *
 * returns -1 on failure, 0 on success
 */
int mime_list(char *blkdata, struct list *mimelist)
{
  int valid_mime_lines=0,idx;
	
  char *endptr, *startptr, *delimiter;
  struct mime_record *mr;
  struct element *el;   
	
  trace (TRACE_INFO, "mime_list(): entering mime loop");
	
  /* alloc mem */
#ifdef USE_EXIT_ON_ERROR
  memtst((mr=(struct mime_record *)malloc(sizeof(struct mime_record)))==NULL);
#else
  mr=(struct mime_record *)malloc(sizeof(struct mime_record));

  if (!mr)
    {
      trace(TRACE_ERROR, "mime_list(): out of memory\n");
      return -1;
    }
#endif

  startptr = blkdata;
  while (*startptr)
    {
      /* quick hack to jump over those naughty \n\t fields */
      endptr = blkdata;
      while (*endptr)
	{
	  if ((endptr[0]=='\n') && (endptr[1]!='\t'))
	    break;
	  endptr++;
	}

      if (!(*endptr))
	{
	  /* end of data block reached */
	}

      /* endptr points to linebreak now */
      /* MIME field+value is string from startptr till endptr */

      *endptr = '\0'; /* replace newline to terminated string */

      trace(TRACE_DEBUG,"mime_list(): captured array [%s]",startptr); 

      /* parsing tmpstring for field and data */
      /* field is name:value */

      delimiter = strchr(startptr,':');

      if (delimiter)
	{
	  /* found ':' */
	  valid_mime_lines++;
	  *delimiter = '\0'; /* split up strings */

	  /* skip all spaces and colons after the fieldname */
	  idx = 1;
	  while ((delimiter[idx]==':') || (delimiter[idx]==' ')) idx++;

	  /* &delimiter[idx] is field value, startptr is field name */
	  strcpy(mr->field, &delimiter[idx]);
	  strcpy(mr->value, &delimiter[idx]);

	  trace (TRACE_DEBUG,"mime_list(): mimepair found: [%s] [%s] \n",mr->field, mr->value); 

#ifdef USE_EXIT_ON_ERROR
	  memtst((el=list_nodeadd(mimelist,mr,sizeof (*mr)))==NULL);
#else
	  el = list_nodeadd(mimelist,mr,sizeof (*mr));
	  if (!el)
	    {
	      trace(TRACE_ERROR, "mime_list(): cannot add element to list\n");
	      free(mr);
	      return -1;
	    }
#endif
	  /* restore blkdata */
	  *delimiter = ':';
	  *endptr = '\n';
	  startptr = endptr+1; /* advance to next field */
	}
      else 
	{
	  /* no field/value delimiter found, non-valid MIME-header */
	  free(mr);
	  list_freelist(mimelist);

	  return -1;
	}
    }

  free(mr); /* no longer need this */

  trace(TRACE_DEBUG,"mime_list(): mimeloop finished");
  if (valid_mime_lines < 2)
    {
#ifdef USE_EXIT_ON_ERROR
      free(blkdata);
      trace(TRACE_STOP,"mime_list(): no valid mime headers found");
#else
      trace(TRACE_ERROR,"mime_list(): no valid mime headers found\n");
      return -1;
#endif
    }

  /* success */
  return 0;
}


/*
 * mime_findfield()
 *
 * finds a MIME header field
 *
 * returns -1 on error, 0 on success
 * 
 * NOTE: if the item is not found 0 is still returned (successfull search)
 */
int mime_findfield(const char *fname, struct list *mimelist, struct mime_record *mr)
{
  struct element *current;

  current = list_getstart(mimelist);
  while (current)
    {

  
  

int mail_adr_list_special(int offset, int max, char *address_array[]) 
{
  int mycount;

  trace (TRACE_INFO,"mail_adr_list_special(): gathering info from command line");
  for (mycount=offset;mycount!=max; mycount++)
    {
      trace(TRACE_DEBUG,"mail_adr_list_special(): adding [%s] to userlist",address_array[mycount]);
      memtst((list_nodeadd(&users,address_array[mycount],(strlen(address_array[mycount])+1)))==NULL);
    }
  return mycount;
}

  
int mail_adr_list(char *scan_for_field, struct list *targetlist, struct list *mimelist)
{
  struct element *raw;
  struct mime_record *mr;
  char *tmpvalue, *ptr,*tmp;

  trace (TRACE_DEBUG,"mail_adr_list(): mimelist currently has [%d] nodes",mimelist->total_nodes);
  if (mimelist->total_nodes==0)
    {
      /* we need to parse the header first 
	 this is because we're in SPECIAL_DELIVERY mode so
	 normally we wouldn't need any scanning */
      trace (TRACE_INFO,"mail_adr_list(): parsing mimeheader from message");
      mime_list(header,headersize);
    }
  
  memtst((tmpvalue=(char *)calloc(MIME_VALUE_MAX,sizeof(char)))==NULL);

  trace (TRACE_INFO,"mail_adr_list(): mail address parser starting");

  raw=list_getstart(mimelist);
  trace (TRACE_DEBUG,"mail_adr_list(): total fields in header %lu",mimelist->total_nodes);
  while (raw!=NULL)
    {
      mr=(struct mime_record *)raw->data;
      trace (TRACE_DEBUG,"mail_adr_list(): scanning for %s",scan_for_field);
      if ((strcasecmp(mr->field, scan_for_field)==0))
	{
	  /* Scan for email addresses and add them to our list */
	  /* the idea is to first find the first @ and go both ways */
	  /* until an non-emailaddress character is found */
	  ptr=strstr(mr->value,"@");
	  while (ptr!=NULL)
	    {
				/* found an @! */
				/* first go as far left as possible */
	      tmp=ptr;
	      while ((tmp!=mr->value) && 
		     (tmp[0]!='<') && 
		     (tmp[0]!=' ') && 
		     (tmp[0]!='\0') && 
		     (tmp[0]!=','))
		tmp--;
	      if ((tmp[0]=='<') || (tmp[0]==' ') || (tmp[0]=='\0')
		  || (tmp[0]==',')) tmp++;
	      while ((ptr!=NULL) &&
		     (ptr[0]!='>') && 
		     (ptr[0]!=' ') && 
		     (ptr[0]!=',') &&
		     (ptr[0]!='\0'))  
		ptr++;
	      memtst((strncpy(tmpvalue,tmp,ptr-tmp))==NULL);
				/* always set last value to \0 to end string */
	      tmpvalue[ptr-tmp]='\0';

				/* one extra for \0 in strlen */
	      memtst((list_nodeadd(targetlist,tmpvalue,
				   (strlen(tmpvalue)+1)))==NULL);

				/* printf ("total nodes:\n");
				   list_showlist(&users);
				   next address */
	      ptr=strstr(ptr,"@");
	      trace (TRACE_DEBUG,"mail_adr_list(): found %s, next in list is %s",
		     tmpvalue,ptr);
	    }
	}
      raw=raw->nextnode;
    }

  free(tmpvalue);

  trace (TRACE_DEBUG,"mail_adr_list(): found %d emailaddresses",list_totalnodes(targetlist));
	
  trace (TRACE_INFO,"mail_adr_list(): mail address parser finished");

  if (list_totalnodes(&users)==0) /* no addresses found */
    return -1;
  return 0;
}
