#include "datasets.h"
#include "neural_network.h"
#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <stdlib.h> 

// compiled and executed on university computers ( specifically on opti3060ws03 )
//plot "Team0.txt" , "Team1.txt" , "centroids.txt"
//plot "Team0.txt", "Team1.txt" , "Team2.txt", "centroids.txt"
//plot "Team0.txt", "Team1.txt" , "Team2.txt", "Team3.txt" ,"Team4.txt", "centroids.txt"
// etc for more 

#define NUMBER_OF_POINTS 900 // total 6 * 150
#define M 10 // M is 2,3,4,5,6,7,10
#define DIMENSIONS 2
#define ITERATIONS 5

Point k_points[NUMBER_OF_POINTS]; // k_points.x1 , k_points.x2
Point centroids[M];
FILE *s2_file; 
double previous_error, current_error;
double total_error = 100000000000000;
double max_error = DBL_MAX;
double total_dist[M];

void open_files(){
    
    if((s2_file = fopen("S2.txt", "r")) == NULL){
        printf("Couldn't open 'S2.txt, propably doesn't exist yet\n");
        printf("Remember to make the dataset first!\n");
        exit(1);
    }

}


void close_files(){

    fclose(s2_file);

}


void load_dataset(){

    Point new_point;

    for(int i=0; i<NUMBER_OF_POINTS; i++){
        if(fscanf(s2_file, "%f", &new_point.x1) == EOF) break;
        if(fscanf(s2_file, "%f", &new_point.x2) == EOF) break;
        k_points[i] = new_point;
    }

}


double distance(Point one, Point two){

    return pow(one.x1 - two.x1, 2) + pow(one.x2 - two.x2, 2);
    //return sqrt( ( ( one.x1 - two.x1 ) * ( one.x1 - two.x1 ) ) + ( ( one.x2 - two.x2 ) * ( one.x2 - two.x2 ) ) );
    // Euclidean distance
}


int * lvq( ) {

    int * clusters_label = (int * ) calloc(NUMBER_OF_POINTS, sizeof(int));
    int step;
    int i,j,k,w,y,z;
    float epoch, repeats = 0.0;
    float n = 0.1;
    Point current_centroids[M], temp_centroids[M];
    previous_error, current_error = DBL_MAX;


    for( w = 0; w < M; w ++){
        step = rand() % (int) (NUMBER_OF_POINTS);
        current_centroids[w] = k_points[step];
        //printf("Current error : %f\n", current_centroids[w].x1);
        //printf("Current error : %f\n", current_centroids[w].x2);
    }

    while(fabs( current_error - previous_error ) > epoch ) { 
        
        for(w = 0; w<M; w++){
            total_dist[w] = 0.0;
        }

        previous_error = current_error;
        current_error = 0.0; // reset errors

        // reset cluster sizes and temporary centroids ( otherwise it gets too big )
        for( y = 0; y < M; y++ ){
            temp_centroids[y].x1 = 0.0;
            temp_centroids[y].x2 = 0.0;
        }

       // printf("size : %f\n", temp_centroids[i].x1);
        for( i = 0; i < NUMBER_OF_POINTS; i++ ){
            double min_dist = DBL_MAX;
            for( j = 0; j < M; j++ ){
                double dist = 0.0;
                dist += distance(k_points[i], current_centroids[j]);
                if( dist < min_dist ){
                    clusters_label[i] = j;
                    //printf("cluber %i\n", clusters_label[i]);
                    min_dist = dist;
                }
                total_dist[j] += dist; 
            }

            // update the temporary centroids of each cluster
            current_centroids[clusters_label[i]].x1 += n*(k_points[i].x1 - current_centroids[clusters_label[i]].x1);
            current_centroids[clusters_label[i]].x2 += n*(k_points[i].x2 - current_centroids[clusters_label[i]].x2);

            current_error += min_dist;
        }

        for( i = 0; i < M; i++)
            centroids[i] = current_centroids[i];
        repeats++;
        if(fabs( current_error - previous_error ) < epoch)
            break;
        else{
            epoch++; 
            n=n*0.95;
        }
        //printf("epoch : %f\n", epoch );
    } 
    //printf("epoch : %f\n", epoch );
    // clean up the ( now ) unwanted memory

    return clusters_label;    
}

void write_files(){

    int *lvq_result[ITERATIONS];
    int index, w;
    double final_centroids[ITERATIONS][M][2];
    for ( int i=0; i< ITERATIONS; i++ ){

        lvq_result[i] = lvq();

        for( int j = 0; j<M; j++){
            final_centroids[i][j][0] = centroids[j].x1;
            final_centroids[i][j][1] = centroids[j].x2;
        }

        double current_distance = 0.0;
        for( w = 0; w < M; w++)
            //printf("Current dist : %lf\n", total_dist[w]);
            current_distance += total_dist[w];
       // printf("Current dist : %lf\n", current_distance);
        if(total_error > current_distance ){
            total_error = current_distance;
            index = i;
        }
    }

    printf("Minimum Error : %lf\n", total_error);
    printf("Output files \n");
    for (int i = 0; i < NUMBER_OF_POINTS; i++) {
        //printf("data point %d is in cluster %d\n", i, bestRun[i]);
        FILE *fp;
        char  filename[100];
        //printf("Index : %d\n", k_result[index][i]);
        // convert 123 to string [buf]
        sprintf(filename,"lvq_Team%d.dat", lvq_result[index][i]);
        //printf("STR : %s\n", filename);
        fp = fopen(filename, "a");
        fprintf(fp, "%f\t%f\n",k_points[i].x1, k_points[i].x2);
        fclose(fp);
    }

    for (int i = 0; i < M; i++) {
        FILE *fp;
        fp = fopen("lvq_centroids.dat", "a");
        fprintf(fp, "%f\t%f\n",final_centroids[index][i][0] ,final_centroids[index][i][1]);
        fclose(fp);
    }

    printf(" %d Files were created!\n", M+1);
}

int main(void){

    srand(time(NULL));
    open_files();
    load_dataset();
    write_files();

    close_files();
    
    return 0;
}