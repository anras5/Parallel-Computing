import csv


def main():
    with open('grouped.csv', 'r') as filein, open('grouped_doku.csv', 'w') as fileout:
        reader = csv.reader(filein)

        headers = next(reader)
        fileout.write(''.join('^ ' + header for header in headers) + ' ^\n')

        for row in reader:
            fileout.write(''.join('| ' + element for element in row) + ' |\n')


if __name__ == '__main__':
    main()
