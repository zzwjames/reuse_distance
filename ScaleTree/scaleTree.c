
unsigned sizeTrace = 0;

/* Find the block node, splay it to the root.  If not found, insert
   a new block node.  Return the distance through a pointer.
   Cannot insert before the smallest tree node. */

Tree * freeNodeList=NULL;

Tree *CompactScaleTree(Tree *root);

Tree  * MallocNode(){
  if (!freeNodeList){
    Tree * l = (Tree *) malloc(sizeof(Tree)*CandidateSize);
    if (l==NULL) {
      trace = CompactScaleTree(trace);
      printf("Tree is compacted because of insufficient memory for scaletree\n");
    }
    else{
      int i;
      for (i=0;i<CandidateSize-1;i++)
	l[i].next = &(l[i+1]);
      l[CandidateSize-1].next=freeNodeList;
      freeNodeList = l;
    }
  }
  Tree * t=freeNodeList;
  freeNodeList=freeNodeList->next;
  return t;
}

void FreeNode(Tree * node){
  node->next=freeNodeList;
  freeNodeList = node;
}

unsigned long FindDistance(unsigned long long i, Tree * t) {
	unsigned long dis;
  //Tree N, *y;
  Tree *y;
  unsigned left = 0, right = 0;
  if (t == NULL) return 0;
  //N.left = N.right = NULL;
  //r = l = &N;
  //N.left = NULL;
  //N.right = NULL;
  //Tree *l = &N;
  //Tree *r = &N;
  //N.weight = t->weight;

  y = t;
  for (;;) {
    if (i < y->item && (y->prev!=NULL && i<=y->prev->item)) {
      assert(i <= y->prev->item);
      if(y->right != NULL) right += y->right->weight;
      if(y->left == NULL) break;
      right += y->nodeWt;
      y = y->left;
    } else if (i > y->item) {
      if(y->left != NULL) left += y->left->weight;
      if(y->right == NULL) break;
      left += y->nodeWt;
      y = y->right;
    }
    else {
	/* i is within the block of y */
	if(y->right != NULL) right += y->right->weight;
	if(y->left != NULL) left += y->left->weight;
	break;
    }
  }
	dis = right;
	return dis;
}



Tree * ScaleTreeSplay(unsigned long long i, Tree * t, unsigned *dis) {
  Tree N, *l, *r, *y;
  unsigned left = 0, right = 0;
  if (t == NULL) return t;
  N.left = N.right = NULL;
  l = r = &N;
  N.weight = t->weight;

  y = t;
  for (;;) {
    if (i < y->item && (y->prev!=NULL && i<=y->prev->item)) {
      assert(i <= y->prev->item);
      if(y->right != NULL) right += y->right->weight;
      if(y->left == NULL) break;
      right += y->nodeWt;
      y = y->left;
    } else if (i > y->item) {
      if(y->left != NULL) left += y->left->weight;
      if(y->right == NULL) break;
      left += y->nodeWt;
      y = y->right;
    }
    else {
	/* i is within the block of y */
	if(y->right != NULL) right += y->right->weight;
	if(y->left != NULL) left += y->left->weight;
	break;
    }
  }

  for (;;) {
    if (i < t->item && (t->prev!=NULL && i<=t->prev->item)) {
      if (t->left == NULL) break;
      if (i < t->left->item && (t->left->prev!=NULL && i<=t->left->prev->item)) {
	y = t->left;                           /* rotate right */
	t->left = y->right;
	y->right = t;
	/* t->weight--; */
	t->weight -= y->nodeWt;
	t = y;
	if (t->left == NULL) break;
	t->right->weight -= t->left->weight;
      }
      t->weight = right;
      /* right--; */
      right -= t->nodeWt;
      if(t->right != NULL) right -= t->right->weight;
      r->left = t;                               /* link right */
      r = t;
      t = t->left;
    } else if (i > t->item) {
      if (t->right == NULL) break;
      if (i > t->right->item) {
	y = t->right;                          /* rotate left */
	t->right = y->left;
	y->left = t;
	/* t->weight--; */
	t->weight -= y->nodeWt;
	t = y;
	if (t->right == NULL) break;
	t->left->weight -= t->right->weight;
      }
      t->weight = left;
      /* left--; */
      left -= t->nodeWt;
      if(t->left != NULL) left -= t->left->weight;
      l->right = t;                              /* link left */
      l = t;
      t = t->right;
    } else {
      break;
    }
  }
  l->right = t->left;                                /* assemble */
  r->left = t->right;
  t->left = N.right;
  t->right = N.left;
  t->weight = N.weight;

  *dis = t->nodeWt/2;
  if (t->right!=NULL) *dis += t->right->weight;
  return t;
}


Tree * ScaleTreeInsertAtFront(unsigned long long blockEnd, Tree * t, Tree *newNode) {
  Tree * newOne, * prev  /*, * next*/;
  //unsigned useless;

  if (newNode==NULL) {
    //newOne = (Tree *) malloc (sizeof (Tree));
    newOne = MallocNode(); // by Zhangchl
    if (newOne == NULL) {
      printf("Ran out of space\n");
      exit(1);
    }
    sizeTrace ++;
  }
  else newOne = newNode;

  newOne->item = blockEnd;
  newOne->nodeWt = 1;
  newOne->maxSize = 1;
  if (t == NULL) {
    newOne->left = newOne->right = NULL;
    newOne->weight = 1;
    newOne->prev = newOne->next = NULL;
    return newOne;
  }

  /* Insert at the front of the tree */
  assert(blockEnd > t->item);
  newOne->weight = t->weight + 1;
  newOne->left = t;
  newOne->right = NULL;

  /* find prev and next */
  newOne->next = NULL;
  prev = newOne->left;
  if (prev!=NULL)
    while (prev->right!=NULL) prev = prev->right;
  newOne->prev = prev;
  if (prev!=NULL) prev->next = newOne;

  /* printf("insert: new %d, prev %d, next %d\n", new->item,
	 new->prev!=NULL?new->prev->item: -1,
	 new->next!=NULL? new->next->item: -1); */

  return newOne;
}


Tree * QueryScaleTree(unsigned long long oldCyc, unsigned long long newCyc, Tree *t, unsigned *dis) {
  unsigned useless;
  Tree *tmp, *recycleNode = NULL;
  unsigned rightChildWt;

  if (oldCyc == newCyc) {
    t = ScaleTreeInsertAtFront(newCyc, t, NULL);
    return t;
  }

  t = ScaleTreeSplay(oldCyc, t, dis);
  if(oldCyc > t->item)printf("Old Cycle: %llu -- New Cycle: %llu\n", oldCyc, newCyc);
  assert(oldCyc <= t->item);

  if (t->prev!=NULL && oldCyc <= t->prev->item)
    assert(0);


  /* set the size of t */
  if (t->right!=NULL) rightChildWt = t->right->weight;
  else rightChildWt = 0;

  t->maxSize = (int) (rightChildWt * errorRate);
  if (t->maxSize==0) t->maxSize = 1;

  /* delete the oldCyc, merge nodes if necessary */
  t->nodeWt --;  t->weight --;
  assert(t->nodeWt>=0);

  if (t->nodeWt <= (t->maxSize >> 1)) {
    if (t->prev!=NULL && t->prev->nodeWt + t->nodeWt <= t->maxSize) {
      t->left = ScaleTreeSplay(t->prev->item, t->left, &useless);
      assert(t->left->right==NULL);  /* largest of the left subtree */
      assert(t->left==t->prev);
      t->left->right = t->right;  /* new tree */
      if (t->nodeWt > 0)   /* otherwise, t is empty */
	t->left->item = t->item;    /* merge  */
      t->left->nodeWt += t->nodeWt;
      t->left->weight += t->nodeWt;
      if (t->right!=NULL) t->left->weight += t->right->weight;
      t->left->next = t->next;    /* new neighbors */
      if (t->next!=NULL)
	t->next->prev = t->left;
      tmp = t;
      t = t->left;
      if (recycleNode == NULL) recycleNode = tmp;
      else {
	//free(tmp);
	FreeNode(tmp);
	sizeTrace --;
      }
    }
    if (t->prev!=NULL) {
      t->prev->maxSize = (int) ((rightChildWt + t->nodeWt) * errorRate);
      if (t->prev->maxSize==0) t->prev->maxSize = 1;
    }
    if (t->next!=NULL) {
      t->next->maxSize = (int) ((rightChildWt - t->next->nodeWt)
	                       * errorRate);
      if (t->next->maxSize==0) t->next->maxSize = 1;
    }
    if (t->next!=NULL && t->next->nodeWt+t->nodeWt <= t->next->maxSize) {
      /* merge next with me */
      t->right = ScaleTreeSplay(t->next->item, t->right, &useless);
      assert(t->right->left==NULL);
      assert(t->right==t->next);
      t->right->left = t->left;   /* new tree */
      t->right->nodeWt += t->nodeWt; /* merge    */
      t->right->weight += t->nodeWt;
      if (t->left!=NULL) t->right->weight += t->left->weight;
      t->right->prev = t->prev;    /* new neighbors */
      if (t->prev!=NULL)
	t->prev->next = t->right;
      tmp = t;
      t = t->right;
      if (recycleNode == NULL) recycleNode = tmp;
      else {
	//free(tmp);
	FreeNode(tmp);
	sizeTrace --;
      }
    }
  }

  /* ytzhong: only one address has been accessed and re-accessed,
     the nodeWt could be zero after compaction
  */
  /*  if (t->nodeWt == 0)
    assert(0);
  */


  /* insert newCyc */
  t = ScaleTreeInsertAtFront(newCyc, t, recycleNode);

  return t;
}

Tree *CompactScaleTree(Tree *root) {
  if (root==NULL)
    return root;
  Tree *cur, *prev;
  unsigned priorWt = 0;
  unsigned totalWt = root->weight;
  /* from the end of trace forward, we set each node with correct
     maxSize and merge with its left neighbor if possible */
  assert(root->right==NULL);  /* root is the most recent */
  cur = root;
  while (cur!=NULL) {
    cur->right = NULL;  /* make it a list */
    cur->weight = totalWt - priorWt;
    cur->maxSize = (int) (priorWt * errorRate);
    if (cur->maxSize==0) cur->maxSize = 1;
    if (cur->prev!=NULL && cur->prev->nodeWt + cur->nodeWt <= cur->maxSize) {
      prev = cur->prev;
      cur->nodeWt += prev->nodeWt;
      cur->prev = prev->prev;
      if (cur->prev!=NULL)
	cur->prev->next = cur;
      cur->left = cur->prev;
      //free(prev);
      FreeNode(prev);
      sizeTrace --;
    }
    else {
      cur->left = cur->prev;
      priorWt += cur->nodeWt;
      cur = cur->left;
    }
  }
  return root;
}
