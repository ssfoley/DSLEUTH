#how the experiment was setup
./src/grow calibrate Scenarios/scenario.demo200_calibrate 
#purpose
observe the effect of the specific loop parallelism in function called spr_spread
#pragma omp parallel for default(shared) reduction(+:temp1,temp2) schedule(dy\
namic, 2048)
for (i = 0; i < total_pixels; i++)
{
    if ((z[i] == 0) && (delta[i] > 0))
    {
        /* new growth being placed into array */
        temp2 += (float) slp[i];
        z[i] = delta[i];
        temp1++;
    }
}

(*num_growth_pix) = temp1;
(*average_slope) = temp2;
#scenario file
scenario.demo200_calibrate
#timing result
30s