#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_SAMPLE_COUNT 10U
#define MAX_SAMPLE_COUNT 100U
#define WARNING_LOW_mC 18000
#define WARNING_HIGH_mC 30000
#define FAILURE_CODE_mC (-999000)

typedef enum {
    MODE_NORMAL,
    MODE_WARNING,
    MODE_FAILURE
} sensor_mode_t;

typedef enum {
    STATUS_OK,
    STATUS_WARNING,
    STATUS_FAILURE
} sensor_status_t;

static double to_celsius(int32_t temperature_mC)
{
    return (double)temperature_mC / 1000.0;
}

static int32_t base_reading(size_t index)
{
    static const int32_t offsets_mC[] = {
            0, 750, -500, 1250, -1000, 500, -250, 1000
    };

    const size_t length = sizeof offsets_mC / sizeof offsets_mC[0];

    return 24000 + offsets_mC[index % length];
}

static int32_t generate_reading(sensor_mode_t mode, size_t index)
{
    const int32_t base = base_reading(index);
    int32_t reading = base;

    switch (mode) {
        case MODE_NORMAL:
            reading = base;
            break;

        case MODE_WARNING:
            if (index % 4 == 3) {
                reading = 31500;
            }
            break;

        case MODE_FAILURE:
            if (index % 5 == 4) {
                reading = FAILURE_CODE_mC;
            }
            break;

        default:
            reading = base;
            break;
    }

    return reading;
}

static sensor_status_t classify_reading(int32_t temperature_mC)
{
    /*
     * Check the failure sentinel first.
     */
    if (temperature_mC == FAILURE_CODE_mC) {
        return STATUS_FAILURE;
    }

    if (temperature_mC >= WARNING_LOW_mC &&
        temperature_mC <= WARNING_HIGH_mC) {
        return STATUS_OK;
    }

    return STATUS_WARNING;
}

static const char *status_text(sensor_status_t status)
{
    switch (status) {
        case STATUS_OK:
            return "OK";

        case STATUS_WARNING:
            return "WARNING";

        case STATUS_FAILURE:
            return "FAILURE";
    }

    return "UNKNOWN";
}

/*
 * D1 — Parse the operating mode.
 *
 * Accepts exactly:
 * normal
 * warning
 * failure
 */
static int parse_mode(const char *text, sensor_mode_t *mode)
{
    if (text == NULL || mode == NULL) {
        return -1;
    }

    if (strcmp(text, "normal") == 0) {
        *mode = MODE_NORMAL;
        return 0;
    }

    if (strcmp(text, "warning") == 0) {
        *mode = MODE_WARNING;
        return 0;
    }

    if (strcmp(text, "failure") == 0) {
        *mode = MODE_FAILURE;
        return 0;
    }

    return -1;
}

/*
 * D2 — Parse the optional sample count.
 *
 * Valid range: 1 through 100.
 */
static int parse_count(const char *text, size_t *count)
{
    char *end = NULL;
    unsigned long value;

    if (text == NULL || count == NULL || text[0] == '\0') {
        return -1;
    }

    errno = 0;

    value = strtoul(text, &end, 10);

    if (errno != 0 ||
        end == text ||
        *end != '\0' ||
        value < 1UL ||
        value > MAX_SAMPLE_COUNT) {
        return -1;
    }

    *count = (size_t)value;

    return 0;
}

int main(int argc, char *argv[])
{
    /*
     * D3 — Default count is 10 when COUNT is omitted.
     */
    size_t count = DEFAULT_SAMPLE_COUNT;

    sensor_mode_t mode;

    size_t ok_count = 0;
    size_t warning_count = 0;
    size_t failure_count = 0;
    size_t valid_count = 0;

    int32_t minimum = INT32_MAX;
    int32_t maximum = INT32_MIN;

    int64_t total = 0;

    /*
     * Enforce:
     *
     * ./sensor_sim MODE [COUNT]
     *
     * argc = 2 -> MODE only
     * argc = 3 -> MODE and COUNT
     */
    if (argc < 2 || argc > 3) {
        fprintf(stderr, "Usage: %s MODE [COUNT]\n", argv[0]);
        fprintf(stderr, "MODE: normal | warning | failure\n");
        fprintf(stderr,
                "COUNT: integer from 1 through 100 (default: 10)\n");

        return EXIT_FAILURE;
    }

    /*
     * Parse MODE.
     */
    if (parse_mode(argv[1], &mode) != 0) {
        fprintf(stderr, "Usage: %s MODE [COUNT]\n", argv[0]);
        fprintf(stderr, "MODE: normal | warning | failure\n");
        fprintf(stderr,
                "COUNT: integer from 1 through 100 (default: 10)\n");

        return EXIT_FAILURE;
    }

    /*
     * Parse COUNT if it was provided.
     *
     * If argc == 2, count remains 10.
     */
    if (argc == 3 && parse_count(argv[2], &count) != 0) {
        fprintf(stderr, "Usage: %s MODE [COUNT]\n", argv[0]);
        fprintf(stderr, "MODE: normal | warning | failure\n");
        fprintf(stderr,
                "COUNT: integer from 1 through 100 (default: 10)\n");

        return EXIT_FAILURE;
    }

    /*
     * Generate and classify sensor readings.
     */
    for (size_t index = 0; index < count; index++) {

        int32_t temperature_mC = generate_reading(mode, index);

        sensor_status_t status = classify_reading(temperature_mC);

        if (status == STATUS_OK) {
            ok_count++;
        }
        else if (status == STATUS_WARNING) {
            warning_count++;
        }
        else if (status == STATUS_FAILURE) {
            failure_count++;
        }

        /*
         * Only valid readings are included in
         * minimum, maximum, and average calculations.
         */
        if (status == STATUS_OK ||
            status == STATUS_WARNING) {

            if (temperature_mC < minimum) {
                minimum = temperature_mC;
            }

            if (temperature_mC > maximum) {
                maximum = temperature_mC;
            }

            total += temperature_mC;
            valid_count++;

            printf(
                    "sample=%02zu temperature=%.3f C status=%s\n",
                    index + 1,
                    to_celsius(temperature_mC),
                    status_text(status)
            );
        }
        else {
            printf(
                    "sample=%02zu temperature=n/a status=%s\n",
                    index + 1,
                    status_text(status)
            );
        }
    }

    /*
     * Print summary.
     */
    printf(
            "summary samples=%zu valid=%zu ok=%zu warning=%zu failure=%zu\n",
            count,
            valid_count,
            ok_count,
            warning_count,
            failure_count
    );

    /*
     * Calculate statistics only when there are valid readings.
     */
    if (valid_count > 0) {

        double average =
                ((double)total / (double)valid_count) / 1000.0;

        printf(
                "temperature min=%.3f C max=%.3f C average=%.3f C\n",
                to_celsius(minimum),
                to_celsius(maximum),
                average
        );
    }

    return 0;
}