/*	node.c -- parse a node
 */

#include "aml.h"
#include "stdlib.h"

/**new_node
 *	allocate a new event node
 */
node *new_node()
{
	node *temp;
	temp = (node *)malloc( sizeof(node) );
	if( temp == NULL ) {
		printf( "%s: couldn't allocate a node\n", pgmName );
		exit(1);
	}
	temp->next = NULL;
	temp->xlink = (void *)temp;
	nnodes++;
	return temp;
}

/**free_node
 *	free a no longer needed event node
 */
void free_node( node *n )
{
	free( n );
	nnodes--;
}

/**free_node_list
 *	free a list of nodes
 */
void free_node_list( node *list )
{
	node *np,*tnp;
	np = list;
	while( np != NULL ) {
		tnp = np;
		np = np->next;
		free_node(tnp);
	}
}

/**insert_node
 *	insert a node into a node list in start time order.  this is a
 *	"stable" insert, in the sense that the new node is inserted AFTER
 *	any nodes with the same start time.  that is, order of insertion
 *	in a list is significant.  in most applications we know that the
 *	start time for the node to be inserted is >= the start time of the
 *	node at the head of the list...
 */
node *insert_node( node *theList, node *theNode )
{
	node *np,*p;
	if( theNode == NULL || theList == NULL ) return;
	if( theNode->start < theList->start ) {
		theNode->next = theList;
		return theNode;
	}
	p = theList;
	np = p->next;
	while( np != NULL && np->start <= theNode->start ) {
		p = np; /* and the computational universe collapses...*/
		np = np->next;
	}
	p->next = theNode;
	theNode->next = np;
	return theList;
}

/**merge_node_lists
 *	merge two node lists together in order of start time.
 *	this is a stable merge, in that nodes from list 1 always
 *	go after nodes from list 0 with the same start time
 */
node * merge_node_lists( node *np0, node *np1 )
{
	node * p;
	node * result;
	
	if( np0 == NULL ) return np1;
	result = p = np0;
	while( np0 != NULL && np1 != NULL ) {
		if( np0->start > np1->start ) {
			p->next = np1;
			p = np1;
			np1 = np1->next;
			while( np1!=NULL && np0->start > np1->start ){
				p = np1;
				np1 = np1->next;
			}
			p->next = np0;
			p = np0;
			np0 = np0->next;
		} else {
			p = np0;
			np0 = np0->next;
		}
	}
	if( np0 == NULL ) p->next = np1;
	return result;
}


void dmp_list(node *list)
{
	node *np;
	int i = 0;
	if( !trace ) return;
	np = list;
	traceinit();
	fprintf(stderr,"\nnode list:\n");
	while( (i++<100) && (np != NULL) ) {
		fprintf(stderr,"       %4d;%f;%f\n",
			(int)np->note,np->start,np->duration);
		np = np->next;
	}
	if( i == 0 ) fprintf(stderr,"      NULL\n");
	if( i > 100 ) fprintf(stderr,"    print count exceeded\n");
	fprintf(stderr,"end node list\n");
	traceclose();
}
