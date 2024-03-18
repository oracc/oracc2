#include <stdio.h>
#include <stdlib.h>
#include "index.h"

int start_column;
struct node *curr_node;
Memo *node_mm;

struct node *
new_node ()
{
  struct node *np;
  np = memo_new (node_mm);
  np->left = np->right = np->down = np->up = NULL;
  np->parallel_info = NULL;
  np->end_of_branch_flag = np->used_flag = 0;
  np->l.text_id = curr_text_id;
  np->l.unit_id = curr_unit_id;
  np->l.word_id = curr_word_id;
  return np;
}
