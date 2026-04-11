/*	node.c -- management of nodes and lists of nodes...

	A node contains information about a single "event" -- a midi 
        event.  They are maintained in lists.  A fairly complete data 
        abstraction is supplied for nodes and node lists.

	A node list is implemented as a singly linked circular list.  
        A node list variable is a pointer to the last element in the 
        list, or null.  Because insertion or deletions can change the 
        last list, all list modifying operations return a new list, 
        which is usually the same as the old list, but sometimes can 
        be different.

	The semantics of merge, insertion, and deletion are such that 
        the order of operations is preserved, as much as possible.

	functions:
		node *	append_list(node *list1, node *list2)
		void 	dmp_list(node *list)
		void	dmp_node(node *n)
		int 	empty( node *list )
		void 	free_node( node *n )
		node *	free_list( node *list )
		node *	head(node *list)
		node *	insert_after_node(node *list,node *mark,node *insert )
		node *	insert_node( node *list, node *n )
		node * 	merge_lists( node *list0, node *list1 )
		node *	new_node()
		node *	tail(node * list)


 */

#include "aml.h"
#include "stdlib.h"

static node_number = 0;
/*new_node
        allocate a new event node
 */
node *new_node()
{
	node *n;
	n = (node *)malloc( sizeof(node) );
	if( n == NULL ) {
		fprintf(stderr, "%s: couldn't allocate a node\n", program );
		exit(1);
	}
	n->type = UNDEFINED;
	n->next = n;
	n->n = node_number++;
	nnodes++;
	return n;
}

/**free_node
	free a no longer needed event node
 */
void free_node( node *n )
{
	free( n );
	nnodes--;
}

/*
	free_list
 */
void free_list( node *list )
{
	node *np,*tnp;

	if( list == NULL ) return;
	np = list->next;
	while( np != list ) {
		tnp = np->next;
		free_node(np);
		np = tnp;
	}
	free_node(np);
}

int empty( node *list )
{
	return (list == NULL );
}

node *head(node *list)
{
	if( list == NULL ) return list;
	return list->next;
}
node *tail(node *list)
{
	return list;
}


/*insert_after_node
        insert a node into a node list in start time order, after a
        given node "mark" (which must be in the list).  Also, the new
        node is assumed to be at least as old the marker node.  this
        is a "stable" insert, in the sense that the new node is
        inserted AFTER any nodes with the same start time.  That is,
        order of insertion is a secondary sort criteria. 
 */
node *insert_after_node( node *list, node *mark, node *insert )
{

	if( list == NULL ) {
		if( mark != NULL ) 
			error("insert_after_node: illegal arguments\n",NULL);
		insert->next = insert;
		return insert;
	}
	if( insert == NULL ) return list;
	if( mark == NULL )
		error("insert_after_node: Illegal arguments\n",NULL);

	if( insert->start < mark->start ) 
		error("insert_after_node: insert < mark\n",NULL);

	while( mark != list && mark->next->start <= insert->start ) {
		mark = mark->next;
	}
	insert->next = mark->next;
	mark->next = insert;
	if( mark == list ) return insert;
	else return list;
}

/*insert_node
        insert a node into a node list in start time order.  this is a
        "stable" insert, in the sense that the new node is inserted
        AFTER any nodes with the same start time.  that is, order of
        insertion in a list is significant.  in most applications we
        know that the start time for the node to be inserted is >= the
        start time of the node at the head of the list... 
 */
node *insert_node( node *list, node *n )
{
	return insert_after_node(list,list,n);
}

/* append_lists
	append the second list at the end of the first
*/
node *append_list(node *list1,node *list2)
{
	node * t;
	if( list1 == NULL ) return list2;
	if( list2 == NULL ) return list1;

	t = list2->next;
	list2->next = list1->next;
	list1->next = t;
	return list2;
}
	

/**merge_lists
        merge two node lists together in order of start time.  nodes
        in list 1 always go after nodes in list 0, if the times are
        equal. 
 */
node * merge_lists( node *list0, node *list1 )
{
	node *p0,*p1;
	node *t0,*t1;
	node * result;
	
	if( list0 == NULL ) return list1;
	if( list1 == NULL ) return list0;

	/* each list has at least one node */
	p0 = head(list0);
	p1 = head(list1);
	while( TRUE ) {
		if( p0 == list0 ) { 	/* reached end of l0 --    */
			t0 = p0->next;	/* append rest of l1 to l0 */
			p0->next = p1;
			list1->next = t0;
			return list1;
		}
		if( p0->next->start > p1->start ) {
			t1 = p1->next;
			p1->next = p0->next;
			p0->next = p1;
			if( p1 == list1 ) break;
			list1 = p1 = t1;
			continue;
		}
		if( p0->next->start <= p1->start ) {
			p0 = p0->next;
			continue;
		}
	}
}


void dmp_list(node *list)
{
	node *np;
	int i = 0;
	if( !trace ) return;
	traceinit();
	fprintf(stderr,"node list:\n");
	if( (list == NULL) ) {
	    fprintf(stderr,"      [NULL]\n");
        }
	else {
	    np = list->next;
            do {
		fprintf(stderr,"    %4d: %s %d, start %f, duration %f\n",
			np->n,
			type_name[np->type],
			np->note,
			np->start,
			np->duration);
		
		np = np->next;
            } while( (i++<100) && (np != head(list)) );
	}
	if( i > 100 ) fprintf(stderr,"    print count exceeded\n");
	fprintf(stderr,"end node list\n");
	traceclose();
}

void dmp_node(node *n)
{
	fprintf(stderr,"%d: %s >%d,s=%f,d=%f,v=%f,d=%4d,c=%3d,n=%4d\n",
		n->n,type_name[n->type],n->next->n,n->start,n->duration,
		n->volume,n->duty,n->channel,n->note);
}
