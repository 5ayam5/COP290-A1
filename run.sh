#!/bin/sh
cmake CMakeLists.txt
make

echo Enter destination parent folder
read dest
mkdir $dest
mkdir $dest/Subsampling
mkdir $dest/Resolution
mkdir $dest/SpatialThreading
mkdir $dest/TemporalThreading

./computeDensities_subsampling trafficvideo.mp4 1 > $dest/Subsampling/1.csv
./computeDensities_subsampling trafficvideo.mp4 2 > $dest/Subsampling/2.csv
./computeDensities_subsampling trafficvideo.mp4 5 > $dest/Subsampling/5.csv
./computeDensities_subsampling trafficvideo.mp4 10 > $dest/Subsampling/10.csv
./computeDensities_subsampling trafficvideo.mp4 20 > $dest/Subsampling/20.csv
echo Subsampling done!

./computeDensities_resolution trafficvideo.mp4 1.0 > $dest/Resolution/1_0.csv
./computeDensities_resolution trafficvideo.mp4 1.2 > $dest/Resolution/1_2.csv
./computeDensities_resolution trafficvideo.mp4 1.5 > $dest/Resolution/1_5.csv
./computeDensities_resolution trafficvideo.mp4 2.0 > $dest/Resolution/2_0.csv
./computeDensities_resolution trafficvideo.mp4 3.0 > $dest/Resolution/3_0.csv
./computeDensities_resolution trafficvideo.mp4 4.0 > $dest/Resolution/4_0.csv
echo Resolution done!

./computeDensities_temporalThreading trafficvideo.mp4 1 > $dest/TemporalThreading/1.csv
./computeDensities_temporalThreading trafficvideo.mp4 2 > $dest/TemporalThreading/2.csv
./computeDensities_temporalThreading trafficvideo.mp4 4 > $dest/TemporalThreading/4.csv
./computeDensities_temporalThreading trafficvideo.mp4 8 > $dest/TemporalThreading/8.csv
./computeDensities_temporalThreading trafficvideo.mp4 12 > $dest/TemporalThreading/12.csv
echo TemporalThreading done!

./computeDensities_spatialThreading trafficvideo.mp4 1 > $dest/SpatialThreading/1.csv
./computeDensities_spatialThreading trafficvideo.mp4 2 > $dest/SpatialThreading/2.csv
./computeDensities_spatialThreading trafficvideo.mp4 4 > $dest/SpatialThreading/4.csv
./computeDensities_spatialThreading trafficvideo.mp4 8 > $dest/SpatialThreading/8.csv
./computeDensities_spatialThreading trafficvideo.mp4 12 > $dest/SpatialThreading/12.csv
echo SpatialThreading done!

python3 $dest/utility.py
