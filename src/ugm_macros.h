#include <unistd.h>
#include "scenario_obj.h"
#include "ugm_defines.h"
#include "globals.h"
#ifdef MAIN_MODULE
  char glb_call_stack[CALL_STACK_SIZE][MAX_FILENAME_LEN];
  int glb_call_stack_index;
#else
  extern char glb_call_stack[CALL_STACK_SIZE][MAX_FILENAME_LEN];
  extern int glb_call_stack_index;
#endif

#ifdef MPI
  #define EXIT(code) MPI_Abort(MPI_COMM_WORLD,code)
#else
  #define EXIT(code) exit(code)
#endif

#define NUM_THREADS 2

#define TRANS_OFFSET(i,j) (i)*landclass_GetNumLandclasses() + (j)

#define PRINT_INT(x) printf("%s = %d\n",#x,(x))

#define PRINT_FLOAT(x) printf("%s = %f\n",#x,(x))

#define LOG_INT(fp,x) fprintf(fp,"%s %u %s = %d\n",                       \
           __FILE__,__LINE__,#x,(x))

#define LOG_FLOAT(fp,x) fprintf(fp,"%s %u %s = %f\n",                     \
          __FILE__,__LINE__,#x,(x))

#define LOG_STRING(fp,x) fprintf(fp,"%s %u %s = %s\n",                    \
          __FILE__,__LINE__,#x,(x))

#ifndef lint
  #define LOG_MEM_CHAR_ARRAY(fp,ptr,size,count)                           \
        fprintf(fp,"%dl s %-27s size=%5ul bytes count=%4ul %s %ul\n",         \
               (ptr),#ptr,(size),(count),__FILE__,__LINE__)

  #define LOG_MEM(fp,ptr,size,count)                                      \
        fprintf(fp,"%dl s %-27s size=%5ul bytes count=%4ul %s %ul\n",         \
               (ptr),#ptr,(size),(count),__FILE__,__LINE__);              \
        fprintf(fp,"%dl e %-27s END OF %s %ul\n",                           \
               (ptr)+count,#ptr,__FILE__,__LINE__)
#else
  #define LOG_MEM_CHAR_ARRAY(fp,ptr,size,count)                           \
        fprintf(fp,"lint test\n")
  #define LOG_MEM(fp,ptr,size,count)                                      \
        fprintf(fp,"lint test\n")
#endif


#define LOG_ERROR(str)                                                    \
        fprintf(stderr,                                                   \
          "\n\nERROR PE: %u line: %d Func: %s Module: %s \n%s\n",         \
           glb_mype,__LINE__,func,__FILE__,(str));                        \
        sprintf(glb_filename,"%sERROR_LOG_%u",                            \
          scen_GetOutputDir(),glb_mype);                                  \
        glb_fp = fopen(glb_filename, "a");                                \
        fprintf(glb_fp,                                                   \
          "\n\nERROR at line: %d Func: %s Module: %s \n%s\n",             \
          __LINE__,func,__FILE__,(str));                                  \
        fclose(glb_fp);                                                   \
        if(scen_GetLogFlag())                                             \
        {                                                                 \
          scen_Append2Log();                                              \
          fprintf(scen_GetLogFP(),                                        \
            "\n\nERROR at line: %d Func: %s Module: %s \n%s\n",           \
            __LINE__,func,__FILE__,(str));                                \
          fflush(scen_GetLogFP());                                        \
          scen_CloseLog();                                                \
        }

#define MAX(a,b) (((a)>(b))?(a):(b))

#define MIN(a,b) (((a)<(b))?(a):(b))

#define OFFSET(i,j)    ((i)*igrid_GetNumCols() + (j))

#define IMAGE_PT(row,col)                                                 \
        (((row) <  igrid_GetNumRows()) &&                                 \
         ((col) <  igrid_GetNumCols()) &&                                 \
         ((row) >= 0)         &&                                          \
         ((col) >= 0))

#define INTERIOR_PT(row,col)                                              \
        (((row) < igrid_GetNumRows() - 1) &&                              \
         ((col) < igrid_GetNumCols() - 1) &&                              \
         ((row) > 0)             &&                                       \
         ((col) > 0))

#define URBANIZE(row,col)                                                 \
        (z[OFFSET ((row),(col))] == 0) &&                                 \
        (delta[OFFSET ((row),(col))] == 0) &&                             \
        (RANDOM_FLOAT < swght[slp[OFFSET ((row),(col))]]) &&              \
        (excld[OFFSET ((row),(col))] < RANDOM_INT (100))

#define FUNC_INIT                                                         \
        CALL_TRACE;                                                       \
        glb_call_stack_index++;                                           \
        if( glb_call_stack_index >= CALL_STACK_SIZE )                     \
        { printf("\n ERROR: call_stack overflow\n");                      \
          printf("increase the size of CALL_STACK_SIZE and recompile\n"); \
          for(glb_i=0;glb_i<glb_call_stack_index;glb_i++)                 \
          {                                                               \
            printf("glb_call_stack[%u]=%s\n",glb_i,glb_call_stack[glb_i]);\
          }                                                               \
        }                                                                 \
        strcpy(glb_call_stack[glb_call_stack_index],func)
       
#define FUNC_END                                                          \
        glb_call_stack_index--;                                           \
        if(glb_call_stack_index<0)                                        \
        {                                                                 \
          printf("%s %u %s ERROR glb_call_stack_index= %d\n"              \
               ,__FILE__,__LINE__,func,glb_call_stack_index);             \
          EXIT(1);                                                        \
        }                                                                 \
        RETURN_TRACE

#define ROUND_BYTES_TO_WORD_BNDRY(bytes) (((bytes)+(BYTES_PER_WORD)-1)/   \
         (BYTES_PER_WORD))*(BYTES_PER_WORD)

#ifdef  CALL_TRACING
  #define CALL_TRACE printf("\nline: %d Entered Func: %s Module: %s\n",   \
               __LINE__,func,__FILE__)
#else
  #define CALL_TRACE
#endif

#ifdef  RETURN_TRACING
  #define RETURN_TRACE                                                    \
            printf("\nline: %d Returning from Func: %s Module: %s\n",     \
            __LINE__,func,__FILE__)
#else
  #define RETURN_TRACE
#endif

#define TRACE printf("\nPE: %u line: %d Func: %s Module: %s\n",           \
         glb_mype,__LINE__,func,__FILE__)

#define PRINT_ERROR(str)                                                  \
        printf("\n\nERROR at PE: %u line: %d Func: %s Module: %s \n%s\n", \
          glb_mype,__LINE__,func,__FILE__,(str))

#define PRINT_MSG(str)                                                    \
        printf("\nPE: %u line: %d Func: %s Module: %s \n%s\n",            \
         glb_mype,__LINE__,func,__FILE__,(str))

#define FILE_OPEN(fp,name,options)                                        \
        (fp) = fopen((name),(options));                                   \
        while((EMFILE==errno)&&(fp==NULL))                                \
        {                                                                 \
          sprintf (msg_buf,"%s %s\n%s %s\n",                              \
            "Unable to open file: ",(name),strerror (errno),              \
            " Trying again");                                             \
          sleep(3);                                                       \
          (fp) = fopen((name),(options));                                 \
          if(fp)                                                          \
          {                                                               \
            sprintf(msg_buf,"Successfully opened file: %s",(name));       \
            PRINT_MSG(msg_buf);                                           \
          }                                                               \
        }                                                                 \
        if(fp==NULL)                                                      \
        {                                                                 \
          sprintf (msg_buf,"Unable to open file: %s\n", (name));          \
          PRINT_MSG(msg_buf);                                             \
          EXIT(1);                                                        \
        }

#ifdef MPI
#define MPI_SingleFileIn                                                  \
        if(glb_mype != 0)                                                 \
        {                                                                 \
          MPI_Recv(&glb_token,1,MPI_INT,MPI_ANY_SOURCE,1,MPI_COMM_WORLD,  \
            &glb_mpi_status);                                             \
        }

#define MPI_SingleFileOut                                                 \
        MPI_Send(&glb_token,1,MPI_INT,glb_mype+1,1,MPI_COMM_WORLD)
#endif

