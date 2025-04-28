
// Estudando esse trem de árvores.
// Nesse endereço aqui: http://andreiformiga.com/post/repformlog/
// Desse cara aqui: Andrei Formiga ( math is hard, let’s go shopping! )



struct tree_node *create_tree_node ( int type, 
                                     struct tree_node *left, 
									 struct tree_node *right )
{
  struct tree_node *R = (struct tree_node *) malloc ( sizeof(struct tree_node) );

  if ( (void *) R == NULL)
    return NULL;

  R->type = type;
  R->left = left;
  R->right = right;

  return (struct tree_node *) R;
};





struct tree_node *neg (struct tree_node *e)
{
    return (struct tree_node *) create_tree_node ( NEG, e, NULL );
}

struct tree_node *and ( struct tree_node *d, struct tree_node *e)
{
  return (struct tree_node *) create_tree_node (AND, d, e);
}

struct tree_node *or(struct tree_node *d, struct tree_node *e)
{
  return (struct tree_node *) create_tree_node (OR, d, e);
}

struct tree_node *imp(struct tree_node *d, struct tree_node *e)
{
  return (struct tree_node *) create_tree_node (IMP, d, e);
}

struct tree_node *bimp(struct tree_node *d, struct tree_node *e)
{
  return (struct tree_node *) create_tree_node (BIMP, d, e);
}

struct tree_node *var_p()
{
    return (struct tree_node *) create_tree_node ( P, NULL, NULL );
}

struct tree_node *var_q()
{
    return (struct tree_node *) create_tree_node ( Q, NULL, NULL );
}

struct tree_node *var_r()
{
    return (struct tree_node *) create_tree_node ( R, NULL, NULL );
}


int indice_variavel( int type )
{
  switch(type) 
  {
      case P:
        return PIND;

  case Q:
    return QIND;

  case R:
    return RIND;

  default:   // demais tipos nao representam variaveis
    return -1;
  }
}

int valor_formula(struct tree_node *f)
{
  switch(f->type) 
  {
      case P:
      case Q:
      case R:
      return I[ indice_variavel(f->tipo) ];

  case NEG:
    return !valor_formula(f->dir);

  case AND:
    return valor_formula(f->dir) && valor_formula(f->esq);

  case OR:
    return valor_formula(f->dir) || valor_formula(f->esq);

  case IMP:
    return IMPVAL( valor_formula(f->dir), valor_formula(f->esq));

  case BIMP:
    return BIMPVAL(valor_formula(f->dir), valor_formula(f->esq));
  }
}














