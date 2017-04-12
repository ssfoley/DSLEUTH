#how the experiment was setup
./src/grow calibrate Scenarios/scenario.demo200_calibrate 
#purpose
observe the effect of the specific loop parallelism in function called spr_GetDiffusionValue
#pragma omp parallel num_threads(2)
{
    #pragma omp sections
    {
        #pragma omp section
        {
        rows_sq = igrid_GetNumRows () * igrid_GetNumRows ();
        }
        #pragma omp section
        {
        cols_sq = igrid_GetNumCols () * igrid_GetNumCols ();
        }
    }
}
#scenario file
scenario.demo200_calibrate
#timing result
21s