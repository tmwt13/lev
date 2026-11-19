//
//  lev.h
//  Texteditor
//
//  Created by Tom Witjes on 14.04.26.
//
#include "eval.h"
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "stf.h"

typedef enum{
  ACTION_IGNORE,
  ACTION_SUBSTITUTE,
  ACTION_DELETE,
  ACTION_INSERT,
} ACTION;

typedef struct {
  int cost;
  ACTION action;
} Patch;

typedef struct {
  int count, capacity;
  Patch *data;
} Patches;

int levenshtein_distance(StringView, StringView, Patches*);

#ifdef _LEV_IMPLEMENTATION

int levenshtein_distance(StringView src, StringView cmp, Patches *patches)
{
  Patch table[src.count + 1][cmp.count + 1];
  
  table[0][0] = (Patch){.cost=0, .action = ACTION_IGNORE}; 

  for (int i=1;i<src.count + 1;++i)
  {
    table[i][0] = (Patch){.cost=i, .action = ACTION_DELETE}; 
  }
  for (int i=1;i<cmp.count + 1;++i)
  {
    table[0][i] = (Patch){.cost=i, .action = ACTION_INSERT}; 
  }

  for (int j=1;j<cmp.count + 1;++j)
  {
    for (int i=1;i<src.count + 1;++i)
    {
      // substitute
      Patch cost = {.cost = table[i-1][j-1].cost + 1, .action=ACTION_SUBSTITUTE};
      
      // ignore
      if (src.data[i-1] == cmp.data[j-1])
      {
        cost = (Patch){.cost = table[i-1][j-1].cost, .action=ACTION_IGNORE};
      } 

      // deletion
      if(table[i-1][j].cost + 1 <= cost.cost)
      {
        cost = (Patch){.cost = table[i-1][j].cost + 1, .action=ACTION_DELETE};
      }

      // insertion
      else if(table[i][j-1].cost + 1 <= cost.cost)
      {
        cost = (Patch){.cost = table[i][j-1].cost + 1, .action=ACTION_INSERT};
      }

      table[i][j] = cost;
    }
  }

  // for (int i=0;i<src.count+1;++i)
  // {
  //   for (int j=0;j<cmp.count+1;++j)
  //   {
  //     printf("%02d ", table[i][j].cost);
  //     printf("(%s) ", patch_desc_str(table[i][j].action));
  //   }
  //   printf("\n");
  // }

  if (patches == NULL) return table[src.count][cmp.count].cost;

  // optimal patches:
  int i = src.count, j=cmp.count;
  while(i != 0 && j != 0)
  {
    Patch current = table[i][j];
    stf_da_append(patches, current);

    switch(current.action)
    {
    case ACTION_IGNORE:     --i;--j; break;
    case ACTION_SUBSTITUTE: --i;--j; break;
    case ACTION_DELETE:     --i;     break;
    case ACTION_INSERT:     --j;     break;
    default: return 0;
    }
  }  

  return table[src.count][cmp.count].cost;
}

const char *patch_desc_str(ACTION action)
{
  switch(action)
  {
    case ACTION_IGNORE: return "_";
    case ACTION_SUBSTITUTE: return "S";
    case ACTION_DELETE: return "D";
    case ACTION_INSERT: return "I";
    default:
      return "ERROR";
  }
}

static void print_patches(Patches *patches){
  stf_da_rforeach(Patch, patch, patches){
    printf("%s ", patch_desc_str(patch->action));
  }
}

#endif // _LEV_IMPLEMENTATION


