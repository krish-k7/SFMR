# Data preprocessing script to normalize audio feature values for unbiased k-nn
import csv


# Helper functions
def to_float(value):
    value = value.strip()
    if value == '':
        return 0.0
    return float(value)


# Main script
input_path = 'dataset.csv'
output_path = 'dataset_normalized.csv'

cols = [
    'danceability',
    'energy',
    'loudness',
    'speechiness',
    'acousticness',
    'instrumentalness',
    'liveness',
    'valence',
    'tempo',
]

with open(input_path, 'r', newline='', encoding='utf-8') as infile:
    reader = csv.DictReader(infile)
    fieldnames = reader.fieldnames
    rows = list(reader)

mins = {col: float('inf') for col in cols}
maxs = {col: float('-inf') for col in cols}

for row in rows:
    for col in cols:
        value = to_float(row[col])
        if value < mins[col]:
            mins[col] = value
        if value > maxs[col]:
            maxs[col] = value

for row in rows:
    for col in cols:
        value = to_float(row[col])
        min_value = mins[col]
        max_value = maxs[col]

        if max_value == min_value:
            normalized = 0.0
        else:
            normalized = (value - min_value) / (max_value - min_value)

        row[col] = str(normalized)

with open(output_path, 'w', newline='', encoding='utf-8') as outfile:
    writer = csv.DictWriter(outfile, fieldnames=fieldnames)
    writer.writeheader()
    writer.writerows(rows)
