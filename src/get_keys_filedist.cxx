/*
 *  * (C) Copyright 2005- ECMWF.
 *   *
 *    * This software is licensed under the terms of the Apache Licence Version 2.0
 *     * which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
 *      *
 *       * In applying this licence, ECMWF does not waive the privileges and immunities granted to it by
 *        * virtue of its status as an intergovernmental organisation nor does it submit to any jurisdiction.
 *         */
 
/*
 *  * C Implementation: grib_get_keys
 *   *
 *    * Description: how to get values using keys from GRIB messages
 *     *
 *      */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "eccodes.h"
#include <ctime>
#include <iostream>
#include <chrono>
#include <sys/time.h>
#include <vector>
#include "mpi.h"


struct pool{
    double* data;
    size_t size;
    size_t current_ptr;
};

int main(int argc, char** argv)
{
    MPI_Init(&argc, &argv);

    
    int rank;
    int mpi_comm_size;

    MPI_Comm_size(MPI_COMM_WORLD, &mpi_comm_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int err = 0;
    int nfields = 1350;
    std::vector<pool> values = {{NULL,0,0}};

    size_t values_len= 0;
    size_t values_len_tot = 0;
    size_t i = 0, len = 0;
 
    double latitudeOfFirstGridPointInDegrees;
    double longitudeOfFirstGridPointInDegrees;
    double latitudeOfLastGridPointInDegrees;
    double longitudeOfLastGridPointInDegrees;
 
    double jDirectionIncrementInDegrees;
    double iDirectionIncrementInDegrees;
 
    long numberOfPointsAlongAParallel;
    long numberOfPointsAlongAMeridian;
    long paramId;
 
    double average = 0;
    char* packingType = NULL;
 
    FILE* in = NULL;
    std::vector<std::string> files = {"/scratch/cosuna/eccodes_bench/laf2020021715_r1","/scratch/cosuna/eccodes_bench/laf2020021715_r2","/scratch/cosuna/eccodes_bench/laf2020021715_r3","/scratch/cosuna/eccodes_bench/laf2020021715_r4"};
    
    if(mpi_comm_size > 4)  {
       std::cerr << "wrong number mpi" << std::endl;
       exit(1);
    }
    codes_handle *h = NULL;
    std::string filename = files[rank];

    in = fopen(filename.c_str(),"r");
    if(!in) {
        printf("ERROR: unable to open file %s\n",filename);
        return 1;
    }
 
    
    auto start = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

    int cnt =0;
    /* create new handle from the first message in the file*/
    while( h = codes_handle_new_from_file(0, in, PRODUCT_GRIB, &err) ) {
 
    /* Store the filename in the key "file" for this handle */
    len = filename.size();
    CODES_CHECK(codes_set_string(h, "file", filename.c_str(), &len), 0);

    /* get as a long*/
    CODES_CHECK(codes_get_long(h,"paramId",&paramId),0);
//    printf("paramId=%ld\n",paramId);
 
    /* get as a long*/
    CODES_CHECK(codes_get_long(h,"Ni",&numberOfPointsAlongAParallel),0);
//    printf("numberOfPointsAlongAParallel=%ld\n",numberOfPointsAlongAParallel);
 
    /* get as a long*/
    CODES_CHECK(codes_get_long(h,"Nj",&numberOfPointsAlongAMeridian),0);
//    printf("numberOfPointsAlongAMeridian=%ld\n",numberOfPointsAlongAMeridian);
 
    /* get as a double*/
    CODES_CHECK(codes_get_double(h,"latitudeOfFirstGridPointInDegrees",&latitudeOfFirstGridPointInDegrees),0);
//    printf("latitudeOfFirstGridPointInDegrees=%g\n",latitudeOfFirstGridPointInDegrees);
 
    /* get as a double*/
    CODES_CHECK(codes_get_double(h,"longitudeOfFirstGridPointInDegrees",&longitudeOfFirstGridPointInDegrees),0);
//    printf("longitudeOfFirstGridPointInDegrees=%g\n",longitudeOfFirstGridPointInDegrees);
 
    /* get as a double*/
    CODES_CHECK(codes_get_double(h,"latitudeOfLastGridPointInDegrees",&latitudeOfLastGridPointInDegrees),0);
//    printf("latitudeOfLastGridPointInDegrees=%g\n",latitudeOfLastGridPointInDegrees);
 
    /* get as a double*/
    CODES_CHECK(codes_get_double(h,"longitudeOfLastGridPointInDegrees",&longitudeOfLastGridPointInDegrees),0);
//    printf("longitudeOfLastGridPointInDegrees=%g\n",longitudeOfLastGridPointInDegrees);
 
    /* get as a double*/
    CODES_CHECK(codes_get_double(h,"jDirectionIncrementInDegrees",&jDirectionIncrementInDegrees),0);
//    printf("jDirectionIncrementInDegrees=%g\n",jDirectionIncrementInDegrees);
 
    /* get as a double*/
    CODES_CHECK(codes_get_double(h,"iDirectionIncrementInDegrees",&iDirectionIncrementInDegrees),0);
//    printf("iDirectionIncrementInDegrees=%g\n",iDirectionIncrementInDegrees);
 
    /* get as string */
    CODES_CHECK(codes_get_length(h, "packingType", &len), 0);
    packingType = (char*)malloc(len*sizeof(char));
    codes_get_string(h, "packingType", packingType, &len);
    free(packingType);
 
    /* get the size of the values array*/
    CODES_CHECK(codes_get_size(h,"values",&values_len),0);
    values_len_tot += values_len;

    if(values.back().data == NULL) {
        values.back().size = values_len*10;
        values.back().data = (double*)malloc(values.back().size*sizeof(double));
        values.back().current_ptr = 0;
    }


    if(values.back().current_ptr + values_len > values.back().size) {
        size_t newsize = values.back().size*2;
        values.push_back({(double*)malloc(newsize*sizeof(double)), newsize, 0});
    }
    /* get data values*/
      CODES_CHECK(codes_get_double_array(h,"values",&(values.back().data[values.back().current_ptr]),&values_len),0);
 
      values.back().current_ptr += values_len;

      codes_handle_delete(h);
      cnt++;
    }

    auto end = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    std::cout << "TIME : " << end - start << std::endl;
    std::cout << "Total size : " << values_len_tot << std::endl;
    std::cout << "N records : " << cnt << std::endl;
    std::cout << "bandwidth : " << values_len_tot*4 / ((end-start)/1000.) / (1024.*1024.*1024.) <<  " GB/s" << std::endl;
    for (int i=0; i < values.size(); ++i) {
        free(values[i].data);
    }
    fclose(in);
 
    return 0;
}
