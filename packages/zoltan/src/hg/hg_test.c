#include "hypergraph.h"

/* =========== TIME */
#ifdef WITHTIME
static long     t=0, t_init=0, t_load=0, t_part=0, t_rest=0;
static void times_output ()
{ long t_all=t_load+t_part+t_rest;
  printf("TIME                : %d:%d:%.2f\n", (int)(t_all/3600000),
   (int)((t_all%3600000)/60000),(float)((float)(t_all%60000)/1000));
  printf("  Load/Check        : %.2f\n",(float)t_load/1000);
  printf("  Part              : %.2f\n",(float)t_part/1000);
  printf("  Rest              : %.2f\n",(float)t_rest/1000);
}
#ifdef CLOCK
#define INIT_TIME()     {if(!t_init)t=clock()/1000;t_init++;}
#define ADD_NEW_TIME(T) {T-=t; T+=(t=clock()/1000);}
#define END_TIME()      {t_init--;}
#else
#include <sys/time.h>
#include <sys/resource.h>
struct rusage    Rusage;
#define INIT_TIME()     {if(!t_init) \
                         { getrusage(RUSAGE_SELF,&Rusage); \
                           t=Rusage.ru_utime.tv_sec*1000+ \
                             Rusage.ru_utime.tv_usec/1000; } \
                         t_init++;}
#define ADD_NEW_TIME(T) {T-=t; getrusage(RUSAGE_SELF,&Rusage); \
                         T+=(t=Rusage.ru_utime.tv_sec*1000+ \
                               Rusage.ru_utime.tv_usec/1000);}
#define END_TIME()      {t_init--;}
#endif
#else
static void times_output () {}
#define INIT_TIME()     {}
#define ADD_NEW_TIME(T) {}
#define END_TIME()      {}
#endif

int main (int argc, char **argv)
{ int    i, p=2, *part, memory_graph;
  char   hgraphfile[100]="grid5x5.hg";
  HGraph hg;
  HGPartParams hgp;
  ZZ     zz;

  hgp.bal_tol = 10.0;
  hgp.redl = 0;
  strcpy(hgp.redm_str, "grg");
  strcpy(hgp.rli_str, "aug3");
  hgp.ews = 1;
  strcpy(hgp.global_str, "lin");
  strcpy(hgp.local_str, "hc");
  hgp.check_graph = 1;

  zz.Debug_Level = 1;

  Zoltan_Memory_Debug(2);

/* Start of the time*/
  INIT_TIME();

/* Read parameters */
  if (argc == 1)
  { puts("hg_test [-flag value] [] [] ...");
    puts("-f      graphfile:        (grid5x5.hg)");
    puts("-p      # of parts:       (2)");
    puts("-bal    balance tolerance:(10.0)");
    puts("-redl   reduction limit:  (0)");
    puts("-redm   reduction method: {mxm,rem,rrm,rhm,grm,lhm,pgm,");
    puts("                           mxp,rep,rrp,rhp,grp,lhp,pgp,");
    puts("                           mxg,reg,rrg,rhg,(grg)}");
    puts("-reda   reduction augment:{no,(aug3)}");
    puts("-reds   reduction scaling:(1)");
    puts("-g      global method:    {ran,(lin)}");
    puts("-l      local method:     (no)");
    puts("-d      debug level:      (1)");
    puts("default values are in brackets ():");
  }
  i = 0;
  while (++i<argc)
  { if     (!strcmp(argv[i],"-f")   &&i+1<argc)strcpy(hgraphfile,argv[++i]);
    else if(!strcmp(argv[i],"-p")   &&i+1<argc)p=atoi(argv[++i]);
    else if(!strcmp(argv[i],"-bal") &&i+1<argc)hgp.bal_tol=atof(argv[++i]);
    else if(!strcmp(argv[i],"-redl")&&i+1<argc)hgp.redl=atoi(argv[++i]);
    else if(!strcmp(argv[i],"-redm")&&i+1<argc)strcpy(hgp.redm_str,argv[++i]);
    else if(!strcmp(argv[i],"-reda")&&i+1<argc)strcpy(hgp.rli_str,argv[++i]);
    else if(!strcmp(argv[i],"-reds")&&i+1<argc)hgp.ews=atoi(argv[++i]);
    else if(!strcmp(argv[i],"-g")   &&i+1<argc)strcpy(hgp.global_str,argv[++i]);
    else if(!strcmp(argv[i],"-l")   &&i+1<argc)strcpy(hgp.local_str,argv[++i]);
    else if(!strcmp(argv[i],"-d")   &&i+1<argc)zz.Debug_Level=atoi(argv[++i]); 
    else 
    { fprintf(stderr,"ERR...option '%s' not legal or without value\n",argv[i]);
      return 1;
    }
  }
  if (Zoltan_HG_Set_Part_Options(&zz, &hgp))
    return 1;
  ADD_NEW_TIME(t_rest);

/* load and info hypergraph */
  if (HG_Readfile(&zz,&hg,hgraphfile))
    return 1;
  memory_graph = Zoltan_Memory_Usage (ZOLTAN_MEM_STAT_TOTAL);
  printf("Initial Memory: %d %d\n", memory_graph,
         Zoltan_Memory_Usage (ZOLTAN_MEM_STAT_MAXIMUM) );
  if (Zoltan_HG_Info (&zz,&hg))
    return 1;
  if (Zoltan_HG_Check (&zz,&hg))
    return 1;
  ADD_NEW_TIME(t_load);

/* partition hypergraph */
  if (!((part) = (int*)calloc((unsigned)(hg.nVtx),sizeof(int))))
    return 1;
  if (Zoltan_HG_HPart_Lib(&zz,&hg,p,part,&hgp))
    return 1;
  ADD_NEW_TIME(t_part);

/* partition info */
  if (Zoltan_HG_HPart_Info (&zz,&hg,p,part))
    return 1;
  free(part);

/* free hypergraph */
  if (Zoltan_HG_HGraph_Free (&hg))
    return 1;
  if (Zoltan_Memory_Usage (ZOLTAN_MEM_STAT_TOTAL) > 0)
  { printf("ERROR: remaining memory: %d\n",Zoltan_Memory_Usage (ZOLTAN_MEM_STAT_TOTAL));
    return 1;
  }
  ADD_NEW_TIME(t_rest);
  END_TIME();
  times_output();

  printf("Final Memory: %d %d  ratio:%f\n",
         Zoltan_Memory_Usage (ZOLTAN_MEM_STAT_TOTAL),
         Zoltan_Memory_Usage (ZOLTAN_MEM_STAT_MAXIMUM),
         (float)Zoltan_Memory_Usage(ZOLTAN_MEM_STAT_MAXIMUM)/memory_graph );

  Zoltan_Memory_Stats();

  return 0;
}
