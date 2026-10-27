// Pattern scanner for the Recaman sequence (experiment 03).
// Build:  gcc -O3 -march=native -o scan scan.c
// Run:    ./scan 1000000000 runs.csv checkpoints.csv
//
// Writes two CSV files:
//   runs.csv         every long zigzag run:  n_start,length,lo,hi,fwd_parity
//                    (fwd_parity = parity of n on the forward steps; decides orange-top vs orange-bottom)
//                    (a zigzag run = steps that strictly alternate back/forward;
//                     "long" = length >= max(2000, n_start/1000))
//   checkpoints.csv  at N = 1,2,5 x 10^k:  N, max, smallest_unvisited, unvisited_below_max
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef uint64_t u64;
static u64 *bits; static u64 cap;

static inline int get(u64 v){ return (bits[v>>6]>>(v&63))&1; }
static inline void setb(u64 v){ bits[v>>6] |= 1ULL<<(v&63); }
static void grow(u64 need){
    u64 nc=cap; while(nc<=need) nc*=2;
    bits=realloc(bits,nc/8); if(!bits){fprintf(stderr,"out of memory\n");exit(1);}
    memset(bits+cap/64,0,(nc-cap)/8); cap=nc;
}

int main(int argc,char**argv){
    if(argc!=4){fprintf(stderr,"usage: %s N runs.csv checkpoints.csv\n",argv[0]);return 1;}
    u64 N=strtoull(argv[1],0,10);
    FILE*fr=fopen(argv[2],"w"), *fc=fopen(argv[3],"w");
    fprintf(fr,"n_start,length,lo,hi,fwd_parity\n"); fprintf(fc,"N,max,smallest_unvisited,unvisited_below_max\n");
    cap=64; while(cap<4*N) cap*=2;
    bits=calloc(cap/64,8); if(!bits){fprintf(stderr,"out of memory\n");return 1;}

    u64 cur=0,max=0,lowest=1; setb(0);
    int prev_back=-1; u64 rs=1, rlen=0, rlo=0, rhi=0; int rdir0=0;   // current zigzag run
    u64 next_cp=1;
    for(u64 n=1;n<=N;n++){
        int back = (cur>n && !get(cur-n));
        cur = back ? cur-n : cur+n;
        if(cur>=cap) grow(cur);
        if(cur>max) max=cur;
        setb(cur);
        // zigzag bookkeeping
        if(prev_back!=-1 && back!=prev_back){ rlen++; if(cur<rlo)rlo=cur; if(cur>rhi)rhi=cur; }
        else {
            u64 thr = rs/1000 > 2000 ? rs/1000 : 2000;
            if(rlen>=thr) fprintf(fr,"%llu,%llu,%llu,%llu,%d\n",(unsigned long long)rs,(unsigned long long)rlen,(unsigned long long)rlo,(unsigned long long)rhi,(int)(rdir0 ? (rs+1)&1 : rs&1));
            rs=n; rlen=1; rlo=cur; rhi=cur; rdir0=back;
            // include the value before this step in the run range
            u64 before = back ? cur+n : cur-n; if(before<rlo)rlo=before; if(before>rhi)rhi=before;
        }
        prev_back=back;
        if(n==next_cp){
            while(get(lowest)) lowest++;
            u64 vis=0; for(u64 w=0;w<=max/64;w++) vis+=__builtin_popcountll(bits[w]);
            fprintf(fc,"%llu,%llu,%llu,%llu\n",(unsigned long long)n,(unsigned long long)max,(unsigned long long)lowest,(unsigned long long)(max+1-vis));
            // next checkpoint: 1,2,5,10,20,50,...
            u64 p=1; while(p*10<=n) p*=10;
            next_cp = (n/p==1)? 2*p : (n/p==2)? 5*p : 10*p;
        }
    }
    fclose(fr); fclose(fc); free(bits);
    printf("done N=%llu max=%llu\n",(unsigned long long)N,(unsigned long long)max);
    return 0;
}
