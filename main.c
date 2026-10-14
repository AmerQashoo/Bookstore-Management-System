/*ID: 1241585
NAME: AMER QASHOO
SECTION: LAB:3 / DISCUSSION: 6 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

#define MAXSIZE 100
#define NAMESIZE 50
#define DATAFILE "books.txt"

/* ---- main operations (same as original) ---- */
void addBook(int bins[], double prices[], char names[][NAMESIZE], int *size);
void removeBook(int bins[], double prices[], char names[][NAMESIZE], int *size);
void searchForBook(int bins[], double prices[], char names[][NAMESIZE], int size);
void uploadDataFile(int bins[], double prices[], char names[][NAMESIZE], int *size);
void updateDataFile(int bins[], double prices[], char names[][NAMESIZE], int size);
void printBooks(int bins[], double prices[], char names[][NAMESIZE], int size);
void displaymenu(int operation, int bins[], double prices[], char names[][NAMESIZE], int size);

/* ---- small helpers (new) ---- */
static void clearLine(void);
static int  readInt(int *out);          /* 1 = ok, 0 = invalid, -1 = EOF */
static int  readDouble(double *out);    /* 1 = ok, 0 = invalid, -1 = EOF */
static int  readName(char *out);        /* 1 = ok, 0 = invalid/empty, -1 = EOF */
static int  findBin(const int bins[], int size, int bin);
static void insertSorted(int bins[], double prices[], char names[][NAMESIZE],
                         int *size, int bin, double price, const char *name);
static int  parseLine(char *line, char *name, int *bin, double *price);
static int  backupDataFile(void);
static void printHeader(void);
static void printRow(const char *name, int bin, double price);
static void printFooter(void);
static int safeToSave = 1;

int main()
{
    int operation = 0;
    int bins[MAXSIZE];
    double prices[MAXSIZE];
    char names[MAXSIZE][NAMESIZE];
    int size = 0;
    printf("Welcome to My BookStore Management System:\n\n");
    uploadDataFile(bins, prices, names, &size);
    displaymenu(operation, bins, prices, names, size);

    return 0;
}

/* ------------------------------------------------------------------ */
/*                              HELPERS                               */
/* ------------------------------------------------------------------ */

/* throw away the rest of the current input line */
static void clearLine(void)
{
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

/* read one whole line from stdin; returns -1 on EOF.
   If the line was longer than the buffer the rest is discarded. */
static int readLine(char *buf, int n)
{
    if (fgets(buf, n, stdin) == NULL)
        return -1;
    if (strchr(buf, '\n') == NULL && !feof(stdin))
        clearLine();
    return 0;
}

static int readInt(int *out)
{
    char buf[64], *end;
    long v;
    if (readLine(buf, sizeof buf) == -1)
        return -1;
    v = strtol(buf, &end, 10);
    if (end == buf)
        return 0;
    while (*end == ' ' || *end == '\t')
        end++;
    if (*end != '\n' && *end != '\0')
        return 0;
    if (v < INT_MIN || v > INT_MAX)
        return 0;
    *out = (int)v;
    return 1;
}

static int readDouble(double *out)
{
    char buf[64], *end;
    double v;
    if (readLine(buf, sizeof buf) == -1)
        return -1;
    v = strtod(buf, &end);
    if (end == buf)
        return 0;
    while (*end == ' ' || *end == '\t')
        end++;
    if (*end != '\n' && *end != '\0')
        return 0;
    if (!(v >= 0) || v > 1000000000.0) /* reject invalid or unrealistic prices */
        return 0;
    *out = v;
    return 1;
}

/* reads a full line (spaces allowed), trims it, keeps at most NAMESIZE-1 chars */
static int readName(char *out)
{
    char buf[256];
    char *start;
    size_t len;
    if (readLine(buf, sizeof buf) == -1)
        return -1;
    start = buf;
    while (isspace((unsigned char)*start))
        start++;
    len = strlen(start);
    while (len > 0 && isspace((unsigned char)start[len - 1]))
        start[--len] = '\0';
    if (len == 0)
        return 0;
    if (len >= NAMESIZE)
    {
        printf("Book name is too long (maximum 49 characters).\n");
        return 0;
    }
    strcpy(out, start);
    return 1;
}

/* returns index of bin, or -1 if not found */
static int findBin(const int bins[], int size, int bin)
{
    for (int i = 0; i < size; i++)
        if (bins[i] == bin)
            return i;
    return -1;
}

/* keeps the list sorted by bin number (same logic as the original addBook) */
static void insertSorted(int bins[], double prices[], char names[][NAMESIZE],
                         int *size, int bin, double price, const char *name)
{
    int position = 0;
    while (position < (*size) && bins[position] < bin)
        position++;
    for (int i = (*size); i > position; i--)
    {
        bins[i] = bins[i - 1];
        prices[i] = prices[i - 1];
        strcpy(names[i], names[i - 1]);
    }
    bins[position] = bin;
    prices[position] = price;
    strcpy(names[position], name);
    (*size)++;
}

/* parses a line of the form:  <name possibly with spaces>  <bin>  <price>
   (parsed from the END of the line, so names with spaces are fine and old
   files written by the original program still load). */
static int parseLine(char *line, char *name, int *bin, double *price)
{
    char *p, *end;
    size_t len = strlen(line);
    long b;

    while (len > 0 && isspace((unsigned char)line[len - 1]))
        line[--len] = '\0';

    p = line + strlen(line);
    while (p > line && !isspace((unsigned char)p[-1]))
        p--;
    if (p > line) p--;
    if (p == NULL)
        return 0;
    *price = strtod(p + 1, &end);
    if (end == p + 1 || *end != '\0' || !(*price >= 0) || *price > 1000000000.0)
        return 0;
    *p = '\0';

    len = strlen(line);
    while (len > 0 && isspace((unsigned char)line[len - 1]))
        line[--len] = '\0';

    p = line + strlen(line);
    while (p > line && !isspace((unsigned char)p[-1]))
        p--;
    if (p > line) p--;
    if (p == NULL)
        return 0;
    b = strtol(p + 1, &end, 10);
    if (end == p + 1 || *end != '\0' || b < 0 || b > INT_MAX)
        return 0;
    *p = '\0';

    len = strlen(line);
    while (len > 0 && isspace((unsigned char)line[len - 1]))
        line[--len] = '\0';
    if (len == 0 || len >= NAMESIZE)
        return 0;

    strcpy(name, line);
    *bin = (int)b;
    return 1;
}

/* copies books.txt to books.txt.bak (used when the file had bad lines) */
static int backupDataFile(void)
{
    char buf[1024];
    size_t n;
    FILE *in = fopen(DATAFILE, "rb");
    FILE *out;
    if (in == NULL)
        return 0;
    out = fopen(DATAFILE ".bak", "wb");
    if (out == NULL)
    {
        fclose(in);
        return 0;
    }
    while ((n = fread(buf, 1, sizeof buf, in)) > 0)
        fwrite(buf, 1, n, out);
    fclose(in);
    fclose(out);
    return 1;
}

/* table output */
static void printHeader(void)
{
    printf("+--------+----------------------------------------------------+------------+\n");
    printf("| Bin#   | Name                                               |      Price |\n");
    printf("+--------+----------------------------------------------------+------------+\n");
}

static void printRow(const char *name, int bin, double price)
{
    printf("| %-6d | %-50s | %10.2f |\n", bin, name, price);
}

static void printFooter(void)
{
    printf("+--------+----------------------------------------------------+------------+\n");
}

/* ------------------------------------------------------------------ */
/*                           MAIN OPERATIONS                          */
/* ------------------------------------------------------------------ */

void addBook(int bins[], double prices[], char names[][NAMESIZE], int *size)
{
    int x;
    double y;
    char z[NAMESIZE];

    if ((*size) >= MAXSIZE)
    {
        printf("The bookstore is full (maximum %d books).\n", MAXSIZE);
        return;
    }

    printf("Enter bin number for book\n");
    if (readInt(&x) != 1 || x < 0)
    {
        printf("Invalid bin number!\n");
        return;
    }
    if (findBin(bins, *size, x) != -1)
    {
        printf("The bin number is already in the list!\n");
        return;
    }

    printf("Enter name of book\n");
    if (readName(z) != 1)
    {
        printf("Invalid book name!\n");
        return;
    }

    printf("Enter price of book\n");
    if (readDouble(&y) != 1 || y < 0)
    {
        printf("Invalid price!\n");
        return;
    }

    insertSorted(bins, prices, names, size, x, y, z);
    printf("Book with bin %d has been added.\n\n", x);
}

void removeBook(int bins[], double prices[], char names[][NAMESIZE], int *size)
{
    int test;
    if ((*size) == 0)
    {
        printf("There are no books to remove.\n");
        return;
    }

    printf("Enter bin number for book to remove.\n");
    if (readInt(&test) != 1)
    {
        printf("Invalid bin number!\n");
        return;
    }

    int position = findBin(bins, *size, test);
    if (position == -1)
    {
        printf("The book with bin %d does not exist!\n", test);
        return;
    }

    for (int j = position; j < (*size) - 1; j++)
    {
        bins[j] = bins[j + 1];
        prices[j] = prices[j + 1];
        strcpy(names[j], names[j + 1]);
    }
    (*size)--;
    printf("Book with bin %d has been removed.\n", test);
}

void searchForBook(int bins[], double prices[], char names[][NAMESIZE], int size)
{
    int test;
    if (size == 0)
    {
        printf("There are no books to search.\n");
        return;
    }

    printf("Enter bin number for book to search for\n");
    if (readInt(&test) != 1)
    {
        printf("Invalid bin number!\n");
        return;
    }

    int i = findBin(bins, size, test);
    if (i == -1)
        printf("The book with bin %d does not exist!\n", test);
    else
    {
        printHeader();
        printRow(names[i], bins[i], prices[i]);
        printFooter();
    }
}

void uploadDataFile(int bins[], double prices[], char names[][NAMESIZE], int *size)
{
    char line[256], name[NAMESIZE];
    int bin, skipped = 0;
    double price;

    printf("Loading data file...\n");
    FILE *input_file = fopen(DATAFILE, "r");
    *size = 0;

    if (input_file == NULL)
    {
        printf("No existing %s found. Starting with an empty bookstore.\n\n", DATAFILE);
        return;
    }

    while (fgets(line, sizeof line, input_file) != NULL)
    {
        /* ignore blank lines */
        char *s = line;
        while (isspace((unsigned char)*s))
            s++;
        if (*s == '\0')
            continue;

        if (*size >= MAXSIZE || !parseLine(line, name, &bin, &price) ||
            findBin(bins, *size, bin) != -1)
        {
            skipped++;
            continue;
        }
        insertSorted(bins, prices, names, size, bin, price, name);
    }
    if (ferror(input_file))
    {
        printf("Error while reading data. Saving disabled to protect the file.\n");
        safeToSave = 0;
    }
    fclose(input_file);

    printf("Loaded %d books.\n", *size);
    if (skipped > 0)
    {
        safeToSave = 0;
        printf("Warning: %d line(s) in %s were skipped (invalid, duplicate or over capacity).\n",
               skipped, DATAFILE);
        if (backupDataFile())
            printf("A copy of the original file was saved as %s.bak\n", DATAFILE);
        else
            printf("Warning: could not create a backup copy of %s!\n", DATAFILE);
    }
    printf("\n");
}

void updateDataFile(int bins[], double prices[], char names[][NAMESIZE], int size)
{
    printf("\nUpdating data file ...\n");
    if (!safeToSave)
    {
        printf("Save cancelled: the original data file could not be read completely.\n");
        return;
    }

    FILE *output_file = fopen(DATAFILE, "w");
    if (output_file == NULL)
    {
        perror("Could not save books.txt");
        return;
    }
    int ok = 1;
    for (int i = 0; i < size; i++)
    {
        if (fprintf(output_file, "%s  %d %.2f\n", names[i], bins[i], prices[i]) < 0)
        {
            ok = 0;
            break;
        }
    }
    if (fclose(output_file) != 0)
        ok = 0;
    if (!ok)
    {
        printf("Error: data may not have been saved completely.\n");
        return;
    }
    printf("Data File updated\n\n");
}

void printBooks(int bins[], double prices[], char names[][NAMESIZE], int size)
{
    if (size == 0)
        printf("There are no books to print.\n");
    else
    {
        printHeader();
        for (int i = 0; i < size; i++)
            printRow(names[i], bins[i], prices[i]);
        printFooter();
        printf("Total: %d book(s)\n", size);
    }
}

void displaymenu(int operation, int bins[], double prices[], char names[][NAMESIZE], int size)
{
    while (operation != 5)
    {
        printf("\nPlease Select an Operation (1-5):\n");
        printf("1- Add a Book:\n2- Remove a Book:\n3- Search for a Book:\n4- Print Book List\n5- Exit System:\n\n");

        int r = readInt(&operation);
        if (r == -1)            /* EOF (Ctrl+D / Ctrl+Z): save and exit */
            operation = 5;
        else if (r == 0)        /* not a number */
            operation = 0;

        printf("\n");
        switch (operation)
        {
        case 1:
            addBook(bins, prices, names, &size);
            break;
        case 2:
            removeBook(bins, prices, names, &size);
            break;
        case 3:
            searchForBook(bins, prices, names, size);
            break;
        case 4:
            printBooks(bins, prices, names, size);
            break;
        case 5:
            updateDataFile(bins, prices, names, size);
            printf("Thank you for using My Bookstore Management System. Goodbye.\n");
            break;
        default:
            printf("The option you selected is not in the list. Please try again.\n");
        }
    }
}