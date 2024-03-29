import csv
import random
from pathlib import Path

def generate_csv(filename, num_rows, num_columns):
    with open(filename, 'w', newline='') as csvfile:
        writer = csv.writer(csvfile)

        for _ in range(num_rows):
            numbers = [random.randint(1, 8) for _ in range(num_columns)]
            
            # Determine whether X is the sum of numbers or an anomaly
            if random.random() < 0.005:
                x = 0
            else:
                x = sum(numbers)

            numbers.append(x)
            writer.writerow(numbers)

    print(f"CSV file '{filename}' generated successfully.")


def main():
    directory = Path("data")
    if not directory.exists():
        directory.mkdir(parents=True)

    columns = 10
    for rows in range(10_000, 60_001, 1000):
        generate_csv(f"data/data_{rows}.csv", rows, columns)

if __name__ == '__main__':
    main()


