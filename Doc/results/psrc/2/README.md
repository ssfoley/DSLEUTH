#how the experiment was setup
./src/grow calibrate Scenarios/wb100_calibrate 
#purpose
observe the effect of the innermost task parallelism
for (diffusion_coeff = coeff_GetStartDiffusion ();
    diffusion_coeff <= coeff_GetStopDiffusion ();
    diffusion_coeff += coeff_GetStepDiffusion ())
{
    for (breed_coeff = coeff_GetStartBreed ();
        breed_coeff <= coeff_GetStopBreed ();
        breed_coeff += coeff_GetStepBreed ())
    {
        for (spread_coeff = coeff_GetStartSpread ();
            spread_coeff <= coeff_GetStopSpread ();
            spread_coeff += coeff_GetStepSpread ())
        {
            for (slope_resistance = coeff_GetStartSlopeResist ();
                slope_resistance <= coeff_GetStopSlopeResist ();
                slope_resistance += coeff_GetStepSlopeResist ())
            {
                #pragma omp parallel for default(shared) num_threads(NUM_THREADS)
                for (road_gravity = coeff_GetStartRoadGravity ();
                road_gravity <= coeff_GetStopRoadGravity ();
                road_gravity += coeff_GetStepRoadGravity ())
                {
                    omission
                }
            }
        }
    }
}
#scenario file
wb100_calibrate 
The start value is 0
The step value is 100
The stop value is 100
#timing result
