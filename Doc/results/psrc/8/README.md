#how the experiment was setup
./src/grow calibrate Scenarios/scenario.demo200_calibrate 
#purpose
observe the effect of the specific loop parallelism in function called spr_urbanize
#pragma omp parallel num_threads(2)
#pragma omp parallel num_threads(2)
{
    #pragma omp sections
    {
        #pragma omp section
        {
            nrows = igrid_GetNumRows ();
        }
        #pragma omp section
        {
            ncols = igrid_GetNumCols ();
        }
    }
}
#scenario file
scenario.demo200_calibrate
#timing result
29s