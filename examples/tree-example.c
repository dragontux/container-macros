#include <stdio.h>
#include <stdlib.h>
#include "../tree.h"
#include "../unique.h"

TREE_PROTO( const char *, tree );

TREE( const char *, tree );
UNIQUE( tree *, utree );

void printtree( const tree *t ){
	if ( t ){
		printf( "[%u] %s\n", tree_size( t ), t->item );
		printtree( t->left );
		printtree( t->right );
	}
}

int main( int argc, char *argv[] ){
	unique(utree, meh, tree_free_all) =
		tree_construct( "testing",
				tree_construct( "this",
					tree_new( "tree tester" ),
					tree_new( "yes, a tree tester" )),
				tree_new( "thing" ));

	printtree( meh.data );

	return 0;
} 
