#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "exam.h"

/*
 * Private function prototypes.
 */

static void _print_huffman_tree(const huffman_tree_t *, int);
static void _print_huffman_tree_codes(const huffman_tree_t *, char *, char *);

/*
 * Prints the given Huffman tree.
 */
void print_huffman_tree(const huffman_tree_t *t) {
  printf("Huffman tree:\n");
  _print_huffman_tree(t, 0);
}

/*
 * Private helper function for print_huffman_tree.
 */
static void _print_huffman_tree(const huffman_tree_t *t, int level) {
  int i;
  for (i = 0; i <= level; i++) {
    printf("  ");
  }

  if (t->left == NULL && t->right == NULL) {
    printf("Leaf: '%c' with count %d\n", t->letter, t->count);
  } else {
    printf("Node: accumulated count %d\n", t->count);

    if (t->left != NULL) {
      _print_huffman_tree(t->left, level + 1);
    }

    if (t->right != NULL) {
      _print_huffman_tree(t->right, level + 1);
    }
  }
}

/*
 * Prints the codes contained in the given Huffman tree.
 */
void print_huffman_tree_codes(const huffman_tree_t *t) {
  printf("Huffman tree codes:\n");

  char *code = calloc(MAX_CODE_LENGTH, sizeof(char)), *code_position = code;
  if (code == NULL) {
    perror("calloc");
    exit(EXIT_FAILURE);
  }

  _print_huffman_tree_codes(t, code, code_position);
  free(code);
}

/*
 * Private helper function for print_huffman_tree_codes.
 */
static void _print_huffman_tree_codes(const huffman_tree_t *t,
                                      char *code, char *code_position) {

  if (t->left == NULL && t->right == NULL) {
    printf("'%c' has code \"%s\"\n", t->letter, code);
  } else if (t->left != NULL) {
    *code_position++ = 'L';
    *code_position = '\0';

    _print_huffman_tree_codes(t->left, code, code_position--);
  }

  if (t->right != NULL) {
    *code_position++ = 'R';
    *code_position = '\0';

    _print_huffman_tree_codes(t->right, code, code_position--);
  }
}

/*
 * Prints a list of Huffman trees.
 */
void print_huffman_tree_list(const huffman_tree_list_t *l) {
  printf("Huffman tree list:\n");

  for (; l != NULL; l = l->next) {
    print_huffman_tree(l->tree);
  }
}

/*
 * Frees a Huffman tree.
 */
void huffman_tree_free(huffman_tree_t *t) {
  if (t != NULL) {
    huffman_tree_free(t->left);
    huffman_tree_free(t->right);
    free(t);
  }
}

/*
 * Frees a list of Huffman trees.
 */
void huffman_tree_list_free(huffman_tree_list_t *l) {
  if (l != NULL) {
    huffman_tree_list_free(l->next);
    huffman_tree_free(l->tree);
    free(l);
  }
}

/*
 * Returns 1 if the string s contains the character c and 0 if it does not.
 */
int contains(char *s, char c) { //good
  while(*s != '\0') {
    if (*s == c) {
      return 1;
    }
    s++;
  }
  return 0;
}

/*
 * Returns the number of occurrences of c in s.
 */
int frequency(char *s, char c) {
  int count = 0;

  while(*s != '\0') {
    if (*s == c) {
      count++;
    }
    s++;
  }

  return count;
}

/*
 * Takes a string s and returns a new heap-allocated string containing only the
 * unique characters of s.
 *
 * Pre: all strings will have fewer than or equal to MAX_STRING_LENGTH - 1
 *      characters.
 */
char *nub(char *s) {
  assert(strlen(s) <= MAX_STRING_LENGTH - 1);
  char *result = malloc(strlen(s)+1);
  //IMPORTANT
  if (result == NULL) {
    perror("malloc failed to allocate\n");
    exit(EXIT_FAILURE);
  }
  // ----------

  int resultIndex = 0;

  for (int i = 0; i < strlen(s); i++) {
    if (!contains(result, s[i])) {
      result[resultIndex] = s[i];
      resultIndex++;
    }
  }

  result[resultIndex] = '\0';
  //REALLOC
  return result;
}

/*
 * Adds the Huffman tree t to the list l, returning a pointer to the new list.
 *
 * Pre:   The list l is sorted according to the frequency counts of the trees
 *        it contains.
 *
 * Post:  The list l is sorted according to the frequency counts of the trees
 *        it contains.
 */
huffman_tree_list_t *huffman_tree_list_add(huffman_tree_list_t *l,
                                            huffman_tree_t *t) {

  if (l == NULL) { // if list is empty
    l = malloc(sizeof(huffman_tree_list_t));

    if (l == NULL) {
      perror("malloc failed to allocate\n");
      exit(EXIT_FAILURE);
    }

    l->tree = t;
    l->next = NULL;
    return l;
  }

  huffman_tree_list_t *iter = l;
  huffman_tree_list_t *prev = l;
  huffman_tree_list_t *new_list = NULL;
  new_list = malloc(sizeof(huffman_tree_list_t));

  if (new_list == NULL) {
    perror("malloc failed to allocate\n");
    exit(EXIT_FAILURE);
  }

  new_list->tree = t;
  new_list->next = NULL;
  iter = iter->next;

  if(t->count < prev->tree->count) {
    new_list->next = prev;
    return new_list;
  }

  while (iter != NULL) {
    if (t->count < iter->tree->count) {
      new_list->next = iter;
      prev->next = new_list;
      return l;

    } else {
      prev = iter;
      iter = iter->next;
    }
  } ;

  prev->next = new_list;

  return l;
}

/*
 * Takes a string s and a lookup table and builds a list of Huffman trees
 * containing leaf nodes for the characters contained in the lookup table. The
 * leaf nodes' frequency counts are derived from the string s.
 *
 * Pre:   t is a duplicate-free version of s.
 *
 * Post:  The resulting list is sorted according to the frequency counts of the
 *        trees it contains.
 */
huffman_tree_list_t *huffman_tree_list_build(char *s, char *t) {
  huffman_tree_list_t *l = NULL;
  huffman_tree_t *tree = NULL;

  //print_huffman_tree_list(l);
  for (int i = 0; i < strlen(t); i++) {
    tree = malloc(sizeof(huffman_tree_t));

    if (tree == NULL) {
      perror("malloc failed to allocate\n");
      exit(EXIT_FAILURE);
    }

    tree->letter = t[i];
    tree->count = frequency(s, t[i]);
    tree->left = NULL;
    tree->right = NULL;

    l = huffman_tree_list_add(l,tree);
  }

  return l;
}

/*
 * Reduces a sorted list of Huffman trees to a single element.
 *
 * Pre:   The list l is non-empty and sorted according to the frequency counts
 *        of the trees it contains.
 *
 * Post:  The resuling list contains a single, correctly-formed Huffman tree.
 */
huffman_tree_list_t *huffman_tree_list_reduce(huffman_tree_list_t *l) {
  huffman_tree_t *new_tree = NULL;

  while(l->next != NULL) {
    new_tree = malloc(sizeof(huffman_tree_t));
    new_tree->count = l->tree->count + l->next->tree->count;
    new_tree->left = l->tree;
    new_tree->right = (l->next)->tree;
    l = huffman_tree_list_add(l->next->next, new_tree);
  }
  return l;
}

/*
 * Accepts a Huffman tree t and a string s and returns a new heap-allocated
 * string containing the encoding of s as per the tree t.
 *
 * Pre: s only contains characters present in the tree t.
 */
char *huffman_tree_encode(huffman_tree_t *t, char *s) {
  return NULL;
}

/*
 * Accepts a Huffman tree t and an encoded string and returns a new
 * heap-allocated string contained the decoding of the code as per the tree t.
 *
 * Pre: the code given is decodable using the supplied tree t.
 */
char *huffman_tree_decode(huffman_tree_t *t, char *code) {
  return NULL;
}
