# Data preprocessing script to filter duplicates and normalize audio feature values for unbiased k-nn
import csv


# Helper functions
def to_float(value):
    value = value.strip()
    if value == '':
        return 0.0
    return float(value)


# Main script
input_path = 'spotify_data.csv'
output_path = 'dataset.csv'

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

filtered_rows = []
seen_track_ids = set()
seen_name_artist = set()

for row in rows:
    track_id = row['track_id'].strip()
    name_artist = (
        row['track_name'].strip().lower(),
        row['artists'].strip().lower()
    )

    if track_id != '' and track_id in seen_track_ids:
        continue

    if name_artist in seen_name_artist:
        continue

    if track_id != '':
        seen_track_ids.add(track_id)
    seen_name_artist.add(name_artist)
    filtered_rows.append(row)

mins = {col: float('inf') for col in cols}
maxs = {col: float('-inf') for col in cols}

for row in filtered_rows:
    for col in cols:
        value = to_float(row[col])
        if value < mins[col]:
            mins[col] = value
        if value > maxs[col]:
            maxs[col] = value

for row in filtered_rows:
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
    writer.writerows(filtered_rows)
