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
	temp->next = temp;
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
	if( np->next == list ) {
		free_node(np);
		return;
	}
	while( np->next != list ) {
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
 *	node lists are circular, and the tail of the list is passed around
 *	so that both the head and the tail can be quickly found
 */
node *insert_node( node *theList, node *theNode )
{
	node *np,*p,*head,*tail; 
	tail = theList;
	head = theList->next;
	tail->next = NULL;
	if( head->start > theNode->start ) {
		theNode->next = head;
		tail->next = theNode;
		return tail;
	}
	p = head;
	np = p->next;
	while( np != NULL && np->start <= theNode->start ) {
		p = np; /* and the computational universe collapses...*/
		np = np->next;
	}
	p->next = theNode;
	if( np == NULL ) {
		tail = theNode;
		theNode->next=head;
	} else {
		theNode->next = np;
	}
	return theList;
}

/**merge_node_lists
 *	merge two node lists together in order of start time.
 *	this is a stable merge, in that nodes from list 1 always
 *	go after nodes from list 0 with the same start time
 */
node * merge_node_lists( node *np0, node *np1 )
{
	node * p,*head0,*tail0,*head1,*tail1;
	node * result;
	
	if( np0 == NULL ) return np1;
	head0 = np0->next;
	tail0 = np0;
	head1 = np1->next;
	tail1 = np1;
	result = p = head0;
	while( head0 != NULL && head1 != NULL ) {
		if( head0->start > head1->start ) {
			p->next = head1;
			p = head1;
			head1 = head1->next;
			while( head1!=NULL && head0->start > head1->start ){
				p = head1;
				head1 = head1->next;
			}
			p->next = head0;
			p = head0;
			head0 = head0->next;
		} else {
			p = head0;
			head0 = head0->next;
		}
	}
	if( head0 == NULL ) p->next = head1;
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
