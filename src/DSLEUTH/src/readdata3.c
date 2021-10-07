/* Read the SLEUTH output control_stats.log file and compute OSM, then order */
/* This code was modified to correct unintended loss of members              */
/* of the top 50 OSM values.  June 23, 2015                                  */
#include <stdio.h>

void push_down(int);
float top50[50][20];
float osm_max[50];

int main(int argc, char* argv[])
{
FILE *fp;
int i, j = 0, done;	
int run, diff, brd, sprd, slp, rg;
float product, compare, pop, edges, clusters, size, leesalee, slope, pc_urban, xmean, ymean, rad, fmatch, osm;

char header1[200], header2[200], infile[20];

/* Skip the two header lines */
/*printf("Enter the input file name:");
fscanf(stdin, "%s", infile);
*/

 if(argc < 2) {
   return -1;
  }


fp = fopen (argv[1], "r");
fgets(header1, 200, fp);
fgets(header2, 200, fp);

for (i = 0; i < 50; i++) {
	for (j = 0; j < 20; j++ ) top50[i][j] = 0.0;
	osm_max[i] = 0.0;
}
j = 0;
do {
fscanf(fp, "%6d", &run);
fscanf(fp, "%f%f%f%f%f%f%f%f%f%f%f%f%f", &product, &compare, &pop, &edges, &clusters, &size, &leesalee, &slope, &pc_urban, &xmean, &ymean, &rad, &fmatch);
fscanf(fp, "%d%d%d%d%d%*1c", &diff, &brd, &sprd, &slp, &rg);

osm = compare * pop * edges * clusters * slope * xmean * ymean;
j = 49;
do {
	done = 0;
	if (osm > osm_max[j]) {
        push_down(j);
		top50[j][0] = (float) run;
		top50[j][1] = product;
		top50[j][2] = compare;
		top50[j][3] = pop;
		top50[j][4] = edges;
		top50[j][5] = clusters;
		top50[j][6] = size;
		top50[j][7] = leesalee;
		top50[j][8] = slope;
		top50[j][9] = pc_urban;
		top50[j][10] = xmean;
		top50[j][11] = ymean;
		top50[j][12] = rad;
		top50[j][13] = fmatch;
		top50[j][14] = (float) diff;
		top50[j][15] = (float) brd;
		top50[j][16] = (float) sprd;
		top50[j][17] = (float) slp;
		top50[j][18] = (float) rg;
		top50[j][19] = osm;
		done = 1;
		osm_max[j] = osm;
	}
	j--;
} while(j >= 0 && !done);
} while (!feof(fp));
fclose(fp);
fp = fopen("top50b.log","w");
fprintf(fp, "Top fifty from file: %s\n", infile);
fprintf(fp, "  OSM         Diff  Brd Sprd Slp Road\n");
for (i = 49; i >= 0; i--) {
		fprintf(fp, "%12.8f", top50[i][19]);
		fprintf(fp, "%5d", (int) top50[i][14]);
		fprintf(fp, "%5d", (int) top50[i][15]);
		fprintf(fp, "%5d", (int) top50[i][16]);
		fprintf(fp, "%5d", (int) top50[i][17]);
		fprintf(fp, "%5d", (int) top50[i][18]);
	fprintf(fp, "\n");
}
fclose (fp);
 return 0;
}

void push_down(int k)
{
    int m, n;

    m = 1;
    while (m <= k)
    {
     if (top50[m][19] < 0.00001) {m++; continue;}
     for (n = 0; n < 20; n++)
          { top50[m-1][n] = top50[m][n]; osm_max[m-1] = osm_max[m]; }
     m++;
    }
    
    return;
}
