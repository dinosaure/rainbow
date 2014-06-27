#include <string.h>
#include <stdio.h>

#include "rainbow.h"

#include <caml/alloc.h>
#include <caml/memory.h>
#include <caml/custom.h>
#include <caml/callback.h>

#define Poney_tag (0)

typedef struct s_poney_node t_poney_node;

struct s_poney_node
{
  RB_NODE(t_poney_node)		link;
  value				key;
  value				value;
};

typedef struct s_poney_tree t_poney_tree;

struct s_poney_tree
{
  RB_TREE(t_poney_node);
  value				compare;
};

#undef RB_GET_COLOR
#define RB_GET_COLOR(_type, _field, _node)				\
  (Bool_val((_node)->_field.is_red))

#undef RB_SET_COLOR
#define RB_SET_COLOR(_type, _field, _node, _red)			\
  do { (_node)->_field.is_red = Val_bool(_red); }			\
  while (0)

#undef RB_TO_BLACK
#define RB_TO_BLACK(_type, _field, _node)				\
  do { (_node)->_field.is_red = Val_bool(false); }			\
  while (0)

#undef RB_TO_RED
#define RB_TO_RED(_type, _field, _node)					\
  do { (_node)->_field.is_red = Val_bool(true); }			\
  while (0)

/* inline is to hide unused function in compilation */

RB_GENERATE(inline static, poney_, t_poney_tree, t_poney_node, link)

/* Compute the path to insert the new [node] and retunrs the number of
 * nodes in the way.
 */

int
compute_insert_path(t_poney_tree *tree, t_poney_node *node, \
  struct poney_stack stack[sizeof(void *) << 4], \
  int (*compare)(t_poney_node *, t_poney_node *))
{
  struct poney_stack *	ptr;
  int			cpt;

  stack->node = tree->root;

  for (cpt = 0, ptr = stack; ptr->node != tree->null; ptr++, cpt++)
    {
      int cmp = ptr->cmp = (*compare)(node, ptr->node);

      if (cmp < 0)
	ptr[1].node = ((ptr[1].node = poney_left(tree, ptr->node)) == NULL) \
		      ? tree->null : ptr[1].node;
      else if (cmp > 0)
	ptr[1].node = ((ptr[1].node = poney_right(tree, ptr->node)) == NULL) \
		      ? tree->null : ptr[1].node;
      else
	return (- cpt);
    }

  ptr->node = node;
  ptr->cmp = 0;

  return (cpt);
}

/* val add : key -> 'a -> 'a t -> 'a t */

CAMLprim value
poney_add(value ml_key, value ml_data, value ml_tree)
{
  CAMLparam3(ml_key, ml_data, ml_tree);

  struct poney_stack	stack[sizeof(void *) << 4];
  t_poney_tree *	new_tree = (t_poney_tree *) ml_tree;
  t_poney_tree *	old_tree;
  t_poney_node		node;
  int			size_of_path;

  int nested_compare (t_poney_node * a, t_poney_node * b)
    { return (Int_val(caml_callback2(Field(ml_tree, 2), a->key, b->key))); }

  poney_new_node(new_tree, &node);
  node.key = ml_key;
  size_of_path = compute_insert_path(new_tree, &node, stack, &nested_compare);

  if (size_of_path < 0) /* key already exists */
    {
      size_of_path = - size_of_path;

      value ret[size_of_path + 2];
      memset(ret, 0, (size_of_path + 1) * sizeof(value));
      CAMLxparamN(ret, size_of_path + 1);

      struct poney_stack * ptr = stack + (size_of_path - 1);
      t_poney_node *       prv;
      t_poney_node *       tmp;
      int                  idx = 2;

      /* create a new tree */
      ret[0] = caml_alloc(3, Poney_tag);

      old_tree = (t_poney_tree *) ret[0];
      poney_new_tree(old_tree);
      Store_field(ret[0], 2, Field(ml_tree, 2));

      /* create a new node */
      ret[1] = caml_alloc(5, Poney_tag);

      poney_new_node(new_tree, (t_poney_node *) ret[1]);
      Store_field(ret[1], 3, ml_key);
      Store_field(ret[1], 4, ml_data);

      stack[size_of_path].node = (t_poney_node *) ret[1];

      tmp = stack[size_of_path].node;
      prv = stack[size_of_path].node;

      for (; ptr >= stack; ptr--)
	{
	  ret[idx] = caml_alloc(5, Poney_tag);
	  tmp = (t_poney_node *) ret[idx];
	  idx = idx + 1;

	  memcpy(tmp, ptr->node, sizeof(t_poney_node));

	  if (ptr->cmp < 0)
	    tmp->link.lnode = prv;
	  else
	    tmp->link.rnode = prv;

	  prv = tmp;
	}

      /* reassign new root on old_tree (a clean copy of ml_tree) */
      old_tree->root = tmp;

      /* swap root between new and old tree */
      t_poney_node * root_temp = new_tree->root;
      new_tree->root = old_tree->root;
      old_tree->root = root_temp;

      CAMLreturn(ret[0]);
    }
  else /* key does not exist */
    {
      /* LOL! Purity has a price. */

      /* It's same as CAMLlocalN without initilization. */
      value ret[(size_of_path * 2) + 2];
      memset(ret, 0, ((size_of_path * 2) + 2) * sizeof(value));
      CAMLxparamN(ret, (size_of_path * 2) + 2);

      struct poney_stack * ptr = stack + (size_of_path - 1);
      struct poney_stack   end;
      t_poney_node *       prv;
      t_poney_node *       tmp;
      int                  idx = 2;

      /* create a new tree */
      ret[0] = caml_alloc(3, Poney_tag);

      old_tree = (t_poney_tree *) ret[0];
      poney_new_tree(old_tree);
      Store_field(ret[0], 2, Field(ml_tree, 2));

      /* create a new node */
      ret[1] = caml_alloc(5, Poney_tag);

      poney_new_node(new_tree, (t_poney_node *) ret[1]);
      Store_field(ret[1], 3, ml_key);
      Store_field(ret[1], 4, ml_data);

      /* save last new node of the path */
      stack[size_of_path].node = (t_poney_node *) ret[1];

      memcpy(&end, stack + size_of_path, sizeof(struct poney_stack));

      stack[size_of_path].node = old_tree->null;
      stack[size_of_path].cmp = 0;

      ret[idx] = Field(ret[0], 1);
      tmp = stack[size_of_path].node;
      prv = stack[size_of_path].node;

      for (; ptr >= stack; ptr--)
	{
	  ret[idx] = caml_alloc(5, Poney_tag);
	  tmp = (t_poney_node *) ret[idx];
	  idx = idx + 1;

	  memcpy(tmp, ptr->node, sizeof(t_poney_node));

	  if (ptr->cmp < 0)
	    {
	      tmp->link.lnode = prv;

	      if (prv->link.lnode == old_tree->null)
		tmp->link.lnode->link.lnode = old_tree->null;
	      else
		{
		  ret[idx] = caml_alloc(5, Poney_tag);
		  memcpy((void *) ret[idx], prv->link.lnode, \
		    sizeof(t_poney_node));
		  tmp->link.lnode->link.lnode = (t_poney_node *) ret[idx];
		  idx = idx + 1;
		}
	    }
	  else
	    {
	      tmp->link.rnode = prv;

	      if (tmp->link.lnode == old_tree->null)
		tmp->link.lnode = old_tree->null;
	      else
		{
		  ret[idx] = caml_alloc(5, Poney_tag);
		  memcpy((void *) ret[idx], tmp->link.lnode, \
		    sizeof(t_poney_node));
		  tmp->link.lnode = (t_poney_node *) ret[idx];
		  idx = idx + 1;
		}
	    }

	  prv = tmp;
	}

      /* reset last new node in the path */
      memcpy(stack + size_of_path, &end, sizeof(struct poney_stack));

      /* reassign new root on old_tree (a clean copy of ml_tree) */
      old_tree->root = tmp;

      /* insert new node in tree */
      poney_insert_with_specific_path(new_tree, (t_poney_node *) ret[1], \
	&nested_compare, stack, stack + size_of_path);

      /* swap root between new and old tree */
      t_poney_node * root_temp = new_tree->root;
      new_tree->root = old_tree->root;
      old_tree->root = root_temp;

      CAMLreturn(ret[0]);
    }
}

/* val empty : (key -> key -> 'a) -> 'a t */

CAMLprim value
poney_empty(value compare)
{
  CAMLparam1(compare);
  CAMLlocal1(ret);

  t_poney_tree * tree;

  ret = caml_alloc(3, Poney_tag);
  tree = (t_poney_tree *) ret;

  poney_new_tree(tree);
  Store_field(ret, 2, compare);

  CAMLreturn(ret);
}

/* val is_empty : 'a t -> bool */

CAMLprim value
poney_is_empty(value ml_tree)
{
  CAMLparam1(ml_tree);
  CAMLlocal1(ret);

  t_poney_tree * tree;

  tree = (t_poney_tree *) ml_tree;

  if (tree->root == tree->null)
    ret = Val_true;
  else
    ret = Val_false;

  CAMLreturn(ret);
}

/* val singleton : (key -> key -> 'a) -> key -> 'a -> 'a t */

CAMLprim value
poney_singleton(value compare, value key, value data)
{
  CAMLparam3(compare, key, data);
  CAMLlocal2(ret, ml_node);

  t_poney_tree * tree;
  t_poney_node * node;

  ret = caml_alloc(3, Poney_tag);
  ml_node = caml_alloc(5, Poney_tag);
  tree = (t_poney_tree *) ret;
  node = (t_poney_node *) ml_node;

  poney_new_tree(tree);
  Store_field(ret, 2, compare);

  poney_new_node(tree, node);
  Store_field(ml_node, 3, key);
  Store_field(ml_node, 4, data);

  int nested_compare (t_poney_node * a, t_poney_node * b)
    { return (Int_val(caml_callback2(Field(ret, 2), a->key, b->key))); }

  poney_insert(tree, node, &nested_compare);

  CAMLreturn(ret);
}

/* val mem : key -> 'a -> 'a t */

CAMLprim value
poney_mem(value key, value ml_tree)
{
  CAMLparam2(key, ml_tree);

  t_poney_tree * tree;
  t_poney_node   node;

  tree = (t_poney_tree *) ml_tree;

  poney_new_node(tree, &node);
  node.key = key;

  int nested_compare (t_poney_node * a, t_poney_node * b)
    { return (Int_val(caml_callback2(Field(ml_tree, 2), a->key, b->key))); }

  if (poney_search(tree, &node, &nested_compare) == NULL)
    CAMLreturn(Val_false);
  else
    CAMLreturn(Val_true);
}

/* val iter : (key -> 'a -> unit) -> 'a t -> unit */

CAMLprim value
poney_mliter(value func, value ml_tree)
{
  CAMLparam2(func, ml_tree);

  t_poney_tree ** tree;

  tree = (t_poney_tree **) &ml_tree;

  void nested_iter (t_poney_node * parent, t_poney_node * node, long ** data)
    {
      (void) parent;
      (void) caml_callback2(func, node->key, node->value);
    }

  poney_iter(tree, &nested_iter, NULL);

  CAMLreturn(Val_unit);
}
