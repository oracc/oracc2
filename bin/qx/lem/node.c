#include <stdio.h>
#include <stdlib.h>
#include "index.h"

int start_column;
struct qnode *curr_node;
Memo *node_mm;

struct qnode *
new_node (void)
{
  struct qnode *np;
  np = memo_new (node_mm);
  np->left = np->right = np->down = np->up = NULL;
  np->parallel_info = NULL;
  np->end_of_branch_flag = np->used_flag = 0;
  np->l.text_id = curr_text_id;
  np->l.unit_id = curr_unit_id;
  np->l.word_id = curr_word_id;
  return np;
}
