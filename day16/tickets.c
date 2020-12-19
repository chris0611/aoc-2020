#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

#define BUFSIZE 256
#define STRINGLEN 20
#define MAXFIELDS 20

typedef struct {
    char name[STRINGLEN];
    int8_t pos;
    uint16_t min1;
    uint16_t max1;
    uint16_t min2;
    uint16_t max2;
} field;

uint8_t num_found(int8_t *arr, uint8_t len)
{
	uint8_t sum = 0;
	for (uint8_t i = 0; i < len; ++i) {
		if (arr[i] != -1) ++sum;
	}
	return sum;
}

uint8_t row_taken(int8_t *arr, uint8_t len, uint8_t row)
{
	for (uint8_t i = 0; i < len; ++i) {
		if (arr[i] == row) return 1;
	}
	return 0;
}

int main(void)
{
    field fields[MAXFIELDS];

    // Initializing
    for (uint8_t i = 0; i < MAXFIELDS; ++i) {
        fields[i].name[0] = '\0';
        fields[i].pos = -1;
        fields[i].min1 = 0;
        fields[i].max1 = 0;
        fields[i].min2 = 0;
        fields[i].max2 = 0;
    }
    uint16_t a = 0, b = 0, c = 0, d = 0;

    uint8_t f_count = 0;
    char buffer[BUFSIZE] = {0};
    char f_name[STRINGLEN] = {0};

    // Reading input fields
    while (fgets(buffer, BUFSIZE, stdin) != NULL ) {
        if ((sscanf(buffer, "%s %*s %hd-%hd or %hd-%hd\n", f_name, &a, &b, &c, &d) == 5) || 
            (sscanf(buffer, "%s %hd-%hd or %hd-%hd\n", f_name, &a, &b, &c, &d) == 5)) {

            strncpy(fields[f_count].name, f_name, STRINGLEN);
            fields[f_count].min1 = a;
            fields[f_count].max1 = b;
            fields[f_count].min2 = c;
            fields[f_count].max2 = d;
            ++f_count;
        } else {
            break;
        }
    }

    uint16_t my_ticket[MAXFIELDS] = {0};
    char num[STRINGLEN] = {0};

    uint8_t ticket_len = 0;
    uint8_t string_len = 0;
    
    if (scanf("\nyour %*s\n") != 0) exit(EXIT_FAILURE);
    if (fgets(buffer, BUFSIZE, stdin) == NULL) exit(EXIT_FAILURE);

    printf("My ticket:\n");
    for (uint8_t i = 0; i < strlen(buffer); ++i) {
        if (isdigit(buffer[i])) {
            printf("%c", buffer[i]);
            num[string_len] = buffer[i];
            ++string_len;
        } else {
            printf(" ");
            num[string_len] = '\0';
            my_ticket[ticket_len] = atoi(num);
            memset(num, 0, (size_t)string_len);
            string_len = 0;
            ++ticket_len;
            if (buffer[i] == '\n') break; 
        }
    }
    printf("\n");
    if (scanf("\nnearby %*s\n") != 0) exit(EXIT_FAILURE);

    uint16_t prev_err_count = 0;
    uint16_t errors = 0;
    uint8_t valid = 0;
    uint16_t amt_valid = 0;
    uint32_t current_f = 0;
    uint16_t num_buf[MAXFIELDS] = {0};
    uint32_t *matched_fields[MAXFIELDS] = {0};
    for (uint8_t i = 0; i < f_count; ++i) {
        matched_fields[i] = malloc(sizeof(uint32_t) * f_count);
    }

    while (fscanf(stdin, "%hd,", &a) == 1) {
        //printf("%4d = %3d\n", current_f, a);
        for (uint8_t i = 0; i < f_count; ++i) {
            if ((a >= fields[i].min1 && a <= fields[i].max1) || (a >= fields[i].min2 && a <= fields[i].max2)) {
                num_buf[current_f % f_count] = a;
                valid = 1;
            }
        }

        if (!valid) errors += a; 
        valid = 0;

        if ((current_f % f_count) == (f_count - 1)) { // end of a ticket
            if (prev_err_count == errors) {
                ++amt_valid;
                for (uint8_t i = 0; i < f_count; ++i) {
                    uint16_t num = num_buf[i];
                    num_buf[i] = 0;
                    for (uint8_t j = 0; j < f_count; ++j) {
                        if ((num >= fields[j].min1 && num <= fields[j].max1) || (num >= fields[j].min2 && num <= fields[j].max2)) {
                            matched_fields[i][j] += 1;
                        }
                    }
                }
            }
            prev_err_count = errors;
        }

        ++current_f;
    }
    printf("Fields read: %d\n", current_f);
    printf("Tickets read: %d\n", current_f/f_count);
    printf("(Part 1) Ticket scanning error rate: %hd\n", errors);
    
    int8_t found_cf[MAXFIELDS] = {0}; // Each field will be assigned the corresponding column as they are detected (-1 means not found yet)
    memset(found_cf, -1, sizeof(int8_t) * MAXFIELDS);


	while (num_found(found_cf, f_count) != f_count) { // iterating until every field is assigned a ticket
        uint32_t num_eq_valid = 0;
		int8_t current_row = -1;
        int8_t current_col = -1;
		for (uint8_t i = 0; i < f_count; ++i) { // iterating through rows (ticket fields)
			uint16_t sum = 0;
			uint8_t amt_max = 0;
			uint8_t temp_col = 0;
			if (row_taken(found_cf, f_count, i)) continue;
			//printf("Row: %d\n", i);
			for (uint8_t j = 0; j < f_count; ++j) { // iterating through columns (fields)
				if (found_cf[j] == -1) {
					sum += matched_fields[i][j];
					if (matched_fields[i][j] == amt_valid) {
						++amt_max;
						temp_col = j;
					} else if (matched_fields[i][j] == amt_valid-1) {
						temp_col = j;
					}
				}
			}
			//printf("Amount equal to max: %d\n", amt_max);
			if (num_eq_valid == 0) {
				num_eq_valid = amt_max;
				current_row = i;
				current_col = temp_col;
			} else if (amt_max != 0 && amt_max < num_eq_valid) {
				num_eq_valid = amt_max;
				current_row = i;
				current_col = temp_col;
			} else if (amt_max == 0) {
				// TODO: figure out how to handle the rows where not every ticket has matched a field
				//current_row = i;
				//current_col = temp_col;
				//printf("hello - row = %d col = %d\n", i, temp_col);
			}
			//printf("Temp col: %d\n", temp_col);
			//printf("Current row: %d\n", current_row);
			//printf("Current col: %d\n", current_col);
			//printf("\n");
		}
		printf("FINAL ROW: %d\n", current_row);
		printf("FINAL COL: %d\n", current_col);
		//break;
		found_cf[current_col] = current_row;
    }

	printf("\033[1;30m    |\033[0m");
	for (uint8_t k = 0; k < f_count; ++k) {
		printf("\033[1;30m F%-2d|\033[0m", k);
	}
	printf("\n");
    for (uint8_t i = 0; i < f_count; ++i) {
        uint8_t j;
        for (j = 0; j <= f_count; ++j) {
            printf("\033[1;30m----+\033[0m");
        }
        printf("\n");
        printf("\033[7;30;1;30mT%-2d\033[0m \033[1;30m|\033[0m ",i);
        for (j = 0; j < f_count; ++j) {
            if (matched_fields[i][j] == 192) {
                printf("\033[0;30m\033[48;5;67m%2X\033[0m \033[1;30m|\033[0m ", matched_fields[i][j]);
            } else {
                printf("\033[1;30m%2X |\033[0m ", matched_fields[i][j]);
            }
        }
        printf("\n");
    }
    printf("Amount valid: %X (%d)\n", amt_valid, amt_valid);
    return 0;
}