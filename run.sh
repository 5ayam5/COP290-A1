#!/bin/sh
cmake CMakeLists.txt
make

./computeDensities_subsampling trafficvideo.mp4 1 > Subsampling/1.csv
./computeDensities_subsampling trafficvideo.mp4 2 > Subsampling/2.csv
./computeDensities_subsampling trafficvideo.mp4 5 > Subsampling/5.csv
./computeDensities_subsampling trafficvideo.mp4 10 > Subsampling/10.csv
./computeDensities_subsampling trafficvideo.mp4 20 > Subsampling/20.csv
echo Subsampling done!

./computeDensities_resolution trafficvideo.mp4 1.0 > Resolution/1_0.csv
./computeDensities_resolution trafficvideo.mp4 1.2 > Resolution/1_2.csv
./computeDensities_resolution trafficvideo.mp4 1.5 > Resolution/1_5.csv
./computeDensities_resolution trafficvideo.mp4 2.0 > Resolution/2_0.csv
./computeDensities_resolution trafficvideo.mp4 3.0 > Resolution/3_0.csv
./computeDensities_resolution trafficvideo.mp4 4.0 > Resolution/4_0.csv
echo Resolution done!

./computeDensities_temporalThreading trafficvideo.mp4 1 > TemporalThreading/1.csv
./computeDensities_temporalThreading trafficvideo.mp4 2 > TemporalThreading/2.csv
./computeDensities_temporalThreading trafficvideo.mp4 4 > TemporalThreading/4.csv
./computeDensities_temporalThreading trafficvideo.mp4 8 > TemporalThreading/8.csv
./computeDensities_temporalThreading trafficvideo.mp4 12 > TemporalThreading/12.csv
echo TemporalThreading done!

./computeDensities_spatialThreading trafficvideo.mp4 1 > SpatialThreading/1.csv
./computeDensities_spatialThreading trafficvideo.mp4 2 > SpatialThreading/2.csv
./computeDensities_spatialThreading trafficvideo.mp4 4 > SpatialThreading/4.csv
./computeDensities_spatialThreading trafficvideo.mp4 8 > SpatialThreading/8.csv
./computeDensities_spatialThreading trafficvideo.mp4 12 > SpatialThreading/12.csv
echo SpatialThreading done!
