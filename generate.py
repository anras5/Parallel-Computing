import csv
import random
import pandas as pd

def generate_csv(filename, num_rows):
    with open(filename, 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)

        for _ in range(num_rows):
            a = random.randint(1, 8)
            b = random.randint(1, 8)
            c = random.randint(1, 8)
            d = random.randint(1, 8)
            
            # Determine whether X is the sum or product of ABCD
            if random.random() < 0.005:
                x = a + b + c + d
            else:
                x = a * b * c * d

            writer.writerow([a, b, c, d, x])

    print(f"CSV file '{filename}' generated successfully.")


filename = 'data.csv'
num_rows = 10000

for i in range(10000, 500_000, 10_000):
    generate_csv(f"data/data_{i}.csv", i)
# generate_csv(filename, num_rows)
