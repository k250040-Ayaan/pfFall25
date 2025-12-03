#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

#define DB_FILENAME "members.dat"

#define NAME_LEN 100
#define BATCH_LEN 32
#define TYPE_LEN 16
#define DATE_LEN 11
#define INTEREST_LEN 8

typedef struct
{
    int studentID;
    char fullName[NAME_LEN];
    char batch[BATCH_LEN];
    char membershipType[TYPE_LEN];
    char registrationDate[DATE_LEN];
    char dob[DATE_LEN];
    char interest[INTEREST_LEN];
} Student;

static Student *db = NULL;
static size_t db_size = 0;
static size_t db_capacity = 0;

static void die(const char *msg)
{
    fprintf(stderr, "Fatal: %s\n", msg);
    exit(EXIT_FAILURE);
}

static void *xmalloc(size_t n)
{
    void *p = malloc(n);
    if (!p)
        die("Out of memory");
    return p;
}

static void *xrealloc(void *p, size_t n)
{
    void *q = realloc(p, n);
    if (!q)
        die("Out of memory");
    return q;
}

static void trim_newline(char *s)
{
    if (!s)
        return;
    size_t L = strlen(s);
    if (L > 0 && s[L - 1] == '\n')
        s[L - 1] = '\0';
}

static void read_line(const char *prompt, char *buf, size_t bufsize)
{
    if (prompt)
        printf("%s", prompt);
    if (fgets(buf, (int)bufsize, stdin) == NULL)
    {
        buf[0] = '\0';
        clearerr(stdin);
        return;
    }
    trim_newline(buf);
}

static int read_int(const char *prompt)
{
    char buf[64];
    int val;
    while (1)
    {
        read_line(prompt, buf, sizeof(buf));
        if (buf[0] == '\0')
            continue;
        char *endptr;
        val = (int)strtol(buf, &endptr, 10);
        if (endptr == buf || *endptr != '\0')
        {
            printf("Please enter a valid integer.\n");
            continue;
        }
        return val;
    }
}

static int validate_date(const char *d)
{
    if (!d)
        return 0;
    if (strlen(d) != 10)
        return 0;
    if (!isdigit((unsigned char)d[0]) || !isdigit((unsigned char)d[1]) ||
        !isdigit((unsigned char)d[2]) || !isdigit((unsigned char)d[3]) ||
        d[4] != '-' ||
        !isdigit((unsigned char)d[5]) || !isdigit((unsigned char)d[6]) ||
        d[7] != '-' ||
        !isdigit((unsigned char)d[8]) || !isdigit((unsigned char)d[9]))
        return 0;
    int year = (d[0] - '0') * 1000 + (d[1] - '0') * 100 + (d[2] - '0') * 10 + (d[3] - '0');
    int month = (d[5] - '0') * 10 + (d[6] - '0');
    int day = (d[8] - '0') * 10 + (d[9] - '0');
    if (month < 1 || month > 12)
        return 0;
    if (day < 1 || day > 31)
        return 0;
    if ((month == 4 || month == 6 || month == 9 || month == 11) && day > 30)
        return 0;
    if (month == 2)
    {
        int leap = (year % 400 == 0) || (year % 4 == 0 && year % 100 != 0);
        if (day > 29)
            return 0;
        if (!leap && day > 28)
            return 0;
    }
    return 1;
}

static void canonicalize_str(char *s)
{
    if (!s)
        return;
    while (*s && isspace((unsigned char)*s))
        memmove(s, s + 1, strlen(s));
    size_t L = strlen(s);
    while (L > 0 && isspace((unsigned char)s[L - 1]))
    {
        s[L - 1] = '\0';
        L--;
    }
}

static void ensure_capacity(size_t min_capacity)
{
    if (db_capacity >= min_capacity)
        return;
    size_t newcap = db_capacity ? db_capacity * 2 : 8;
    while (newcap < min_capacity)
        newcap *= 2;
    db = (Student *)xrealloc(db, newcap * sizeof(Student));
    db_capacity = newcap;
}

static int find_index_by_id(int studentID)
{
    for (size_t i = 0; i < db_size; ++i)
    {
        if (db[i].studentID == studentID)
            return (int)i;
    }
    return -1;
}

int loadDatabase(const char *filename)
{
    FILE *f = fopen(filename, "rb");
    if (!f)
    {
        if (errno == ENOENT)
        {
            db_size = 0;
            return 0;
        }
        perror("Error opening database file for reading");
        return -1;
    }

    if (fseek(f, 0, SEEK_END) != 0)
    {
        perror("fseek failed");
        fclose(f);
        return -1;
    }
    long file_size = ftell(f);
    if (file_size < 0)
        file_size = 0;
    rewind(f);

    size_t record_count = (size_t)(file_size / sizeof(Student));
    ensure_capacity(record_count);
    size_t read_count = fread(db, sizeof(Student), record_count, f);
    if (read_count != record_count)
    {
        if (ferror(f))
        {
            perror("Error reading database file");
            fclose(f);
            return -1;
        }
    }
    db_size = read_count;
    fclose(f);
    return (int)db_size;
}

int saveDatabase(const char *filename)
{
    FILE *f = fopen(filename, "wb");
    if (!f)
    {
        perror("Error opening database file for writing");
        return -1;
    }
    if (db_size > 0)
    {
        size_t wrote = fwrite(db, sizeof(Student), db_size, f);
        if (wrote != db_size)
        {
            perror("Error writing to database file");
            fclose(f);
            return -1;
        }
    }
    if (fclose(f) != 0)
    {
        perror("Error closing database file after writing");
        return -1;
    }
    return 0;
}

int addStudent(Student s, const char *filename)
{
    if (find_index_by_id(s.studentID) != -1)
    {
        fprintf(stderr, "Error: Student ID %d already exists.\n", s.studentID);
        return -1;
    }

    FILE *f = fopen(filename, "ab");
    if (!f)
    {
        perror("Error opening database file for appending");
        return -1;
    }
    size_t wrote = fwrite(&s, sizeof(Student), 1, f);
    if (wrote != 1)
    {
        perror("Error appending new student record to file");
        fclose(f);
        return -1;
    }
    if (fclose(f) != 0)
    {
        perror("Error closing database file after append");
        return -1;
    }

    ensure_capacity(db_size + 1);
    db[db_size] = s;
    db_size++;
    return 0;
}

int updateStudent(int studentID)
{
    int idx = find_index_by_id(studentID);
    if (idx == -1)
    {
        printf("Student with ID %d not found.\n", studentID);
        return -1;
    }
    printf("Found student: %d | %s | %s | %s | Reg: %s | DOB: %s | Interest: %s\n",
           db[idx].studentID, db[idx].fullName, db[idx].batch, db[idx].membershipType,
           db[idx].registrationDate, db[idx].dob, db[idx].interest);

    printf("What would you like to update?\n");
    printf("1) Batch\n2) Membership Type\n3) Interest\n4) Full name\n5) Registration date\n6) Date of birth\n0) Cancel\n");
    int choice = read_int("Choice: ");
    char tmp[128];

    switch (choice)
    {
    case 1:
        read_line("Enter new Batch (CS, SE, Cyber Security, AI): ", tmp, sizeof(tmp));
        canonicalize_str(tmp);
        if (strlen(tmp) == 0)
        {
            printf("No change.\n");
            return 0;
        }
        strncpy(db[idx].batch, tmp, BATCH_LEN - 1);
        db[idx].batch[BATCH_LEN - 1] = '\0';
        break;
    case 2:
        read_line("Enter new Membership Type (IEEE or ACM): ", tmp, sizeof(tmp));
        canonicalize_str(tmp);
        if (strlen(tmp) == 0)
        {
            printf("No change.\n");
            return 0;
        }
        strncpy(db[idx].membershipType, tmp, TYPE_LEN - 1);
        db[idx].membershipType[TYPE_LEN - 1] = '\0';
        break;
    case 3:
        read_line("Enter Interest (IEEE / ACM / Both): ", tmp, sizeof(tmp));
        canonicalize_str(tmp);
        if (strlen(tmp) == 0)
        {
            printf("No change.\n");
            return 0;
        }
        strncpy(db[idx].interest, tmp, INTEREST_LEN - 1);
        db[idx].interest[INTEREST_LEN - 1] = '\0';
        break;
    case 4:
        read_line("Enter new Full Name: ", tmp, sizeof(tmp));
        if (strlen(tmp) == 0)
        {
            printf("No change.\n");
            return 0;
        }
        strncpy(db[idx].fullName, tmp, NAME_LEN - 1);
        db[idx].fullName[NAME_LEN - 1] = '\0';
        break;
    case 5:
        while (1)
        {
            read_line("Enter new Registration Date (YYYY-MM-DD): ", tmp, sizeof(tmp));
            if (strlen(tmp) == 0)
            {
                printf("No change.\n");
                return 0;
            }
            if (validate_date(tmp))
            {
                strncpy(db[idx].registrationDate, tmp, DATE_LEN - 1);
                db[idx].registrationDate[DATE_LEN - 1] = '\0';
                break;
            }
            else
                printf("Invalid date format. Try again.\n");
        }
        break;
    case 6:
        while (1)
        {
            read_line("Enter new Date of Birth (YYYY-MM-DD): ", tmp, sizeof(tmp));
            if (strlen(tmp) == 0)
            {
                printf("No change.\n");
                return 0;
            }
            if (validate_date(tmp))
            {
                strncpy(db[idx].dob, tmp, DATE_LEN - 1);
                db[idx].dob[DATE_LEN - 1] = '\0';
                break;
            }
            else
                printf("Invalid date format. Try again.\n");
        }
        break;
    case 0:
        printf("Update canceled.\n");
        return 0;
    default:
        printf("Invalid choice.\n");
        return -1;
    }

    if (saveDatabase(DB_FILENAME) != 0)
    {
        printf("Warning: Could not save update to file.\n");
        return -1;
    }
    printf("Student updated and changes saved.\n");
    return 0;
}

int deleteStudent(int studentID)
{
    int idx = find_index_by_id(studentID);
    if (idx == -1)
    {
        printf("Student with ID %d not found.\n", studentID);
        return -1;
    }
    for (size_t i = idx; i + 1 < db_size; ++i)
        db[i] = db[i + 1];
    db_size--;

    if (saveDatabase(DB_FILENAME) != 0)
    {
        printf("Warning: Could not save deletion to file.\n");
        return -1;
    }
    printf("Student ID %d deleted and changes saved.\n", studentID);
    return 0;
}

void show_student(const Student *s)
{
    if (!s)
        return;
    printf("ID:%d | Name:%s | Batch:%s | Type:%s | Reg:%s | DOB:%s | Interest:%s\n",
           s->studentID, s->fullName, s->batch, s->membershipType,
           s->registrationDate, s->dob, s->interest);
}

void display_all_students(void)
{
    if (db_size == 0)
    {
        printf("No students registered yet.\n");
        return;
    }
    printf("Total students: %zu\n", db_size);
    for (size_t i = 0; i < db_size; ++i)
    {
        show_student(&db[i]);
    }
}

void batch_report(void)
{
    char batchQuery[BATCH_LEN];
    char typeQuery[TYPE_LEN];
    read_line("Enter Batch to filter (CS, SE, Cyber Security, AI): ", batchQuery, sizeof(batchQuery));
    canonicalize_str(batchQuery);
    read_line("Enter Membership/Interest to filter (IEEE, ACM, Both) - leave blank for any: ", typeQuery, sizeof(typeQuery));
    canonicalize_str(typeQuery);

    size_t count = 0;
    for (size_t i = 0; i < db_size; ++i)
    {
        int matchBatch = (strlen(batchQuery) == 0) ? 1 : (strcasecmp(db[i].batch, batchQuery) == 0);
        int matchType = 1;
        if (strlen(typeQuery) != 0)
        {
            if (strcasecmp(typeQuery, db[i].membershipType) == 0)
                matchType = 1;
            else if (strcasecmp(typeQuery, db[i].interest) == 0)
                matchType = 1;
            else if (strcasecmp(db[i].interest, "Both") == 0)
                matchType = 1;
            else
                matchType = 0;
        }
        if (matchBatch && matchType)
        {
            show_student(&db[i]);
            count++;
        }
    }
    if (count == 0)
        printf("No matching records found.\n");
    else
        printf("Found %zu matching records.\n", count);
}

static int get_unique_student_id(void)
{
    while (1)
    {
        int id = read_int("Enter Student ID (integer): ");
        if (find_index_by_id(id) != -1)
        {
            printf("Student ID %d already exists. Enter a different ID.\n", id);
            continue;
        }
        return id;
    }
}

void register_new_student(void)
{
    Student s;
    char buf[128];

    s.studentID = get_unique_student_id();

    read_line("Enter Full Name: ", buf, sizeof(buf));
    while (strlen(buf) == 0)
    {
        printf("Name cannot be empty.\n");
        read_line("Enter Full Name: ", buf, sizeof(buf));
    }
    strncpy(s.fullName, buf, NAME_LEN - 1);
    s.fullName[NAME_LEN - 1] = '\0';

    read_line("Enter Batch (CS, SE, Cyber Security, AI): ", buf, sizeof(buf));
    canonicalize_str(buf);
    strncpy(s.batch, buf, BATCH_LEN - 1);
    s.batch[BATCH_LEN - 1] = '\0';

    read_line("Enter Membership Type (IEEE or ACM): ", buf, sizeof(buf));
    canonicalize_str(buf);
    strncpy(s.membershipType, buf, TYPE_LEN - 1);
    s.membershipType[TYPE_LEN - 1] = '\0';

    while (1)
    {
        read_line("Enter Registration Date (YYYY-MM-DD): ", buf, sizeof(buf));
        if (validate_date(buf))
        {
            strncpy(s.registrationDate, buf, DATE_LEN - 1);
            s.registrationDate[DATE_LEN - 1] = '\0';
            break;
        }
        printf("Invalid date format. Try again.\n");
    }

    while (1)
    {
        read_line("Enter Date of Birth (YYYY-MM-DD): ", buf, sizeof(buf));
        if (validate_date(buf))
        {
            strncpy(s.dob, buf, DATE_LEN - 1);
            s.dob[DATE_LEN - 1] = '\0';
            break;
        }
        printf("Invalid date format. Try again.\n");
    }

    read_line("Enter Interest (IEEE / ACM / Both): ", buf, sizeof(buf));
    canonicalize_str(buf);
    strncpy(s.interest, buf, INTEREST_LEN - 1);
    s.interest[INTEREST_LEN - 1] = '\0';

    if (addStudent(s, DB_FILENAME) == 0)
    {
        printf("Student registered successfully and persisted to %s.\n", DB_FILENAME);
    }
    else
    {
        printf("Failed to add student.\n");
    }
}

void stress_test(void)
{
    int n = read_int("How many students to add (suggest 20-30): ");
    if (n <= 0)
    {
        printf("Canceled.\n");
        return;
    }

    srand((unsigned int)time(NULL));

    const char *fnames[] = {"Ali Khan", "Sara Ahmed", "Bilal Qureshi", "Ayesha Malik", "Hassan Raza", "Zainab Ali", "Omar Farooq", "Mariam Noor", "Usman Tariq", "Hina Iqbal"};
    const char *batches[] = {"CS", "SE", "Cyber Security", "AI"};
    const char *types[] = {"IEEE", "ACM"};
    const char *interests[] = {"IEEE", "ACM", "Both"};

    int added = 0;
    for (int i = 0; i < n; ++i)
    {
        Student s;
        int tries = 0;
        do
        {
            s.studentID = 1000 + rand() % 9000;
            tries++;
            if (tries > 50)
                s.studentID = 100000 + i;
        } while (find_index_by_id(s.studentID) != -1);

        strncpy(s.fullName, fnames[rand() % (sizeof(fnames) / sizeof(fnames[0]))], NAME_LEN - 1);
        s.fullName[NAME_LEN - 1] = '\0';
        strncpy(s.batch, batches[rand() % (sizeof(batches) / sizeof(batches[0]))], BATCH_LEN - 1);
        strncpy(s.membershipType, types[rand() % (sizeof(types) / sizeof(types[0]))], TYPE_LEN - 1);

        int ry = 2020 + rand() % 6;
        int rm = 1 + rand() % 12;
        int rd = 1 + rand() % 28;
        snprintf(s.registrationDate, DATE_LEN, "%04d-%02d-%02d", ry, rm, rd);

        int dy = 1996 + rand() % 11;
        int dm = 1 + rand() % 12;
        int dd = 1 + rand() % 28;
        snprintf(s.dob, DATE_LEN, "%04d-%02d-%02d", dy, dm, dd);

        strncpy(s.interest, interests[rand() % (sizeof(interests) / sizeof(interests[0]))], INTEREST_LEN - 1);

        if (addStudent(s, DB_FILENAME) == 0)
            added++;
    }
    printf("Added %d random students.\n", added);

    int delCount = (n >= 5) ? (rand() % 6 + 5) : (rand() % (n + 1));
    if (delCount > (int)db_size)
        delCount = (int)db_size / 2;
    for (int i = 0; i < delCount; ++i)
    {
        if (db_size == 0)
            break;
        int pickIdx = rand() % (int)db_size;
        int sid = db[pickIdx].studentID;
        deleteStudent(sid);
    }
    printf("Deleted %d random students.\n", delCount);

    int updateCount = (n >= 5) ? (rand() % 6 + 5) : (rand() % (n + 1));
    if (updateCount > (int)db_size)
        updateCount = (int)db_size;
    for (int i = 0; i < updateCount; ++i)
    {
        if (db_size == 0)
            break;
        int pickIdx = rand() % (int)db_size;
        strncpy(db[pickIdx].batch, batches[rand() % (sizeof(batches) / sizeof(batches[0]))], BATCH_LEN - 1);
        strncpy(db[pickIdx].membershipType, types[rand() % (sizeof(types) / sizeof(types[0]))], TYPE_LEN - 1);
    }
    if (saveDatabase(DB_FILENAME) == 0)
    {
        printf("Updated %d random records and saved.\n", updateCount);
    }
    else
    {
        printf("Warning: Could not save updates after stress test.\n");
    }

    printf("Stress test completed. Current DB size: %zu\n", db_size);
}

void print_menu(void)
{
    printf("\n====== Membership Manager ======\n");
    printf("1) Register a new student\n");
    printf("2) Update student (batch/membership/etc.)\n");
    printf("3) Delete a student\n");
    printf("4) View all registrations\n");
    printf("5) Generate batch-wise report\n");
    printf("6) Stress-test file handling (auto add/update/delete)\n");
    printf("0) Exit (saves DB)\n");
}

int main(void)
{
    if (loadDatabase(DB_FILENAME) < 0)
    {
        fprintf(stderr, "Error loading database. Starting with empty DB.\n");
        db_size = 0;
    }
    else
    {
        printf("Loaded %zu students from %s.\n", db_size, DB_FILENAME);
    }

    while (1)
    {
        print_menu();
        int choice = read_int("Choose an option: ");
        if (choice == 0)
        {
            if (saveDatabase(DB_FILENAME) != 0)
            {
                printf("Warning: problem saving database on exit.\n");
            }
            else
            {
                printf("Database saved to %s. Exiting.\n", DB_FILENAME);
            }
            break;
        }
        switch (choice)
        {
        case 1:
            register_new_student();
            break;
        case 2:
        {
            int id = read_int("Enter Student ID to update: ");
            updateStudent(id);
            break;
        }
        case 3:
        {
            int id = read_int("Enter Student ID to delete: ");
            deleteStudent(id);
            break;
        }
        case 4:
            display_all_students();
            break;
        case 5:
            batch_report();
            break;
        case 6:
            stress_test();
            break;
        default:
            printf("Invalid choice. Try again.\n");
        }
    }

    free(db);
    db = NULL;
    db_size = db_capacity = 0;
    return 0;
}
