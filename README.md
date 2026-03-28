# SFMR

SFMR (Simple & Fast Music Recommendations) is a music recommendation program implemented with two spatial data structures:

- K-D Trees
- Vantage Point Trees

Given an input song, it determines the `k` most similar songs based on distances between their audio feature vectors.

## Setup

1. Download the Spotify tracks dataset from Kaggle:
https://www.kaggle.com/datasets/amitanshjoshi/spotify-1million-tracks/data
2. Place the CSV file in the `datasets` directory.
3. In your terminal, change to the `datasets` directory and run the data preprocessing script.

This script will output `dataset.csv`, which is the preprocessed dataset that SFMR should load.

## Running SFMR
### Option 1. Build from source
To build from source and execute the output binary, run:
```
make
./SFMR
```

### Option 2. Run the precompiled binary

If you are on Linux (x86), you can run the precompiled binary instead of building from source:
```
chmod +x SFMR
./SFMR
```