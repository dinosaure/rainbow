#include <string.h>
#include <stdio.h>

#include "rainbow.h"

#include <caml/alloc.h>
#include <caml/memory.h>
#include <caml/custom.h>
#include <caml/callback.h>
#include <caml/fail.h>

#define Unicorn_tag (0)

typedef struct s_unicorn_node t_unicorn_node;

struct s_unicorn_node
{
  RB_NODE(t_unicorn_node)	link;
  value				key;
  value				value;
};

typedef struct s_unicorn_tree t_unicorn_tree;

struct s_unicorn_tree
{
  RB_TREE(t_unicorn_node);
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

RB_GENERATE(inline static, unicorn_, t_unicorn_tree, t_unicorn_node, link)

/* val create : (key -> key -> 'a) -> 'a t */

CAMLprim value
unicorn_create(value compare)
{
  CAMLparam1(compare);
  CAMLlocal1(ret);

  t_unicorn_tree * tree;

  ret = caml_alloc(3, Unicorn_tag);
  tree = (t_unicorn_tree *) ret;

  unicorn_new_tree(tree);
  Store_field(ret, 2, compare);

  CAMLreturn(ret);
}

/* val clear : 'a t -> unit */

CAMLprim value
unicorn_clear(value ml_tree)
{
  CAMLparam1(ml_tree);

  t_unicorn_tree * tree = (t_unicorn_tree *) ml_tree;
  unicorn_new_tree(tree);

  CAMLreturn(Val_unit);
}

/* val add : 'a t -> key -> 'a -> unit */

CAMLprim value
unicorn_add(value ml_tree, value key, value data)
{
  CAMLparam3(ml_tree, key, data);
  CAMLlocal1(ml_node);

  t_unicorn_tree * tree;
  t_unicorn_node * node;

  ml_node = caml_alloc(5, Unicorn_tag);
  tree = (t_unicorn_tree *) ml_tree;
  node = (t_unicorn_node *) ml_node;

  unicorn_new_node(tree, node);
  Store_field(ml_node, 3, key);
  Store_field(ml_node, 4, data);

  int nested_compare (t_unicorn_node * a, t_unicorn_node * b)
    { return (Int_val(caml_callback2(Field(ml_tree, 2), a->key, b->key))); }

  unicorn_insert(tree, node, &nested_compare);

  CAMLreturn(Val_unit);
}

/* val delete : 'a t -> key -> unit */

CAMLprim value
unicorn_delete(value ml_tree, value key)
{
  CAMLparam2(ml_tree, key);

  t_unicorn_tree * tree;
  t_unicorn_node   node;
  t_unicorn_node * proxy = &node;

  tree = (t_unicorn_tree *) ml_tree;

  unicorn_new_node(tree, &node);
  node.key = key;

  int nested_compare (t_unicorn_node * a, t_unicorn_node * b)
    { return (Int_val(caml_callback2(Field(ml_tree, 2), a->key, b->key))); }

  unicorn_remove(tree, &proxy, false, &nested_compare);

  CAMLreturn(Val_unit);
}

/* val find : 'a t -> key -> 'a */

CAMLprim value
unicorn_find(value ml_tree, value key)
{
  CAMLparam2(ml_tree, key);

  t_unicorn_tree * tree;
  t_unicorn_node   node;
  t_unicorn_node * result;

  tree = (t_unicorn_tree *) ml_tree;

  unicorn_new_node(tree, &node);
  node.key = key;

  int nested_compare (t_unicorn_node * a, t_unicorn_node * b)
    { return (Int_val(caml_callback2(Field(ml_tree, 2), a->key, b->key))); }

  if ((result = unicorn_search(tree, &node, &nested_compare)) != NULL)
    CAMLreturn(result->value);
  else
    caml_raise_not_found();
}

/* val mem : 'a t -> key -> bool */

CAMLprim value
unicorn_mem(value ml_tree, value key)
{
  CAMLparam2(ml_tree, key);

  t_unicorn_tree * tree;
  t_unicorn_node   node;

  tree = (t_unicorn_tree *) ml_tree;

  unicorn_new_node(tree, &node);
  node.key = key;

  int nested_compare (t_unicorn_node * a, t_unicorn_node * b)
    { return (Int_val(caml_callback2(Field(ml_tree, 2), a->key, b->key))); }

  if (unicorn_search(tree, &node, &nested_compare) != NULL)
    CAMLreturn(Val_false);
  else
    CAMLreturn(Val_true);
}

/* val iter : (key -> 'a -> unit) -> 'a t -> unit */

CAMLprim value
unicorn_mliter(value func, value ml_tree)
{
  CAMLparam2(func, ml_tree);

  t_unicorn_tree * tree;

  tree = (t_unicorn_tree *) ml_tree;

  long * nested_iter (t_unicorn_node * parent, t_unicorn_node * node, long * data)
    {
      (void) parent;
      (void) caml_callback2(func, node->key, node->value);

      return (data);
    }

  (void) unicorn_iter(tree, &nested_iter, NULL);

  CAMLreturn(Val_unit);
}

/* val fold: (key -> 'a -> 'b -> 'b) -> 'a t -> 'b -> 'b */

CAMLprim value
unicorn_fold(value func, value ml_tree, value acc)
{
  CAMLparam3(func, ml_tree, acc);

  t_unicorn_tree * tree;

  tree = (t_unicorn_tree *) ml_tree;

  long * nested_func (t_unicorn_node * parent, t_unicorn_node * node, long * data)
    {
      (void) parent;

      return ((long *) (acc = caml_callback3(func, node->key, node->value, (value) data)));
    }

  CAMLreturn((value) unicorn_iter(tree, &nested_func, (long *) acc));
}
